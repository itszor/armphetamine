	.file	"decode.c"
	.version	"01.01"
gcc2_compiled.:
.section	.rodata
.LC0:
	.string	"and"
.LC1:
	.string	"eor"
.LC2:
	.string	"sub"
.LC3:
	.string	"rsb"
.LC4:
	.string	"add"
.LC5:
	.string	"adc"
.LC6:
	.string	"sbc"
.LC7:
	.string	"rsc"
.LC8:
	.string	"tst"
.LC9:
	.string	"teq"
.LC10:
	.string	"cmp"
.LC11:
	.string	"cmn"
.LC12:
	.string	"orr"
.LC13:
	.string	"mov"
.LC14:
	.string	"bic"
.LC15:
	.string	"mvn"
.LC16:
	.string	"branch"
.LC17:
	.string	"multiply with accumulate"
.LC18:
	.string	"multiply"
.LC19:
	.string	"single-data transfer"
.LC20:
	.string	"block-data transfer"
.LC21:
	.string	"swi"
.LC22:
	.string	"co-processor data transfer"
.LC23:
	.string	"co-processor data op"
	.align 32
.LC24:
	.string	"co-processor register transfer"
.LC25:
	.string	"single data swap"
.LC26:
	.string	"undefined instruction"
.LC27:
	.string	"bad instruction pattern"
.text
	.align 16
.globl decode
	.type	 decode,@function
decode:
	subl $56,%esp
	pushl %ebp
	pushl %edi
	pushl %esi
	pushl %ebx
	movl 76(%esp),%edi
	movl %edi,%eax
	shrl $28,%eax
	cmpl $15,%eax
	ja .L17
	jmp *.L49(,%eax,4)
	.p2align 4,,7
.section	.rodata
	.align 4
	.align 4
.L49:
	.long .L18
	.long .L20
	.long .L22
	.long .L24
	.long .L26
	.long .L28
	.long .L30
	.long .L32
	.long .L34
	.long .L36
	.long .L38
	.long .L40
	.long .L42
	.long .L45
	.long .L17
	.long .L207
.text
	.p2align 4,,7
.L18:
	movl 80(%esp),%ecx
	testb $64,(%ecx)
	jne .L17
	jmp .L207
	.p2align 4,,7
.L20:
	movl 80(%esp),%esi
	testb $64,(%esi)
	je .L17
	jmp .L241
	.p2align 4,,7
.L22:
	movl 80(%esp),%ebx
	testb $1,(%ebx)
	jne .L17
	jmp .L149
	.p2align 4,,7
.L24:
	movl 80(%esp),%ecx
	testb $1,(%ecx)
	je .L17
	jmp .L207
	.p2align 4,,7
.L26:
	movl 80(%esp),%esi
	testb $128,(%esi)
	jne .L17
	jmp .L241
	.p2align 4,,7
.L28:
	movl 80(%esp),%ebx
	testb $128,(%ebx)
	je .L17
	jmp .L149
	.p2align 4,,7
.L30:
	movl 80(%esp),%ecx
	testb $8,1(%ecx)
	jne .L17
	jmp .L207
	.p2align 4,,7
.L32:
	movl 80(%esp),%esi
	testb $8,1(%esi)
	je .L17
	jmp .L241
	.p2align 4,,7
.L34:
	movl 80(%esp),%ebx
	movb (%ebx),%al
	andb $65,%al
	cmpb $1,%al
	je .L17
	jmp .L149
	.p2align 4,,7
.L36:
	movl 80(%esp),%ecx
	movb (%ecx),%al
	andb $65,%al
	cmpb $1,%al
	jne .L17
	jmp .L207
	.p2align 4,,7
.L38:
	movl 80(%esp),%esi
	movb (%esi),%dl
	movb 1(%esi),%al
	shrb $7,%dl
	shrb $3,%al
	movzbl %dl,%edx
	andl $1,%eax
	cmpl %eax,%edx
	je .L17
	jmp .L241
	.p2align 4,,7
.L40:
	movl 80(%esp),%ebx
	movb (%ebx),%dl
	movb 1(%ebx),%al
	shrb $7,%dl
	shrb $3,%al
	movzbl %dl,%edx
	andl $1,%eax
	cmpl %eax,%edx
	jne .L17
	jmp .L149
	.p2align 4,,7
.L42:
	movl 80(%esp),%ecx
	movb (%ecx),%dl
	testb $64,%dl
	jne .L207
	shrb $7,%dl
	movzbl %dl,%eax
	movb 1(%ecx),%dl
	shrb $3,%dl
	andl $1,%edx
	cmpl %edx,%eax
	je .L17
	jmp .L207
	.p2align 4,,7
.L45:
	movl 80(%esp),%esi
	movb (%esi),%dl
	testb $64,%dl
	jne .L17
	shrb $7,%dl
	movzbl %dl,%eax
	movb 1(%esi),%dl
	shrb $3,%dl
	andl $1,%edx
	cmpl %edx,%eax
	jne .L17
.L241:
	movl 84(%esp),%ecx
	addl $4,60(%ecx)
	jmp .L16
	.p2align 4,,7
