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
  long count = 0;
  long stack[MAX_STACK];
  
  // Values
  long val;
  long left;
  long right;

  // Operants
  int op;

  // Find the first valid token
  s = skipws(s);

  while (s) { // not EOF
    // printf("\ns: %c ", s[0]);

    int token = tokenType(s);
    if (token == TOK_INT) {
      s = consumeInt(s, &val);
    } else if (token == TOK_OP) {
      // Check the length
      if (count < 2) {
        fatalError("insufficient arguments");
      }
      right = stackPop(stack, &count);
      left = stackPop(stack, &count);

      s = consumeOp(s, &op);
      val = evalOp(op, left, right);

    } else { // unknown
      fatalError("unrecognized character");
    }

    // Need to always push something back
    stackPush(stack, &count, val); // need to pass address for count
    s = skipws(s); // find next valid token
  }

// printf("c: %li\n", count);
if (count != 1) {
  fatalError("string cannot be computed");
}

return stackPop(stack, &count);

}
