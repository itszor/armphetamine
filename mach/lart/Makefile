include ../../Makefile.start

LART :=         $(APBASEDIR)/mach/lart

OBJBASE :=	$(APBASEDIR)/mach/lart

LARTOBJS :=	$(OBJBASE)/fifo.o $(OBJBASE)/intctrl.o $(OBJBASE)/mexreg.o \
                $(OBJBASE)/ostimer.o $(OBJBASE)/sapcm.o $(OBJBASE)/lartmem.o

INCLUDE :=	-I. -I$(APBASEDIR) -I$(APBASEDIR)/libjtype \
		-I/usr/include -I/usr/local/include -I/usr/include/readline

CFLAGS :=        -g -O0 -pipe -W -Wall -DVERSION=\"$(VERSION)\" -DEMULART

OBJ :=	        $(LJTOBJS) $(COREOBJS) $(DROBJS) $(DRX86OBJS) $(LARTOBJS) \
                $(DEBUGOBJS)

TARGET :=	$(LART)/virtualart

all:	$(TARGET)

lartrun:	$(TARGET)

$(TARGET): $(OBJ)
	$(CC) -o $@ $(OBJ) $(LDFLAGS) $(LIBS) 

include ../../Makefile.end
