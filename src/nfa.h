#ifndef NFA_H
#define NFA_H

#include "state.h"

typedef struct Nfa {
  State* start;
  State* accept;
} Nfa;

/// @note The ownership of all the states connected between start and accept are
/// taken by the NFA.
Nfa* create_nfa(State* start, State* accept);

/// @brief Deletes the NFA and all the states it contains.
void delete_nfa(Nfa*);

#endif /* end of include guard: NFA_H s*/
