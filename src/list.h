#ifndef __LIST_H__
#define __LIST_H__

#include "poly.h"

typedef struct MonoList {
  Mono content;
  struct MonoList* next;
} MonoList;

void listFree(MonoList *head);

void listIter(MonoList *head, void (*f)(Mono *));

#endif //__LIST_H__
