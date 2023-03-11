#include <stdbool.h>
#include <string.h>

#include "regex.h"

#define BUF_SIZE 8000
#define MAX_NESTED_DEPTH 100
#define EXPLICIT_CONCAT '.'

// Unary operators eat up symbols immediately, while the two binary operators, .
// and |, don't. We have to count them and transform them when it's time.
typedef struct {
  int num_to_union;
  int num_to_concat;
} unit_t;

/**
 * @brief whether the buffer may overflow after adding explicit concatenation
 * symbols.
 */
static bool buf_may_overflow(const char* re);
static bool has_too_many_nested_parens(const int depth);
static bool has_unit_to_operate(unit_t unit);
static bool has_units_to_concat(unit_t unit);
static bool has_units_to_union(unit_t unit);
static bool has_stashed_unit(unit_t* unit_stack, unit_t* top_unit);
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
  unit_t* top_unit = paren_units;  // it's in fact the one above the top
  unit_t curr_paren_unit = {.num_to_union = 0, .num_to_concat = 0};

  if (buf_may_overflow(re)) {
    return NULL;
  }
  for (; *re; re++) {
    switch (*re) {
      case '(':
        if (has_too_many_nested_parens(top_unit - paren_units)) {
          return NULL;
        }
        // because concatenation is left associative, the previous units are
        // transformed first
        if (has_units_to_concat(curr_paren_unit)) {
          --curr_paren_unit.num_to_concat;
          *buf_top++ = EXPLICIT_CONCAT;
        }
        // after the transformation, stash it, and move on the new parenthesized
        // unit
        stash_unit(&top_unit, curr_paren_unit);
        init_unit(&curr_paren_unit);
        break;
      case '|':
        if (!has_unit_to_operate(curr_paren_unit)) {
          return NULL;
        }
        // union has lower precedence than concatenation,
        // so the previous concatenations are transformed first
        while (has_units_to_concat(curr_paren_unit)) {
          --curr_paren_unit.num_to_concat;
          *buf_top++ = EXPLICIT_CONCAT;
        }
        // the previous unit is to be ​unionized instead of concatenated
        curr_paren_unit.num_to_concat--;
        curr_paren_unit.num_to_union++;
        // another addition for the current unit
        curr_paren_unit.num_to_union++;
        break;
      case ')':
        if (!has_stashed_unit(paren_units, top_unit) ||
            !has_unit_to_operate(curr_paren_unit)) {
          return NULL;
        }
        if (has_units_to_concat(curr_paren_unit)) {
          --curr_paren_unit.num_to_concat;
          *buf_top++ = EXPLICIT_CONCAT;
        }
        while (has_units_to_union(curr_paren_unit)) {
          --curr_paren_unit.num_to_union;
          *buf_top++ = '|';
        }
        // the nested parenthesized unit is transformed, restore
        restore_unit(&top_unit, &curr_paren_unit);
        curr_paren_unit.num_to_concat++;
        break;
      case '*':
      case '+':
      case '?':
        if (!has_unit_to_operate(curr_paren_unit)) {
          return NULL;
        }
        // unary left-associative with highest precedence,
        // append right next to the previous unit
        *buf_top++ = *re;
        break;
      default:
        // because concatenation is left associative,
        // the previous units are transformed first
        if (has_units_to_concat(curr_paren_unit)) {
          --curr_paren_unit.num_to_concat;
          *buf_top++ = EXPLICIT_CONCAT;
        }
        *buf_top++ = *re;
        curr_paren_unit.num_to_concat++;
        break;
    }
  }
  if (has_stashed_unit(paren_units, top_unit)) {
    return NULL;  // unmatched parentheses
  }
  if (has_units_to_concat(curr_paren_unit)) {
    --curr_paren_unit.num_to_concat;
    *buf_top++ = EXPLICIT_CONCAT;
  }
  while (has_units_to_union(curr_paren_unit)) {
    --curr_paren_unit.num_to_union;
    *buf_top++ = '|';
  }
  *buf_top = '\0';
  return buf;
}

static void init_unit(unit_t* unit) {
  *unit = (unit_t){.num_to_union = 0, .num_to_concat = 0};
}

static void stash_unit(unit_t** unit_stack, unit_t unit) {
  **unit_stack = unit;
  (*unit_stack)++;  // push
}

static void restore_unit(unit_t** unit_stack, unit_t* unit) {
  unit_t* top = *unit_stack - 1;
  *unit = *top;
  (*unit_stack)--;  // pop
}

static bool buf_may_overflow(const char* re) {
  // the number of concat symbol can at most be strlen(re) - 1 when re contains
  // only concatenations
  return strlen(re) > BUF_SIZE / 2;
}

static bool has_too_many_nested_parens(const int depth) {
  return depth > MAX_NESTED_DEPTH;
}

static bool has_unit_to_operate(unit_t unit) {
  return unit.num_to_concat > 0;
}

static bool has_units_to_concat(unit_t unit) {
  return unit.num_to_concat >= 2;
}

static bool has_units_to_union(unit_t unit) {
  return unit.num_to_union >= 2;
}

static bool has_stashed_unit(unit_t* unit_stack, unit_t* top_unit) {
  return top_unit != unit_stack;
}
