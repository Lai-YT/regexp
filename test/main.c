#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>

// clang-format off
// cmocka allows test applications to use custom definitions of C standard
// library functions and types by having us include the necessary headers.
#include <cmocka.h>
// clang-format on

/* A test case that does nothing and succeeds. */
static void null_test_success(void **state) {
  (void)state; /* unused */
}

int main(void) {
  const struct CMUnitTest tests[] = {
      cmocka_unit_test(null_test_success),
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}
