#include <assert.h>
#include <stddef.h>
#include <stdio.h>

#include "cnew.h"
#include "machine.h"
#include "allocate.h"
#include "block.h"
#include "x86asm.h"
#include "pseudo.h"
#include "codegen.h"
#include "registers.h"

// registers to use for general-purpose calculation
uint3 iregset[] = {EAX, EBX, ECX, EDX, ESI, EDI};

void allocate_cpyregtomem(nativeblockinfo* nat, allocationinfo* alloc,
                          uint5 ireg, uint5 var)
{
  uint5 mod, disp, rm, reg;

  assert(var<CACHEABLE);

  if (alloc->vloc[var].in.valid && alloc->vloc[var].in.reg && 
      !alloc->vloc[var].in.mem)
  {
	  mod = mod_DISP8;  // 8-bit displacement
	  disp = var*4;
	  rm = EBP;
	  reg = iregset[ireg];

	  #define MODSIBDISP MODDISP8
	  x86asm_out(nat, MOVlmr, 0);
	  #undef MODSIBDISP

    alloc->vloc[var].in.mem = 1;
  }
  else
  {
    #ifdef DEBUG
    fprintf(stderr, "Warning: register %d not saved to %d\n  (valid=%d, reg=%d, mem=%d)\n", ireg, var, alloc->vloc[var].in.valid, alloc->vloc[var].in.reg,
    alloc->vloc[var].in.mem);
    #endif
  }
}

void allocate_cpymemtoreg(nativeblockinfo* nat, allocationinfo* alloc,
                          uint5 var, uint5 ireg)
{
  uint5 mod, disp, rm, reg;

  assert(var<CACHEABLE);

  if (alloc->vloc[var].in.mem)
  {
	  mod = mod_DISP8;
	  disp = var*4;
	  rm = EBP;
	  reg = iregset[ireg];

	  #define MODSIBDISP MODDISP8
	  x86asm_out(nat, MOVlrm, 0);
	  #undef MODSIBDISP

    alloc->vloc[var].in.reg = 1;
    alloc->vloc[var].in.valid = 1;
    alloc->vloc[var].in.number = ireg;
    alloc->rctn[ireg].all = 1<<var;
  }
  else
  {
    #ifdef DEBUG
    fprintf(stderr, "Warning: register %d not loaded from %d\n", ireg, var);
    #endif
  }
}

// get an x86 register, clearing out any others if necessary
uint5 allocate_obtain(allocationinfo* alloc, uint5 lock, nativeblockinfo* nat)
{
  uint5 i;
  
  for (i=0; i<NUMIREG; i++) if (!alloc->rctn[i].all) return i;

  for (i=0; i<NUMIREG; i++)
  {
    if (++alloc->spill>=NUMIREG) alloc->spill = 0;
    
/*    fprintf(stderr, "Attempting to tip reg %x under lock %x\n",
      alloc->rctn[alloc->spill].all, lock);
    
    if (alloc->rctn[alloc->spill].all==0x10000 && lock==0xffc00000) abort();*/
    
    if (!(alloc->rctn[alloc->spill].all & lock))
    {
      uint5 j;

      // 'potentially' more than one variable (intermediate reg) per ireg
      for (j=0; j<32; j++)
      {
        if (alloc->rctn[alloc->spill].all & (1<<j))
        {
          allocate_cpyregtomem(nat, alloc, alloc->spill, j);
          alloc->rctn[alloc->spill].all &= ~(1<<j);
          alloc->vloc[j].in.reg = alloc->vloc[j].in.valid = 0;
          alloc->vloc[j].in.mem = 1;
          #ifdef DEBUG
          fprintf(stderr, "Spilled variable %d\n", j);
          #endif
        }
      }

      return alloc->spill;
    }
  }

  fprintf(stderr, "Can't spill register, dying?\n");
  abort();
}

