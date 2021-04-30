#include "poly.h"
#include "polylib.h"

void PolyDestroy(Poly *p) {
  if (!PolyIsCoeff(p)) {
    MonoList *iter = p->list;
    while (iter->next) {
      MonoDestroy(&(iter->next->m));
      iter = iter->next;
    }

    listFree(p->list);
  }
}

Poly PolyClone(const Poly *p) {
  if (PolyIsCoeff(p)) {
    return (Poly){.coeff = p->coeff, .list = NULL};
  } else {
    assert(p->list->next);

    return (Poly){.coeff = p->coeff, .list = listClone(p->list)};
  }
}

static void PolyPlusEqualConstant(Poly *p, poly_coeff_t c);

static void PolyMerge(Poly *p, const Poly *q);

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
    PolyMerge(p, q);
  }
}

static void PolyMerge(Poly *p, const Poly *q) {
  assert(p->list && q->list);

  MonoList *pIter = p->list;
  MonoList *qIter = q->list;
  while (pIter->next && qIter->next) {
    if (listNextMono(pIter)->exp > listNextMono(qIter)->exp) {
      pIter = pIter->next;
    } else if (listNextMono(pIter)->exp < listNextMono(qIter)->exp) {
      Mono copy = MonoClone(&(qIter->next->m));
      listInsertNext(pIter, &copy);

      pIter = pIter->next;
      qIter = qIter->next;
    }
    //Equal exponents.
    else {
      PolyPlusEqual(&(pIter->next->m.p), &(qIter->next->m.p));

      pIter = pIter->next;
      qIter = qIter->next;
    }
  }
  // dDEEEEEP
  if (qIter->next) { pIter->next = listUndummify(listClone(qIter)); }
}

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
      listInsertNext(iter, &constant);
    }
  }
}
static void MonoRemoveZeros(Mono *m);

static inline bool MonoIsZero(const Mono *m) { return PolyIsZero(&(m->p)); }

static void PolyRemoveZeros(Poly *p) {
  if (!PolyIsCoeff(p)) {
    assert(p->list);

    MonoList *iter = p->list;
    while (iter && iter->next) {
      MonoRemoveZeros(&(iter->next->m));
      if (MonoIsZero(&(iter->next->m))) { listFreeNext(iter); }
      else { iter = iter->next; }
    }

    if (listIsEmpty(p->list)) {
      listFree(p->list);
      *p = PolyZero();
    }
  }
}

static inline void MonoRemoveZeros(Mono *m) { PolyRemoveZeros(&(m->p)); }

static inline bool MonoIsConstant(const Mono *m) {
  return m->exp == 0 && PolyIsCoeff(&(m->p));
}
static inline void MonoNormalizeConstants(Mono *m);

static inline void PolyCoeffy(Poly *p) {
  poly_coeff_t c = p->list->next->m.p.coeff;
  listFree(p->list);
  p->list = NULL;
  p->coeff = c;
}

static void PolyNormalizeConstants(Poly *p) {
  if (!PolyIsCoeff(p)) {
    assert(p->list);

    MonoList *iter = p->list;
    while (iter->next) {
      MonoNormalizeConstants(&(iter->next->m));
      iter = iter->next;
    }

    if (listIsOneElement(p->list) &&
        MonoIsConstant(&(p->list->next->m))) {
      PolyCoeffy(p);
    }
  }
}

static inline void MonoNormalizeConstants(Mono *m) {
  PolyNormalizeConstants(&(m->p));
}

Poly PolyAdd(const Poly *p, const Poly *q) {
  Poly result = PolyZero();

  PolyPlusEqual(&result, p);
  PolyPlusEqual(&result, q);

  PolyRemoveZeros(&result);
  PolyNormalizeConstants(&result);

  return result;
}
static inline Poly PolyFromMono(Mono *m) {
  MonoList *new = newMonoList();
  listInsertNext(new, m);

  return (Poly){.list = new};
}

Poly PolyAddMonos(size_t count, const Mono monos[]) {
  if (count == 0) { return PolyZero(); }
  MonoList *head = newMonoList();

  MonoList *iter;
  Mono mono;
  for (size_t i = 0; i < count; ++i) {
    iter = head;
    mono = monos[i];
    while (iter->next && listNextMono(iter)->exp > mono.exp) {
    iter = iter->next;
    }
    if (!iter->next || listNextMono(iter)->exp < mono.exp) {
      listInsertNext(iter, &mono);
    }
    else {
    PolyPlusEqual(&(listNextMono(iter)->p), &(mono.p));
    MonoDestroy(&mono);
    }
  }

  Poly result = (Poly){.list = head};
  PolyRemoveZeros(&result);
  PolyNormalizeConstants(&result);

  return result;
}

static bool MonoIsEq(const Mono *m, const Mono *n) {
  return m->exp == n->exp && PolyIsEq(&(m->p), &(n->p));
}

static inline void MonoNegate(Mono *m);

void PolyNegate(Poly *p) {
  if (PolyIsCoeff(p)) {
    p->coeff *= -1;
  }
  else {
    (assert(p->list));

    MonoList *iter = p->list;
    while (iter->next) {
      MonoNegate(listNextMono(iter));

      iter = iter->next;
    }
  }
}

static inline void MonoNegate(Mono *m) { PolyNegate(&(m->p)); }

Poly PolyNeg(const Poly *p) {
  Poly result = PolyClone(p);
  PolyNegate(&result);

  return result;
}

Poly PolySub(const Poly *p, const Poly *q) {
  Poly qNeg = PolyNeg(q);
  Poly result = PolyAdd(p, &qNeg);
  PolyDestroy(&qNeg);

  return result;
}

poly_exp_t PolyDegBy(const Poly *p, size_t var_idx) {
  if (PolyIsCoeff(p)) { return var_idx == 0 ? 0 : -1; }
  else if (var_idx == 0) { return listNextMono(p->list)->exp; }
  else {
    assert(p->list);

    poly_exp_t maxDeg = 0;
    poly_exp_t newDeg;
    MonoList *iter = p->list;
    while(iter->next) {
      newDeg = PolyDegBy(&(listNextMono(iter)->p), var_idx - 1);
      if (newDeg > maxDeg) {  maxDeg = newDeg; }

      iter = iter->next;
    }

    return maxDeg;
  }
}

poly_exp_t PolyDeg(const Poly *p) {
  if (PolyIsZero(p)) { return -1; }
  else if (PolyIsCoeff(p)) { return 0; }
  else {
    assert(p->list);

    poly_exp_t maxDeg = 0;
    poly_exp_t newDeg;
    Mono *mono;
    MonoList *iter = p->list;
    while (iter->next) {
      mono = listNextMono(iter);
      newDeg = mono->exp;
      newDeg += PolyDeg(&(mono->p));

      if (newDeg > maxDeg) {  maxDeg = newDeg; }

      iter = iter->next;
    }

    return maxDeg;
  }
}

bool PolyIsEq(const Poly *p, const Poly *q) {
  bool pIsCoeff = PolyIsCoeff(p);
  bool qIsCoeff = PolyIsCoeff(q);

  if (pIsCoeff && qIsCoeff) { return p->coeff == q->coeff; }
  else if (pIsCoeff || qIsCoeff) { return false; }
  else {
    assert(p->list && q->list);

    MonoList *pIter = p->list;
    MonoList *qIter = q->list;
    while (pIter->next && qIter->next) {
      if (!MonoIsEq(listNextMono(pIter), listNextMono(qIter))) { return false; }

      pIter = pIter->next;
      qIter = qIter->next;
    }
    if (pIter->next || qIter->next) { return false; }

    return true;
  }

}
