#include <stdio.h>

extern "C" {
#include "defs.h"
#include "pseudo.h"
#include "list.h"
}

#include "pheta3.h"
#include "decode4.h"

ph_node::ph_node(uint3 oper, uint5 s, uint5 r)
{
  op = oper;
  sets.value = s;
  requires.value = r;
  destroys.value = 0;
  rd = ph_null();
  rm = ph_null();
  rn = ph_null();
}

ph_node::ph_node(uint3 oper, uint5 s, uint5 r, ph_operand d1)
{
  op = oper;
  sets.value = s;
  requires.value = r;
  switch (oper)
  {
    case op_XJMP:
    case op_BEQ...op_BAL:
    rd = ph_null();
    rm = d1;
    rn = ph_null();
    break;
    
 /*   case op_SETPC:
    rd = d1;
    rm = ph_operand();
    rn = ph_operand();
    break;*/
    
    default:
    break;
  }
}

ph_node::ph_node(uint3 oper, uint5 s, uint5 r, ph_operand d1, ph_operand d2)
{
  op = oper;
  sets.value = s;
  requires.value = r;
  switch (oper)
  {
    case op_MOV:
    case op_MVN:
    case op_RRX:
    case op_LDW:
    case op_LDB:
    case op_RECONSTRUCTPC:
    rd = d1;
    rm = d2;
    rn = ph_null();
    break;
    
    case op_STW:
    case op_STB:
    case op_CMP:
    case op_CMN:
    case op_TEQ:
    case op_TST:
    rd = ph_null();
    rm = d1;
    rn = d2;
    break;
    
    default:
    break;
  }
}

ph_node::ph_node(uint3 oper, uint5 s, uint5 r, ph_operand d1, ph_operand d2,
                 ph_operand d3)
{
  op = oper;
  sets.value = s;
  requires.value = r;
  rd = d1;
  rm = d2;
  rn = d3;
}

ph_block::ph_block()
{
  base = end = 0;
  writeback = 0;
  tempno = 0;
}

ph_block::~ph_block()
{
  ph_node* n;
  
  while (base)
  {
    n = base->next;
    delete base;
    base = n;
  }
}

void ph_block::emit(ph_node* p)
{
  if (!base)
  {
    base = end = p;
  }
  while (writeback)
  {
    ph_address* ad = (ph_address*)writeback->data;
    ad->set(p);
    list_removehead(&writeback);
  }
  end->next = p;
  p->next = 0;
  end = p;
}

// oh dear, and this was a JOKE in intercal...
void ph_block::comefrom(ph_address* ad)
{
  list* newitem = list_add(&writeback);
  newitem->data = ad;
}

void ph_block::translate(machineinfo* machine, blockinfo* block, 
                         instructionformat* base)
{
  int i;
  instructionformat inst;
  
  for (i=0; i<block->length; i++)
  {
    switch (dec4_insttype(inst=base[i]))
    {
      case dec4_dp:     dp(machine, inst);     break;
      case dec4_dp_imm: dp_imm(machine, inst); break;
      case dec4_mul:    mul(machine, inst);    break;
      case dec4_sdt:    sdt(machine, inst);    break;
      case dec4_bdt:    bdt(machine, inst);    break;
      case dec4_bra:    bra(machine, inst);    break;
      case dec4_swi:    swi(machine, inst);    break;
      case dec4_cdt:    cdt(machine, inst);    break;
      case dec4_cdo:    cdo(machine, inst);    break;
      case dec4_crt:    crt(machine, inst);    break;
      case dec4_sds:    sds(machine, inst);    break;
      case dec4_und:    und(machine, inst);    break;
    }
  }
}

