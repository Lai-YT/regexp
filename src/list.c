#include "list.h"

#include <stdlib.h>

List* create_list(void* val) {
  List* l;
  l = malloc(sizeof *l);
  l->val = val;
  l->next = NULL;
  return l;
}

void append_list(List* a, List* b) {
  while (a->next) {
    a = a->next;
  }
  a->next = b;
}

void delete_list(List* l) {
  List* tmp = l;
  while (l) {
    tmp = l->next;
    free(l);
    l = tmp;
  }
}

List* copy_list(List* l) {
  if (!l) {
    return NULL;
  }
  List* res = create_list(l->val);
  l = l->next;
  for (; l; l = l->next) {
    append_list(res, create_list(l->val));
  }
  return res;
}
