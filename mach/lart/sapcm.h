#ifndef SAPCM_H
#define SAPCM_H 1

/* SA-1100 peripheral control module */

#include "defs.h"
#include "memory.h"
#include "machine.h"
#include "fifo.h"

struct sapcm_dma_channel {
  uint5 device_address;
  uint5 control_status;
  uint5 buffer_a_start_address;
  uint5 buffer_a_tfer_count;
  uint5 buffer_b_start_address;
  uint5 buffer_b_tfer_count;
};

typedef struct sapcm_dma_channel sapcm_dma_channel;

struct sapcm_serial_fifo {
  fifo_info* in;
  fifo_info* out;
  uint5 uart_status_0;
  uint5 uart_status_1;
  uint5 uart_control_0;
  uint5 uart_control_1;
  uint5 uart_control_2;
  uint5 uart_control_3;
  uint5 uart_data;
};

typedef struct sapcm_serial_fifo sapcm_serial_fifo;

#define sapcm_INTERRUPT_LCD          12
#define sapcm_INTERRUPT_SERIAL0      13
#define sapcm_INTERRUPT_SERIAL1_SDLC 14
#define sapcm_INTERRUPT_SERIAL1_UART 15
#define sapcm_INTERRUPT_SERIAL2_ICP  16
#define sapcm_INTERRUPT_SERIAL3_UART 17
#define sapcm_INTERRUPT_SERIAL4_MCP  18
#define sapcm_INTERRUPT_SERIAL4_SSP  19

extern void sa1100_serial_initialise(meminfo* mem);

extern uint5 sa1100_serial_read(meminfo* mem, uint5 physaddress);

extern void sa1100_serial_write(meminfo* mem, uint5 physaddress, uint5 data);

extern uint5 sa1100_lcd_read(meminfo* mem, uint5 physaddress);

extern void sa1100_lcd_write(meminfo* mem, uint5 physaddress, uint5 data);

extern void sapcm_clock(machineinfo* machine);

#endif
