/** @file
  Implementacja klasy kalkulatora dla wielomianów rzadkich wielu
  zmiennych

  @authors Tomasz Ziębowicz <t.ziebowicz@students.mimuw.edu.pl>
  @copyright Uniwersytet Warszawski
  @date 2021
*/

#include "poly_calc.h"
#include "parser.h"
#include "stack.h"

#include <errno.h>
#include <stdio.h>

static void checkMemoryError(int errnum) {
  if (errnum == ENOMEM)
    exit(1);
}

static void PrintError(size_t idx, char *errorName) {
  printf("ERROR %zu %s\n", idx, errorName);
}

static void PrintPoly(Poly *p);

static void PrintMono(Mono *m) {
  printf("(");
  PrintPoly(&m->p);
  printf(",%d)", m->exp);
}

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

static void ProcessOneArgCommand(char *line, ssize_t lineLength,
                                 PolyStack *stack, size_t i) {
  if (CommandIsZero(line)) {
    PolyStackPush(&stack, PolyZero());
  } else if (CommandIsPop(line)) {
    PolyStackPop(&stack);
  } else {
    Poly top = PolyStackPeek(&stack);

    if (CommandIsIsCoeff(line)) {
      printf("%d\n", PolyIsCoeff(&top));
    } else if (CommandIsIsZero(line)) {
      printf("%d\n", PolyIsZero(&top));
    } else if (CommandIsClone(line)) {
      PolyStackPush(&stack, PolyClone(&top));
    } else if (CommandIsNeg(line)) {
      Poly negTop = PolyNeg(&top);

      PolyStackPop(&stack);
      PolyStackPush(&stack, negTop);
    } else if (CommandIsDeg(line)) {
      printf("%d\n", PolyDeg(&top));
    } else if (CommandIsDegBy(line)) {
      bool error;
      size_t idx = CommandDegByIdx(line, lineLength, &error);

      if (error) {
        PrintError(i, "DEG BY WRONG VARIABLE");
      } else {
        printf("%d\n", PolyDegBy(&top, idx));
      }
    } else if (CommandIsAt(line)) {
      bool error;
      poly_coeff_t x = CommandAtX(line, lineLength, &error);

      if (error) {
        PrintError(i, "AT WRONG VALUE");
      } else {
        Poly res = PolyAt(&top, x);

        PolyStackPop(&stack);
        PolyStackPush(&stack, res);
      }
    } else if (CommandIsPrint(line)) {
      PrintPoly(&top);
    }
  }
}

static void ProcessTwoArgCommand(char *line, PolyStack *stack) {
  Poly top = PolyStackPeek(stack);
  Poly subTop = PolyStackPeekSecond(stack);

  if (CommandIsIsEq(line)) {
    printf("%d\n", PolyIsEq(&top, &subTop));
  } else {
    Poly res;

    if (CommandIsAdd(line)) {
      res = PolyAdd(&top, &subTop);
    } else if (CommandIsMul(line)) {
      res = PolyMul(&top, &subTop);
    } else if (CommandIsSub(line)) {
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
    if (LineIsCommand(newLine, lineLength)) {
      if (CommandIsOneArg(newLine)) {
        if (PolyStackIsEmpty(&stack)) {
          PrintError(i, "STACK UNDERFLOW");
        } else {
          ProcessOneArgCommand(newLine, lineLength, &stack, i);
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
    } else if (LineIsPoly(newLine, lineLength)) {
    }

    free(newLine);
    bufferLength = 0;

    errno = 0;
    lineLength = getline(&newLine, &bufferLength, stdin);

    errnum = errno;
    checkMemoryError(errnum);
  }
}
