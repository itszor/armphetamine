#include "defs.h"
#include "machine.h"
#include "decode.h"
#include "disassemble.h"
#include "execute.h"
#include "pheta.h"

// disassembler function table
insttab diss = {
	diss_dp,
	diss_dp_imm,
	diss_mul,
  diss_mull,
	diss_sdt,
  diss_sdth,
	diss_bdt,
	diss_bra,
	diss_swi,
	diss_cdt,
	diss_cdo,
	diss_crt,
	diss_sds,
	diss_und,
  NULL
};

// execution function table, 26-bit
insttab exec26 = {
	exec_dp_26,
	exec_dp_imm_26,
	exec_mul_26,
  exec_mull_26,
	exec_sdt_26,
  exec_sdth_26,
	exec_bdt_26,
	exec_bra_26,
	exec_swi_26,
	exec_cdt_26,
	exec_cdo_26,
	exec_crt_26,
	exec_sds_26,
	exec_und_26,
  NULL
};

// execution function table, 32-bit
insttab exec32 = {
	exec_dp_32,
	exec_dp_imm_32,
	exec_mul_32,
  exec_mull_32,
	exec_sdt_32,
  exec_sdth_32,
	exec_bdt_32,
	exec_bra_32,
	exec_swi_32,
	exec_cdt_32,
	exec_cdo_32,
	exec_crt_32,
	exec_sds_32,
	exec_und_32,
  NULL
};

// execution function table, thumb mode (subtle differences to 32-bit, etc)
insttab execthm = {
	exec_dp_thm,
	exec_dp_imm_thm,
	exec_mul_thm,
  exec_mull_thm,
	exec_sdt_thm,
  exec_sdth_thm,
	exec_bdt_thm,
	exec_bra_thm,
	exec_swi_thm,
	exec_cdt_thm,
	exec_cdo_thm,
	exec_crt_thm,
	exec_sds_thm,
	exec_und_thm,
  exec_thumbl_thm
};

// intermediate code translator function table
/*
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
	pseudo_und,
  NULL
};
*/

// new-style intermediate code
insttab phet4 = {
  pheta_dp,
  pheta_dp_imm,
  pheta_mul,
  pheta_mull,
  pheta_sdt,
  pheta_sdth,
  pheta_bdt,
  pheta_bra,
  pheta_swi,
  pheta_cdt,
  pheta_cdo,
  pheta_crt,
  pheta_sds,
  pheta_und,
  NULL
};

// Tree-shaped instruction dispatch
int dispatch(machineinfo* machine, instructionformat inst, insttab* action,
              void* data)
{
	switch (inst.generic.type)
	{
		case 0:	// either data processing op, PSR tranfer or multiply
		{
			if ((inst.mul.ident&9)==9)
			{
        if (inst.mul.ident==9)
        {
          switch (inst.mul.ident2)
          {
            case 0:
				    // it's a multiply
				    return action->mul(machine, inst, data);

            case 1:
            goto actually_dp;  // !!! is this necessary?

            case 2:
            case 3:
            return action->mull(machine, inst, data);
          }
        }
        else
        {
          return action->sdth(machine, inst, data);
        }
			}
			else
			{
        actually_dp:
				return action->dp(machine, inst, data);
			}
		}
		break;
		
		case 1: // single data swap/DP
		{
			if ((inst.sds.ident&9)==9)
			{
        if ((inst.sds.ident&15)==9)  // not sure if that's enough
        {
			    // it's a single data swap
				  return action->sds(machine, inst, data);
        }
        else
        {
          return action->sdth(machine, inst, data);
        }
			}
			else
			{
				return action->dp(machine, inst, data);
			}
		}
		break;
		
		case 2: // DP (immediate operand)
		case 3: // ditto
		{
			return action->dp_imm(machine, inst, data);
		}
		break;
		
		case 4: // single data transfer
		case 5: // SDT
		{
			return action->sdt(machine, inst, data);
		}
		break;
		
		case 6: // SDT/undefined
		case 7: // ditto
		{
			return action->sdt(machine, inst, data);
		}
		break;
		
		case 8: // Block data transfer
		case 9: // ditto
		{
			return action->bdt(machine, inst, data);
		}
		break;
		
		case 10: // Branch
		{
			return action->bra(machine, inst, data);
		}
		break;
		
		case 11: // Branch-with-link
		{
			return action->bra(machine, inst, data);
		}
		break;
		
		case 12: // Coproc data transfer
		case 13: // ditto
		{
		  return action->cdt(machine, inst, data);
		}
		break;
		
		case 14: // Coproc data operation/register transfer
		{
		  if (inst.cdo.ident)
			{
		    return action->crt(machine, inst, data);
			}
			else
			{
			  return action->cdo(machine, inst, data);
			}
		}
		break;
		
		case 15: // Software interrupt
		{
			return action->swi(machine, inst, data);
		}
		break;
	}
  return 0;
}
