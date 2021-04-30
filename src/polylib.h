#ifndef __POLYLIB_H__
#define __POLYLIB_H__

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>

/** To jest typ reprezentujący współczynniki. */
typedef long poly_coeff_t;

/** To jest typ reprezentujący wykładniki. */
typedef int poly_exp_t;

struct Mono;

struct MonoList;

/**
 * To jest struktura przechowująca wielomian.
 * Wielomian jest albo liczbą całkowitą, czyli wielomianem stałym
 * (wtedy `list == NULL`),
 * albo niepustą listą jednomianów (wtedy `list != NULL`).
 */
typedef struct Poly {
  /**
  * To współczynnik wielomianu.
  * Jeżeli `list == NULL`, wtedy jest to współczynnik będący liczbą całkowitą.
  * W przeciwnym przypadku jest on nieokreślony.
  */
  poly_coeff_t coeff;///< współczynnik
  /** To jest lista jednomianów.*/
  struct MonoList *list;
} Poly;

/**
 * To jest struktura przechowująca jednomian.
 * Jednomian ma postać @f$px_i^n@f$.
 * Współczynnik @f$p@f$ może też być
 * wielomianem nad kolejną zmienną @f$x_{i+1}@f$.
 */
typedef struct Mono {
  Poly p;        ///< współczynnik
  poly_exp_t exp;///< wykładnik
} Mono;

typedef struct MonoList {
  Mono m;
  struct MonoList* next;
} MonoList;

poly_coeff_t poly_coeff_t_pow(poly_coeff_t base, poly_exp_t exp);

MonoList *newMonoList();

static inline bool listIsEmpty(MonoList *head) { return !head->next; }

static inline bool listIsOneElement(MonoList *head) {
  return head->next && !head->next->next;
}

Mono *listNextMono(MonoList *precedingElement);

void listFree(MonoList *head);

void listInsertNext(MonoList *precedingElement, const Mono *content);

void listFreeNext(MonoList *precedingElement);

MonoList *listFromArray(size_t count, const Mono monos[]);

MonoList *listClone(MonoList *src);

MonoList *listUndummify(MonoList *head);

//MonoList *listDummify(MonoList *head);

#endif //__POLYLIB_H__
