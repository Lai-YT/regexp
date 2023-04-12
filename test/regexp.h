#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>

#include "../src/map.h"
#include "../src/nfa.h"
#include "../src/post2nfa.h"
#include "../src/re2post.h"
#include "../src/regexp.h"
#include "../src/state.h"

// clang-format off
// cmocka allows test applications to use custom definitions of C standard
// library functions and types by having us include the necessary headers.
#include <cmocka.h>
// clang-format on

static void test_epsilon_closure_on_epsilon() {
  State* accept = create_state(ACCEPT, NULL);
  State* start = create_state(EPSILON, &accept);
  Map* from = create_map();
  insert_pair(from, start->id, start);

  Map* closure = epsilon_closure(from);

  assert_int_equal(get_size(closure), 2);
  assert_true(get_value(closure, start->id));
  assert_true(get_value(closure, accept->id));

  delete_map(closure);
  delete_map(from);
  delete_state(start);
  delete_state(accept);
}

static void test_epsilon_closure_on_split() {
  State* accept1 = create_state(ACCEPT, NULL);
  State* accept2 = create_state(ACCEPT, NULL);
  State* outs[2] = {accept1, accept2};
  State* start = create_state(SPLIT, outs);
  Map* from = create_map();
  insert_pair(from, start->id, start);

  Map* closure = epsilon_closure(from);

  assert_int_equal(get_size(closure), 3);
  assert_true(get_value(closure, start->id));
  assert_true(get_value(closure, accept1->id));
  assert_true(get_value(closure, accept2->id));

  delete_map(closure);
  delete_map(from);
  delete_state(start);
  delete_state(accept2);
  delete_state(accept1);
}

static void test_epsilon_closure_on_chain() {
  State* accept = create_state(ACCEPT, NULL);
  State* s = create_state(EPSILON, &accept);
  State* start = create_state(EPSILON, &s);
  Map* from = create_map();
  insert_pair(from, start->id, start);

  Map* closure = epsilon_closure(from);

  assert_int_equal(get_size(closure), 3);
  assert_true(get_value(closure, start->id));
  assert_true(get_value(closure, s->id));
  assert_true(get_value(closure, accept->id));

  delete_map(closure);
  delete_map(from);
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
  Map* from = create_map();
  insert_pair(from, start->id, start);

  Map* closure = epsilon_closure(from);

  assert_int_equal(get_size(closure), 3);
  assert_true(get_value(closure, start->id));
  assert_true(get_value(closure, s->id));
  assert_true(get_value(closure, accept->id));

  delete_map(closure);
  delete_map(from);
  delete_state(start);
  delete_state(accept);
  delete_state(s);
}

static void test_move_should_be_one_hop_only() {
  // a -> a -> accept
  State* accept = create_state(ACCEPT, NULL);
  State* a2 = create_state('a', &accept);
  State* a1 = create_state('a', &a2);
  Map* from = create_map();
  insert_pair(from, a1->id, a1);

  Map* moves = move(from, 'a');

  assert_int_equal(get_size(moves), 1);
  assert_true(get_value(moves, a2->id));

  delete_map(moves);
  delete_map(from);
  delete_state(a2);
  delete_state(a1);
  delete_state(accept);
}

static void test_move_null() {
  // a -> b -> accept
  State* accept = create_state(ACCEPT, NULL);
  State* b = create_state('b', &accept);
  State* a = create_state('a', &b);
  Map* from = create_map();
  insert_pair(from, a->id, a);

  Map* moves = move(from, 'c');

  assert_int_equal(get_size(moves), 0);

  delete_map(moves);
  delete_map(from);
  delete_state(a);
  delete_state(b);
  delete_state(accept);
}

static void test_is_accepted() {
  // a -> b -> accept
  State* accept = create_state(ACCEPT, NULL);
  State* b = create_state('b', &accept);
  State* a = create_state('a', &b);
  Nfa* nfa = create_nfa(a, accept);

  assert_true(is_accepted(nfa, "ab"));

  delete_nfa(nfa);
}

static void test_is_accepted_with_cache() {
  // a -> b -> accept
  State* accept = create_state(ACCEPT, NULL);
  State* b = create_state('b', &accept);
  State* a = create_state('a', &b);
  Nfa* nfa = create_nfa(a, accept);

  assert_true(is_accepted_with_cache(nfa, "ab"));

  delete_nfa(nfa);
}

static void test_regexp_paren_and_zero_or_more() {
  const char* re = "(a|b)*abb";  // consists only a/b and ends with abb

  const char* post = re2post(re);
  Nfa* nfa = post2nfa(post);

  assert_true(is_accepted(nfa, "abb"));
  assert_true(is_accepted(nfa, "babb"));
  assert_true(is_accepted(nfa, "bbbbabb"));
  assert_true(is_accepted(nfa, "abaabbaabb"));
  assert_false(is_accepted(nfa, "abaabbbb"));
  assert_false(is_accepted(nfa, "abaabbab"));

  delete_nfa(nfa);
}

static void test_regexp_paren_and_zero_or_more_with_cache() {
  const char* re = "(a|b)*abb";  // consists only a/b and ends with abb

  const char* post = re2post(re);
  Nfa* nfa = post2nfa(post);

  assert_true(is_accepted_with_cache(nfa, "abb"));
  assert_true(is_accepted_with_cache(nfa, "babb"));
  assert_true(is_accepted_with_cache(nfa, "bbbbabb"));
  assert_true(is_accepted_with_cache(nfa, "abaabbaabb"));
  assert_false(is_accepted_with_cache(nfa, "abaabbbb"));
  assert_false(is_accepted_with_cache(nfa, "abaabbab"));

  delete_nfa(nfa);
}
