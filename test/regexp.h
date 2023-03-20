#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>

#include "../src/regexp.h"

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

  assert_int_equal(labeled_state->label, label);
  assert_ptr_equal(labeled_state->outs[0], outs[0]);
}

static void test_create_epsilon_state() {
  State out1, out2;  // ill-formed but doesn't matter
  State* outs[] = {&out1, &out2};

  State* epsilon_state = create_state(SPLIT, outs);

  assert_int_equal(epsilon_state->label, SPLIT);
  assert_ptr_equal(epsilon_state->outs[0], &out1);
  assert_ptr_equal(epsilon_state->outs[1], &out2);
}

static void test_create_accepting_state() {
  State* accepting_state = create_state(ACCEPT, NULL);

  assert_int_equal(accepting_state->label, ACCEPT);
  assert_null(accepting_state->outs[0]);
}

static void test_create_nfa() {
  State start, accept;  // ill-formed but doesn't matter

  Nfa* nfa = create_nfa(&start, &accept);

  assert_ptr_equal(nfa->start, &start);
  assert_ptr_equal(nfa->accept, &accept);
}

static void test_post2nfa_concat_only() {
  const char* post = "ab.c.";

  Nfa* nfa = post2nfa(post);

  assert_non_null(nfa);
  State* s = nfa->start;
  assert_int_equal(s->label, 'a');
  s = s->outs[0];
  assert_int_equal(s->label, 'b');
  s = s->outs[0];
  assert_int_equal(s->label, 'c');
  s = s->outs[0];
  assert_int_equal(s->label, ACCEPT);
}

// TODO: refactor tests

static void test_post2nfa_union_only_single() {
  const char* post = "ab|";

  Nfa* nfa = post2nfa(post);

  assert_non_null(nfa);
  State* s = nfa->start;
  assert_int_equal(s->label, SPLIT);
  {
    State* t = s->outs[0];
    assert_int_equal(t->label, 'a');
    t = t->outs[0];
    assert_int_equal(t->label, EPSILON);
    t = t->outs[0];
    assert_int_equal(t->label, ACCEPT);
  }
  {
    State* t = s->outs[1];
    assert_int_equal(t->label, 'b');
    t = t->outs[0];
    assert_int_equal(t->label, EPSILON);
    t = t->outs[0];
    assert_int_equal(t->label, ACCEPT);
  }
}

static void test_post2nfa_union_only_complex() {
  const char* post = "abc||";

  Nfa* nfa = post2nfa(post);

  assert_non_null(nfa);
  State* s = nfa->start;
  assert_int_equal(s->label, SPLIT);
  {
    State* t = s->outs[0];
    assert_int_equal(t->label, 'a');
    t = t->outs[0];
    assert_int_equal(t->label, EPSILON);
    t = t->outs[0];
    assert_int_equal(t->label, ACCEPT);
  }
  {
    State* t = s->outs[1];
    assert_int_equal(t->label, SPLIT);
    {
      State* u = t->outs[0];
      assert_int_equal(u->label, 'b');
      u = u->outs[0];
      assert_int_equal(u->label, EPSILON);
      u = u->outs[0];
      assert_int_equal(u->label, EPSILON);
    }
    {
      State* u = t->outs[1];
      assert_int_equal(u->label, 'c');
      u = u->outs[0];
      assert_int_equal(u->label, EPSILON);
      u = u->outs[0];
      assert_int_equal(u->label, EPSILON);
      t = u->outs[0];
      assert_int_equal(t->label, ACCEPT);
    }
  }
}

static void test_post2nfa_zero_or_more() {
  const char* post = "a*";

  Nfa* nfa = post2nfa(post);

  assert_non_null(nfa);
  State* s = nfa->start;
  assert_int_equal(s->label, SPLIT);
  {
    State* a = s->outs[0];
    assert_int_equal(a->label, 'a');
    State* t = a->outs[0];
    assert_int_equal(t->label, SPLIT);
    {
      State* u = t->outs[0];
      assert_ptr_equal(u, a);  // should come back
    }
    {
      State* u = t->outs[1];
      assert_int_equal(u->label, ACCEPT);
    }
  }
  {
    State* t = s->outs[1];
    assert_int_equal(t->label, ACCEPT);
  }
}

static void test_post2nfa_zero_or_one() {
  const char* post = "a?";

  Nfa* nfa = post2nfa(post);

  assert_non_null(nfa);
  State* s = nfa->start;
  assert_int_equal(s->label, SPLIT);
  {
    State* t = s->outs[0];
    assert_int_equal(t->label, 'a');
    t = t->outs[0];
    assert_int_equal(t->label, EPSILON);
    t = t->outs[0];
    assert_int_equal(t->label, ACCEPT);
  }
  {
    State* t = s->outs[1];
    assert_int_equal(t->label, ACCEPT);
  }
}

static void test_post2nfa_one_or_more() {
  const char* post = "a+";

  Nfa* nfa = post2nfa(post);

  assert_non_null(nfa);
  State* a = nfa->start;
  assert_int_equal(a->label, 'a');
  State* t = a->outs[0];
  assert_int_equal(t->label, SPLIT);
  {
    State* u = t->outs[0];
    assert_ptr_equal(u, a);  // should come back
  }
  {
    State* u = t->outs[1];
    assert_int_equal(u->label, ACCEPT);
  }
}

static void test_post2nfa_mix() {
  const char* post = "ab*c|.";  // a(b*|c)

  Nfa* nfa = post2nfa(post);

  assert_non_null(nfa);
  State* s = nfa->start;
  assert_int_equal(s->label, 'a');
  s = s->outs[0];
  assert_int_equal(s->label, SPLIT);
  {
    State* t = s->outs[0];
    assert_int_equal(t->label, SPLIT);
    {
      State* a = t->outs[0];
      assert_int_equal(a->label, 'b');
      State* u = a->outs[0];
      assert_int_equal(u->label, SPLIT);
      {
        State* v = u->outs[0];
        assert_ptr_equal(v, a);  // should come back
      }
      {
        State* v = u->outs[1];
        assert_int_equal(v->label, EPSILON);
      }
    }
    {
      State* u = t->outs[1];
      assert_int_equal(u->label, EPSILON);
      u = u->outs[0];
      assert_int_equal(u->label, ACCEPT);
    }
  }
  {
    State* t = s->outs[1];
    assert_int_equal(t->label, 'c');
    t = t->outs[0];
    assert_int_equal(t->label, EPSILON);
    t = t->outs[0];
    assert_int_equal(t->label, ACCEPT);
  }
}

static void test_missing_operator_should_return_null() {
  assert_null(post2nfa("ab"));
  assert_null(post2nfa("abc"));
}

static void test_missing_operand_should_return_null() {
  assert_null(post2nfa("a."));
  assert_null(post2nfa("*"));
}
