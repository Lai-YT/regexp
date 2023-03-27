#ifndef STATE_H
#define STATE_H

enum {
  EPSILON = 128,
  SPLIT = 129,
  ACCEPT = 130,
};

typedef struct State {
  int label;
  struct State** outs;
  /// @brief The unique id field is for NFAs to better handle their interior
  /// states.
  int id;
} State;

/// @brief A labeled state have 1 labeled transition, an epsilon state (label ==
/// EPSILON) has 1 epsilon transition, a spliting state (label == SPLIT) has 2
/// epsilon transitions, an accepting state (label == ACCEPT) has 1 reserved
/// transition.
int num_of_outs(int label);

/// @brief An epsilon state has 1, a spliting state has 2, others have 0.
int num_of_epsilon_outs(int label);

/// @note The accepting state may later become part of an NFA and turns into an
/// epsilon state. outs is ignored under this condition since the space is only
/// reserved.
State* create_state(const int label, State** outs);

/// @brief Deletes the state but not the states it transits to.
void delete_state(State*);

#endif /* end of include guard: STATE_H */
