/** @file
  Implementacja klasy stosu wielomianów rzadkich wielu zmiennych

  @authors Tomasz Ziębowicz <t.ziebowicz@students.mimuw.edu.pl>
  @copyright Uniwersytet Warszawski
  @date 2021
*/

#include "stack.h"

PolyStack NewPolyStack() {
  PolyStack s;

  s.top = -1;
  s.capacity = STACK_MINIMUM_CAPACITY;
  s.array = malloc(STACK_MINIMUM_CAPACITY * sizeof(Poly));

  return s;
}

Poly PolyStackPeek(PolyStack* s){}

Poly PolyStackPop(PolyStack* s){}

void PolyStackPush(PolyStack* s, Poly* p){}

void PolyStackDestroy(PolyStack* s){}