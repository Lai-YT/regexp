#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>

#include "../src/post2nfa.h"
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

static void test_create_nfa() {
  State* accept = create_state(ACCEPT, NULL);
  State* start = create_state('a', &accept);

  Nfa* nfa = create_nfa(start, accept);

  assert_ptr_equal(nfa->start, start);
  assert_ptr_equal(nfa->accept, accept);

  delete_nfa(nfa);
}

static void test_post2nfa_concat_only() {
  const char* post = "ab.c.";

  Nfa* nfa = post2nfa(post);

  assert_non_null(nfa);
  ASSERT_NON_SPLIT_TRANSITION_LABELS(nfa->start, 'a', 'b', 'c', ACCEPT);

  delete_nfa(nfa);
}

// Conventions used when testing NFAs:
// - The transitions of a SPLIT state is tested in a new block
// - A transition should be in a new block if it requires more than one
// statement to test

static void test_post2nfa_union_only_single() {
  const char* post = "ab|";

  Nfa* nfa = post2nfa(post);

  assert_non_null(nfa);
  ASSERT_LABEL(nfa->start, SPLIT);
  {
    ASSERT_NON_SPLIT_TRANSITION_LABELS(nfa->start->outs[0], 'a', EPSILON,
                                       ACCEPT);
    ASSERT_NON_SPLIT_TRANSITION_LABELS(nfa->start->outs[1], 'b', EPSILON,
                                       ACCEPT);
  }

  delete_nfa(nfa);
}

static void test_post2nfa_union_only_complex() {
  const char* post = "abc||";

  Nfa* nfa = post2nfa(post);

  assert_non_null(nfa);
  ASSERT_LABEL(nfa->start, SPLIT);
  {
    ASSERT_NON_SPLIT_TRANSITION_LABELS(nfa->start->outs[0], 'a', EPSILON,
                                       ACCEPT);
    {
      State* s = nfa->start->outs[1];
      ASSERT_LABEL(s, SPLIT);
      {
        ASSERT_NON_SPLIT_TRANSITION_LABELS(s->outs[0], 'b', EPSILON, EPSILON,
                                           ACCEPT);
        ASSERT_NON_SPLIT_TRANSITION_LABELS(s->outs[1], 'c', EPSILON, EPSILON,
                                           ACCEPT);
      }
    }
  }

  delete_nfa(nfa);
}

static void test_post2nfa_zero_or_more() {
  const char* post = "a*";

  Nfa* nfa = post2nfa(post);

  assert_non_null(nfa);
  ASSERT_LABEL(nfa->start, SPLIT);
  {
    {
      State* start_of_a = nfa->start->outs[0];
      ASSERT_LABEL(start_of_a, 'a');
      State* out_of_a = start_of_a->outs[0];
      ASSERT_LABEL(out_of_a, SPLIT);
      {
        assert_ptr_equal(out_of_a->outs[0], start_of_a);  // should come back
        ASSERT_LABEL(out_of_a->outs[1], ACCEPT);
      }
    }
    ASSERT_LABEL(nfa->start->outs[1], ACCEPT);
  }

  delete_nfa(nfa);
}

static void test_post2nfa_zero_or_one() {
  const char* post = "a?";

  Nfa* nfa = post2nfa(post);

  assert_non_null(nfa);
  ASSERT_LABEL(nfa->start, SPLIT);
  {
    ASSERT_NON_SPLIT_TRANSITION_LABELS(nfa->start->outs[0], 'a', EPSILON,
                                       ACCEPT);
    ASSERT_LABEL(nfa->start->outs[1], ACCEPT);
  }

  delete_nfa(nfa);
}

static void test_post2nfa_one_or_more() {
  const char* post = "a+";

  Nfa* nfa = post2nfa(post);

  assert_non_null(nfa);
  State* start_of_a = nfa->start;
  ASSERT_LABEL(start_of_a, 'a');
  State* out_of_a = start_of_a->outs[0];
  ASSERT_LABEL(out_of_a, SPLIT);
  {
    assert_ptr_equal(out_of_a->outs[0], start_of_a);  // should come back
    ASSERT_LABEL(out_of_a->outs[1], ACCEPT);
  }

  delete_nfa(nfa);
}

static void test_post2nfa_mix() {
  const char* post = "ab*c|.";  // a(b*|c)

  Nfa* nfa = post2nfa(post);

  assert_non_null(nfa);
  State* s = nfa->start;
  ASSERT_LABEL(s, 'a');
  s = s->outs[0];
  ASSERT_LABEL(s, SPLIT);
  {
    {
      State* t = s->outs[0];
      ASSERT_LABEL(t, SPLIT);
      {
        {
          State* start_of_b = t->outs[0];
          ASSERT_LABEL(start_of_b, 'b');
          State* out_of_b = start_of_b->outs[0];
          ASSERT_LABEL(out_of_b, SPLIT);
          {
            assert_ptr_equal(out_of_b->outs[0],
                             start_of_b);  // should come back
            ASSERT_NON_SPLIT_TRANSITION_LABELS(out_of_b->outs[1], EPSILON,
                                               ACCEPT);
          }
        }
        ASSERT_NON_SPLIT_TRANSITION_LABELS(t->outs[1], EPSILON, ACCEPT);
      }
    }
    ASSERT_NON_SPLIT_TRANSITION_LABELS(s->outs[1], 'c', EPSILON, ACCEPT);
  }

  delete_nfa(nfa);
}

static void test_missing_operator_should_return_null() {
  assert_null(post2nfa("ab"));
  assert_null(post2nfa("abc"));
}

static void test_missing_operand_should_return_null() {
  assert_null(post2nfa("a."));
  assert_null(post2nfa("*"));
}
