#include "set.h"

#include <math.h>    // floor, sqrt
#include <stddef.h>  //size_t
#include <stdint.h>  // intptr_t
#include <stdlib.h>

static const size_t BASE_CAPACITY = 53;

static Set* create_set_with_capacity(size_t capacity) {
  Set* s = malloc(sizeof(Set));
  s->capacity = capacity;
  s->size = 0;
  // initialize to NULL, which means unused
  s->keys = calloc(s->capacity, sizeof *s->keys);
  return s;
}

Set* create_set() {
  return create_set_with_capacity(BASE_CAPACITY);
}

void delete_set(Set* s) {
  free(s->keys);
  free(s);
}

static bool is_prime(int n);

/// @return The first prime equal or greater than n.
static int next_prime(int n);

/// @return An integer in [0, prime - 1].
/// @details Division hashing.
static int hash1(void* key, int prime) {
  return (intptr_t)key % prime;
}

/// @return A integer in [1, prime].
/// @note Does not yield 0.
/// @details Division hashing.
static int hash2(void* key, int prime) {
  return prime - (intptr_t)key % prime;
}

/// @brief A double hashing function that hashes the key into [0, ht_capacity).
/// @note Hashing is a large topic. This hash function is simple but may not be
/// good.
static int get_hashed_key(void* key, size_t capacity, int attempt) {
  // the capacity we used is already a prime number
  int prime1 = capacity;
  // a smaller prime in comparison with prime1
  // and should be relatively prime with the capacity
  int prime2 = 7;
  return (hash1(key, prime1) + hash2(key, prime2) * attempt) % capacity;
}
/// @note Type isn't important. We'll depend on the memory location.
static int* DELETE_MARKER = NULL;

/// @brief Since the key may be the DELETE_MARKER, use this helper to check the
/// key correctly.
/// @return Whether the key of the item is "key".
/// @note Not null-safe.
static bool is_key(void* key, void* expected_key) {
  return key != DELETE_MARKER && key == expected_key;
}

/// @return The the position of the key. If the key does not exist, returns the
/// position where the key will be if it is inserted.
static int search_key_pos(Set* s, void* key) {
  int attempt = 0;
  int pos;
  do {
    pos = get_hashed_key(key, s->capacity, attempt++);
  } while (s->keys[pos] && !is_key(s->keys[pos], key));
  return pos;
}

/// @return An integer between 0 and 100.
static int get_load_factor(Set* s) {
  // avoid floating-point arithmetic
  return s->size * 100 / s->capacity;
}

static bool is_high_load(Set* s) {
  return get_load_factor(s) > 70;
}

static bool is_low_load(Set* s) {
  return get_load_factor(s) < 10;
}

/// @note If the capacity is lower than the BASE_CAPACITY, BASE_CAPACITY is
/// used; if the capacity is not a prime number, the greater closet prime number
/// is used.
static void resize_set(Set*, size_t capacity);

/// @details Double up the capacity if the set is under high load after the
/// insertion.
void insert_key(Set* s, void* key) {
  int pos = search_key_pos(s, key);
  if (is_key(s, key)) {
    s->size--;
  }
  s->keys[pos] = key;
  s->size++;

  if (is_high_load(s)) {
    resize_set(s, s->capacity * 2);
  }
}

bool has_key(Set* s, void* key) {
  int pos = search_key_pos(s, key);
  return is_key(s->keys[pos], key);
}

void delete_key(Set* s, void* key) {
  int pos = search_key_pos(s, key);
  if (is_key(s->keys[pos], key)) {
    s->keys[pos] = DELETE_MARKER;
    s->size--;

    if (is_low_load(s)) {
      resize_set(s, s->capacity / 2);
    }
  }
}

static void resize_set(Set* old, size_t new_capacity) {
  if (new_capacity < BASE_CAPACITY) {
    new_capacity = BASE_CAPACITY;
  }
  if (!is_prime(new_capacity)) {
    new_capacity = next_prime(new_capacity);
  }
  // 1. Create a new set with the new capacity
  Set* new = create_set_with_capacity(new_capacity);
  // 2. Insert the keys of the old set into the new set
  for (size_t i = 0; i < old->capacity; i++) {
    if (old->keys[i] && old->keys[i] != DELETE_MARKER) {
      insert_key(new, old->keys[i]);
    }
  }
  // 3. Point to the new set
  Set s_to_delete = *old;
  *old = *new;
  // 4. Delete the old set
  *new = s_to_delete;
  delete_set(new);
}

static bool is_prime(int n) {
  if (n < 2) {
    return false;
  }
  if (n < 4) {
    return true;
  }

  // fast pass the even number easily, and check odd numbers only
  if (n % 2 == 0) {
    return false;
  }
  const int square_root = floor(sqrt(n));
  for (int i = 3; i <= square_root; i += 2) {
    if (n % i == 0) {
      return false;
    }
  }
  return true;
}

static int next_prime(int n) {
  if (n < 0) {
    return 2;
  }
  while (!is_prime(n)) {
    n++;
  }
  return n;
}
