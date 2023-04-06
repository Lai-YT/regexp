#ifndef CACHE_H
#define CACHE_H

#include "map.h"

typedef struct DfaState {
  int id;
  Map* states;
  int next[256];  // -1 if there's no transition on such character
} DfaState;

DfaState* create_dfa_state(Map* states);
void delete_dfa_state(DfaState* root);

#endif
