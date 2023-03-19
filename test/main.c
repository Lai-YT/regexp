#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>

#include "re2post.h"
#include "regexp.h"

// clang-format off
// cmocka allows test applications to use custom definitions of C standard
// library functions and types by having us include the necessary headers.
#include <cmocka.h>
// clang-format on

int main(void) {
  const struct CMUnitTest tests[] = {
      cmocka_unit_test(test_re2post_concat),
      cmocka_unit_test(test_re2post_union),
      cmocka_unit_test(test_re2post_paren),
      cmocka_unit_test(test_re2post_union_and_concat),
      cmocka_unit_test(test_re2post_concat_with_paren),
      cmocka_unit_test(test_re2post_union_with_paren),
      cmocka_unit_test(test_re2post_mix),
      cmocka_unit_test(test_create_labeled_state),
      cmocka_unit_test(test_create_epsilon_state),
      cmocka_unit_test(test_create_accepting_state),
      cmocka_unit_test(test_create_nfa),
      cmocka_unit_test(test_post2nfa_concat_only),
      cmocka_unit_test(test_post2nfa_union_only_single),
      cmocka_unit_test(test_post2nfa_union_only_complex),
      cmocka_unit_test(test_post2nfa_zero_or_more),
      cmocka_unit_test(test_post2nfa_zero_or_one),
      cmocka_unit_test(test_post2nfa_one_or_more),
      cmocka_unit_test(test_post2nfa_mix),
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}
