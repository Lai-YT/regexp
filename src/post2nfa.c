#include "post2nfa.h"

#include <stdbool.h>
#include <stddef.h>  // size_t
#include <stdlib.h>

#include "list.h"
#include "set.h"

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

/// @brief Collects all of the states reachable from start into the set.
/// @param states Set of states which the states will be inserted into.
/// @param start The start state which all the states reachable from it will be
/// collected.
static void collect_states(Set* states, State* start) {
  if (has_key(states, start)) {
    return;
  }
  insert_key(states, start);
  if (start->label != ACCEPT) {
    collect_states(states, start->outs[0]);
    if (start->label == SPLIT) {
      collect_states(states, start->outs[1]);
    }
  }
}

/// @brief Deletes all of the states reachable from s.
static void delete_state_chain(State* s);

void delete_nfa(Nfa* nfa) {
  delete_state_chain(nfa->start);
  free(nfa);
}

/// @brief Deletes all of the states that are reachable from state and are in
/// the set.
/// @note A helper function for delete_state_chain.
static void delete_state_chain_recursion(Set* states_to_delete, State* state) {
  if (!has_key(states_to_delete, state)) {
    return;
  }
  // Note that you have to mark as deleted before dealing with the outs,
  // otherwise if there's a cycle, you'll be in an infinite loop, which causes
  // the stack to overflow.
  delete_key(states_to_delete, state);
  if (state->label != ACCEPT) {
    delete_state_chain_recursion(states_to_delete, state->outs[0]);
    if (state->label == SPLIT) {
      delete_state_chain_recursion(states_to_delete, state->outs[1]);
    }
  }
  delete_state(state);
}

static void delete_state_chain(State* s) {
  Set* states_to_delete = create_set();
  collect_states(states_to_delete, s);
  delete_state_chain_recursion(states_to_delete, s);
  delete_set(states_to_delete);
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
