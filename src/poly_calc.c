/** @file
  Implementacja klasy kalkulatora dla wielomianów rzadkich wielu
  zmiennych

  @authors Tomasz Ziębowicz <t.ziebowicz@students.mimuw.edu.pl>
  @copyright Uniwersytet Warszawski
  @date 2021
*/

#include "poly_calc.h"
#include "parser.h"
#include "poly.h"
#include "stack.h"

#include <errno.h>
#include <stdio.h>

/**
 * Jeśli errnum to kod ENOMEM, funkcja awaryjnie kończy działanie programu z
 * kodem 1.
 * @param[in] errnum : kod błędu
 */
static void checkMemoryError(int errnum) {
  if (errnum == ENOMEM)
    exit(1);
}

/**
 * Wyświetla komunikat o błędzie o indeksie idx, nazwie errorName na stderr.
 * @param[in] idx : indeks
 * @param[in] errorName : nazwa błędu
 */
static void PrintError(size_t idx, char *errorName) {
  fprintf(stderr, "ERROR %zu %s\n", idx, errorName);
}

static void PrintPoly(Poly *p);

/**
 * Wyświetla jednomian m na stdout.
 * @param[in] m : jednomian
 */
static void PrintMono(Mono *m) {
  printf("(");
  PrintPoly(&m->p);
  printf(",%d)", m->exp);
}

/**
 * Wyświetla wielomian p na stdout.
 * @param[in] p : wielomian
 */
static void PrintPoly(Poly *p) {
  if (PolyIsCoeff(p)) {
    printf("%ld", p->coeff);
  } else {
    PrintMono(MonoListNextMono(p->list));

    MonoList *iter = p->list->next;
    while (iter->next) {
      printf("+");
      PrintMono(MonoListNextMono(iter));

      iter = iter->next;
    }
  }
}

/**
 * Przetwarza jednoargumentową komendę command korzystając ze stosu stack.
 * @param[in] command : komenda
 * @param[in] stack : stos
 */
static void ProcessOneArgCommand(char *command, PolyStack *stack) {
  if (CommandIsPop(command)) {
    PolyStackPop(stack);
  } else {
    Poly top = PolyStackPeek(stack);

    if (CommandIsIsCoeff(command)) {
      printf("%d\n", PolyIsCoeff(&top));
    } else if (CommandIsIsZero(command)) {
      printf("%d\n", PolyIsZero(&top));
    } else if (CommandIsClone(command)) {
      PolyStackPush(stack, PolyClone(&top));
    } else if (CommandIsNeg(command)) {
      Poly negTop = PolyNeg(&top);

      PolyStackPop(stack);
      PolyStackPush(stack, negTop);
    } else if (CommandIsDeg(command)) {
      printf("%d\n", PolyDeg(&top));
    } else if (CommandIsPrint(command)) {
      PrintPoly(&top);
      printf("\n");
    }
  }
}

/**
 * Przetwarza dwuargumentową komendę command korzystając ze stosu stack.
 * @param[in] command : komenda
 * @param[in] stack : stos
 */
static void ProcessTwoArgCommand(char *command, PolyStack *stack) {
  Poly top = PolyStackPeek(stack);
  Poly subTop = PolyStackPeekSecond(stack);

  if (CommandIsIsEq(command)) {
    printf("%d\n", PolyIsEq(&top, &subTop));
  } else {
    Poly res;

    if (CommandIsAdd(command)) {
      res = PolyAdd(&top, &subTop);
    } else if (CommandIsMul(command)) {
      res = PolyMul(&top, &subTop);
    } else if (CommandIsSub(command)) {
      res = PolySub(&top, &subTop);
    }

    PolyStackPop(stack);
    PolyStackPop(stack);
    PolyStackPush(stack, res);
  }
}

void CalcStart() {
  PolyStack stack = NewPolyStack();
  char *newLine = NULL;
  size_t bufferLength = 0;
  ssize_t lineLength;
  int errnum;

  errno = 0;
  lineLength = getline(&newLine, &bufferLength, stdin);

  errnum = errno;
  checkMemoryError(errnum);

  for (size_t i = 1; lineLength != EOF; i++) {
    if (!LineIsIgnorable(newLine, lineLength)) {
      LineNormalize(newLine);

      if (LineIsCommand(newLine)) {
        if (CommandIsZero(newLine)) {
          PolyStackPush(&stack, PolyZero());
        } else if (CommandIsDegBy(newLine)) {
          bool error;
          size_t idx = CommandDegByIdx(newLine, lineLength, &error);

          if (error) {
            PrintError(i, "DEG BY WRONG VARIABLE");
          } else if (PolyStackIsEmpty(&stack)) {
            PrintError(i, "STACK UNDERFLOW");
          } else {
            Poly top = PolyStackPeek(&stack);

            printf("%d\n", PolyDegBy(&top, idx));
          }
        } else if (CommandIsAt(newLine)) {
          bool error;
          poly_coeff_t x = CommandAtX(newLine, lineLength, &error);

          if (error) {
            PrintError(i, "AT WRONG VALUE");
          } else if (PolyStackIsEmpty(&stack)) {
            PrintError(i, "STACK UNDERFLOW");
          } else {
            Poly top = PolyStackPeek(&stack);
            Poly res = PolyAt(&top, x);

            PolyStackPop(&stack);
            PolyStackPush(&stack, res);
          }
        } else if (CommandIsOneArg(newLine)) {
          if (PolyStackIsEmpty(&stack)) {
            PrintError(i, "STACK UNDERFLOW");
          } else {
            ProcessOneArgCommand(newLine, &stack);
          }
        } else if (CommandIsTwoArg(newLine)) {
          if (PolyStackSize(&stack) < 2) {
            PrintError(i, "STACK UNDERFLOW");
          } else {
            ProcessTwoArgCommand(newLine, &stack);
          }
        } else {
          PrintError(i, "WRONG COMMAND");
        }
      } else {
        Poly p;
        char *endptr;

        if (!PolyParse(newLine, &endptr, &p)) {
          PrintError(i, "WRONG POLY");
        } else if (*endptr != '\0') {
          PolyNormalizeConstants(&p);
          PolyRemoveZeros(&p);
          PolyDestroy(&p);

          PrintError(i, "WRONG POLY");
        } else {
          PolyNormalizeConstants(&p);
          PolyRemoveZeros(&p);

          PolyStackPush(&stack, p);
        }
      }
    }

    errno = 0;
    lineLength = getline(&newLine, &bufferLength, stdin);

    errnum = errno;
    checkMemoryError(errnum);
  }

  free(newLine);
  PolyStackDestroy(&stack);
}
