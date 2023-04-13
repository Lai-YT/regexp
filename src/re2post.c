/*
Copyright (c) 2007 Russ Cox
Copyright (c) 2023 Lai-YT

Can be distributed under the MIT license.
The full text of the license can be found in
https://opensource.org/license/mit/.
 */

#include "re2post.h"

#include <stdbool.h>
#include <string.h>

#define BUF_SIZE 8000
#define MAX_NESTED_DEPTH 100

/// @brief operators eat up symbols immediately, while the two binary operators,
/// . and |, don't. Since the union operator is explicitly notated in the
/// regular expression, it's being counted.
typedef struct Unit {
  int num_of_union;
  int num_of_unit;
} Unit;

/// @brief whether the buffer may overflow after adding explicit concatenation
/// symbols.
static bool buf_may_overflow(const char* re);
static bool has_too_many_nested_parens(const int depth);

static bool has_unit_to_operate(Unit unit);
static bool has_units_to_concat(Unit unit);
static bool has_units_to_union(Unit unit);

static bool has_stashed_unit(Unit* stack, Unit* top);
static void stash_unit(Unit** stack, Unit unit);
static void restore_unit(Unit** stack, Unit* unit);
static void init_unit(Unit* unit);

/// @brief Tries to append a explicit concatenation operator to the result.
/// @param unit records the number of concatenations awaiting.
/// @param result appends the operator to.
///
/// @note Russ Cox uses while loops to append the concatenation operators in his
/// implementation. But there shouldn't be more than 1 pair of un-concatenated
/// units awaiting since concatenation is treated as left-associative, so an if
/// statement suffices.
static void try_append_concat(Unit* unit, char** result);

/// @brief Tries to append union operators to the result.
/// @param unit records the number of unions awaiting.
/// @param result appends the operator to.
static void try_append_unions(Unit* unit, char** result);

/// @details Tracks the parentheses with a stack, and counts the number
/// of operation units so we know where to place an operator after every two
/// units. An operation unit can be a single symbol or a parenthesized set of
/// symbols/operators. Each parenthesized set of symbols/operators is treated
/// as a single unit after being converted.
char* re2post(const char* re) {
  static char result[BUF_SIZE];

  if (buf_may_overflow(re)) {
    return NULL;
  }

  char* result_tail = result;

  /// @brief A stack. Stashing the nested parentheses units seen so far, so we
  /// can restore them after converting inner nested parenthesized units. Treat
  /// the converted unit as a single unit, and resume the conversion.
  Unit paren_units[MAX_NESTED_DEPTH];
  Unit* top_unit = paren_units;  // it's in fact the one above the top
  Unit curr_paren_unit;          // the unit that we're now converting
  init_unit(&curr_paren_unit);

  for (; *re; re++) {
    switch (*re) {
      case '(':
        if (has_too_many_nested_parens(top_unit - paren_units)) {
          return NULL;
        }
        // the previous units are converted first
        // because concatenation is left-associative,
        try_append_concat(&curr_paren_unit, &result_tail);

        // a new parenthesized unit is now about to start,
        // stash the current one and move on
        stash_unit(&top_unit, curr_paren_unit);
        init_unit(&curr_paren_unit);
        break;
      case '|':
        if (!has_unit_to_operate(curr_paren_unit)) {
          return NULL;
        }
        // the previous concatenations are converted first
        // because union has lower precedence than concatenation,
        try_append_concat(&curr_paren_unit, &result_tail);

        curr_paren_unit.num_of_union++;
        break;
      case ')':
        if (!has_stashed_unit(paren_units, top_unit)
            || !has_unit_to_operate(curr_paren_unit)) {
          return NULL;
        }
        // The current unit is about to complete, append the awaiting operators.
        try_append_concat(&curr_paren_unit, &result_tail);
        try_append_unions(&curr_paren_unit, &result_tail);

        // the current parenthesized unit is converted and becomes a single
        // unit. Restore the outer unit
        restore_unit(&top_unit, &curr_paren_unit);
        curr_paren_unit.num_of_unit++;
        break;
      case '*':
      case '+':
      case '?':
        if (!has_unit_to_operate(curr_paren_unit)) {
          return NULL;
        }
        // unary left-associative with highest precedence,
        // append right next to the previous unit
        *result_tail++ = *re;
        break;
      default:
        // the previous units are converted first
        // because concatenation is left-associative
        try_append_concat(&curr_paren_unit, &result_tail);

        *result_tail++ = *re;
        curr_paren_unit.num_of_unit++;
        break;
    }
  }
  if (has_stashed_unit(paren_units, top_unit)) {
    return NULL;  // unmatched parentheses
  }
  // The conversion is about to complete, append the awaiting operators.
  try_append_concat(&curr_paren_unit, &result_tail);
  try_append_unions(&curr_paren_unit, &result_tail);

  if (curr_paren_unit.num_of_union != 0) {
    return NULL;  // missing operand
  }

  *result_tail = '\0';
  return result;
}

static void init_unit(Unit* unit) {
  *unit = (Unit){.num_of_union = 0, .num_of_unit = 0};
}

static void stash_unit(Unit** stack, Unit unit) {
  **stack = unit;
  (*stack)++;  // push
}

static void restore_unit(Unit** stack, Unit* unit) {
  Unit* top = *stack - 1;
  *unit = *top;
  (*stack)--;  // pop
}

static bool buf_may_overflow(const char* re) {
  // the number of concat symbol can at most be strlen(re) - 1 when re contains
  // only concatenations
  return strlen(re) > BUF_SIZE / 2;
}

static bool has_too_many_nested_parens(const int depth) {
  return depth > MAX_NESTED_DEPTH;
}

static void try_append_concat(Unit* unit, char** result) {
  if (has_units_to_concat(*unit)) {
    --unit->num_of_unit;
    **result = EXPLICIT_CONCAT;
    (*result)++;
  }
}
static void try_append_unions(Unit* unit, char** result) {
  while (has_units_to_union(*unit)) {
    --unit->num_of_union;
    --unit->num_of_unit;
    **result = '|';
    (*result)++;
  }
}

static bool has_unit_to_operate(Unit unit) {
  return unit.num_of_unit > 0;
}

static bool has_units_to_concat(Unit unit) {
  // binary operator needs at least 2 units
  return unit.num_of_unit - unit.num_of_union >= 2;
}

static bool has_units_to_union(Unit unit) {
  // binary operator needs at least 2 units
  return unit.num_of_union >= 1 && unit.num_of_unit >= 2;
}

static bool has_stashed_unit(Unit* stack, Unit* top) {
  return top != stack;
}
