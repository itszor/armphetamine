include ../../Makefile.start

LART =          .

OBJBASE =	$(APBASEDIR)/mach/lart

LARTOBJS =      $(OBJBASE)/fifo.o $(OBJBASE)/intctrl.o $(OBJBASE)/mexreg.o \
                $(OBJBASE)/ostimer.o $(OBJBASE)/sapcm.o $(OBJBASE)/lartmem.o

INCLUDE =   -I. -I$(APBASEDIR)/libjtype -I$(APBASEDIR)/core \
                -I$(APBASEDIR)/shell/debug -I$(APBASEDIR)/mach/lart \
                -I$(APBASEDIR)/dynarec -I$(APBASEDIR)/dynarec/arch/x86 \
                -I/usr/include -I/usr/local/include \
                -I/usr/include/readline

CFLAGS =        -g -O3 -pipe -W -Wall -DVERSION=\"$(VERSION)\" -DEMULART -DRECOMPILE

OBJ =	        $(LJTOBJS) $(COREOBJS) $(DROBJS) $(DRX86OBJS) $(LARTOBJS) \
                $(DEBUGOBJS)

TARGET =	$(LART)/virtualart

$(TARGET): $(OBJ)
	$(CC) -o $@ $(OBJ) $(LDFLAGS) $(LIBS) 

include ../../Makefile.end
