include ../../Makefile.start

RISCPC =        $(APBASEDIR)/mach/riscpc

RISCPCOBJS =    $(RISCPC)/iomd.o $(RISCPC)/keyboard.o $(RISCPC)/mouse.o \
                $(RISCPC)/vidc20.o $(RISCPC)/riscpcmem.o

CFLAGS =  -g -O3 -pipe -W -Wall -DVERSION=\"$(VERSION)\" -DRISCPCEMU

INCLUDE =       -I. -I$(APBASEDIR)/libjtype -I$(APBASEDIR)/core \
                -I$(APBASEDIR)/shell/debug -I$(APBASEDIR)/mach/riscpc \
                -I$(APBASEDIR)/dynarec -I$(APBASEDIR)/dynarec/arch/x86 \
                -I/usr/include -I/usr/local/include \
                -I/usr/include/readline 

OBJ =   $(LJTOBJS) $(COREOBJS) $(DROBJS) $(DRX86OBJS) $(RISCPCOBJS) \
        $(DEBUGOBJ)

LIBS += -lSDL

TARGET	=	riscpcemu

$(TARGET): $(OBJ)
	$(CC) -o $@ $(OBJ) $(LDFLAGS) $(LIBS) 

include ../../Makefile.end
