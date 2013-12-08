.PHONY: clean stm32f107x

include hal.mk

stm32f107x:
	make -C stm32f107x
	ln -fs stm32f107x/libhal.o libhal.o
	ln -fs stm32f107x/hal.h hal.h

clean:
	-rm -f hal.h
	-rm -f libhal.o
	make -C stm32f107x clean
