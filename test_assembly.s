	.file	"test_assembly.c"
	.text
	.globl	main
	.type	main, @function
main:
.LFB5:
	.cfi_startproc
	movl	$312, -16(%rsp)
	movl	$91, -12(%rsp)
	movl	$2, -8(%rsp)
	movl	$3, -4(%rsp)
	movl	-12(%rsp), %edx
	movl	-8(%rsp), %eax
	addl	%edx, %eax
	cltd
	idivl	-4(%rsp)
	imull	-16(%rsp), %eax
	ret
	.cfi_endproc
.LFE5:
	.size	main, .-main
	.ident	"GCC: (Ubuntu 7.5.0-3ubuntu1~18.04) 7.5.0"
	.section	.note.GNU-stack,"",@progbits
