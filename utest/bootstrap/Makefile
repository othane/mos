# build the bootstrap unit test

.PHONY: all clean

PRJ = bootstrap_utest
APP1 = app1
APP2 = app2
PRJ_FULL = $(PRJ).hex

include ../../hal/hal.mk

SRC1 = app1.c
SRC2 = app2.c

OBJS1 = $(SRC1:.c=.o)
OBJS2 = $(SRC2:.c=.o)

CPFLAGS += -DNOHW_H

INCDIR += ../../
INC = $(patsubst %,-I%,$(INCDIR))

LDSCRIPT1 = ./../../hal/$(ARCH)/utest_app1.ld
LDSCRIPT2 = ./../../hal/$(ARCH)/utest_app2.ld

all: $(PRJ_FULL)

$(PRJ_FULL): ../lib/libcrc.so ../../bootstrap/bootstrap.hex $(APP1).hex $(APP2).hex
	python ../../scripts/add_header.py $(APP1).hex > $(APP1)_tmp.hex
	python ../../scripts/add_header.py $(APP2).hex > $(APP2)_tmp.hex
	cp ../../bootstrap/bootstrap.hex $(PRJ_FULL)
	python ../../scripts/add_progs.py $(PRJ_FULL) $(APP1)_tmp.hex
	-rm -f $(APP1)_tmp.hex
	python ../../scripts/add_progs.py $(PRJ_FULL) $(APP2)_tmp.hex
	-rm -f $(APP2)_tmp.hex

../lib/libcrc.so:
	make -C ../../lib libcrc.so CC=gcc CFLAGS= CPFLAGS=

$(APP1).elf: ../../hal/libhal.o $(OBJS1) $(LDSCRIPT)
	$(CC) $(OBJS1) ../../hal/libhal.o -Wl,-Map=$(PRJ).map $(LDFLAGS) -T$(LDSCRIPT1) -o $@

$(APP2).elf: ../../hal/libhal.o $(OBJS2) $(LDSCRIPT)
	$(CC) $(OBJS2) ../../hal/libhal.o -Wl,-Map=$(PRJ).map $(LDFLAGS) -T$(LDSCRIPT2) -o $@

../../bootstrap/bootstrap.hex:
	make -C ../../bootstrap/

../../hal/libhal.o:
	make -C ../../hal

%.hex: %.elf
	$(BIN) $< $@

%.o : %.c
	$(CC) -c $(CPFLAGS) -Wa,-ahlms=$(<:.c=.lst) -I . $(INC) $< -o $@

clean:
	-rm -f $(OBJS1)
	-rm -f $(OBJS1:.o=.lst)
	-rm -f $(OBJS2)
	-rm -f $(OBJS2:.o=.lst)
	-rm -f $(APP1).lst
	-rm -f $(APP1).map
	-rm -f $(APP1).elf
	-rm -f $(APP1).hex
	-rm -f $(APP2).lst
	-rm -f $(APP2).map
	-rm -f $(APP2).elf
	-rm -f $(APP2).hex
	-rm -f $(PRJ).lst
	-rm -f $(PRJ).map
	-rm -f $(PRJ).elf
	-rm -f $(PRJ_FULL)
	make -C ../../lib clean
	make -C ../../hal clean
	make -C ../../bootstrap clean
	
