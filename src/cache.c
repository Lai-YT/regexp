#include "cache.h"

#include <stdlib.h>

#include "map.h"

static int state_id = 0;

DfaState* create_dfa_state(Map* states) {
  DfaState* state = malloc(sizeof(DfaState));
  state->id = state_id++;
  state->states = states;
  for (int i = 0; i < 128; i++) {
    state->next[i] = NO_CACHE;
  }
  return state;
}

void delete_dfa_state(DfaState* dstate) {
  delete_map(dstate->states);
  free(dstate);
}
