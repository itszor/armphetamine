.text
        .align  2
        .global simple
        .type    simple,function

simple:
	mov r0,#5
	adds pc,r0,pc,lsl #2
	mov r2,r2
	mov r2,r2
	mov r2,r2
	mov r2,r2
	mov r2,r2
	
	mov pc,r14
