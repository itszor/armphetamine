#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "defs.h"
#include "iomd.h"
#include "keyboard.h"
#include "mouse.h"
#include "machine.h"

#ifdef IOMDSUPPORT

void iomd_writebyte(meminfo* mem, uint5 address, uint5 data)
{
  iomd_writeword(mem, address, data);
}

void iomd_writeword(meminfo* mem, uint5 address, uint5 data)
{
  fprintf(stderr, "Writing %x to IOMD address %x\n", data,
          address & 0x000fffff);
          
  switch (address & 0x000fffff)
  {
    case 0x00:  // I/O control
    break;
    
    case 0x04:  // Keyboard data
    mem->io.kbd.busy = 1;
    break;
    
    case 0x08:  // Keyboard control
    if (data==8) mem->io.kbd.busy = 0;
    break;
    
    case 0x0c:  // I/O port control
    break;
    
    case 0x10:  // IRQ A interrupt status
    break;
    
    case 0x14:  // IRQ A interrupt request/clear
    mem->io.irqa.request = data;
    break;
    
    case 0x18:  // IRQ A interrupt mask
    mem->io.irqa.mask = data;
    break;
    
    case 0x1c:  // SUSPEND mode
    break;
    
    case 0x20:  // IRQ B interrupt status
    break;
    
    case 0x24:  // IRQ B interrupt request/clear
    mem->io.irqb.request = data;
    break;
    
    case 0x28:  // IRQ B interrupt mask
    mem->io.irqb.mask = data;
    break;
    
    case 0x2c:  // STOP mode
    break;
    
    case 0x30:  // FIQ interrupts status
    break;
    
    case 0x34:  // FIQ interrupts request
    mem->io.fiq.request = data;
    break;
    
    case 0x38:  // FIQ interrupts mask
    mem->io.fiq.mask = data;
    break;
    
    case 0x3c:  // Clock control
    fprintf(stderr, "Writing to clock control\n");
    break;
    
    case 0x40:  // Timer 0 LOW bits
    mem->io.t0lolat = data;
    break;
    
    case 0x44:  // Timer 0 HIGH bits
    mem->io.t0hilat = data;
    break;
    
    case 0x48:  // Timer 0 Go command
    {
      mem->io.timer0 = mem->io.t0lolat | (mem->io.t0hilat << 8);
      fprintf(stderr, "Timer 0 is go! (%d)\n", mem->io.timer0);
    }
    break;
    
    case 0x4c:  // Timer 0 Latch command
    mem->io.t0lolat = mem->io.timer0 & 0xff;
    mem->io.t0hilat = (mem->io.timer0 >> 8) & 0xff;
/*    if (data==1) ((machineinfo*)mem->parent)->trace = 1;*/
    break;
    
    case 0x50:  // Timer 1 LOW bits
    mem->io.t1lolat = data;
    break;
    
    case 0x54:  // Timer 1 HIGH bits
    mem->io.t1hilat = data;
    break;
    
    case 0x58:  // Timer 1 Go command
    {
      mem->io.timer1 = mem->io.t1lolat | (mem->io.t1hilat << 8);
      fprintf(stderr, "Timer 1 is go! (%d)\n", mem->io.timer1);
    }
    break;
    
    case 0x5c:  // Timer 1 Latch command
    mem->io.t1lolat = mem->io.timer1 & 0xff;
    mem->io.t1hilat = (mem->io.timer1 >> 8) & 0xff;
    break;
    
    case 0x60:  // IRQ C interrupts status
    break;
    
    case 0x64:  // IRQ C interrupts request
    mem->io.irqc.request = data;
    break;
    
    case 0x68:  // IRQ C interrupts mask
    mem->io.irqc.mask = data;
    break;
    
    case 0x6c:  // Video LCD and Serial Sound Mux control
    break;
    
    case 0x70:  // IRQ D interrupts status
    break;
    
    case 0x74:  // IRQ D interrupts request
    mem->io.irqd.request = data;
    break;
    
    case 0x78:  // IRQ D interrupts mask
    mem->io.irqd.mask = data;
    break;
    
    case 0x80:
    case 0x84:  // ROM control
    break;
    
    case 0x8c:  // Refresh period
    if (data==1)
      fprintf(stderr, "16us refresh period\n");
    else
      fprintf(stderr, "No refresh\n");
    break;
    
    case 0x94:  // Chip ID number LOW byte
    break;
    
    case 0x98:  // Chip ID number HIGH byte
    break;
    
    case 0x9c:  // Chip version number
    break;
    
    case 0xa8:  // Mouse data
    mem->io.mouse.busy = 1;
    break;
    
    case 0xac:  // Mouse control
    if (data==8) mem->io.mouse.busy = 0;
    break;
    
    case 0xc4:  // I/O timing control
    break;
    
    case 0xc8:  // I/O Expansion card timing control
    break;
    
    case 0xcc:  // I/O Asynchronous timing control
    break;
    
    case 0xd0:  // DRAM width control
    break;
    
    case 0xd4:  // DRAM self-refresh control
    break;
    
    case 0xe0:  // A to D interrupt control
    break;
    
    case 0xe4:  // A to D status
    break;
    
    case 0xe8:  // A to D converter control
    break;
    
    case 0xec:  // A to D counter 1
    break;
    
    case 0xf0:  // A to D counter 2
    break;
    
    case 0xf4:  // A to D counter 3
    break;
    
    case 0xf8:  // A to D counter 4
    break;
    
    case 0x180:  // Sound DMA Current A
    break;
    
    case 0x184:  // Sound DMA End A
    break;
    
    case 0x188:  // Sound DMA Current B
    break;
    
    case 0x18c:  // Sound DMA End B
    break;
    
    case 0x190:  // Sound DMA control
    break;
    
    case 0x194:  // Sound DMA status
    break;
    
    case 0x1c0:  // Cursor DMA current
    break;
    
    case 0x1c4:  // Cursor DMA init
    break;
    
    case 0x1c8:  // Duplex LCD video DMA current B
    break;
    
    case 0x1d0:  // Video DMA current A
    break;
    
    case 0x1d4:  // Video DMA End
    break;
    
    case 0x1d8:  // Video DMA Start
    break;
    
    case 0x1dc:  // Video DMA Init A
    break;
    
    case 0x1e0:  // Video DMA control
    break;
    
    case 0x1e8:  // Duplex LCD Video DMA Init B
    break;
    
    case 0x1f0:  // DMA interrupt status
    break;
    
    case 0x1f4:  // DMA interrupt request
    mem->io.sounddma.request = data;
    break;
    
    case 0x1f8:  // DMA interrupt mask
    mem->io.sounddma.mask = data;
    break;
  }
}

