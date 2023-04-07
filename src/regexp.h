#ifndef REGEXP_H
#define REGEXP_H

#include <stdbool.h>

#include "map.h"
#include "post2nfa.h"

/// @return Whether the string is accepted by the NFA.
/// @details Simulates the NFA.
bool is_accepted(const Nfa*, const char*);

/// @return The states that are reachable from start with only epsilon
/// transitions, including all of the start states itself.
Map* epsilon_closure(Map* start);

/// @return The states that are reachable from the map of states on label
/// c with non-epsilon moves.
Map* move(Map*, char c);

#endif /* end of include guard: REGEXP_H */
