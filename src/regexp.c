#include "regexp.h"

#include <stdbool.h>
#include <stdlib.h>

#include "map.h"
#include "post2nfa.h"
#include "re2post.h"
#include "stack.h"

/// @details Simulates the NFA by moving between the possible set of states.
/// If the accepting state is in the set after the last input character is
/// consumed, the NFA accepts the string.
bool is_accepted(const Nfa* nfa, const char* s) {
  Map* start = create_map();
  insert_pair(start, nfa->start->id, nfa->start);
  Map* states = epsilon_closure(start);
  for (; *s; s++) {
    Map* moves = move(states, *s);
    delete_map(states);
    states = epsilon_closure(moves);
    delete_map(moves);
  }

  /// Thompson's algorithm proves that: For any regular language L, there is an
  /// NFA that accepts L that has exactly one accepting state t, which is
  /// distinct from the starting state s.
  /// See
  /// https://courses.engr.illinois.edu/cs374/fa2018/notes/models/04-nfa.pdf.
  const bool accepted = get_value(states, nfa->accept->id);
  delete_map(states);
  delete_map(start);
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
