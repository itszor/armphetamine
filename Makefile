# A little makefile, copied from someone else

CC	= gcc
CFLAGS	= -g -O3 -pipe -W -Wall -DVERSION=\"0.4\" -DEMULART -DRECOMPILE
NASM	= nasm
AS	= as

INCLUDE	= -I/usr/include -I/usr/local/include -I/usr/include/readline -I.

LDFLAGS= -g 

OBJ =	cnew.o decode.o disassemble.o \
	execute26.o execute32.o fakesys.o hash.o list.o \
	loadaout.o machine.o main.o memory.o nativesupport.o pqueue.o \
	registers.o riscos.o x86dism.o asmalu.o \
	rtasm.o rtasm_fns.o flush.o pheta.o phetadism.o processor.o vidc20.o \
	iomd.o debug.o bset.o asmalutab.o fifo.o mouse.o keyboard.o \
	genx86.o palloc.o decodet.o executethm.o clist.o relocate.o \
	dynsupport.o genx86_tab.o transmap.o profile.o recompile.o sapcm.o \
	ostimer.o intctrl.o mexreg.o

STRUCTOBJ =	structsupport.o

TEMPFILES	= genx86_tab.c rtasm_fns.c rtasm_fns.h

TESTS =	divide simple armtest

LIBS = -lm -lreadline -lhistory -lncurses -lutil

.PHONY: clean cleaner package webpkg romdump lartrun

all:	emulate simple divide armtest

clean:
	rm -f *.o emulate $(TESTS)

cleaner:
	rm -f *.o *.d emulate $(TESTS) $(TEMPFILES) rtasm_fns.h

lartrun:	emulate
	./emulate "script lartup.txt"

emulate: $(OBJ)
	$(CC) -o emulate $(OBJ) $(LDFLAGS) $(LIBS) 

simple.o:	simple.arm.s
	arm-linux-as $< -o $@

divide.o:	divide.arm.s
	arm-linux-as $< -o $@

simple:	simple.o
	arm-linux-ld simple.o -o simple

divide:	divide.o
	arm-linux-ld divide.o -o divide

armtest:	armtest.c testtmpl.txt Makefile
	arm-linux-gcc -static -O2 armtest.c -o armtest
	./subst.sh

package:	cleaner
	tar -c -v -z -C .. -f ../armphetamine-0.2.tar.gz armphetamine

webpkg:
	tar -c -v -z -C docs -f ../webphetamine.tar.gz html

romdump:
	arm-linux-objdump --disassemble-all --target=binary \
	--architecture=arm Rom | less

#test:
#	cp test.c /net/jtb20.quns.cam.ac.uk/home/jules/test.c
#	rsh jtb20.quns.cam.ac.uk "gcc -O2 test.c -o object"
#	cp /net/jtb20.quns.cam.ac.uk/home/jules/object .

#gen_dp:	gen_dp.c
#	$(CC) -O6 gen_dp.c -o gen_dp

#emit2.h:	emit2_fns.h
#rtasm.h:	rtasm_fns.h
#rtasm.c:	rtasm_fns.h
#rtasm_fns.c:	mkintel.pl intel.dat

#emit2.c:	emit2.magic
#	touch emit2.magic

#emit2_fns.h:	emit2.magic
#	gcc -DE2HEADER $(INCLUDE) -E emit2.c | grep "void p86_"|grep -v "void p86_gen"|sed s/\{.*\}//g |sed "s/void/extern void/g" > emit2_fns.h
#	rm -f emit2.magic

rtasm_fns.c:	mkintel.pl
	./mkintel.pl > rtasm_fns.c

rtasm_fns.h:	rtasm_fns.c
	./rtasm_mkheader.pl > rtasm_fns.h

genx86_tab.c:	genx86tabsrc.dat mkx86tab.py
	./mkx86tab.py

structsupport:	$(STRUCTOBJ)
	gcc -O2 $< -o $@

structsupport.inc:	structsupport
	./structsupport > structsupport.inc

dynsupport.asm:	structsupport.inc

asmalu.s:	gen_dp.pl
	./gen_dp.pl > $@

asmalu.o:	asmalu.s
	$(AS) $< -o $@

%.o:	%.c
	$(CC) -c $(INCLUDE) $(CFLAGS) -c $< -o $@

%.o:	%.asm
	$(NASM) -f elf $< -o $@

%.d:	%.c $(TEMPFILES)
	$(CC) $(INCLUDE) -MM $< > $@

%.d:	%.asm
	$(NASM) -M $< > $@

asmalu.d:
	touch $@

include $(OBJ:.o=.d)
include $(STRUCTOBJ:.o=.d)

