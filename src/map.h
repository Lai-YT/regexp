#ifndef MAP_H
#define MAP_H

#include <stdbool.h>
#include <stddef.h>

#ifndef MAP_BASE_CAPACITY
/// @brief Define before including this file if you want to use another base
/// capacity. The actual capacity will be the greater closest prime.
#define MAP_BASE_CAPACITY 53
#endif

typedef struct Map Map;

size_t get_size(Map*);

/// @note Should be freed after use with delete_map.
Map* create_map();

/// @note Frees the map created previously with create_map.
void delete_map(Map*);

/// @brief Inserts the key-val pair into the map. val is updated if key
/// already exists.
/// @note The val may or may not be heap-allocated since the map does
/// not take the ownership. If val is heap allocated, the caller has to free it
/// manually.
void insert_pair(Map* ht, int key, void* val);

/// @return The val mapped by key; NULL if not exists.
void* get_value(Map*, int key);

/// @brief Deletes the key and its val from map if the key exists.
/// @note The val is not freed since its ownership isn't taken.
void delete_pair(Map*, int key);

typedef struct MapIterator MapIterator;

/// @note The iterator becomes invalid once an operation is made during
/// iteration.
MapIterator* create_map_iterator(Map*);
void delete_map_iterator(MapIterator*);

/// @exception Assertion error if no preceding call on to_next.
int get_current_key(MapIterator*);

/// @exception Assertion error if no preceding call on to_next.
void* get_current_value(MapIterator*);

bool has_next(MapIterator*);

/// @exception Assertion error if there's no more to_next.
void to_next(MapIterator*);

#endif /* end of include guard: MAP_H */