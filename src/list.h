#ifndef __LIST_H__
#define __LIST_H__

#include <stdlib.h>
#include "poly.h"

typedef struct MonoList {
  Mono content;
  struct MonoList* next;
} MonoList;

MonoList *newMonoList();

Mono listNextContent(MonoList *precedingElement);

void listFree(MonoList *head);

void listInsertNext(MonoList *precedingElement, Mono content);

void listRemoveNext(MonoList *precedingElement);

#endif //__LIST_H__
