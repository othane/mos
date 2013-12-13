.PHONY: clean stm32f107x stm32f373

include hal.mk

all:
	make -C $(ARCH) 
	ln -fs $(ARCH)/libhal.o libhal.o
	ln -fs $(ARCH)/hal.h hal.h

clean:
	-rm -f hal.h
	-rm -f libhal.o
	make -C $(ARCH) clean
