.text
        .align  2
        .global simple
        .type    simple,function

simple:
	mov r0,#15
loop:
	ldr r4,[r2,r3]
	add r3,r4,r5,lsl r6
	mla r0,r1,r6,r7
	mla r7,r1,r3,r7
	add r3,r4,r3,lsr r7
	subs r0,r0,r1
	bgt loop - .

	mov pc,r14
