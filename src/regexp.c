#include "regexp.h"

#include <stdbool.h>
#include <stdlib.h>

#include "list.h"
#include "map.h"
#include "post2nfa.h"
#include "re2post.h"
#include "set.h"

bool regexp(const char* re, const char* s) {
  const char* post = re2post(re);
  Nfa* nfa = post2nfa(post);
  const bool is_accepted = accepted(nfa, s);
  delete_nfa(nfa);
  return is_accepted;
}

Map* epsilon_closure_xx(Map* start) {
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
    while (has_next_xx(itr)) {
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

Map* move_xx(Map* from, char c) {
  Map* outs = create_map();
  MapIterator* itr = create_map_iterator(from);
  while (has_next_xx(itr)) {
    to_next(itr);
    State* s = get_current_value(itr);
    if (s->label == c) {
      insert_pair(outs, s->outs[0]->id, s->outs[0]);
    }
  }
  delete_map_iterator(itr);
  return outs;
}

/// @details Simulates the NFA by moving between the possible set of states.
/// If the accepting state is in the set after the last input character is
/// consumed, the NFA accepts the string.
bool accepted(const Nfa* nfa, const char* s) {
  Set* start = create_set();
  Map* start_xx = create_map();
  insert_pair(start_xx, nfa->start->id, nfa->start);
  insert_key(start, nfa->start);
  Set* states = epsilon_closure(start);
  Map* states_xx = epsilon_closure_xx(start_xx);
  for (; *s; s++) {
    Set* moves = move(states, *s);
    Map* moves_xx = move_xx(states_xx, *s);
    delete_set(states);
    delete_map(states_xx);
    states = epsilon_closure(moves);
    states_xx = epsilon_closure_xx(moves_xx);
    delete_set(moves);
    delete_map(moves_xx);
  }

  /// Thompson's algorithm proves that: For any regular language L, there is an
  /// NFA that accepts L that has exactly one accepting state t, which is
  /// distinct from the starting state s.
  /// See
  /// https://courses.engr.illinois.edu/cs374/fa2018/notes/models/04-nfa.pdf.
  const bool accepted = has_key(states, nfa->accept);
  const bool accepted_xx = get_value(states_xx, nfa->accept->id);
  delete_set(states);
  delete_set(start);
  delete_map(states_xx);
  delete_map(start_xx);
  return accepted_xx;
}

Set* epsilon_closure(Set* start) {
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

  Set* closure = create_set();
  // Inserts all of the start states into the closure and the stack.
  {  // limit the scope of itr
    SetIterator* itr = create_iterator(start);
    while (has_next(itr)) {
      next(itr);
      PUSH(get_key(itr));
      insert_key(closure, get_key(itr));
    }
    delete_iterator(itr);
  }
  while (stack) {
    State* top = stack->val;
    POP();
    for (size_t i = 0; i < num_of_epsilon_outs(top->label); i++) {
      State* out = top->outs[i];
      if (!has_key(closure, out)) {
        insert_key(closure, out);
        PUSH(out);
      }
    }
  }
  return closure;

#undef POP
#undef PUSH
}

Set* move(Set* from, char c) {
  Set* outs = create_set();
  SetIterator* itr = create_iterator(from);
  while (has_next(itr)) {
    next(itr);
    State* s = get_key(itr);
    if (s->label == c) {
      insert_key(outs, s->outs[0]);
    }
  }
  delete_iterator(itr);
  return outs;
}
