	.file	"decode3.c"
	.version	"01.01"
gcc2_compiled.:
.globl diss
.data
	.align 32
	.type	 diss,@object
	.size	 diss,48
diss:
	.long diss_dp
	.long diss_dp_imm
	.long diss_mul
	.long diss_sdt
	.long diss_bdt
	.long diss_bra
	.long diss_swi
	.long diss_cdt
	.long diss_cdo
	.long diss_crt
	.long diss_sds
	.long diss_und
.globl exec
	.align 32
	.type	 exec,@object
	.size	 exec,48
exec:
	.long exec_dp
	.long exec_dp_imm
	.long exec_mul
	.long exec_sdt
	.long exec_bdt
	.long exec_bra
	.long exec_swi
	.long exec_cdt
	.long exec_cdo
	.long exec_crt
	.long exec_sds
	.long exec_und
.globl pseudo
	.align 32
	.type	 pseudo,@object
	.size	 pseudo,48
pseudo:
	.long pseudo_dp
	.long pseudo_dp_imm
	.long pseudo_mul
	.long pseudo_sdt
	.long pseudo_bdt
	.long pseudo_bra
	.long pseudo_swi
	.long pseudo_cdt
	.long pseudo_cdo
	.long pseudo_crt
	.long pseudo_sds
	.long pseudo_und
.text
	.align 16
.globl dispatch
	.type	 dispatch,@function
dispatch:
	pushfd
	pushl %esi
	pushl %ebx
	movl 16(%esp),%edx
	movl %edx,%eax
	movl 12(%esp),%ebx
	shrl $24,%eax
	movl 20(%esp),%esi
	movl 24(%esp),%ecx
	andl $15,%eax
	cmpl $15,%eax
	ja .L2
	jmp *.L25(,%eax,4)
	.p2align 4,,7
.section	.rodata
	.align 4
	.align 4
.L25:
	.long .L3
	.long .L6
	.long .L10
	.long .L10
	.long .L12
	.long .L12
	.long .L14
	.long .L14
	.long .L16
	.long .L16
	.long .L17
	.long .L18
	.long .L20
	.long .L20
	.long .L21
	.long .L24
.text
	.p2align 4,,7
.L3:
	movl %edx,%eax
	andl $264241392,%eax
	cmpl $144,%eax
	jne .L4
	pushl %ecx
	pushl %edx
	pushl %ebx
	movl 8(%esi),%eax
	jmp .L27
	.p2align 4,,7
.L4:
	pushl %ecx
	pushl %edx
	pushl %ebx
	movl (%esi),%eax
	jmp .L27
	.p2align 4,,7
.L6:
	movl %edx,%eax
	andl $263196656,%eax
	cmpl $16777360,%eax
	jne .L7
	pushl %ecx
	pushl %edx
	pushl %ebx
	movl 40(%esi),%eax
	jmp .L27
	.p2align 4,,7
.L7:
	pushl %ecx
	pushl %edx
	pushl %ebx
	movl (%esi),%eax
	jmp .L27
	.p2align 4,,7
.L10:
	pushl %ecx
	pushl %edx
	pushl %ebx
	movl 4(%esi),%eax
	jmp .L27
	.p2align 4,,7
.L12:
	pushl %ecx
	pushl %edx
	pushl %ebx
	movl 12(%esi),%eax
	jmp .L27
	.p2align 4,,7
.L14:
	pushl %ecx
	pushl %edx
	pushl %ebx
	movl 12(%esi),%eax
	jmp .L27
	.p2align 4,,7
.L16:
	pushl %ecx
	pushl %edx
	pushl %ebx
	movl 16(%esi),%eax
	jmp .L27
	.p2align 4,,7
.L17:
	pushl %ecx
	pushl %edx
	pushl %ebx
	movl 20(%esi),%eax
	jmp .L27
	.p2align 4,,7
.L18:
	pushl %ecx
	pushl %edx
	pushl %ebx
	movl 20(%esi),%eax
	jmp .L27
	.p2align 4,,7
.L20:
	pushl %ecx
	pushl %edx
	pushl %ebx
	movl 28(%esi),%eax
	jmp .L27
	.p2align 4,,7
.L21:
	testb $16,%dl
	je .L22
	pushl %ecx
	pushl %edx
	pushl %ebx
	movl 36(%esi),%eax
	jmp .L27
	.p2align 4,,7
.L22:
	pushl %ecx
	pushl %edx
	pushl %ebx
	movl 32(%esi),%eax
	jmp .L27
	.p2align 4,,7
.L24:
	pushl %ecx
	pushl %edx
	pushl %ebx
	movl 24(%esi),%eax
.L27:
	call *%eax
	addl $12,%esp
.L2:
	popl %ebx
	popl %esi
	ret
.Lfe1:
	.size	 dispatch,.Lfe1-dispatch
	.ident	"GCC: (GNU) pgcc-2.91.66 19990314 (egcs-1.1.2 release)"
