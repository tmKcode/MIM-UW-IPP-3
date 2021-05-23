/** @file
  Interfejs klasy stosu wielomianów rzadkich wielu zmiennych

  @authors Tomasz Ziębowicz <t.ziebowicz@students.mimuw.edu.pl>
  @copyright Uniwersytet Warszawski
  @date 2021
*/

#ifndef STACK_H
#define STACK_H

#define STACK_MINIMUM_CAPACITY 2
#define STACK_GROWTH_FACTOR 2

#include "poly.h"

#include <stddef.h>
#include <stdlib.h>


typedef struct PolyStack {
  size_t top;
  size_t capacity;

  Poly *array;
} PolyStack;

PolyStack NewPolyStack();

Poly PolyStackPeek(PolyStack* s);

Poly PolyStackPop(PolyStack* s);

void PolyStackPush(PolyStack* s, Poly* p);

void PolyStackDestroy(PolyStack* s);

#endif // STACK_H