void ph_block::dp(machineinfo* machine, instructionformat inst)
{
  uint5 temp = inst.dp.operand2;
  uint5 shifttype = (temp>>5)&3, amount;
  uint3 regshift = (temp & 16) ? 1 : 0;
  const uint5 logic = 0xf303;
  const uint5 affectrd = 0xf0ff;
  int islogic = logic & (1<<inst.dp.opcode);
  sint5 shiftbyreg = -1;
  uint5 rm = temp&15, t;
  ph_operand op2;
  
//  ph2_condition();

  if (regshift)
  {
    ph_address* doshift, *noshift;
    shiftbyreg = temp>>8;

    op2 = _T(mktemp());
    
    t = mktemp();
    emit(new ph_node(op_AND, AUXCODE|NECODE, 0, _T(t), _R(shiftbyreg),
                     _C(0xff)));
    emit(new ph_node(op_BNE, 0, AUXCODE|NECODE, _A(&doshift)));
    emit(new ph_node(op_MOV, 0, 0, op2, _R(rm)));
    emit(new ph_node(op_BAL, 0, 0, _A(&noshift)));
    comefrom(doshift);

    switch (shifttype)
    {
      case 0:  // lsl
      {
        ph_address* l1, *l2, *l3, *l4;
        emit(new ph_node(op_CMP, AUXCODE|LTCODE|GTCODE, 0, _T(t), _C(32)));
        emit(new ph_node(op_BLT, 0, AUXCODE|LTCODE, _A(&l1)));
        emit(new ph_node(op_BGT, 0, AUXCODE|GTCODE, _A(&l2)));

        // shift equal to 32
        emit(new ph_node(op_MOV, 0, 0, op2, _C(0)));
        if (islogic && inst.dp.s)
          emit(new ph_node(op_AND, CFLG, 0, _F(CFLAG), _R(rm), _C(1)));
        emit(new ph_node(op_BAL, 0, 0, _A(&l3)));

        // shift less than 32
        comefrom(l1);
        if (islogic && inst.dp.s)
          emit(new ph_node(op_LSL, CFLG, 0, op2, _R(rm), _T(t)));
        else
          emit(new ph_node(op_LSL, 0, 0, op2, _R(rm), _T(t)));
        emit(new ph_node(op_BAL, 0, 0, _A(&l4)));

        comefrom(l2);
        // shift greater than 32
        emit(new ph_node(op_MOV, 0, 0, op2, _C(0)));
        if (islogic && inst.dp.s)
          emit(new ph_node(op_MOV, CFLG, 0, _F(CFLG), _C(0)));

        comefrom(l3);
        comefrom(l4);
      }
      break;
      
      case 1:  // lsr
      {
        ph_address* l1, *l2, *l3, *l4;
        emit(new ph_node(op_CMP, AUXCODE|LTCODE|GTCODE, 0, _T(t), _C(32)));
        emit(new ph_node(op_BLT, 0, AUXCODE|LTCODE, _A(&l1)));
        emit(new ph_node(op_BGT, 0, AUXCODE|GTCODE, _A(&l2)));

        // shift equal to 32
        emit(new ph_node(op_MOV, 0, 0, op2, _C(0)));
        if (islogic && inst.dp.s)
          emit(new ph_node(op_AND, CFLG, 0, _F(CFLG), _R(rm), _C(1U<<31)));
        emit(new ph_node(op_BAL, 0, 0, _A(&l3)));

        // shift less than 32
        comefrom(l1);
        if (islogic && inst.dp.s)
          emit(new ph_node(op_LSR, CFLG, 0, op2, _R(rm), _T(t)));
        else
          emit(new ph_node(op_LSR, 0, 0, op2, _R(rm), _T(t)));
        emit(new ph_node(op_BAL, 0, 0, _A(&l4)));

        comefrom(l2);
        // shift greater than 32
        emit(new ph_node(op_MOV, 0, 0, op2, _C(0)));
        if (islogic && inst.dp.s)
          emit(new ph_node(op_MOV, CFLG, 0, _F(CFLG), _C(0)));

        comefrom(l3);
        comefrom(l4);
      }
      break;
      
      case 2:  // asr
      {
        ph_address* l1, *l2;
        emit(new ph_node(op_CMP, AUXCODE|LTCODE|GTCODE, 0, _T(t), _C(32)));
        emit(new ph_node(op_BLT, 0, AUXCODE|LTCODE, _A(&l1)));

        // shift equal to 32
        emit(new ph_node(op_ASR, 0, 0, op2, _R(rm), _C(31)));
        if (islogic && inst.dp.s)
          emit(new ph_node(op_AND, CFLG, 0, _F(CFLG), _R(rm), _C(1U<<31)));
        emit(new ph_node(op_BAL, 0, 0, _A(&l2)));

        // shift less than 32
        comefrom(l1);
        if (islogic && inst.dp.s)
          emit(new ph_node(op_ASR, CFLG, 0, op2, _R(rm), _T(t)));
        else
          emit(new ph_node(op_ASR, 0, 0, op2, _R(rm), _T(t)));

        comefrom(l2);
      }
      break;
      
      case 3:  // ror
      {
        ph_address* l1, *l2;
        t = mktemp();
        emit(new ph_node(op_AND, 0, 0, _T(t), _R(shiftbyreg), _C(31)));
        if (islogic && inst.dp.s)
          emit(new ph_node(op_ROR, CFLG, 0, op2, _R(rm), _T(t)));
        else
          emit(new ph_node(op_ROR, 0, 0, op2, _R(rm), _T(t)));
      }
    }
    // if amount in register was zero...
    comefrom(noshift);
  }
  else  // immediate shift
  {
    amount = temp>>7;
    
    switch (shifttype)
    {
      case 0:  // lsl immediate
      {
        if (amount==0)
        {
          op2 = _R(rm);
        }
        else
        {
          op2 = _T(mktemp());
          if (islogic && inst.dp.s)
            emit(new ph_node(op_LSL, CFLG, 0, op2, _R(rm), _C(amount)));
          else
            emit(new ph_node(op_LSL, 0, 0, op2, _R(rm), _C(amount)));
        }
      }
      break;
      
      case 1:  // lsr immediate (0 means 32)
      {
        if (amount)
        {
          if (islogic && inst.dp.s)
            emit(new ph_node(op_LSR, CFLG, 0, op2, _R(rm), _C(amount)));
          else
            emit(new ph_node(op_LSR, 0, 0, op2, _R(rm), _C(amount)));
        }
        else  // shift by 32
        {
          if (islogic && inst.dp.s)
          {
            emit(new ph_node(op_AND, CFLG, 0, _F(CFLG), _R(rm), _C(1U<<31)));
            emit(new ph_node(op_MOV, 0, 0, op2, _C(0)));
          }
          else
            emit(new ph_node(op_MOV, 0, 0, op2, _C(0)));
        }
      }
      break;

      case 2:  // asr immediate (0 means 32)
      {
        if (amount)
        {
          if (islogic && inst.dp.s)
            emit(new ph_node(op_ASR, CFLG, 0, op2, _R(rm), _C(amount)));
          else
            emit(new ph_node(op_ASR, 0, 0, op2, _R(rm), _C(amount)));
        }
        else  // shift by 32
        {
          if (islogic && inst.dp.s)
          {
            emit(new ph_node(op_AND, CFLG, 0, _F(CFLG), _R(rm), _C(1U<<31)));
            emit(new ph_node(op_ASR, 0, 0, op2, _R(rm), _C(31)));
          }
          else
            emit(new ph_node(op_ASR, 0, 0, op2, _R(rm), _C(31)));
        }
      }
      break;
      
      case 3:  // ror immediate / rrx
      {
        if (amount)  // ror
        {
          if (islogic && inst.dp.s)
            emit(new ph_node(op_ROR, CFLG, 0, op2, _R(rm), _C(amount)));
          else
            emit(new ph_node(op_ROR, 0, 0, op2, _R(rm), _C(amount)));
        }
        else  // rrx
        {
          if (islogic && inst.dp.s)
            emit(new ph_node(op_RRX, CFLG, REALCCODE, op2, _R(rm)));
          else
            emit(new ph_node(op_RRX, 0, REALCCODE, op2, _R(rm)));
        }
      }
    }
  }
  dp_imm_guts(machine, inst, op2);
}