.L17:
	movl %edi,%edx
	andl $201326592,%edx
	jne .L51
	movl %edi,%eax
	movl 84(%esp),%esi
	movl %edi,%edx
	shrl $14,%eax
	andl $4095,%edx
	andl $60,%eax
	movl (%eax,%esi),%eax
	movl %eax,68(%esp)
	testl $33554432,%edi
	je .L52
	movl %edx,%eax
	movl %edi,28(%esp)
	shrl $8,%edx
	andl $255,%eax
	addl %edx,%edx
	movl %eax,%ebp
	movl %edx,%ecx
	shrl $21,28(%esp)
	shrl %cl,%ebp
	movl $32,%ecx
	subl %edx,%ecx
	sall %cl,%eax
	orl %eax,%ebp
	jmp .L53
	.p2align 4,,7
.L52:
	movl 84(%esp),%ebx
	movl %edx,%eax
	andl $15,%eax
	movl (%ebx,%eax,4),%eax
	movl %edi,28(%esp)
	shrl $21,28(%esp)
	movl 28(%esp),%ecx
	movl %edx,20(%esp)
	movl $1,56(%esp)
	andl $15,%ecx
	shrl $5,20(%esp)
	sall %cl,56(%esp)
	andl $3,20(%esp)
	movl %eax,60(%esp)
	andl $62211,56(%esp)
	testb $16,%dl
	je .L54
	shrl $4,%edx
	movl (%ebx,%edx,4),%edx
	jmp .L242
	.p2align 4,,7
.L54:
	shrl $3,%edx
.L242:
	movl %edx,16(%esp)
	cmpl $1,20(%esp)
	je .L60
	jb .L57
	cmpl $2,20(%esp)
	je .L66
	cmpl $3,20(%esp)
	je .L74
	jmp .L53
	.p2align 4,,7
.L57:
	movl 60(%esp),%ebp
	cmpl $0,16(%esp)
	je .L58
	movl 16(%esp),%ecx
	sall %cl,%ebp
.L58:
	cmpl $0,56(%esp)
	setne %al
	cmpl $0,16(%esp)
	setne %dl
	andl %edx,%eax
	testb $1,%al
	je .L53
	testl $1048576,%edi
	je .L53
	movl 80(%esp),%esi
	movl $32,%eax
	subl 16(%esp),%eax
	jmp .L243
	.p2align 4,,7
.L60:
	cmpl $0,16(%esp)
	setne %al
	movl 16(%esp),%ecx
	movzbl %al,%ebp
	movl 60(%esp),%eax
	negl %ebp
	shrl %cl,%eax
	andl %eax,%ebp
	cmpl $0,56(%esp)
	je .L53
	testl $1048576,%edi
	je .L53
	movl 80(%esp),%ebx
	movb (%ebx),%dl
	testl %ecx,%ecx
	je .L64
	movl %ecx,%eax
	leal -1(%eax),%ecx
	sall %cl,20(%esp)
	movl 20(%esp),%ebx
	testl %ebx,60(%esp)
	setne %al
	movzbl %al,%eax
	jmp .L65
	.p2align 4,,7
.L64:
	movl 60(%esp),%eax
	shrl $31,%eax
.L65:
	andb $1,%al
	andb $-2,%dl
	movl 80(%esp),%esi
	orb %al,%dl
	movb %dl,(%esi)
	jmp .L53
	.p2align 4,,7
.L66:
	cmpl $0,16(%esp)
	je .L67
	movl 60(%esp),%ebp
	movl 16(%esp),%ecx
	sarl %cl,%ebp
	jmp .L68
	.p2align 4,,7
.L67:
	cmpl $0,60(%esp)
	setl %al
	movzbl %al,%ebp
	negl %ebp
.L68:
	cmpl $0,56(%esp)
	je .L53
	testl $1048576,%edi
	je .L53
	movl 80(%esp),%ebx
	movb (%ebx),%dl
	cmpl $0,16(%esp)
	je .L72
	movl 16(%esp),%eax
	decl %eax
	movl 60(%esp),%ecx
	btl %eax,%ecx
	setc %al
	movzbl %al,%eax
	jmp .L73
	.p2align 4,,7
.L72:
	movl 60(%esp),%eax
	shrl $31,%eax
.L73:
	andb $1,%al
	andb $-2,%dl
	movl 80(%esp),%ebx
	orb %al,%dl
	movb %dl,(%ebx)
	jmp .L53
	.p2align 4,,7
.L74:
	cmpl $0,16(%esp)
	jne .L75
	movl 80(%esp),%esi
	movl 60(%esp),%ebp
	shrl $1,%ebp
	movzbl (%esi),%eax
	sall $31,%eax
	orl %eax,%ebp
	cmpl $0,56(%esp)
	je .L53
	testl $1048576,%edi
	je .L53
	shrl $31,60(%esp)
	movb 60(%esp),%dl
	movb (%esi),%al
	andb $1,%dl
	jmp .L244
	.p2align 4,,7
.L75:
	movl 16(%esp),%ecx
	movl $32,%edx
	movl 60(%esp),%ebp
	subl %ecx,%edx
	shrl %cl,%ebp
	movl 60(%esp),%eax
	movl %edx,%ecx
	sall %cl,%eax
	orl %eax,%ebp
	cmpl $0,56(%esp)
	je .L53
	testl $1048576,%edi
	je .L53
	movl 16(%esp),%eax
	movl 80(%esp),%esi
	decl %eax
.L243:
	movl 60(%esp),%ebx
	btl %eax,%ebx
	setc %dl
	movb (%esi),%al
.L244:
	andb $254,%al
	orb %dl,%al
	movb %al,(%esi)
