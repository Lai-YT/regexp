#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>

#include "../src/nfa.h"

// clang-format off
// cmocka allows test applications to use custom definitions of C standard
// library functions and types by having us include the necessary headers.
#include <cmocka.h>
// clang-format on

static void test_create_nfa() {
  State* accept = create_state(ACCEPT, NULL);
  State* start = create_state('a', &accept);

  Nfa* nfa = create_nfa(start, accept);

  assert_ptr_equal(nfa->start, start);
  assert_ptr_equal(nfa->accept, accept);

  delete_nfa(nfa);
}
