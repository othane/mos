target remote localhost:3333
file ../../bootstrap/bootstrap.elf
mon reset halt
tbreak main
c

define reset
	mon reset halt
end

