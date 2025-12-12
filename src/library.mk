###
### Makefile to build the Toolbox as a static link library for inclusion into other programs
###
### This makefile is intended to be used with the Calypsi CC compiler and builds a library
### suitable for a program built by the same compiler.
###
### Parameters:
### - UNIT = the name of the target computer (F256K2X, FA2560K2, etc.)
### - DATA = Calypsi data model size: large, small, compact, etc.
### - CODE = Calyspi code model size: large, small, compact, etc.
###

# Define default parameters. The user can over-ride these with arguments to MAKE.

UNIT := FA2560K2
DATA := large
CODE := large

# Define OS-dependent variables

ifeq ($(OS),Windows_NT)
	RM = del /F/Q
else
	RM = rm -f
endif

# Set machine specific parameters
# Currently: just do it for the FA2560K2
# TODO: actually set the parameters based on the UNIT, supporting other models

ifeq ($(UNIT),FA2560K2)
	CPU=m68k
	SRCS_FOR_UNIT = FA2560K2/io_stubs.c
	DEV_FOR_UNIT = bitmap_fa2560k2.c indicators_fa2560k2.c interrupts_fa2560k2.c txt_fa2560k2.c sdc_spi.c kbd_f256.c kbd_f256k.c serial_common.c
	SND_FOR_UNIT = psg.c codec.c sid.c yamaha.c
	FAT_FOR_UNIT = toolbox_bdev.c
	CFLAGS_FOR_UNIT=-DMODEL=24 -DCPU=5 --core 68000

else
	$(error, The UNIT $(UNIT) is not supported.)
endif

# Figure out the source files for the subsystems (devices, sound, file system)

DEV_SRC = block.c channel.c console.c fsys.c txt_screen.c rtc.c uart.c $(DEV_FOR_UNIT)
FAT_SRC = ff.c ffsystem.c ffunicode.c $(FAT_FOR_UNIT)
SND_SRC = $(SND_FOR_UNIT)
SUB_SRCS = $(addprefix dev/,$(DEV_SRC)) $(addprefix fatfs/,$(FAT_SRC)) $(addprefix snd/,$(SND_SRC))

# Figure out all the source and object files

SRCS = init.c log.c memory.c proc.c ring_buffer.c simpleio.c sys_general.c timers.c utilities.c $(SUB_SRCS) $(SRCS_FOR_UNIT)
OBJS = $(patsubst %.s,%.o,$(patsubst %.c,%.o,$(SRCS)))
OBJS4RM = $(subst /,\\,$(OBJS))

# Figure out the flags for the C compiler and the assembler

INCLUDES=-I. -I./include
CFLAGS=$(INCLUDES) $(CFLAGS_FOR_UNIT) --data-model $(DATA) --code-model $(CODE)
ASFLAGS=$(INCLUDES) --data-model $(DATA) --code-model $(CODE)

# Figure out which compiler tools to use based on the CPU

ifeq ($(CPU),w65816)
	CC=cc65816
	AS=as65816
	AR=nlib
else ifeq ($(CPU),m68k)
	CC=cc68k
	AS=as68k
	AR=nlib
else
	$(error CPU $(CPU) is not supported.)
endif

.PHONY: clean

toolbox.a: $(OBJS)
	$(AR) $@ $^

# Build the object files from C
%.o: %.c
	$(CC) $(CFLAGS) -o $@ $^

# Build the object files from assembly
%.o: %.s
	$(AS) $(ASFLAGS) -o $@ $^

# Clean up after a build
clean:
	$(RM) $(OBJS4RM) 
