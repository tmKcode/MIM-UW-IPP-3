#include "poly.h"
#include "monolist.h"

void PolyDestroy(Poly *p) {
  if (!PolyIsCoeff(p)) {
    assert(p->list->next != NULL);

    MonoList *iter = p->list;
    while (iter->next != NULL) {
      MonoDestroy(&(iter->next->content));
    }

    listFree(p->list);
  }
}

Poly PolyClone(const Poly *p) {
  if (PolyIsCoeff(p)) {
    return (Poly){.coeff = p->coeff, .list = NULL};
  }
  else {
    assert(p->list->next != NULL);

    return (Poly){.coeff = p->coeff, .list = listClone(p->list)};
  }
}

static void PolyPlusEqual(Poly *p, const Poly *q);

static void PolyMerge(Poly *p, const Poly *q) {
  assert(p->list != NULL && q->list != NULL);

  MonoList *pIter = p->list;
  MonoList *qIter = q->list;
  while (pIter->next != NULL && qIter->next != NULL) {
    if (listNextContent(pIter).exp < listNextContent(qIter).exp) {
      pIter = pIter->next;
    }
    else if (listNextContent(pIter).exp > listNextContent(qIter).exp) {
      listInsertNext(pIter, MonoClone(&(qIter->content)));
      qIter = qIter->next;
    }
    else {
      PolyPlusEqual(&(pIter->next->content.p), &(qIter->next->content.p));
    }
  }
  if (qIter->next != NULL) {
    pIter->next = qIter->next;
  }
}