#ifndef __LIST_H__
#define __LIST_H__

#include "poly.h"

typedef struct MonoList {
  struct Mono* content;
  struct MonoList* next;
} MonoList;

#endif //__LIST_H__
