#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

typedef enum { FLOAT, PAIR } ObjectType;

typedef struct Object_S {
  union {
    float float_number;
    struct Object_S *pair[2];
  } value;
  ObjectType object_type;
} Object;

// float_number: 4
// pair: 2*8
// object_type: 4
// 4 + 2*8 + 4 = 24

#define STACK_LENGTH 5

typedef struct Stack {
  Object *entries[STACK_LENGTH];
  // The next index available to fill.
  size_t fill_pointer;
} Stack;

// void junk() {
//   Stack stack = {0}; // Initialize all fields to 0.
//   Object *o = malloc(sizeof(Object));
//   o->object_type = FLOAT;
//   o->value.float_number = 5.7;
//   stack.entries[stack.fill_pointer] = o;
// }

void printObject(Object *o) {
  printf("float_number: %f\n", o->value.float_number);
}

Stack g_s;

// Print every element of the stack, including portions of the stack that have
// not yet been filled.
void printStack(Stack *s) {
  printf("fill_pointer: %zu\n", s->fill_pointer);

  for (int i = 0; i < STACK_LENGTH; i++) {
    printf("index: %i\n", i);
    Object *o = s->entries[i];
    if (o == NULL) {
      printf("Object is NULL!\n");
    } else {
      printObject(s->entries[i]);
    }
  }
};

void stackInit(Stack *s) { s->fill_pointer = 0; }

void stackPush(ObjectType object_type, float value) {
   // Allocate the object.
  Object *o = malloc(sizeof(Object));
  // Initialize the object.
  o->object_type = object_type;
  o->value.float_number = 6.0;
  // Push the object on the stack.
  g_s.entries[g_s.fill_pointer] = o;
  g_s.fill_pointer += 1;
}
void test() {
  stackInit(&g_s);
  {
    Object *o = allocateObject();
    setFloat(o, 6.0);
    stackPush(&g_s, o);

    Object *o = allocateFloat(6.0);
    stackPush(&g_s, o);

    stackPushFloat(&g_s, 6.0);
  }
  printStack(&g_s);

//  stack_free(&g_s);
  drop(&g_s);
  shared_ptr<Object> o = allocateObject();

  def f():
      x = [0]
      x[0] = x
  f()

  
}

int main() { test(); }