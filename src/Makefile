# VPATH=.:../../module/Calypsi-remote-debug/src
DEBUGGER=../module/Calypsi-remote-debug/src

UNIT := F256K2
MEMORY := ROM

# Define OS-dependent variables

ifeq ($(OS),Windows_NT)
	RM = del /F/Q
else
	RM = rm -f
endif

# Define model-specific variables, including tools, source files, compiler flags, etc.

ifeq ($(UNIT),C256U)
	CPU=w65816
	C_SRCS_DEBUGGER=$(DEBUGGER)/agent.c $(DEBUGGER)/c256-uart.c $(DEBUGGER)/low_level_WDC65816.s
	SRCS_FOR_UNIT=
	CFLAGS_FOR_UNIT=-DMODEL=1 -DCPU=255 --code-model large --data-model large
	LDFLAGS_FOR_UNIT=C256/ld_lc_c256_u.scm clib-lc-ld.a
else ifeq ($(UNIT),C256U_PLUS)
	CPU=w65816
	C_SRCS_DEBUGGER=$(DEBUGGER)/agent.c $(DEBUGGER)/c256-uart.c $(DEBUGGER)/low_level_WDC65816.s
	SRCS_FOR_UNIT=C256/jumptable.s C256/io_stubs.c C256/extras.s
	CFLAGS_FOR_UNIT=-DMODEL=5 -DCPU=255 --code-model large --data-model large

	ifeq ($(MEMORY),ROM)
		LDFLAGS_FOR_UNIT=C256/flash-u-plus.scm clib-lc-ld.a --rtattr printf=medium
	else
		LDFLAGS_FOR_UNIT=C256/ld_lc_c256_fmx.scm clib-lc-ld.a --rtattr printf=medium
	endif
else ifeq ($(UNIT),C256_FMX)
	CPU=w65816
	C_SRCS_DEBUGGER=$(DEBUGGER)/agent.c $(DEBUGGER)/c256-uart.c $(DEBUGGER)/low_level_WDC65816.s
	SRCS_FOR_UNIT=
	CFLAGS_FOR_UNIT=-DMODEL=0 -DCPU=255 --code-model large --data-model large
	LDFLAGS_FOR_UNIT=C256/ld_lc_c256_fmx.scm clib-lc-ld.a 
else ifeq ($(UNIT),F256)
	CPU=w65816
	C_SRCS_DEBUGGER=$(DEBUGGER)/agent.c $(DEBUGGER)/c256-uart.c $(DEBUGGER)/low_level_WDC65816.s
	SRCS_FOR_UNIT=cartridge.c C256/jumptable.s C256/io_stubs.c C256/extras.s C256/iecll.s C256/interrupts.s C256/f256-cstartup.s 
	CFLAGS_FOR_UNIT=-DMODEL=2 -DCPU=255 --code-model large --data-model large --target f256

	ifeq ($(MEMORY),ROM)
		LDFLAGS_FOR_UNIT=C256/f256-flash.scm clib-lc-ld.a --rtattr printf=medium --cstartup=f256
	else
		LDFLAGS_FOR_UNIT=C256/f256-ld_lc.scm clib-lc-ld.a --rtattr printf=medium --cstartup=f256
	endif
else ifeq ($(UNIT),F256JR2)
	CPU=w65816
	C_SRCS_DEBUGGER=$(DEBUGGER)/agent.c $(DEBUGGER)/c256-uart.c $(DEBUGGER)/low_level_WDC65816.s
	SRCS_FOR_UNIT=cartridge.c C256/jumptable.s C256/io_stubs.c C256/extras.s C256/iecll.s C256/interrupts.s C256/f256-cstartup.s 
	CFLAGS_FOR_UNIT=-DMODEL=35 -DCPU=255 --code-model large --data-model large --target f256

	ifeq ($(MEMORY),ROM)
		LDFLAGS_FOR_UNIT=C256/f256k2-flash.scm clib-lc-ld.a --rtattr printf=medium --cstartup=f256
	else
		LDFLAGS_FOR_UNIT=C256/f256-ld_lc.scm clib-lc-ld.a --rtattr printf=medium --cstartup=f256
	endif