// new allocator, old one went stale...
registermapping allocate_reg(allocationinfo* alloc, uint5 var, uint5 lock,
                             uint5 type, nativeblockinfo* nat)
{
  registermapping r;

  #ifdef DEBUG
  fprintf(stderr, "Lock seems to be %x\n", lock);
  #endif

  switch (type)
  {
    case alloc_VALID:
    {
      assert(var<CACHEABLE);
      if (alloc->vloc[var].in.reg)
      {
        if (!alloc->vloc[var].in.valid)
          allocate_cpymemtoreg(nat, alloc, var, alloc->vloc[var].in.number);

        r.parts.original = var;
        r.parts.mapnum = alloc->vloc[var].in.number;
        r.parts.mappedto = iregset[r.parts.mapnum];
        r.parts.status = alloc_VALID;
        #ifdef DEBUG
        fprintf(stderr, "Valid allocation returning register, var %d\n", var);
        #endif
      }
      else
      {
        uint5 ireg = allocate_obtain(alloc, lock, nat);
        allocate_cpymemtoreg(nat, alloc, var, ireg);
        r.parts.original = var;
        r.parts.mapnum = ireg;
        r.parts.mappedto = iregset[ireg];
        r.parts.status = alloc_VALID;
        #ifdef DEBUG
        fprintf(stderr, "Valid allocation returning new register, var %d\n",
          var);
        #endif
      }
    }
    break;
    
    case alloc_INVALID:
    {
      assert(var<CACHEABLE);
      if (alloc->vloc[var].in.reg)
      {
        r.parts.original = var;
        r.parts.mapnum = alloc->vloc[var].in.number;
        r.parts.mappedto = iregset[r.parts.mapnum];
        r.parts.status = alloc->vloc[var].in.valid ? alloc_VALID : 
          alloc_INVALID;
        #ifdef DEBUG
        fprintf(stderr, "Invalid allocation returning existing register, "
          "var %d\n", var);
        #endif
      }
      else
      {
        uint5 ireg = allocate_obtain(alloc, lock, nat);
        alloc->vloc[var].in.reg = 1;
        alloc->vloc[var].in.valid = 0;
        alloc->vloc[var].in.number = ireg;
        alloc->rctn[ireg].all = 1<<var;
        r.parts.original = var;
        r.parts.mapnum = ireg;
        r.parts.mappedto = iregset[ireg];
        r.parts.status = alloc_INVALID;
        #ifdef DEBUG
        fprintf(stderr, "Invalid allocation returning new register, var %d\n",
          var);
        #endif
      }
    }
    break;
    
    case alloc_MEMORY:
    {
      if (alloc->vloc[var].in.reg)
      {
        if (!alloc->vloc[var].in.valid)
          allocate_cpymemtoreg(nat, alloc, var, alloc->vloc[var].in.number);

        r.parts.original = var;
        r.parts.mapnum = alloc->vloc[var].in.number;
        r.parts.mappedto = iregset[r.parts.mapnum];
        r.parts.status = alloc_VALID;
        #ifdef DEBUG
        fprintf(stderr, "Memory allocation returning register, var %d\n", var);
        #endif
      }
      else
      {
        // if this fails, something's horribly broken
        assert(alloc->vloc[var].in.mem);
        r.parts.original = var;
        r.parts.mapnum = r.parts.mappedto = 255;
        r.parts.status = alloc_MEMORY;
        #ifdef DEBUG
        fprintf(stderr, "Memory allocation returning memory, var %d\n", var);
        #endif
      }
    }
    break;
    
    case alloc_HIDDEN:
    {
      r.parts.original = var;
      r.parts.mapnum = r.parts.mappedto = 255;
      r.parts.status = alloc_HIDDEN;
      #ifdef DEBUG
      fprintf(stderr, "Hidden allocation returning nothing, var %d\n", var);
      #endif
    }
  }
  
  return r;
}

// set up an allocation state (flags and registers) for an instruction
void allocate_setup(allocationinfo* alloc, matchstate* mstate, uint5 ins,
                    nativeblockinfo* nat, uint5* lock)
{
  uint5 i, flags = 0;
  pseudoformat inst;
  uint5 allocmsk = 0x003fffff;
  
  inst.value = ins;
  
  for (i=0; i<mstate->num; i++) *lock |= 1<<mstate->regset[i].parts.original;

  #ifdef DEBUG
  fprintf(stderr, "Lock=%x\n", *lock);
  #endif
  
  if (inst.parts.rm>=reg_CPSRC && inst.parts.rm<=reg_CPSRN)
    flags |= (1<<(inst.parts.rm-reg_CPSRC));
  if (inst.parts.rn>=reg_CPSRC && inst.parts.rn<=reg_CPSRN)
    flags |= (1<<(inst.parts.rn-reg_CPSRC));
  if (inst.parts.ro>=reg_CPSRC && inst.parts.ro<=reg_CPSRN)
    flags |= (1<<(inst.parts.ro-reg_CPSRC));

  allocate_ensureflags(alloc, mstate, flags, *lock, nat);

  for (i=0; i<mstate->num; i++)
  {
    if (allocmsk & (1<<mstate->regset[i].parts.original))
    {
      mstate->regset[i] = allocate_reg(alloc, mstate->regset[i].parts.original,
        *lock, (mstate->overwrite & (1<<i)) ? alloc_INVALID : ((mstate->hidden &
        (1<<i)) ? alloc_HIDDEN : alloc_MEMORY), nat);
    }
    else
    {
      mstate->regset[i].parts.status = alloc_UNSET;
    }
  }
}

// any registers which have been intentionally set by a generated instruction
// are now valid, but are no longer in step with the register dump in memory
void allocate_npostfix(allocationinfo* alloc, matchstate* mstate)
{
  uint5 i;
  
  for (i=0; i<mstate->num; i++)
  {
    if (mstate->overwrite & (1<<i))
    {
      uint5 orig = mstate->regset[i].parts.original;
      alloc->vloc[orig].in.valid = 1;
      alloc->vloc[orig].in.mem = 0;
      #ifdef DEBUG
      fprintf(stderr, "IC register %d set to valid\n  (reg=%d)\n", orig,
        alloc->vloc[orig].in.reg);
      #endif
    }
  }
}

// if it's discovered that a lazily allocated (alloc_MEMORY) register actually
// needs to be in a real register, call this
uint5 allocate_nsolidify(allocationinfo* alloc, registermapping* rmap,
                         uint5 lock, nativeblockinfo* nat)
{
  uint5 var = rmap->parts.original;
  
  if (rmap->parts.status==alloc_VALID) return rmap->parts.mappedto;
  if (rmap->parts.status==alloc_HIDDEN)
  {
    fprintf(stderr, "Warning: trying to solidify a hidden register?\n");
    abort();
  }
  
  *rmap = allocate_reg(alloc, rmap->parts.original, lock, alloc_VALID, nat);
  
  return rmap->parts.mappedto;
}

