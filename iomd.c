#include <stdlib.h>
#include <stdio.h>

#include "defs.h"
#include "iomd.h"

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
    break;
    
    case 0x08:  // Keyboard control
    break;
    
    case 0x0c:  // I/O port control
    break;
    
    case 0x10:  // IRQ A interrupt status
    break;
    
    case 0x14:  // IRQ A interrupt request/clear
    break;
    
    case 0x18:  // IRQ A interrupt mask
    break;
    
    case 0x1c:  // SUSPEND mode
    break;
    
    case 0x20:  // IRQ B interrupt status
    break;
    
    case 0x24:  // IRQ B interrupt request/clear
    break;
    
    case 0x28:  // IRQ B interrupt mask
    break;
    
    case 0x2c:  // STOP mode
    break;
    
    case 0x30:  // FIQ interrupts status
    break;
    
    case 0x34:  // FIQ interrupts request
    break;
    
    case 0x38:  // FIQ interrupts mask
    break;
    
    case 0x3c:  // Clock control
    break;
    
    case 0x40:  // Timer 0 LOW bits
    break;
    
    case 0x44:  // Timer 0 HIGH bits
    break;
    
    case 0x48:  // Timer 0 Go command
    break;
    
    case 0x4c:  // Timer 0 Latch command
    break;
    
    case 0x50:  // Timer 1 LOW bits
    break;
    
    case 0x54:  // Timer 1 HIGH bits
    break;
    
    case 0x58:  // Timer 1 Go command
    break;
    
    case 0x5c:  // Timer 1 Latch command
    break;
    
    case 0x60:  // IRQ C interrupts status
    break;
    
    case 0x64:  // IRQ C interrupts request
    break;
    
    case 0x68:  // IRQ C interrupts mask
    break;
    
    case 0x6c:  // Video LCD and Serial Sound Mux control
    break;
    
    case 0x70:  // IRQ D interrupts status
    break;
    
    case 0x74:  // IRQ D interrupts request
    break;
    
    case 0x78:  // IRQ D interrupts mask
    break;
    
    case 0x80:
    case 0x84:  // ROM control
    break;
    
    case 0x8c:  // Refresh period
    break;
    
    case 0x94:  // Chip ID number LOW byte
    break;
    
    case 0x98:  // Chip ID number HIGH byte
    break;
    
    case 0x9c:  // Chip version number
    break;
    
    case 0xa8:  // Mouse data
    break;
    
    case 0xac:  // Mouse control
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
    
    case 0x1f0:
    case 0x1f4:
    case 0x1f8:  // DMA interrupt control
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
    break;
    
    case 0x0c:  // I/O port control
    break;
    
    case 0x10:  // IRQ A interrupt status
    break;
    
    case 0x14:  // IRQ A interrupt request/clear
    break;
    
    case 0x18:  // IRQ A interrupt mask
    break;
    
    case 0x1c:  // SUSPEND mode
    break;
    
    case 0x20:  // IRQ B interrupt status
    break;
    
    case 0x24:  // IRQ B interrupt request/clear
    break;
    
    case 0x28:  // IRQ B interrupt mask
    break;
    
    case 0x2c:  // STOP mode
    break;
    
    case 0x30:  // FIQ interrupts status
    break;
    
    case 0x34:  // FIQ interrupts request
    break;
    
    case 0x38:  // FIQ interrupts mask
    break;
    
    case 0x3c:  // Clock control
    break;
    
    case 0x40:  // Timer 0 LOW bits
    break;
    
    case 0x44:  // Timer 0 HIGH bits
    break;
    
    case 0x48:  // Timer 0 Go command
    break;
    
    case 0x4c:  // Timer 0 Latch command
    break;
    
    case 0x50:  // Timer 1 LOW bits
    break;
    
    case 0x54:  // Timer 1 HIGH bits
    break;
    
    case 0x58:  // Timer 1 Go command
    break;
    
    case 0x5c:  // Timer 1 Latch command
    break;
    
    case 0x60:  // IRQ C interrupts status
    break;
    
    case 0x64:  // IRQ C interrupts request
    break;
    
    case 0x68:  // IRQ C interrupts mask
    break;
    
    case 0x6c:  // Video LCD and Serial Sound Mux control
    break;
    
    case 0x70:  // IRQ D interrupts status
    break;
    
    case 0x74:  // IRQ D interrupts request
    break;
    
    case 0x78:  // IRQ D interrupts mask
    break;
    
    case 0x80:
    case 0x84:  // ROM control
    break;
    
    case 0x8c:  // Refresh period
    break;
    
    case 0x94:  // Chip ID number LOW byte
    break;
    
    case 0x98:  // Chip ID number HIGH byte
    break;
    
    case 0x9c:  // Chip version number
    break;
    
    case 0xa8:  // Mouse data
    break;
    
    case 0xac:  // Mouse control
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
    
    case 0x1f0:
    case 0x1f4:
    case 0x1f8:  // DMA interrupt control
    break;
  }
  return 0;
}
