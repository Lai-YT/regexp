#include "map.h"

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

#include "prime.h"

typedef struct MapPair {
  int key;
  void* val;  // void* is used for generic
} MapPair;

struct Map {
  int capacity;
  int size;
  MapPair** pairs;
};

size_t get_size(Map* map) {
  return map->size;
}

/// @note val may or may not be heap-allocated, its ownership isn't taken.
static MapPair* create_map_pair(int key, void* val) {
  MapPair* item = malloc(sizeof(MapPair));
  item->key = key;
  item->val = val;
  return item;
}

/// @note The val in the map pair is not freed.
static void delete_map_pair(MapPair* item) {
  free(item);
}

static Map* create_map_with_capacity(int capacity) {
  Map* map = malloc(sizeof(Map));
  map->capacity = capacity;
  map->size = 0;
  // initialize to NULL, which means unused
  map->pairs = calloc(map->capacity, sizeof(MapPair));
  return map;
}

Map* create_map() {
  return create_map_with_capacity(next_prime(MAP_BASE_CAPACITY));
}

void delete_map(Map* map) {
  for (int i = 0; i < map->capacity; i++) {
    if (map->pairs[i]) {
      delete_map_pair(map->pairs[i]);
      map->pairs[i] = NULL;
    }
  }
  free(map->pairs);
  free(map);
}

/// @return An integer in [0, prime - 1].
/// @details Division hashing.
static int hash1(int n, int prime) {
  return n % prime;
}

/// @return A integer in [1, prime].
/// @note Does not yield 0.
/// @details Division hashing.
static int hash2(int n, int prime) {
  return prime - n % prime;
}

/// @brief A double hashing function that hashes the key into [0, capacity).
/// @note Hashing is a large topic. This hash function is easy but may not be
/// good.
static int get_hashed_key(int key, int capacity, int attempt) {
  // the capacity we used is already a prime number
  int prime_1 = capacity;
  // a smaller prime in comparison with prime_1
  // and should be relatively prime with the capacity
  int prime_2 = 7;
  return (hash1(key, prime_1) + hash2(key, prime_2) * attempt) % capacity;
}

static MapPair* const PAIR_DELETED_MARKER = NULL;

/// @return Whether the key of the pair is "key".
/// @note Not null-safe.
static bool has_key(MapPair* pair, int key) {
  return pair != PAIR_DELETED_MARKER && pair->key == key;
}

/// @note An internal-use-only helper that returns the pair for modification.
/// @return The pointer to the position of the pair with key. If the key does
/// not exist, returns the position where the key will be if it is inserted.
static MapPair** search_map_pair(Map* map, int key) {
  int attempt = 0;
  int i;
  do {
    i = get_hashed_key(key, map->capacity, attempt++);
  } while (map->pairs[i] && !has_key(map->pairs[i], key));
  return &map->pairs[i];
}

/// @return An integer between 0 and 100.
static int get_load_factor(Map* map) {
  // avoid floating-point arithmetic
  return map->size * 100 / map->capacity;
}

static bool is_high_load(Map* map) {
  return get_load_factor(map) > 70;
}

static bool is_low_load(Map* map) {
  return get_load_factor(map) < 10;
}

/// @note If the capacity is lower than MAP_BASE_CAPACITY, MAP_BASE_CAPACITY is
/// used; if the capacity is not a prime number, the greater closet prime number
/// is used.
static void resize_map(Map* old, int capacity);

/// @details Double up the capacity if the map is under high load after the
/// insertion.
void insert_pair(Map* map, int key, void* val) {
  MapPair** pair_p = search_map_pair(map, key);
  if (*pair_p) {
    delete_map_pair(*pair_p);
    map->size--;
  }
  *pair_p = create_map_pair(key, val);
  map->size++;

  if (is_high_load(map)) {
    resize_map(map, map->capacity * 2);
  }
}

void* get_value(Map* map, int key) {
  MapPair** pair_p = search_map_pair(map, key);
  return *pair_p ? (*pair_p)->val : NULL;
}

/// @details Half down the capacity if the map is under low load after the
/// deletion.
void delete_pair(Map* map, int key) {
  MapPair** pair_p = search_map_pair(map, key);
  if (*pair_p) {
    delete_map_pair(*pair_p);
    *pair_p = PAIR_DELETED_MARKER;
    map->size--;

    if (is_low_load(map)) {
      resize_map(map, map->capacity / 2);
    }
  }
}

static void resize_map(Map* old, int capacity) {
  if (capacity < MAP_BASE_CAPACITY) {
    capacity = MAP_BASE_CAPACITY;
  }
  if (!is_prime(capacity)) {
    capacity = next_prime(capacity);
  }

  // 1. Create a new map with the new capacity
  Map* new = create_map_with_capacity(capacity);

  // 2. Insert the pairs of the old map into the new map
  for (int i = 0; i < old->capacity; i++) {
    MapPair* item = old->pairs[i];
    if (item && item != PAIR_DELETED_MARKER) {
      insert_pair(new, item->key, item->val);
    }
  }

  // 3. Point to the new map
  Map map_to_free = *old;
  *old = *new;

  // 4. Free the old map
  *new = map_to_free;
  // free the pairs from old and the map itself from new
  delete_map(new);
}

struct MapIterator {
  int pos;
  unsigned seen_so_far;
  Map* map;
};

MapIterator* create_map_iterator(Map* map) {
  MapIterator* itr = malloc(sizeof(MapIterator));
  itr->map = map;
  itr->pos = -1;  // if init to 0, to_next may skip the first used pair
  itr->seen_so_far = 0;
  return itr;
}

void delete_map_iterator(MapIterator* itr) {
  free(itr);
}

bool has_next(MapIterator* itr) {
  return itr->seen_so_far != itr->map->size;
}

void to_next(MapIterator* itr) {
  assert(has_next(itr));
  for (itr->pos++; itr->pos < itr->map->capacity; itr->pos++) {
    if (itr->map->pairs[itr->pos]
        && itr->map->pairs[itr->pos] != PAIR_DELETED_MARKER) {
      itr->seen_so_far++;
      return;
    }
  }
}

int get_current_key(MapIterator* itr) {
  assert(itr->pos != -1);
  return itr->map->pairs[itr->pos]->key;
}

void* get_current_value(MapIterator* itr) {
  assert(itr->pos != -1);
  return itr->map->pairs[itr->pos]->val;
}
