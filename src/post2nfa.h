#ifndef POST2NFA_H
#define POST2NFA_H

#include "nfa.h"
#include "re2post.h"
#include "state.h"

Nfa* post2nfa(const char* post);

#endif /* end of include guard: POST2NFA_H */
