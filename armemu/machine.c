#include "cnew.h"
#include "machine.h"
#include "hash.h"
#include "block.h"

machineinfo* machine_create(uint5 memory)
{
	machineinfo* machine = cnew(machineinfo);
	
	machine->mem = memory_initialise(memory);
	machine->reg = registers_new();
//	machine->x86flags.value = 0;
	machine->blocks = hash_new(17001);  // pick a number...
	machine->live = 0;
	machine->translaterules = 0;
	
	return machine;
}
