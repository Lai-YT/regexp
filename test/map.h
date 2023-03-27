#include <setjmp.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "../src/map.h"

// clang-format off
// cmocka allows test applications to use custom definitions of C standard
// library functions and types by having us include the necessary headers.
#include <cmocka.h>
// clang-format on

/// @brief Inserts some key-value pairs. They should be stored into the map.
static void test_map_insert_and_search() {
  Map* map = create_map();
  int keys[] = {9, 217, 100};
  int vals[] = {90, 2170, 1000};

  for (size_t i = 0; i < 3; i++) {
    insert_pair(map, keys[i], vals + i);
  }

  for (size_t i = 0; i < 3; i++) {
    int* val = get_value(map, keys[i]);
    assert_non_null(val);
    assert_ptr_equal(val, vals + i);
  }
  assert_null(get_value(map, 7));

  delete_map(map);
}

/// @brief Deletes a key. Its should be removed without breaking other keys.
void test_map_delete() {
  Map* map = create_map();
  int keys[] = {9, 100, 217};
  int vals[] = {90, 2170, 1000};
  for (size_t i = 0; i < 3; i++) {
    insert_pair(map, keys[i], vals + i);
  }

  delete_pair(map, keys[1]);

  assert_true(get_value(map, keys[0]) && get_value(map, keys[0]) == vals);
  assert_null(get_value(map, keys[1]));
  assert_true(get_value(map, keys[2]) && get_value(map, keys[2]) == vals + 2);

  delete_map(map);
}

/// @brief Inserts enough many key-value pairs. They should all be inserted into
/// the map.
void test_map_capacity_should_grow() {
  enum {  // fix warning on stack-protector: variable length buffer
    NUM_OF_PAIRS = MAP_BASE_CAPACITY * 2
  };
  Map* map = create_map();
  int keys[NUM_OF_PAIRS];
  for (size_t i = 0; i < NUM_OF_PAIRS; i++) {
    keys[i] = i;
  }
  int vals[NUM_OF_PAIRS];  // using the memory address, not initialized

  for (size_t i = 0; i < NUM_OF_PAIRS; i++) {
    insert_pair(map, keys[i], vals + i);
  }

  for (size_t i = 0; i < NUM_OF_PAIRS; i++) {
    int* val = get_value(map, keys[i]);
    assert_non_null(val);
    assert_ptr_equal(vals + i, val);
  }

  delete_map(map);
}

/// @brief This test demonstrates how the iterator works.
static void test_map_iterator() {
  Map* map = create_map();
  int keys[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};  // as same as indices
  int vals[10];  // using the memory address, not initialized
  for (size_t i = 0; i < 10; i++) {
    insert_pair(map, keys[i], vals + i);
  }

  // to check whether we get a single key twice
  bool has_seen[10] = {false};

  MapIterator* itr = create_map_iterator(map);
  int i = 0;
  while (has_next_xx(itr)) {
    to_next(itr);
    const int key = get_current_key(itr);
    assert_false(has_seen[key]);
    has_seen[key] = true;
    i++;
  }
  assert_int_equal(i, 10);

  delete_map_iterator(itr);
  delete_map(map);
}
