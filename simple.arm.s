.text
        .align  2
        .global simple
        .type    simple,function

simple:
	add pc,pc,r1,lsl #2
	mov r0,r0
	b one
	b two
one:
	mov r5,#7
	b done
two:
	mov r5,#13
done:
	mov pc,r14