// store all ARM-equivalent IC registers in memory dump
void allocate_reset(allocationinfo* alloc, nativeblockinfo* nat)
{
  uint5 i;
  
  for (i=0; i<CACHEABLE; i++)
  {
    if (alloc->vloc[i].in.reg && !alloc->vloc[i].in.mem && 
        alloc->vloc[i].in.valid)
    {
      allocate_cpyregtomem(nat, alloc, alloc->vloc[i].in.number, i);
      // reset usually called before clobbering things...?
      alloc->vloc[i].in.reg = alloc->vloc[i].in.valid = 0;
    }
  }
}

// takes two IC registers, and swaps their contents
void allocate_swap(allocationinfo* alloc, registermapping* x,
                   registermapping* y, nativeblockinfo* nat)
{
  uint5 rm, reg, temp;
  
  assert(x->parts.status!=alloc_MEMORY && y->parts.status!=alloc_MEMORY);

  if (x->parts.mappedto != y->parts.mappedto)
  {  
    rm = x->parts.mappedto;
    reg = y->parts.mappedto;
    x86asm_out(nat, XCHGlrr, 0);

    temp = alloc->rctn[x->parts.mapnum].all;
    alloc->rctn[x->parts.mapnum].all = alloc->rctn[y->parts.mapnum].all;
    alloc->rctn[y->parts.mapnum].all = temp;

    temp = alloc->vloc[x->parts.original].all;
    alloc->vloc[x->parts.original].all = alloc->vloc[y->parts.original].all;
    alloc->vloc[y->parts.original].all = temp;
  }
}

// emit instructions necessary to move from one allocation context into another
void allocate_coerce(allocationinfo* from, allocationinfo* to, nativeblockinfo*
                     nat)
{
  // matchstate is used because it has the right sort of fields, it may not
  // be the best thing representationally
  matchstate mfrom, mto;
  uint5 i;

  #ifdef DEBUG
  fprintf(stderr, "Coercing\n");
  #endif
  
 /* mfrom.num = mto.num = 0;
  
  for (i=0; i<16; i++)
  {
    if (from->vloc[i].in.reg && from->vloc[i].in.valid)
    {
      mfrom.regset[mfrom.num].parts.original = i;
      mfrom.regset[mfrom.num++].parts.mapnum = from->vloc[i].in.number;
      assert(mfrom.num<6);
    }
    if (to->vloc[i].in.reg && to->vloc[i].in.valid)
    {
      mto.regset[mto.num].parts.original = i;
      mto.regset[mto.num++].parts.mapnum = to->vloc[i].in.number;
      assert(mto.num<6);
    }
  }*/
  
  // first, swap any registers which are fully allocated in both domains,
  // but in different iregs
  for (i=0; i<CACHEABLE; i++)
  {
    if (from->vloc[i].in.reg && from->vloc[i].in.valid &&
        to->vloc[i].in.reg && to->vloc[i].in.valid)
    {
      registermapping x, y;
      x.parts.original = y.parts.original = i;
      x.parts.mapnum = from->vloc[i].in.number;
      x.parts.mappedto = iregset[x.parts.mapnum];
      y.parts.mapnum = to->vloc[i].in.number;
      y.parts.mappedto = iregset[y.parts.mapnum];
      allocate_swap(from, &x, &y, nat);
    }
  }
  
  // next, save any registers which are allocated in the 'from' domain
  // but not the 'to' domain
  for (i=0; i<CACHEABLE; i++)
  {
    if (from->vloc[i].in.reg && from->vloc[i].in.valid &&
        ((!to->vloc[i].in.reg || !to->vloc[i].in.valid) ||
        (!from->vloc[i].in.mem && to->vloc[i].in.mem)))
    {
      allocate_cpyregtomem(nat, from, from->vloc[i].in.number, i);
    }
  }
  
  // finally, load any registers which are allocated in the 'to' domain
  // but not in the 'from' domain
  for (i=0; i<CACHEABLE; i++)
  {
    if ((to->vloc[i].in.reg && (!from->vloc[i].in.reg ||
        (from->vloc[i].in.reg && (to->vloc[i].in.valid &&
         !from->vloc[i].in.valid)))))
    {
      allocate_cpymemtoreg(nat, from, i, to->vloc[i].in.number);
    }
  }
}

// force a particular (single) register mapping
void allocate_force(allocationinfo* alloc, registermapping req,
                    nativeblockinfo* nat)
{
  registermapping cur;
  
  req.parts.status = alloc_VALID;
  
  // register contains something...
  if (alloc->rctn[req.parts.mapnum].all)
  {
    uint5 i;
    sint5 ireg = -1, var = -1;
    registermapping x, y;
    char* names[] = {"eax","ebx","ecx","edx","esi","edi"};
    
    // find where register is currently
    for (i=0; i<NUMIREG && ireg==-1; i++)
    {
      if (alloc->rctn[i].all & (1<<req.parts.original)) ireg = i;
    }
    
    for (i=0; i<32 && var==-1; i++)
    {
      if (alloc->vloc[i].in.reg && alloc->vloc[i].in.number==req.parts.mapnum)
        var = i;
    }
    
    assert(ireg!=-1 && var!=-1);
    
    x.parts.original = req.parts.original;
    x.parts.mapnum = ireg;
    x.parts.mappedto = iregset[ireg];
    x.parts.status = alloc_VALID;
    y.parts.original = var;
    y.parts.mapnum = req.parts.mapnum;
    y.parts.mappedto = iregset[req.parts.mapnum];
    y.parts.status = alloc_VALID;
    #ifdef DEBUG
    fprintf(stderr, "Forced swap (%s,%d) (%s,%d)\n", names[x.parts.mapnum],
      x.parts.original, names[y.parts.mapnum], y.parts.original);
    #endif
    allocate_swap(alloc, &x, &y, nat);
  }
}

