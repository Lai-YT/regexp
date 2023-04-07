#include "regexp.h"

#include <stdbool.h>
#include <stdlib.h>

#include "cache.h"
#include "map.h"
#include "post2nfa.h"
#include "re2post.h"
#include "stack.h"

/// @return The epsilon closure from start.
static Map* get_start_states(State* start);

static bool cache_on_the_fly = true;

/// @details Simulates the NFA by moving between the possible set of states.
/// If the accepting state is in the set after the last input character is
/// consumed, the NFA accepts the string.
bool is_accepted(const Nfa* nfa, const char* s) {
  if (cache_on_the_fly) {
    /// @brief Caching the ids of the DFA states.
    Map* cache_table = create_map();
    DfaState* curr_dstate = create_dfa_state(get_start_states(nfa->start));
    cache_dstate(cache_table, curr_dstate);
    for (; *s; s++) {
      if (!has_cache(cache_table, curr_dstate, *s)) {
        DfaState* next_dstate
            = create_dfa_state(get_next_states(curr_dstate->states, *s));
        cache_dstate(cache_table, next_dstate);
        curr_dstate->next[(int)*s] = next_dstate->id;
      }
      curr_dstate = get_value(cache_table, curr_dstate->next[(int)*s]);
    }

    const bool accepted = get_value(curr_dstate->states, nfa->accept->id);

    // traverse the cache table to delete all the DFA states
    FOR_EACH_ITR(cache_table, itr, delete_dfa_state(get_current_value(itr)));
    delete_map(cache_table);

    return accepted;
  }
  Map* states = get_start_states(nfa->start);
  for (; *s; s++) {
    Map* next_states = get_next_states(states, *s);
    delete_map(states);
    states = next_states;
  }

  /// Thompson's algorithm proves that: For any regular language L, there is
  /// an NFA that accepts L that has exactly one accepting state t, which is
  /// distinct from the starting state s.
  /// See
  /// https://courses.engr.illinois.edu/cs374/fa2018/notes/models/04-nfa.pdf.
  const bool accepted = get_value(states, nfa->accept->id);
  delete_map(states);
  return accepted;
}

Map* epsilon_closure(Map* start) {
  Stack* to_reach_out = create_stack();

  Map* closure = create_map();
  // Inserts all of the start states into the closure and mark as to reach out.
  FOR_EACH_ITR(start, itr, {
    push_stack(to_reach_out, get_current_value(itr));
    insert_pair(closure, get_current_key(itr), get_current_value(itr));
  });

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
  FOR_EACH_ITR(from, itr, {
    State* s = get_current_value(itr);
    if (s->label == c) {
      insert_pair(outs, s->outs[0]->id, s->outs[0]);
    }
  });
  return outs;
}

static Map* get_start_states(State* start) {
  Map* start_states = create_map();
  insert_pair(start_states, start->id, start);
  Map* tmp = epsilon_closure(start_states);
  delete_map(start_states);
  start_states = tmp;
  return start_states;
}

Map* get_next_states(Map* current_states, char c) {
  Map* moves = move(current_states, c);
  Map* next_states = epsilon_closure(moves);
  delete_map(moves);
  return next_states;
}
