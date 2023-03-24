#include "post2nfa.h"

#include <stdbool.h>
#include <stddef.h>  // size_t
#include <stdlib.h>

#include "list.h"

/*
 * Implements the McNaughton-Yamada-Thompson algorithm with extra supports on +
 * (one or more) and ? (zero or one) operators.
 */

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

void delete_state(State* s) {
  free(s->outs);
  free(s);
}

Nfa* create_nfa(State* start, State* accept) {
  Nfa* n;
  n = malloc(sizeof *n);
  n->start = start;
  n->accept = accept;
  return n;
}

static bool contains(List* l, void* val) {
  for (; l; l = l->next) {
    if (val == l->val) {
      return true;
    }
  }
  return false;
}

static void collect_states(List** states, State* start) {
  if (contains(*states, start)) {
    return;
  }
  if (!(*states)) {
    *states = create_list(start);
  } else {
    append_list(*states, create_list(start));
  }
  if (start->label == ACCEPT) {
    return;
  }
  collect_states(states, start->outs[0]);
  if (start->label == SPLIT) {
    collect_states(states, start->outs[1]);
  }
}

static void delete_state_chain(State* s) {
  List* states = NULL;
  collect_states(&states, s);
  for (List* c = states; c; c = c->next) {
    State* s_ = c->val;
    if (s_->label < 128) {
    } else {
    }
    delete_state(c->val);
  }
  delete_list(states);
}

void delete_nfa(Nfa* nfa) {
  delete_state_chain(nfa->start);
  free(nfa);
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
          if (n1) {
            delete_nfa(n1);
          }
          if (n2) {
            delete_nfa(n2);
          }
          return NULL;
        }
        merge_state(n1->accept, n2->start);
        PUSH(create_nfa(n1->start, n2->accept));
        free(n1);
        free(n2);
      } break;
      case '|': {
        Nfa* n2 = POP();
        Nfa* n1 = POP();
        if (!n1 || !n2) {
          if (n1) {
            delete_nfa(n1);
          }
          if (n2) {
            delete_nfa(n2);
          }
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
        free(n1);
        free(n2);
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
        free(n);
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
        free(n);
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
        free(n);
      } break;
      default: {
        State* accept = create_state(ACCEPT, NULL);
        State* start = create_state(*post, &accept);
        PUSH(create_nfa(start, accept));
      } break;
    }
  }

  Nfa* nfa = POP();
  if (!IS_EMPTY()) {
    delete_nfa(nfa);
    while (!IS_EMPTY()) {
      Nfa* n = POP();
      delete_nfa(n);
    }
    return NULL;
  }
  return nfa;

#undef POP
#undef PUSH
#undef IS_EMPTY
}

void merge_state(State* a, State* b) {
  free(a->outs);
  a->label = b->label;
  a->outs = b->outs;
  free(b);
}
