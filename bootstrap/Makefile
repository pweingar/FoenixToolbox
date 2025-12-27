#
# Makefile for the Foenix Toolbox Bootstrap
#

UNIT := FA2560K2
DATA := large
CODE := large
MEMORY := RAM

# Define OS-dependent variables

ifeq ($(OS),Windows_NT)
	RM = del /F/Q
else
	RM = rm -f
endif

ifeq ($(UNIT),FA2560K2)
	CPU=m68k
	ARC_SRCS = a2560-lowlevel.c a2560-startup.s extras.s jumptable.s
	ARC_SRCS_EXPAND = $(addprefix arch/fa2560k2/,$(ARC_SRCS))
	LDFLAGS_FOR_UNIT=arch/fa2560k2/a2560-ram.scm ../binary/toolbox_fa2560k2_lc_ld.a clib-68000-lc-ld.a --rtattr printf=medium --cstartup=a2560
else
	$(error, The UNIT $(UNIT) is not supported.)
endif

# Derive the source and object files from the machine-specific sources

CORE_SRCS = tb_firmware.c boot.c
SRCS = $(CORE_SRCS) $(ARC_SRCS_EXPAND)
OBJS = $(patsubst %.s,%.o,$(patsubst %.c,%.o,$(SRCS)))
INCLUDES=-I. -I./include -I../src/include -I../src/dev
CFLAGS=$(INCLUDES) $(CFLAGS_FOR_UNIT) --data-model $(DATA) --code-model $(CODE)
ASFLAGS=$(INCLUDES) --data-model $(DATA) --code-model $(CODE)
LDFLAGS=$(LDFLAGS_FOR_UNIT)

# Figure out which compiler tools to use based on the CPU

ifeq ($(CPU),w65816)
	CC=cc65816
	AS=as65816
	LD=ln65816
else ifeq ($(CPU),m68k)
	CC=cc68k
	AS=as68k
	LD=ln68k
else
	$(error CPU $(CPU) is not supported.)
endif

.PHONY: clean

tb_firmware.hex: $(OBJS) $(LIBS)
	$(LD) $(LDFLAGS) --output-format intel-hex -o $@ $^
	
# Build the object files from C

%.o: %.c
	$(CC) $(CFLAGS) -o $@ $^

# Build the object files from assembly

%.o: %.s
	$(AS) $(ASFLAGS) -o $@ $^

# Clean up after a build

clean:
	$(RM) $(OBJS) *.hex