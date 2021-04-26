#include "list.h"
#include "poly.h"
#include <stdlib.h>

#define CHECK_PTR(p)                                                           \
  do {                                                                         \
    if (p == NULL) { exit(1); }                                                \
  } while (0)

MonoList *newMonoList() {
  MonoList *newList = malloc(sizeof(MonoList));
  CHECK_PTR(newList);
  newList->next = NULL;

  return newList;
}

static inline void listNext(MonoList **element) { *element = (*element)->next; }

Mono listNextContent(MonoList *precedingElement) {
  assert(precedingElement != NULL);
  assert(precedingElement->next != NULL);

  return(precedingElement->next->content);
}

void listInsertNext(MonoList *precedingElement, Mono content) {
  assert(precedingElement != NULL);

  MonoList *newElement = newMonoList();
  newElement->content = content;
  newElement->next = precedingElement->next;
  precedingElement->next = newElement;
}

void listRemoveNext(MonoList *precedingElement) {
  assert(precedingElement != NULL);
  assert(precedingElement->next != NULL);

  MonoDestroy(&precedingElement->next->content);

  MonoList *tmp = precedingElement->next->next;
  free(precedingElement->next);
  precedingElement->next = tmp;
}

void listFree(MonoList *head) {
  assert(head != NULL);

  while (head->next != NULL) {
    listRemoveNext(head);
  }

  free(head);
}

void listIter(MonoList *head, void (*f)(struct Mono *)) {
  assert(head != NULL);

  while (head->next != NULL) {
    f(head->next->content);
    head = head->next;
  }
}

void listFold(MonoList *head, void (*f)(Mono *, void *), void *acc) {
  assert(head != NULL);

  while (head->next != NULL) {
    f(head->next->content, acc);
    head = head->next;
  }
}
// Modify to create deep copy.
void listCopy(MonoList *src, MonoList *dest) {
  assert(src != NULL);
  assert(dest != NULL);

  while (src->next != NULL) {
    listInsertNext(dest, src->next->content);

    src = src->next;
    dest = dest->next;
  }
}