uint5 iomd_readbyte(meminfo* mem, uint5 address)
{
  return iomd_readword(mem, address) & 0xff;
}

uint5 iomd_readword(meminfo* mem, uint5 address)
{
  fprintf(stderr, "Reading from IOMD address %x\n", address & 0x000fffff);
          
  switch (address & 0x000fffff)
  {
    case 0x00:  // I/O control
    break;
    
    case 0x04:  // Keyboard data
    break;
    
    case 0x08:  // Keyboard control
    if (!mem->io.kbd.busy) return 128; else return 0;
    break;
    
    case 0x0c:  // I/O port control
    break;
    
    case 0x10:  // IRQ A interrupt status
    break;
    
    case 0x14:  // IRQ A interrupt request/clear
    return mem->io.irqa.request & mem->io.irqa.mask;
    break;
    
    case 0x18:  // IRQ A interrupt mask
    return mem->io.irqa.mask;
    break;
    
    case 0x1c:  // SUSPEND mode
    break;
    
    case 0x20:  // IRQ B interrupt status
    break;
    
    case 0x24:  // IRQ B interrupt request/clear
    return mem->io.irqb.request & mem->io.irqb.mask;
    break;
    
    case 0x28:  // IRQ B interrupt mask
    return mem->io.irqb.mask;
    break;
    
    case 0x2c:  // STOP mode
    break;
    
    case 0x30:  // FIQ interrupts status
    break;
    
    case 0x34:  // FIQ interrupts request
    return mem->io.fiq.request & mem->io.fiq.mask;
    break;
    
    case 0x38:  // FIQ interrupts mask
    return mem->io.fiq.mask;
    break;
    
    case 0x3c:  // Clock control
    break;
    
    case 0x40:  // Timer 0 LOW bits
    return mem->io.t0lolat;
    break;
    
    case 0x44:  // Timer 0 HIGH bits
    return mem->io.t0hilat;
    break;
    
    case 0x48:  // Timer 0 Go command
    break;
    
    case 0x4c:  // Timer 0 Latch command
    break;
    
    case 0x50:  // Timer 1 LOW bits
    return mem->io.t1lolat;
    break;
    
    case 0x54:  // Timer 1 HIGH bits
    return mem->io.t1hilat;
    break;
    
    case 0x58:  // Timer 1 Go command
    break;
    
    case 0x5c:  // Timer 1 Latch command
    break;
    
    case 0x60:  // IRQ C interrupts status
    break;
    
    case 0x64:  // IRQ C interrupts request
    return mem->io.irqc.request & mem->io.irqc.mask;
    break;
    
    case 0x68:  // IRQ C interrupts mask
    return mem->io.irqc.mask;
    break;
    
    case 0x6c:  // Video LCD and Serial Sound Mux control
    break;
    
    case 0x70:  // IRQ D interrupts status
    break;
    
    case 0x74:  // IRQ D interrupts request
    return mem->io.irqd.request & mem->io.irqd.mask;
    break;
    
    case 0x78:  // IRQ D interrupts mask
    return mem->io.irqd.mask;
    break;
    
    case 0x80:
    case 0x84:  // ROM control
    break;
    
    case 0x8c:  // Refresh period
    break;
    
    case 0x94:  // Chip ID number LOW byte
    return 152;
    break;
    
    case 0x98:  // Chip ID number HIGH byte
    return 91;
    break;
    
    case 0x9c:  // Chip version number
    break;
    
    case 0xa8:  // Mouse data
    break;
    
    case 0xac:  // Mouse control
    if (!mem->io.mouse.busy) return 128; else return 0;
    break;
    
    case 0xc4:  // I/O timing control
    fprintf(stderr, "Setting I/O timing control\n");
    break;
    
    case 0xc8:  // I/O Expansion card timing control
    fprintf(stderr, "Setting I/O expansion card timing control\n");
    break;
    
    case 0xcc:  // I/O Asynchronous timing control
    break;
    
    case 0xd0:  // DRAM width control
    return 0; // 32-bit DRAM banks
    break;
    
    case 0xd4:  // DRAM self-refresh control
    break;
    
    case 0xe0:  // A to D interrupt control
    break;
    
    case 0xe4:  // A to D status
    break;
    
    case 0xe8:  // A to D converter control
    break;
    
    case 0xec:  // A to D counter 1
    break;
    
    case 0xf0:  // A to D counter 2
    break;
    
    case 0xf4:  // A to D counter 3
    break;
    
    case 0xf8:  // A to D counter 4
    break;
    
    case 0x180:  // Sound DMA Current A
    break;
    
    case 0x184:  // Sound DMA End A
    break;
    
    case 0x188:  // Sound DMA Current B
    break;
    
    case 0x18c:  // Sound DMA End B
    break;
    
    case 0x190:  // Sound DMA control
    break;
    
    case 0x194:  // Sound DMA status
    break;
    
    case 0x1c0:  // Cursor DMA current
    break;
    
    case 0x1c4:  // Cursor DMA init
    break;
    
    case 0x1c8:  // Duplex LCD video DMA current B
    break;
    
    case 0x1d0:  // Video DMA current A
    break;
    
    case 0x1d4:  // Video DMA End
    break;
    
    case 0x1d8:  // Video DMA Start
    break;
    
    case 0x1dc:  // Video DMA Init A
    break;
    
    case 0x1e0:  // Video DMA control
    break;
    
    case 0x1e8:  // Duplex LCD Video DMA Init B
    break;
    
    case 0x1f0:  // DMA interrupt status
    break;
    
    case 0x1f4:  // DMA interrupt request
    return mem->io.sounddma.request & mem->io.sounddma.mask;
    break;
    
    case 0x1f8:  // DMA interrupt mask
    return mem->io.sounddma.mask;
    break;
  }
  return 0;
}

#endif
