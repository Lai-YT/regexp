#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>

#include "map.h"
#include "nfa.h"
#include "post2nfa.h"
#include "re2post.h"
#include "regexp.h"
#include "state.h"

// clang-format off
// cmocka allows test applications to use custom definitions of C standard
// library functions and types by having us include the necessary headers.
#include <cmocka.h>
// clang-format on

int main(void) {
  const struct CMUnitTest tests[] = {
      // re2post.h
      cmocka_unit_test(test_re2post_single_character),
      cmocka_unit_test(test_re2post_concat),
      cmocka_unit_test(test_re2post_union),
      cmocka_unit_test(test_re2post_paren),
      cmocka_unit_test(test_re2post_zero_or_more),
      cmocka_unit_test(test_re2post_one_or_more),
      cmocka_unit_test(test_re2post_zero_or_one),
      cmocka_unit_test(test_re2post_any),
      cmocka_unit_test(test_re2post_union_and_concat),
      cmocka_unit_test(test_re2post_concat_with_paren),
      cmocka_unit_test(test_re2post_union_with_paren),
      cmocka_unit_test(test_re2post_mix),
      cmocka_unit_test(test_re2post_empty_re_should_be_empty_post),
      cmocka_unit_test(test_re2post_missing_operand_should_return_null),
      cmocka_unit_test(test_re2post_mismatch_paren_should_return_null),
      // state.h
      cmocka_unit_test(test_create_labeled_state),
      cmocka_unit_test(test_create_epsilon_state),
      cmocka_unit_test(test_create_accepting_state),
      cmocka_unit_test(test_create_any_state),
      // nfa.h
      cmocka_unit_test(test_create_nfa),
      // post2nfa.h
      cmocka_unit_test(test_post2nfa_single_character),
      cmocka_unit_test(test_post2nfa_concat_only),
      cmocka_unit_test(test_post2nfa_union_only_single),
      cmocka_unit_test(test_post2nfa_union_only_complex),
      cmocka_unit_test(test_post2nfa_zero_or_more),
      cmocka_unit_test(test_post2nfa_zero_or_one),
      cmocka_unit_test(test_post2nfa_one_or_more),
      cmocka_unit_test(test_post2nfa_any_concat),
      cmocka_unit_test(test_post2nfa_mix),
      cmocka_unit_test(test_post2nfa_concat_one_or_more),
      cmocka_unit_test(test_post2nfa_concat_zero_or_more),
      cmocka_unit_test(test_post2nfa_concat_zero_or_one),
      cmocka_unit_test(test_post2nfa_missing_operator_should_return_null),
      cmocka_unit_test(test_post2nfa_missing_operand_should_return_null),
      cmocka_unit_test(test_post2nfa_empty_post_should_return_null),
      // regexp.h
      cmocka_unit_test(test_epsilon_closure_on_epsilon),
      cmocka_unit_test(test_epsilon_closure_on_split),
      cmocka_unit_test(test_epsilon_closure_on_chain),
      cmocka_unit_test(test_epsilon_closure_duplicate),
      cmocka_unit_test(test_move_should_be_one_hop_only),
      cmocka_unit_test(test_move_null),
      cmocka_unit_test(test_is_accepted),
      cmocka_unit_test(test_is_accepted_with_cache),
      cmocka_unit_test(test_regexp_paren_and_zero_or_more),
      cmocka_unit_test(test_regexp_paren_and_zero_or_more_with_cache),
      cmocka_unit_test(test_regexp_any_and_one_or_more),
      cmocka_unit_test(test_regexp_any_and_one_or_more_with_cache),
      // map.h
      cmocka_unit_test(test_map_insert_and_search),
      cmocka_unit_test(test_map_delete),
      cmocka_unit_test(test_map_capacity_should_grow),
      cmocka_unit_test(test_map_iterator),
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}
