/** @file
  Implementacja klasy parsera komend kalkulatora dla wielomianów rzadkich wielu
  zmiennych

  @authors Tomasz Ziębowicz <t.ziebowicz@students.mimuw.edu.pl>
  @copyright Uniwersytet Warszawski
  @date 2021
*/

#include "parser.h"
#include "poly.h"

#include <argp.h>
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
      (command[AT_STRLEN + 1] != '-' &&
       !CharIsDigit(command[AT_STRLEN + 1]))) {
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

static bool CoeffParse(const char *src, char **endptr, Poly *p) {
  errno = 0;
  poly_coeff_t c = strtol(src, endptr, 10);
  int errnum = errno;

  if (errnum == EINVAL || errnum == ERANGE) {
    return false;
  }

  *p = PolyFromCoeff(c);

  return true;
}

static bool ExpParse(const char *src, char **endptr, poly_exp_t *exp) {
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

static bool MonoParse(char *src, char **endptr, Mono *m) {
  if (src[0] != '(') {
    return false;
  }

  src++;
  Poly p;

  if (!PolyParse(src, endptr, &p) || **endptr != ',' || *(*endptr + 1) ==')') {
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

static bool PolyParseMonos(char *src, char **endptr, Poly *p) {
  MonoList *head = NewMonoList();
  Mono m;

  if(!MonoParse(src, endptr, &m)) {
    Poly util = (Poly){.list = head};
    PolyDestroy(&util);

    return false;
  }

  MonoListInsert(head, &m);
  while(**endptr == '+') {
    src = *endptr;
    src++;

    if (!MonoParse(src, endptr, &m)) {
      Poly util = (Poly){.list = head};
      PolyDestroy(&util);

      return false;
    }

    MonoListInsert(head, &m);
  }

  if (**endptr != '\0' && **endptr != ',') {
    Poly util = (Poly){.list = head};
    PolyDestroy(&util);

    return false;
  }

  *p = (Poly){.list = head};

  return true;
}

bool PolyParse(char *src, char **endptr, Poly *p) {
  if (src[0] == '(') {
//    printf("PARSE MONOS!\n");

    return PolyParseMonos(src, endptr, p);
  }

  if (src[0] == '-' || CharIsDigit(src[0])) {
    return CoeffParse(src, endptr, p);
  }

  return false;
}

// static Mono MonoParse(char **ptr, ssize_t *length, bool *error);
//
// static Poly PolyParseMonos(char *src, char **end, bool *error) {
//  MonoList *head = NewMonoList();
//
//  do {
//    Mono m =
//
//    MonoListInsert(head, &m);
//  }
//}
//
// static Mono MonoParseError(bool *error) {
//  *error = true;
//
//  Poly zero = PolyZero();
//  return MonoFromPoly(&zero, 0);
//}
//
// Poly PolyParse(char *src, char **end, bool *error);
//
// static Mono MonoParse(char **ptr, ssize_t *length, bool *error) {
//  if (*error || *length == 0 || *ptr[0] != '(') {
//    return MonoParseError(error);
//  }
//
//  (*ptr)++;
//  (*length)--;
//  Poly p = PolyParse(ptr, length, error);
//
//  if (*error || *length < 2 || *ptr[0] != ',' || !CharIsDigit(*ptr[1])) {
//    return MonoParseError(error);
//  }
//
//  (*ptr)++;
//  (*length)--;
//
//  char *endptr;
//  errno = 0;
//  long expLong = strtol(*ptr, &endptr, 10);
//  int errnum = errno;
//
//  if (errnum == EINVAL || errnum == ERANGE) {
//    MonoParseError(error);
//  }
//}
//
// static Poly PolyParseError(bool *error) {
//  *error = true;
//
//  return PolyZero();
//}
//
// Poly PolyParse(char *src, char **endptr, bool *error) {
//  if (*error || src[0] == '\0' ||
//      (!CharIsDigit(src[0]) && src[0] != '-' && src[0] != '(')) {
//    return PolyParseError(error);
//  }
//
//  if (src[0] == '(') {
//    return PolyParseMonos(src, endptr, error);
//  }
//
//  errno = 0;
//  poly_coeff_t c = strtol(src + 1, endptr, 10);
//  int errnr = errno;
//
//  if (errnr == ERANGE || errnr == EINVAL || **endptr != '\0') {
//    return PolyParseError(error);
//  }
//
//  return PolyFromCoeff(c);
//}
