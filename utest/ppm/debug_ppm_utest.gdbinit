target remote localhost:3333
file ppm_utest.elf
mon reset halt
tbreak main
c

define reset
	mon reset halt
end

