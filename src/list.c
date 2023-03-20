#include <stdlib.h>

#include "list.h"

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
