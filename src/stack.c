/** @file
  Implementacja klasy stosu wielomianów rzadkich wielu zmiennych

  @authors Tomasz Ziębowicz <t.ziebowicz@students.mimuw.edu.pl>
  @copyright Uniwersytet Warszawski
  @date 2021
*/

#include "stack.h"

/**
 * Sprawdza, czy alokacja została poprawnie wykonana.
 * Jeśli nie, zakańcza wykonywanie programu z kodem @f$1@f$.
 * @param[in] p : wskaźnik pod którym alokowano pamięć
 */
#define CHECK_PTR(p)                                                           \
  do {                                                                         \
    if (!p) {                                                                  \
      exit(1);                                                                 \
    }                                                                          \
  } while (0)

PolyStack NewPolyStack() {
  PolyStack s =
      (PolyStack){.top = -1,
                  .capacity = STACK_MINIMUM_CAPACITY,
                  .array = malloc(STACK_MINIMUM_CAPACITY * sizeof(Poly))};

  CHECK_PTR(s.array);

  return s;
}

size_t PolyStackSize(PolyStack *s) {
  assert(s);

  return s->top + 1;
}

Poly PolyStackPeek(PolyStack *s) {
  assert(!PolyStackIsEmpty(s));

  return s->array[s->top];
}

Poly PolyStackPeekSecond(PolyStack *s) {
  assert(PolyStackSize(s) >= 2);

  return s->array[s->top - 1];
}

static bool PolyStackShouldShrink(PolyStack *s) {
  size_t size = PolyStackSize(s);

  return size < s->capacity / (STACK_RESIZE_FACTOR * 2) &&
         size > STACK_MINIMUM_CAPACITY;
}

static void PolyStackShrink(PolyStack *s) {
  assert(s);

  s->capacity /= STACK_RESIZE_FACTOR;
  s->array = realloc(s->array, s->capacity * sizeof(Poly));

  CHECK_PTR(s->array);
}

void PolyStackPop(PolyStack *s) {
  assert(!PolyStackIsEmpty(s));

  PolyDestroy(&(s->array[s->top--]));

  if (PolyStackShouldShrink(s)) {
    PolyStackShrink(s);
  }
}

static bool PolyStackShouldGrow(PolyStack *s) {
  return PolyStackSize(s) == s->capacity;
}

static void PolyStackGrow(PolyStack *s) {
  assert(s);

  s->capacity *= STACK_RESIZE_FACTOR;
  s->array = realloc(s->array, s->capacity * sizeof(Poly));

  CHECK_PTR(s->array);
}

void PolyStackPush(PolyStack *s, Poly p) {
  if (PolyStackShouldGrow(s)) {
    PolyStackGrow(s);
  }

  s->array[++s->top] = p;
}

void PolyStackDestroy(PolyStack *s) {
  while (PolyStackSize(s) > 0) {
    PolyStackPop(s);
  }

  free(s->array);
}