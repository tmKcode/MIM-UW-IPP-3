/** @file
  Implementacja klasy stosu wielomianów rzadkich wielu zmiennych

  @authors Tomasz Ziębowicz <t.ziebowicz@students.mimuw.edu.pl>
  @copyright Uniwersytet Warszawski
  @date 2021
*/

#include "stack.h"

/**
 * Minimalny rozmiar stosu wielomianów.
 */
#define STACK_MINIMUM_CAPACITY 16

/**
 * Współczynnik, o który zmienia się rozmiar stosu.
 */
#define STACK_RESIZE_FACTOR 2

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

size_t PolyStackSize(const PolyStack *s) {
  assert(s);

  return s->top + 1;
}

Poly PolyStackPeek(const PolyStack *s) {
  assert(!PolyStackIsEmpty(s));

  return s->array[s->top];
}

Poly PolyStackPeekSecond(const PolyStack *s) {
  assert(PolyStackSize(s) >= 2);

  return s->array[s->top - 1];
}

/**
 * Sprawdza, czy stos s powinien się zmiejszyć.
 * @param[in] s : stos
 * @return Czy stos s powinien się zmiejszyć?
 */
static bool PolyStackShouldShrink(PolyStack *s) {
  size_t size = PolyStackSize(s);

  return size < s->capacity / (STACK_RESIZE_FACTOR * 2) &&
         size > STACK_MINIMUM_CAPACITY;
}

/**
 * Zmniejsza stos s.
 * @param[in,out] s : stos
 */
static void PolyStackShrink(PolyStack *s) {
  assert(s);

  s->capacity /= STACK_RESIZE_FACTOR;
  s->array = realloc(s->array, s->capacity * sizeof(Poly));

  CHECK_PTR(s->array);
}

void PolyStackPop(PolyStack *s) {
  assert(!PolyStackIsEmpty(s));

  PolyDestroy(&(s->array[s->top]));
  s->top--;

  if (PolyStackShouldShrink(s)) {
    PolyStackShrink(s);
  }
}

/**
 * Sprawdza, czy stos s powinien się zwiększyć.
 * @param[in] s : stos
 * @return Czy stos s powinien się zwiększyć?
 */
static bool PolyStackShouldGrow(PolyStack *s) {
  return PolyStackSize(s) == s->capacity;
}

/**
 * Zwiększa stos s.
 * @param[in,out] s : stos
 */
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
  for (long i = s->top; i >= 0; i--) {
    PolyDestroy(&(s->array[i]));
  }

  free(s->array);
}