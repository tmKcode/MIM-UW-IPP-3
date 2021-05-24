/** @file
  Interfejs klasy parsera komend kalkulatora dla wielomianów rzadkich wielu
  zmiennych

  @authors Tomasz Ziębowicz <t.ziebowicz@students.mimuw.edu.pl>
  @copyright Uniwersytet Warszawski
  @date 2021
*/

#ifndef PARSER_H
#define PARSER_H

#include "poly.h"

#include <aio.h>
#include <stdbool.h>
#include <string.h>

#define ASCII_L_LB 97
#define ASCII_L_UB 122
#define ASCII_U_LB 65
#define ASCII_U_UB 90
#define DEG_BY_STRLEN 6
#define AT_STRLEN 2

static inline bool LineIsIgnorable(char *line, ssize_t length) {
  return length == 0 || line[0] == '#' || (length = 1 && line[0] == '\n');
}

static inline bool CharIsLetter(char c) {
  return ((int)c >= ASCII_L_LB && (int)c <= ASCII_L_UB) ||
         ((int)c >= ASCII_U_LB && (int)c <= ASCII_U_UB);
}

void LineNormalize(char *line);

static inline bool LineIsCommand(char *line, ssize_t length) {
  return CharIsLetter(line[0]);
}

static inline bool LineIsPoly(char *line, ssize_t length) {
  return !CharIsLetter(line[0]);
}

static inline bool CommandIsZero(char *command) {
  return strcmp(command, "ZERO") == 0;
}

static inline bool CommandIsIsCoeff(char *command) {
  return strcmp(command, "IS_COEFF") == 0;
}

static inline bool CommandIsIsZero(char *command) {
  return strcmp(command, "IS_ZERO") == 0;
}

static inline bool CommandIsClone(char *command) {
  return strcmp(command, "CLONE") == 0;
}

static inline bool CommandIsAdd(char *command) {
  return strcmp(command, "ADD") == 0;
}

static inline bool CommandIsMul(char *command) {
  return strcmp(command, "MUL") == 0;
}

static inline bool CommandIsNeg(char *command) {
  return strcmp(command, "NEG") == 0;
}

static inline bool CommandIsSub(char *command) {
  return strcmp(command, "SUB") == 0;
}

static inline bool CommandIsIsEq(char *command) {
  return strcmp(command, "IS_EQ") == 0;
}

static inline bool CommandIsDeg(char *command) {
  return strcmp(command, "DEG") == 0;
}

static inline bool CommandIsDegBy(char *command) {
  return strncmp(command, "DEG_BY", DEG_BY_STRLEN) == 0;
}

static inline bool CommandIsAt(char *command) {
  return strncmp(command, "AT", AT_STRLEN) == 0;
}

static inline bool CommandIsPrint(char *command) {
  return strcmp(command, "PRINT") == 0;
}

static inline bool CommandIsPop(char *command) {
  return strcmp(command, "POP") == 0;
}

static inline bool CommandIsOneArg(char *command) {
  return CommandIsIsCoeff(command) || CommandIsIsZero(command) ||
         CommandIsClone(command) || CommandIsNeg(command) ||
         CommandIsDeg(command) || CommandIsDegBy(command) ||
         CommandIsAt(command) || CommandIsPop(command) ||
         CommandIsPrint(command);
}

static inline bool CommandIsTwoArg(char *command) {
  return CommandIsAdd(command) || CommandIsMul(command) ||
         CommandIsSub(command) || CommandIsIsEq(command);
}

size_t CommandDegByIdx(char *command, ssize_t length, bool *error);

poly_coeff_t CommandAtX(char *command, ssize_t length, bool *error);

bool PolyParse(char *src, char **endptr, Poly *p);

#endif // PARSER_H
