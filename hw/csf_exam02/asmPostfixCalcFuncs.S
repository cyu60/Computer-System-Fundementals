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
eStackFull: .string "the stack is full"
eStackEmpty: .string "stack is empty"
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
	subq $8, %rsp                /* Align stack pointer */
	
	/* store rdi in a different func */
	/* add the formating to the strings */
	/* Call printf? */

	/* printf("Error: %s\n", msg) */
	/* 			^ 1st arg	 ^ 2nd arg */

	movl %edi, %esi 			/* pass the 1st arg as the 2nd arg */
	movq $eErrorMsg, %rdi 		/* pass the formating as the first arg*/
	call printf

	movq $1, %rdi				/* Pass the exit code 1 */
	call exit 					/* Call the exit code  function */

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
	jl .LisDigitFalse			/* If not digit, jump to .LisDigitFalse */

	cmpq $57, %rdi				/* Compare %rdi to 57, ascii code for 9 */
	jg .LisDigitFalse			/* If not digit, jump to .LisDigitFalse */

	jmp .LisDigitTrue			/* If space, jump to .LisSpaceTrue */

.LisDigitTrue:
	movq $1, %rax				/* Set return value to 1 if is space char */
	addq $8, %rsp				/* restore stack pointer */
	ret

.LisDigitFalse:					/* Is this even neccessary?? */
	movq $0, %rax				/* Set return value to 0 if not space */
	addq $8, %rsp				/* Restore stack pointer */
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
	movzbl (%r10), %edi		/* Copy first char stored in r10 to edi and zero extend it */
	
	cmpq $0, %rdi 				/* Check for null char, marking end of string (edi is part of rdi) */
	je .LisNullChar				/* If null char, exit loop and return null */
	
	call isSpace
	cmp $1, %rax 				/* Check with return value*/
	je .LIsSpacetop				/* Increment s by 1 */

	movq %r10, %rax 			/* Assign the current pointer to the return value */
	addq $8, %rsp				/* Restore stack pointer */
	ret

.LisNullChar:
	movq $0, %rax				/* If end of string reached, set return value to 0 (NULL) */
	addq $8, %rsp				/* Restore stack pointer */
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
	movzbl (%r10), %edi		/* Copy first char stored in r10 to edi and zero extend it */
	
	/* Check if it is an operant */
	cmpl $42, %edi				/* Compare %edi to 42, ascii code for '*' */
	je .LisOperantToken			/* If '*', jump to .LisOperantToken */
	cmpl $43, %edi				/* Compare %edi to 43, ascii code for '+' */
	je .LisOperantToken			/* If '+', jump to .LisOperantToken */
	cmpl $45, %edi				/* Compare %edi to 45, ascii code for '-' */
	je .LisOperantToken			/* If '-', jump to .LisOperantToken */
	cmpl $47, %edi				/* Compare %edi to 47, ascii code for '/' */
	je .LisOperantToken			/* If '/', jump to .LisOperantToken */

	/* Check if it is a digit */
	call isDigit				/* check if it is digit */
	cmp $1, %rax 				/* Check with return value*/
	je .LisDigitToken			    

	/* otherwise, Unkown token */
	jmp .LisUnkownToken

.LisDigitToken:
	movq $TOK_INT, %rax 		/* Assign the current pointer to the return value */
	addq $8, %rsp				/* Restore stack pointer */
	ret

.LisOperantToken:
	movq $TOK_OP, %rax 			/* Assign the current pointer to the return value */
	addq $8, %rsp				/* Restore stack pointer */
	ret

.LisUnkownToken:
	movq $TOK_UNKNOWN, %rax 	/* Assign the current pointer to the return value */
	addq $8, %rsp				/* Restore stack pointer */
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

	movq $0, %r11				/* set the val of p (2nd param) to zero */

	/* While loop */
	jmp .LcheckIsDigit

.LIsDigitTop:
	imulq $10, %r11				/* rsi (p) *= 10 */
	addq %rdi ,%r11				/* p += s[0] -- %rdi is s[0] since we overwrite earlier */ 
	subq $48 ,%r11				/* p -= 48 offset for ascii */ 
	incq %r10					/* Increment r10 by 1 to move to next char in s */

.LcheckIsDigit:
	movzbl (%r10), %edi		/* Copy first char stored in r10 to edi and zero extend it */
	call isDigit
	cmp $1, %rax 				/* Check with return value*/
	je .LIsDigitTop				/* Increment s by 1 */	

	movq  %r11 ,(%rsi)			/* Assign the p value to the actual val from pointer */
	movq %r10, %rax 			/* Assign the current pointer to the return value */
	addq $8, %rsp				/* Restore stack pointer */
	ret

/* NEW FUNC */
	.globl is_min_heap
is_min_heap:
	subq $8, %rsp               /* Align stack pointer */
	movl $0, %r10d				/* Set 0 to cur */

	/* While loop */
	jmp .LcheckHasChildren

