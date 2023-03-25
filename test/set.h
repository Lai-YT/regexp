#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>

#include "../src/set.h"

// clang-format off
// cmocka allows test applications to use custom definitions of C standard
// library functions and types by having us include the necessary headers.
#include <cmocka.h>
// clang-format on

/// @brief Inserts some keys. They should be stored into the set.
static void test_insert_and_search() {
  Set* s = create_set();
  int keys[] = {9, 100, 217, 9, 100};

  for (size_t i = 0; i < 3; i++) {
    insert_key(s, keys + i);
  }

  for (size_t i = 0; i < 3; i++) {
    assert_true(has_key(s, keys + i));
  }
  assert_false(has_key(s, keys + 3));
  assert_false(has_key(s, keys + 4));

  delete_set(s);
}

/// @brief Deletes a key. Its should be removed without breaking other keys.
void test_delete() {
  Set* s = create_set();
  int keys[] = {9, 100, 217};
  for (size_t i = 0; i < 3; i++) {
    insert_key(s, keys + i);
  }

  delete_key(s, keys + 1);

  assert_true(has_key(s, keys));
  assert_false(has_key(s, keys + 1));
  assert_true(has_key(s, keys + 2));

  delete_set(s);
}

/// @brief Inserts enough many keys, The capacity of the set should grow.
void test_resize_double_up() {
  Set* s = create_set();
  int keys[100] = {0};
  const size_t base_capacity = s->capacity;

  for (size_t i = 0; i < base_capacity; i++) {
    insert_key(s, keys + i);
  }

  assert_true(s->capacity >= base_capacity * 2);

  delete_set(s);
}
