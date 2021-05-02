#include "poly.h"
#include <stdlib.h>
//#include "polylib.h"

#define CHECK_PTR(p)                                                           \
  do {                                                                         \
    if (!p) { exit(1); }                                                       \
  } while (0)

/**
 * Podnosi liczbę naturalną do naturalnej potęgi.
 * @param[in] base :  podstawa @f$base@f$
 * @param[in] exp : wykładnik @f$exp@f$
 * @return @f$base^exp@f$
 */
poly_coeff_t poly_coeff_tPow(poly_coeff_t base, poly_exp_t exp) {
  poly_coeff_t result = 1;
  while (exp != 0) {
    if (exp % 2 != 0) result *= base;
    exp /= 2;
    base *= base;
  }

  return result;
}

/**
 * Zwraca pierwszy węzeł jednoelementowej listy zawierającej wartownika.
 * @return pierwszy węzeł nowej listy
 */
static MonoList *NewMonoList() {
  MonoList *newList = malloc(sizeof(MonoList));
  CHECK_PTR(newList);
  newList->next = NULL;

  return newList;
}

/**
 * Sprawdza, czy lista składa się z samego wartownika (ma jeden węzeł).
 * @param[in] head : pierwszy węzeł
 * @return Czy lista składa się tylko z wartownika?
 */
static inline bool listIsEmpty(const MonoList *head) { return !head->next; }

/**
 * Sprawdza, czy lista składa się z jednego znaczącego elementu.
 * @param[in] head : pierwszy węzeł
 * @return Czy lista ma tylko jeden znaczący element?
 */
static inline bool listIsOneElement(const MonoList *head) {
  return head->next && !head->next->next;
}

/**
 * Zwraca następny jednomian z listy.
 * @param[in] precedingElement : poprzedzający węzeł
 * @return następny jednomian
 */
static Mono *MonoListNextMono(const MonoList *precedingElement) {
  assert(precedingElement);
  assert(precedingElement->next);

  return (&(precedingElement->next->m));
}

/**
 * Wstawia jednomian jako następny węzeł w liście.
 * Funkcja przejmuje jednomian na własność.
 * @param[in] precedingElement : poprzedzający węzeł
 * @param[in] content : wstawiany jednomian
 */
static void MonoListInsertNext(MonoList *precedingElement, Mono *content) {
  assert(precedingElement);

  MonoList *newElement = NewMonoList();
  newElement->m = *content;
  newElement->next = precedingElement->next;
  precedingElement->next = newElement;
}

/**
 * Usuwa następny węzeł listy z pamięci.
 * Funckja zachowując strukturę listy.
 * Funkcja nie usuwa jednomianu należącego do usuniętego węzła z pamięci.
 * @param[in] precedingElement : poprzedzający węzeł
 */
static void MonoListFreeNext(MonoList *precedingElement) {
  assert(precedingElement);
  assert(precedingElement->next);

  MonoList *tmp = precedingElement->next->next;
  free(precedingElement->next);
  precedingElement->next = tmp;
}

/**
 * Usuwa listę z pamięci.
 * Funkcja nie usuwa jednomianów zawartych w liście z pamięci.
 * @param[in] head : pierwszy węzeł
 */
static void MonoListFree(MonoList *head) {
  assert(head);

  while (head->next) { MonoListFreeNext(head); }

  free(head);
}

/**
 * Usuwa pierwszy element listy (wartownika) z pamięci.
 * Funckja zwraca nowy początek listy.
 * @param[in] head : pierwszy węzeł
 * @return nowy pierwszy węzeł
 */
static MonoList *MonoListRemoveDummy(MonoList *head) {
  MonoList *newHead = head->next;
  free(head);

  return newHead;
}

/**
 * Robi pełną, głęboką kopię listy.
 * @param[in] src : pierwszy węzeł kopiowanej listy
 * @return pierwszy węzeł skopiowanej listy
 */
static MonoList *MonoListClone(const MonoList *src) {
  assert(src);

  MonoList *copy = NewMonoList();
  MonoList *copyIter = copy;
  while (src->next) {
    Mono monoCopy = MonoClone(MonoListNextMono(src));
    MonoListInsertNext(copyIter, &monoCopy);
    src = src->next;
    copyIter = copyIter->next;
  }

  return copy;
}

void PolyDestroy(Poly *p) {
  if (!PolyIsCoeff(p)) {
    MonoList *iter = p->list;
    while (iter->next) {
      MonoDestroy(&(iter->next->m));
      iter = iter->next;
    }

    MonoListFree(p->list);
  }
}

