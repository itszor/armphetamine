rfp	.req	r9
sl	.req	r10
fp	.req	r11
ip	.req	r12
sp	.req	r13
lr	.req	r14
pc	.req	r15
gcc2_compiled.:
___gnu_compiled_c:
.text
	.align	0
LC0:
	.ascii	"Hello world\012\000"
	.align	0
	.global	_main
	.type	 _main,#function
_main:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 1, current_function_anonymous_args = 0
	mov	ip, sp
	stmfd	sp!, {fp, ip, lr, pc}
	sub	fp, ip, #4
	bl	___main
	ldr	r0, L6
	bl	_printf
	mov	r0, #0
	ldmea	fp, {fp, sp, pc}
L7:
	.align	0
L6:
	.word	LC0
Lfe1:
	.size	 _main,Lfe1-_main
