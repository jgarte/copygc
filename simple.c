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

#define STACK_LENGTH 45

typedef struct Stack {
  Object *entries[STACK_LENGTH];
  int fill_pointer;
} Stack;

// void junk() {
//   Stack stack = {0}; // Initialize all fields to 0.
//   Object *o = malloc(sizeof(Object));
//   o->object_type = FLOAT;
//   o->value.float_number = 5.7;
//   stack.entries[stack.fill_pointer] = o;
// }

Stack s;

void test() {
  {
    float x = 3.0;
    Object *o = malloc(sizeof(Object));
    s.entries[0] = o;
    s.fill_pointer = 1;
  }
  {
    float x = 5.0;
    Object *o = malloc(sizeof(Object));
    s.entries[0] = o;
    s.fill_pointer = 1;
  }
  {
    float x = 1.0;
    Object *o = malloc(sizeof(Object));
    s.entries[0] = o;
    s.fill_pointer = 1;
  }
}

int main(int argc, char *argv[]) {
  printf("sizeof(Object *): %zu\n", sizeof(Object *));
  printf("sizeof(float): %zu\n", sizeof(float));
  printf("sizeof(ObjectType): %zu\n", sizeof(ObjectType));
  printf("sizeof(Object): %zu\n", sizeof(Object));
  return 0;
}