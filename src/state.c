#include "state.h"

#include <stddef.h>
#include <stdlib.h>

int num_of_outs(int label) {
  if (label == SPLIT) {
    return 2;
  }
  return 1;
}

int num_of_epsilon_outs(int label) {
  if (label == SPLIT) {
    return 2;
  }
  if (label == EPSILON) {
    return 1;
  }
  return 0;
}

State* create_state(const int label, State** outs) {
  State* new_state;
  new_state = malloc(sizeof *new_state);
  new_state->label = label;

  new_state->outs = malloc(sizeof *new_state->outs * num_of_outs(label));
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
