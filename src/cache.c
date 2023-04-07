#include "cache.h"

#include <stdbool.h>
#include <stdlib.h>

#include "map.h"
#include "regexp.h"  // get_next_states

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

void cache_dstate(Map* cache_table, DfaState* dstate) {
  insert_pair(cache_table, dstate->id, dstate);
}

/// @details Worst case time complexity O(n^2 * m).
/// m is the capacity of the map, which is the cost of iterating over a map; n
/// is the size of the map, each search has worst case O(n). Another n for
/// checking every key.
static bool map_equal(Map* m1, Map* m2) {
  if (get_size(m1) != get_size(m2)) {
    return false;
  }
  FOR_EACH_ITR(m1, itr, {
    if (!get_value(m2, get_current_key(itr))) {
      delete_map_iterator(itr);
      return false;
    }
  });
  return true;
}

bool has_cache(Map* cache_table, DfaState* curr_dstate, char c) {
  if (curr_dstate->next[(int)c] != NO_CACHE) {
    return true;
  }
  bool has_cache = false;
  Map* next_states = get_next_states(curr_dstate->states, c);
  FOR_EACH_ITR(cache_table, itr, {
    DfaState* dstate = get_current_value(itr);
    if (map_equal(next_states, dstate->states)) {
      curr_dstate->next[(int)c] = dstate->id;
      has_cache = true;
      break;
    }
  });
  delete_map(next_states);
  return has_cache;
}
