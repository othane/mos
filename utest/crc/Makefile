# build the crc unit test

.PHONY: all clean

PRJ = crc_utest
ifdef EMBEDDED
PRJ_FULL = $(PRJ).hex
else
PRJ_FULL = $(PRJ)
endif

ifdef EMBEDDED
include ../../hal/hal.mk
endif

SRC = crc_utest.c 
OBJS = $(SRC:.c=.o)

ifdef EMBEDDED
CPFLAGS += -DEMBEDDED -DNOHW_H
else
export CPFLAGS += -DPRINT_RESULT -g
endif

INCDIR += ./../../lib
INCDIR += ../../hal/
INC = $(patsubst %,-I%,$(INCDIR))

ifdef EMBEDDED
LDSCRIPT = ./../../hal/$(ARCH)/utest.ld
LDFLAGS += -T$(LDSCRIPT)
endif

all: $(PRJ_FULL)
	echo $(PRJ_FULL)

$(PRJ): ../../lib/libcrc.so $(OBJS)
	$(CC) $(OBJS) -L../../lib -lcrc -o $@

$(PRJ).elf: ../../hal/libhal.o ../../lib/lib.o $(OBJS) $(LDSCRIPT)
	$(CC) $(OBJS) ../../hal/libhal.o ../../lib/lib.o -Wl,-Map=$(PRJ).map $(LDFLAGS) -o $@

../../lib/libcrc.so:
	make -C ../../lib/ libcrc.so

../../lib/lib.o:
	make -C ../../lib

../../hal/libhal.o:
	make -C ../../hal

%.hex: %.elf
	$(BIN) $< $@

%.o : %.c
	$(CC) -c $(CPFLAGS) -Wa,-ahlms=$(<:.c=.lst) -I . $(INC) $< -o $@

clean:
	-rm -f $(OBJS)
	-rm -f $(OBJS:.o=.lst)
	-rm -f $(PRJ).lst
	-rm -f $(PRJ).map
	-rm -f $(PRJ).elf
	-rm -f $(PRJ_FULL)
	make -C ../../hal clean
	make -C ../../lib clean
	
