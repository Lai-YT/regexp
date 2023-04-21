#include "state.h"

#include <stddef.h>
#include <stdlib.h>

size_t num_of_outs(int label) {
  if (label == SPLIT) {
    return 2;
  }
  return 1;
}

size_t num_of_epsilon_outs(int label) {
  if (label == SPLIT) {
    return 2;
  }
  if (label == EPSILON) {
    return 1;
  }
  return 0;
}

/// @brief A non-duplicate state id that assigned to the state right after the
/// creation and then incremented.
static int state_id = 0;

State* create_state(const int label, State** outs) {
  State* new_state = malloc(sizeof(State));
  new_state->label = label;
  new_state->id = state_id++;

  new_state->outs = malloc(sizeof(State) * num_of_outs(label));
  if (label == ACCEPT) {
    new_state->outs[0] = NULL;
  } else {
    for (size_t i = 0; i < num_of_outs(label); i++) {
      new_state->outs[i] = outs[i];
    }
  }

  return new_state;
}

void delete_state(State* s) {
  free(s->outs);
  free(s);
}