.L53:
	movl 28(%esp),%eax
	movl %edi,32(%esp)
	andl $15,%eax
	shrl $20,32(%esp)
	cmpl $15,%eax
	ja .L81
	jmp *.L138(,%eax,4)
	.p2align 4,,7
.section	.rodata
	.align 4
	.align 4
.L138:
	.long .L82
	.long .L83
	.long .L84
	.long .L90
	.long .L96
	.long .L102
	.long .L108
	.long .L114
	.long .L120
	.long .L121
	.long .L122
	.long .L128
	.long .L134
	.long .L135
	.long .L136
	.long .L137
.text
	.p2align 4,,7
.L82:
	pushl $.LC0
	shrl $10,%edi
	call puts
	andl $60,%edi
	andl 72(%esp),%ebp
	movl %ebp,68(%esp)
	movl 88(%esp),%ecx
	movl %ebp,%ebx
	movl %ebx,(%edi,%ecx)
	jmp .L245
	.p2align 4,,7
.L83:
	pushl $.LC1
	shrl $10,%edi
	call puts
	andl $60,%edi
	xorl 72(%esp),%ebp
	movl %ebp,68(%esp)
	movl 88(%esp),%esi
	movl %ebp,%ecx
	movl %ecx,(%edi,%esi)
	jmp .L245
	.p2align 4,,7
.L84:
	pushl $.LC2
	call puts
	movl 72(%esp),%ebx
	movl %ebx,68(%esp)
	subl %ebp,68(%esp)
	addl $4,%esp
	movl 32(%esp),%esi
	testl $1,%esi
	je .L103
	movl %ebp,24(%esp)
	movl 68(%esp),%edx
	notl 24(%esp)
	movl 64(%esp),%eax
	movl 24(%esp),%ecx
	movl $0,16(%esp)
	notl %eax
	movl %ecx,20(%esp)
	andl %ecx,%edx
	testl %edx,%eax
	jl .L87
	movl 68(%esp),%eax
	notl %eax
	andl %ebp,%eax
	testl %eax,64(%esp)
	jge .L86
.L87:
	movl $1,16(%esp)
.L86:
	movl 80(%esp),%ebx
	movb 16(%esp),%al
	andb $1,%al
	movb 1(%ebx),%dl
	salb $3,%al
	andb $247,%dl
	orb %al,%dl
	movb %dl,1(%ebx)
	xorl %eax,%eax
	movl 20(%esp),%esi
	testl %esi,68(%esp)
	jl .L95
	movl 64(%esp),%ecx
	testl %ecx,68(%esp)
	jl .L95
	movl 24(%esp),%ebx
	testl %ecx,%ebx
	jge .L94
	jmp .L95
	.p2align 4,,7
.L90:
	pushl $.LC3
	call puts
	movl %ebp,68(%esp)
	movl 72(%esp),%esi
	subl %esi,68(%esp)
	addl $4,%esp
	movl 32(%esp),%ecx
	testb $1,%cl
	je .L103
	movl %esi,%ebx
	movl %ebp,%edx
	movl 64(%esp),%eax
	movl %ebx,24(%esp)
	notl %eax
	notl 24(%esp)
	movl 24(%esp),%esi
	movl $0,16(%esp)
	movl %esi,20(%esp)
	andl %esi,%edx
	testl %edx,%eax
	jl .L93
	movl %ebp,%eax
	notl %eax
	andl %ebx,%eax
	testl %eax,64(%esp)
	jge .L92
.L93:
	movl $1,16(%esp)
.L92:
	movl 80(%esp),%ecx
	movb 16(%esp),%al
	andb $1,%al
	movb 1(%ecx),%dl
	salb $3,%al
	andb $247,%dl
	orb %al,%dl
	movb %dl,1(%ecx)
	xorl %eax,%eax
	testl %ebp,20(%esp)
	jl .L95
	testl %ebp,64(%esp)
	jl .L95
	movl 24(%esp),%ebx
	testl %ebx,64(%esp)
	jge .L94
.L95:
	movl $1,%eax
.L94:
	movl 80(%esp),%esi
	movb %al,%dl
	andb $1,%dl
	movb (%esi),%al
	andb $254,%al
	orb %dl,%al
	movb %al,(%esi)
	jmp .L103
	.p2align 4,,7
.L96:
	pushl $.LC4
	call puts
	movl 72(%esp),%esi
	addl %ebp,%esi
	movl %esi,68(%esp)
	addl $4,%esp
	movl 32(%esp),%ecx
	testb $1,%cl
	je .L115
	movl 64(%esp),%esi
	xorl %edx,%edx
	movl %esi,24(%esp)
	movl 68(%esp),%ebx
	notl 24(%esp)
	movl %ebx,16(%esp)
	movl 24(%esp),%ecx
	andl %ebp,16(%esp)
	movl %ecx,20(%esp)
	testl %ecx,16(%esp)
	jl .L99
	movl %ebp,%eax
	orl %ebx,%eax
	notl %eax
	testl %esi,%eax
	jge .L98
.L99:
	movl $1,%edx
.L98:
	movl 80(%esp),%ebx
	movb %dl,%al
	andb $1,%al
	salb $3,%al
	movb 1(%ebx),%dl
	andb $247,%dl
	orb %al,%dl
	movb %dl,1(%ebx)
	xorl %eax,%eax
	cmpl $0,16(%esp)
	jl .L119
	movl 20(%esp),%esi
	testl %esi,68(%esp)
	jl .L119
	testl %ebp,24(%esp)
	jge .L118
	jmp .L119
	.p2align 4,,7
