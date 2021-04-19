#include "list.h"
#include "poly.h"
#include <stdlib.h>

static inline MonoList *newMonoList() {
  MonoList *new = malloc(sizeof(MonoList));
  new->content = NULL;
  new->next = NULL;

  return new;
}

void listInsertNext(MonoList *precedingElement, Mono *content) {
  assert(precedingElement != NULL);

  MonoList *newElement = newMonoList();
  newElement->content = content;
  newElement->next = precedingElement->next;
  precedingElement->next = newElement;
}

void listRemoveNext(MonoList *precedingElement) {
  assert(precedingElement != NULL);
  assert(precedingElement->next != NULL);

  MonoDestroy(precedingElement->next->content);

  MonoList *tmp = precedingElement->next->next;
  free(precedingElement->next);
  precedingElement->next = tmp;
}

void listFree(MonoList *head) {
  assert(head != NULL);

  while (head->next != NULL) listRemoveNext(head);

  free(head);
}

void listMap(MonoList *head, void (*f)(Mono *)) {
  assert(head != NULL);

  while (head->next != NULL) {
    f(head->next->content);
    head = head->next;
  }
}