# A little makefile, copied from someone else

CC	= gcc
#CFLAGS	= -g -DVERSION=\"0.4\" -DEMULART
NASM	= nasm
AS	= as

LDFLAGS= -g 

LJT =           libjtype

LJTOBJS =       $(LJT)/bset.o $(LJT)/clist.o $(LJT)/list.o \
                $(LJT)/hash.o $(LJT)/cnew.o $(LJT)/pqueue.o

CORE =          core

COREOBJS =      $(CORE)/decode.o $(CORE)/decodet.o $(CORE)/disassemble.o \
                $(CORE)/execute26.o $(CORE)/execute32.o $(CORE)/executethm.o \
                $(CORE)/machine.o $(CORE)/memory.o $(CORE)/processor.o \
                $(CORE)/registers.o

DYNAREC =       dynarec

DROBJS =        $(DYNAREC)/block.o $(DYNAREC)/flush.o $(DYNAREC)/palloc.o \
                $(DYNAREC)/pheta.o $(DYNAREC)/phetadism.o $(DYNAREC)/profile.o \
                $(DYNAREC)/recompile.o $(DYNAREC)/relocate.o $(DYNAREC)/transmap.o

DRX86 =         $(DYNAREC)/arch/x86

DRX86OBJS =     $(DRX86)/genx86.o $(DRX86)/genx86_tab.o \
                $(DRX86)/nativesupport.o $(DRX86)/rtasm.o \
                $(DRX86)/x86dism.o

LART =          mach/lart

LARTOBJS =      $(LART)/fifo.o $(LART)/intctrl.o $(LART)/mexreg.o \
                $(LART)/ostimer.o $(LART)/ostimer.o $(LART)/sapcm.o \
                $(LART)/lartmem.o

ROHLE =         mach/rohle

ROHLEOBJS =     $(ROHLE)/fakesys.o $(ROHLE)/riscos.o

RISCPC =        mach/riscpc

RISCPCOBJS =    $(RISCPC)/iomd.o $(RISCPC)/keyboard.o $(RISCPC)/mouse.o \
                $(RISCPC)/vidc20.o

DEBUG =         shell/debug

DEBUGOBJS =     $(DEBUG)/debug.o $(DEBUG)/main.o

OBJ =	$(LJTOBJS) $(COREOBJS) $(DROBJS) $(DRX86OBJS) $(LARTOBJS) $(DEBUGOBJS)

LARTINCLUDE =   -I/usr/include -I/usr/local/include -I/usr/include/readline -I. \
                -Ilibjtype -Icore -Ishell/debug -Imach/lart -Idynarec \
                -Idynarec/arch/x86

LARTCFLAGS =    -g -O3 -pipe -W -Wall -DVERSION=\"0.4\" -DEMULART

ROHLEINCLUDE =  -I/usr/include -I/usr/local/include -I/usr/include/readline -I. \
                -Ilibjtype -Icore -Ishell/debug -Imach/rohle -Idynarec \
                -Idynarec/arch/x86

ROHLECFLAGS =   -g -O3 -pipe -W -Wall -DVERSION=\"0.4\" -DROHLE

RISCPCCFLAGS =  -g -O3 -pipe -W -Wall -DVERSION=\"0.4\" -DRISCPCEMU

RISCPCINCLUDE = -I/usr/include -I/usr/local/include -I/usr/include/readline -I. \
                -Ilibjtype -Icore -Ishell/debug -Imach/riscpc -Idynarec \
                -Idynarec/arch/x86

# Change these for different machine!
CFLAGS =        $(LARTCFLAGS)

INCLUDE =       $(LARTINCLUDE)

TARGET =        virtualart
#TARGET =       rohle
#TARGET =       riscpcemu

STRUCTOBJ =	$(DRX86)/structsupport.o

TEMPFILES =     $(DRX86)/genx86_tab.c $(DRX86)/rtasm_fns.c \
                $(DRX86)/rtasm_fns.h

TESTS =	divide simple armtest

LIBS = -lm -lreadline -lhistory -lncurses -lutil

.PHONY: clean cleaner package webpkg romdump lartrun

# virtualart rohle riscpcemu
all:	virtualart 

clean:
	rm -f *.o emulate $(TESTS)

cleaner:
	rm -f *.o *.d emulate $(TESTS) $(TEMPFILES) rtasm_fns.h

lartrun:	virtualart
	./virtualart "script lartup.txt"

virtualart: $(OBJ)
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

$(DRX86)/rtasm.h:       $(DRX86)/rtasm_fns.h

$(DRX86)/rtasm_fns.c:	$(DRX86)/mkintel.pl $(DRX86)/intel.dat
	$(DRX86)/mkintel.pl > rtasm_fns.c

$(DRX86)/rtasm_fns.h:	$(DRX86)/rtasm_fns.c $(DRX86)/rtasm_mkheader.pl
	$(DRX86)/rtasm_mkheader.pl > $(DRX86)/rtasm_fns.h

$(DRX86)/genx86_tab.c:	$(DRX86)/genx86tabsrc.dat $(DRX86)/mkx86tab.py
	$(DRX86)/mkx86tab.py

$(DRX86)/structsupport:	$(STRUCTOBJ)
	gcc -O2 $< -o $@

$(DRX86)/structsupport.inc:	$(DRX86)/structsupport
	$(DRX86)/structsupport > structsupport.inc

$(DRX86)/dynsupport.asm:	$(DRX86)/structsupport.inc

$(CORE)/asmalu.s:	$(CORE)/gen_dp.pl
	$(CORE)/gen_dp.pl > $@

$(CORE)/asmalu.o:	$(CORE)/asmalu.s
	$(AS) $< -o $@

%.o:	%.c
	$(CC) -c $(INCLUDE) $(CFLAGS) -c $< -o $@

%.o:	%.asm
	$(NASM) -f elf $< -o $@

%.d:	%.c
	$(CC) $(INCLUDE) -MM $< > $@

%.d:	%.asm
	$(NASM) -M $< > $@

asmalu.d:
	touch $@

include $(OBJ:.o=.d)
include $(STRUCTOBJ:.o=.d)

