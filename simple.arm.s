.text
        .align  2
        .global simple
        .type    simple,function

simple:
	mov r0,#5
	add pc,pc,r0,lsl #2
	mov r2,r2
	mov r2,r2
	mov r2,r2
	mov r2,r2
	mov r2,r2
	
	mov pc,r14
