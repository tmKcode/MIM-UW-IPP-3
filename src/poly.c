#include "poly.h"
#include "list.h"

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

    MonoList *list = newMonoList();
    MonoList *iterSrc = p->list;
    MonoList *iterDest = list;
    while (iterSrc->next != NULL) {
      listInsertNext(iterDest, MonoClone(&listNextContent(iterSrc)));
      iterSrc = iterSrc->next;
      iterDest = iterDest->next;
    }

    return (Poly){.coeff = p->coeff, .list = list};
  }
}