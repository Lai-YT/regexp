#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>

#include "../src/nfa.h"
#include "../src/post2nfa.h"
#include "../src/state.h"
#include "assertion.h"

// clang-format off
// cmocka allows test applications to use custom definitions of C standard
// library functions and types by having us include the necessary headers.
#include <cmocka.h>
// clang-format on

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
  ASSERT_LABEL(nfa->start, EPSILON);
  State* start_of_a = nfa->start->outs[0];
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

static void test_post2nfa_concat_one_or_more() {
  const char* post = "a+b+.";

  Nfa* nfa = post2nfa(post);

  assert_non_null(nfa);
  ASSERT_LABEL(nfa->start, EPSILON);
  State* start_of_a = nfa->start->outs[0];
  ASSERT_LABEL(start_of_a, 'a');
  State* out_of_a = start_of_a->outs[0];
  ASSERT_LABEL(out_of_a, SPLIT);
  {
    assert_ptr_equal(out_of_a->outs[0], start_of_a);  // should come back
    {
      ASSERT_LABEL(out_of_a->outs[1], EPSILON);
      State* start_of_b = out_of_a->outs[1]->outs[0];
      ASSERT_LABEL(start_of_b, 'b');
      State* out_of_b = start_of_b->outs[0];
      ASSERT_LABEL(out_of_b, SPLIT);
      {
        assert_ptr_equal(out_of_b->outs[0], start_of_b);  // should come back
        ASSERT_LABEL(out_of_b->outs[1], ACCEPT);
      }
    }
  }

  delete_nfa(nfa);
}

static void test_post2nfa_concat_zero_or_more() {
  const char* post = "a*b*.";

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
        ASSERT_LABEL(out_of_a->outs[1], SPLIT);
        {
          {
            State* start_of_b = out_of_a->outs[1]->outs[0];
            ASSERT_LABEL(start_of_b, 'b');
            State* out_of_b = start_of_b->outs[0];
            ASSERT_LABEL(out_of_b, SPLIT);
            {
              assert_ptr_equal(out_of_b->outs[0],
                               start_of_b);  // should come back
              ASSERT_LABEL(out_of_b->outs[1], ACCEPT);
            }
          }
          ASSERT_LABEL(out_of_a->outs[1]->outs[1], ACCEPT);
        }
      }
    }
    ASSERT_LABEL(nfa->start->outs[1], SPLIT);
  }

  delete_nfa(nfa);
}

static void test_post2nfa_concat_zero_or_one() {
  const char* post = "a?b?.";

  Nfa* nfa = post2nfa(post);

  assert_non_null(nfa);
  ASSERT_LABEL(nfa->start, SPLIT);
  {
    // "ab", no fastforward
    ASSERT_NON_SPLIT_TRANSITION_LABELS(nfa->start->outs[0], 'a', EPSILON, SPLIT,
                                       'b', EPSILON, ACCEPT);
    {  // "", yet another fastforward
      ASSERT_LABEL(nfa->start->outs[1], SPLIT);
      ASSERT_LABEL(nfa->start->outs[1]->outs[1], ACCEPT);
    }
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
