#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>

#include "../src/state.h"
#include "assertion.h"

// clang-format off
// cmocka allows test applications to use custom definitions of C standard
// library functions and types by having us include the necessary headers.
#include <cmocka.h>
// clang-format on

static void test_create_labeled_state() {
  const int label = 100;
  State out;  // ill-formed but doesn't matter
  State* outs[] = {&out};

  State* labeled_state = create_state(label, outs);

  ASSERT_LABEL(labeled_state, label);
  assert_ptr_equal(labeled_state->outs[0], outs[0]);

  delete_state(labeled_state);
}

static void test_create_epsilon_state() {
  State out1, out2;  // ill-formed but doesn't matter
  State* outs[] = {&out1, &out2};

  State* epsilon_state = create_state(SPLIT, outs);

  ASSERT_LABEL(epsilon_state, SPLIT);
  assert_ptr_equal(epsilon_state->outs[0], &out1);
  assert_ptr_equal(epsilon_state->outs[1], &out2);

  delete_state(epsilon_state);
}

static void test_create_accepting_state() {
  State* accepting_state = create_state(ACCEPT, NULL);

  ASSERT_LABEL(accepting_state, ACCEPT);
  assert_null(accepting_state->outs[0]);

  delete_state(accepting_state);
}

static void test_create_any_state() {
  State out;  // ill-formed but doesn't matter
  State* outs[] = {&out};

  State* any_state = create_state(ANY, outs);

  ASSERT_LABEL(any_state, ANY);
  assert_ptr_equal(any_state->outs[0], outs[0]);

  delete_state(any_state);
}
