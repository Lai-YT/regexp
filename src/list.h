#ifndef LIST_H
#define LIST_H

typedef struct List {
  void* val;
  struct List* next;
} List;

List* create_list(void* val);
void append_list(List* a, List* b);

#endif /* end of include guard: LIST_H */
