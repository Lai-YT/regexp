#include "regexp.h"

#include <stdbool.h>
#include <stdlib.h>

#include "list.h"
#include "post2nfa.h"
#include "re2post.h"
#include "set.h"

bool regexp(const char* re, const char* s) {
  const char* post = re2post(re);
  const Nfa* nfa = post2nfa(post);
  return accepted(nfa, s);
}

/// @details Simulates the NFA by moving between the possible set of states.
/// If the accepting state is in the set after the last input character is
/// consumed, the NFA accepts the string.
bool accepted(const Nfa* nfa, const char* s) {
  Set* start = create_set();
  insert_key(start, nfa->start);
  Set* states = epsilon_closure(start);
  for (; *s; s++) {
    Set* moves = move(states, *s);
    delete_set(states);
    states = epsilon_closure(moves);
    delete_set(moves);
  }

  /// Thompson's algorithm proves that: For any regular language L, there is an
  /// NFA that accepts L that has exactly one accepting state t, which is
  /// distinct from the starting state s.
  /// See
  /// https://courses.engr.illinois.edu/cs374/fa2018/notes/models/04-nfa.pdf.
  const bool accepted = has_key(states, nfa->accept);
  delete_set(states);
  delete_set(start);
  return accepted;
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
    size_t num_of_epsilon_outs = 0;
    if (top->label == EPSILON) {
      num_of_epsilon_outs = 1;
    } else if (top->label == SPLIT) {
      num_of_epsilon_outs = 2;
    }
    for (size_t i = 0; i < num_of_epsilon_outs; i++) {
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
