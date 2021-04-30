#include "polylib.h"
#include "poly.h"
#include <stdlib.h>

#define CHECK_PTR(p)                                                            \
  do {                                                                          \
    if (!p) { exit(1); }                                                        \
  } while (0)

MonoList *newMonoList() {
  MonoList *newList = malloc(sizeof(MonoList));
  CHECK_PTR(newList);
  newList->next = NULL;

  return newList;
}

//rm
static inline void listNext(MonoList **element) { *element = (*element)->next; }

Mono *listNextMono(MonoList *precedingElement) {
  assert(precedingElement);
  assert(precedingElement->next);

  return(&(precedingElement->next->m));
}

void listInsertNext(MonoList *precedingElement, const Mono *content) {
  assert(precedingElement);

  MonoList *newElement = newMonoList();
  newElement->m = *content;
  newElement->next = precedingElement->next;
  precedingElement->next = newElement;
}

void listFreeNext(MonoList *precedingElement) {
  assert(precedingElement);
  assert(precedingElement->next);

  MonoList *tmp = precedingElement->next->next;
  free(precedingElement->next);
  precedingElement->next = tmp;
}

void listFree(MonoList *head) {
  assert(head);

  while (head->next) { listFreeNext(head); }

  free(head);
}

//void listIter(MonoList *head, void (*f)(struct Mono *)) {
//  assert(head);
//
//  while (head->next) {
//    f(head->next->content);
//    head = head->next;
//  }
//}
//
//void listFold(MonoList *head, void (*f)(Mono *, void *), void *acc) {
//  assert(head);
//
//  while (head->next) {
//    f(head->next->content, acc);
//    head = head->next;
//  }
//}
//Deep copy.
MonoList *listClone(MonoList *src) {
  assert(src);

  MonoList *copy = newMonoList();
  MonoList *copyIter = copy;
  while (src->next) {
    Mono monoCopy = MonoClone(listNextMono(src));
    listInsertNext(copyIter, &monoCopy);
    src = src->next;
    copyIter = copyIter->next;
  }

  return copy;
}

MonoList *listUndummify(MonoList *head) {
  MonoList *newHead = head->next;
  free(head);

  return newHead;
}

MonoList *listDummify(MonoList *head) {
  MonoList *newHead = newMonoList();
  newHead->next = head;

  return newHead;
}