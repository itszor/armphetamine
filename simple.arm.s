.text
        .align  2
        .global simple
        .type    simple,function

simple:
	mov r2,#0
	mov r0,#100
loop2:
	mov r1,#99
loop:
	mla r2,r1,r0,r2
	subs r1,r1,#1
	bgt loop - .

	subs r0,r0,#1
	bgt loop2 - .

	mov pc,r14
