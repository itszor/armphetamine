# A little makefile, copied from someone else

CFLAGS = -g -Wall -O6 -march=pentium -DQUICKREAD -DQUICKWRITE -DASM_ALU -DARM26BIT -DVERSION=\"0.1\"

LDFLAGS= -g -lm

OBJ =	allocate.o \
analyse.o \
asmalu.o \
asmalutab.o \
block.o \
cnew.o \
codegen.o \
decode3.o \
disassemble.o \
execute.o \
fakesys.o \
generators.o \
hash.o \
list.o \
loadaout.o \
machine.o \
main.o \
memory.o \
nativesupport.o \
pqueue.o \
pseudo.o \
pseudodism.o \
registers.o \
riscos.o \
x86asm.o \
x86dism.o \
rtasm.o \
rtasm_fns.o

LIBS = 

all: emulate

clean:
	rm -f *.o emulate rtasm_fns.c rtasm_fns.h emit2_fns.h

cleaner:
	rm -f *.o emulate rtasm_fns.c rtasm_fns.h emit2_fns.h .depend

emulate: $(OBJ)
	$(CC) -o emulate $(OBJ) $(LDFLAGS) $(LIBS) 

test:
	cp test.c /net/jtb20.quns.cam.ac.uk/home/jules/test.c
	rsh jtb20.quns.cam.ac.uk "gcc -O2 test.c -o object"
	cp /net/jtb20.quns.cam.ac.uk/home/jules/object .

#gen_dp:	gen_dp.c
#	$(CC) -O6 gen_dp.c -o gen_dp

emit2.h:	emit2_fns.h
rtasm.h:	rtasm_fns.h
rtasm.c:	rtasm_fns.h

emit2_fns.h:	emit2.c
	gcc -DE2HEADER -E emit2.c | grep "void p86_"|grep -v "void p86_gen"|sed s/\{.*\}//g |sed "s/void/extern void/g" > emit2_fns.h

rtasm_fns.c:	mkintel.pl intel.dat
	./mkintel.pl > rtasm_fns.c

rtasm_fns.h:	rtasm_fns.c
	./rtasm_mkheader.pl > rtasm_fns.h

rtasm_check:	rtasm_fns.c
	gcc rtasm_fns.c -o /dev/null 2> error.log

asmalu.o:	gen_dp.pl
	./gen_dp.pl | as -o asmalu.o

#.c.o:
#	$(CC) -c $(CFLAGS) $< 

.depend:	Makefile
	gcc $(CFLAGS) -MM *.c > .depend

include .depend

# DO NOT DELETE