Poly PolyClone(const Poly *p) {
  if (PolyIsCoeff(p)) {
    return (Poly){.coeff = p->coeff, .list = NULL};
  } else {
    assert(p->list->next);

    return (Poly){.coeff = p->coeff, .list = MonoListClone(p->list)};
  }
}

static void PolyPlusEqualConstant(Poly *p, poly_coeff_t c);

static void PolyAddMerge(Poly *p, const Poly *q);

/**
 * Dodaje wielomian do wielomianu.
 * @param[in] p : zwiększany wielomian
 * @param[in] q : dodawany wielomian
 */
static void PolyPlusEqual(Poly *p, const Poly *q) {
  bool pIsCoeff = PolyIsCoeff(p);
  bool qIsCoeff = PolyIsCoeff(q);

  if (pIsCoeff && qIsCoeff) {
    p->coeff += q->coeff;
  } else if (pIsCoeff) {
    Poly copy = PolyClone(q);
    PolyPlusEqualConstant(&copy, p->coeff);
    *p = copy;
  } else if (qIsCoeff) {
    PolyPlusEqualConstant(p, q->coeff);
  } else {
    PolyAddMerge(p, q);
  }
}

/**
 * Dodaje wielomian niebędący współczynnikiem
 * do wielomianu niebędącego współczynnikiem.
 * @param[in] p : zwiększany wielomian
 * @param[in] q : dodawany wielomian
 */
static void PolyAddMerge(Poly *p, const Poly *q) {
  assert(p->list && q->list);

  MonoList *pIter = p->list;
  MonoList *qIter = q->list;
  while (pIter->next && qIter->next) {
    if (MonoListNextMono(pIter)->exp > MonoListNextMono(qIter)->exp) {
      pIter = pIter->next;
    } else if (MonoListNextMono(pIter)->exp < MonoListNextMono(qIter)->exp) {
      Mono copy = MonoClone(&(qIter->next->m));
      MonoListInsertNext(pIter, &copy);

      pIter = pIter->next;
      qIter = qIter->next;
    } else {
      PolyPlusEqual(&(pIter->next->m.p), &(qIter->next->m.p));

      pIter = pIter->next;
      qIter = qIter->next;
    }
  }

  if (qIter->next) { pIter->next = MonoListRemoveDummy(MonoListClone(qIter)); }
}

/**
 * Dodaje stałą do wielomianu niebędącego współczynnikiem.
 * @param[in] p : zwiększany wielomian
 * @param[in] c : dodawana stała
 */
static void PolyPlusEqualConstant(Poly *p, poly_coeff_t c) {
  if (PolyIsCoeff(p)) {
    p->coeff += c;
  } else {
    assert(p->list);

    MonoList *iter = p->list;
    while (iter->next) { iter = iter->next; }

    if (MonoGetExp(&(iter->m)) == 0) {
      PolyPlusEqualConstant(&(iter->m.p), c);
    } else {
      Poly coeff = PolyFromCoeff(c);
      Mono constant = MonoFromPoly(&coeff, 0);
      MonoListInsertNext(iter, &constant);
    }
  }
}

static void MonoRemoveZeros(Mono *m);

/**
 * Sprawdza, czy jednomian jest tożsamościowo równy zeru.
 * @param[in] m : jednomian
 * @return Czy jednomian jest równy zeru?
 */
static inline bool MonoIsZero(const Mono *m) { return PolyIsZero(&(m->p)); }

/**
 * Usuwa jednomiany tożsamościowo równe zeru z wielomianu.
 * Przekształca wielomian do poprawnej formy.
 * @param[in] p : wielomian
 */
static void PolyRemoveZeros(Poly *p) {
  if (!PolyIsCoeff(p)) {
    assert(p->list);

    MonoList *iter = p->list;
    while (iter && iter->next) {
      MonoRemoveZeros(&(iter->next->m));
      if (MonoIsZero(&(iter->next->m))) {
        MonoListFreeNext(iter);
      } else {
        iter = iter->next;
      }
    }

    if (listIsEmpty(p->list)) {
      MonoListFree(p->list);
      *p = PolyZero();
    }
  }
}

/**
 * Usuwa jednomiany tożsamościowo równe zeru z jednomianu.
 * @param[in] p : jednomian
 */
static inline void MonoRemoveZeros(Mono *m) { PolyRemoveZeros(&(m->p)); }

/**
 * Sprawdza, czy jednomian jest stałą.
 * @param[in] m : jednomian
 * @return Czy jednomian jest stałą?
 */
static inline bool MonoIsConstant(const Mono *m) {
  return m->exp == 0 && PolyIsCoeff(&(m->p));
}
static inline void MonoNormalizeConstants(Mono *m);

