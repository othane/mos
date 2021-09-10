file adc_utest.elf
target remote localhost:3333
mon reset halt
tbreak main
c

define reset
	mon reset halt
end

