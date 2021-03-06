#include <stdio.h>
#include <stdlib.h>
#include "cPostfixCalc.h"

/*
 * Example function (see tests in cTests.c)
 */
long addPositive(long a, long b) {
  if (a < 0L || b < 0L) {
    printf("Error: values must be positive\n");
    exit(1);
  }
  return a + b;
}
void letter_hist(const char *str, int *counts) {
  return;
}

int is_min_heap(int *arr, int len){
  int cur = 0;
  // while has children
  while (cur*2 + 1 < len) {
	  int curElement = arr[cur];
	//   printf("curEl: %d\n", curElement);
  // Check children element at position
	int left = arr[cur*2 + 1];
	if (left < curElement) {
		return 0;
	}
  // check right child
	if (cur*2 + 2 < len) {
		int right = arr[cur*2 + 2]; // can use same var as left?
		if (right < curElement) {
			// printf("right: %d at %d\n", right, cur*2 + 2);
		return 0;
		}
	}
	cur++;
//   // if not min heap
//   return 0;
  }
  return 1; // is a min heap
}
/*
 * fatalError - print an error message an exit with an exit code of 1.
 * Prints a message of the form "Error: <message text>" to standard output.
 * Does not return!
 *
 * Params:
 *   msg - description of the error which occurred
 */
void fatalError(const char *msg) {
  printf("Error: %s\n", msg);
  exit(1); // no return
}

/*
 * isSpace - tests a character code to determine whether or not it
 * is a space character.
 *
 * Params:
 *   c - an ASCII character code
 *
 * Returns:
 *   1 if c is a space or tab character, 0 otherwise
 *   (the function is allowed to also return 1 if c is a newline
 *   or carriage return, but it's only required to handle
 *   spaces and tabs)
 */
int isSpace(int c) {
  // 32 is space, 9 should be tab
  return c == 32 || c == 9;

}

/*
 * isDigit - tests a character code to determine whether it is one
 * of the digits '0' through '9'
 *
 * Params:
 *   c - an ASCII character code
 *
 * Returns:
 *   1 if c is a digit, 0 otherwise
 */
int isDigit(int c) {
  // 0 is 48, 9 is 57
  return c >= 48 && c <= 57;
}

/*
 * skipws - skips any whitespace characters (as determined by isSpace)
 * at the beginning of a C character string.
 *
 * Params:
 *   s - pointer to a C character string
 *
 * Returns:
 *   pointer to the first non-space character in the string, or NULL
 *   if there are no non-space characters before the end of the string
 *   is reached
 */
const char *skipws(const char *s) {
  while (isSpace(s[0])) {
    s+=1; // shift s by 1
  }
  return s[0] == '\0' ? NULL : s; // EOL no non-space
}

/*
 * tokenType - return the token type (TOK_INT, TOK_OP, or TOK_UNKNOWN)
 * of the token at the beginning of given C character string.
 *
 * Params:
 *   s - nonempty C character string (which should not have any leading
 *       space characters)
 *
 * Returns:
 *   the token type
 */
int tokenType(const char *s) {
  if (isDigit(s[0])) {
    return TOK_INT;
  } else if (s[0] == '*' || s[0] == '+' || s[0] == '-' || s[0] == '/') { // isOp
    return TOK_OP;
  }
  return TOK_UNKNOWN;
}

/*
 * consumeInt - consume one or more digit characters at the beginning
 * of given C character string, convert them to an integer value, and
 * store the integer value in the long (64 bit signed integer)
 * variable that pval points to.
 *
 * Params:
 *   s    - pointer to a nonempty C character string which should start
 *          with a digit character
 *   pval - points to a long variable where the converted integer value
 *          should be stored
 *
 * Returns:
 *   pointer to the first character in the string that is not a digit
 */
const char *consumeInt(const char *s, long *pval) {

  *pval = 0; // reset 0
  
  while (isDigit(s[0])) {
    *pval *= 10;
    *pval += s[0] - 48; // offset for ascii
    s++;
  }

  return s;
}

/*
 * consumeOp - consume an operator character from the beginning of the
 * given C character string.
 *
 * Params:
 *   s - pointer to a nonempty C character string
 *   op - pointer to an int (32 bit signed integer) variable where the
 *        ASCII code of the first character of s should be stored
 *
 * Returns:
 *   a pointer to the second character of s
 */
const char *consumeOp(const char *s, int *op) {
  *op = s[0];
  // return s+=1; // Both works... https://stackoverflow.com/questions/12988140/what-is-the-difference-between-and-1-operators
  return ++s; 
}

/*
 * stackPush - push a long (64 bit signed integer) value onto the stack.
 * Calls the fatalError function if the stack is full.
 * A full stack contains MAX_STACK (20) values.
 * If a value is successfully pushed on the stack, the count should
 * be incremented by 1.
 *
 * Params:
 *   stack - pointer to the beginning of the stack storage array
 *   count - pointer to a long variable storing the number of items
 *           currently on the stack
 *   val   - the value to push onto the stack
 *
 * Returns:
 *   nothing
 */
void stackPush(long stack[], long *count, long val) {
  if (*count >= MAX_STACK) {
    fatalError("the stack is full");
    return;
  }

  stack[*count] = val; // already offset by 1
  *count += 1;
}

/*
 * stackPop - pops a long (64 bit signed integer) value from the stack.
 * Calls the fatalError function if the stack is empty.
 * If a value is successfully popped off the stack, the count should
 * be decremented by 1.
 *
 * Params:
 *   stack - pointer to the beginning of the stack storage array
 *   count - pointer to a long variable storing the number of items
 *           currently on the stack
 *
 * Returns:
 *   the value popped from the stack
 */
long stackPop(long stack[], long *count) {
  // Check empty stack
  if (*count == 0) {
    fatalError("stack is empty");
    return -1; // Need to check? Error code?
  }
  *count-=1;
  return stack[*count]; // already offset by 1
}

/*
 * evalOp - evaluate a computation by applying an operator to two operands.
 *
 * Params:
 *   op - the operator ('+', '-', '*', or '/')
 *   left - the left operand
 *   right - the right operand
 *
 * Returns:
 *   the result of applying the operator to the operands
 */
long evalOp(int op, long left, long right) {
  switch (op)
  {
  case '+':
    return left + right;
  case '-':
    return left - right;
  case '*':
    return left * right;
  case '/':
    if (right == 0) {
      fatalError("division by 0 is undefined"); // Handle division by 0 // NO NEED TO HANDLE!!
    }
    return left / right;
  default:
    break;
  }
  return -1;
}
