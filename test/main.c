#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>

#include "post2nfa.h"
#include "re2post.h"
#include "regexp.h"
#include "set.h"

// clang-format off
// cmocka allows test applications to use custom definitions of C standard
// library functions and types by having us include the necessary headers.
#include <cmocka.h>
// clang-format on

int main(void) {
  const struct CMUnitTest tests[] = {
      // re2post.h
      cmocka_unit_test(test_re2post_concat),
      cmocka_unit_test(test_re2post_union),
      cmocka_unit_test(test_re2post_paren),
      cmocka_unit_test(test_re2post_union_and_concat),
      cmocka_unit_test(test_re2post_concat_with_paren),
      cmocka_unit_test(test_re2post_union_with_paren),
      cmocka_unit_test(test_re2post_mix),
      // post2nfa.h
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
      cmocka_unit_test(test_missing_operator_should_return_null),
      cmocka_unit_test(test_missing_operand_should_return_null),
      // regexp.h
      cmocka_unit_test(test_epsilon_closure_on_epsilon),
      cmocka_unit_test(test_epsilon_closure_on_split),
      cmocka_unit_test(test_epsilon_closure_on_chain),
      cmocka_unit_test(test_epsilon_closure_duplicate),
      cmocka_unit_test(test_move_should_be_one_hop_only),
      cmocka_unit_test(test_move_null),
      cmocka_unit_test(test_accepted),
      cmocka_unit_test(test_all_regexp),
      // set.h
      cmocka_unit_test(test_insert_and_search),
      cmocka_unit_test(test_delete),
      cmocka_unit_test(test_resize_double_up),

  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}
