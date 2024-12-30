#include <signal.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define STACK_MAX 256
#define INIT_OBJ_NUM_MAX 8
#define HEAP_MAX 65536
#define DEFAULT_ALIGN 8

typedef enum { OBJ_INT, OBJ_PAIR, OBJ_FRWD } ObjectType;

typedef struct sObject {
  ObjectType type;
  union {
    /* OBJ_INT */
    int value;

    /* OBJ_PAIR */
    struct {
      struct sObject *head;
      struct sObject *tail;
    };
    /* OBJ_FRWD */
    struct sObject *forward;
  };
} Object;

/* used for the two regions of the collector.*/
typedef enum { RGN_BOBA, RGN_KIKI } Region;

typedef struct {
  Region region;
  size_t bump_offset;
  unsigned char region_boba[HEAP_MAX];
  unsigned char region_kiki[HEAP_MAX];
} Heap;

typedef struct {
  Object *stack[STACK_MAX];
  int stackSize;
  // (make-array STACK_MAX :fill-pointer 0)

  /* Linked list of work objects. */
  Object *firstObject;
  Object *lastObject;

  /* The total number of currently allocated objects. */
  int numObjects;

  /* The number of objects required to trigger a GC. */
  int maxObjects;

  /* Pointer to a Heap object*/
  Heap *heap;
} VM;


#define inRange(low, high, value) \
   ((low) <= (value) && (value) < (high))

/* Accepts a pointer and checks if that pointer is within the (unused region of?) the heap. */
bool inFromHeap(Heap *heap, void *ptr) {
  if (heap->region == RGN_KIKI) {
    return inRange((void *)heap->region_boba,
                   (void *)(heap->region_boba + HEAP_MAX),
                   ptr);
  } else {
    return inRange((void *)heap->region_kiki,
                   (void *)(heap->region_kiki + HEAP_MAX),
                   ptr);
  }
}

static void *heapAlloc(Heap *vm, size_t size);

void assertPrint(bool condition, const char *message) {
  if (!condition) {
    printf("%s\n", message);
    raise(SIGTRAP);
    exit(1);
  }
}

// Allocates and creates new Heap
Heap *newHeap() {
  Heap *heap = malloc(sizeof(Heap));
  memset(heap->region_boba, 0, sizeof heap->region_boba);
  memset(heap->region_kiki, 0, sizeof heap->region_kiki);
  heap->region = RGN_BOBA;
  heap->bump_offset = 0;
  return heap;
}

VM *newVM() {
  VM *vm = malloc(sizeof(VM));
  vm->stackSize = 0;
  vm->firstObject = NULL;
  vm->lastObject = NULL;
  vm->numObjects = 0;
  vm->maxObjects = INIT_OBJ_NUM_MAX;
  vm->heap = newHeap();
  return vm;
}

void push(VM *vm, Object *value) {
  assertPrint(vm->stackSize < STACK_MAX, "Stack overflow!");
  vm->stack[vm->stackSize++] = value;
}

Object *pop(VM *vm) {
  assertPrint(vm->stackSize > 0, "Stack underflow!");
  return vm->stack[--vm->stackSize];
}

/// @brief Performs the copying/forwarding and if not forwarded, adds the item
/// to worklist.
Object *forward(VM *vm, Object *object) {
  assertPrint(inFromHeap(vm->heap, object), "Object must be in from-heap.");

  if (object->type == OBJ_FRWD)
    return object->forward;

  Object *copied = heapAlloc(vm->heap, sizeof(Object));
  vm->numObjects++;
  memcpy(copied, object, sizeof(Object));
  // thread copied object into worklist

  // set forwarding pointer
  object->type = OBJ_FRWD;
  object->forward = copied;
  vm->lastObject = copied + 1;
  return copied;
}

void processRoots(VM *vm) {
  for (int i = 0; i < vm->stackSize; i++) {
    vm->stack[i] = forward(vm, vm->stack[i]);
  }
}

void processWorklist(VM *vm) {
  while (vm->firstObject != vm->lastObject) {
    // forward sub-pointers of Pair object
    if (vm->firstObject->type == OBJ_PAIR) {
      vm->firstObject->head = forward(vm, vm->firstObject->head);

      vm->firstObject->tail = forward(vm, vm->firstObject->tail);
    }

    vm->firstObject++;
  }
}

void swapHeap(VM *vm) {
  if (vm->heap->region == RGN_BOBA) {
    vm->heap->region = RGN_KIKI;
    vm->firstObject = vm->lastObject = (Object *)vm->heap->region_kiki;
  } else {
    vm->heap->region = RGN_BOBA;
    vm->firstObject = vm->lastObject = (Object *)vm->heap->region_boba;
  }
  vm->heap->bump_offset = 0;
}

