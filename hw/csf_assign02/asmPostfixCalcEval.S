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
	/* Will need to push and restore the rbx and r12 pointers? */
	pushq %rbp
	pushq %rbx
	pushq %r12
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

	movq $0, 0(%rbp)			/* assign count to start at 0 */

	/* NOTE: NEED TO DIFFERENTIATE BETWEEN ADDRESS AND VALUES FOR STACK VARS?? */
	/* Find the 1st valid token */
	call skipws
	movq %rax, %rbx 			/* update s value and copy to rbx */

	/* While loop */
	jmp .LcheckString

.LIsStringTop:
	call tokenType 				/* Call tokentype to determine what type of char */
	/* Assign token type to %r11 (int of 4 bytes) */
	movq %rax, %r11 /* Need to use diff prefix? */

	/* Check for the token type */
	cmpq $TOK_INT, %r11				
	je .LisTOK_INT				/* If int, jump to .LisTOK_INT */
	cmpq $TOK_OP, %r11		
	je .LisTOK_OP				/* If op, jump to .LisTOK_OP */
	cmpq $TOK_UNKNOWN, %r11		
	je .LisTOK_UNKOWN			/* If unkown, jump to .LisTOK_UNKOWN */

.LisTOK_INT:
	movq %rbx, %rdi 			/* Assign the string to the first arg */
	leaq 8(%rbp), %rsi			/* Assign val pointer to the 2nd arg */
	call consumeInt				/* Getting a seg fault here?? */
	movq %rax, %rbx 			/* update the value of s */
	jmp .LIsStringBottom 

.LisTOK_UNKOWN:
	movq $eUnrecognizedChar, %rdi
	call fatalError
9:	jmp 9b                       /* should not get here */

.LisTOK_OP:
	/* Check count < 2 */
	movq 0(%rbp), %r11		/* using r11 temp */
	cmpq $2, %r11 			/* 0(%rbp) is the long count */
	jg .LinsufficentArgs

	/* Call stack pop (stack, count) for right */
	leaq 36(%rbp), %rdi			/* 36(%rbp) is the start of long stack[20] */
	leaq 0(%rbp), %rsi
	movq %rax, 24(%rbp)			/* assign return to right 24(%rbp) */
	/* Call stack pop (stack, count) for left */
	leaq 36(%rbp), %rdi			/* 36(%rbp) is the start of long stack[20] */
	leaq 0(%rbp), %rsi
	movq %rax, 16(%rbp)			/* assign return to left 16(%rbp) */

	/* Call consume Operant */
	movq %rbx, %rdi 			/* Assign the string to the first arg */
	leaq 32(%rbp), %rsi			/* Assign op to the 2nd arg (op is an int!) */
	call consumeOp
	movq %rax, %rbx 			/* update the value of s */

	/* Call eval Op (op, left, right) */
	movzbl 32(%rbp), %edi		/* Copy int stored in 32(%rbp) (op) to edi and zero extend it */
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
	leaq 36(%rbp), %rdi			/* 36(%rbp) is the start of long stack[20] */
	leaq 0(%rbp), %rsi			/* 0(%rbp) is the long count */
	movq 8(%rbp), %rdx			/* assign right val to 3nd param */
	call stackPush

	/* Find the next valid token */
	call skipws
	movq %rax, %rbx 			/* update s value and copy to rbx */

.LcheckString:
	/* Need to check the full val and not just pointer?? */
	cmpq $0, %rbx 				/* Check for null terminator, marking end of string (edi is part of rdi) */
	je .LafterCheckString
	/* movzbl (%rbx), %edi			/* Copy first char stored in rbx to edi and zero extend it */
	movq %rbx, %rdi			/* Copy pointer address to rdi */
	jmp .LIsStringTop				/* Go to while loop content */	

.LafterCheckString:
	movq 0(%rbp), %r11
	cmpq $1, %r11			/* check count == 1 */
	je .LstringComputed

	jmp .LstringNotComputed

.LstringComputed:
	movq 36(%rbp), %rdi			/* 36(%rbp) is the start of long stack[20] */
	movq 0(%rbp), %rsi			/* 0(%rbp) is the long count */
	call stackPop

	addq $288, %rsp 			/* restore the stack pointer */
	popq %r12
	popq %rbx
	popq %rbp
	ret

.LstringNotComputed:
	movq $eCannotCompute, %rdi
	call fatalError
9:	jmp 9b                       /* should not get here */
/* vim:ft=gas:
 */
