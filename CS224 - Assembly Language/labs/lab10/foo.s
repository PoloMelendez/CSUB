	.file	"foo.c"
	.section	.rodata
.LC0:
	.string	"log %llu is %d.\n"
	.text
	.globl	main
	.type	main, @function
main:
.LFB0:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$32, %rsp
	movq	$1, -16(%rbp)
	movq	$1, -24(%rbp)
	movq	-16(%rbp), %rax
	salq	$3, %rax
	movq	%rax, -24(%rbp)
	movq	-16(%rbp), %rax
	shrq	$3, %rax
	movq	%rax, -24(%rbp)
	movl	$5, -28(%rbp)
	movl	-28(%rbp), %eax
	movl	%eax, %ecx
	salq	%cl, -24(%rbp)
	movq	$32, -24(%rbp)
	movq	-24(%rbp), %rax
	movl	%eax, -4(%rbp)
	movl	$0, -8(%rbp)
	jmp	.L2
.L3:
	sarl	-4(%rbp)
	addl	$1, -8(%rbp)
.L2:
	cmpl	$1, -4(%rbp)
	jg	.L3
	movl	-8(%rbp), %edx
	movq	-24(%rbp), %rax
	movq	%rax, %rsi
	movl	$.LC0, %edi
	movl	$0, %eax
	call	printf
	movl	$0, %eax
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE0:
	.size	main, .-main
	.ident	"GCC: (Debian 4.7.2-5) 4.7.2"
	.section	.note.GNU-stack,"",@progbits
