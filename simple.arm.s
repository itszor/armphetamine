.text
        .align  2
        .global simple
        .type    simple,function

simple:
	mov r0,#7
loop:
	and r1,r2,r0
	teq r2,r3
	sbc r3,r4,r0,rrx
	mla r5,r6,r7,r0
	rsc r5,r6,#0
	sub r0,r0,#1
	cmp r0,#6
	bgt loop - .
