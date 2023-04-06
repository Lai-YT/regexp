#ifndef CACHE_H
#define CACHE_H

#include "map.h"

/// @brief A DfaState is a set of NFA state with possible transitions on 128
/// ASCII characters.
typedef struct DfaState {
  int id;
  Map* states;
  int next[128];  // -1 if there's no transition on such character
} DfaState;

DfaState* create_dfa_state(Map* states);
void delete_dfa_state(DfaState* root);

#endif
