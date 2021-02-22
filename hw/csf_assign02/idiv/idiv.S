	.section .rodata
sPromptMsg: .string "Enter two integers: "
sInputFmt: .string "%ld %ld"
sResultMsg: .string "%ld / %ld = %ld\n"

	.section .bss
	.align 8
firstval: .space 8
secondval: .space 8

	.section .text

/*
 * Divide one long value by another and return the
 * quotient.
 *
 * Parameters:
 *   %rdi - the dividend
 *   %rsi - the divisor
 *
 * Returns (in %rax):
 *   the quotient
 */
divideLongs:
	movq %rdi, %rax               /* put dividend in %rax */
	cqto                          /* sign-extend dividend into %rdx */
	idivq %rsi                    /* divide by the divisor */
	ret                           /* quotient is now in %rax */

	.globl main
main:
	subq $8, %rsp

	/* prompt user to enter two integer values */
	movq $sPromptMsg, %rdi
	call printf

	/* read two integer values */
	movq $sInputFmt, %rdi
	movq $firstval, %rsi
	movq $secondval, %rdx
	call scanf

	/* call divideLongs to do the division */
	movq firstval, %rdi
	movq secondval, %rsi
	call divideLongs

	/* print result message */
	movq $sResultMsg, %rdi
	movq firstval, %rsi
	movq secondval, %rdx
	movq %rax, %rcx               /* quotient is 4th arg to printf */
	call printf

	addq $8, %rsp
	movl $0, %eax
	ret