.L102:
	pushl $.LC5
	call puts
	movl 84(%esp),%ecx
	movl 72(%esp),%edx
	addl %ebp,%edx
	movzbl (%ecx),%eax
	andl $1,%eax
	addl %eax,%edx
	movl %edx,68(%esp)
	addl $4,%esp
	movl 32(%esp),%ebx
	testb $1,%bl
	je .L103
	movl 64(%esp),%ecx
	xorl %edx,%edx
	movl %ecx,24(%esp)
	movl 68(%esp),%esi
	notl 24(%esp)
	movl %esi,16(%esp)
	movl 24(%esp),%ebx
	andl %ebp,16(%esp)
	movl %ebx,20(%esp)
	testl %ebx,16(%esp)
	jl .L105
	movl %ebp,%eax
	orl %esi,%eax
	notl %eax
	testl %ecx,%eax
	jge .L104
.L105:
	movl $1,%edx
.L104:
	movl 80(%esp),%esi
	movb %dl,%al
	andb $1,%al
	salb $3,%al
	movb 1(%esi),%dl
	andb $247,%dl
	orb %al,%dl
	movb %dl,1(%esi)
	xorl %eax,%eax
	cmpl $0,16(%esp)
	jl .L107
	movl 20(%esp),%ecx
	testl %ecx,68(%esp)
	jl .L107
	testl %ebp,24(%esp)
	jge .L106
.L107:
	movl $1,%eax
.L106:
	movl 80(%esp),%ebx
	movb %al,%dl
	andb $1,%dl
	movb (%ebx),%al
	andb $254,%al
	orb %dl,%al
	movb %al,(%ebx)
.L103:
	shrl $10,%edi
	movl 64(%esp),%ecx
	andl $60,%edi
	movl 84(%esp),%ebx
	movl %ecx,(%edi,%ebx)
	jmp .L81
	.p2align 4,,7
.L108:
	pushl $.LC6
	call puts
	movl 72(%esp),%edx
	addl $4,%esp
	subl %ebp,%edx
	movl 80(%esp),%esi
	testb $1,(%esi)
	sete %al
	movl 32(%esp),%ecx
	movzbl %al,%eax
	subl %eax,%edx
	movl %edx,64(%esp)
	testb $1,%cl
	je .L115
	movl %ebp,24(%esp)
	movl 68(%esp),%edx
	notl 24(%esp)
	movl 64(%esp),%eax
	movl 24(%esp),%ebx
	movl $0,16(%esp)
	notl %eax
	movl %ebx,20(%esp)
	andl %ebx,%edx
	testl %edx,%eax
	jl .L111
	movl 68(%esp),%eax
	notl %eax
	andl %ebp,%eax
	testl %eax,64(%esp)
	jge .L110
.L111:
	movl $1,16(%esp)
.L110:
	movl 80(%esp),%esi
	movb 16(%esp),%al
	andb $1,%al
	movb 1(%esi),%dl
	salb $3,%al
	andb $247,%dl
	orb %al,%dl
	movb %dl,1(%esi)
	xorl %eax,%eax
	movl 20(%esp),%ecx
	testl %ecx,68(%esp)
	jl .L119
	movl 64(%esp),%ebx
	testl %ebx,68(%esp)
	jl .L119
	movl 24(%esp),%esi
	testl %ebx,%esi
	jge .L118
	jmp .L119
	.p2align 4,,7
.L114:
	pushl $.LC7
	call puts
	movl %ebp,%edx
	subl 72(%esp),%edx
	addl $4,%esp
	movl 80(%esp),%ecx
	testb $1,(%ecx)
	sete %al
	movl 32(%esp),%ebx
	movzbl %al,%eax
	subl %eax,%edx
	movl %edx,64(%esp)
	testb $1,%bl
	je .L115
	movl 68(%esp),%esi
	movl %ebp,%edx
	movl $0,16(%esp)
	movl %esi,24(%esp)
	movl 64(%esp),%eax
	notl 24(%esp)
	notl %eax
	movl 24(%esp),%ecx
	movl %ecx,20(%esp)
	andl %ecx,%edx
	testl %edx,%eax
	jl .L117
	movl %ebp,%eax
	notl %eax
	andl %esi,%eax
	testl %eax,64(%esp)
	jge .L116
.L117:
	movl $1,16(%esp)
.L116:
	movl 80(%esp),%ebx
	movb 16(%esp),%al
	andb $1,%al
	movb 1(%ebx),%dl
	salb $3,%al
	andb $247,%dl
	orb %al,%dl
	movb %dl,1(%ebx)
	xorl %eax,%eax
	testl %ebp,20(%esp)
	jl .L119
	testl %ebp,64(%esp)
	jl .L119
	movl 24(%esp),%esi
	testl %esi,64(%esp)
	jge .L118
.L119:
	movl $1,%eax
.L118:
	movl 80(%esp),%ecx
	movb %al,%dl
	andb $1,%dl
	movb (%ecx),%al
	andb $254,%al
	orb %dl,%al
	movb %al,(%ecx)
.L115:
	shrl $10,%edi
	movl 64(%esp),%ebx
	andl $60,%edi
	movl 84(%esp),%esi
	movl %ebx,(%edi,%esi)
	jmp .L81
	.p2align 4,,7
