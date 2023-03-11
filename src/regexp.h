/**
 * Converts infix regexp re to postfix notation.
 * Inserts . as explicit concatenation operator.
 * Cheesy parser, returns static buffer.
 */
char* re2post(const char* re);
