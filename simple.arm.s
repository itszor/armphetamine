.text
        .align  2
        .global simple
        .type    simple,function

simple:
	mov r0,#7
loop:
	and r1,r2,r0
	sub r2,r3,r0
	sbc r3,r4,r0
	mla r5,r6,r7,r0
	rsc r5,r6,#0
	orr r6,r7,r8
	subs r0,r0,#1
	bgt loop - .