.L120:
	pushl $.LC8
	call puts
	andl 72(%esp),%ebp
	movl %ebp,68(%esp)
	jmp .L245
	.p2align 4,,7
.L121:
	pushl $.LC9
	call puts
	xorl 72(%esp),%ebp
	movl %ebp,68(%esp)
	jmp .L245
	.p2align 4,,7
.L122:
	pushl $.LC10
	call puts
	movl 72(%esp),%ecx
	movl %ecx,68(%esp)
	subl %ebp,68(%esp)
	addl $4,%esp
	movl 32(%esp),%ebx
	testb $1,%bl
	je .L81
	movl %ebp,24(%esp)
	movl %ecx,%edx
	movl 64(%esp),%eax
	notl 24(%esp)
	notl %eax
	movl 24(%esp),%edi
	movl $0,16(%esp)
	andl %edi,%edx
	testl %edx,%eax
	jl .L125
	movl %ecx,%eax
	notl %eax
	andl %ebp,%eax
	testl %eax,64(%esp)
	jge .L124
.L125:
	movl $1,16(%esp)
.L124:
	movl 80(%esp),%esi
	movb 16(%esp),%al
	andb $1,%al
	movb 1(%esi),%dl
	salb $3,%al
	andb $247,%dl
	orb %al,%dl
	movb %dl,1(%esi)
	xorl %eax,%eax
	testl %edi,68(%esp)
	jl .L133
	movl 64(%esp),%ecx
	testl %ecx,68(%esp)
	jl .L133
	movl 24(%esp),%ebx
	testl %ecx,%ebx
	jge .L132
	jmp .L133
	.p2align 4,,7
.L128:
	pushl $.LC11
	call puts
	movl 72(%esp),%ecx
	addl %ebp,%ecx
	movl %ecx,68(%esp)
	addl $4,%esp
	movl 32(%esp),%ebx
	testb $1,%bl
	je .L81
	movl 64(%esp),%ecx
	xorl %edx,%edx
	movl %ecx,24(%esp)
	movl 68(%esp),%esi
	movl %esi,16(%esp)
	notl 24(%esp)
	andl %ebp,16(%esp)
	movl 24(%esp),%edi
	testl %edi,16(%esp)
	jl .L131
	movl %ebp,%eax
	orl %esi,%eax
	notl %eax
	testl %ecx,%eax
	jge .L130
.L131:
	movl $1,%edx
.L130:
	movl 80(%esp),%ebx
	movb %dl,%al
	andb $1,%al
	salb $3,%al
	movb 1(%ebx),%dl
	andb $247,%dl
	orb %al,%dl
	movb %dl,1(%ebx)
	xorl %eax,%eax
	cmpl $0,16(%esp)
	jl .L133
	testl %edi,68(%esp)
	jl .L133
	testl %ebp,24(%esp)
	jge .L132
.L133:
	movl $1,%eax
.L132:
	movl 80(%esp),%esi
	movb %al,%dl
	andb $1,%dl
	movb (%esi),%al
	andb $254,%al
	orb %dl,%al
	movb %al,(%esi)
	jmp .L81
	.p2align 4,,7
.L134:
	pushl $.LC12
	shrl $10,%edi
	call puts
	andl $60,%edi
	orl 72(%esp),%ebp
	movl %ebp,68(%esp)
	movl 88(%esp),%ecx
	movl %ebp,%ebx
	movl %ebx,(%edi,%ecx)
	jmp .L245
	.p2align 4,,7
.L135:
	pushl $.LC13
	shrl $10,%edi
	call puts
	andl $60,%edi
	movl %ebp,68(%esp)
	movl 88(%esp),%esi
	movl %ebp,%ecx
	addl $4,%esp
	movl %ecx,(%edi,%esi)
	jmp .L81
	.p2align 4,,7
.L136:
	pushl $.LC14
	notl %ebp
	call puts
	shrl $10,%edi
	movl %ebp,68(%esp)
	movl 72(%esp),%ebx
	andl %ebx,68(%esp)
	andl $60,%edi
	movl 68(%esp),%ecx
	movl 88(%esp),%esi
	movl %ecx,(%edi,%esi)
	jmp .L245
	.p2align 4,,7
.L137:
	pushl $.LC15
	notl %ebp
	call puts
	shrl $10,%edi
	movl %ebp,68(%esp)
	movl 88(%esp),%ebx
	andl $60,%edi
	movl %ebp,%esi
	movl %esi,(%edi,%ebx)
.L245:
	addl $4,%esp
.L81:
	movl 32(%esp),%ecx
	testb $1,%cl
	je .L149
	movl 80(%esp),%ebx
	cmpl $0,64(%esp)
	sete %al
	salb $6,%al
	movb (%ebx),%dl
	andb $191,%dl
	shrl $31,64(%esp)
	orb %al,%dl
	movb 64(%esp),%al
	jmp .L248
	.p2align 4,,7
.L51:
	movl %edi,%ebp
	andl $234881024,%ebp
	cmpl $167772160,%ebp
	jne .L142
	pushl $.LC16
	call puts
	movl %edi,%eax
	addl $4,%esp
	andl $16777215,%eax
	leal 0(,%eax,4),%ebp
	testl $8388608,%eax
	je .L143
	orl $-67108864,%ebp
