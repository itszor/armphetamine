.text
        .align  2
        .global divide
        .type    divide,function

divide:
        stmfd r13!,{r4,r14}
        mov r2,r0
        mov r3,r1
 
        mov r4,#0
        cmp r2,#0
        rsblt r2,r2,#0
        sublt r4,r4,#1
        cmp r3,#0
        rsblt r3,r3,#0
        mvnlt r4,r4
 
        mov r0,#32
        mov r1,#0
divloop:
        adds r2,r2,r2
        adcs r1,r1,r1
        cmp r1,r3
        subge r1,r1,r3
        addge r2,r2,#1
        subs r0,r0,#1
        bne divloop
        cmp r4,#0
        rsbne r2,r2,#0
        mov r0,r2
 
        ldmfd r13!,{r4,pc}^
