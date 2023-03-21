#ifndef LIST_H
#define LIST_H

typedef struct List {
  void* val;
  struct List* next;
} List;

List* create_list(void* val);
void append_list(List* a, List* b);

/// @brief Deletes the list with all vals untouched,
void delete_list(List*);

/// @brief Returns a shallow copied list.
List* copy_list(List*);

#endif /* end of include guard: LIST_H */