uint5 allocate_push(nativeblockinfo* nat, allocationinfo* alloc, uint5 ireg,
                    uint5 var)
{
  if (alloc->vloc[var].in.valid && alloc->vloc[var].in.reg && 
      !alloc->vloc[var].in.mem)
  {
	  uint5 rm = iregset[ireg];
	  x86asm_out(nat, PUSHlr, 0);

    alloc->vloc[var].in.mem = 1;
    return 1;
  }
  else
  {
    fprintf(stderr, "Warning: register %d not pushed onto stack\n", ireg);
    return 0;
  }
}

uint5 allocate_pop(nativeblockinfo* nat, allocationinfo* alloc, uint5 var,
                   uint5 ireg)
{
  if (alloc->vloc[var].in.mem)
  {
	  uint5 rm = iregset[ireg];

	  x86asm_out(nat, POPlr, 0);

    alloc->vloc[var].in.reg = 1;
    alloc->vloc[var].in.valid = 1;
    alloc->vloc[var].in.number = ireg;
    alloc->rctn[ireg].all = 1<<var;
    return 1;
  }
  else
  {
    fprintf(stderr, "Warning: register %d not popped from stack\n", ireg);
    return 0;
  }
}

// force some registers into memory (real ICR) or the stack (virtual ICR)
// (just memory now)
uint5 allocate_rpush(allocationinfo* alloc, uint5 which, nativeblockinfo* nat)
{
  uint5 i, stacked = 0;
  
  for (i=0; i<NUMIREG; i++)
  {
    if ((which & (1<<i)) && alloc->rctn[i].all)
    {
      if (alloc->rctn[i].all & 0x3fffff)
      {
        uint5 j;
        // low; store to register dump
        for (j=0; j<CACHEABLE; j++)
        {
          if (alloc->rctn[i].all & (1<<j))
          {
            allocate_cpyregtomem(nat, alloc, i, j);
            alloc->vloc[j].in.valid = 0;
          }
        }
      }
/*      else
      {
        uint5 j;
        // high; stick on the stack
        for (j=reg_T0; j<=reg_T5; j++)
        {
          if (alloc->rctn[i].all & (1<<j))
          {
            //if (allocate_push(nat, alloc, i, j))
            stacked |= 1<<j;
          }
        }
      }*/
    }
  }

/*  for (i=reg_T0; i<=reg_T5; i++)
  {
    if (stacked & (1<<i))
      if (!allocate_push(nat, alloc, alloc->vloc[i].in.number, i))
        stacked &= ~(1<<i);
  }*/

  return stacked;
}

// restore stacked (virtual IC) registers
void allocate_rpop(allocationinfo* alloc, uint5 stacked,
                   nativeblockinfo* nat)
{
  uint5 i;

/*  for (i=reg_T5; i>=reg_T0; i--)
  {
    if (stacked & (1<<i))
    {
      allocate_pop(nat, alloc, i, alloc->vloc[i].in.number);
      alloc->vloc[i].in.mem = 0;
    }
  }*/
}

