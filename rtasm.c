#include "defs.h"
#include "cnew.h"
#include "x86asm.h"
#include "rtasm.h"

nativeblockinfo* rtasm_new(void)
{
  nativeblockinfo* nat = cnew(nativeblockinfo);
  nat->base = cnewarray(char, nat->size=2048);
  nat->length = 0;
  return nat;
}

void rtasm_putbyte(nativeblockinfo* nat, uint3 byte)
{
  nat->base[nat->length] = byte;
  if (++nat->length==nat->size)
    nat->base = realloc(nat->base, nat->size *= 2);
}

void rtasm_mem(nativeblockinfo* nat, rtasm_mtype* mem)
{
  switch (mem->mod)
  {
    case mod_INDIRECT:
    {
      if (mem->rm == USESIB)
      {
        rtasm_putbyte(nat, mem->scale | (mem->index<<2) | (mem->base<<5));
      }
      else if (mem->rm == USEDISP)
      {
        rtasm_putbyte(nat, mem->disp & 0xff);
        rtasm_putbyte(nat, (mem->disp>>8) & 0xff);
        rtasm_putbyte(nat, (mem->disp>>16) & 0xff);
        rtasm_putbyte(nat, mem->disp>>24);
      }
      // else nothing else needs to be added
    }
    break;
    
    case mod_DISP8:
    {
      if (mem->rm == USESIB)
      {
        rtasm_putbyte(nat, mem->scale | (mem->index<<2) | (mem->base<<5));
      }
      rtasm_putbyte(nat, mem->disp);
    }
    break;
    
    case mod_DISP32:
    {
      if (mem->rm == USESIB)
      {
        rtasm_putbyte(nat, mem->scale | (mem->index<<2) | (mem->base<<5));
      }
      rtasm_putbyte(nat, mem->disp & 0xff);
      rtasm_putbyte(nat, (mem->disp>>8) & 0xff);
      rtasm_putbyte(nat, (mem->disp>>16) & 0xff);
      rtasm_putbyte(nat, mem->disp>>24);
    }
    break;
    
    case mod_REGISTER:
    // nothing extra needs to be done
    break;
  }
}

// use with care, I guess.
rtasm_mtype rtasm_reg(uint3 r)
{
  rtasm_mtype thing;
  thing.mod = mod_REGISTER;
  thing.rm = r;
  return thing;
}

rtasm_mtype rtasm_ind(uint3 r)
{
  rtasm_mtype thing;
  thing.mod = mod_INDIRECT;
  thing.rm = r;
  return thing;
}

rtasm_mtype rtasm_ind8(uint3 base, uint3 offset)
{
  rtasm_mtype thing;
  thing.mod = mod_DISP8;
  thing.rm = base;
  thing.disp = offset;
  return thing;
}

rtasm_mtype rtasm_ind32(uint3 base, uint5 offset)
{
  rtasm_mtype thing;
  thing.mod = mod_DISP32;
  thing.rm = base;
  thing.disp = offset;
  return thing;
}

rtasm_mtype rtasm_scind(uint3 base, uint3 index, uint3 scale)
{
  rtasm_mtype thing;
  thing.mod = mod_INDIRECT;
  thing.rm = USESIB;
  thing.base = base;
  thing.index = index;
  thing.scale = scale;
  return thing;
}

rtasm_mtype rtasm_scind8(uint3 base, uint3 index, uint3 scale, uint3 offset)
{
  rtasm_mtype thing;
  thing.mod = mod_DISP8;
  thing.rm = USESIB;
  thing.base = base;
  thing.index = index;
  thing.scale = scale;
  thing.disp = offset;
  return thing;
}

rtasm_mtype rtasm_scind32(uint3 base, uint3 index, uint3 scale, uint5 offset)
{
  rtasm_mtype thing;
  thing.mod = mod_DISP32;
  thing.rm = USESIB;
  thing.base = base;
  thing.index = index;
  thing.scale = scale;
  thing.disp = offset;
  return thing;
}

rtasm_mtype rtasm_addr(uint5 address)
{
  rtasm_mtype thing;
  thing.mod = mod_INDIRECT;
  thing.rm = USEDISP;
  thing.disp = address;
  return thing;
}
