#include "poly.h"
#include "list.h"

void PolyDestroy(Poly *p) {
  listIter(p->list, &MonoDestroy);

  listFree(p->list);
}