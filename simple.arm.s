.text
        .align  2
        .global simple
        .type    simple,function

simple:
	movs r1,#1024
	beq true
false:
	mla r2,r1,r1,r2
	bal done
true:
	subs r1,r1,#1
done:
	rsb r8,r8,#0
	mov pc,r14

