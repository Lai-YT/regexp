#ifndef CACHE_H
#define CACHE_H

#include "map.h"

static const int NO_CACHE = -1;

/// @brief A DfaState is a set of NFA state with possible transitions on 128
/// ASCII characters.
typedef struct DfaState {
  int id;
  Map* states;
  /// @brief The id of the next DFA state on input character; -1 if is not yet
  /// cached.
  int next[128];
} DfaState;

/// @param states The NFA states in this DFA state.
/// @note The ownership of the states is taken by the DFA state.
DfaState* create_dfa_state(Map* states);

/// @note Does not delete the next DFA states.
void delete_dfa_state(DfaState*);

#endif