// if 'type' is alloc_INVALID, a register is assigned but contains nonsense
// if alloc_VALID, a register is assigned and made valid
// if alloc_MEMORY, a variable is left in memory unless it is already in
// a register
/*registermapping allocate_reg(allocationinfo* alloc, uint5 var, uint5 lock,
                             uint5 type, nativeblockinfo* nat)
{
  registermapping rmap;
  uint5 mod, disp, rm, reg, i;
	sint5 ireg = -1;

  rmap.parts.original = var;
  rmap.parts.mappedto = 0;
  rmap.parts.mapnum = 0;
  rmap.parts.status = alloc_UNSET;

  #ifdef DEBUG
  fprintf(stderr, "Attempting allocation for %d, type %d, current location "
    "%x\n", var, type, alloc->vloc[var].all);
  #endif

  // it's not up to this allocator to deal with these things
  if ((var>=reg_CPSRC && var<=reg_CPSRN) || var==reg_UNUSED)
    return rmap;

  // is variable already in an x86 register, and also valid?
	if (alloc->vloc[var].all & var_REGISTER)
	{
		for (i=0; i<NUMIREG; i++)
    {
		  if ((alloc->vloc[var].all & (var_EAX<<i)) && ((type!=alloc_VALID) ||
          type==alloc_VALID && alloc->vloc[var].in.valid))
      {
        rmap.parts.mapnum = i;
        rmap.parts.mappedto = iregset[i];
        rmap.parts.status = alloc_VALID;
        return rmap;
      }
    }
	}

  if (type==alloc_MEMORY)
  {
    rmap.parts.mapnum = rmap.parts.mappedto = 0;
    rmap.parts.status = alloc_MEMORY;
    return rmap;
  }
	
  // if there are any free registers, find one	
	for (i=0; i<NUMIREG; i++)
	{
	  if (!alloc->rctn[i].all)
		{
		  ireg = i;
			break;
		}
	}

  // otherwise, one will have to be spilled
	if (ireg==-1)
	{
  	// a more sophisticated algorithm here might be nice
		while (ireg==-1)
		{
	  	// don't spill a register locked by this instruction
			if (!(alloc->rctn[alloc->spill].all & lock))
			  ireg = alloc->spill;

  		if (++alloc->spill==NUMIREG) alloc->spill=0;
		}

    allocate_spill(alloc, ireg, 0, nat);
	}
	
  switch (type)
  {
    case alloc_INVALID:
    alloc->rctn[ireg].all = 1<<var;
    alloc->vloc[var].all = var_EAX<<ireg;
    rmap.parts.status = alloc_INVALID;
    #ifdef DEBUG
    fprintf(stderr, "Invalid allocation: ireg=%d, var=%d, location %x\n", ireg, 
      var, alloc->vloc[var].all);
    #endif
    break;
    
    case alloc_VALID:
	  if (alloc->vloc[var].in.mem)
		{
//	  	assert(alloc->vloc[var].in.mem);
	  	// preserved variable; need to load previous value from dump
			mod = mod_DISP8;
			disp = var*4;
			rm = EBP;
			reg = iregset[ireg];
			#define MODSIBDISP MODDISP8
			x86asm_out(nat, MOVlrm, 0);
			#undef MODSIBDISP
		}
	  alloc->rctn[ireg].all = 1<<var;
	  alloc->vloc[var].all |= (var_EAX<<ireg) | var_VALID;
    rmap.parts.status = alloc_VALID;
    break;
  }

  // if (var>=reg_T0 && var<=reg_T4) rmap.parts.status = alloc_INREG;
		
  rmap.parts.mapnum = ireg;
  rmap.parts.mappedto = iregset[ireg];
    
	return rmap;
}
*/

/*
void allocate_registerpattern(allocationinfo* alloc, matchstate* mstate,
                              uint5* lock, nativeblockinfo* native)
{
  uint5 j;

	// lock all registers used by this instruction (so they can't be spilled)
	for (j=0; j<mstate->num; j++) *lock |= 1<<mstate->regset[j].parts.original;

	// allocate ARM registers in pattern to x86 registers
	for (j=0; j<mstate->num; j++)
	{
		mstate->regset[j] = allocate_reg(alloc,
      mstate->regset[j].parts.original, *lock, (mstate->overwrite & (1<<j))
      ? alloc_INVALID : alloc_MEMORY, native);
    allocate_checkalloc(alloc);
	}
}*/

/*
void allocate_postfix(allocationinfo* alloc, matchstate* mstate)
{
  uint5 j;

  // any registers overwritten (set) are now valid
	for (j=0; j<mstate->num; j++)
  {
    if (mstate->overwrite & (1<<j))
    {
      alloc->vloc[mstate->regset[j].parts.original].in.valid = 1;
      alloc->vloc[mstate->regset[j].parts.original].in.mem = 0;
   |* alloc->rctn[mstate->regset[j].parts.mapnum].all = 
        1<<mstate->regset[j].parts.original;  *|
      #ifdef DEBUG
      fprintf(stderr, "Fiddled validity for original reg %d, match number" \
        " %d, variable location %x\n", mstate->regset[j].parts.original, j,
        alloc->vloc[mstate->regset[j].parts.original].all);
      #endif
    }
  }
}*/

// check validity of allocationinfo structure
/*void allocate_checkalloc(allocationinfo* alloc)
{
  uint5 i;
  
  for (i=0; i<32; i++)
  {
    uint5 j;
    for (j=0; j<NUMIREG; j++)
    {
      if ((alloc->vloc[i].all & (var_EAX<<j)) && alloc->vloc[i].in.valid)
      {
        assert(alloc->rctn[j].all & (1<<i));
      }
    }
  }
  
  for (i=0; i<NUMIREG; i++)
  {
    uint5 j;
    for (j=0; j<32; j++)
    {
      if (alloc->rctn[i].all & (1<<j))
      {
        assert((alloc->vloc[j].all & (var_EAX<<i)) || !alloc->vloc[i].in.valid);
      }
    }
  }
}*/

