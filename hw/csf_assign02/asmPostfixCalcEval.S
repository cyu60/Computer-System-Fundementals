/*
 * postfix calculator (assembly language version) eval function implementation
 */

.equ TOK_INT, 0
.equ TOK_OP, 1
.equ TOK_UNKNOWN, 2

	.section .rodata

eInsufficientArgs: .string "insufficient arguments"
eUnrecognizedChar: .string "unrecognized character"
eCannotCompute: .string "string cannot be computed"

/* TODO: add string constants and other read-only data here */

	.section .text

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
	.globl eval
eval:
	pushq %rbp
	subq $208, %rsp				/* 0(%rbp) is the long count */
								/* 8(%rbp) is the long val */
								/* 16(%rbp) is the long left */
								/* 24(%rbp) is the long right */
								/* 32(%rbp) is the int op */
								/* 36(%rbp) is the start of long stack[20] */
								/* Note: size = factor (8) * num element (20) */
								/* 196(%rbp) is the end of long stack[20] */
								/* Need an additional 12 to be multiple of 16! at 208 */
	movq %rsp, %rbp				/* keep the base pointer */

	movq $0, 0(%rsp)			/* assign count to start at 0 */

	/* NOTE: SHOULD CONSIDER A CALLEE SAVED REGISTER RATHER THAN r10?? */
	/* Find the 1st valid token */
	call skipws
	movq %rax, %r10 			/* update s value and copy to r10 */

	/* While loop */
	jmp .LcheckString

.LIsStringTop:
	call tokenType
	/* Assign token type to %r11 (int of 4 bytes) */
	movq %rax, %r11 /* Need to use diff prefix? */

	/* Check for the token type */
	cmpl $TOK_INT, %r11				/* Compare %edi to 42, ascii code for '*' */
	je .LisTOK_INT			/* If '*', jump to .LisTOK_INT */
	cmpl $TOK_OP, %r11				/* Compare %edi to 43, ascii code for '+' */
	je .LisTOK_OP			/* If '*', jump to .LisTOK_OP */
	cmpl $TOK_UNKNOWN, %r11				/* Compare %edi to 43, ascii code for '+' */
	je .LisTOK_UNKOWN			/* If '*', jump to .LisTOK_UNKOWN */

.LisTOK_INT:
	movq %r10, %rdi 			/* Assign the string to the first arg */
	movq 8(%rsp), %rsi			/* Assign val to the 2nd arg */
	call consumeInt
	movq %rax, %r10 			/* update the value of s */
	jmp .LIsStringBottom 

.LisTOK_UNKOWN:
	movq $eUnrecognizedChar, %rdi
	call fatalError
9:	jmp 9b                       /* should not get here */

.LisTOK_OP:
	/* Check count < 2 */
	cmpq 0(%rbp), $2			/* 0(%rbp) is the long count */
	jl .LinsufficentArgs

	/* Call stack pop (stack, count) for right */
	movq 36(%rbp), %rdi			/* 36(%rbp) is the start of long stack[20] */
	movq 0(%rbp), %rsi
	movq %rax, 24(%rbp)			/* assign return to right 24(%rbp) */
	/* Call stack pop (stack, count) for left */
	movq 36(%rbp), %rdi			/* 36(%rbp) is the start of long stack[20] */
	movq 0(%rbp), %rsi
	movq %rax, 16(%rbp)			/* assign return to left 16(%rbp) */

	/* Call consome Operant */
	movq %r10, %rdi 			/* Assign the string to the first arg */
	movl 32(%rsp), %rsi			/* Assign op to the 2nd arg (op is an int!) */
	call consumeOp
	movq %rax, %r10 			/* update the value of s */

	/* Call eval Op (op, left, right) */
	movzbl 32(%rsp), %edi		/* Copy int stored in 32(%rsp) (op) to edi and zero extend it */
	movq 16(%rbp), %rsi			/* assign left 16(%rbp) to 2nd param */
	movq 24(%rbp), %rdx			/* assign right 24(%rbp) to 3nd param */
	call evalOp
	movq %rax, 8(%rbp)			/* assign the return to val */

	jmp .LIsStringBottom 

.LinsufficentArgs:
	movq $eInsufficientArgs, %rdi
	call fatalError
9:	jmp 9b                       /* should not get here */

.LIsStringBottom:
	/* update the stack with the val with stack push */
	movq 36(%rbp), %rdi			/* 36(%rbp) is the start of long stack[20] */
	movq 0(%rbp), %rsi			/* 0(%rbp) is the long count */
	movq (8(%rbp)), %rdx			/* assign right val to 3nd param */
	/* Find the next valid token */
	call skipws
	movq %rax, %r10 			/* update s value and copy to r10 */

	jmp .LcheckString

.LcheckString:
	movzbl (%r10d), %edi		/* Copy first char stored in r10 to edi and zero extend it */
	cmpq $0, %rdi 				/* Check for null char, marking end of string (edi is part of rdi) */
	je .LIsStringTop				/* Increment s by 1 */	

	comq $1, (0(%rsp))			/* check count == 1 */
	je .LstringComputed

	jmp .LstringNotComputed

.LstringComputed:
	movq 36(%rbp), %rdi			/* 36(%rbp) is the start of long stack[20] */
	movq 0(%rbp), %rsi			/* 0(%rbp) is the long count */
	call stackPop

	addq $288, %rsp 			/* restore the stack pointer */
	popq %rbp
	ret

.LstringNotComputed:
	movq $eCannotCompute, %rdi
	call fatalError
9:	jmp 9b                       /* should not get here */
/* vim:ft=gas:
 */
