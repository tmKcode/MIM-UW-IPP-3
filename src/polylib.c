#include "polylib.h"
#include "poly.h"
#include <stdlib.h>

#define CHECK_PTR(p)                                                            \
  do {                                                                          \
    if (!p) { exit(1); }                                                        \
  } while (0)

poly_coeff_t poly_coeff_tPow(poly_coeff_t base, poly_exp_t exp) {
  poly_coeff_t result = 1;
  while (exp != 0 ) {
    if (exp % 2 != 0)
      result *= base;
    exp /= 2;
    base *= base;
  }

  return result;
}

MonoList *NewMonoList() {
  MonoList *newList = malloc(sizeof(MonoList));
  CHECK_PTR(newList);
  newList->next = NULL;

  return newList;
}


Mono *MonoListNextMono(MonoList *precedingElement) {
  assert(precedingElement);
  assert(precedingElement->next);

  return(&(precedingElement->next->m));
}

void MonoListInsertNext(MonoList *precedingElement, Mono *content) {
  assert(precedingElement);

  MonoList *newElement = NewMonoList();
  newElement->m = *content;
  newElement->next = precedingElement->next;
  precedingElement->next = newElement;
}

void MonoListFreeNext(MonoList *precedingElement) {
  assert(precedingElement);
  assert(precedingElement->next);

  MonoList *tmp = precedingElement->next->next;
  free(precedingElement->next);
  precedingElement->next = tmp;
}

void MonoListFree(MonoList *head) {
  assert(head);

  while (head->next) { MonoListFreeNext(head); }

  free(head);
}

//MonoList *listFromArray(size_t count, const Mono monos[]) {
//  MonoList *head = NewMonoList();
//
//  MonoList *iter = head;
//  for (size_t i = 0; i < count; ++i) {
//    MonoListInsertNext(iter, &monos[i]);
//    iter = iter->next;
//  }
//
//  return head;
//}

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
MonoList *MonoListClone(MonoList *src) {
  assert(src);

  MonoList *copy = NewMonoList();
  MonoList *copyIter = copy;
  while (src->next) {
    Mono monoCopy = MonoClone(MonoListNextMono(src));
    MonoListInsertNext(copyIter, &monoCopy);
    src = src->next;
    copyIter = copyIter->next;
  }

  return copy;
}

MonoList *MonoListRemoveDummy(MonoList *head) {
  MonoList *newHead = head->next;
  free(head);

  return newHead;
}



//MonoList *listDummify(MonoList *head) {
//  MonoList *newHead = NewMonoList();
//  newHead->next = head;
//
//  return newHead;
//}