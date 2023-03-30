#include "post2nfa.h"

#include <stdlib.h>

#include "nfa.h"
#include "state.h"

/// @brief Merges b into a, which connects the outs of b to a.
/// @param a The state to be replace. It's contents will be lost.
/// @param b The state to replace with.
/// @note State b is deleted after the merge.
static void merge_state(State* a, State* b) {
  free(a->outs);
  a->label = b->label;
  a->outs = b->outs;
  free(b);
}

/*
 * Implements the McNaughton-Yamada-Thompson algorithm with extra supports on +
 * (one or more) and ? (zero or one) operators.
 */

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
        // this extra epsilon transition is necessary so that the link doesn't
        // break when merging the start state in concatenation
        State* start = create_state(EPSILON, &n->start);
        PUSH(create_nfa(start, accept));
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
