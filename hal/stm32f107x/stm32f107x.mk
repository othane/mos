# setup compiler and flags for stm32107x build
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

MCU = cortex-m3
#FPU = -mfloat-abi=hard -mfpu=fpv4-sp-d16 -D__FPU_USED=1
FPU = 
DEFS = -DUSE_STDPERIPH_DRIVER -DSTM32F10X_CL -DRUN_FROM_FLASH=1
OPT ?= -O0 
MCFLAGS = -mthumb -mcpu=$(MCU) $(FPU)

export ASFLAGS  = $(MCFLAGS) $(OPT) -g -gdwarf-2 $(ADEFS)
CPFLAGS += $(MCFLAGS) $(OPT) -gdwarf-2 -Wall -Wno-attributes -fverbose-asm 
CPFLAGS += -ffunction-sections -fdata-sections $(DEFS)
export CPFLAGS
export CFLAGS += $(CPFLAGS)

export LDFLAGS  = $(MCFLAGS) -nostartfiles -Wl,--cref,--gc-sections,--no-warn-mismatch $(LIBDIR)

HINCDIR += ./CMSIS/CM3/CoreSupport/ \
	./CMSIS/CM3/DeviceSupport/ST/STM32F10x/ \
	./STM32F10x_StdPeriph_Driver/inc/ \
	./
export INCDIR = $(patsubst %,$(SELF_DIR)%,$(HINCDIR))


# openocd variables and targets
OPENOCD_PATH ?= /usr/local/share/openocd/
export OPENOCD_BIN = openocd
export OPENOCD_INTERFACE = $(OPENOCD_PATH)/scripts/interface/stlink-v2.cfg
export OPENOCD_TARGET = $(OPENOCD_PATH)/scripts/target/stm32f1x_stlink.cfg

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

.flash:
	$(OPENOCD_BIN) -f $(OPENOCD_INTERFACE) -f $(OPENOCD_TARGET) -c init $(OPENOCD_FLASH_CMDS)

.erase:
	$(OPENOCD_BIN) -f $(OPENOCD_INTERFACE) -f $(OPENOCD_TARGET) -c init $(OPENOCD_ERASE_CMDS)

.run:
	$(OPENOCD_BIN) -f $(OPENOCD_INTERFACE) -f $(OPENOCD_TARGET) -c init $(OPENOCD_RUN_CMDS)

.debug:
	$(OPENOCD_BIN) -f $(OPENOCD_INTERFACE) -f $(OPENOCD_TARGET) -c init $(OPENOCD_DEBUG_CMDS)

