# Really lame global makefile.
# Make a particular target by going into mach/* and executing "make".
# Make sure you "make clean" first!

# choose target 
all:	clean

clean:
	make -C mach/lart clean
	make -C mach/riscose clean
	make -C mach/riscpc clean
	make -C mach/rohle clean

virtualart:
	make -C mach/lart

riscose:
	make -C mach/riscose

riscpc:
	make -C mach/riscpc

rohle:
	make -C mach/rohle
