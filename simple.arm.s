.text
        .align  2
        .global simple
        .type    simple,function

simple:
	mov r0,#1024
	mov r2,#5
	ldr r1,[r0,#12]!
	add r2,r2,r2
