#ifndef VISSTATE_H
#define VISSTATE_H

#include <stdio.h>

#include "post2nfa.h"

/// @brief Converts the states in nfa into a Graphviz dot file
/// and writes into stream f.
void nfa2dot(const Nfa* nfa, FILE* f);

#endif /* end of include guard: VISSTATE_H */
