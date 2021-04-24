#ifndef __LIST_H__
#define __LIST_H__

#include "poly.h"

typedef struct MonoList {
  Mono content;
  struct MonoList* next;
} MonoList;

void listFree(MonoList *head);

#endif //__LIST_H__
