/*
 * postfix calculator (assembly language version)
 * function implementation
 */

.equ TOK_INT, 0
.equ TOK_OP, 1
.equ TOK_UNKNOWN, 2

.equ MAX_STACK, 20

	.section .rodata

eNegativeValue: .string "Negative value"
eErrorMsg: .string "Error: %s\n"

/* TODO: add other string constants and other read-only data here */

	.section .text

/*
 * addPositive function (this is just an example function)
 */
	.globl addPositive
addPositive:
	subq $8, %rsp                /* Align stack pointer */

	movq $0, %r10                /* let %r10 be 0 */
	cmpq %rdi, %r10              /* compare 1st param to 0 */
	jg .LaddPositiveNegativeOperand /* if less, it's negative */
	cmpq %rsi, %r10              /* compare 2nd param to 0 */
	jg .LaddPositiveNegativeOperand /* if less, it's negative */

	/* both operands are non-negative, do the addition */
	movq %rsi, %rax              /* move second parameter to %rax */
	addq %rdi, %rax              /* add first parameter to second */
	jmp .LaddPositiveDone

.LaddPositiveNegativeOperand:
	movq $eNegativeValue, %rdi
	call fatalError
9:	jmp 9b                       /* should not get here */

.LaddPositiveDone:
	/* the sum is now in %rax */
	addq $8, %rsp                /* Restore stack pointer */
	ret

/*
 * fatalError - print an error message an exit with an exit code of 1.
 * Prints a message of the form "Error: <message text>" to standard output.
 * Does not return!
 *
 * Params:
 *   msg - description of the error which occurred
 */
	.globl fatalError
fatalError:
	/* TODO: implement */
	subq $8, %rsp                /* Align stack pointer */
	
	/* store rdi in a different func */
	/* add the formating to the strings */
	/* Call printf? */

	/* printf("Error: %s\n", msg) */
	/* 			^ 1st arg	 ^ 2nd arg */

	movl %edi, %esi 		/* pass the 1st arg as the 2nd arg */
	movq $eErrorMsg, %rdi 	/* pass the formating as the first arg*/
	call printf
	/* Do I need to restore the stack here??? */
	movq $1, %rdi
	call exit 				/* Call the exit code  function */

9:	jmp 9b                       /* should not get here */
	/* note that this function should not return */
	ret

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
	.globl isSpace
isSpace:
	subq $8, %rsp               /* Align stack pointer */
	cmpq $32, %rdi				/* Compare %rdi to 32, ascii code for space */
	je .LisSpaceTrue			/* If space, jump to .LisSpaceTrue */

	cmpq $9, %rdi				/* Compare %rdi to 9, ascii code for tab character */
	je .LisSpaceTrue			/* If space, jump to .LisSpaceTrue */

.LisSpaceFalse:					/* Is this even neccessary?? */
	movq $0, %rax				/* Set return value to 0 if not space */
	addq $8, %rsp				/* restore stack pointer */
	ret

.LisSpaceTrue:
	movq $1, %rax				/* Set return value to 1 if is space char */
	addq $8, %rsp				/* restore stack pointer */
	ret

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
	.globl isDigit
isDigit:
	subq $8, %rsp               /* Align stack pointer */
	cmpq $48, %rdi				/* Compare %rdi to 40, ascii code for 0 */
	jl .LisDigitFalse			/* If digit, jump to .LisDigitTrue */

	cmpq $57, %rdi				/* Compare %rdi to 57, ascii code for 9 */
	jg .LisDigitFalse			/* If digit, jump to .LisDigitTrue */

	jmp .LisDigitTrue			/* If not space, jump to .LisSpaceFalse */

.LisDigitTrue:
	movq $1, %rax				/* Set return value to 1 if is space char */
	addq $8, %rsp				/* restore stack pointer */
	ret

.LisDigitFalse:					/* Is this even neccessary?? */
	movq $0, %rax				/* Set return value to 0 if not space */
	addq $8, %rsp				/* Restore stack pointer */
	ret

	ret

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
	.globl skipws
skipws:
	subq $8, %rsp               /* Align stack pointer */
	movq %rdi, %r10				/* Copy s to r10 */

	/* While loop */
	jmp .LcheckIsSpace

