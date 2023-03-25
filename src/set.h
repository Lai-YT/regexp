#ifndef SET_H
#define SET_H

#include <stdbool.h>
#include <stddef.h>  // size_t

/// @brief This is a specialized set which directly hashes the pointer as keys.
typedef struct Set {
  size_t capacity;
  size_t size;
  void** keys;
} Set;

/// @return An empty set.
Set* create_set();
void delete_set(Set*);

void insert_key(Set*, void* key);
bool has_key(Set*, void* key);
void delete_key(Set*, void* key);

#endif /* end of include guard: SET_H */
