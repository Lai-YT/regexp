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

typedef struct SetIterator SetIterator;

SetIterator* create_iterator(Set*);
void delete_iterator(SetIterator*);

/// @exception Assertion error if no preceding call on next.
void* get_key(SetIterator*);

bool has_next(SetIterator*);

/// @exception Assertion error if there's no more next.
void next(SetIterator*);

#endif /* end of include guard: SET_H */
