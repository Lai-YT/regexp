#include "stack.h"

#include <stdlib.h>

typedef struct StackNode {
  void* val;
  struct StackNode* next;
} StackNode;

static StackNode* create_stack_node(void* value, StackNode* next) {
  StackNode* node = malloc(sizeof(StackNode));
  node->val = value;
  node->next = next;
  return node;
}

static void delete_stack_node(StackNode* node) {
  free(node);
}

struct Stack {
  StackNode* top;
};

Stack* create_stack() {
  Stack* s = malloc(sizeof(Stack));
  s->top = NULL;
  return s;
}

void push_stack(Stack* s, void* value) {
  StackNode* new_node = create_stack_node(value, s->top);
  s->top = new_node;
}

void* pop_stack(Stack* s) {
  if (is_empty_stack(s)) {
    return NULL;
  }
  StackNode* top = s->top;
  void* top_value = top->val;
  s->top = s->top->next;
  delete_stack_node(top);
  return top_value;
}

bool is_empty_stack(Stack* s) {
  return s->top == NULL;
}

void delete_stack(Stack* s) {
  while (!is_empty_stack(s)) {
    StackNode* top = s->top;
    s->top = s->top->next;
    delete_stack_node(top);
  }
  free(s);
}
