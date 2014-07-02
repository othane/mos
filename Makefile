LIBMOS = libmos.o
LIBHAL = hal/libhal.o

.PHONY: clean all utest $(LIBHAL) $(LIBMOS)

include hal/hal.mk

OBJS =  $(LIBHAL) \
		lib/lib.o \
		sched/libsched.o

all: $(LIBMOS) utest
	echo "libmos built"

$(LIBMOS): $(OBJS)
	$(LD) -r $(OBJS) -o $@

$(LIBHAL):
	make -C hal

lib/lib.o:
	make -C lib lib.o

sched/libsched.o:
	make -C sched

utest:
	make -C utest

clean:
	make -C hal clean
	make -C lib clean
	make -C sched clean
	make -C utest clean
	-rm -f $(LIBMOS)