// spill a plain register into memory, or the stack if it's a temporary
/*uint5 allocate_spill(allocationinfo* alloc, uint5 ireg, registermapping* into,
                     nativeblockinfo* nat)
{
  uint5 j;
//  allocate_checkalloc(alloc);
  #ifdef DEBUG
  fprintf(stderr, "Trying to spill %d\n", ireg);
  #endif
	// store variable(s) (only first 16) in this register in the dump
	for (j=0; j<16; j++)
	{
		if (alloc->rctn[ireg].all & (1<<j))
		{
      if (alloc->vloc[j].in.valid && !alloc->vloc[j].in.mem)
      {
			  uint5 mod = mod_DISP8;  // 8-bit displacement
			  uint5 disp = j*4;
			  uint5 rm = EBP;
			  uint5 reg = iregset[ireg];

			  #define MODSIBDISP MODDISP8
			  x86asm_out(nat, MOVlmr, 0);
			  #undef MODSIBDISP
      }

|*			alloc->vloc[j].all &= ~(var_EAX<<ireg);*|
			alloc->vloc[j].all = var_MEM;
      alloc->rctn[ireg].all = 0;
      #ifdef DEBUG
      fprintf(stderr, "First type\n");
      #endif
      return 0;
		}
	}
  for (j=reg_T0; j<=reg_T5; j++)
  {
    if ((alloc->rctn[ireg].all & (1<<j)))
    {
      uint5 rm = iregset[ireg];
      x86asm_out(nat, PUSHlr, 0);
      alloc->vloc[j].in.valid = 0;
      alloc->rctn[ireg].all = 0;
//      allocate_checkalloc(alloc);
      if (into)
      {
        into->parts.original = j;
        into->parts.mapnum = ireg;
        into->parts.mappedto = iregset[ireg];
      }
      #ifdef DEBUG
      fprintf(stderr, "Second type\n");
      #endif
      return 1;
    }
  }

  if (alloc->rctn[ireg].all)
  {
    fprintf(stderr, "Failed to spill register %d (rctn[ireg]=%x) (locked or "
      "wrong type?)\n", ireg, alloc->rctn[ireg]);
    abort();
  }
  return 0;
}*/

/*
void allocate_unspill(allocationinfo* alloc, registermapping* thisone,
                      nativeblockinfo* nat)
{
  uint5 rm = thisone->parts.mappedto;
  x86asm_out(nat, POPlr, 0);
  alloc->vloc[thisone->parts.original].all |= (var_EAX<<thisone->parts.mapnum)
                                           | var_VALID;
  alloc->rctn[thisone->parts.mapnum].all |= 1<<thisone->parts.original;
}*/

/*
uint5 allocate_solidify(allocationinfo* alloc, registermapping* rmap,
                        uint5 lock, nativeblockinfo* nat)
{
  if (!(alloc->vloc[rmap->parts.original].all & var_REGISTER))
  {
    rmap->all = allocate_reg(alloc, rmap->parts.original, lock, alloc_VALID,
                             nat).all;
  }
  else if (!alloc->vloc[rmap->parts.original].in.valid)
  {
    uint5 mod, disp, rm, reg;
	  // need to load previous value from dump
		mod = mod_DISP8;
		disp = rmap->parts.original*4;
		rm = EBP;
		reg = rmap->parts.mappedto;
		#define MODSIBDISP MODDISP8
		x86asm_out(nat, MOVlrm, 0);
		#undef MODSIBDISP
  }
  return rmap->parts.mappedto;
}*/

// move data from one x86 register to another
// this isn't right yet
/*void allocate_move(allocationinfo* alloc, registermapping from,
                   registermapping to, nativeblockinfo* nat)
{
  uint5 rm, reg;
  if (from.all==to.all) return;
  
  rm = to.parts.mappedto;
  reg = from.parts.mappedto;
  x86asm_out(nat, MOVlrr, 0);
}*/

// temporarily force a register to contain the value held in another register
// use extreme caution with allocator after this (register may only be valid
// until the next allocation?)
/*sint5 allocate_forcereg(allocationinfo* alloc, matchstate* mstate, uint5 ireg,
                        uint5 forceto, uint5 lock, nativeblockinfo* nat)
{
  uint5 i, mod, disp, reg, rm;
  registermapping temp;
	
	// If it doesn't contain anything, don't bother saving it
	if (alloc->rctn[ireg].all==0 || iregset[ireg]==forceto) return -1;

  // store any variables held in this register
  rm = iregset[ireg];
  x86asm_out(nat, PUSHlr, 0);
  
|*  for (i=0; i<32; i++)
  {
    if (alloc->rctn[ireg].all & (1<<i))
    {
      alloc->vloc[i].all = var_MEM;
      if (lock & (1<<i))
      {
        fprintf(stderr, "Warning - reallocating register...\n");
        allocate_reg(alloc, i, lock, alloc_VALID, nat);
      }
	    mod = mod_DISP8;  // 8-bit displacement
	    disp = i*4;
	    rm = EBP;
	    reg = iregset[ireg];

	    #define MODSIBDISP MODDISP8
	    x86asm_out(nat, MOVlmr, 0);
	    #undef MODSIBDISP
    }
  }*|
  
	rm = iregset[ireg];
	reg = forceto;
	x86asm_out(nat, MOVlrr, 0);

  return ireg;
}*/

/*
void allocate_restorereg(allocationinfo* alloc, uint5 ireg, sint5 t,
                         nativeblockinfo* nat)
{
  uint5 rm;
  if (t != -1)
  {
    rm = iregset[t];
    x86asm_out(nat, POPlr, 0);
  }
|*
  uint5 rm, reg, i, j;
  registermapping temp;
    
  if (t != -1)
	{
    temp.all = t;
    rm = iregset[ireg];
		reg = temp.parts.mappedto;
		x86asm_out(base, ptr, MOVlrr, 0);
    alloc->rctn[ireg].all &= ~reg_CPSRMODE;
	}
  *|
}*/

