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
#include <ctype.h>
#include <stdbool.h>
#include <string.h>

#define ASCII_L_LB 97
#define ASCII_L_UB 122
#define ASCII_U_LB 65
#define ASCII_U_UB 90
#define DEG_BY_STRLEN 6
#define AT_STRLEN 2

/**
 * Sprawdza, czy linię tekstu należy zignorować (wg składni kalkulatora).
 * @param[in] line : linia tekstu
 * @param[in] length : długość linii
 * @return Czy linię tekstu należy zignorować?
 */
static inline bool LineIsIgnorable(char *line, ssize_t length) {
  return length == 0 || line[0] == '#' || (length = 1 && line[0] == '\n');
}

/**
 * Sprawdza, czy znak jest literą alfabetu angielskiego.
 * @param[in] c : znak
 * @return Czy znak jest literą alfabetu angielskiego?
 */
static inline bool CharIsLetter(char c) {
  return ((int)c >= ASCII_L_LB && (int)c <= ASCII_L_UB) ||
         ((int)c >= ASCII_U_LB && (int)c <= ASCII_U_UB);
}

/**
 * Zamienia pierwszy napotkany znak nowej lini na nullbyte.
 * @param[in] line : linia tekstu
 */
void LineNormalize(char *line);

/**
 * Sprawdza, czy linia tekstu jest komendą (wg składni kalkulatora).
 * @param[in] line : linia tekstu
 * @return Czy linię tekstu jest komendą?
 */
static inline bool LineIsCommand(char *line) { return CharIsLetter(line[0]); }

/**
 * Sprawdza, czy linia tekstu jest komendą "ZERO" (wg składni kalkulatora).
 * @param[in] line : linia tekstu
 * @return Czy linię tekstu jest komendą "ZERO"?
 */
static inline bool CommandIsZero(char *command) {
  return strcmp(command, "ZERO") == 0;
}

/**
 * Sprawdza, czy linia tekstu jest komendą "IS_COEFF" (wg składni kalkulatora).
 * @param[in] line : linia tekstu
 * @return Czy linię tekstu jest komendą "IS_COEFF"?
 */
static inline bool CommandIsIsCoeff(char *command) {
  return strcmp(command, "IS_COEFF") == 0;
}

/**
 * Sprawdza, czy linia tekstu jest komendą "IS_ZERO" (wg składni kalkulatora).
 * @param[in] line : linia tekstu
 * @return Czy linię tekstu jest komendą "IS_ZERO"?
 */
static inline bool CommandIsIsZero(char *command) {
  return strcmp(command, "IS_ZERO") == 0;
}

/**
 * Sprawdza, czy linia tekstu jest komendą "CLONE" (wg składni kalkulatora).
 * @param[in] line : linia tekstu
 * @return Czy linię tekstu jest komendą "CLONE"?
 */
static inline bool CommandIsClone(char *command) {
  return strcmp(command, "CLONE") == 0;
}

/**
 * Sprawdza, czy linia tekstu jest komendą "ADD" (wg składni kalkulatora).
 * @param[in] line : linia tekstu
 * @return Czy linię tekstu jest komendą "ADD"?
 */
static inline bool CommandIsAdd(char *command) {
  return strcmp(command, "ADD") == 0;
}

/**
 * Sprawdza, czy linia tekstu jest komendą "MUL" (wg składni kalkulatora).
 * @param[in] line : linia tekstu
 * @return Czy linię tekstu jest komendą "MUL"?
 */
static inline bool CommandIsMul(char *command) {
  return strcmp(command, "MUL") == 0;
}

/**
 * Sprawdza, czy linia tekstu jest komendą "NEG" (wg składni kalkulatora).
 * @param[in] line : linia tekstu
 * @return Czy linię tekstu jest komendą "NEG"?
 */
static inline bool CommandIsNeg(char *command) {
  return strcmp(command, "NEG") == 0;
}

/**
 * Sprawdza, czy linia tekstu jest komendą "SUB" (wg składni kalkulatora).
 * @param[in] line : linia tekstu
 * @return Czy linię tekstu jest komendą "SUB"?
 */
static inline bool CommandIsSub(char *command) {
  return strcmp(command, "SUB") == 0;
}

/**
 * Sprawdza, czy linia tekstu jest komendą "IS_EQ" (wg składni kalkulatora).
 * @param[in] line : linia tekstu
 * @return Czy linię tekstu jest komendą "IS_EQ"?
 */
static inline bool CommandIsIsEq(char *command) {
  return strcmp(command, "IS_EQ") == 0;
}