void gc(VM *vm) {
  int numObjects = vm->numObjects;
  vm->numObjects = 0;

  swapHeap(vm);
  /* TODO: Figure out what the next two do */
  processRoots(vm);
  processWorklist(vm);
  vm->lastObject = NULL;
  vm->firstObject = NULL;

  vm->maxObjects = vm->numObjects == 0 ? INIT_OBJ_NUM_MAX : vm->numObjects * 2;

  printf("Collected %d objects, %d remaining.\n", numObjects - vm->numObjects,
         vm->numObjects);
}

void *align_up(void *ptr) {
  uintptr_t value = (uintptr_t)ptr;
  value = (value + (DEFAULT_ALIGN - 1)) & -DEFAULT_ALIGN;
  assertPrint(value % DEFAULT_ALIGN == 0,
              "Pointer must be aligned to DEFAULT_ALIGN");
  return (void *)value;
}

void *heapAlloc(Heap *heap, size_t size) {
  assertPrint(heap->bump_offset + size < HEAP_MAX,
              "Attempted to allocate more items that can be in heap");
  void *pointer = NULL;

  if (heap->region == RGN_BOBA) {
    pointer = heap->region_boba + heap->bump_offset;
  } else {
    pointer = heap->region_kiki + heap->bump_offset;
  }
  heap->bump_offset += size;
  return pointer;
}

Object *newObject(VM *vm, ObjectType type) {
  if (vm->numObjects == vm->maxObjects)
    gc(vm);

  Object *object = heapAlloc(vm->heap, sizeof(Object));
  object->type = type;
  vm->numObjects++;

  return object;
}

void pushInt(VM *vm, int intValue) {
  Object *object = newObject(vm, OBJ_INT);
  object->value = intValue;

  push(vm, object);
}

Object *pushPair(VM *vm) {
  Object *object = newObject(vm, OBJ_PAIR);
  object->tail = pop(vm);
  object->head = pop(vm);

  push(vm, object);
  return object;
}

void objectPrint(Object *object) {
  switch (object->type) {
  case OBJ_INT:
    printf("%d", object->value);
    break;

  case OBJ_PAIR:
    printf("(");
    objectPrint(object->head);
    printf(", ");
    objectPrint(object->tail);
    printf(")");
    break;

  default:
    assertPrint(0, "You shouldn't be seeing this!\n");
    break;
  }
}

void freeVM(VM *vm) {
  vm->stackSize = 0;
  gc(vm);
  free(vm->heap);
  free(vm);
}

void test1() {
  printf("Test 1: Objects on stack are preserved.\n");
  VM *vm = newVM();
  pushInt(vm, 1);
  pushInt(vm, 2);

  gc(vm);
  assertPrint(vm->numObjects == 2, "Should have preserved objects.");
  freeVM(vm);
}

void test2() {
  printf("Test 2: Unreached objects are collected.\n");
  VM *vm = newVM();
  pushInt(vm, 1);
  pushInt(vm, 2);
  pop(vm);
  pop(vm);

  gc(vm);
  assertPrint(vm->numObjects == 0, "Should have collected objects.");
  freeVM(vm);
}

void test3() {
  printf("Test 3: Reach nested objects.\n");
  VM *vm = newVM();
  pushInt(vm, 1);
  pushInt(vm, 2);
  pushPair(vm);
  pushInt(vm, 3);
  pushInt(vm, 4);
  pushPair(vm);
  pushPair(vm);

  gc(vm);
  assertPrint(vm->numObjects == 7, "Should have reached objects.");
  freeVM(vm);
}

void test4() {
  printf("Test 4: Handle cycles.\n");
  VM *vm = newVM();
  pushInt(vm, 1);
  pushInt(vm, 2);
  Object *a = pushPair(vm);
  pushInt(vm, 3);
  pushInt(vm, 4);
  Object *b = pushPair(vm);

  /* Set up a cycle, and also make 2 and 4 unreachable and collectible. */
  a->tail = b;
  b->tail = a;

  gc(vm);
  assertPrint(vm->numObjects == 4, "Should have collected objects.");
  freeVM(vm);
}

void perfTest() {
  printf("Performance Test.\n");
  VM *vm = newVM();

  for (int i = 0; i < 1000; i++) {
    for (int j = 0; j < 20; j++) {
      pushInt(vm, i);
    }

    for (int k = 0; k < 20; k++) {
      pop(vm);
    }
  }
  freeVM(vm);
}

int main(int argc, const char *argv[]) {
  (void)argc;
  (void)argv;

  test1();
  test2();
  test3();
  test4();
  perfTest();

  return 0;
}
