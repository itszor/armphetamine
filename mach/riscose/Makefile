include ../../Makefile.start

RISCOSE =	.

RISCOSEOBJS =	$(RISCOSE)/regaccess.o $(RISCOSE)/riscosemem.o

INCLUDE =	-I. -I$(APBASEDIR)/libjtype -I$(APBASEDIR)/core \
                -I$(APBASEDIR)/shell/debug -I$(APBASEDIR)/mach/riscose \
		-I$(APBASEDIR)/dynarec -I$(APBASEDIR)/dynarec/arch/x86 \
                -I/usr/include -I/usr/local/include -I/usr/include/readline

CFLAGS =	-g -O3 -pipe -W -Wall -DVERSION=\"$(VERSION)\" -DRISCOSE

OBJ =           $(LJTOBJS) $(COREOBJS) $(DROBJS) $(DRX86OBJS) $(RISCOSEOBJS)


TARGET =	libarmphetamine.a

$(RISCOSE)/libarmphetamine.a:	$(OBJ)
	$(AR) $(ARFLAGS) $@ $(OBJ)

include ../../Makefile.end
