#include "defs.h"
#include "decode4.h"

uint5 dec4_insttype(instructionformat inst)
{
  switch (inst.generic.type)
  {
    case 0:	 // either data processing op, PSR tranfer or multiply
    return (inst.mul.ident2==0 && inst.mul.ident==9) ? dec4_mul : dec4_dp;
    
    case 1:  // single data swap/DP
    return (inst.sds.ident3==2 && inst.sds.ident2==0 && inst.sds.ident==9)
           ? dec4_sds : dec4_dp;
    
    case 2:  // DP (immediate operand)
    case 3:  // ditto
    return dec4_dp_imm;
    
    case 4:  // single data transfer
    case 5:  // ditto
    return dec4_sdt;
    
    case 6:  // SDT/undefined
    case 7:  // ditto
    return dec4_sdt;
    
    case 8:  // block data transfer
    case 9:  // ditto
    return dec4_bdt;
    
    case 10:  // branch
    return dec4_bra;
    
    case 11:  // branch-with-link
    return dec4_bra;
    
    case 12:  // coprocessor data transfer
    case 13:  // ditto
    return dec4_cdt;
    
    case 14:  // Coproc data operation/register transfer
    return (inst.cdo.ident) ? dec4_crt : dec4_cdo;
    
    case 15:  // software interrupt
    return dec4_swi;
  }
}
