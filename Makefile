TARGET = main

# Default target chip.
#MCU ?= STM32F031K6
MCU ?= STM32F051K8
# TODO: Support F303
#MCU ?= STM32F303K8

ifeq ($(MCU), STM32F031K6)
	MCU_FILES  = STM32F031K6T6
	ST_MCU_DEF = STM32F031x6
	MCU_CLASS  = F0
else ifeq ($(MCU), STM32F051K8)
	MCU_FILES  = STM32F051K8T6
	ST_MCU_DEF = STM32F051x8
	MCU_CLASS  = F0
else ifeq ($(MCU), STM32F303K8)
	MCU_FILES  = STM32F303K8T6
	ST_MCU_DEF = STM32F303x8
	MCU_CLASS  = F3
endif

# Define the linker script location and chip architecture.
LD_SCRIPT = $(MCU_FILES).ld
ifeq ($(MCU_CLASS), F0)
	MCU_SPEC = cortex-m0
else ifeq ($(MCU_CLASS), F3)
	MCU_SPEC = cortex-m4
endif

# Toolchain definitions (ARM bare metal defaults)
TOOLCHAIN = /usr
CC = $(TOOLCHAIN)/bin/arm-none-eabi-gcc
AS = $(TOOLCHAIN)/bin/arm-none-eabi-as
LD = $(TOOLCHAIN)/bin/arm-none-eabi-ld
OC = $(TOOLCHAIN)/bin/arm-none-eabi-objcopy
OD = $(TOOLCHAIN)/bin/arm-none-eabi-objdump
OS = $(TOOLCHAIN)/bin/arm-none-eabi-size

# Assembly directives.
ASFLAGS += -c
ASFLAGS += -O0
ASFLAGS += -mcpu=$(MCU_SPEC)
ASFLAGS += -mthumb
ASFLAGS += -Wall
# (Set error messages to appear on a single line.)
ASFLAGS += -fmessage-length=0
ASFLAGS += -DVVC_$(MCU_CLASS)

# C compilation directives
CFLAGS += -mcpu=$(MCU_SPEC)
CFLAGS += -mthumb
CFLAGS += -Wall
CFLAGS += -g
# (Set error messages to appear on a single line.)
CFLAGS += -fmessage-length=0
# (Set system to ignore semihosted junk)
CFLAGS += --specs=nosys.specs
CFLAGS += -D$(ST_MCU_DEF)
CFLAGS += -DVVC_$(MCU_CLASS)

# Linker directives.
LSCRIPT = ./ld/$(LD_SCRIPT)
LFLAGS += -mcpu=$(MCU_SPEC)
LFLAGS += -mthumb
LFLAGS += -Wall
LFLAGS += --specs=nosys.specs
LFLAGS += -nostdlib
LFLAGS += -lgcc
LFLAGS += -lc
LFLAGS += -T$(LSCRIPT)

AS_SRC   =  ./boot_s/$(MCU_FILES)_boot.S
AS_SRC   += ./vector_tables/$(MCU_FILES)_vt.S
AS_SRC   += ./src/util.S
C_SRC    =  ./src/main.c
C_SRC    += ./src/util_c.c
C_SRC    += ./src/interrupts_c.c
C_SRC    += ./src/peripherals.c
C_SRC    += ./src/sspi.c

INCLUDE  =  -I./
INCLUDE  += -I./src
INCLUDE  += -I./device_headers

OBJS  = $(AS_SRC:.S=.o)
OBJS += $(C_SRC:.c=.o)

.PHONY: all
all: $(TARGET).bin

%.o: %.S
	$(CC) -x assembler-with-cpp $(ASFLAGS) $< -o $@

%.o: %.c
	$(CC) -c $(CFLAGS) $(INCLUDE) $< -o $@

$(TARGET).elf: $(OBJS)
	$(CC) $^ $(LFLAGS) -o $@

$(TARGET).bin: $(TARGET).elf
	$(OC) -S -O binary $< $@
	$(OS) $<

.PHONY: clean
clean:
	rm -f $(OBJS)
	rm -f $(TARGET).elf
	rm -f $(TARGET).bin
