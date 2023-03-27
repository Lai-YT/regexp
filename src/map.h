#ifndef MAP_H
#define MAP_H

#include <stddef.h>

#ifndef MAP_BASE_CAPACITY
/// @brief Define before including this file if you want to use another base
/// capacity. The actual capacity will be the greater closest prime.
#define MAP_BASE_CAPACITY 53
#endif

typedef struct Map Map;

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

#endif /* end of include guard: MAP_H */
