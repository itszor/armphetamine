#ifndef FAKESYS_H
#define FAKESYS_H 1

#include "machine.h"

#define VERBOSESYSCALL 1
//#undef VERBOSESYSCALL

extern void fake_syscall(machineinfo* machine, uint5 number);

#endif