/**
 * Sprawdza, czy linia tekstu jest komendą "DEG" (wg składni kalkulatora).
 * @param[in] line : linia tekstu
 * @return Czy linię tekstu jest komendą "DEG"?
 */
static inline bool CommandIsDeg(char *command) {
  return strcmp(command, "DEG") == 0;
}

/**
 * Sprawdza, czy linia tekstu jest komendą "DEG_BY" (wg składni kalkulatora).
 * @param[in] line : linia tekstu
 * @return Czy linię tekstu jest komendą "DEG_BY"?
 */
static inline bool CommandIsDegBy(char *command) {
  return (strncmp(command, "DEG_BY", DEG_BY_STRLEN) == 0 &&
          (command[DEG_BY_STRLEN] == '\0' || isspace(command[DEG_BY_STRLEN])));
}

/**
 * Sprawdza, czy linia tekstu jest komendą "AT" (wg składni kalkulatora).
 * @param[in] line : linia tekstu
 * @return Czy linię tekstu jest komendą "AT"?
 */
static inline bool CommandIsAt(char *command) {
  return (strncmp(command, "AT", AT_STRLEN) == 0 &&
          (command[AT_STRLEN] == '\0' || isspace(command[AT_STRLEN])));
}

/**
 * Sprawdza, czy linia tekstu jest komendą "PRINT" (wg składni kalkulatora).
 * @param[in] line : linia tekstu
 * @return Czy linię tekstu jest komendą "PRINT"?
 */
static inline bool CommandIsPrint(char *command) {
  return strcmp(command, "PRINT") == 0;
}

/**
 * Sprawdza, czy linia tekstu jest komendą "POP" (wg składni kalkulatora).
 * @param[in] line : linia tekstu
 * @return Czy linię tekstu jest komendą "POP"?
 */
static inline bool CommandIsPop(char *command) {
  return strcmp(command, "POP") == 0;
}

/**
 * Sprawdza, czy linia tekstu jest komendą jednoargumentową (wg składni
 * kalkulatora).
 * @param[in] line : linia tekstu
 * @return Czy linię tekstu jest komendą jednoargumentową?
 */
static inline bool CommandIsOneArg(char *command) {
  return CommandIsIsCoeff(command) || CommandIsIsZero(command) ||
         CommandIsClone(command) || CommandIsNeg(command) ||
         CommandIsDeg(command) || CommandIsDegBy(command) ||
         CommandIsAt(command) || CommandIsPop(command) ||
         CommandIsPrint(command);
}

/**
 * Sprawdza, czy linia tekstu jest komendą dwuargumentową (wg składni
 * kalkulatora).
 * @param[in] line : linia tekstu
 * @return Czy linię tekstu jest komendą dwuargumentową?
 */
static inline bool CommandIsTwoArg(char *command) {
  return CommandIsAdd(command) || CommandIsMul(command) ||
         CommandIsSub(command) || CommandIsIsEq(command);
}

/**
 * Zwraca indeks zmiennej będący argumentem komendy "DEG_BY" (wg składni
 * kalkulatora).
 * Jeśli parsowanie komendy się nie powiedzie, error wskazuje na false.
 * W p.p. error wskazuje na true.
 * @param[in] command : komenda "DEG_BY"
 * @param[in] length : długość komendy
 * @param[in] error : flaga błędu
 * @return Indeks zmiennej będący argumentem komendy "DEG_BY".
 */
size_t CommandDegByIdx(char *command, ssize_t length, bool *error);

/**
 * Zwraca wartość zmiennej będącej argumentem komendy "AT" (wg składni
 * kalkulatora).
 * Jeśli parsowanie komendy się nie powiedzie, error wskazuje na false.
 * W p.p. error wskazuje na true.
 * @param[in] command : komenda "AT"
 * @param[in] length : długość komendy
 * @param[in] error : flaga błędu
 * @return Wartość zmiennej będącej argumentem komendy "AT".
 */
poly_coeff_t CommandAtX(char *command, ssize_t length, bool *error);

/**
 * Parsuje wielomian - komendę "POLY" (wg składni kalkulatora).
 * Jeśli parsowanie komendy się nie powiedzie, funkcja zwraca false.
 * W p.p. zapisuje sparsowany wielomian pod wskaźnik p, wskazuje wartość
 * wskaźnika endptr na miejsce zakończenia parsowania i zwraca true;
 * @param[in] src : komenda "POLY"
 * @param[in] endptr : wskaźnik na ciąg znaków
 * @param[in] p : wskaźnik na wielomian
 * @return Czy parsowanie się powiodło?
 */
bool PolyParse(char *src, char **endptr, Poly *p);

#endif // PARSER_H
