extern "C" {
#include "defs.h"
#include "list.h"
}

#include "pheta3.h"
#include "emit3.h"

p86_node::p86_node(p86_opcode o, uint5 s, uint5 r)
{
  op = o;
  rd = ph_null();
  rs = ph_null();
  sets = s;
  requires = r;
}

p86_node::p86_node(p86_opcode o, uint5 s, uint5 r, ph_operand reg)
{
  op = o;
  rd = reg;
  rs = ph_null();
  sets = s;
  requires = r;
}

p86_node::p86_node(p86_opcode o, uint5 s, uint5 r, ph_operand source,
                   ph_operand dest)
{
  op = o;
  rd = dest;
  rs = source;
  sets = s;
  requires = r;
}

p86_block::p86_block()
{
  base = end = 0;
  reloc = 0;
}

p86_block::~p86_block()
{
}

void p86_block::emit(p86_node* n)
{
  if (!base)
  {
    base = end = n;
  }
  end->next = n;
  n->next = 0;
  end = n;
}

// do a dumb translation
void p86_block::translate(ph_block* in)
{
  uint5 none = 0;

  for (ph_node* node = in->start(); node; node = node->next)
  {
    switch (node->getop())
    {
      case op_BEQ:
	    case op_BNE:
	    case op_BCS:
	    case op_BCC:
	    case op_BMI:
	    case op_BPL:
	    case op_BVS:
	    case op_BVC:
	    case op_BHI:
	    case op_BLS:
	    case op_BGE:
	    case op_BLT:
	    case op_BGT:
	    case op_BLE:
	    case op_BAL:
      break;
      
	    case op_LSL:
      {
        ph_temporary t = _T(in->mktemp());
        emit(new p86_node(p86_MOV, none, none, node->getrm(), t));
        emit(new p86_node(p86_SHL, node->getsetcc(), node->getrequiredcc(),
             node->getrn(), t));
        emit(new p86_node(p86_MOV, none, none, t, node->getrd()));
      }
      break;
      
	    case op_LSR:
      {
        ph_temporary t = _T(in->mktemp());
        emit(new p86_node(p86_MOV, none, none, node->getrm(), t));
        emit(new p86_node(p86_SHR, node->getsetcc(), node->getrequiredcc(),
             node->getrn(), t));
        emit(new p86_node(p86_MOV, none, none, t, node->getrd()));
      }
      break;
      
	    case op_ASR:
      {
        ph_temporary t = _T(in->mktemp());
        emit(new p86_node(p86_MOV, none, none, node->getrm(), t));
        emit(new p86_node(p86_SAR, node->getsetcc(), node->getrequiredcc(),
             node->getrn(), t));
        emit(new p86_node(p86_MOV, none, none, t, node->getrd()));
      }
      break;

	    case op_ROR:
      {
        ph_temporary t = _T(in->mktemp());
        emit(new p86_node(p86_MOV, none, none, node->getrm(), t));
        emit(new p86_node(p86_ROR, node->getsetcc(), node->getrequiredcc(),
             node->getrn(), t));
        emit(new p86_node(p86_MOV, none, none, t, node->getrd()));
      }
      break;

	    case op_RRX:
      {
        ph_temporary t = _T(in->mktemp());
        emit(new p86_node(p86_MOV, none, none, node->getrm(), t));
        emit(new p86_node(p86_RCR, node->getsetcc(), node->getrequiredcc(),
             _C(1), t));
        emit(new p86_node(p86_MOV, none, none, t, node->getrd()));
      }
      break;

	    case op_AND:
      {
        ph_temporary t = _T(in->mktemp());
        emit(new p86_node(p86_MOV, none, none, node->getrm(), t));
        emit(new p86_node(p86_AND, node->getsetcc(), node->getrequiredcc(),
             node->getrn(), t));
        emit(new p86_node(p86_MOV, none, none, t, node->getrd()));
      }
      break;

	    case op_EOR:
      {
        ph_temporary t = _T(in->mktemp());
        emit(new p86_node(p86_MOV, none, none, node->getrm(), t));
        emit(new p86_node(p86_XOR, node->getsetcc(), node->getrequiredcc(),
             node->getrn(), t));
        emit(new p86_node(p86_MOV, none, none, t, node->getrd()));
      }
      break;

	    case op_SUB:
      {
        ph_temporary t = _T(in->mktemp());
        uint5 s = node->getsetcc(), r = node->getrequiredcc();

        emit(new p86_node(p86_MOV, none, none, node->getrm(), t));

        if (s & CFLG)  // if it's supposed to set the carry flag...
        {
          emit(new p86_node(p86_SUB, (s|REALINVCCODE)&~CFLG, r, node->getrn(),
               t));
          emit(new p86_node(p86_CMC, REALCCODE|CFLG, REALINVCCODE));
        }
        else
        {
          emit(new p86_node(p86_SUB, s, r, node->getrn(), t));
        }

        emit(new p86_node(p86_MOV, none, none, t, node->getrd()));
      }
      break;

	    case op_SBC:
      {
        ph_temporary t = _T(in->mktemp());
        uint5 s = node->getsetcc(), r = node->getrequiredcc();

        emit(new p86_node(p86_MOV, none, none, node->getrm(), t));

        if (s & CFLG)
        {
          emit(new p86_node(p86_CMC, REALINVCCODE, r|REALCCODE));
          emit(new p86_node(p86_SBB, (s|REALINVCCODE)&~CFLG, REALINVCCODE,
               node->getrn(), t));
          emit(new p86_node(p86_CMC, REALCCODE|CFLG, REALINVCCODE));
        }
        else
        {
          emit(new p86_node(p86_CMC, REALINVCCODE, r|REALCCODE));
          emit(new p86_node(p86_SBB, s, REALINVCCODE, node->getrn(), t));
        }

        emit(new p86_node(p86_MOV, none, none, t, node->getrd()));
      }
      break;

	    case op_ADD:
      {
        ph_temporary t = _T(in->mktemp());
        emit(new p86_node(p86_MOV, none, none, node->getrm(), t));
        emit(new p86_node(p86_ADD, node->getsetcc(), node->getrequiredcc(),
             node->getrn(), t));
        emit(new p86_node(p86_MOV, none, none, t, node->getrd()));
      }
      break;

	    case op_ADC:
      {
        ph_temporary t = _T(in->mktemp());
        emit(new p86_node(p86_MOV, none, none, node->getrm(), t));
        emit(new p86_node(p86_ADC, node->getsetcc(), node->getrequiredcc(),
             node->getrn(), t));
        emit(new p86_node(p86_MOV, none, none, t, node->getrd()));
      }
      break;

	    case op_OR:
      {
        ph_temporary t = _T(in->mktemp());
        emit(new p86_node(p86_MOV, none, none, node->getrm(), t));
        emit(new p86_node(p86_OR, node->getsetcc(), node->getrequiredcc(),
             node->getrn(), t));
        emit(new p86_node(p86_MOV, none, none, t, node->getrd()));
      }
      break;

	    case op_MOV:
      {
        emit(new p86_node(p86_MOV, none, node->getrequiredcc(),
             node->getrm(), node->getrd()));
        emit(new p86_node(p86_TEST, node->getsetcc(), none, node->getrd(),
             node->getrd()));
      }
      break;
      
	    case op_MVN:
      {
        ph_temporary t = _T(in->mktemp());
        emit(new p86_node(p86_MOV, none, none, node->getrm(), t));
        emit(new p86_node(p86_NOT, none, node->getrequiredcc(), t));
        emit(new p86_node(p86_MOV, none, none, t, node->getrd()));
        emit(new p86_node(p86_TEST, node->getsetcc(), none, node->getrd(),
             node->getrd()));
      }
      break;

	    case op_CMP:
      {
        emit(new p86_node(p86_CMP, node->getsetcc()|REALINVCCODE,
             node->getrequiredcc(), node->getrn(), node->getrm()));
        emit(new p86_node(p86_CMC, REALCCODE, REALINVCCODE));
      }
      break;

      case op_CMN:
      {
        ph_temporary t = _T(in->mktemp());
        emit(new p86_node(p86_MOV, none, none, node->getrn(), t));
        emit(new p86_node(p86_ADD, node->getsetcc(), node->getrequiredcc(),
             node->getrm(), t));
      }
      break;

      case op_TEQ:
      {
        ph_temporary t = _T(in->mktemp());
        emit(new p86_node(p86_MOV, none, none, node->getrn(), t));
        emit(new p86_node(p86_XOR, node->getsetcc(), node->getrequiredcc(),
             node->getrm(), t));
      }
      break;

      case op_TST:
      {
        emit(new p86_node(p86_TEST, node->getsetcc(), node->getrequiredcc(),
             node->getrn(), node->getrm()));
      }
      break;

	    case op_MUL:
      {
        ph_temporary t = _T(in->mktemp());
        emit(new p86_node(p86_MOV, none, none, node->getrm(), t));
        emit(new p86_node(p86_IMUL, node->getsetcc(), node->getrequiredcc(),
             node->getrn(), t));
        emit(new p86_node(p86_MOV, none, none, t, node->getrd()));
      }
      break;

	    case op_CONST:
	    case op_LDW:
	    case op_LDB:
	    case op_STW:
	    case op_STB:
	    case op_SWI:
	    case op_XJMP:
	    case op_SETPC:
      case op_RECONSTRUCTPC:
	    case op_END:
      break;
    }
  }
}
