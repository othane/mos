.PHONY: clean all sys gpio nvm spis crc bootstrap sched i2c

all: sys gpio nvm spis crc bootstrap sched i2c

sys:
	make -C sys

gpio:
	make -C gpio

nvm:
	make -C nvm

spis:
	make -C spis

uart:
	make -C tmr

crc:
	make -C crc EMBEDDED=1

bootstrap:
	make -C bootstrap

sched:
	make -C sched

i2c:
	make -C i2c

clean:
	make -C sys clean
	make -C gpio clean
	make -C nvm clean
	make -C spis clean
	make -C tmr clean
	make -C crc EMBEDDED=1 clean
	make -C bootstrap clean
	make -C sched clean
	make -C i2c clean
