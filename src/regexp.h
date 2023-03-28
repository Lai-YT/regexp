#ifndef REGEXP_H
#define REGEXP_H

#include <stdbool.h>

#include "list.h"
#include "map.h"
#include "post2nfa.h"

/// @brief Returns whether s is matched by re.
bool regexp(const char* re, const char* s);

/// @brief Simulates the NFA.
/// @return whether the NFA accepts the string.
bool accepted(const Nfa*, const char*);

/// @brief Returns the states that are reachable from start with only epsilon
/// transitions, including all of the start states itself.
Map* epsilon_closure(Map* start);

/// @brief Returns the states that are reachable from the map of states on label
/// c with non-epsilon moves.
Map* move(Map*, char c);

#endif /* end of include guard: REGEXP_H */
