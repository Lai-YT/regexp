#include <stdbool.h>
#include <string.h>

#include "regex.h"

#define BUF_SIZE 8000
#define MAX_NESTED_DEPTH 100
#define EXPLICIT_CONCAT '.'

typedef struct {
  /* union has the lowest precedence, record separately */
  int num_of_union;
  int num_of_unit;
} unit_t;

/**
 * @brief whether the buffer may overflow after adding explicit concatenation
 * symbols.
 */
static bool buf_may_overflow(const char* re);
static void stash_unit(unit_t** unit_stack, unit_t unit);
static void restore_unit(unit_t** unit_stack, unit_t* unit);
static void init_unit(unit_t* unit);

char* re2post(const char* re) {
  static char buf[BUF_SIZE];
  char* buf_top = buf;

  /*
   * We are to track the parentheses with a stack, and counts the number of
   * operation units so we know where to place an operator after every two
   * units. An operation units can be a single symbol or a parenthesized set of
   * symbols/operators. Each parenthesized set of symbols/operators is treated
   * as a single unit after being transformed.
   */

  // paren_units works as a stack. Stashing the nested parentheses units seen
  // so far, so we can restore them after transforming inner nested
  // parenthesized units. Treat the transformed unit as a single unit, and
  // resume the transformation.
  unit_t paren_units[MAX_NESTED_DEPTH];
  unit_t* top_unit = paren_units;  // its in fact the one above the top
  unit_t curr_paren_unit = {.num_of_union = 0, .num_of_unit = 0};

  if (buf_may_overflow(re)) {
    return NULL;
  }
  for (; *re; re++) {
    switch (*re) {
      case '(':
        // Starts a new parenthesized operation unit. It's concatenated with the
        // operation unit on its left, so add a concat symbol.
        if (curr_paren_unit.num_of_unit > 1) {
          --curr_paren_unit.num_of_unit;
          *buf_top++ = EXPLICIT_CONCAT;
        }
        // error, too many nested parentheses, stack overflows
        if (top_unit - paren_units > MAX_NESTED_DEPTH) {
          return NULL;
        }
        stash_unit(&top_unit, curr_paren_unit);
        init_unit(&curr_paren_unit);
        break;
      case '|':
        // error, no left unit to union with
        if (curr_paren_unit.num_of_unit == 0) {
          return NULL;
        }
        while (--curr_paren_unit.num_of_unit > 0) {
          *buf_top++ = EXPLICIT_CONCAT;
        }
        curr_paren_unit.num_of_union++;
        break;
      case ')':
        // error, nothing inside the parenthesis
        if (top_unit == paren_units || curr_paren_unit.num_of_unit == 0) {
          return NULL;
        }
        while (--curr_paren_unit.num_of_unit > 0) {
          *buf_top++ = EXPLICIT_CONCAT;
        }
        while (curr_paren_unit.num_of_union-- > 0) {
          *buf_top++ = '|';
        }
        // the nested parenthesized unit is transformed, restore
        restore_unit(&top_unit, &curr_paren_unit);
        curr_paren_unit.num_of_unit++;
        break;
      case '*':
      case '+':
      case '?':
        // error, nothing to operate on
        if (curr_paren_unit.num_of_unit == 0) {
          return NULL;
        }
        // they have the highest precedence
        *buf_top++ = *re;
        break;
      default:
        if (curr_paren_unit.num_of_unit > 1) {
          --curr_paren_unit.num_of_unit;
          *buf_top++ = EXPLICIT_CONCAT;
        }
        *buf_top++ = *re;
        curr_paren_unit.num_of_unit++;
        break;
    }
  }

  // unmatched parentheses
  if (top_unit != paren_units) {
    return NULL;
  }
  while (--curr_paren_unit.num_of_unit > 0) {
    *buf_top++ = EXPLICIT_CONCAT;
  }
  while (curr_paren_unit.num_of_union-- > 0) {
    *buf_top++ = '|';
  }
  *buf_top = '\0';
  return buf;
}

static bool buf_may_overflow(const char* re) {
  // the number of concat symbol can at most be strlen(re) - 1 when re contains
  // only concatenations
  return strlen(re) > BUF_SIZE / 2;
}

static void stash_unit(unit_t** unit_stack, unit_t unit) {
  **unit_stack = unit;
  (*unit_stack)++;  // push
}

static void init_unit(unit_t* unit) {
  *unit = (unit_t){.num_of_union = 0, .num_of_unit = 0};
}

static void restore_unit(unit_t** unit_stack, unit_t* unit) {
  unit_t* top = *unit_stack - 1;
  *unit = *top;
  (*unit_stack)--;  // pop
}
