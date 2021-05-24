#define _GNU_SOURCE

#include "parser.h"

#include <stdio.h>

int main(){
  char *line = NULL;
  size_t n = 0;

  ssize_t length = getline(&line, &n, stdin);

  printf(CommandIsAdd(line) ? "ADD\n" : "NOT ADD\n");
  printf(CommandIsMul(line) ? "MUL\n" : "NOT MUL\n");
  printf(CommandIsSub(line) ? "SUB\n" : "NOT SUB\n");
  printf(CommandIsDeg(line) ? "DEG\n" : "NOT DEG\n");
  printf(CommandIsDegBy(line) ? "DEGBY\n" : "NOT DEGBY\n");
  printf(CommandIsZero(line) ? "ZERO\n" : "NOT ZERO\n");
  printf(CommandIsClone(line) ? "CLONE\n" : "NOT CLONE\n");
  printf(CommandIsIsEq(line) ? "ISEQ\n" : "NOT ISEQ\n");
  printf(CommandIsPrint(line) ? "PRINT\n" : "NOT PRINT\n");
  printf(CommandIsNeg(line) ? "NEG\n" : "NOT NEG\n");
  printf(CommandIsAt(line) ? "AT\n" : "NOT AT\n");
  printf(CommandIsIsZero(line) ? "ISZERO\n" : "NOT ISZERO\n");
  printf(CommandIsIsCoeff(line) ? "ISCOEFF\n" : "NOT ISCOEFF\n");
  printf(CommandIsPop(line) ? "POP\n" : "NOT POP\n");

}