.L143:
	testl $16777216,%edi
	je .L144
	movl 84(%esp),%ecx
	movl 60(%ecx),%eax
	movl %eax,56(%ecx)
.L144:
	movl 84(%esp),%ebx
	movl 60(%ebx),%eax
	movl %eax,%edx
	andl $67108860,%edx
	andl $-67108861,%eax
	leal 8(%edx,%ebp),%edx
	andl $67108860,%edx
	orl %edx,%eax
	movl %eax,60(%ebx)
	jmp .L16
	.p2align 4,,7
.L142:
	movl %edi,%eax
	andl $264241392,%eax
	cmpl $144,%eax
	jne .L146
	movl 84(%esp),%esi
	movl %edi,%eax
	andl $15,%eax
	movl (%esi,%eax,4),%ebp
	movl %edi,%eax
	shrl $6,%eax
	andl $60,%eax
	movl (%eax,%esi),%eax
	movl %eax,16(%esp)
	testl $2097152,%edi
	je .L147
	pushl $.LC17
	call puts
	movl %edi,%eax
	imull 20(%esp),%ebp
	shrl $10,%eax
	addl $4,%esp
	andl $60,%eax
	addl (%eax,%esi),%ebp
	jmp .L148
	.p2align 4,,7
.L147:
	pushl $.LC18
	call puts
	imull 20(%esp),%ebp
	addl $4,%esp
.L148:
	movl %edi,%eax
	movl 84(%esp),%ecx
	shrl $14,%eax
	andl $60,%eax
	movl %ebp,(%eax,%ecx)
	testl $1048576,%edi
	je .L149
	movl 80(%esp),%ebx
	testl %ebp,%ebp
	sete %al
	salb $6,%al
	shrl $31,%ebp
	movb (%ebx),%dl
	andb $191,%dl
	orb %al,%dl
	movl %ebp,%eax
.L248:
	andb $127,%dl
	salb $7,%al
	orb %al,%dl
	movb %dl,(%ebx)
.L149:
	movl 84(%esp),%esi
	addl $4,60(%esi)
	jmp .L16
	.p2align 4,,7
.L146:
	cmpl $67108864,%edx
	jne .L151
	movl %edi,48(%esp)
	movl 84(%esp),%ecx
	shrl $16,48(%esp)
	movl 48(%esp),%eax
	andl $15,%eax
	movl (%ecx,%eax,4),%eax
	movl %eax,52(%esp)
	testl $33554432,%edi
	je .L152
	movl %edi,%edx
	andl $4095,%edx
	movl %edx,%eax
	andl $15,%eax
	shrl $5,%edx
	movl (%ecx,%eax,4),%ebp
	movl %edx,%eax
	andl $3,%eax
	shrl $2,%edx
	cmpl $1,%eax
	je .L156
	jb .L154
	cmpl $2,%eax
	je .L159
	cmpl $3,%eax
	je .L164
	jmp .L169
	.p2align 4,,7
.L154:
	movl %ebp,44(%esp)
	testl %edx,%edx
	je .L169
	movl %edx,%ecx
	sall %cl,44(%esp)
	jmp .L169
	.p2align 4,,7
.L156:
	testl %edx,%edx
	setne %al
	movl %edx,%ecx
	movzbl %al,%eax
	shrl %cl,%ebp
	negl %eax
	movl %eax,44(%esp)
	andl %ebp,44(%esp)
	jmp .L169
	.p2align 4,,7
.L159:
	testl %edx,%edx
	je .L160
	movl %edx,%ecx
	sarl %cl,%ebp
	movl %ebp,44(%esp)
	jmp .L169
	.p2align 4,,7
.L160:
	testl %ebp,%ebp
	setl %al
	movzbl %al,%eax
	negl %eax
	movl %eax,44(%esp)
	jmp .L169
	.p2align 4,,7
.L164:
	testl %edx,%edx
	jne .L165
	movl 80(%esp),%ebx
	shrl $1,%ebp
	movzbl (%ebx),%eax
	sall $31,%eax
	orl %eax,%ebp
	movl %ebp,44(%esp)
	jmp .L169
	.p2align 4,,7
.L165:
	movl %ebp,44(%esp)
	movl %edx,%ecx
	shrl %cl,44(%esp)
	movl $32,%ecx
	subl %edx,%ecx
	sall %cl,%ebp
	orl %ebp,44(%esp)
	jmp .L169
	.p2align 4,,7
.L152:
	movl %edi,44(%esp)
	andl $4095,44(%esp)
.L169:
	testl $8388608,%edi
	jne .L170
	negl 44(%esp)
.L170:
	movl %edi,40(%esp)
	shrl $24,40(%esp)
	movl 40(%esp),%ebx
	testb $1,%bl
	je .L171
	movl 44(%esp),%esi
	addl %esi,52(%esp)
.L171:
	movl 52(%esp),%ecx
	pushl %ecx
	movl 92(%esp),%ebx
	pushl %ebx
	call memory_lookup
	movl %eax,24(%esp)
	movl %eax,%ebp
	movl %eax,%edx
	andl $-4,%ebp
	andl $3,%edx
	addl $8,%esp
	testl $1048576,%edi
	je .L172
	testl $4194304,%edi
	je .L173
	movl 16(%esp),%esi
	movl %edi,%eax
	shrl $10,%eax
	movl 84(%esp),%ecx
	andl $60,%eax
	movzbl (%esi),%edx
	movl %edx,(%eax,%ecx)
	jmp .L177
	.p2align 4,,7
