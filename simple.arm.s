.text
        .align  2
        .global simple
        .type    simple,function

simple:
	mov r0,#100
loop:
	adds r1,r1,r0
	adceq r2,r2,r1
	subs r0,r0,#1
	bge loop-.
	mov r2,#200
	movs pc,r14
