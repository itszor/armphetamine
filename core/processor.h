#ifndef PROCESSOR_H
#define PROCESSOR_H 1

#include "libjtype/defs.h"

// flags are held in Intel EFLAG register format, just because, even for
// non-recompiled emulation. If direct access to flags is required, necessary
// PC reconstruction will be done there.
// (this is no longer true...)
typedef union {
  struct {
    uint5 c						: 1;  // previously 'carry'
    uint5 padding     : 1;
    uint5 parity      : 1;
    uint5 padding2    : 1;
    uint5 auxiliary   : 1;
    uint5 padding3    : 1;
    uint5 z		        : 1;	// previously 'zero'
    uint5 n						: 1;	// previously 'sign'
    uint5 trap        : 1;
    uint5 interrupt   : 1;
    uint5 direction   : 1;
    uint5 v						: 1;	// previously 'overflow'
    uint5 iopriv      : 2;
    uint5 nestedtask  : 1;
    uint5 padding4    : 1;
    uint5 resume      : 1;
    uint5 virtualmode : 1;
    uint5 padding5    : 14;
  } flag;
  uint5 value;
} x86flaginfo;

// Program counter in a 26-bit mode
typedef union {
	struct {
		uint5 mode			: 2;
		uint5 address		: 24;
		uint5 interrupt	: 2;
		uint5 v					: 1;
		uint5 c					: 1;
		uint5 z					: 1;
		uint5 n					: 1;
	} flag;
	uint5 value;
} pcinfo;

// Processor status register format on a 32-bit capable processor
typedef union {
	struct {
		uint5 mode			: 5;
		uint5 padding		: 1;
		uint5 interrupt : 2;
		uint5 padding2	: 20;
		uint5 v					: 1;
		uint5 c					: 1;
		uint5 z					: 1;
		uint5 n					: 1;
	} flag;
	uint5 value;
} psrinfo;

typedef enum {
  pm_USR26=0,
	pm_FIQ26,
	pm_IRQ26,
	pm_SVC26,
  pm_USR32=16,
	pm_FIQ32,
	pm_IRQ32,
	pm_SVC32,
	pm_ABT32=23,
	pm_UND32=27
} modenumber;

struct machineinfo;

extern void processor_reg_savecurrent(struct machineinfo*, uint5);
extern void processor_reg_restorenew(struct machineinfo*, uint5);
extern void processor_mode(struct machineinfo*, uint5 omode, uint5 nmode);
extern void processor_fiq(struct machineinfo*);
extern void processor_irq(struct machineinfo*);
extern void processor_prefetchabort(struct machineinfo*);
extern void processor_dataabort(struct machineinfo*);
extern void processor_swi(struct machineinfo*);
extern void processor_und(struct machineinfo*);
extern void processor_reset(struct machineinfo*);

#endif
