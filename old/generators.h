#ifndef GENERATORS_H
#define GENERATORS_H 1

#include "codegen.h"

extern hashtable* generators_register(void);
extern void generators_fncall(nativeblockinfo* native, allocationinfo* alloc,
                              uint5 lock, void* fnaddr, sint5 dest, uint5 narg, 
                              ...);

// code generator function prototypes

extern GENFN(end0);
extern GENFN(setpc0);
extern GENFN(xjmp0);
extern GENFN(swi0);
extern GENFN(reconstructpc0);

extern GENFN(beq0);
extern GENFN(bne0);
extern GENFN(bcs0);
extern GENFN(bcc0);
extern GENFN(bmi0);
extern GENFN(bpl0);
extern GENFN(bvs0);
extern GENFN(bvc0);
extern GENFN(bhi0);
extern GENFN(bls0);
extern GENFN(bge0);
extern GENFN(blt0);
extern GENFN(bgt0);
extern GENFN(ble0);
extern GENFN(bal0);

extern GENFN(lsl0);
extern GENFN(lsl1);
extern GENFN(lsl2);
extern GENFN(lsl3);

extern GENFN(lsr0);
extern GENFN(lsr1);
extern GENFN(lsr2);
extern GENFN(lsr3);

extern GENFN(asr0);
extern GENFN(asr1);
extern GENFN(asr2);
extern GENFN(asr3);

extern GENFN(ror0);
extern GENFN(ror1);
extern GENFN(ror2);
extern GENFN(ror3);

extern GENFN(rrx0);
extern GENFN(rrx1);

extern GENFN(const0);

extern GENFN(mov0);
extern GENFN(mov1);
extern GENFN(mov2);

extern GENFN(mvn0);
extern GENFN(mvn1);
extern GENFN(mvn2);

extern GENFN(add0);
extern GENFN(add1);
extern GENFN(add2);
extern GENFN(add3);
extern GENFN(add4);
extern GENFN(add5);
extern GENFN(add6);
extern GENFN(add7);
extern GENFN(add8);

extern GENFN(adc0);
extern GENFN(adc1);
extern GENFN(adc2);
extern GENFN(adc3);
extern GENFN(adc4);
extern GENFN(adc5);
extern GENFN(adc6);
extern GENFN(adc7);
extern GENFN(adc8);

extern GENFN(and0);
extern GENFN(and1);
extern GENFN(and2);
extern GENFN(and3);
extern GENFN(and4);
extern GENFN(and5);
extern GENFN(and6);
extern GENFN(and7);
extern GENFN(and8);

extern GENFN(or0);
extern GENFN(or1);
extern GENFN(or2);
extern GENFN(or3);
extern GENFN(or4);
extern GENFN(or5);
extern GENFN(or6);
extern GENFN(or7);
extern GENFN(or8);

extern GENFN(eor0);
extern GENFN(eor1);
extern GENFN(eor2);
extern GENFN(eor3);
extern GENFN(eor4);
extern GENFN(eor5);
extern GENFN(eor6);
extern GENFN(eor7);
extern GENFN(eor8);

extern GENFN(mul0);
extern GENFN(mul1);
extern GENFN(mul2);
extern GENFN(mul3);
extern GENFN(mul4);

extern GENFN(sub0);
extern GENFN(sub1);
extern GENFN(sub2);
extern GENFN(sub3);
extern GENFN(sub4);
extern GENFN(sub5);
extern GENFN(sub6);

extern GENFN(sbc0);
extern GENFN(sbc1);
extern GENFN(sbc2);
extern GENFN(sbc3);
extern GENFN(sbc4);
extern GENFN(sbc5);
extern GENFN(sbc6);

extern GENFN(cmp0);
extern GENFN(cmp1);
extern GENFN(cmp2);

extern GENFN(cmn0);
extern GENFN(cmn1);
extern GENFN(cmn2);

extern GENFN(tst0);
extern GENFN(tst1);
extern GENFN(tst2);

extern GENFN(teq0);
extern GENFN(teq1);
extern GENFN(teq2);

extern GENFN(ldw0);
extern GENFN(ldw1);

extern GENFN(ldb0);
extern GENFN(ldb1);

extern GENFN(stw0);
extern GENFN(stw1);

extern GENFN(stb0);
extern GENFN(stb1);

#endif