.L173:
	testl %edx,%edx
	je .L175
	movl %edi,24(%esp)
	sall $3,%edx
	movl 84(%esp),%ebx
	shrl $10,24(%esp)
	movl $32,16(%esp)
	andl $60,24(%esp)
	subl %edx,16(%esp)
	movl %edx,20(%esp)
	movl (%ebp),%edx
	movl 16(%esp),%ecx
	movl %edx,%eax
	shrl %cl,%eax
	movl 24(%esp),%esi
	movl 20(%esp),%ecx
	sall %cl,%edx
	orl %edx,%eax
	movl %eax,(%esi,%ebx)
	jmp .L177
	.p2align 4,,7
.L175:
	movl %edi,%eax
	movl (%ebp),%edx
	shrl $10,%eax
	movl 84(%esp),%ecx
	andl $60,%eax
	movl %edx,(%eax,%ecx)
	jmp .L177
	.p2align 4,,7
.L172:
	testl $4194304,%edi
	je .L178
	movl 84(%esp),%ebx
	movl %edi,%eax
	shrl $10,%eax
	movl 16(%esp),%esi
	andl $60,%eax
	movb (%eax,%ebx),%al
	movb %al,(%esi)
	jmp .L177
	.p2align 4,,7
.L178:
	testl %edx,%edx
	je .L180
	movl 84(%esp),%ecx
	movl %edi,%eax
	movl $32,20(%esp)
	sall $3,%edx
	shrl $10,%eax
	subl %edx,20(%esp)
	movl %edx,16(%esp)
	andl $60,%eax
	movl (%eax,%ecx),%edx
	movl 20(%esp),%ecx
	movl %edx,%eax
	shrl %cl,%eax
	movl 16(%esp),%ecx
	sall %cl,%edx
	orl %edx,%eax
	jmp .L246
	.p2align 4,,7
.L180:
	movl 84(%esp),%ebx
	movl %edi,%eax
	shrl $10,%eax
	andl $60,%eax
	movl (%eax,%ebx),%eax
.L246:
	movl %eax,(%ebp)
.L177:
	movl 40(%esp),%esi
	testl $1,%esi
	jne .L182
	movl 52(%esp),%ebx
	movl 44(%esp),%ecx
	movl 48(%esp),%eax
	leal (%ebx,%ecx,4),%ebx
	andl $15,%eax
	movl %ebx,52(%esp)
	movl 84(%esp),%esi
	movl %ebx,(%esi,%eax,4)
.L182:
	testl $2097152,%edi
	je .L183
	andl $15,48(%esp)
	movl 52(%esp),%esi
	movl 48(%esp),%ecx
	movl 84(%esp),%ebx
	movl %esi,(%ebx,%ecx,4)
.L183:
	pushl $.LC19
	jmp .L247
	.p2align 4,,7
.L151:
	cmpl $134217728,%ebp
	jne .L185
	movl 84(%esp),%ecx
	movl %edi,%eax
	shrl $14,%eax
	andl $60,%eax
	movl (%eax,%ecx),%ebp
	pushl $.LC20
	call puts
	addl $4,%esp
	testl $8388608,%edi
	je .L186
	pushl %ebp
	movl 92(%esp),%ebx
	pushl %ebx
	call memory_lookup
	movl %eax,32(%esp)
	andb $252,32(%esp)
	addl $8,%esp
	testl $16777216,%edi
	je .L187
	movzwl %di,%esi
	movl $0,16(%esp)
	movl %esi,36(%esp)
	movl $0,20(%esp)
	.p2align 4,,7
.L191:
	movl 36(%esp),%ecx
	movl 16(%esp),%ebx
	btl %ebx,%ecx
	jnc .L190
	movl 24(%esp),%edx
	movl %edi,32(%esp)
	addl $4,%ebp
	addl $4,24(%esp)
	shrl $12,%edx
	movl 24(%esp),%eax
	shrl $20,32(%esp)
	shrl $12,%eax
	cmpl %eax,%edx
	je .L193
	pushl %ebp
	movl 92(%esp),%esi
	pushl %esi
	call memory_lookup
	movl %eax,32(%esp)
	addl $8,%esp
.L193:
	movl 32(%esp),%ecx
	testb $1,%cl
	je .L194
	movl 24(%esp),%ebx
	movl 84(%esp),%esi
	movl 20(%esp),%ecx
	movl (%ebx),%eax
	movl %eax,(%ecx,%esi)
	jmp .L190
	.p2align 4,,7
.L194:
	movl 84(%esp),%ebx
	movl 20(%esp),%esi
	movl 24(%esp),%ecx
	movl (%esi,%ebx),%eax
	movl %eax,(%ecx)
.L190:
	addl $4,20(%esp)
	incl 16(%esp)
	cmpl $15,16(%esp)
	jle .L191
	jmp .L207
	.p2align 4,,7
.L187:
	movzwl %di,%ebx
	movl $0,16(%esp)
	movl %ebx,36(%esp)
	shrl $20,%edi
	movl $0,20(%esp)
	.p2align 4,,7
.L201:
	movl 36(%esp),%esi
	movl 16(%esp),%ecx
	btl %ecx,%esi
	jnc .L200
	movl 24(%esp),%edx
	testl $1,%edi
	je .L203
	movl (%edx),%eax
	movl 84(%esp),%ebx
	movl 20(%esp),%esi
	movl %eax,(%esi,%ebx)
	jmp .L204
	.p2align 4,,7
