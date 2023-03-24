#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>

#include "../src/post2nfa.h"

// clang-format off
// cmocka allows test applications to use custom definitions of C standard
// library functions and types by having us include the necessary headers.
#include <cmocka.h>
// clang-format on

#ifndef ASSERT_LABEL
#define ASSERT_LABEL(state, expected_label) \
  assert_int_equal((state)->label, (expected_label));
#endif

#ifndef ASSERT_NON_SPLIT_TRANSITION_LABELS
#define ASSERT_NON_SPLIT_TRANSITION_LABELS(start, ...)           \
  {                                                              \
    size_t n = sizeof((int[]){__VA_ARGS__}) / sizeof(int);       \
    assert_non_split_transition_labels((start), n, __VA_ARGS__); \
  }

void assert_non_split_transition_labels(State* start, size_t n_labels, ...) {
  va_list args;
  va_start(args, n_labels);
  for (size_t i = 0; i < n_labels; i++) {
    ASSERT_LABEL(start, va_arg(args, int));
    // non split, only check 0
    start = start->outs[0];
  }
}
#endif
