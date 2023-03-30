#include <limits.h>
#include <stdio.h>

#include "list.h"
#include "map.h"
#include "post2nfa.h"

static void push(List** stack, State* s) {
  List* tmp = create_list((s));
  append_list(tmp, *stack);
  *stack = tmp;
}

static State* pop(List** stack) {
  State* top = (*stack)->val;
  List* top_list = *stack;
  *stack = (*stack)->next;
  top_list->next = NULL;
  delete_list(top_list);
  return top;
}

static void state2dot(State* state, FILE* f) {
  for (size_t i = 0; i < num_of_outs(state->label); i++) {
    fprintf(f, "\t%d -> %d", state->id, state->outs[i]->id);
    fputs(" [label = ", f);
    if (state->label > CHAR_MAX) {
      fputs("eps", f);  // epsilon, avoid unicode
    } else {
      fputc(state->label, f);
    }
    fputs("]\n", f);
  }
}

static void states2dot(State* start, FILE* f) {
  Map* converted = create_map();
  List* to_convert = NULL;
  push(&to_convert, start);
  while (to_convert) {  // depth-first traversal
    State* s = pop(&to_convert);
    if (get_value(converted, s->id) /* is converted */
        || s->label == ACCEPT /* has no transition */) {
      continue;
    }
    state2dot(s, f);
    insert_pair(converted, s->id, s);  // mark as converted
    for (size_t i = 0; i < num_of_outs(s->label); i++) {
      push(&to_convert, s->outs[i]);
    }
  }
}

void nfa2dot(const Nfa* nfa, FILE* f) {
  fputs("strict digraph nfa {\n", f);
  fputs("\trankdir=LR;\n", f);
  fprintf(f, "\tnode [shape = doublecircle]; %d;", nfa->accept->id);
  fputs("\tnode [shape = circle];\n", f);
  states2dot(nfa->start, f);
  fputs("}\n", f);
}