.LHasChildrem:
	
	/* Check left */
	movl (%rdi, %r10, 4), %r11d /* arr[cur] == %rdi (base) + %r10d (index) * 4 (scale factor) */
	movl (%rdi, %r9, 4), %r8d /* arr[cur] == %rdi (base) + %r10d (index) * 4 (scale factor) */
	/* movl (%rdi, %r10d, 4), %r11d /* arr[cur] == %rdi (base) + %r10d (index) * 4 (scale factor) */
	/* movl (%rdi, %r9d, 4), %r8d /* arr[cur] == %rdi (base) + %r10d (index) * 4 (scale factor) */
	cmp %r8d, %r11d
	jl .LisNotMinHeap
	/* Check right */
	incl %r9d
	cmp %r9d, %esi /* see if right child exists */
	jl .LHasRightChild
	jmp .LIsMinHeap /* Finish checking */

.LHasRightChild:
	movl (%rdi, %r9, 4), %r8d /* arr[cur] == %rdi (base) + %r9d (index) * 4 (scale factor) */
	cmp %r8d, %r11d
	jl .LisNotMinHeap
	incl %r10d					/* Increment r10d by 1 */
	jmp .LcheckHasChildren		/* continue with while loop*/

.LcheckHasChildren:
	movl (1, %r10d, 2), %r9d
	cmp %r9d, %esi 	/* cur * 2 + 1 compare to len */
	/* cmp (1, %r10d, 2), %esi 	/* cur * 2 + 1 compare to len */
	jl .LHasChildrem			/* Go to start of loop */	
	/* Should otherwise jump to is min heap? */
	jmp .LIsMinHeap

.LisNotMinHeap:
	movq $0, %rax 			/* Assign 0 to the return value */
	addq $8, %rsp				/* Restore stack pointer */
	ret

.LIsMinHeap:
	movq $1, %rax 			/* Assign 1 to the return value */
	addq $8, %rsp				/* Restore stack pointer */
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
	subq $8, %rsp               /* Align stack pointer */
	movq %rdi, %r10				/* Copy s to r10 */

	movzbl (%r10), %edi		/* Copy first char stored in r10 to edi and zero extend it */
	
	movl  %edi ,(%rsi)			/* Assign the s[0] value to op -- (needs to be an int!) -- 3 bytes */
	
	incq %r10					/* Increment r10 by 1 to move to next char in s (s+=1) */
	movq %r10, %rax 			/* Assign the current pointer to the return value */
	addq $8, %rsp				/* Restore stack pointer */
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
	subq $8, %rsp               /* Align stack pointer */
	cmpq $MAX_STACK, (%rsi)		/* Compare count to max stack */
	je .LisStackFull			/* If stack full, jump to .LisStackFull */

	
	/* Asccess the stack value at count */
	movq (%rsi), %r10 			/* Assgin count to r10 */
	movq %rdx, (%rdi, %r10, 8) 	/* stack[count] == %rdi (base) + %r10 (index) * 8 (scale factor) */
	
	incq (%rsi)					/* Increment count by 1 */

	addq $8, %rsp				/* Restore stack pointer */
	ret

.LisStackFull:
	movq $eStackFull, %rdi
	call fatalError
9:	jmp 9b                      /* should not get here */

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
	subq $8, %rsp               /* Align stack pointer */
	cmpq $0, (%rsi)				/* Compare count to 0 */
	je .LisStackEmpty			/* If stack full, jump to .LisStackEmpty */
	
	decq (%rsi)					/* Decrement count by 1 */
	
	/* Asccess the stack value at count */
	movq (%rsi), %r10 			/* Assgin count to r10 */
	movq (%rdi, %r10, 8), %rax 	/* stack[count] == %rdi (base) + %r10 (index) * 8 (scale factor) */
	
	addq $8, %rsp				/* Restore stack pointer */
	ret

.LisStackEmpty:
	movq $eStackEmpty, %rdi
	call fatalError
9:	jmp 9b                      /* should not get here */

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
	subq $8, %rsp               /* Align stack pointer */

	/* Check which operant operant */
	cmpl $42, %edi				/* Compare %edi to 42, ascii code for '*' */
	je .LisMultiply				/* If '*', jump to .LisMultiply */
	cmpl $43, %edi				/* Compare %edi to 43, ascii code for '+' */
	je .LisAdd					/* If '+', jump to .LisAdd */
	cmpl $45, %edi				/* Compare %edi to 45, ascii code for '-' */
	je .LisSubtract				/* If '-', jump to .LisSubtract */
	cmpl $47, %edi				/* Compare %edi to 47, ascii code for '/' */
	je .LisDivide				/* If '/', jump to .LisDivide */

.LisMultiply:
	imulq %rdx, %rsi 			/* set %rsi to %rdx (right) * %rsi (left) */
	movq %rsi, %rax
	addq $8, %rsp				/* Restore stack pointer */
	ret

.LisAdd:
	addq %rdx, %rsi 
	movq %rsi, %rax
	addq $8, %rsp				/* Restore stack pointer */
	ret

.LisSubtract:
	subq %rdx, %rsi  			/* rsi (left) - rdx (right) */ 
	movq %rsi, %rax
	addq $8, %rsp				/* Restore stack pointer */
	ret

.LisDivide:
	movq %rsi, %rdi 			/* store 1nd val in rdi */
	movq %rdx, %rsi 			/* store 2nd val in rsi */

	/* Taken from the provided example code */
	movq %rdi, %rax             /* put dividend in %rax */
	cqto                        /* sign-extend dividend into %rdx */
	idivq %rsi                  /* divide by the divisor */
	/* quotient is now in %rax */
	
	addq $8, %rsp				/* Restore stack pointer */
	ret


/* vim:ft=gas:
 */
