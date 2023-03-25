#ifndef POST2NFA_H
#define POST2NFA_H

#include "re2post.h"

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
/// transition.
int num_of_outs(int label);

/// @brief An epsilon state has 1, a spliting state has 2, others have 0.
int num_of_epsilon_outs(int label);

/// @note The accepting state may later become part of an NFA and turns into an
/// epsilon state. outs is ignored under this condition since the space is only
/// reserved.
State* create_state(const int label, State** outs);

/// @brief Deletes the state but not the states it transits to.
void delete_state(State*);

typedef struct Nfa {
  State* start;
  State* accept;
} Nfa;

/// @note The ownership of all the states connected between start and accept are
/// taken by the NFA.
Nfa* create_nfa(State* start, State* accept);

/// @brief Deletes the NFA and all the states it contains.
void delete_nfa(Nfa*);

Nfa* post2nfa(const char* post);

/// @brief Merges b into a, which connects the outs of b to a.
/// @param a The state to be replace. It's contents will be lost.
/// @param b The state to replace with.
/// @note State b is deleted after the merge.
void merge_state(State* a, State* b);

#endif /* end of include guard: POST2NFA_H */
