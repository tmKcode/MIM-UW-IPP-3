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