/**
 * Zamienia wielomian tożsamościowo równy stałej na stałą.
 * @param[in] m : wielomian
 */
static inline void PolyCoeffy(Poly *p) {
  poly_coeff_t c = p->list->next->m.p.coeff;
  MonoListFree(p->list);
  p->list = NULL;
  p->coeff = c;
}

/**
 * Modyfikuje wielomian,
 * zamieniając wielomiany i jednomiany tożsamościowo równe stałym na stałe.
 * @param[in] m : wielomian
 */
static void PolyNormalizeConstants(Poly *p) {
  if (!PolyIsCoeff(p)) {
    assert(p->list);

    MonoList *iter = p->list;
    while (iter->next) {
      MonoNormalizeConstants(&(iter->next->m));
      iter = iter->next;
    }

    if (listIsOneElement(p->list) && MonoIsConstant(&(p->list->next->m))) {
      PolyCoeffy(p);
    }
  }
}

/**
 * Modyfikuje jednomian,
 * zamieniając wielomiany i jednomiany tożsamościowo równe stałym na stałe.
 * @param[in] m : jednomian
 */
static inline void MonoNormalizeConstants(Mono *m) {
  PolyNormalizeConstants(&(m->p));
}

Poly PolyAdd(const Poly *p, const Poly *q) {
  Poly result = PolyClone(p);
  PolyPlusEqual(&result, q);

  PolyRemoveZeros(&result);
  PolyNormalizeConstants(&result);

  return result;
}

/**
 * Wstawia jednomian do listy jednomianów.
 * Jeśli w liście znajduje się jednomian o tym samym wykładniku,
 * jest on zwiększany przez wstawiany wielomian.
 * @param[in] head : pierwszy węzeł
 * @param[in] m : jednomian
 */
static void MonoListInsert(MonoList *head, Mono *m) {
  while (head->next && MonoListNextMono(head)->exp > m->exp) {
    head = head->next;
  }

  if (!head->next || MonoListNextMono(head)->exp < m->exp) {
    MonoListInsertNext(head, m);
  } else {
    PolyPlusEqual(&(MonoListNextMono(head)->p), &(m->p));
    MonoDestroy(m);
  }
}

Poly PolyAddMonos(size_t count, const Mono monos[]) {
  if (count == 0) { return PolyZero(); }
  MonoList *head = NewMonoList();

  Mono mono;
  for (size_t i = 0; i < count; ++i) {
    mono = monos[i];
    MonoListInsert(head, &mono);
  }

  Poly result = (Poly){.list = head};
  PolyRemoveZeros(&result);
  PolyNormalizeConstants(&result);

  return result;
}

/**
 * Mnoży wielomian przez stałą.
 * @param[in] p : wielomian
 * @param[in] c : stała
 */
static void PolyDotEqualConstant(Poly *p, poly_coeff_t c) {
  if (PolyIsCoeff(p)) {
    p->coeff *= c;
  } else {
    assert(p->list);

    MonoList *iter = p->list;
    Mono *mono;
    while (iter->next) {
      mono = MonoListNextMono(iter);
      PolyDotEqualConstant(&(mono->p), c);

      iter = iter->next;
    }
  }
}

static Poly PolyMulMonos(const Poly *p, const Poly *q);

Poly PolyMul(const Poly *p, const Poly *q) {
  bool pIsCoeff = PolyIsCoeff(p);
  bool qIsCoeff = PolyIsCoeff(q);

  if (pIsCoeff && qIsCoeff) {
    return (Poly){.coeff = p->coeff * q->coeff, .list = NULL};
  } else if (pIsCoeff) {
    Poly result = PolyClone(q);
    PolyDotEqualConstant(&result, p->coeff);

    PolyRemoveZeros(&result);
    PolyNormalizeConstants(&result);

    return result;
  } else if (qIsCoeff) {
    Poly result = PolyClone(p);
    PolyDotEqualConstant(&result, q->coeff);

    PolyRemoveZeros(&result);
    PolyNormalizeConstants(&result);

    return result;
  } else {
    return PolyMulMonos(p, q);
  }
}

/**
 * Mnoży dwa wielomiany niebędące stałymi.
 * @param[in] p : wielomian @f$p@f$
 * @param[in] q : wielomian @f$q@f$
 * @return @f$p * q@f$
 */
