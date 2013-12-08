# include this in the top level makefile to set up compilers & flags
SELF_DIR := $(dir $(lastword $(MAKEFILE_LIST)))

ifneq ($(MAKECMDGOALS),clean)

ifeq ($(ARCH),)
$(error Please set ARCH for the required target eg stm32f107x)
else
include $(SELF_DIR)/$(ARCH)/$(ARCH).mk
endif

endif
