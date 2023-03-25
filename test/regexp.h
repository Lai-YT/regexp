#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>

#include "../src/regexp.h"
#include "../src/set.h"

// clang-format off
// cmocka allows test applications to use custom definitions of C standard
// library functions and types by having us include the necessary headers.
#include <cmocka.h>
// clang-format on

static void test_epsilon_closure_on_epsilon() {
  State* accept = create_state(ACCEPT, NULL);
  State* start = create_state(EPSILON, &accept);
  Set* from = create_set();
  insert_key(from, start);

  Set* closure = epsilon_closure(from);

  assert_int_equal(closure->size, 2);
  assert_true(has_key(closure, start));
  assert_true(has_key(closure, accept));

  delete_set(closure);
  delete_set(from);
  delete_state(start);
  delete_state(accept);
}

static void test_epsilon_closure_on_split() {
  State* accept1 = create_state(ACCEPT, NULL);
  State* accept2 = create_state(ACCEPT, NULL);
  State* outs[2] = {accept1, accept2};
  State* start = create_state(SPLIT, outs);
  Set* from = create_set();
  insert_key(from, start);

  Set* closure = epsilon_closure(from);

  assert_int_equal(closure->size, 3);
  assert_true(has_key(closure, start));
  assert_true(has_key(closure, accept1));
  assert_true(has_key(closure, accept2));

  delete_set(closure);
  delete_set(from);
  delete_state(start);
  delete_state(accept2);
  delete_state(accept1);
}

static void test_epsilon_closure_on_chain() {
  State* accept = create_state(ACCEPT, NULL);
  State* s = create_state(EPSILON, &accept);
  State* start = create_state(EPSILON, &s);
  Set* from = create_set();
  insert_key(from, start);

  Set* closure = epsilon_closure(from);

  assert_int_equal(closure->size, 3);
  assert_true(has_key(closure, start));
  assert_true(has_key(closure, s));
  assert_true(has_key(closure, accept));

  delete_set(closure);
  delete_set(from);
  delete_state(start);
  delete_state(accept);
  delete_state(s);
}

static void test_epsilon_closure_duplicate() {
  State* accept = create_state(ACCEPT, NULL);
  State* s = create_state(EPSILON, &accept);
  State* start = create_state(EPSILON, &s);
  // introduce an epsilon loop
  accept->label = EPSILON;
  accept->outs[0] = start;
  Set* from = create_set();
  insert_key(from, start);

  Set* closure = epsilon_closure(from);

  assert_int_equal(closure->size, 3);
  assert_true(has_key(closure, start));
  assert_true(has_key(closure, s));
  assert_true(has_key(closure, accept));

  delete_set(closure);
  delete_set(from);
  delete_state(start);
  delete_state(accept);
  delete_state(s);
}

static void test_move_should_be_one_hop_only() {
  // a -> a -> accept
  State* accept = create_state(ACCEPT, NULL);
  State* a2 = create_state('a', &accept);
  State* a1 = create_state('a', &a2);
  Set* from = create_set();
  insert_key(from, a1);

  Set* moves = move(from, 'a');

  assert_int_equal(moves->size, 1);
  assert_true(has_key(moves, a2));

  delete_set(moves);
  delete_set(from);
  delete_state(a2);
  delete_state(a1);
  delete_state(accept);
}

static void test_move_null() {
  // a -> b -> accept
  State* accept = create_state(ACCEPT, NULL);
  State* b = create_state('b', &accept);
  State* a = create_state('a', &b);
  Set* from = create_set();
  insert_key(from, a);

  Set* moves = move(from, 'c');

  assert_int_equal(moves->size, 0);

  delete_set(moves);
  delete_set(from);
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
