.text
        .align  2
        .global simple
        .type    simple,function

simple:
	mov r0,#1024
	ldr r1,[r0,#12]!
	mov pc,r14
