#include "../src/regexp.h"

#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>

// clang-format off
// cmocka allows test applications to use custom definitions of C standard
// library functions and types by having us include the necessary headers.
#include <cmocka.h>
// clang-format on

static void test_re2post_concatenation(void** state) {
  (void)state;
  const char* re = "abba";
  const char* post = "ab.b.a.";

  const char* result = re2post(re);

  assert_string_equal(result, post);
}

static void test_re2post_union(void** state) {
  (void)state;
  const char* re = "ab|ba";
  const char* post = "ab.ba.|";

  const char* result = re2post(re);

  assert_string_equal(result, post);
}

static void test_re2post_concatenation_with_paren(void** state) {
  (void)state;
  const char* re = "a(bb)a";
  const char* post = "abb..a.";

  const char* result = re2post(re);

  assert_string_equal(result, post);
}

static void test_re2post_union_with_parent(void** state) {
  (void)state;
  const char* re = "a(b|a)a";
  const char* post = "aba|.a.";

  const char* result = re2post(re);

  assert_string_equal(result, post);
}

static void test_re2post_mix(void** state) {
  (void)state;
  const char* re = "a(bb)+a";
  const char* post = "abb.+.a.";

  const char* result = re2post(re);

  assert_string_equal(result, post);
}

int main(void) {
  const struct CMUnitTest tests[] = {
      cmocka_unit_test(test_re2post_concatenation),
      cmocka_unit_test(test_re2post_union),
      cmocka_unit_test(test_re2post_concatenation_with_paren),
      cmocka_unit_test(test_re2post_union_with_parent),
      cmocka_unit_test(test_re2post_mix),
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}
