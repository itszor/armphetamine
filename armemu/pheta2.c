#include <stdarg.h>
#include <stdio.h>

#include "defs.h"
#include "machine.h"
#include "pheta2.h"
#include "cnew.h"
#include "execute.h"
#include "hash.h"
#include "block.h"

ph_block* ph2_newblock(uint5 startsize)
{
  ph_block* phb = cnew(ph_block);

	phb->base = cnewarray(uint5, startsize);
  phb->tempno = phb->length = 0;
  phb->size = startsize;

	return phb;
}

void ph2_deleteblock(ph_block* phb)
{
  free(phb->base);
  free(phb);
}

#define _C(X) pht_CONSTANT, (X)
#define _R(X) pht_REGISTER, (X)
#define _T(X) pht_TEMPORARY, (X)
#define _O(X) pht_CONSTANT, (((ph_block*)blk)->length+(X))
#define _F(X) pht_FLAG, (X)

void ph2_emit(ph_block* p, uint5 sets, uint5 requires, uint3 op, ...)
{
  ph_node* pn;
  va_list ap;

  void fill(ph_operand* x, va_list a)
  {
    x->type = va_arg(a, uint5);
    x->id = va_arg(a, uint5);
  }
  
  void empty(ph_operand* x)
  {
    x->type = pht_UNUSED;
  }

  if (++p->length == p->size) p->base = realloc(p->base, p->size*=2);
  
  pn = &p->base[p->length-1];
  
  pn->operator = op;
  pn->sets.value = sets;
  pn->requires.value = requires;
  pn->destroys.value = 0;
  
  va_start(ap, op);
  
  switch (op)
  {
    case op_MOV:
    case op_MVN:
    case op_RRX:
    case op_LDW:
    case op_LDB:
    fill(&pn->rd, ap);
    fill(&pn->rm, ap);
    empty(&pn->rn);
    empty(&pn->ro);
    break;
    
    case op_STW:
    case op_STB:
    case op_CMP:
    case op_CMN:
    case op_TEQ:
    case op_TST:
    empty(&pn->rd);
    fill(&pn->rm, ap);
    fill(&pn->rn, ap);
    empty(&pn->ro);
    break;
    
    case op_BEQ...op_BAL:
    empty(&pn->rd);
    fill(&pn->rm, ap);
    empty(&pn->rn);
    empty(&pn->ro);
    break;
    
    case op_SWI:
    case op_END:
    empty(&pn->rd);
    empty(&pn->rm);
    empty(&pn->rn);
    empty(&pn->ro);
    break;
    
    case op_SETPC:
    fill(&pn->rd, ap);
    empty(&pn->rm);
    empty(&pn->rn);
    empty(&pn->ro);
    break;
    
    case op_RECONSTRUCTPC:
    fill(&pn->rd, ap);
    fill(&pn->rm, ap);
    empty(&pn->rn);
    empty(&pn->ro);
    break;
    
    case op_XJMP:
    empty(&pn->rd);
    fill(&pn->rm, ap);
    empty(&pn->rn);
    empty(&pn->ro);
    break;
    
    default:
    fill(&pn->rd, ap);
    fill(&pn->rm, ap);
    fill(&pn->rn, ap);
    empty(&pn->ro);
  }
  
  va_end(ap);
}

uint5 ph2_temp(ph_block* p)
{
  return p->tempno++;
}

void ph2_dp(machineinfo* machine, instructionformat inst, void* blk)
{
  uint5 temp = inst.dp.operand2;
  uint5 shifttype = (temp>>5)&3, amount;
  uint3 regshift = (temp & 16) ? 1 : 0;
  const uint5 logic = 0xf303;
  const uint5 affectrd = 0xf0ff;
  int islogic = logic & (1<<inst.dp.opcode);
  sint5 shiftbyreg = -1;
  uint5 op2reg, rm = temp&15, t;
  
//  ph2_condition();

  if (regshift)
  {
    shiftbyreg = temp>>8;
    
    if (shifttype!=3)
    {
      t = ph2_temp(blk);
      ph2_emit(blk, 0, 0, op_AND, _T(t), _R(shiftbyreg), _C(0xff));
    }
    
    op2reg = ph2_temp(blk);
    
    switch (shifttype)
    {
      case 0:  // lsl
      {
        ph2_emit(blk, AUXCODE|GTCODE, 0, op_CMP, _T(t), _C(31));
        ph2_emit(blk, 0, AUXCODE|GTCODE, op_BGT, _O(XXX));
        if (islogic && inst.dp.s)
          ph2_emit(blk, CAFFECT, 0, op_LSL, _T(op2reg), _R(rm), _T(t));
        else
          ph2_emit(blk, 0, 0, op_LSL, _T(op2reg), _R(rm), _T(t));
        ph2_emit(blk, 0, 0, op_BAL, _O(YYY));
        ph2_emit(blk, AUXCODE|EQCODE, 0, op_CMP, _T(t), _C(32));
        ph2_emit(blk, 0, AUXCODE|EQCODE, op_BEQ, _O(XXX));
        ph2_emit(blk, 0, 0, op_MOV, _T(op2reg), _C(0));
        ph2_emit(blk, CSCODE, 0, op_MOV, _F(CFLAG), _C(0));
      }
      break;
    }
  }
  else  // immediate shift
  {
  }
}

void ph2_dp_guts(machineinfo* machine, instructionformat inst, ph_block* blk,
                 uint5 op2reg)
{
  
}
