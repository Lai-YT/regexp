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

static void delete_state_chain(State* s) {
  Map* states_to_delete = create_map();
  collect_states(states_to_delete, s);

  MapIterator* itr = create_map_iterator(states_to_delete);
  while (has_next(itr)) {
    to_next(itr);
    delete_state(get_current_value(itr));
  }
  delete_map_iterator(itr);

  delete_map(states_to_delete);
}
