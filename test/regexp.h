#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>

#include "../src/list.h"
#include "../src/regexp.h"

// clang-format off
// cmocka allows test applications to use custom definitions of C standard
// library functions and types by having us include the necessary headers.
#include <cmocka.h>
// clang-format on

static void test_epsilon_closure_on_epsilon() {
  State* s = create_state(ACCEPT, NULL);
  State* start = create_state(EPSILON, &s);
  List* l = create_list(start);

  List* closure = epsilon_closure(l);

  assert_ptr_equal(closure->val, start);
  assert_ptr_equal(closure->next->val, s);

  delete_list(closure);
  delete_list(l);
  delete_state(start);
  delete_state(s);
}

static void test_epsilon_closure_on_split() {
  State* s1 = create_state(ACCEPT, NULL);
  State* s2 = create_state(ACCEPT, NULL);
  State* outs[2] = {s1, s2};
  State* start = create_state(SPLIT, outs);
  List* l = create_list(start);

  List* closure = epsilon_closure(l);

  assert_ptr_equal(closure->val, start);
  assert_ptr_equal(closure->next->val, s1);
  assert_ptr_equal(closure->next->next->val, s2);

  delete_list(closure);
  delete_list(l);
  delete_state(start);
  delete_state(s2);
  delete_state(s1);
}

static void test_epsilon_closure_on_chain() {
  State* s2 = create_state(ACCEPT, NULL);
  State* s1 = create_state(EPSILON, &s2);
  State* start = create_state(EPSILON, &s1);
  List* l = create_list(start);

  List* closure = epsilon_closure(l);

  assert_ptr_equal(closure->val, start);
  assert_ptr_equal(closure->next->val, s1);
  assert_ptr_equal(closure->next->next->val, s2);

  delete_list(closure);
  delete_list(l);
  delete_state(start);
  delete_state(s2);
  delete_state(s1);
}

static void test_epsilon_closure_duplicate() {
  State* s2 = create_state(ACCEPT, NULL);
  State* s1 = create_state(EPSILON, &s2);
  State* start = create_state(EPSILON, &s1);
  // introduce an epsilon loop
  s2->label = EPSILON;
  s2->outs[0] = start;
  List* l = create_list(start);

  List* closure = epsilon_closure(l);

  assert_ptr_equal(closure->val, start);
  assert_ptr_equal(closure->next->val, s1);
  assert_ptr_equal(closure->next->next->val, s2);

  delete_list(closure);
  delete_list(l);
  delete_state(start);
  delete_state(s2);
  delete_state(s1);
}

static void test_move_should_be_one_hop_only() {
  // a -> a -> accept
  State* accept = create_state(ACCEPT, NULL);
  State* a2 = create_state('a', &accept);
  State* a1 = create_state('a', &a2);
  List* l = create_list(a1);

  List* moves = move(l, 'a');
  assert_ptr_equal(moves->val, a2);

  delete_list(moves);
  delete_list(l);
  delete_state(a2);
  delete_state(a1);
  delete_state(accept);
}

static void test_move_null() {
  // a -> b -> accept
  State* accept = create_state(ACCEPT, NULL);
  State* b = create_state('b', &accept);
  State* a = create_state('a', &b);
  List* l = create_list(a);

  List* moves = move(l, 'c');
  assert_null(moves);

  delete_list(moves);
  delete_list(l);
  delete_state(a);
  delete_state(b);
  delete_state(accept);
}

static void test_accepted() {
  // a -> b -> accept
  State* accept = create_state(ACCEPT, NULL);
  State* b = create_state('b', &accept);
  State* a = create_state('a', &b);
  Nfa* nfa = create_nfa(a, accept);

  assert_true(accepted(nfa, "ab"));

  delete_nfa(nfa);
}

static void test_all_regexp() {
  const char* re = "(a|b)*abb";  // consists only a/b and ends with abb
  const char* post = re2post(re);
  Nfa* nfa = post2nfa(post);

  assert_true(accepted(nfa, "abb"));
  assert_true(accepted(nfa, "babb"));
  assert_true(accepted(nfa, "bbbbabb"));
  assert_true(accepted(nfa, "abaabbaabb"));
  assert_false(accepted(nfa, "abaabbbb"));
  assert_false(accepted(nfa, "abaabbab"));

  delete_nfa(nfa);
}
