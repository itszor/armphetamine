	.word 0xe91baff0
	msr spsr_fx,r13
	.word 0xe160f00d
	.word 0xe161f00d
	.word 0xe162f00d
	.word 0xe163f00d
	.word 0xe164f00d
	.word 0xe165f00d
	.word 0xe166f00d
	.word 0xe167f00d
        .word 0xe168f00d
        .word 0xe169f00d
        .word 0xe16af00d
        .word 0xe16bf00d
        .word 0xe16cf00d
        .word 0xe16df00d
        .word 0xe16ef00d
	.word 0xe16ff00d
	msr	CPSR_fsxc, sp
	msr	CPSR, sp
	msr	SPSR, sp
	msr	CPSR_flg, sp
