include ../../Makefile.start

ROHLE =         .

ROHLEOBJS =     $(ROHLE)/fakesys.o $(ROHLE)/riscos.o $(ROHLE)/rohlemem.o

INCLUDE =       -I. -I$(APBASEDIR)/libjtype -I$(APBASEDIR)/core \
                -I$(APBASEDIR)/shell/debug -I$(APBASEDIR)/mach/rohle \
                -I$(APBASEDIR)/dynarec -I$(APBASEDIR)/dynarec/arch/x86 \
                -I/usr/include -I/usr/local/include -I/usr/include/readline 

CFLAGS =   -g -O3 -pipe -W -Wall -DVERSION=\"$(VERSION)\" -DROHLE

OBJ =           $(LJTOBJS) $(COREOBJS) $(DROBJS) $(DRX86OBJS) $(ROHLEOBJS) \
                $(DEBUGOBJ)

TARGET =        $(ROHLE)/rohle

$(TARGET): $(OBJ)
	$(CC) -o $@ $(OBJ) $(LDFLAGS) $(LIBS) 

include ../../Makefile.end
