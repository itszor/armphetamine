#include <stdio.h>
#include <pty.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "defs.h"
#include "sapcm.h"
#include "memory.h"

void sa1100_serial_initialise(meminfo* mem)
{
  pid_t child;
  char ptyname[40];
  int namelen;
  int devno;

  if (openpty(&mem->sapcm.amaster, &mem->sapcm.aslave, ptyname,
    NULL, NULL) == -1)
  {
    fprintf(stderr, "Can't open pty, error=%d\n", errno);
    return errno;
  }
  
  namelen = strlen(ptyname);
/*  fprintf(stderr, "ptyname=%s\n", ptyname);*/
  for (; ptyname[namelen]!='/'; namelen--);
  sscanf(&ptyname[namelen+1], "%d", &devno);
/*  fprintf(stderr, "devno=%d\n", devno);*/
    
  if ((child=fork())==0)
  {
    // we are the child
    char slave[40];
  
    sprintf(slave, "-S%c%c%d", "pqrs"[devno/16], 
      "0123456789abcdef"[devno&15], mem->sapcm.amaster);
  /*  fprintf(stderr, "Using slave %s\n", slave);*/
    execlp("/usr/X11/bin/xterm", "/usr/X11/bin/xterm", &slave[0], "-T", 
      "VirtuaLART", NULL);
  }
 /* sleep(1);*/
}

uint5 sa1100_serial_read(meminfo* mem, uint5 physaddress)
{
 /* fprintf(stderr, "Read from serial register %.8x\n", physaddress);*/
  switch ((physaddress>>16) & 0xf)
  {  
    case 0x5:
    {
      switch (physaddress & 0xff)
      {
        case 0x00:
        return mem->sapcm.serial_fifo->uart_control_0;

        case 0x04:
        return mem->sapcm.serial_fifo->uart_control_0;

        case 0x08:
        return mem->sapcm.serial_fifo->uart_control_0;

        case 0x0c:
        return mem->sapcm.serial_fifo->uart_control_0;

        case 0x14:
        return mem->sapcm.serial_fifo->uart_data;

        case 0x1c:
        return mem->sapcm.serial_fifo->uart_status_0;
        
        case 0x20:
        return mem->sapcm.serial_fifo->uart_status_1;
      }
    }
    break;
    
    default:
    fprintf(stderr, "Unimplemented serial register\n");
  }
  return 0xdeadbeef;
}

void sa1100_serial_write(meminfo* mem, uint5 physaddress, uint5 data)
{
/*  fprintf(stderr, "Write %.8x to serial register %.8x\n", data, physaddress);*/
  switch ((physaddress>>16) & 0xf)
  {  
    case 0x5:
    {
      switch (physaddress & 0xff)
      {
        case 0x00:
        mem->sapcm.serial_fifo->uart_control_0 = data;
        break;

        case 0x04:
        mem->sapcm.serial_fifo->uart_control_0 = data;
        break;

        case 0x08:
        mem->sapcm.serial_fifo->uart_control_0 = data;
        break;

        case 0x0c:
        mem->sapcm.serial_fifo->uart_control_0 = data;
        break;

        case 0x14:
        mem->sapcm.serial_fifo->uart_data = data;
        write(mem->sapcm.aslave, &data, 1);
        break;

        case 0x1c:
        mem->sapcm.serial_fifo->uart_status_0 = data;
        break;
        
        case 0x20:
        mem->sapcm.serial_fifo->uart_status_1 = data;
        break;
      }
    }
    break;
    
    default:
    fprintf(stderr, "Unimplemented serial register\n");
  }
}

uint5 sa1100_lcd_read(meminfo* mem, uint5 physaddress)
{
  switch (physaddress>>24)
  {
    case 0x00:
    {
      uint5 channel = physaddress >> 5;
      switch (physaddress & 0x1f)
      {
        case 0x00:
        return mem->sapcm.dma[channel].device_address;

        case 0x04:
        case 0x08:
        case 0x0c:
        return mem->sapcm.dma[channel].control_status;

        case 0x10:
        return mem->sapcm.dma[channel].buffer_a_start_address;

        case 0x14:
        return mem->sapcm.dma[channel].buffer_a_tfer_count;

        case 0x18:
        return mem->sapcm.dma[channel].buffer_b_start_address;

        case 0x1c:
        return mem->sapcm.dma[channel].buffer_b_tfer_count;
      }
    }
    break;
    
    case 0x10:
    {
      fprintf(stderr, "Read LCD stuff\n");
    }
    break;
  }
  return 0xdeadbeef;
}

void sa1100_lcd_write(meminfo* mem, uint5 physaddress, uint5 data)
{
  fprintf(stderr, "Write %.8x to lcd register %.8x\n", data, physaddress);
  switch (physaddress>>24)
  {
    case 0x00:
    {
      uint5 channel = physaddress >> 5;
      switch (physaddress & 0x1f)
      {
        case 0x00:
        mem->sapcm.dma[channel].device_address = data;
        break;

        case 0x04:
        mem->sapcm.dma[channel].control_status |= data;
        break;
        
        case 0x08:
        mem->sapcm.dma[channel].control_status &= ~data;
        break;

        case 0x0c:
        /* read only, do nothing */
        break;

        case 0x10:
        mem->sapcm.dma[channel].buffer_a_start_address = data;
        break;

        case 0x14:
        mem->sapcm.dma[channel].buffer_a_tfer_count = data;
        break;

        case 0x18:
        mem->sapcm.dma[channel].buffer_b_start_address = data;
        break;

        case 0x1c:
        mem->sapcm.dma[channel].buffer_b_tfer_count = data;
        break;
      }
    }
    break;
    
    case 0x10:
    {
      fprintf(stderr, "Write LCD stuff\n");
    }
    break;
  }
}

void sapcm_clock(machineinfo* machine)
{
  meminfo* mem = machine->mem;
 /* fprintf(stderr, "Clocked serial\n");*/
  mem->sapcm.serial_fifo->uart_status_1 =
      (fifo_empty(mem->sapcm.serial_fifo->in) ? 0 : 2)
    | (fifo_full(mem->sapcm.serial_fifo->out) ? 0 : 4);
}
