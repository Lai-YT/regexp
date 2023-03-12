#include "../src/re2post.h"

#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>

// clang-format off
// cmocka allows test applications to use custom definitions of C standard
// library functions and types by having us include the necessary headers.
#include <cmocka.h>
// clang-format on

static void test_re2post_concat(void** state) {
  (void)state;
  assert_string_equal(re2post("abba"), "ab.b.a.");
}

static void test_re2post_union(void** state) {
  (void)state;
  assert_string_equal(re2post("a|b|c|d|e|f"), "abcdef|||||");
}

static void test_re2post_union_and_concat(void** state) {
  (void)state;
  assert_string_equal(re2post("ab|ba"), "ab.ba.|");
}

static void test_re2post_paren(void** state) {
  (void)state;
  assert_string_equal(re2post("(a(b(c(d(e)))))"), "abcde....");
}

static void test_re2post_concatenation_with_paren(void** state) {
  (void)state;
  assert_string_equal(re2post("a(bb)a"), "abb..a.");
}

static void test_re2post_union_with_paren(void** state) {
  (void)state;
  assert_string_equal(re2post("a|((b|c)|(d|e))|f"), "abc|de||f||");
}

static void test_re2post_mix(void** state) {
  (void)state;
  assert_string_equal(re2post("a(bbbb|a|bab)+a"), "abb.b.b.aba.b.||+.a.");
}

int main(void) {
  const struct CMUnitTest tests[] = {
      cmocka_unit_test(test_re2post_concat),
      cmocka_unit_test(test_re2post_union),
      cmocka_unit_test(test_re2post_paren),
      cmocka_unit_test(test_re2post_union_and_concat),
      cmocka_unit_test(test_re2post_concatenation_with_paren),
      cmocka_unit_test(test_re2post_union_with_paren),
      cmocka_unit_test(test_re2post_mix),
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}
