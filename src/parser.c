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
#include <stdlib.h>

#define ASCII_D_LB 48
#define ASCII_D_UB 57

bool CharIsDigit(char c) {
  return (int)c >= ASCII_D_LB && (int)c <= ASCII_D_UB;
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

  if (errnum == EINVAL || errnum == ERANGE) {
    *error = true;

    return 0;
  }

  if (*endptr != '\n') {
    *error = true;

    return 0;
  }

  *error = false;

  return idx;
}

poly_coeff_t CommandAtX(char *command, ssize_t length, bool *error) {
  if (length < AT_STRLEN + 2 || command[AT_STRLEN] != ' ' ||
      (command[DEG_BY_STRLEN + 1] != '-' &&
       !CharIsDigit(command[DEG_BY_STRLEN + 1]))) {
    *error = true;

    return 0;
  }

  char *xString = command + DEG_BY_STRLEN + 1;
  char *endptr;
  int errnum;

  errno = 0;
  poly_coeff_t x = strtol(xString, &endptr, 10);
  errnum = errno;

  if (errnum == EINVAL || errnum == ERANGE) {
    *error = true;

    return 0;
  }

  if (*endptr != '\n') {
    *error = true;

    return 0;
  }

  *error = false;

  return x;
}

static Mono MonoParseError(bool *error) {
  *error = true;

  Poly zero = PolyZero();
  return MonoFromPoly(&zero, 0);
}

Poly PolyParse(char **ptr, ssize_t *length, bool *error);

static Mono MonoParse(char **ptr, ssize_t *length, bool *error) {
  if (*error || *length == 0 || *ptr[0] != '(') {
    return MonoParseError(error);
  }

  (*ptr)++;
  (*length)--;
  Poly p = PolyParse(ptr, length, error);

  if(*error || *length < 2 || *ptr[0] != ',' || !CharIsDigit(*ptr[1])) {
    return MonoParseError(error);
  }

  (*ptr)++;
  (*length)--;

  char *endptr;
  errno = 0;
  long expLong = strtol(*ptr, &endptr, 10);
  int errnum = errno;

  if (errnum == EINVAL || errnum == ERANGE) {
    MonoParseError(error);
  }
}

static Poly PolyParseError(bool *error) {
  *error = true;

  return PolyZero();
}

Poly PolyParse(char **ptr, ssize_t *length, bool *error) {
  if (*error || (!CharIsDigit(*ptr[0]) && *ptr[0] != '-')) {
    PolyParseError(error);
  }

  if (CharIsDigit(*ptr[0]) || *ptr[0] == '-') {
    char *endptr;
    int errnum;

    errno = 0;
    poly_coeff_t c = strtol(*ptr, &endptr, 10);
    errnum = errno;

    if (errnum == EINVAL || errnum == ERANGE) {
      PolyParseError(error);
    }

    if (*endptr != '\n') {
      PolyParseError(error);
    }

    return PolyFromCoeff(c);
  }

  if (*ptr[0] == '(') {
  }
}
