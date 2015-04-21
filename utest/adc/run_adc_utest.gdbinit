target remote localhost:3333
set confirm off
delete

file adc_utest.elf
tbreak adc_handle_buffer
c

define reset
	mon reset halt
end

