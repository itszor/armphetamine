#include <stdio.h>
#include <stdarg.h>
#include <stddef.h>

#include "defs.h"
#include "codegen.h"
#include "generators.h"
#include "x86asm.h"
#include "pseudo.h"
#include "analyse.h"
#include "codegen_p.h"
#include "registers.h"
#include "nativesupport.h"
#include "allocate.h"

#define mk codegen_merge

// Instruction encodings are built here
// The encoding goes:
//   <operator> <destination> <arg1> <arg2> <arg3> ... <done>

hashtable* generators_register(void)
{
  hashtable* base = hash_new(50);
	
	mk(base, r_beq0, OP|op_BEQ, REGX, IMM, DONE);
	mk(base, r_bne0, OP|op_BNE, REGX, IMM, DONE);
	mk(base, r_bcs0, OP|op_BCS, REGX, IMM, DONE);
	mk(base, r_bcc0, OP|op_BCC, REGX, IMM, DONE);
	mk(base, r_bmi0, OP|op_BMI, REGX, IMM, DONE);
	mk(base, r_bpl0, OP|op_BPL, REGX, IMM, DONE);
	mk(base, r_bvs0, OP|op_BVS, REGX, IMM, DONE);
	mk(base, r_bvc0, OP|op_BVC, REGX, IMM, DONE);
	mk(base, r_bhi0, OP|op_BHI, REGX, REGY, IMM, DONE);
	mk(base, r_bls0, OP|op_BLS, REGX, REGY, IMM, DONE);
	mk(base, r_bge0, OP|op_BGE, REGX, REGY, IMM, DONE);
	mk(base, r_blt0, OP|op_BLT, REGX, REGY, IMM, DONE);
	mk(base, r_bgt0, OP|op_BGT, REGX, REGY, REGZ, IMM, DONE);
	mk(base, r_ble0, OP|op_BLE, REGX, REGY, REGZ, IMM, DONE);
	mk(base, r_bal0, OP|op_BAL, IMM, DONE);

  // overloaded for atomic cmp/bnnx in register shift
	mk(base, r_beq0, OP|op_BEQ, IMM, DONE);
	mk(base, r_bne0, OP|op_BNE, IMM, DONE);
	mk(base, r_bcs0, OP|op_BCS, IMM, DONE);
	mk(base, r_bcc0, OP|op_BCC, IMM, DONE);
	mk(base, r_bmi0, OP|op_BMI, IMM, DONE);
	mk(base, r_bpl0, OP|op_BPL, IMM, DONE);
	mk(base, r_bvs0, OP|op_BVS, IMM, DONE);
	mk(base, r_bvc0, OP|op_BVC, IMM, DONE);
	mk(base, r_bhi0, OP|op_BHI, IMM, DONE);
	mk(base, r_bls0, OP|op_BLS, IMM, DONE);
	mk(base, r_bge0, OP|op_BGE, IMM, DONE);
	mk(base, r_blt0, OP|op_BLT, IMM, DONE);
	mk(base, r_bgt0, OP|op_BGT, IMM, DONE);
	mk(base, r_ble0, OP|op_BLE, IMM, DONE);

#define SHIFTOP(t,T) \
	mk(base, r_##t##0, OP|op_##T, REGX, REGX, REGY, DONE); \
	mk(base, r_##t##1, OP|op_##T, REGX, REGY, REGZ, DONE); \
	mk(base, r_##t##2, OP|op_##T, REGX, REGX, OP|op_CONST, REGY, IMM, DONE); \
	mk(base, r_##t##3, OP|op_##T, REGX, REGY, OP|op_CONST, REGZ, IMM, DONE);
	
	SHIFTOP(lsl,LSL);
	SHIFTOP(lsr,LSR);
	SHIFTOP(asr,ASR);
	SHIFTOP(ror,ROR);

#undef SHIFTOP

	mk(base, r_rrx0, OP|op_RRX, REGX, REGX, REGY, DONE);
	mk(base, r_rrx1, OP|op_RRX, REGX, REGY, REGZ, DONE);

	mk(base, r_const0, OP|op_CONST, REGX, IMM, DONE);
	
	mk(base, r_mov0, OP|op_MOV, REGX, REGX, DONE);
	mk(base, r_mov1, OP|op_MOV, REGX, REGY, DONE);
	mk(base, r_mov2, OP|op_MOV, REGX, OP|op_CONST, REGY, IMM, DONE);
	
	mk(base, r_mvn0, OP|op_MVN, REGX, REGX, DONE);
	mk(base, r_mvn1, OP|op_MVN, REGX, REGY, DONE);
	mk(base, r_mvn2, OP|op_MVN, REGX, OP|op_CONST, REGY, IMM, DONE);
	
	mk(base, r_end0, OP|op_END, DONE);
  mk(base, r_setpc0, OP|op_SETPC, REGX, DONE);
  mk(base, r_xjmp0, OP|op_XJMP, IMM, DONE);
  mk(base, r_reconstructpc0, OP|op_RECONSTRUCTPC, REGX, REGY, REGZ, REGT, IMM, 
     DONE);
  
  mk(base, r_swi0, OP|op_SWI, IMM, DONE);

#define BINOP(t,T) \
  mk(base, r_##t##0, OP|op_##T, REGX, REGX, REGX, DONE); \
  mk(base, r_##t##1, OP|op_##T, REGX, REGX, REGY, DONE); \
	mk(base, r_##t##2, OP|op_##T, REGX, REGY, REGX, DONE); \
	mk(base, r_##t##3, OP|op_##T, REGX, REGY, REGY, DONE); \
	mk(base, r_##t##4, OP|op_##T, REGX, REGY, REGZ, DONE); \
	mk(base, r_##t##5, OP|op_##T, REGX, REGX, OP|op_CONST, REGY, IMM, DONE); \
	mk(base, r_##t##6, OP|op_##T, REGX, REGY, OP|op_CONST, REGZ, IMM, DONE); \
	mk(base, r_##t##7, OP|op_##T, REGX, OP|op_CONST, REGY, IMM, REGX, DONE); \
	mk(base, r_##t##8, OP|op_##T, REGX, OP|op_CONST, REGY, IMM, REGZ, DONE);

  BINOP(add,ADD);
//	BINOP(adc,ADC);
	BINOP(and,AND);
	BINOP(or,OR);
	BINOP(eor,EOR);
//	BINOP(sub,SUB);

#undef BINOP

  mk(base, r_adc0, OP|op_ADC, REGX, REGX, REGX, REGY, DONE);
  mk(base, r_adc1, OP|op_ADC, REGX, REGX, REGY, REGZ, DONE);
	mk(base, r_adc2, OP|op_ADC, REGX, REGY, REGX, REGZ, DONE);
	mk(base, r_adc3, OP|op_ADC, REGX, REGY, REGY, REGZ, DONE);
	mk(base, r_adc4, OP|op_ADC, REGX, REGY, REGZ, REGT, DONE);
	mk(base, r_adc5, OP|op_ADC, REGX, REGX, OP|op_CONST, REGY, IMM, REGZ, DONE);
	mk(base, r_adc6, OP|op_ADC, REGX, REGY, OP|op_CONST, REGZ, IMM, REGT, DONE);
	mk(base, r_adc7, OP|op_ADC, REGX, OP|op_CONST, REGY, IMM, REGX, REGZ, DONE);
	mk(base, r_adc8, OP|op_ADC, REGX, OP|op_CONST, REGY, IMM, REGZ, REGT, DONE);

  mk(base, r_sub0, OP|op_SUB, REGX, REGX, REGX, DONE);
  mk(base, r_sub1, OP|op_SUB, REGX, REGX, REGY, DONE);
	mk(base, r_sub2, OP|op_SUB, REGX, REGY, REGX, DONE);
	mk(base, r_sub3, OP|op_SUB, REGX, REGY, REGY, DONE);
	mk(base, r_sub4, OP|op_SUB, REGX, REGY, REGZ, DONE);
	mk(base, r_sub5, OP|op_SUB, REGX, REGX, OP|op_CONST, REGY, IMM, DONE);
	mk(base, r_sub6, OP|op_SUB, REGX, REGY, OP|op_CONST, REGZ, IMM, DONE);

  mk(base, r_sbc0, OP|op_SBC, REGX, REGX, REGX, REGY, DONE);
  mk(base, r_sbc1, OP|op_SBC, REGX, REGX, REGY, REGZ, DONE);
	mk(base, r_sbc2, OP|op_SBC, REGX, REGY, REGX, REGZ, DONE);
	mk(base, r_sbc3, OP|op_SBC, REGX, REGY, REGY, REGZ, DONE);
	mk(base, r_sbc4, OP|op_SBC, REGX, REGY, REGZ, REGT, DONE);
	mk(base, r_sbc5, OP|op_SBC, REGX, REGX, OP|op_CONST, REGY, IMM, REGZ, DONE);
	mk(base, r_sbc6, OP|op_SBC, REGX, REGY, OP|op_CONST, REGZ, IMM, REGT, DONE);

  mk(base, r_mul0, OP|op_MUL, REGX, REGX, REGX, DONE);
  mk(base, r_mul1, OP|op_MUL, REGX, REGX, REGY, DONE);
	mk(base, r_mul2, OP|op_MUL, REGX, REGY, REGX, DONE);
	mk(base, r_mul3, OP|op_MUL, REGX, REGY, REGY, DONE);
	mk(base, r_mul4, OP|op_MUL, REGX, REGY, REGZ, DONE);
	
#define CMPOP(t,T) \
  mk(base, r_##t##0, OP|op_##T, REGX, REGX, DONE); \
	mk(base, r_##t##1, OP|op_##T, REGX, REGY, DONE); \
	mk(base, r_##t##2, OP|op_##T, REGX, OP|op_CONST, REGY, IMM, DONE);
  
  CMPOP(cmp,CMP);
  CMPOP(cmn,CMN);
  CMPOP(teq,TEQ);
  CMPOP(tst,TST);

#undef CMPOP

	mk(base, r_cmp0, OP|op_CMP, REGX, REGX, DONE);
	mk(base, r_cmp1, OP|op_CMP, REGX, REGY, DONE);
	mk(base, r_cmp2, OP|op_CMP, REGX, OP|op_CONST, REGY, IMM, DONE);
	
	mk(base, r_ldw0, OP|op_LDW, REGX, REGX, DONE);
	mk(base, r_ldw1, OP|op_LDW, REGX, REGY, DONE);
	mk(base, r_ldb0, OP|op_LDB, REGX, REGX, DONE);
	mk(base, r_ldb1, OP|op_LDB, REGX, REGY, DONE);
	mk(base, r_stw0, OP|op_STW, REGX, REGX, DONE);
	mk(base, r_stw1, OP|op_STW, REGX, REGY, DONE);
	mk(base, r_stb0, OP|op_STB, REGX, REGX, DONE);
	mk(base, r_stb1, OP|op_STB, REGX, REGY, DONE);
	
	return base;
}

#undef mk

// generate function call sequence
void generators_fncall(nativeblockinfo* native, allocationinfo* alloc,
                       uint5 lock, void* fnaddr, sint5 dest, uint5 narg, ...)
{
  va_list ap;
  uint5 i;
  registermapping map, ra, rc, rd;
  uint5 rm, imm, eaxst=0, ecxst, edxst;
  uint5 alsolock = alloc->rctn[ireg_EAX].all;
  uint5 stacked;
  allocationinfo temp;

  #ifdef DEBUG    
  fprintf(stderr, "Locking %x\n", alsolock);
  #endif
/*
  if (dest != EAX) eaxst = allocate_spill(alloc, ireg_EAX, &ra, native);
  ecxst = allocate_spill(alloc, ireg_ECX, &rc, native);
  edxst = allocate_spill(alloc, ireg_EDX, &rd, native);
*/
  stacked = allocate_rpush(alloc, msk_EAX | msk_ECX | msk_EDX, native);
  temp = *alloc;
  
  map = allocate_reg(alloc, reg_T5, lock | ctn_T5 | alsolock, 
                     alloc_INVALID, native);
  
  va_start(ap, narg);

  for (i=0; i<narg; i++)
  {
    uint5 t = va_arg(ap, uint5);
    if (t==0)  // register
    {
      registermapping r = va_arg(ap, registermapping);

      if (r.parts.status != alloc_VALID)
      {
        uint5 mod = mod_DISP8, disp;
        rm = EBP;
        if (r.parts.original<CACHEABLE)
          disp = r.parts.original*4;
        else
        {
          fprintf(stderr, "Bad register\n");
          abort();
        }
        #define MODSIBDISP MODDISP8
        x86asm_out(native, PUSHlm, 0);
        #undef MODSIBDISP
      }
      else
      {
        rm = r.parts.mappedto;
        x86asm_out(native, PUSHlr, 0);
      }
    }
    else  // immediate
    {
      uint5 imm = va_arg(ap, uint5);
      x86asm_out(native, PUSHli, 0);
    }
  }
  va_end(ap);
    
  imm = (uint5)fnaddr;
  rm = map.parts.mappedto;
  x86asm_out(native, MOVlri, CALLnar, 0);
  // goodbye args
  imm = narg*4;
  rm = ESP;
  x86asm_out(native, ADDlri, 0);

  if (dest != EAX && dest != -1)
  {
    uint5 reg = EAX;
    rm = dest;
    x86asm_out(native, MOVlrr, 0);
  }
  
/*  if (edxst) allocate_unspill(alloc, &rd, native);
  if (ecxst) allocate_unspill(alloc, &rc, native);
  if (eaxst) allocate_unspill(alloc, &ra, native);*/
  allocate_rpop(&temp, stacked, native);
}

#define INMEM(X) (mstate->regset[(X)].parts.status==alloc_MEMORY)

#define ARG(X) mstate->regset[(X)].parts.mappedto

#define FORCEARG(X) \
  allocate_nsolidify(alloc, &mstate->regset[(X)], lock, native)

#define MEMSETUP(F,T) \
  uint5 mod = mod_DISP8; \
  uint5 disp = mstate->regset[(F)].parts.original*4; \
  uint5 rm = EBP; \
  uint5 reg = mstate->regset[(T)].parts.mappedto;

#define REGSETUP(F,T) \
  uint5 reg = mstate->regset[(F)].parts.mappedto; \
  uint5 rm = mstate->regset[(T)].parts.mappedto;

// Unfortunately, the formal parameters for these functions are obscured by
// the use of the GENFN macro. They are:
// void <fn> (machineinfo* machine, blockinfo* blk, allocationinfo* alloc,
//   matchstate* mstate, uint5 lock, uint5 flagsset)

#define JUMP(t,T,O) \
  GENFN(t##0) \
  { \
    hashentry* h = hash_lookup(basic, mstate->imm); \
    basicblockinfo* bl = h ? (basicblockinfo*) h->data : 0; \
    uint5 disp; \
    allocate_preflags(alloc, Jdef|Jundef, flagsset, native); \
    if (((imapinfo*)info)[mstate->imm].special.flag.allocated) \
    { \
      allocate_coerce(alloc, &bl->startalloc, native); \
      *alloc = bl->startalloc; \
    } \
    disp = codegen_getinteladdr(basic, mstate->imm, \
                                native->length+(O)); \
	  x86asm_out(native, T##nr, 0); \
    allocate_postflags(alloc, mstate, Jdef, flagsset, native); \
  }

// sometimes (bhi/bls) we need to invert the carry flag...
#define JUMP2(t,T,O) \
  GENFN(t##0) \
  { \
    hashentry* h = hash_lookup(basic, mstate->imm); \
    basicblockinfo* bl = h ? (basicblockinfo*) h->data : 0; \
    uint5 disp; \
    allocate_preflags(alloc, Jdef|Jundef, flagsset, native); \
    if (((imapinfo*)info)[mstate->imm].special.flag.allocated) \
    { \
      allocate_coerce(alloc, &bl->startalloc, native); \
      *alloc = bl->startalloc; \
    } \
    if (!alloc->vloc[reg_CPSRC].in.invertedflag) \
    { \
      x86asm_out(native, CMC, 0); \
      alloc->vloc[reg_CPSRC].in.invertedflag = 1; \
    } \
    disp = codegen_getinteladdr(basic, mstate->imm, native->length+(O)); \
	  x86asm_out(native, T##nr, 0); \
    allocate_postflags(alloc, mstate, Jdef, flagsset, native); \
  }

// the last arg is the offset of the address from the start of the instruction
JUMP(beq,JZ,2);
JUMP(bne,JNZ,2);
JUMP(bcs,JC,2);
JUMP(bcc,JNC,2);
JUMP(bmi,JS,2);
JUMP(bpl,JNS,2);
JUMP(bvs,JO,2);
JUMP(bvc,JNO,2);
JUMP2(bhi,JA,2);
JUMP2(bls,JBE,2);
JUMP(bge,JGE,2);
JUMP(blt,JL,2);
JUMP(bgt,JG,2);
JUMP(ble,JLE,2);
JUMP(bal,JMP,1);

#undef JUMP
#undef JUMP2

// regx <- regx shift regy
#define SHIFT0(t,T) \
	GENFN(t##0) \
	{ \
		uint5 temp, reg, rm = ARG(0); \
    registermapping f; \
    lock |= 1<<mstate->regset[1].parts.original; \
    FORCEARG(1); \
    f.parts.original = mstate->regset[1].parts.original; \
    f.parts.mapnum = ireg_ECX; \
    f.parts.mappedto = ECX; \
    f.parts.status = alloc_VALID; \
    allocate_preflags(alloc, T##def|T##undef, flagsset, native); \
		allocate_force(alloc, f, native); \
		x86asm_out(native, T##lrc, 0); \
    allocate_postflags(alloc, mstate, T##def, flagsset, native); \
	}

// regx <- regy shift regz
#define SHIFT1(t,T) \
	GENFN(t##1) \
	{ \
		uint5 temp, reg, rm = ARG(0); \
    registermapping f; \
    lock |= 1<<mstate->regset[2].parts.original; \
    reg = FORCEARG(1); \
    FORCEARG(2); \
    fprintf(stderr, "rm=%d, status(0)=%d\n", rm, mstate->regset[0].parts.status); \
    f.parts.original = mstate->regset[2].parts.original; \
    f.parts.mapnum = ireg_ECX; \
    f.parts.mappedto = ECX; \
    f.parts.status = alloc_VALID; \
    allocate_preflags(alloc, MOVdef|MOVundef|T##def|T##undef, flagsset, \
                      native); \
		x86asm_out(native, MOVlrr, 0); \
		allocate_force(alloc, f, native); \
		x86asm_out(native, T##lrc, 0); \
    allocate_postflags(alloc, mstate, T##def, flagsset, native); \
	}

// regx <- regx shift (regy <- #imm)
#define SHIFT2(t,T) \
	GENFN(t##2) \
	{ \
  	uint5 rm = ARG(0); \
		uint5 imm = mstate->imm; \
    allocate_preflags(alloc, T##def|T##undef, flagsset, native); \
		x86asm_out(native, T##lri, 0); \
    allocate_postflags(alloc, mstate, T##def, flagsset, native); \
	}

// regx <- regy shift (regz <- #imm)
#define SHIFT3(t,T) \
	GENFN(t##3) \
	{ \
  	uint5 rm = ARG(0); \
		uint5 reg = FORCEARG(1); \
		uint5 imm = mstate->imm; \
    allocate_preflags(alloc, MOVdef|MOVundef|T##def|T##undef, flagsset, \
                      native); \
		x86asm_out(native, MOVlrr, T##lri, 0); \
    allocate_postflags(alloc, mstate, T##def, flagsset, native); \
	}

#define SHIFT(t,T) \
  SHIFT0(t,T) \
	SHIFT1(t,T) \
	SHIFT2(t,T) \
	SHIFT3(t,T)

SHIFT(lsl,SHL);
SHIFT(lsr,SHR);
SHIFT(asr,SAR);
SHIFT(ror,ROR);

#undef SHIFT
#undef SHIFT0
#undef SHIFT1
#undef SHIFT2
#undef SHIFT3

// regx <- rrx regx
GENFN(rrx0)
{
  uint5 imm = 1;
	uint5 rm = FORCEARG(0);
  allocate_preflags(alloc, RCRdef|RCRundef, flagsset, native);
	x86asm_out(native, RCRlri, 0);
  allocate_postflags(alloc, mstate, RCRdef, flagsset, native);
}

// regx <- rrx regy
GENFN(rrx1)
{
  uint5 imm = 1;
	uint5 reg = FORCEARG(1);
	uint5 rm = ARG(0);
  allocate_preflags(alloc, MOVdef|MOVundef|RCRdef|RCRundef, flagsset,
                    native);
	x86asm_out(native, MOVlrr, RCRlri, 0);
  allocate_postflags(alloc, mstate, RCRdef, flagsset, native);
}

// regx <- #imm
GENFN(const0)
{
  uint5 rm = ARG(0);
	uint5 imm = mstate->imm;
  if (mstate->regset[0].parts.original==reg_CPSRC)  // ooh, special case
  {
    allocate_preflags(alloc, STCdef|STCundef, flagsset, native);
    if (imm==1)
      x86asm_out(native, STC, 0);
    else
      x86asm_out(native, CLC, 0);
    allocate_postflags(alloc, mstate, STCdef, flagsset, native);
  }
  else
  {
    allocate_preflags(alloc, MOVdef|MOVundef, flagsset, native);
    x86asm_out(native, MOVlri, 0);
    allocate_postflags(alloc, mstate, MOVdef, flagsset, native);
  }
}

// regx <- regx
GENFN(mov0)
{
  FORCEARG(0);
  allocate_preflags(alloc, MOVdef|MOVundef, flagsset, native);
  allocate_postflags(alloc, mstate, MOVdef, flagsset, native);
}

// regx <- regy
GENFN(mov1)
{
  if (mstate->regset[1].parts.original>=reg_CPSRC &&
      mstate->regset[1].parts.original<=reg_CPSRN)
  {
    uint5 mod, rm, disp, reg;

    // ensure this flag is spilled to memory
    allocate_preflags(alloc, 1<<(mstate->regset[1].parts.original-reg_CPSRC),
      0, native);

    mod = mod_DISP8;
    rm = EBP;
    switch (mstate->regset[1].parts.original)
    {
      case reg_CPSRC: disp = offsetof(registerinfo, cflag); break;
      case reg_CPSRV: disp = offsetof(registerinfo, vflag); break;
      case reg_CPSRN: disp = offsetof(registerinfo, nflag); break;
      case reg_CPSRZ: disp = offsetof(registerinfo, zflag); break;
    }
    reg = ARG(0);
    #define MODSIBDISP MODDISP8
    x86asm_out(native, MOVZXlrbm, 0);
    #undef MODSIBDISP
    allocate_postflags(alloc, mstate,
      1<<(mstate->regset[1].parts.original-reg_CPSRC), 0, native);
  }
  else
  {
    allocate_preflags(alloc, MOVdef|MOVundef, flagsset, native);
    if (INMEM(1))
    {
      MEMSETUP(1,0);
      #define MODSIBDISP MODDISP8
      x86asm_out(native, MOVlrm, 0);
      #undef MODSIBDISP
    }
    else
    {
      REGSETUP(1,0);
      x86asm_out(native, MOVlrr, 0);
    }
    allocate_postflags(alloc, mstate, MOVdef, flagsset, native);
  }
}

// regx <- (regy <- #imm)
GENFN(mov2)
{
  uint5 rm = ARG(0);
	uint5 imm = mstate->imm;
  if (mstate->regset[0].parts.original==reg_CPSRC)  // ooh, special case
  {
    allocate_preflags(alloc, STCdef|STCundef, flagsset, native);
    if (imm==1)
      x86asm_out(native, STC, 0);
    else
      x86asm_out(native, CLC, 0);
    allocate_postflags(alloc, mstate, STCdef, flagsset, native);
  }
  else
  {
    allocate_preflags(alloc, MOVdef|MOVundef, flagsset, native);
  	x86asm_out(native, MOVlri, 0);
    allocate_postflags(alloc, mstate, MOVdef, flagsset, native);
  }
}

// regx <- ~regx
GENFN(mvn0)
{
  uint5 rm = FORCEARG(0);
  allocate_preflags(alloc, NOTdef|NOTundef, flagsset, native);
	x86asm_out(native, NOTlr, 0);
  allocate_postflags(alloc, mstate, NOTdef, flagsset, native);
}

// regx <- ~regy
GENFN(mvn1)
{
  uint5 rm = ARG(0);
	uint5 reg = FORCEARG(1);
  allocate_preflags(alloc, NOTdef|NOTundef|MOVdef|MOVundef, flagsset,
                    native);
	x86asm_out(native, MOVlrr, NOTlr, 0);
  allocate_postflags(alloc, mstate, NOTdef, flagsset, native);
}

// regx <- ~(regy <- #imm)
GENFN(mvn2)
{
  uint5 rm = ARG(0);
	uint5 imm = ~mstate->imm;
  allocate_preflags(alloc, MOVdef|MOVundef, flagsset, native);
	x86asm_out(native, MOVlri, 0);
  allocate_postflags(alloc, mstate, MOVdef, flagsset, native);
}

GENFN(end0)
{
  allocate_preflags(alloc, ALLFLAGS, 0, native);
  allocate_reset(alloc, native);
  x86asm_out(native, RET, 0);
}

GENFN(setpc0)
{
  uint5 imm = 8, mod, disp;
  uint5 rm = FORCEARG(0);
  
  allocate_preflags(alloc, ADDdef|ADDundef, 0, native);
  x86asm_out(native, ADDlri, 0);
  allocate_postflags(alloc, mstate, ADDdef, 0, native);

  if (flagsset!=ALLFLAGS)
  {
    imm = ~0xfc000003;
    allocate_preflags(alloc, ANDdef|ANDundef, 0, native);
    x86asm_out(native, ANDlri, 0);
  }
  else
  {
    // Here, a (26-bit) program counter will have status bits included in the
    // main program counter. We need to split them back out into their
    // easy-access containers.
    #ifdef ARM26BIT
    allocate_preflags(alloc, BTdef|BTundef|ANDdef|ANDundef|SETdef|SETundef, 
      flagsset, native);

    #define FLAGFROMPC(f,o) \
      mod = mod_DISP8; \
      imm = (o); \
      rm = ARG(0); \
      x86asm_out(native, BTlri, 0); \
      disp = offsetof(registerinfo, f##flag); \
      rm = EBP; \
      x86asm_out(native, SETCm, 0);

    #define MODSIBDISP MODDISP8
    FLAGFROMPC(v,28);
    FLAGFROMPC(c,29);
    FLAGFROMPC(z,30);
    FLAGFROMPC(n,31);
    #undef MODSIBDISP
    #undef FLAGFROMPC

    rm = ARG(0);
    imm = ~0xfc000003;
    x86asm_out(native, ANDlri, 0);
    
//    allocate_postflags(alloc, mstate, 0, flagsset, native);
    alloc->vloc[reg_CPSRC].all
      = alloc->vloc[reg_CPSRV].all
      = alloc->vloc[reg_CPSRN].all
      = alloc->vloc[reg_CPSRZ].all = var_MEM;
    #else
//    #error "32-bit mode not supported yet"
    #endif
  }

  // call npostfix early to mark pc as valid, not in memory...
  allocate_npostfix(alloc, mstate);

  allocate_reset(alloc, native);
  // this is probably a no-op
  allocate_preflags(alloc, ALLFLAGS, 0, native);
  x86asm_out(native, RET, 0);
}

// if the value of the PC is used (eg, in an arithmetic op or if it's saved),
// it needs to have the correct status bits set (in 26-bit mode). Here we
// mock-up what the PC would have looked like when executing the original ARM
// code.
GENFN(reconstructpc0)
{
  allocate_preflags(alloc, ALLFLAGS, 0, native);
  generators_fncall(native, alloc, lock, (void*)&nativesupport_reconstructpc,
    ARG(0), 2, 1, machine->reg, 1, mstate->imm);
}

// jump directly to another block of x86 code
GENFN(xjmp0)
{
  blockinfo* destblk = (blockinfo*) mstate->imm;
  uint5 imm = (uint5)destblk->native->base;
  registermapping temp = allocate_reg(alloc, reg_T5, lock | ctn_T5,
                                      alloc_INVALID, native);
  uint5 rm = temp.parts.mappedto;
  allocate_preflags(alloc, ALLFLAGS, 0, native);
  allocate_reset(alloc, native);
  x86asm_out(native, MOVlri, JMPnar, 0);
}

GENFN(swi0)
{
  uint5 number = mstate->imm;
  // massage to state at start of this block (ie, start of any block)
  allocate_reset(alloc, native);
  allocate_preflags(alloc, ALLFLAGS, 0, native);
  generators_fncall(native, alloc, lock, (void*)&nativesupport_swi, -1, 2, 
    1, number, 1, machine);
}

// LOGICx macros should work OK for some commutative operators

#define INVCHECK

// regx <- regx op regx
#define LOGIC0(t,T) \
  GENFN(t##0) \
	{ \
	  uint5 reg = FORCEARG(0); \
		uint5 rm = reg; \
    allocate_preflags(alloc, T##def|T##undef, flagsset, native); \
    INVCHECK \
		x86asm_out(native, T##lrr, 0); \
    allocate_postflags(alloc, mstate, T##def, flagsset, native); \
	}

// regx <- regx op regy
#define LOGIC1(t,T) \
  GENFN(t##1) \
	{ \
    allocate_preflags(alloc, T##def|T##undef, flagsset, native); \
    FORCEARG(0); \
    INVCHECK \
    if (INMEM(1)) \
    { \
      MEMSETUP(1,0); \
      x86asm_out(native, T##lrm, 0); \
    } \
    else \
    { \
      REGSETUP(1,0); \
		  x86asm_out(native, T##lrr, 0); \
    } \
    allocate_postflags(alloc, mstate, T##def, flagsset, native); \
	}

// regx <- regy op regx
#define LOGIC2(t,T) \
  GENFN(t##2) \
	{ \
    allocate_preflags(alloc, T##def|T##undef, flagsset, native); \
    FORCEARG(0); \
    INVCHECK \
    if (INMEM(1)) \
    { \
      MEMSETUP(1,0); \
      x86asm_out(native, T##lrm, 0); \
    } \
    else \
    { \
      REGSETUP(1,0); \
		  x86asm_out(native, T##lrr, 0); \
    } \
    allocate_postflags(alloc, mstate, T##def, flagsset, native); \
	}

// regx <- regy op regy
#define LOGIC3(t,T) \
  GENFN(t##3) \
	{ \
	  uint5 reg = FORCEARG(1); \
		uint5 rm = ARG(0); \
    allocate_preflags(alloc, T##def|T##undef|MOVdef|MOVundef, flagsset, \
                      native); \
    INVCHECK \
    x86asm_out(native, MOVlrr, 0); \
		reg = rm; \
		x86asm_out(native, T##lrr, 0); \
    allocate_postflags(alloc, mstate, T##def, flagsset, native); \
	}

// regx <- regy op regz
#define LOGIC4(t,T) \
  GENFN(t##4) \
	{ \
    allocate_preflags(alloc, T##def|T##undef|MOVdef|MOVundef, flagsset, \
                      native); \
    INVCHECK \
    if (INMEM(1)) \
    { \
      MEMSETUP(1,0); \
	    x86asm_out(native, MOVlrm, 0); \
    } \
    else \
    { \
      REGSETUP(1,0); \
      x86asm_out(native, MOVlrr, 0); \
    } \
    if (INMEM(2)) \
    { \
      MEMSETUP(2,0); \
      x86asm_out(native, T##lrm, 0); \
    } \
    else \
    { \
      REGSETUP(2,0); \
		  x86asm_out(native, T##lrr, 0); \
    } \
    allocate_postflags(alloc, mstate, T##def, flagsset, native); \
	}

// regx <- regx op (regy <- #imm)
#define LOGIC5(t,T) \
  GENFN(t##5) \
	{ \
	  uint5 rm = FORCEARG(0); \
		uint5 imm = mstate->imm; \
    allocate_preflags(alloc, T##def|T##undef, flagsset, native); \
    INVCHECK \
		x86asm_out(native, T##lri, 0); \
    allocate_postflags(alloc, mstate, T##def, flagsset, native); \
	}

// regx <- regy op (regz <- #imm)
#define LOGIC6(t,T) \
  GENFN(t##6) \
	{ \
    uint5 reg = FORCEARG(1); \
		uint5 rm = ARG(0); \
		uint5 imm = mstate->imm; \
    allocate_preflags(alloc, T##def|T##undef|MOVdef|MOVundef, flagsset, \
                      native); \
    INVCHECK \
		x86asm_out(native, MOVlrr, T##lri, 0); \
    allocate_postflags(alloc, mstate, T##def, flagsset, native); \
	}

// regx <- (regy <- #imm) op regx
#define LOGIC7(t,T) \
  GENFN(t##7) \
	{ \
	  uint5 rm = FORCEARG(0); \
		uint5 imm = mstate->imm; \
    allocate_preflags(alloc, T##def|T##undef, flagsset, native); \
    INVCHECK \
		x86asm_out(native, T##lri, 0); \
    allocate_postflags(alloc, mstate, T##def, flagsset, native); \
	}

// regx <- (regy <- #imm) op regz
#define LOGIC8(t,T) \
  GENFN(t##8) \
	{ \
	  uint5 reg = FORCEARG(2); \
		uint5 rm = ARG(0); \
		uint5 imm = mstate->imm; \
    allocate_preflags(alloc, T##def|T##undef|MOVdef|MOVundef, flagsset, \
                      native); \
    INVCHECK \
		x86asm_out(native, MOVlrr, T##lri, 0); \
    allocate_postflags(alloc, mstate, T##def, flagsset, native); \
	}

#define LOGIC(t,T) \
  LOGIC0(t,T) \
	LOGIC1(t,T) \
	LOGIC2(t,T) \
	LOGIC3(t,T) \
	LOGIC4(t,T) \
	LOGIC5(t,T) \
	LOGIC6(t,T) \
	LOGIC7(t,T) \
	LOGIC8(t,T)

// expand for the following...
#define MODSIBDISP MODDISP8
LOGIC(add,ADD);
LOGIC(adc,ADC);
LOGIC(and,AND);
LOGIC(or,OR);
LOGIC(eor,XOR);

// regx <- regy op regx, non-commutative
#define NONCOM2(t,T) \
  GENFN(t##2) \
  { \
    uint5 reg; \
    uint5 rm; \
    allocate_preflags(alloc, XCHGdef|XCHGundef|T##def|T##undef, flagsset, \
                      native); \
    INVCHECK \
    rm = FORCEARG(1); \
    x86asm_out(native, PUSHlr, 0); \
    reg = rm; \
    rm = FORCEARG(0); \
    x86asm_out(native, XCHGlrr, T##lrr, 0); \
    rm = reg; \
    x86asm_out(native, POPlr, 0); \
    allocate_postflags(alloc, mstate, SUBdef, flagsset, native); \
  }

// although sub isn't commutative, these ones should be fine
LOGIC0(sub,SUB);
LOGIC1(sub,SUB);
NONCOM2(sub,SUB);
LOGIC3(sub,SUB);
LOGIC4(sub,SUB);
LOGIC5(sub,SUB);
LOGIC6(sub,SUB);

#undef INVCHECK
#define INVCHECK \
  if (!alloc->vloc[reg_CPSRC].in.invertedflag) \
  { \
    x86asm_out(native, CMC, 0); \
    alloc->vloc[reg_CPSRC].in.invertedflag = 1; \
  }

LOGIC0(sbc,SBB);
LOGIC1(sbc,SBB);
NONCOM2(sbc,SBB);
LOGIC3(sbc,SBB);
LOGIC4(sbc,SBB);
LOGIC5(sbc,SBB);
LOGIC6(sbc,SBB);

#undef INVCHECK
#define INVCHECK
#undef MODSIBDISP

/*
// regx <- (regy <- #imm) - regx
GENFN(sub7)
{
	uint5 rm = FORCEARG(0);
	uint5 imm = mstate->imm;
  allocate_preflags(alloc, NEGdef|NEGundef|SUBdef|SUBundef, flagsset,
                    native);
	x86asm_out(native, SUBlri, NEGlr, 0);
  allocate_postflags(alloc, mstate, SUBdef, flagsset, native,
                     &blk->natlength);
}

// regx <- (regy <- #imm) - regz
GENFN(sub8)
{
	uint5 reg = FORCEARG(2);
	uint5 rm = ARG(0);
	uint5 imm = mstate->imm;
  allocate_preflags(alloc, NEGdef|NEGundef|SUBdef|SUBundef, flagsset,
                    native);
	x86asm_out(native, MOVlrr, SUBlri, NEGlr, 0);
  allocate_postflags(alloc, mstate, SUBdef, flagsset, native,
                     &blk->natlength);
}
*/

#undef LOGIC
#undef LOGIC0
#undef LOGIC1
#undef LOGIC2
#undef LOGIC3
#undef LOGIC4
#undef LOGIC5
#undef LOGIC6
#undef LOGIC7
#undef LOGIC8

// regx <- regx * regx
GENFN(mul0)
{
  uint5 rm = FORCEARG(0);
  uint5 reg = rm;
  allocate_preflags(alloc, IMULdef|IMULundef, flagsset, native);
	x86asm_out(native, IMULlrr, 0);
  allocate_postflags(alloc, mstate, IMULdef, flagsset, native);
}

// regx <- regx * regy
GENFN(mul1)
{
  uint5 rm = FORCEARG(0);
  uint5 reg = FORCEARG(1);
  allocate_preflags(alloc, IMULdef|IMULundef, flagsset, native);
	x86asm_out(native, IMULlrr, 0);
  allocate_postflags(alloc, mstate, IMULdef, flagsset, native);
}

// regx <- regy * regx
GENFN(mul2)
{
  uint5 rm = FORCEARG(0);
  uint5 reg = FORCEARG(1);
  allocate_preflags(alloc, IMULdef|IMULundef, flagsset, native);
	x86asm_out(native, IMULlrr, 0);
  allocate_postflags(alloc, mstate, IMULdef, flagsset, native);
}

// regx <- regy * regy
GENFN(mul3)
{
  uint5 rm = ARG(0);
  uint5 reg = FORCEARG(1);
  allocate_preflags(alloc, MOVdef|MOVundef|IMULdef|IMULundef, flagsset,
                    native);
	x86asm_out(native, MOVlrr, 0);
	reg = rm;
	x86asm_out(native, IMULlrr, 0);
  allocate_postflags(alloc, mstate, IMULdef, flagsset, native);
}

// regx <- regy * regz
GENFN(mul4)
{
  uint5 rm = ARG(0);
	uint5 reg = FORCEARG(1);
  allocate_preflags(alloc, MOVdef|MOVundef|IMULdef|IMULundef, flagsset,
                    native);
	x86asm_out(native, MOVlrr, 0);
	reg = FORCEARG(2);
	x86asm_out(native, IMULlrr, 0);
  allocate_postflags(alloc, mstate, IMULdef, flagsset, native);
}

// regx cmp regx
#define COMPARE0(t,T) \
  GENFN(t##0) \
  { \
    uint5 rm = FORCEARG(0); \
	  uint5 reg = rm; \
    allocate_preflags(alloc, T##def|T##undef, flagsset, native); \
	  x86asm_out(native, T##lrr, 0); \
    allocate_postflags(alloc, mstate, T##def, flagsset, native); \
  }

// regx cmp regy
#define COMPARE1(t,T) \
  GENFN(t##1) \
  { \
    allocate_preflags(alloc, T##def|T##undef, flagsset, native); \
    FORCEARG(0); \
    if (INMEM(1)) \
    { \
      MEMSETUP(1,0); \
      x86asm_out(native, T##lrm, 0); \
    } \
    else \
    { \
      REGSETUP(1,0); \
      x86asm_out(native, T##lrr, 0); \
    } \
    allocate_postflags(alloc, mstate, T##def, flagsset, native); \
  }

// regx cmp (regy <- #imm)
#define COMPARE2(t,T) \
  GENFN(t##2) \
  { \
    uint5 rm = FORCEARG(0); \
	  uint5 imm = mstate->imm; \
    allocate_preflags(alloc, T##def|T##undef, flagsset, native); \
	  x86asm_out(native, T##lri, 0); \
    allocate_postflags(alloc, mstate, T##def, flagsset, native); \
  }

#define COMPARE(t,T) \
  COMPARE0(t,T) \
  COMPARE1(t,T) \
  COMPARE2(t,T)

// null <- regx cmp regx
#define COMPAREB0(t,T) \
  GENFN(t##0) \
  { \
    uint5 reg = FORCEARG(0); \
	  registermapping temp = allocate_reg(alloc, reg_T5, lock | ctn_T5, \
                                        alloc_INVALID, native); \
    uint5 rm = temp.parts.mappedto; \
    allocate_preflags(alloc, T##def|T##undef, flagsset, native); \
	  x86asm_out(native, MOVlrr, 0); \
    reg = rm; \
    x86asm_out(native, T##lrr, 0); \
    allocate_postflags(alloc, mstate, T##def, flagsset, native); \
  }

// null <- regx cmp regy
#define COMPAREB1(t,T) \
  GENFN(t##1) \
  { \
    registermapping temp = allocate_reg(alloc, reg_T5, lock | ctn_T5, \
                                        alloc_INVALID, native); \
    allocate_preflags(alloc, T##def|T##undef|MOVdef|MOVundef, flagsset, \
                      native); \
    mstate->regset[2] = mstate->regset[1]; \
    mstate->regset[1] = mstate->regset[0]; \
    mstate->regset[0] = temp; \
    INVCHECK \
    if (INMEM(1)) \
    { \
      MEMSETUP(1,0); \
	    x86asm_out(native, MOVlrm, 0); \
    } \
    else \
    { \
      REGSETUP(1,0); \
      x86asm_out(native, MOVlrr, 0); \
    } \
    if (INMEM(2)) \
    { \
      MEMSETUP(2,0); \
      x86asm_out(native, T##lrm, 0); \
    } \
    else \
    { \
      REGSETUP(2,0); \
		  x86asm_out(native, T##lrr, 0); \
    } \
    allocate_postflags(alloc, mstate, T##def, flagsset, native); \
    fprintf(stderr, "%c%c%c%c\n", alloc->vloc[reg_CPSRC].in.mem?'C':'c', \
                                  alloc->vloc[reg_CPSRV].in.mem?'V':'v', \
                                  alloc->vloc[reg_CPSRN].in.mem?'N':'n', \
                                  alloc->vloc[reg_CPSRZ].in.mem?'Z':'z'); \
  }

// null <- regx cmp (regy <- #imm)
#define COMPAREB2(t,T) \
  GENFN(t##2) \
  { \
    uint5 reg = FORCEARG(0); \
		registermapping temp = allocate_reg(alloc, reg_T5, lock | ctn_T5, \
                                        alloc_INVALID, native); \
		uint5 imm = mstate->imm, rm = temp.parts.mappedto; \
    allocate_preflags(alloc, T##def|T##undef|MOVdef|MOVundef, flagsset, \
                      native); \
    INVCHECK \
		x86asm_out(native, MOVlrr, T##lri, 0); \
    allocate_postflags(alloc, mstate, T##def, flagsset, native); \
  }

#define COMPAREB(t,T) \
  COMPAREB0(t,T) \
  COMPAREB1(t,T) \
  COMPAREB2(t,T)

#define MODSIBDISP MODDISP8
COMPARE(cmp,CMP);
COMPARE(tst,TEST);
// no direct analogues for these two, unfortunately
COMPAREB(teq,XOR);
COMPAREB(cmn,ADD);
#undef MODSIBDISP

#define LOAD(n,f,s,d) \
  GENFN(n) \
  { \
    generators_fncall(native, alloc, lock, (void*)&(f), ARG(s), 2, \
      0, mstate->regset[d], 1, machine->mem); \
  }

// regx <- !regx
LOAD(ldw0, nativesupport_readword, 0, 0);

// regx <- !regy
LOAD(ldw1, nativesupport_readword, 0, 1);

// regx <- ?regx
LOAD(ldb0, nativesupport_readbyte, 0, 0);

// regx <- ?regy
LOAD(ldb1, nativesupport_readbyte, 0, 1);

#define STORE(n,f,s,d) \
  GENFN(n) \
  { \
    generators_fncall(native, alloc, lock, (void*)&(f), -1, 3, \
      0, mstate->regset[d], \
      0, mstate->regset[s], \
      1, machine->mem); \
  }

// !regx <- regx
STORE(stw0, nativesupport_writeword, 0, 0);

// !regx <- regy
STORE(stw1, nativesupport_writeword, 0, 1);

// ?regx <- regx
STORE(stb0, nativesupport_writebyte, 0, 0);

// ?regx <- regy
STORE(stb1, nativesupport_writebyte, 0, 1);

