	.file	"main.c"
	.version	"01.01"
gcc2_compiled.:
.section	.rodata
.LC0:
	.string	"%d ROR %d=%d (%x)\n"
.LC1:
	.string	"mem->memory=%x\n"
.LC2:
	.string	"r"
	.align 32
.LC3:
	.string	"/home/jules/ARMemu/source/object"
.LC4:
	.string	"Beginning emulation...\n"
.LC5:
	.string	"pc=%d\n"
.LC6:
	.string	"r1=%d\n"
.text
	.align 16
.globl main
	.type	 main,@function
main:
	pushl %ebp
	pushl %edi
	pushl %esi
	movl $32,%edi
	pushl %ebx
	movl $127,%esi
	xorl %ebx,%ebx
	.p2align 4,,7
.L35:
	movl %ebx,%ecx
	movl %esi,%edx
	movl %edi,%ebp
	sarl %cl,%edx
	subl %ebx,%ebp
	movl %esi,%eax
	movl %ebp,%ecx
	sall %cl,%eax
	orl %eax,%edx
	pushl %edx
	pushl %edx
	pushl %ebx
	pushl $127
	incl %ebx
	pushl $.LC0
	call printf
	addl $20,%esp
	cmpl $31,%ebx
	jle .L35
	pushl $4194304
	call memory_initialise
	movl %eax,%edi
	pushl $64
	call safemalloc
	movl %eax,%esi
	pushl $4
	call safemalloc
	movl %eax,%ebp
	movl (%edi),%eax
	pushl %eax
	pushl $.LC1
	call printf
	pushl $.LC2
	pushl $.LC3
	call fopen
	movl %eax,%ebx
	pushl %ebx
	pushl $63044
	pushl $1
	movl (%edi),%eax
	pushl %eax
	call fread
	addl $44,%esp
	pushl %ebx
	call fclose
	pushl $.LC4
	movl stderr,%eax
	pushl %eax
	call fprintf
	addl $12,%esp
	.p2align 4,,7
.L39:
	movl 60(%esi),%ebx
	andl $-4,%ebx
	pushl %ebx
	pushl %edi
	call memory_lookup
	movl %eax,%ebx
	movl 60(%esi),%eax
	pushl %eax
	pushl $.LC5
	call printf
	pushl %edi
	pushl %esi
	pushl %ebp
	movl (%ebx),%eax
	pushl %eax
	call decode
	movl 4(%esi),%eax
	addl $32,%esp
	pushl %eax
	pushl $.LC6
	call printf
	movl stdin,%eax
	addl $8,%esp
	pushl %eax
	call _IO_getc
	addl $4,%esp
	jmp .L39
.Lfe1:
	.size	 main,.Lfe1-main
	.ident	"GCC: (GNU) pgcc-2.91.66 19990314 (egcs-1.1.2 release)"