/* Increment r10 by 1 to move to next char in s */
.LIsSpacetop:
	incq %r10

.LcheckIsSpace:
	cmpq $0, (%r10) 		/* Check for null char, marking end of string */
	je .LisNullChar			/* If null char, exit loop and return null */
	
	movzbl (%r10d), %edi	/* Copy first char stored in r10 to edi and zero extend it */
	call isSpace
	cmp $1, %rax 			/* Check with return value*/
	je .LIsSpacetop			/* Increment s by 1 */

	movq %r10, %rax 		/* Assign the current pointer to the return value */
	addq $8, %rsp			/* Restore stack pointer */
	ret

.LisNullChar:
	movq $0, %rax			/* If end of string reached, set return value to 0 (NULL) */
	addq $8, %rsp			/* Restore stack pointer */
	ret

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
	.globl tokenType
tokenType:
	subq $8, %rsp               /* Align stack pointer */
	movq %rdi, %r10				/* Copy s to r10 */
	movzbl (%r10d), %edi		/* Copy first char stored in r10 to edi and zero extend it - s[0] */
	
	/* Check if it is an operant */
	cmpl $42, %edi				/* Compare %edi to 32, ascii code for '*' */
	je .LisOperantToken			/* If '*', jump to .LisOpToken */
	cmpl $43, %edi				/* Compare %edi to 43, ascii code for '+' */
	je .LisOperantToken			/* If '+', jump to .LisOpToken */
	cmpl $45, %edi				/* Compare %edi to 45, ascii code for '-' */
	je .LisOperantToken			/* If '-', jump to .LisOpToken */
	cmpl $47, %edi				/* Compare %edi to 47, ascii code for '/' */
	je .LisOperantToken			/* If '/', jump to .LisOpToken */

	/* Check if it is a digit */
	call isDigit				/* check if it is digit */
	cmp $1, %rax 				/* Check with return value*/
	je .LisDigitToken			    

	/* otherwise, Unkown token */
	jmp .LisUnkownToken

.LisDigitToken:
	movq $TOK_INT, %rax 		/* Assign the current pointer to the return value */
	addq $8, %rsp			/* Restore stack pointer */
	ret

.LisOperantToken:
	movq $TOK_OP, %rax 		/* Assign the current pointer to the return value */
	addq $8, %rsp			/* Restore stack pointer */
	ret

.LisUnkownToken:
	movq $TOK_UNKNOWN, %rax 		/* Assign the current pointer to the return value */
	addq $8, %rsp			/* Restore stack pointer */
	ret


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
	.globl consumeInt
consumeInt:
	subq $8, %rsp               /* Align stack pointer */
	movq %rdi, %r10				/* Copy s to r10 */

	movq $0, (%rsi)				/* reset the val of p (2nd param) to zero */

	/* While loop */
	jmp .LcheckIsDigit

.LIsDigitTop:
	imulq $10, (%rsi)			/* rsi (p) *= 10 */
	movq $0, %r11 
	movl %edi, %r11d			/* promote the edi value to r11 of 64 bit*/
	/* movzbl (%edi), %r11			/* promote the edi value to r11 of 64 bit*/
	/*  addq (%edi) ,(%rsi)			/* p += s[0] -- need to pad z? */ 
	addq %r11 ,(%rsi)			/* p += s[0] -- need to pad z? */ 
	subq $48 ,(%rsi)			/* p -= 48 offset for ascii */ 
	incq %r10			/* Increment r10 by 1 to move to next char in s */

.LcheckIsDigit:
	movzbl (%r10d), %edi	/* Copy first char stored in r10 to edi and zero extend it */
	call isDigit
	cmp $1, %rax 			/* Check with return value*/
	je .LIsDigitTop			/* Increment s by 1 */	

	movq %r10, %rax 		/* Assign the current pointer to the return value */
	addq $8, %rsp			/* Restore stack pointer */
	ret

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
	.globl consumeOp
consumeOp:
	/* TODO: implement */
	ret

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
	.globl stackPush
stackPush:
	/* TODO: implement */
	ret

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
	.globl stackPop
stackPop:
	/* TODO: implement */
	ret

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
	.globl evalOp
evalOp:
	ret

/* vim:ft=gas:
 */
