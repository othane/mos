# build the bootstrap

.PHONY: all clean

PRJ = bootstrap
PRJ_FULL = $(PRJ).hex

include ../hal/hal.mk
export CPFLAGS += -Os
export CPFLAGS += -DNOHW_H

SRC = bootstrap_main.c

OBJS = $(SRC:.c=.o)

INCDIR += ../hal/
INCDIR += ../lib/
INC = $(patsubst %,-I%,$(INCDIR))

LDSCRIPT = ../hal/$(ARCH)/bootstrap.ld
LDFLAGS += -T$(LDSCRIPT)

all: $(PRJ_FULL)
	echo $(PRJ_FULL)

$(PRJ).elf: ../hal/libhal.o ../lib/lib.o $(OBJS) $(LDSCRIPT)
	$(CC) $(OBJS) ../hal/libhal.o ../lib/lib.o -Wl,-Map=$(PRJ).map $(LDFLAGS) -o $@

../lib/lib.o:
	make -C ../lib

../hal/libhal.o:
	make -C ../hal

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
	make -C ../hal clean
	make -C ../lib clean
	

