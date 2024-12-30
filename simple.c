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

Stack s;

void printStack(Stack *s) {
  printf("fill_pointer: %zu\n", s->fill_pointer);

  for (int i = 0; i < STACK_LENGTH; i++) {
    printf("index: %i\n", i);
    printObject(s->entries[i]);
  }
};

void test() {
  {
    Object *o = malloc(sizeof(Object));
    s.entries[0] = o;
    s.fill_pointer = 1;
  }
  {
    Object *o = malloc(sizeof(Object));
    s.entries[1] = o;
    s.fill_pointer = 2;
  }
  {
    Object *o = malloc(sizeof(Object));
    s.entries[2] = o;
    s.fill_pointer = 3;
  }
  {
    Object *o = malloc(sizeof(Object));
    s.entries[3] = o;
    s.fill_pointer = 4;
  }
  {
    Object *o = malloc(sizeof(Object));
    s.entries[4] = o;
    s.fill_pointer = 4;
  }
  printStack(&s);
}

int main() { test(); }