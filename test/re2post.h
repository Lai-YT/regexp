#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>

#include "../src/re2post.h"

// clang-format off
// cmocka allows test applications to use custom definitions of C standard
// library functions and types by having us include the necessary headers.
#include <cmocka.h>
// clang-format on

static void test_re2post_single_character() {
  assert_string_equal(re2post("a"), "a");
}

static void test_re2post_concat() {
  assert_string_equal(re2post("abba"), "ab#b#a#");
}

static void test_re2post_union() {
  assert_string_equal(re2post("a|b|c|d|e|f"), "abcdef|||||");
}

static void test_re2post_zero_or_more() {
  assert_string_equal(re2post("a*b"), "a*b#");
}

static void test_re2post_one_or_more() {
  assert_string_equal(re2post("a+b"), "a+b#");
}

static void test_re2post_zero_or_one() {
  assert_string_equal(re2post("a?b"), "a?b#");
}

static void test_re2post_any() {
  assert_string_equal(re2post("a.b"), "a.#b#");
}

static void test_re2post_union_and_concat() {
  assert_string_equal(re2post("ab|ba"), "ab#ba#|");
}

static void test_re2post_paren() {
  assert_string_equal(re2post("(a(b(c(d(e)))))"), "abcde####");
}

static void test_re2post_concat_with_paren() {
  assert_string_equal(re2post("a(bb)a"), "abb##a#");
}

static void test_re2post_union_with_paren() {
  assert_string_equal(re2post("a|((b|c)|(d|e))|f"), "abc|de||f||");
}

static void test_re2post_mix() {
  assert_string_equal(re2post("a(bb?b.b|a|b*ab)+a"),
                      "abb?#b#.#b#ab*a#b#||+#a#");
}

static void test_re2post_empty_re_should_be_empty_post() {
  assert_string_equal(re2post(""), "");
}
