#include "defs.h"
#include "memory.h"
#include "mexreg.h"
#include "cnew.h"

mex_registers* mexreg_new()
{
  return jt_new(mex_registers);
}

void sa1100_mexreg_write(meminfo* mem, uint5 physaddress, uint5 data)
{
  fprintf(stderr, "Memory/expansion register write, addr %.8x data %.8x\n",
    physaddress, data);
  switch (physaddress & 0xff)
  {
    case 0x00: mem->mex->dramconfig = data; break;
    case 0x04: mem->mex->casshift0 = data; break;
    case 0x08: mem->mex->casshift1 = data; break;
    case 0x0c: mem->mex->casshift2 = data; break;
    case 0x10: mem->mex->staticctrl0 = data; break;
    case 0x14: mem->mex->staticctrl1 = data; break;
    case 0x18: mem->mex->expansionctrl = data; break;
  }
}

uint5 sa1100_mexreg_read(meminfo* mem, uint5 physaddress)
{
  fprintf(stderr, "Memory/expansion register read, addr %.8x\n", physaddress);
  switch (physaddress & 0xff)
  {
    case 0x00: return mem->mex->dramconfig;
    case 0x04: return mem->mex->casshift0;
    case 0x08: return mem->mex->casshift1;
    case 0x0c: return mem->mex->casshift2;
    case 0x10: return mem->mex->staticctrl0;
    case 0x14: return mem->mex->staticctrl1;
    case 0x18: return mem->mex->expansionctrl;
  }
  return 0x0;
}
