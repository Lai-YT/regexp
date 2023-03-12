#ifndef EXPLICIT_CONCAT
#define EXPLICIT_CONCAT '.'
#endif

/// @brief Converts infix regexp re to postfix notation.
/// Inserts . as explicit concatenation operator.
/// Cheesy parser, returns static buffer.
///
/// @note Associative Property holds for concatenation and union operator, the
/// postfix notation isn't unique. This function has concatenation and union
/// operator be left and right-associative, respectively.
char* re2post(const char* re);