else ifeq ($(UNIT),F256K)
	CPU=w65816
	C_SRCS_DEBUGGER=$(DEBUGGER)/agent.c $(DEBUGGER)/c256-uart.c $(DEBUGGER)/low_level_WDC65816.s
	SRCS_FOR_UNIT=cartridge.c C256/jumptable.s C256/io_stubs.c C256/extras.s C256/iecll.s C256/interrupts.s C256/f256-cstartup.s 
	CFLAGS_FOR_UNIT=-DMODEL=18 -DCPU=255 --code-model large --data-model large --target f256

	ifeq ($(MEMORY),ROM)
		LDFLAGS_FOR_UNIT=C256/f256-flash.scm clib-lc-ld.a --rtattr printf=medium --cstartup=f256
	else
		LDFLAGS_FOR_UNIT=C256/f256-ld_lc.scm clib-lc-ld.a --rtattr printf=medium --cstartup=f256
	endif
else ifeq ($(UNIT),F256K2)
	CPU=w65816
	C_SRCS_DEBUGGER=$(DEBUGGER)/agent.c $(DEBUGGER)/c256-uart.c $(DEBUGGER)/low_level_WDC65816.s
	SRCS_FOR_UNIT=cartridge.c C256/jumptable.s C256/io_stubs.c C256/extras.s C256/iecll.s C256/interrupts.s C256/f256-cstartup.s 
	CFLAGS_FOR_UNIT=-DMODEL=17 -DCPU=255 --code-model large --data-model large --target f256

	ifeq ($(MEMORY),ROM)
		LDFLAGS_FOR_UNIT=C256/f256k2-flash.scm clib-lc-ld.a --rtattr printf=medium --cstartup=f256
	else
		LDFLAGS_FOR_UNIT=C256/f256-ld_lc.scm clib-lc-ld.a --rtattr printf=medium --cstartup=f256
	endif
endif

ifeq ($(CPU),w65816)
	CC=cc65816
	AS=as65816
	LD=ln65816
	AR=nlib
endif

INCLUDES=-I. -I./include
CFLAGS=$(INCLUDES) $(CFLAGS_FOR_UNIT) -l # -l -D_CALYPSI_MCP_DEBUGGER
ASFLAGS=$(INCLUDES) --data-model large --code-model large -l

ifeq ($(MEMORY),ROM)
	LDFLAGS=--rom-code $(LDFLAGS_FOR_UNIT) --list-file toolbox.map
else
	LDFLAGS=$(LDFLAGS_FOR_UNIT) --list-file toolbox.map
endif

SRCS = toolbox.c log.c boot.c memory.c proc.c ring_buffer.c simpleio.c sys_general.c timers.c utilities.c $(SRCS_FOR_UNIT) # $(C_SRCS_DEBUGGER)
OBJS = $(patsubst %.s,%.o,$(patsubst %.c,%.o,$(SRCS)))
OBJS4RM = $(subst /,\\,$(OBJS))
LIBS = dev/devices.a snd/sound.a fatfs/fatfs.a

.PHONY: clean

toolbox.s37: $(OBJS) $(LIBS)
	$(LD) $(LDFLAGS) --output-format s37 -o $@ $^

toolbox.raw: $(OBJS) $(LIBS)
	$(LD) $(LDFLAGS) --raw-multiple-memories --output-format raw -o $@ $^

dev/devices.a:
	$(MAKE) UNIT=$(UNIT) --directory=dev

snd/sound.a:
	$(MAKE) UNIT=$(UNIT) --directory=snd

fatfs/fatfs.a:
	$(MAKE) UNIT=$(UNIT) --directory=fatfs

# Build the object files from C
%.o: %.c
	$(CC) $(CFLAGS) -o $@ $^

# Build the object files from assembly
%.o: %.s
	$(AS) $(ASFLAGS) -o $@ $^

# Clean up after a build
clean:
	$(RM) $(OBJS4RM) toolbox.s37 *.lst *.map *.raw
	$(MAKE) --directory=dev clean
	$(MAKE) --directory=snd clean
	$(MAKE) --directory=fatfs clean