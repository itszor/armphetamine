.text
        .align  2
        .global simple
        .type    simple,function

simple:
	mov r0,#7
loop:
	mla r4,r3,r0,r4
	adds r2,r3,r4,lsl r5
	subs r0,r0,#1
	bgt loop - .
	mov pc,r14
	eor r5,r6,r7
	and r8,r9,r10
	mvn r2,r3,ror r4