void ph_block::dp_imm(machineinfo* machine, instructionformat inst)
{
  ph_operand op2;
  uint5 value = inst.dp.operand2 & 0xff, amount = (inst.dp.operand2>>8)*2;
	const uint5 logic = 0xf303;
	int islogic = logic & (1<<inst.dp.opcode);
  
  // ph2_condition()
  
//  if (inst.dp.rn==reg_R15) ph2_reconstitutepc();
  op2 = _C(ROR(value, amount));
  
  if (inst.dp.s && islogic && amount)
    emit(new ph_node(op_MOV, CFLG, 0, _F(CFLG),
         _C((value & (1U<<amount-1)) ? 1 : 0)));
  
  dp_imm_guts(machine, inst, op2);
}

void ph_block::dp_imm_guts(machineinfo* machine, instructionformat inst,
                           ph_operand op2)
{
  uint3 sflagset = inst.dp.s;
  
  if (inst.dp.rd==15) inst.dp.s = 0;
  
  switch (inst.dp.opcode)
  {
    case dp_AND:
    {
      if (inst.dp.s)
        emit(new ph_node(op_AND, NFLG|ZFLG, 0, _R(inst.dp.rd),
             _R(inst.dp.rn), op2));
      else
        emit(new ph_node(op_AND, 0, 0, _R(inst.dp.rd), _R(inst.dp.rn), op2));
    }
    break;
    
    case dp_EOR:
    {
      if (inst.dp.s)
        emit(new ph_node(op_EOR, NFLG|ZFLG, 0, _R(inst.dp.rd),
             _R(inst.dp.rn), op2));
      else
        emit(new ph_node(op_EOR, 0, 0, _R(inst.dp.rd), _R(inst.dp.rn), op2));
    }
    break;
    
    case dp_SUB:
    {
      if (inst.dp.s)
        emit(new ph_node(op_SUB, CFLG|VFLG|NFLG|ZFLG, 0, _R(inst.dp.rd),
             _R(inst.dp.rn), op2));
      else
        emit(new ph_node(op_SUB, 0, 0, _R(inst.dp.rd), _R(inst.dp.rn), op2));
    }
    break;

    case dp_RSB:
    {
      if (inst.dp.s)
        emit(new ph_node(op_SUB, CFLG|VFLG|NFLG|ZFLG, 0, _R(inst.dp.rd),
             op2, _R(inst.dp.rn)));
      else
        emit(new ph_node(op_SUB, 0, 0, _R(inst.dp.rd), op2, _R(inst.dp.rn)));
    }
    break;

    case dp_ADD:
    {
      if (inst.dp.s)
        emit(new ph_node(op_ADD, CFLG|VFLG|NFLG|ZFLG, 0, _R(inst.dp.rd),
             _R(inst.dp.rn), op2));
      else
        emit(new ph_node(op_ADD, 0, 0, _R(inst.dp.rd), _R(inst.dp.rn), op2));
    }
    break;

    case dp_ADC:
    {
      if (inst.dp.s)
        emit(new ph_node(op_ADC, CFLG|VFLG|NFLG|ZFLG, REALCCODE, _R(inst.dp.rd),
             _R(inst.dp.rn), op2));
      else
        emit(new ph_node(op_ADC, 0, REALCCODE, _R(inst.dp.rd), _R(inst.dp.rn),
             op2));
    }
    break;

    case dp_SBC:
    {
      if (inst.dp.s)
        emit(new ph_node(op_SBC, CFLG|VFLG|NFLG|ZFLG, REALCCODE, _R(inst.dp.rd),
             _R(inst.dp.rn), op2));
      else
        emit(new ph_node(op_SBC, 0, REALCCODE, _R(inst.dp.rd), _R(inst.dp.rn),
             op2));
    }
    break;
    
    case dp_RSC:
    {
      if (inst.dp.s)
        emit(new ph_node(op_SBC, CFLG|VFLG|NFLG|ZFLG, REALCCODE, _R(inst.dp.rd),
             op2, _R(inst.dp.rn)));
      else
        emit(new ph_node(op_SBC, 0, REALCCODE, _R(inst.dp.rd), op2,
             _R(inst.dp.rn)));
    }
    break;
    
    case dp_TST:
    {
      if (inst.dp.s)
        emit(new ph_node(op_TST, NFLG|ZFLG, 0, _R(inst.dp.rn), op2));
      else
        fprintf(stderr, "Warning: TST with clear S bit\n");
    }
    break;

    case dp_TEQ:
    {
      if (inst.dp.s)
        emit(new ph_node(op_TEQ, NFLG|ZFLG, 0, _R(inst.dp.rn), op2));
      else
        fprintf(stderr, "Warning: TEQ with clear S bit\n");
    }
    break;

    case dp_CMP:
    {
      if (inst.dp.s)
        emit(new ph_node(op_CMP, CFLG|VFLG|NFLG|ZFLG, 0, _R(inst.dp.rn), op2));
      else
        fprintf(stderr, "Warning: CMP with clear S bit\n");
    }
    break;

    case dp_CMN:
    {
      if (inst.dp.s)
        emit(new ph_node(op_CMN, CFLG|VFLG|NFLG|ZFLG, 0, _R(inst.dp.rn), op2));
      else
        fprintf(stderr, "Warning: CMN with clear S bit\n");
    }
    break;

    case dp_ORR:
    {
      if (inst.dp.s)
        emit(new ph_node(op_OR, NFLG|ZFLG, 0, _R(inst.dp.rd),
             _R(inst.dp.rn), op2));
      else
        emit(new ph_node(op_OR, 0, 0, _R(inst.dp.rd), _R(inst.dp.rn), op2));
    }
    break;

    case dp_MOV:
    {
      if (inst.dp.s)
        emit(new ph_node(op_MOV, NFLG|ZFLG, 0, _R(inst.dp.rd), op2));
      else
        emit(new ph_node(op_MOV, 0, 0, _R(inst.dp.rd), op2));
    }
    break;
    
    case dp_BIC:
    {
      ph_temporary inv = _T(mktemp());
      emit(new ph_node(op_MVN, 0, 0, inv, op2));
      if (inst.dp.s)
        emit(new ph_node(op_AND, NFLG|ZFLG, 0, _R(inst.dp.rd), _R(inst.dp.rn),
             inv));
      else
        emit(new ph_node(op_AND, 0, 0, _R(inst.dp.rd), _R(inst.dp.rn), inv));
    }
    break;
    
    case dp_MVN:
    {
      if (inst.dp.s)
        emit(new ph_node(op_MVN, NFLG|ZFLG, 0, _R(inst.dp.rd), op2));
      else
        emit(new ph_node(op_MVN, 0, 0, _R(inst.dp.rd), op2));
    }
  }
  
  if (inst.dp.rd==15)
  {
    if (sflagset)
      emit(new ph_node(op_SETPC, 0, ALLCODES));
    else
      emit(new ph_node(op_SETPC, 0, 0));
  }
}

void ph_block::mul(machineinfo* machine, instructionformat inst) {}
void ph_block::sdt(machineinfo* machine, instructionformat inst) {}
void ph_block::bdt(machineinfo* machine, instructionformat inst) {}
void ph_block::bra(machineinfo* machine, instructionformat inst) {}
void ph_block::swi(machineinfo* machine, instructionformat inst) {}
void ph_block::cdt(machineinfo* machine, instructionformat inst) {}
void ph_block::cdo(machineinfo* machine, instructionformat inst) {}
void ph_block::crt(machineinfo* machine, instructionformat inst) {}
void ph_block::sds(machineinfo* machine, instructionformat inst) {}
void ph_block::und(machineinfo* machine, instructionformat inst) {}

int main(void)
{
  ph_node n1 = ph_node(op_AND, 0, 0, _T(0), _R(0), _C(5));
  
  return 0;
}
