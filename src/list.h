#ifndef __LIST_H__
#define __LIST_H__

#include "poly.h"

typedef struct MonoList {
  Mono* content;
  MonoList* next;
} MonoList;

#endif //__LIST_H__
