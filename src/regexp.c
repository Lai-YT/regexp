#include "regexp.h"

#include <stdbool.h>
#include <stdlib.h>

#include "cache.h"
#include "map.h"
#include "post2nfa.h"
#include "re2post.h"
#include "stack.h"

/// @return The states reachable from the current states on label c with epsilon
/// moves.
Map* get_next_states(Map* current_states, char c);

/// @details Worst case time complexity O(n^2 * m).
/// m is the capacity of the map, which is the cost of iterating over a map; n
/// is the size of the map, each search has worst case O(n). Another n for
/// checking every key.
bool map_equal(Map* m1, Map* m2);

/// @brief Caching the ids of the DFA states.
static Map* cache_table = NULL;

/// @brief Stores the DFA state to the cache_table.
void cache_dstate(DfaState* dstate);

/// @note This function has side effect on modifing the next states of
/// curr_dstate on label c, which happens if the next states has a
/// corresponding DFA state on the cache table.
bool has_cache(DfaState* curr_dstate, char c);

/// @brief Stores the DFA state to the cache_table.
void cache_dstate(DfaState* dstate) {
  insert_pair(cache_table, dstate->id, dstate);
}

/// @note This function has side effect on modifing the next states of
/// curr_dstate on character c, which happens if the next states has a
/// corresponding DFA state on the cache table.
bool has_cache(DfaState* curr_dstate, char c) {
  if (curr_dstate->next[(int)c] != NO_CACHE) {
    return true;
  }
  bool has_cache = false;
  Map* next_states = get_next_states(curr_dstate->states, c);
  MapIterator* itr = create_map_iterator(cache_table);
  while (has_next(itr)) {
    to_next(itr);
    DfaState* dstate = get_current_value(itr);
    if (map_equal(next_states, dstate->states)) {
      curr_dstate->next[(int)c] = dstate->id;
      has_cache = true;
      break;
    }
  }
  delete_map_iterator(itr);
  delete_map(next_states);
  return has_cache;
}

Map* get_next_states(Map* current_states, char c) {
  Map* moves = move(current_states, c);
  Map* next_states = epsilon_closure(moves);
  delete_map(moves);
  return next_states;
}

/// @return The epsilon closure from start.
static Map* get_start_states(State* start) {
  Map* start_states = create_map();
  insert_pair(start_states, start->id, start);
  Map* tmp = epsilon_closure(start_states);
  delete_map(start_states);
  start_states = tmp;
  return start_states;
}

bool map_equal(Map* m1, Map* m2) {
  if (get_size(m1) != get_size(m2)) {
    return false;
  }
  MapIterator* itr = create_map_iterator(m1);
  while (has_next(itr)) {
    to_next(itr);
    if (!get_value(m2, get_current_key(itr))) {
      delete_map_iterator(itr);
      return false;
    }
  }
  delete_map_iterator(itr);
  return true;
}

/// @details Simulates the NFA by moving between the possible set of states.
/// If the accepting state is in the set after the last input character is
/// consumed, the NFA accepts the string.
bool is_accepted(const Nfa* nfa, const char* s) {
  cache_table = create_map();
  DfaState* curr_dstate = create_dfa_state(get_start_states(nfa->start));
  cache_dstate(curr_dstate);
  for (; *s; s++) {
    if (!has_cache(curr_dstate, *s)) {
      DfaState* next_dstate
          = create_dfa_state(get_next_states(curr_dstate->states, *s));
      cache_dstate(next_dstate);
      curr_dstate->next[(int)*s] = next_dstate->id;
    }
    curr_dstate = get_value(cache_table, curr_dstate->next[(int)*s]);
  }

  /// Thompson's algorithm proves that: For any regular language L, there is an
  /// NFA that accepts L that has exactly one accepting state t, which is
  /// distinct from the starting state s.
  /// See
  /// https://courses.engr.illinois.edu/cs374/fa2018/notes/models/04-nfa.pdf.
  const bool accepted = get_value(curr_dstate->states, nfa->accept->id);

  // traverse the cache table to delete all the DFA states
  MapIterator* itr = create_map_iterator(cache_table);
  while (has_next(itr)) {
    to_next(itr);
    delete_dfa_state(get_current_value(itr));
  }
  delete_map_iterator(itr);
  delete_map(cache_table);

  return accepted;
}

Map* epsilon_closure(Map* start) {
  Stack* to_reach_out = create_stack();

  Map* closure = create_map();
  // Inserts all of the start states into the closure and mark as to reach out.
  {  // limit the scope of itr
    MapIterator* itr = create_map_iterator(start);
    while (has_next(itr)) {
      to_next(itr);
      push_stack(to_reach_out, get_current_value(itr));
      insert_pair(closure, get_current_key(itr), get_current_value(itr));
    }
    delete_map_iterator(itr);
  }

  while (!is_empty_stack(to_reach_out)) {
    State* top = pop_stack(to_reach_out);
    for (size_t i = 0; i < num_of_epsilon_outs(top->label); i++) {
      State* out = top->outs[i];
      if (!get_value(closure, out->id)) {
        insert_pair(closure, out->id, out);
        push_stack(to_reach_out, out);
      }
    }
  }
  delete_stack(to_reach_out);
  return closure;
}

Map* move(Map* from, char c) {
  Map* outs = create_map();
  MapIterator* itr = create_map_iterator(from);
  while (has_next(itr)) {
    to_next(itr);
    State* s = get_current_value(itr);
    if (s->label == c) {
      insert_pair(outs, s->outs[0]->id, s->outs[0]);
    }
  }
  delete_map_iterator(itr);
  return outs;
}
