#include <stdbool.h>
#include <stdlib.h>

#include "list.h"
#include "regexp.h"

State* create_state(const int label, State** outs) {
  State* new_state;
  new_state = malloc(sizeof *new_state);
  new_state->label = label;

  size_t num_of_trans = 0;
  if (label == SPLIT) {
    num_of_trans = 2;
  } else {
    num_of_trans = 1;
  }

  new_state->outs = malloc(sizeof *new_state->outs * num_of_trans);
  if (label == ACCEPT) {
    new_state->outs[0] = NULL;
  } else {
    for (size_t i = 0; i < num_of_trans; i++) {
      new_state->outs[i] = outs[i];
    }
  }

  return new_state;
}

Nfa* create_nfa(State* start, State* accept) {
  Nfa* n;
  n = malloc(sizeof *n);
  n->start = start;
  n->accept = accept;
  return n;
}

Nfa* post2nfa(const char* post) {
  Nfa* stack[1000];
  Nfa** top = stack;

#define IS_EMPTY() (top == stack)
#define PUSH(s) (*top++ = (s))
#define POP() IS_EMPTY() ? NULL : (*--top)

  for (; *post; post++) {
    switch (*post) {
      case '.': {
        Nfa* n2 = POP();
        Nfa* n1 = POP();
        if (!n1 || !n2) {
          return NULL;
        }
        merge_state(n1->accept, n2->start);
        PUSH(create_nfa(n1->start, n2->accept));
      } break;
      case '|': {
        Nfa* n2 = POP();
        Nfa* n1 = POP();
        if (!n1 || !n2) {
          return NULL;
        }
        State* outs[2] = {n1->start, n2->start};
        State* start = create_state(SPLIT, outs);
        State* accept = create_state(ACCEPT, NULL);
        n1->accept->label = EPSILON;
        n1->accept->outs[0] = accept;
        n2->accept->label = EPSILON;
        n2->accept->outs[0] = accept;
        PUSH(create_nfa(start, accept));
      } break;
      case '*': {
        Nfa* n = POP();
        if (!n) {
          return NULL;
        }
        State* accept = create_state(ACCEPT, NULL);
        State* outs[2] = {n->start, accept};
        State* start = create_state(SPLIT, outs);
        State* come_back = create_state(SPLIT, start->outs);
        merge_state(n->accept, come_back);
        PUSH(create_nfa(start, accept));
      } break;
      case '?': {
        Nfa* n = POP();
        if (!n) {
          return NULL;
        }
        State* accept = create_state(ACCEPT, NULL);
        State* outs[2] = {n->start, accept};
        State* start = create_state(SPLIT, outs);
        n->accept->label = EPSILON;
        n->accept->outs[0] = accept;
        PUSH(create_nfa(start, accept));
      } break;
      case '+': {
        Nfa* n = POP();
        if (!n) {
          return NULL;
        }
        State* accept = create_state(ACCEPT, NULL);
        State* outs[2] = {n->start, accept};
        State* come_back = create_state(SPLIT, outs);
        merge_state(n->accept, come_back);
        PUSH(create_nfa(n->start, accept));
      } break;
      default: {
        State* accept = create_state(ACCEPT, NULL);
        State* start = create_state(*post, &accept);
        PUSH(create_nfa(start, accept));
      } break;
    }
  }

  Nfa* nfa = POP();
  return IS_EMPTY() ? nfa : NULL;

#undef POP
#undef PUSH
#undef IS_EMPTY
}

void merge_state(State* a, State* b) {
  a->label = b->label;
  a->outs = b->outs;
}

bool contains(List* l, void* val) {
  for (; l; l = l->next) {
    if (val == l->val) {
      return true;
    }
  }
  return false;
}

List* epsilon_closure(List* start) {
  List* stack = NULL;

#define PUSH(s)                 \
  List* tmp = create_list((s)); \
  append_list(tmp, stack);      \
  stack = tmp;
#define POP() (stack = stack->next)

  for (List* l = start; l; l = l->next) {
    PUSH(l->val);
  }

  List* closure = start;
  while (stack) {
    State* top = stack->val;
    POP();
    size_t num_of_epsilon_outs = 0;
    if (top->label == EPSILON) {
      num_of_epsilon_outs = 1;
    } else if (top->label == SPLIT) {
      num_of_epsilon_outs = 2;
    }
    for (size_t i = 0; i < num_of_epsilon_outs; i++) {
      if (!contains(closure, top->outs[i])) {
        append_list(closure, create_list(top->outs[i]));
        PUSH(top->outs[i]);
      }
    }
  }
  return closure;

#undef POP
#undef PUSH
}
