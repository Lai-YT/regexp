#ifndef REGEXP_H
#define REGEXP_H

#include <stdbool.h>

#include "map.h"
#include "post2nfa.h"

/// @return Whether the string is accepted by the NFA.
/// @details Simulates the NFA.
bool is_accepted(const Nfa*, const char*);

/// @return Whether the string is accepted by the NFA.
/// @note Caches the states to build a DFA on the fly.
bool is_accepted_with_cache(const Nfa* nfa, const char* s);

/// @return The states that are reachable from start with only epsilon
/// transitions, including all of the start states itself.
Map* epsilon_closure(Map* start);

/// @return The states that are reachable from the map of states on label
/// c with non-epsilon moves.
Map* move(Map*, char c);

/// @return The states reachable from the current states on label c with epsilon
/// moves.
Map* get_next_states(Map* current_states, char c);

#endif /* end of include guard: REGEXP_H */
