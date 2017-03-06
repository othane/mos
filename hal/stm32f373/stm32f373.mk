# setup compiler and flags for stm32f373 build
SELF_DIR := $(dir $(lastword $(MAKEFILE_LIST)))

CROSS_COMPILE ?= arm-none-eabi-
export CC = $(CROSS_COMPILE)gcc
export AS = $(CROSS_COMPILE)gcc -x assembler-with-cpp
export AR = $(CROSS_COMPILE)ar
export LD = $(CROSS_COMPILE)ld
export OD   = $(CROSS_COMPILE)objdump
export BIN  = $(CROSS_COMPILE)objcopy -O ihex
export SIZE = $(CROSS_COMPILE)size
export GDB = $(CROSS_COMPILE)gdb

MCU = cortex-m4
FPU = -mfloat-abi=hard -mfpu=fpv4-sp-d16 -D__FPU_USED=1 -D__FPU_PRESENT=1 -DARM_MATH_CM4
DEFS = -DUSE_STDPERIPH_DRIVER -DSTM32F37X -DRUN_FROM_FLASH=1 -DHSE_VALUE=8000000
OPT ?= -O0 
MCFLAGS = -mthumb -mcpu=$(MCU) $(FPU)

export ASFLAGS  = $(MCFLAGS) $(OPT) -g -gdwarf-2 $(ADEFS)
CPFLAGS += $(MCFLAGS) $(OPT) -gdwarf-2 -Wall -Wno-attributes -fverbose-asm 
CPFLAGS += -ffunction-sections -fdata-sections $(DEFS)
export CPFLAGS
export CFLAGS += $(CPFLAGS)

export LDFLAGS  = $(MCFLAGS) -nostartfiles -Wl,--cref,--gc-sections,--no-warn-mismatch $(LIBDIR)

HINCDIR += ./STM32F37x_DSP_StdPeriph_Lib_V1.0.0/Libraries/CMSIS/Include/ \
	./STM32F37x_DSP_StdPeriph_Lib_V1.0.0/Libraries/CMSIS/Device/ST/STM32F37x/Include/ \
	./STM32F37x_DSP_StdPeriph_Lib_V1.0.0/Libraries/STM32F37x_StdPeriph_Driver/inc/ \
	./
export INCDIR = $(patsubst %,$(SELF_DIR)%,$(HINCDIR))


# openocd variables and targets
OPENOCD_PATH ?= /usr/local/share/openocd/
export OPENOCD_BIN = openocd
export OPENOCD_INTERFACE ?= $(OPENOCD_PATH)/scripts/interface/stlink-v2.cfg
export OPENOCD_TARGET = $(OPENOCD_PATH)/scripts/target/stm32f3x.cfg

OPENOCD_FLASH_CMDS = ''
OPENOCD_FLASH_CMDS += -c 'reset halt'
OPENOCD_FLASH_CMDS += -c 'sleep 10'
OPENOCD_FLASH_CMDS += -c 'stm32f1x unlock 0'
OPENOCD_FLASH_CMDS += -c 'flash write_image erase $(PRJ_FULL) 0 ihex'
OPENOCD_FLASH_CMDS += -c shutdown
export OPENOCD_FLASH_CMDS

OPENOCD_ERASE_CMDS = ''
OPENOCD_ERASE_CMDS += -c 'reset halt'
OPENOCD_ERASE_CMDS += -c 'sleep 10'
OPENOCD_ERASE_CMDS += -c 'sleep 10'
OPENOCD_FLASH_CMDS += -c 'stm32f1x unlock 0'
OPENOCD_ERASE_CMDS += -c 'stm32f1x mass_erase 0'
OPENOCD_ERASE_CMDS += -c shutdown
export OPENOCD_ERASE_CMDS

OPENOCD_RUN_CMDS = ''
OPENOCD_RUN_CMDS += -c 'reset halt'
OPENOCD_RUN_CMDS += -c 'sleep 10'
OPENOCD_RUN_CMDS += -c 'reset run'
OPENOCD_RUN_CMDS += -c 'sleep 10' 
OPENOCD_RUN_CMDS += -c shutdown
export OPENOCD_RUN_CMDS

OPENOCD_DEBUG_CMDS = ''
OPENOCD_DEBUG_CMDS += -c 'halt'
OPENOCD_DEBUG_CMDS += -c 'sleep 10'

OPENOCD_VERIFY_CMDS = ''
OPENOCD_VERIFY_CMDS += -c 'reset halt'
OPENOCD_VERIFY_CMDS += -c 'sleep 10'
OPENOCD_VERIFY_CMDS += -c 'verify_image $(PRJ_FULL) 0 ihex'
OPENOCD_VERIFY_CMDS += -c shutdown

OPENOCD_DUMP_CMDS = ''
OPENOCD_DUMP_CMDS += -c 'reset halt'
OPENOCD_DUMP_CMDS += -c 'sleep 10'
OPENOCD_DUMP_CMDS += -c 'dump_image dump.bin 0 0x40000'
OPENOCD_DUMP_CMDS += -c shutdown

.flash:
	$(OPENOCD_BIN) -f $(OPENOCD_INTERFACE) -f $(OPENOCD_TARGET) -c init $(OPENOCD_FLASH_CMDS)

.erase:
	$(OPENOCD_BIN) -f $(OPENOCD_INTERFACE) -f $(OPENOCD_TARGET) -c init $(OPENOCD_ERASE_CMDS)

.run:
	$(OPENOCD_BIN) -f $(OPENOCD_INTERFACE) -f $(OPENOCD_TARGET) -c init $(OPENOCD_RUN_CMDS)

.debug:
	$(OPENOCD_BIN) -f $(OPENOCD_INTERFACE) -f $(OPENOCD_TARGET) -c init $(OPENOCD_DEBUG_CMDS)

.verify:
	$(OPENOCD_BIN) -f $(OPENOCD_INTERFACE) -f $(OPENOCD_TARGET) -c init $(OPENOCD_VERIFY_CMDS)

.dump:
	$(OPENOCD_BIN) -f $(OPENOCD_INTERFACE) -f $(OPENOCD_TARGET) -c init $(OPENOCD_DUMP_CMDS)
	$(BIN) -I binary dump.bin dump.hex

