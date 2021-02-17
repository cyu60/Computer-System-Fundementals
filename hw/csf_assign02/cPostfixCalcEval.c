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

  while (s[0] != '\0' && s != NULL) { // not EOF


    s = skipws(s);
    printf("\ns: %c ", s[0]);
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
      left = stackPop(stack, &count);
      s = consumeOp(s, &op);
      // s[0] is the op?
      val = evalOp(op, left, right);
    } else { // unknown
      fatalError("unrecognized character");
    }
    // Need to always push something back
    stackPush(stack, &count, val); // need to pass address for count
  }

printf("c: %li\n", count);
if (count != 1) {
  fatalError("string cannot be computed");
}

return stackPop(stack, &count);

}
