#include "regexp.h"

#include <stdbool.h>
#include <stdlib.h>

#include "list.h"
#include "post2nfa.h"
#include "re2post.h"

bool regexp(const char* re, const char* s) {
  const char* post = re2post(re);
  const Nfa* nfa = post2nfa(post);
  return accepted(nfa, s);
}

bool accepted(const Nfa* nfa, const char* s) {
  List* states = epsilon_closure(create_list(nfa->start));
  for (; *s; s++) {
    states = epsilon_closure(move(states, *s));
  }
  return has_accept(states);
}

bool contains(List* l, void* val) {
  for (; l; l = l->next) {
    if (val == l->val) {
      return true;
    }
  }
  return false;
}

List* epsilon_closure(List* start) {
  List* stack = NULL;

#define PUSH(s)                 \
  List* tmp = create_list((s)); \
  append_list(tmp, stack);      \
  stack = tmp;
#define POP() (stack = stack->next)

  for (List* l = start; l; l = l->next) {
    PUSH(l->val);
  }

  List* closure = start;
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
      if (!contains(closure, top->outs[i])) {
        append_list(closure, create_list(top->outs[i]));
        PUSH(top->outs[i]);
      }
    }
  }
  return closure;

#undef POP
#undef PUSH
}

List* move(List* l, char c) {
  List* outs = NULL;
  for (; l; l = l->next) {
    State* s = l->val;
    if (s->label == c) {
      // appending is not safe since outs may be NULL, so prepends
      List* tmp = create_list(s->outs[0]);
      append_list(tmp, outs);
      outs = tmp;
    }
  }
  return outs;
}

bool has_accept(List* l) {
  for (; l; l = l->next) {
    State* s = l->val;
    if (s->label == ACCEPT) {
      return true;
    }
  }
  return false;
}
