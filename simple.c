#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

// Like puts but without the newline added.
void put(char *s) { fputs(s, stdout); }

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
  putchar('\n');
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
  putchar('\n');
};

void stackInit(Stack *s) { s->fill_pointer = 0; }

Object *allocateObject() {
  // Allocate the object.
  return malloc(sizeof(Object));
}

void setFloat(Object *o, float f) {
  // Initialize the object.
  o->object_type = FLOAT;
  o->value.float_number = f;
}

// What happens when fill_pointer is 0?
Object *stackPop(Stack *s) {
  if (s->fill_pointer == 0) {
    return NULL;
  } else {
    --s->fill_pointer;
    Object *o = s->entries[s->fill_pointer];
    s->entries[s->fill_pointer] = NULL;
    return o;
  }
}

bool stackPush(Stack *s, Object *o) {
  // Push the object on the stack.
  if (s->fill_pointer >= STACK_LENGTH) {
    return false;
  } else {
    s->entries[s->fill_pointer++] = o;
    return true;
  }
}

void test_helper(float f) {
  Object *o = allocateObject();
  setFloat(o, f);
  if (stackPush(&g_s, o)) {
    puts("Success");
  } else {
    puts("There was an error");
  }
}

typedef enum { Opcode_NOOP, Opcode_EXIT, Opcode_ADD } Opcode;

typedef uint8_t u8;

void single_step(bool *running, u8 *byte_code, u8 *instruction_counter) {
    u8 opcode = byte_code[*instruction_counter];
    ++(*instruction_counter);
    switch (opcode) {
    case Opcode_NOOP:
        puts("NOOP");
        break;
    case Opcode_EXIT:
        puts("EXIT");
        *running = false;
        break;
    case Opcode_ADD:
        puts("ADD");
        break;
    }
}

void vm() {
  bool running = true;
  u8 instruction_counter = 0;
  u8 byte_code[] = {Opcode_NOOP, Opcode_EXIT};
  while (running) {
      single_step(&running, byte_code, &instruction_counter);
  }
}

int main() { vm(); }
