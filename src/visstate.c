#include <stdio.h>

#include "map.h"
#include "post2nfa.h"
#include "stack.h"

/// @details The label of an epsilon transition is "eps", others are the
/// characters they take.
static void state2dot(State* state, FILE* f) {
  for (size_t i = 0; i < num_of_outs(state->label); i++) {
    fprintf(f, "\t%d -> %d", state->id, state->outs[i]->id);
    fputs(" [label = \"", f);
    if (state->label == SPLIT || state->label == EPSILON) {
      fputs("eps", f);  // epsilon, avoid unicode
    } else if (state->label == ANY) {
      fputs("any", f);
    } else {
      fputc(state->label, f);
    }
    fputs("\"]\n", f);
  }
}

static void states2dot(State* start, FILE* f) {
  Map* converted = create_map();
  Stack* to_convert = create_stack();
  push_stack(to_convert, start);
  while (!is_empty_stack(to_convert)) {  // depth-first traversal
    State* s = pop_stack(to_convert);
    if (get_value(converted, s->id) /* is converted */
        || s->label == ACCEPT /* has no transition */) {
      continue;
    }
    state2dot(s, f);
    insert_pair(converted, s->id, s);  // mark as converted
    for (size_t i = 0; i < num_of_outs(s->label); i++) {
      push_stack(to_convert, s->outs[i]);
    }
  }
  delete_stack(to_convert);
}

/// @details A strict digraph that goes from left to right. The accepting state
/// is represented as a double circle, others are circles. Every states have the
/// same node size.
void nfa2dot(const Nfa* nfa, FILE* f) {
  fputs("strict digraph nfa {\n", f);
  fputs("\trankdir=LR;\n", f);
  fputs("\tnode [fixedsize=true];\n", f);
  fprintf(f, "\tnode [shape = doublecircle]; %d;\n", nfa->accept->id);
  fputs("\tnode [shape = circle];\n", f);
  states2dot(nfa->start, f);
  // Generate an in-arrow to the start state.
  // The name is irrelevant since it won't be displayed.
  fputs("\tstart [shape = none, label = \"\"];\n", f);
  fprintf(f, "\tstart -> %d\n", nfa->start->id);
  fputs("}\n", f);
}
