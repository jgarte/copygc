#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "canvas.h"

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

bool stackPop(Stack *s) {
  if (s->fill_pointer < 0) {
    return false;
  }
  s->entries[s->fill_pointer] = NULL;
  --s->fill_pointer;
  return true;
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

typedef enum {
  Opcode_NOOP,
  Opcode_EXIT,
  Opcode_ADD,
  Opcode_PUSH,
  Opcode_POP,
  Opcode_PRINT
} Opcode;

typedef uint8_t u8;

u8 read_argument(u8 *byte_code, u8 *instruction_counter) {
  return byte_code[(*instruction_counter)++];
}

void instruction_push_float(u8 *byte_code, u8 *instruction_counter) {
  Object *o = allocateObject();
  u8 number = read_argument(byte_code, instruction_counter);
  float f = number;
  setFloat(o, f);
  stackPush(&g_s, o);
}

void single_step(bool *running, u8 *byte_code, u8 *instruction_counter,
                 Canvas *canvas) {
  (void)canvas;
  u8 opcode = byte_code[*instruction_counter];
  ++(*instruction_counter);
  switch (opcode) {
  case Opcode_NOOP:
    break;
  case Opcode_EXIT:
    *running = false;
    break;
  case Opcode_ADD:
    break;
  case Opcode_PUSH: {
    instruction_push_float(byte_code, instruction_counter);
  }
  }
}

void stackFree(Stack *s) {
  for (size_t i = 0; i < s->fill_pointer; i++) {
    free(s->entries[i]);
  }
}

void vm(Canvas *canvas) {
  bool running = true;
  u8 instruction_counter = 0;
  u8 byte_code[] = {Opcode_PUSH, 1, Opcode_PUSH, 2, Opcode_PUSH, 3,
                    Opcode_PUSH, 4, Opcode_EXIT};
  while (running) {
    single_step(&running, byte_code, &instruction_counter, canvas);
    /* canvas_render(canvas); */
  }
  printStack(&g_s);
  stackFree(&g_s);
}

int main() {
  Canvas canvas;
  /* canvas_init(&canvas, 16, 600); */

  vm(&canvas);

  /* while (1) { */
  /*   SDL_Event event; */
  /*   SDL_PollEvent(&event); */
  /*   if (event.type == SDL_EVENT_QUIT) break; */
  /*   canvas_render(&canvas); */
  /* } */

  /* canvas_quit(&canvas); */
}