static Poly PolyMulMonos(const Poly *p, const Poly *q) {
  assert(p->list && q->list);

  Poly result = (Poly){.list = NewMonoList()};
  MonoList *pIter = p->list;
  MonoList *qIter;
  Mono *pMono;
  Mono *qMono;
  Mono subResult;

  while (pIter->next) {
    qIter = q->list;
    pMono = MonoListNextMono(pIter);

    while (qIter->next) {
      qMono = MonoListNextMono(qIter);

      subResult = (Mono){.exp = pMono->exp + qMono->exp,
                         .p = PolyMul(&(pMono->p), &(qMono->p))};
      MonoListInsert(result.list, &subResult);

      qIter = qIter->next;
    }

    pIter = pIter->next;
  }

  PolyRemoveZeros(&result);
  PolyNormalizeConstants(&result);

  return result;
}

//static inline void MonoNegate(Mono *m);
//
//void PolyNegate(Poly *p) {
//  if (PolyIsCoeff(p)) {
//    p->coeff *= -1;
//  } else {
//    (assert(p->list));
//
//    MonoList *iter = p->list;
//    while (iter->next) {
//      MonoNegate(MonoListNextMono(iter));
//
//      iter = iter->next;
//    }
//  }
//}
//
//static inline void MonoNegate(Mono *m) { PolyNegate(&(m->p)); }

Poly PolyNeg(const Poly *p) {
  Poly negOne = PolyFromCoeff(-1);

  return PolyMul(p, &negOne);
}

Poly PolySub(const Poly *p, const Poly *q) {
  Poly qNeg = PolyNeg(q);
  Poly result = PolyAdd(p, &qNeg);
  PolyDestroy(&qNeg);

  return result;
}

poly_exp_t PolyDegBy(const Poly *p, size_t varIdx) {
  if (PolyIsCoeff(p)) {
    return PolyIsZero(p) ? -1 : 0;
  } else if (varIdx == 0) {
    return MonoListNextMono(p->list)->exp;
  } else {
    assert(p->list);

    poly_exp_t maxDeg = 0;
    poly_exp_t newDeg;
    MonoList *iter = p->list;
    while (iter->next) {
      newDeg = PolyDegBy(&(MonoListNextMono(iter)->p), varIdx - 1);
      if (newDeg > maxDeg) { maxDeg = newDeg; }

      iter = iter->next;
    }

    return maxDeg;
  }
}

poly_exp_t PolyDeg(const Poly *p) {
  if (PolyIsZero(p)) {
    return -1;
  } else if (PolyIsCoeff(p)) {
    return 0;
  } else {
    assert(p->list);

    poly_exp_t maxDeg = 0;
    poly_exp_t newDeg;
    Mono *mono;
    MonoList *iter = p->list;
    while (iter->next) {
      mono = MonoListNextMono(iter);
      newDeg = mono->exp;
      newDeg += PolyDeg(&(mono->p));

      if (newDeg > maxDeg) { maxDeg = newDeg; }

      iter = iter->next;
    }

    return maxDeg;
  }
}

static bool MonoIsEq(const Mono *m, const Mono *n) {
  return m->exp == n->exp && PolyIsEq(&(m->p), &(n->p));
}

bool PolyIsEq(const Poly *p, const Poly *q) {
  bool pIsCoeff = PolyIsCoeff(p);
  bool qIsCoeff = PolyIsCoeff(q);

  if (pIsCoeff && qIsCoeff) {
    return p->coeff == q->coeff;
  } else if (pIsCoeff || qIsCoeff) {
    return false;
  } else {
    assert(p->list && q->list);

    MonoList *pIter = p->list;
    MonoList *qIter = q->list;
    while (pIter->next && qIter->next) {
      if (!MonoIsEq(MonoListNextMono(pIter), MonoListNextMono(qIter))) {
        return false;
      }

      pIter = pIter->next;
      qIter = qIter->next;
    }
    if (pIter->next || qIter->next) { return false; }

    return true;
  }
}

Poly PolyAt(const Poly *p, poly_coeff_t x) {
  if (PolyIsCoeff(p)) {
    return PolyClone(p);
  } else if (x == 0) {
    MonoList *iter = p->list;
    Mono mono;

    while (iter->next) { iter = iter->next; }

    mono = iter->m;
    if (mono.exp == 0) {
      return PolyClone(&(mono.p));
    } else {
      return PolyZero();
    }
  } else {
    assert(p->list);

    Poly result = PolyZero();
    MonoList *iter = p->list;
    Mono *mono;
    Poly subResult;
    Poly constant;

    while (iter->next) {
      mono = MonoListNextMono(iter);
      constant = PolyFromCoeff(poly_coeff_tPow(x, mono->exp));
      subResult = PolyMul(&(mono->p), &constant);

      PolyPlusEqual(&result, &subResult);
      PolyDestroy(&subResult);

      iter = iter->next;
    }

    PolyRemoveZeros(&result);
    PolyNormalizeConstants(&result);

    return result;
  }
}
