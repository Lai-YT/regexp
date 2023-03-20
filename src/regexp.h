#ifndef REGEXP_H
#define REGEXP_H

#include <stdlib.h>

/*
 * Implements the McNaughton-Yamada-Thompson algorithm with extra supports on +
 * (one or more) and ? (zero or one) operators.
 */

enum {
  EPSILON = 128,
  SPLIT = 129,
  ACCEPT = 130,
};

typedef struct State {
  int label;
  struct State** outs;
} State;

/// @brief A labeled state have 1 labeled transition, an epsilon state (label ==
/// EPSILON) has 1 epsilon transition, a spliting state (label == SPLIT) has 2
/// epsilon transitions, an accepting state (label == ACCEPT) has 1 reserved
/// transition (see note).
/// @note The accepting state may later become part of an NFA and turns into an
/// epsilon state. outs is ignored under this condition since the space is only
/// reserved.
State* create_state(const int label, State** outs);

typedef struct Nfa {
  State* start;
  State* accept;
} Nfa;

Nfa* create_nfa(State* start, State* accept);

Nfa* post2nfa(const char* post);

/// @brief Merges b into a, which connects the outs of b to a.
/// @param a The state to be replace. It's contents will be lost.
/// @param b The state to replace with.
void merge_state(State* a, State* b);

/// @brief Returns the states that are reachable from start with only epsilon
/// transitions, including start itself.
List* epsilon_closure(List* start);

#endif /* end of include guard: REGEXP_H */
