# A little makefile, copied from someone else

CC	= gcc
CFLAGS	= -g -pipe -Wall -O6 -mcpu=k6 -march=k6 -DASM_ALU -DQUICKWRITE \
	  -DVERSION=\"0.2\" -DVIDCSUPPORT -DIOMDSUPPORT
INCLUDE	= -I/usr/include -I/usr/local/include -I/usr/include/readline -I.

LDFLAGS= -g

OBJ =	allocate.o analyse.o block.o cnew.o codegen.o decode.o disassemble.o \
	execute26.o execute32.o fakesys.o generators.o hash.o list.o \
	loadaout.o machine.o main.o memory.o nativesupport.o pqueue.o \
	pseudo.o pseudodism.o registers.o riscos.o x86asm.o x86dism.o \
	rtasm.o rtasm_fns.o flush.o pheta.o phetadism.o processor.o vidc20.o \
	iomd.o asmalu.o debug.o bset.o asmalutab.o

SRC =	allocate.c analyse.c asmalutab.c block.c cnew.c codegen.c decode.c \
	disassemble.c execute26.c execute32.c fakesys.c flush.c generators.c \
	hash.c iomd.c list.c loadaout.c machine.c main.c memory.c \
	nativesupport.c pheta.c phetadism.c pqueue.c processor.c pseudo.c \
	pseudodism.c registers.c riscos.c rtasm.c vidc20.c x86asm.c x86dism.c \
	rtasm.c flush.c pheta.c phetadism.c vidc20.c iomd.c debug.c bset.c \
	asmalutab.c

LIBS = -lm -lreadline -lhistory -lncurses -lSDL -lpthread

.PHONY:	clean cleaner package webpkg romdump

all:	.depend emulate simple

clean:
	rm -f *.o emulate

cleaner:
	rm -f *.o emulate rtasm_fns.c rtasm_fns.h .depend

execute26.c:	execute.inc.c
execute32.c:	execute.inc.c

emulate: $(OBJ)
	$(CC) -o emulate $(OBJ) $(LDFLAGS) $(LIBS) 

simple:	simple.arm.s
	arm-linux-elf-as simple.arm.s -o simple

package:	cleaner
	tar -c -v -z -C .. -f ../armphetamine-0.2.tar.gz armphetamine

webpkg:
	tar -c -v -z -C docs -f ../webphetamine.tar.gz html

romdump:
	arm-linux-elf-objdump --disassemble-all --target=binary \
	--architecture=arm Rom | less

#test:
#	cp test.c /net/jtb20.quns.cam.ac.uk/home/jules/test.c
#	rsh jtb20.quns.cam.ac.uk "gcc -O2 test.c -o object"
#	cp /net/jtb20.quns.cam.ac.uk/home/jules/object .

#gen_dp:	gen_dp.c
#	$(CC) -O6 gen_dp.c -o gen_dp

#emit2.h:	emit2_fns.h
rtasm.h:	rtasm_fns.h
rtasm.c:	rtasm_fns.h
rtasm_fns.c:	mkintel.pl intel.dat

#emit2.c:	emit2.magic
#	touch emit2.magic

#emit2_fns.h:	emit2.magic
#	gcc -DE2HEADER $(INCLUDE) -E emit2.c | grep "void p86_"|grep -v "void p86_gen"|sed s/\{.*\}//g |sed "s/void/extern void/g" > emit2_fns.h
#	rm -f emit2.magic

rtasm_fns.c:	mkintel.pl
	./mkintel.pl > rtasm_fns.c

rtasm_fns.h:	rtasm_fns.c
	./rtasm_mkheader.pl > rtasm_fns.h

rtasm_check:	rtasm_fns.c
	gcc rtasm_fns.c -o /dev/null 2> error.log

asmalu.o:	gen_dp.pl
	./gen_dp.pl | as -o asmalu.o

%.o:	%.c
	$(CC) -c $(INCLUDE) $(CFLAGS) -c $< -o $@


.depend:	rtasm_fns.h Makefile
	$(CC) $(CFLAGS) $(INCLUDE) -MM $(SRC) > .depend

include .depend

