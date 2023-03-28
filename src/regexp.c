#include "regexp.h"

#include <stdbool.h>
#include <stdlib.h>

#include "list.h"
#include "map.h"
#include "post2nfa.h"
#include "re2post.h"

bool regexp(const char* re, const char* s) {
  const char* post = re2post(re);
  Nfa* nfa = post2nfa(post);
  const bool is_accepted = accepted(nfa, s);
  delete_nfa(nfa);
  return is_accepted;
}

/// @details Simulates the NFA by moving between the possible set of states.
/// If the accepting state is in the set after the last input character is
/// consumed, the NFA accepts the string.
bool accepted(const Nfa* nfa, const char* s) {
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
  List* stack = NULL;

#define PUSH(s)                 \
  List* tmp = create_list((s)); \
  append_list(tmp, stack);      \
  stack = tmp;

#define POP()          \
  List* tmp = stack;   \
  stack = stack->next; \
  tmp->next = NULL;    \
  delete_list(tmp);

  Map* closure = create_map();
  // Inserts all of the start states into the closure and the stack.
  {  // limit the scope of itr
    MapIterator* itr = create_map_iterator(start);
    while (has_next(itr)) {
      to_next(itr);
      PUSH(get_current_value(itr));
      insert_pair(closure, get_current_key(itr), get_current_value(itr));
    }
    delete_map_iterator(itr);
  }
  while (stack) {
    State* top = stack->val;
    POP();
    for (size_t i = 0; i < num_of_epsilon_outs(top->label); i++) {
      State* out = top->outs[i];
      if (!get_value(closure, out->id)) {
        insert_pair(closure, out->id, out);
        PUSH(out);
      }
    }
  }
  return closure;

#undef POP
#undef PUSH
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
