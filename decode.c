#include "machine.h"
#include "decode.h"
#include "disassemble.h"
#include "execute.h"
#include "pseudo.h"
#include "pheta.h"

// disassembler function table
insttab diss = {
	diss_dp,
	diss_dp_imm,
	diss_mul,
	diss_sdt,
	diss_bdt,
	diss_bra,
	diss_swi,
	diss_cdt,
	diss_cdo,
	diss_crt,
	diss_sds,
	diss_und
};

// execution function table
insttab exec = {
	exec_dp,
	exec_dp_imm,
	exec_mul,
	exec_sdt,
	exec_bdt,
	exec_bra,
	exec_swi,
	exec_cdt,
	exec_cdo,
	exec_crt,
	exec_sds,
	exec_und
};

// intermediate code translator function table
insttab pseudo = {
  pseudo_dp,
	pseudo_dp_imm,
	pseudo_mul,
	pseudo_sdt,
	pseudo_bdt,
	pseudo_bra,
	pseudo_swi,
	pseudo_cdt,
	pseudo_cdo,
	pseudo_crt,
	pseudo_sds,
	pseudo_und
};

insttab phet4 = {
  pheta_dp,
  pheta_dp_imm,
  pheta_mul,
  pheta_sdt,
  pheta_bdt,
  pheta_bra,
  pheta_swi,
  pheta_cdt,
  pheta_cdo,
  pheta_crt,
  pheta_sds,
  pheta_und
};

// Tree-shaped instruction dispatch
void dispatch(machineinfo* machine, instructionformat inst, insttab* action,
              void* data)
{
	switch (inst.generic.type)
	{
		case 0:	// either data processing op, PSR tranfer or multiply
		{
			if (inst.mul.ident2==0 && inst.mul.ident==9)
			{
				// it's a multiply
				action->mul(machine, inst, data);
			}
			else
			{
				action->dp(machine, inst, data);
			}
		}
		break;
		
		case 1: // single data swap/DP
		{
			if (inst.sds.ident3==2 && inst.sds.ident2==0 && inst.sds.ident==9)
			{
			  // it's a single data swap
				action->sds(machine, inst, data);
			}
			else
			{
				action->dp(machine, inst, data);
			}
		}
		break;
		
		case 2: // DP (immediate operand)
		case 3: // ditto
		{
			action->dp_imm(machine, inst, data);
		}
		break;
		
		case 4: // single data transfer
		case 5: // SDT
		{
			action->sdt(machine, inst, data);
		}
		break;
		
		case 6: // SDT/undefined
		case 7: // ditto
		{
			action->sdt(machine, inst, data);
		}
		break;
		
		case 8: // Block data transfer
		case 9: // ditto
		{
			action->bdt(machine, inst, data);
		}
		break;
		
		case 10: // Branch
		{
			action->bra(machine, inst, data);
		}
		break;
		
		case 11: // Branch-with-link
		{
			action->bra(machine, inst, data);
		}
		break;
		
		case 12: // Coproc data transfer
		case 13: // ditto
		{
		  action->cdt(machine, inst, data);
		}
		break;
		
		case 14: // Coproc data operation/register transfer
		{
		  if (inst.cdo.ident)
			{
		    action->crt(machine, inst, data);
			}
			else
			{
			  action->cdo(machine, inst, data);
			}
		}
		break;
		
		case 15: // Software interrupt
		{
			action->swi(machine, inst, data);
		}
		break;
	}
}
