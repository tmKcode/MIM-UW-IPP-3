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

/**
 * To jest struktura przechowująca stos wielomianów.
 */
typedef struct PolyStack {
  /**
   * To jest indeks wierzchu stosu w tablicy array.
   */
  size_t top;
  /**
   * To jest rozmiar tablicy array.
   */
  size_t capacity;
  /**
   * To jest tablica przechowująca wielomiany
   */
  Poly *array;
} PolyStack;

/**
 * Zwraca nowy, pusty stos wielomianów.
 * @return pusty stos
 */
PolyStack NewPolyStack();

/**
 * Zwraca rozmiar stosu s.
 * @param[in] s : stos
 * @return rozmiar stosu
 */
size_t PolyStackSize(const PolyStack *s);

/**
 * Sprawdza, czy stos s jest pusty.
 * @param[in] s : stos
 * @return Czy stos s jest pusty?
 */
static inline bool PolyStackIsEmpty(const PolyStack *s) {
  return PolyStackSize(s) == 0;
}

/**
 * Zwraca wielomian na wierzchu stosu s.
 * @param[in] s : stos
 * @return wielomian na wierzchu stosu
 */
Poly PolyStackPeek(const PolyStack *s);

/**
 * Zwraca drugi od góry wielomian ze stosu s.
 * @param[in] s : stos
 * @return drugi od góry wielomian ze stosu
 */
Poly PolyStackPeekSecond(const PolyStack *s);

/**
 * Usuwa wielomian na wierzchu ze stosu s.
 * Zwalnia zajętą przez niego pamięć.
 * @param[in,out] s : stos
 */
void PolyStackPop(PolyStack *s);

/**
 * Wstawia wielomian p na wierzch stosu s.
 * @param[in,out] s : stos
 * @param[in] p : wielomian
 */
void PolyStackPush(PolyStack *s, Poly p);

/**
 * Usuwa stos s.
 * @param[in,out] s : stos
 */
void PolyStackDestroy(PolyStack *s);

#endif // STACK_H