// If flags are about to be clobbered, save them away nicely
void allocate_preflags(allocationinfo* alloc, uint5 corrupts, uint5 overwrites,
                       nativeblockinfo* nat)
{
  uint5 clobbered = corrupts & ~overwrites;
	uint5 mod, rm, disp;

	if (alloc->vloc[reg_CPSRC].in.flag && (clobbered & CFLAG) && 
        !alloc->vloc[reg_CPSRC].in.mem)
	{
	  mod = mod_DISP8;
		rm = EBP;
		disp = offsetof(registerinfo, cflag);
    #define MODSIBDISP MODDISP8
    if (alloc->vloc[reg_CPSRC].in.invertedflag)
    {
      // store inverted carry flag
      x86asm_out(nat, CMC, SETCm, 0);
      alloc->vloc[reg_CPSRC].in.invertedflag = 0;
    }
    else
    {
		  x86asm_out(nat, SETCm, 0);
    }
    #undef MODSIBDISP
		alloc->vloc[reg_CPSRC].all = var_MEM;
	}

	if (alloc->vloc[reg_CPSRV].in.flag && (clobbered & VFLAG) && 
      !alloc->vloc[reg_CPSRV].in.mem)
	{
	  mod = mod_DISP8;
		rm = EBP;
		disp = offsetof(registerinfo, vflag);
		#define MODSIBDISP MODDISP8
		x86asm_out(nat, SETOm, 0);
		#undef MODSIBDISP
		alloc->vloc[reg_CPSRV].all = var_MEM;
	}
		
	if (alloc->vloc[reg_CPSRZ].in.flag && (clobbered & ZFLAG) && 
      !alloc->vloc[reg_CPSRZ].in.mem)
	{
	  mod = mod_DISP8;
		rm = EBP;
		disp = offsetof(registerinfo, zflag);
		#define MODSIBDISP MODDISP8
		x86asm_out(nat, SETZm, 0);
		#undef MODSIBDISP
		alloc->vloc[reg_CPSRZ].all = var_MEM;
	}

	if (alloc->vloc[reg_CPSRN].in.flag && (clobbered & NFLAG) && 
      !alloc->vloc[reg_CPSRN].in.mem)
	{
	  mod = mod_DISP8;
		rm = EBP;
		disp = offsetof(registerinfo, nflag);
		#define MODSIBDISP MODDISP8
		x86asm_out(nat, SETSm, 0);
		#undef MODSIBDISP
		alloc->vloc[reg_CPSRN].all = var_MEM;
	}
}

// if an instruction doesn't set quite enough flags (eg, movs), make sure it
// sets some more. Also make sure it knows where the flags are now.
void allocate_postflags(allocationinfo* alloc, matchstate* mstate, uint5 sets,
                        uint5 required, nativeblockinfo* nat)
{
  uint5 invcarry = (sets & ICFLAG) ? 1 : 0;
 
  if (required & (NFLAG | ZFLAG) && (~sets & (NFLAG | ZFLAG)))
  {
    uint5 rm = mstate->regset[0].parts.mappedto;
    uint5 reg = rm;
    allocate_preflags(alloc, TESTdef|TESTundef, TESTdef, nat);
    x86asm_out(nat, TESTlrr, 0);
    sets |= NFLAG | ZFLAG;
  }
  
  sets &= required;
  
  if (sets & ZFLAG) alloc->vloc[reg_CPSRZ].all = var_FLAG;
  if (sets & NFLAG) alloc->vloc[reg_CPSRN].all = var_FLAG;
  if (sets & VFLAG) alloc->vloc[reg_CPSRV].all = var_FLAG;
  if (sets & CFLAG)
    alloc->vloc[reg_CPSRC].all = invcarry ? var_FLAG | var_INVERTEDFLAG
                                          : var_FLAG;

  assert(!(required & ~sets));
/*
  uint3 invcarry = (sets & ICFLAG) ? 1 : 0;
  if ((required & ~sets) & (NFLAG|ZFLAG))
  {
    uint5 rm=mstate->regset[0].parts.mappedto, imm=0;
    sets |= NFLAG | ZFLAG;

    sets &= required;

    if (sets & ZFLAG) alloc->vloc[reg_CPSRZ].all = var_FLAG;
    if (sets & NFLAG) alloc->vloc[reg_CPSRN].all = var_FLAG;

    x86asm_out(nat, TESTlri, 0);
  }
  else
  {
    sets &= required;

    if (sets & ZFLAG) alloc->vloc[reg_CPSRZ].all = var_FLAG;
    if (sets & NFLAG) alloc->vloc[reg_CPSRN].all = var_FLAG;
  }

  if (sets & VFLAG) alloc->vloc[reg_CPSRV].all = var_FLAG;
  if (sets & CFLAG)
    alloc->vloc[reg_CPSRC].all = invcarry ? var_FLAG | var_INVERTEDFLAG
                                          : var_FLAG;*/
}

