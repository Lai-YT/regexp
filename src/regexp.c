#include <assert.h>

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

#define PUSH(s) (*top++ = (s))
#define POP() (*--top)

  for (; *post; post++) {
    switch (*post) {
      case '.': {
        Nfa* n2 = POP();
        Nfa* n1 = POP();
        // NOTE: the original algorithm fuses the accept state of n1 with the
        // start state of n2 while here we don't, which leads to an extra
        // epsilon transition.
        n1->accept->label = EPSILON;
        n1->accept->outs[0] = n2->start;
        PUSH(create_nfa(n1->start, n2->accept));
      } break;
      case '|': {
        Nfa* n2 = POP();
        Nfa* n1 = POP();
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
        State* accept = create_state(ACCEPT, NULL);
        State* outs[2] = {n->start, accept};
        State* start = create_state(SPLIT, outs);
        // NOTE: the original algorithm fuses the accept state of n with
        // come_back while here we don't, which leads to an extra epsilon
        // transition.
        State* come_back = create_state(SPLIT, start->outs);
        n->accept->label = EPSILON;
        n->accept->outs[0] = come_back;
        PUSH(create_nfa(start, accept));
      } break;
      case '?': {
        Nfa* n = POP();
        State* accept = create_state(ACCEPT, NULL);
        State* outs[2] = {n->start, accept};
        State* start = create_state(SPLIT, outs);
        n->accept->label = EPSILON;
        n->accept->outs[0] = accept;
        PUSH(create_nfa(start, accept));
      } break;
      case '+': {
        Nfa* n = POP();
        State* accept = create_state(ACCEPT, NULL);
        State* outs[2] = {n->start, accept};
        State* come_back = create_state(SPLIT, outs);
        n->accept->label = EPSILON;
        n->accept->outs[0] = come_back;
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
#define IS_EMPTY() (top == stack)
  return IS_EMPTY() ? nfa : NULL;
#undef IS_EMPTY
#undef POP
#undef PUSH
}
