#ifndef REGEXP_H
#define REGEXP_H

#include <stdbool.h>

#include "list.h"
#include "post2nfa.h"
#include "set.h"

/// @brief Returns whether s is matched by re.
bool regexp(const char* re, const char* s);

/// @brief Simulates the NFA.
/// @return whether the NFA accepts the string.
bool accepted(const Nfa*, const char*);

/// @brief Returns the states that are reachable from start with only epsilon
/// transitions, including all of the start states itself.
Set* epsilon_closure(Set* start);

/// @brief Returns the states that are reachable from list of states l on label
/// c with non-epsilon moves.
Set* move(Set* from, char c);

#endif /* end of include guard: REGEXP_H */
