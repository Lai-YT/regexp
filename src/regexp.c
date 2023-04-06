#include "regexp.h"

#include <stdbool.h>
#include <stdlib.h>

#include "map.h"
#include "post2nfa.h"
#include "re2post.h"
#include "stack.h"

static int state_id = 0;

typedef struct DfaState {
  int id;
  Map* states;
  int next[256];
} DfaState;

DfaState* create_dfa_state(Map* states) {
  DfaState* state = malloc(sizeof(DfaState));
  state->id = state_id++;
  state->states = states;
  for (int i = 0; i < 256; i++) {
    state->next[i] = -1;
  }
  return state;
}

void delete_dfa_state(DfaState* root) {
  delete_map(root->states);
  free(root);
}

/// @details Worst case time complexity O(n^2 * m).
/// m is the capacity of the map, which is the cost of iterating over a map; n
/// is the size of the map, each search has worst case O(n). Another n for
/// checking every key.
bool map_cmp(Map* m1, Map* m2) {
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
  Map* start = create_map();
  insert_pair(start, nfa->start->id, nfa->start);
  Map* states = epsilon_closure(start);
  delete_map(start);
  Map* dstates = create_map();
  DfaState* dstate = create_dfa_state(states);
  insert_pair(dstates, dstate->id, dstate);
  for (; *s; s++) {
    if (dstate->next[(int)*s] == -1) {
      bool has_cache = false;
      Map* moves = move(dstate->states, *s);
      states = epsilon_closure(moves);
      delete_map(moves);
      MapIterator* itr = create_map_iterator(dstates);
      while (has_next(itr)) {
        to_next(itr);
        if (map_cmp(states, ((DfaState*)get_current_value(itr))->states)) {
          dstate->next[(int)*s] = get_current_key(itr);
          delete_map(states);
          states = NULL;
          has_cache = true;
          break;
        }
      }
      delete_map_iterator(itr);
      if (!has_cache) {
        DfaState* next_dstate = create_dfa_state(states);
        insert_pair(dstates, next_dstate->id, next_dstate);
        dstate->next[(int)*s] = next_dstate->id;
      }
    }
    dstate = get_value(dstates, dstate->next[(int)*s]);
  }

  /// Thompson's algorithm proves that: For any regular language L, there is an
  /// NFA that accepts L that has exactly one accepting state t, which is
  /// distinct from the starting state s.
  /// See
  /// https://courses.engr.illinois.edu/cs374/fa2018/notes/models/04-nfa.pdf.
  const bool accepted = get_value(dstate->states, nfa->accept->id);
  MapIterator* itr = create_map_iterator(dstates);
  while (has_next(itr)) {
    to_next(itr);
    delete_dfa_state(get_current_value(itr));
  }
  delete_map_iterator(itr);
  delete_map(dstates);
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