.L203:
	movl 84(%esp),%ecx
	movl 20(%esp),%ebx
	movl 24(%esp),%esi
	movl (%ebx,%ecx),%eax
	movl %eax,(%esi)
.L204:
	addl $4,24(%esp)
	addl $4,%ebp
	movl 24(%esp),%eax
	shrl $12,%edx
	shrl $12,%eax
	cmpl %eax,%edx
	je .L200
	pushl %ebp
	movl 92(%esp),%ecx
	pushl %ecx
	call memory_lookup
	movl %eax,32(%esp)
	addl $8,%esp
.L200:
	addl $4,20(%esp)
	incl 16(%esp)
	cmpl $15,16(%esp)
	jle .L201
	jmp .L207
	.p2align 4,,7
.L186:
	pushl %ebp
	movl 92(%esp),%ebx
	pushl %ebx
	call memory_lookup
	movl %eax,32(%esp)
	andb $252,32(%esp)
	addl $8,%esp
	testl $16777216,%edi
	je .L208
	movzwl %di,%esi
	movl $15,16(%esp)
	movl %esi,36(%esp)
	movl $60,20(%esp)
	.p2align 4,,7
.L212:
	movl 36(%esp),%ecx
	movl 16(%esp),%ebx
	btl %ebx,%ecx
	jnc .L211
	movl 24(%esp),%edx
	movl %edi,32(%esp)
	addl $-4,%ebp
	addl $-4,24(%esp)
	shrl $12,%edx
	movl 24(%esp),%eax
	shrl $20,32(%esp)
	shrl $12,%eax
	cmpl %eax,%edx
	je .L214
	pushl %ebp
	movl 92(%esp),%esi
	pushl %esi
	call memory_lookup
	movl %eax,32(%esp)
	addl $8,%esp
.L214:
	movl 32(%esp),%ecx
	testb $1,%cl
	je .L215
	movl 24(%esp),%ebx
	movl 84(%esp),%esi
	movl 20(%esp),%ecx
	movl (%ebx),%eax
	movl %eax,(%ecx,%esi)
	jmp .L211
	.p2align 4,,7
.L215:
	movl 84(%esp),%ebx
	movl 20(%esp),%esi
	movl 24(%esp),%ecx
	movl (%esi,%ebx),%eax
	movl %eax,(%ecx)
.L211:
	addl $-4,20(%esp)
	decl 16(%esp)
	jns .L212
	jmp .L207
	.p2align 4,,7
.L208:
	movzwl %di,%ebx
	movl $15,16(%esp)
	movl %ebx,36(%esp)
	shrl $20,%edi
	movl $60,20(%esp)
	.p2align 4,,7
.L222:
	movl 36(%esp),%esi
	movl 16(%esp),%ecx
	btl %ecx,%esi
	jnc .L221
	movl 24(%esp),%edx
	testl $1,%edi
	je .L224
	movl (%edx),%eax
	movl 84(%esp),%ebx
	movl 20(%esp),%esi
	movl %eax,(%esi,%ebx)
	jmp .L225
	.p2align 4,,7
.L224:
	movl 84(%esp),%ecx
	movl 20(%esp),%ebx
	movl 24(%esp),%esi
	movl (%ebx,%ecx),%eax
	movl %eax,(%esi)
.L225:
	addl $-4,24(%esp)
	addl $-4,%ebp
	movl 24(%esp),%eax
	shrl $12,%edx
	shrl $12,%eax
	cmpl %eax,%edx
	je .L221
	pushl %ebp
	movl 92(%esp),%ecx
	pushl %ecx
	call memory_lookup
	movl %eax,32(%esp)
	addl $8,%esp
.L221:
	addl $-4,20(%esp)
	decl 16(%esp)
	jns .L222
.L207:
	movl 84(%esp),%ebx
	addl $4,60(%ebx)
	jmp .L16
	.p2align 4,,7
.L185:
	movl %edi,%eax
	andl $251658240,%eax
	cmpl $251658240,%eax
	jne .L229
	pushl $.LC21
	jmp .L247
	.p2align 4,,7
.L229:
	cmpl $201326592,%ebp
	jne .L231
	pushl $.LC22
	jmp .L247
	.p2align 4,,7
.L231:
	movl %edi,%eax
	andl $251658256,%eax
	cmpl $234881024,%eax
	jne .L233
	pushl $.LC23
	jmp .L247
	.p2align 4,,7
.L233:
	cmpl $234881040,%eax
	jne .L235
	pushl $.LC24
	jmp .L247
	.p2align 4,,7
.L235:
	movl %edi,%eax
	andl $263196656,%eax
	cmpl $16777360,%eax
	jne .L237
	pushl $.LC25
	jmp .L247
	.p2align 4,,7
.L237:
	andl $234881040,%edi
	cmpl $100663296,%edi
	jne .L239
	pushl $.LC26
	jmp .L247
	.p2align 4,,7
.L239:
	pushl $.LC27
.L247:
	call puts
	addl $4,%esp
.L16:
	popl %ebx
	popl %esi
	popl %edi
	popl %ebp
	addl $56,%esp
	ret
.Lfe1:
	.size	 decode,.Lfe1-decode
	.ident	"GCC: (GNU) pgcc-2.91.66 19990314 (egcs-1.1.2 release)"
