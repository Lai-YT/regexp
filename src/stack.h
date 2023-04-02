#ifndef STACK_H
#define STACK_H

#include <stdbool.h>

typedef struct Stack Stack;

/// @return An empty stack.
Stack* create_stack();

/// @brief Pushes the value onto the stack.
/// @note The ownership of the value isn't taken by the stack.
void push_stack(Stack*, void* value);

/// @brief Removes the top-most value from the stack.
/// @return The top-most value of the stack; NULL if the stack is empty.
void* pop_stack(Stack*);

bool is_empty_stack(Stack*);

/// @brief Deletes the stack.
/// @note The values store inside the stack are not deleted.
void delete_stack(Stack*);

#endif /* end of include guard: STACK_H */
