.text
        .align  2
        .global simple
        .type    simple,function

simple:
	strb r0,[r2]
	movs r2,r3,lsl #5
	ldr r1,[r0,#12]!
	addeq r2,r2,r2
