#include <stdio.h>
#include <stdlib.h>
#include "cPostfixCalc.h"

/*
 * eval - evaluate a postfix expression
 * Should directly or indirectly call fatalError if the expression is invalid.
 *
 * Params:
 *   s - C string containing a postfix expression
 *
 * Returns:
 *   the result of evaluating the expression
 */
long eval(const char *s) {
  /* TODO: implement */

  /* Note: this function should be implemented by calling functions
   * declared in cPostfixCalc.h and defined in cPostfixCalcFuncs.c
   */
  long count = 0;
  long stack[MAX_STACK];
  
  // Values
  long val;
  long left;
  long right;

  // Operants
  int op;

  while (s != NULL && s[0] != '\0') { // not EOF


    printf("\nall good");
    s = skipws(s);
    printf("\ns: %c ", s[0]);
    // printf("\nall good here ");
    int token = tokenType(s);
    if (token == TOK_INT) {
      s = consumeInt(s, &val);
    } else if (token == TOK_OP) {
      printf("c: %li\n", count);

      // Check the length
      if (count < 2) {
        fatalError("insufficient arguments");
      }
      right = stackPop(stack, &count);
      printf("r: %li\n", right);

      left = stackPop(stack, &count);
      printf("l: %li\n", left);

      s = consumeOp(s, &op);
      printf("op: %c\n",op);

      val = evalOp(op, left, right);
      printf("val: %li\n", val);
      printf("s[0]: %i\n",s[0]);

    } else { // unknown
      fatalError("unrecognized character");
    }
    

    // Need to always push something back
    stackPush(stack, &count, val); // need to pass address for count
    printf("\nall good to push");
    printf("\ns: %li", stack[0]);
  }

printf("c: %li\n", count);
if (count != 1) {
  fatalError("string cannot be computed");
}

long res = stackPop(stack, &count);
printf("res: %li\n", res);

return res;

}
