/*
 * postfix calculator (assembly language version)
 * main function implementation
 */

.section .rodata


eWrongArgs: .string "Wrong number of arguments\n"
sResultfmt: .string "Result is: %ld\n"

/* TODO: add string constants and other read-only data here */

.section .text 

/*
 * main function: program entry point
 *
 * Params:
 *   argc - number of command line arguments (including program name)
 *   argv - array of pointers to command line arguments
 *
 * Returns:
 *   exit code
 */
	.globl main
main:
	subq $8, %rsp                /* Align stack pointer */

	cmpl $2, %edi				/* check argc is 2 */
	je .LisTwoArg

.LisNotTwoArg:
	movq $eWrongArgs, %rdi
	call printf
	
	movq $1, %rdi				/* return with exit code 1 */
	addq $8, %rsp                /* Restore stack pointer */
	ret

.LisTwoArg:
	incq %rsi 					/* move to  argv[1] (array of pointers 8 bytes) */

	/* movq 8(%rsi), %rsi 					/* move to argv[1] (array of pointers of size 8) */
	movq (%rsi), %rdi				/* assign s (argv[1]) to the first arg */
	call eval

	movq $sResultfmt, %rdi 		/* move the result format to the first arg */
	movq %rax, %rsi				/* move result to the second arg */
	call printf

	movq $0, %rdi				/* return with exit code 0 */
	addq $8, %rsp                /* Restore stack pointer */
	ret

/* vim:ft=gas:
 */
