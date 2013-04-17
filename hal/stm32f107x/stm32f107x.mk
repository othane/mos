# setup compiler and flags for stm32107x build

CROSS_COMPILE = arm-none-eabi-
CC = $(CROSS_COMPILE)gcc
AS = $(CROSS_COMPILE)gcc -x assembler-with-cpp
AR = $(CROSS_COMPILE)ar
LD = $(CROSS_COMPILE)ld
OD   = $(CROSS_COMPILE)objdump
BIN  = $(CROSS_COMPILE)objcopy -O ihex
SIZE = $(CROSS_COMPILE)size

MCU = cortex-m3
FPU = -mfloat-abi=hard -mfpu=fpv4-sp-d16 -D__FPU_USED=1
DEFS = -DUSE_STDPERIPH_DRIVER -DSTM32F10X_CL
OPT ?= -O0 
MCFLAGS = -mthumb -mcpu=$(MCU) $(FPU)

ASFLAGS  = $(MCFLAGS) $(OPT) -g -gdwarf-2 $(ADEFS)
CPFLAGS  = $(MCFLAGS) $(OPT) -gdwarf-2 -Wall -Wno-attributes -fverbose-asm 
CPFLAGS += -ffunction-sections -fdata-sections $(DEFS)

LDFLAGS  = $(MCFLAGS) -nostartfiles -Wl,--cref,--gc-sections,--no-warn-mismatch $(LIBDIR)


# openocd variables and targets
OPENOCD_PATH ?= /usr/local/share/openocd/
OPENOCD_BIN = openocd
OPENOCD_INTERFACE = $(OPENOCD_PATH)/scripts/interface/stlink-v2.cfg
OPENOCD_TARGET = $(OPENOCD_PATH)/scripts/target/stm32f1x_stlink.cfg

OPENOCD_FLASH_CMDS = ''
OPENOCD_FLASH_CMDS += -c 'reset halt'
OPENOCD_FLASH_CMDS += -c 'sleep 10' 
OPENOCD_FLASH_CMDS += -c 'stm32f1x unlock 0'
OPENOCD_FLASH_CMDS += -c 'flash write_image erase $(PRJ).hex 0 ihex'
OPENOCD_FLASH_CMDS += -c shutdown

