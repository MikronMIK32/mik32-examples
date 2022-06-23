
BASE_PATH = ../..

MAKE_COMMON = $(BASE_PATH)/makefile.common

DIR_INCLUDE = . $(BASE_PATH)/shared $(BASE_PATH)/shared/include  
SHARED_FILES = $(BASE_PATH)/shared/runtime/crt0.S 
SHARED_FILES +=../common.c 
SHARED_FILES +=$(BASE_PATH)/shared/libs/uart_lib.c $(BASE_PATH)/shared/libs/xprintf.c
SHARED_FILES += ../i2c_irq_common/i2c_irq_common.c


DIR_SRC = .
DIR_OBJ = build
DIR_BIN = build

LD_SCRIPT = $(BASE_PATH)/shared/ldscripts/link.ld

# CC_OPTS  =  -march=rv32imc -mabi=ilp32 -DXLEN32
# CC_OPTS +=   -O0 -g -ffreestanding
# CC_OPTS +=   -nostartfiles -nostdlib -nodefaultlibs
CC_OPTS  =  -march=rv32imc -mabi=ilp32  
CC_OPTS +=   -O0 -g
CC_OPTS_A += $(CC_OPTS) -D_ASSEMBLER_
# Include common compiler options defined in $(MAKE_COMMON)
include $(MAKE_COMMON)


