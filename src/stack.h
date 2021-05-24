/** @file
  Interfejs klasy stosu wielomianów rzadkich wielu zmiennych

  @authors Tomasz Ziębowicz <t.ziebowicz@students.mimuw.edu.pl>
  @copyright Uniwersytet Warszawski
  @date 2021
*/

#ifndef STACK_H
#define STACK_H

#include "poly.h"

#include <stddef.h>
#include <stdlib.h>

#define STACK_MINIMUM_CAPACITY 2
#define STACK_RESIZE_FACTOR 2

typedef struct PolyStack {
  size_t top;
  size_t capacity;

  Poly *array;
} PolyStack;

PolyStack NewPolyStack();

size_t PolyStackSize(PolyStack *s);

static inline bool PolyStackIsEmpty(PolyStack *s) {
  return PolyStackSize(s) > 0;
}

Poly PolyStackPeek(PolyStack *s);

Poly PolyStackPeekSecond(PolyStack *s);

void PolyStackPop(PolyStack *s);

void PolyStackPush(PolyStack *s, Poly p);

void PolyStackDestroy(PolyStack *s);

#endif // STACK_H