// eurgh.
void allocate_ensureflags(allocationinfo* alloc, matchstate* mstate,
                          uint5 flags, uint5 lock, nativeblockinfo* nat)
{
  registermapping temp1, temp2;
  uint5 rm, imm, reg, mod, disp;

  flags &= ~(alloc->vloc[reg_CPSRC].in.flag ? CFLAG : 0) &
           ~(alloc->vloc[reg_CPSRV].in.flag ? VFLAG : 0) &
           ~(alloc->vloc[reg_CPSRN].in.flag ? NFLAG : 0) &
           ~(alloc->vloc[reg_CPSRZ].in.flag ? ZFLAG : 0);

  if (flags)
  {
    temp1 = allocate_reg(alloc, reg_T5, lock|ctn_T4|ctn_T5, alloc_INVALID, nat);
    temp2 = allocate_reg(alloc, reg_T4, lock|ctn_T4|ctn_T5, alloc_INVALID, nat);

    rm = temp1.parts.mappedto;
    x86asm_out(nat, PUSHF, POPlr, 0);
    imm = ~((flags&CFLAG?0x1:0)|(flags&ZFLAG?0x40:0)|(flags&NFLAG?0x80:0)|
            (flags&VFLAG?0x800:0));
    x86asm_out(nat, ANDlri, 0);

    if (flags & CFLAG)
    {
      mod = mod_DISP8;
      rm = EBP;
      disp = offsetof(registerinfo, cflag);
      reg = temp2.parts.mappedto;
      #define MODSIBDISP MODDISP8
      x86asm_out(nat, MOVZXlrbm, 0);
      #undef MODSIBDISP
      rm = temp1.parts.mappedto;
      reg = temp2.parts.mappedto;
      x86asm_out(nat, ORlrr, 0);
      alloc->vloc[reg_CPSRC].all |= var_FLAG;
    }

    if (flags & ZFLAG)
    {
      mod = mod_DISP8;
      rm = EBP;
      disp = offsetof(registerinfo, zflag);
      reg = temp2.parts.mappedto;
      #define MODSIBDISP MODDISP8
      x86asm_out(nat, MOVZXlrbm, 0);
      #undef MODSIBDISP
      rm = temp2.parts.mappedto;
      imm = 6;
      x86asm_out(nat, SHLlri, 0);
      rm = temp1.parts.mappedto;
      reg = temp2.parts.mappedto;
      x86asm_out(nat, ORlrr, 0);
      alloc->vloc[reg_CPSRZ].all |= var_FLAG;
    }

    if (flags & NFLAG)
    {
      mod = mod_DISP8;
      rm = EBP;
      disp = offsetof(registerinfo, nflag);
      reg = temp2.parts.mappedto;
      #define MODSIBDISP MODDISP8
      x86asm_out(nat, MOVZXlrbm, 0);
      #undef MODSIBDISP
      rm = temp2.parts.mappedto;
      imm = 7;
      x86asm_out(nat, SHLlri, 0);
      rm = temp1.parts.mappedto;
      reg = temp2.parts.mappedto;
      x86asm_out(nat, ORlrr, 0);
      alloc->vloc[reg_CPSRN].all |= var_FLAG;
    }

    if (flags & VFLAG)
    {
      mod = mod_DISP8;
      rm = EBP;
      disp = offsetof(registerinfo, vflag);
      reg = temp2.parts.mappedto;
      #define MODSIBDISP MODDISP8
      x86asm_out(nat, MOVZXlrbm, 0);
      #undef MODSIBDISP
      rm = temp2.parts.mappedto;
      imm = 11;
      x86asm_out(nat, SHLlri, 0);
      rm = temp1.parts.mappedto;
      reg = temp2.parts.mappedto;
      x86asm_out(nat, ORlrr, 0);
      alloc->vloc[reg_CPSRV].all |= var_FLAG;
    }

    rm = temp1.parts.mappedto;
    x86asm_out(nat, PUSHlr, POPF, 0);
/*    alloc->rctn[temp1.parts.mapnum].all = 0;
    alloc->rctn[temp2.parts.mapnum].all = 0;*/
  }
}

// generate instructions necessary to move from one allocation state into
// another
/*void allocate_massage(allocationinfo* from, allocationinfo* to,
                      nativeblockinfo* nat)
{
  uint5 i, mod, disp, rm, reg;

  #ifdef DEBUG
  fprintf(stderr, "Massaging at x86 address %x\n", nat->length);
  #endif

  for (i=0; i<NUMIREG; i++)
  {
    // if register is located in different places in two contexts...
    if ((from->rctn[i].all&0xffff) != (to->rctn[i].all&0xffff))
    {
      uint5 j;
      for (j=0; j<16; j++)
      {
        uint5 fromalloc = (from->rctn[i].all & (1<<j)) ? 1 : 0;
        uint5 toalloc = (to->rctn[i].all & (1<<j)) ? 1 : 0;
        // store registers...
        if (!toalloc && fromalloc && (from->vloc[j].all & var_REGISTER))
        {
          #ifdef DEBUG
          fprintf(stderr, "storing x86 reg %d mapped to intermediate reg %d\n",
                  i, j);
          #endif
          mod = mod_DISP8;
          disp = j*4;
          rm = EBP;
          reg = iregset[i];
          #define MODSIBDISP MODDISP8
          x86asm_out(nat, MOVlmr, 0);
          #undef MODSIBDISP
        }
        // load registers...
        if (toalloc && !fromalloc)
        {
          #ifdef DEBUG
          fprintf(stderr, "loading x86 reg %d mapped to intermediate reg %d\n", 
                  i, j);
          #endif
          mod = mod_DISP8;
          disp = j*4;
          rm = EBP;
          reg = iregset[i];
          #define MODSIBDISP MODDISP8
          x86asm_out(nat, MOVlrm, 0);
          #undef MODSIBDISP
          break;
        }
      }
    }
  }
}
*/
