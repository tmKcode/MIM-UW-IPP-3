/** @file
  Implementacja klasy parsera komend kalkulatora dla wielomianów rzadkich wielu
  zmiennych

  @authors Tomasz Ziębowicz <t.ziebowicz@students.mimuw.edu.pl>
  @copyright Uniwersytet Warszawski
  @date 2021
*/

#include "parser.h"
#include "poly.h"

#include <errno.h>
#include <limits.h>
#include <stdlib.h>

#define ASCII_D_LB 48
#define ASCII_D_UB 57

bool CharIsDigit(char c) {
  return (int)c >= ASCII_D_LB && (int)c <= ASCII_D_UB;
}

void LineNormalize(char *line) {
  for (int i = 0; line[i] != '\0'; ++i) {
    if (line[i] == '\n') {
      line[i] = '\0';

      return;
    }
  }
}

size_t CommandDegByIdx(char *command, ssize_t length, bool *error) {
  if (length < DEG_BY_STRLEN + 2 || command[DEG_BY_STRLEN] != ' ' ||
      !CharIsDigit(command[DEG_BY_STRLEN + 1])) {
    *error = true;

    return 0;
  }

  char *idxString = command + DEG_BY_STRLEN + 1;
  char *endptr;
  int errnum;

  errno = 0;
  size_t idx = strtoul(idxString, &endptr, 10);
  errnum = errno;

  if (errnum == EINVAL || errnum == ERANGE || *endptr != '\0') {
    *error = true;

    return 0;
  }

  *error = false;

  return idx;
}

poly_coeff_t CommandAtX(char *command, ssize_t length, bool *error) {
  if (length < AT_STRLEN + 2 || command[AT_STRLEN] != ' ' ||
      (command[AT_STRLEN + 1] != '-' && !CharIsDigit(command[AT_STRLEN + 1]))) {
    *error = true;

    return 0;
  }

  char *xString = command + AT_STRLEN + 1;
  char *endptr;
  int errnum;

  errno = 0;
  poly_coeff_t x = strtol(xString, &endptr, 10);
  errnum = errno;

  if (errnum == EINVAL || errnum == ERANGE || *endptr != '\0') {
    *error = true;

    return 0;
  }

  *error = false;

  return x;
}

/**
 * Parsuje wielomian będący współczynnikiem (wg składni kalkulatora).
 * Jeśli parsowanie komendy się nie powiedzie, funkcja zwraca false.
 * W p.p. zapisuje sparsowany wielomian pod wskaźnik p, wskazuje wartość
 * wskaźnika endptr na miejsce zakończenia parsowania i zwraca true.
 * @param[in] src : wskaźnik na początek wielomianu
 * @param[in] endptr : wskaźnik na ciąg znaków
 * @param[in] p : wskaźnik na wielomian
 * @return Czy parsowanie się powiodło?
 */
static bool CoeffParse(const char *src, char **endptr, Poly *p) {
  if (!CharIsDigit(src[0]) && src[0] != '-') {
    return false;
  }

  errno = 0;
  poly_coeff_t c = strtol(src, endptr, 10);
  int errnum = errno;

  if (errnum == EINVAL || errnum == ERANGE) {
    return false;
  }

  *p = PolyFromCoeff(c);

  return true;
}

/**
 * Parsuje wykładnik jednomianu (wg składni kalkulatora).
 * Jeśli parsowanie komendy się nie powiedzie, funkcja zwraca false.
 * W p.p. zapisuje sparsowany wykładnik pod wskaźnik exp, wskazuje wartość
 * wskaźnika endptr na miejsce zakończenia parsowania i zwraca true.
 * @param[in] src : wskaźnik na początek wykładnika
 * @param[in] endptr : wskaźnik na ciąg znaków
 * @param[in] exp : wskaźnik na wykładnik
 * @return Czy parsowanie się powiodło?
 */
static bool ExpParse(const char *src, char **endptr, poly_exp_t *exp) {
  if (!CharIsDigit(src[0])) {
    return false;
  }

  errno = 0;
  long expLong = strtol(src, endptr, 10);
  int errnum = errno;

  if (errnum == EINVAL || errnum == ERANGE || expLong > INT_MAX ||
      expLong < 0) {
    return false;
  }

  *exp = expLong;

  return true;
}

bool PolyParse(char *src, char **endptr, Poly *p);

/**
 * Parsuje jednomian (wg składni kalkulatora).
 * Jeśli parsowanie komendy się nie powiedzie, funkcja zwraca false.
 * W p.p. zapisuje sparsowany jednomian pod wskaźnik m, wskazuje wartość
 * wskaźnika endptr na miejsce zakończenia parsowania i zwraca true.
 * @param[in] src : wskaźnik na początek jednomianu
 * @param[in] endptr : wskaźnik na ciąg znaków
 * @param[in] m : wskaźnik na jednomian
 * @return Czy parsowanie się powiodło?
 */
static bool MonoParse(char *src, char **endptr, Mono *m) {
  if (src[0] != '(') {
    return false;
  }

  src++;
  Poly p;

  if (!PolyParse(src, endptr, &p)) {
    return false;
  }

  if (**endptr != ',' || *(*endptr + 1) == ')') {
    PolyDestroy(&p);

    return false;
  }

  src = *endptr;
  src++;

  poly_exp_t exp;

  if (!ExpParse(src, endptr, &exp) || **endptr != ')') {
    PolyDestroy(&p);

    return false;
  }

  *m = MonoFromPoly(&p, exp);
  *endptr = *endptr + 1;

  return true;
}

/**
 * Zwalnia pamięć zajętą w wyniku stworzenia listy lednomianów podczas
 * parsowania wielomianu.
 * @param[in] head : pierwszy węzeł listy
 */
static void PolyParseListDestroy(MonoList *head) {
  if (!head) {
    return;
  }

  MonoList *iter = head;
  while (iter->next) {
    MonoDestroy(MonoListNextMono(iter));

    iter = iter->next;
  }

  MonoListFree(head);
}

/**
 * Parsuje wielomian będący sumą jednomianów (wg składni kalkulatora).
 * Jeśli parsowanie komendy się nie powiedzie, funkcja zwraca false.
 * W p.p. zapisuje sparsowany wielomian pod wskaźnik p, wskazuje wartość
 * wskaźnika endptr na miejsce zakończenia parsowania i zwraca true.
 * @param[in] src : wskaźnik na początek wielomianu
 * @param[in] endptr : wskaźnik na ciąg znaków
 * @param[in] p : wskaźnik na wielomian
 * @return Czy parsowanie się powiodło?
 */
static bool PolyParseMonos(char *src, char **endptr, Poly *p) {
  MonoList *head = NewMonoList();
  Mono m;

  if (!MonoParse(src, endptr, &m)) {
    PolyParseListDestroy(head);

    return false;
  }

  MonoListInsert(head, &m);
  while (**endptr == '+') {
    src = *endptr;
    src++;

    if (!MonoParse(src, endptr, &m)) {
      PolyParseListDestroy(head);

      return false;
    }

    MonoListInsert(head, &m);
  }

  if (**endptr != '\0' && **endptr != ',') {
    PolyParseListDestroy(head);

    return false;
  }

  *p = (Poly){.list = head};

  return true;
}

bool PolyParse(char *src, char **endptr, Poly *p) {
  if (src[0] == '(') {
    return PolyParseMonos(src, endptr, p);
  }

  if (src[0] == '-' || CharIsDigit(src[0])) {
    return CoeffParse(src, endptr, p);
  }

  return false;
}
