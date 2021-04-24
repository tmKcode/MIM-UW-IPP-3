#include "poly.h"
#include "list.h"

void PolyDestroy(Poly *p) {
  listFree(p->list);
}