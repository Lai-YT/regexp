/*
Copyright (c) 2007 Russ Cox
Copyright (c) 2023 Lai-YT

Can be distributed under the MIT license.
The full text of the license can be found in
https://opensource.org/license/mit/.
 */

#ifndef RE2POST_H
#define RE2POST_H

#ifndef EXPLICIT_CONCAT
#define EXPLICIT_CONCAT '#'
#endif

/// @brief Converts infix regexp re to postfix notation.
/// Inserts . as explicit concatenation operator.
/// Cheesy parser, returns static buffer.
/// @return The postfix form of re; NULL if its ill-formed of too long.
/// @note Associative Property holds for concatenation and union operator, the
/// postfix notation isn't unique. This function has concatenation and union
/// operator be left and right-associative, respectively.
char* re2post(const char* re);

#endif /* end of include guard: RE2POST_H */
