#include "nfa.h"

#include <stdlib.h>

#include "map.h"
#include "state.h"

Nfa* create_nfa(State* start, State* accept) {
  Nfa* n;
  n = malloc(sizeof *n);
  n->start = start;
  n->accept = accept;
  return n;
}

/// @brief Collects all of the states reachable from start into the map.
/// @param states Mapping of states which the states will be inserted into.
/// @param start The start state which all the states reachable from it will be
/// collected.
static void collect_states(Map* states, State* start) {
  if (get_value(states, start->id)) {
    return;
  }
  insert_pair(states, start->id, start);
  if (start->label != ACCEPT) {
    collect_states(states, start->outs[0]);
    if (start->label == SPLIT) {
      collect_states(states, start->outs[1]);
    }
  }
}

/// @brief Deletes all of the states reachable from s.
static void delete_state_chain(State* s);

void delete_nfa(Nfa* nfa) {
  delete_state_chain(nfa->start);
  free(nfa);
}

/// @brief Deletes all of the states that are reachable from state and are in
/// the map.
/// @note A helper function for delete_state_chain.
static void delete_state_chain_recursion(Map* states_to_delete, State* state) {
  if (!get_value(states_to_delete, state->id)) {
    return;
  }
  // Note that you have to mark as deleted before dealing with the outs,
  // otherwise if there's a cycle, you'll be in an infinite loop, which causes
  // the stack to overflow.
  delete_pair(states_to_delete, state->id);
  if (state->label != ACCEPT) {
    delete_state_chain_recursion(states_to_delete, state->outs[0]);
    if (state->label == SPLIT) {
      delete_state_chain_recursion(states_to_delete, state->outs[1]);
    }
  }
  delete_state(state);
}

static void delete_state_chain(State* s) {
  Map* states_to_delete = create_map();
  collect_states(states_to_delete, s);
  delete_state_chain_recursion(states_to_delete, s);
  delete_map(states_to_delete);
}
