.PHONY: clean

include hal.mk
export HALCFG

LIBHAL?=libhal.o

all:
	make -C $(ARCH) 
	ln -fs $(ARCH)/$(LIBHAL) $(LIBHAL)
	ln -fs $(ARCH)/hal.h hal.h

clean:
	-rm -f hal.h
	-rm -f $(LIBHAL)
	make -C $(ARCH) clean
