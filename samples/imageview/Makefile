# VPATH=.:../../module/Calypsi-remote-debug/src
DEBUGGER=../module/Calypsi-remote-debug/src

UNIT := F256K
MEMORY := RAM

# Define OS-dependent variables

ifeq ($(OS),Windows_NT)
	RM = del /F/Q
else
	RM = rm -f
endif

# Define model-specific variables, including tools, source files, compiler flags, etc.

ifeq ($(UNIT),F256K)
	CPU=w65816
	C_SRCS_DEBUGGER=$(DEBUGGER)/agent.c $(DEBUGGER)/c256-uart.c $(DEBUGGER)/low_level_WDC65816.s
	SRCS_FOR_UNIT=
	CFLAGS_FOR_UNIT=-DMODEL=17 -DCPU=255 --code-model large --data-model large

	ifeq ($(MEMORY),ROM)
		LDFLAGS_FOR_UNIT=clib-lc-ld.a --rtattr printf=medium --target=f256 # C256/f256-flash.scm 
	else
		LDFLAGS_FOR_UNIT=f256-plain.scm clib-lc-ld.a --rtattr printf=medium --target=f256
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
ASFLAGS=$(INCLUDES) --data-model large --code-model large

ifeq ($(MEMORY),ROM)
	LDFLAGS=--rom-code $(LDFLAGS_FOR_UNIT) --list-file toolbox.map
else
	LDFLAGS=$(LDFLAGS_FOR_UNIT) --list-file toolbox.map
endif

SRCS = imageview.c header.s $(SRCS_FOR_UNIT) # $(C_SRCS_DEBUGGER)  ram-startup.s
OBJS = $(patsubst %.s,%.o,$(patsubst %.c,%.o,$(SRCS)))
OBJS4RM = $(subst /,\\,$(OBJS))
LIBS = ../../client/src/toolbox.a

.PHONY: clean

imageview.s37: $(OBJS) $(LIBS)
	$(LD) $(LDFLAGS) --output-format s37 -o $@ $^ 

# Build the object files from C
%.o: %.c
	$(CC) $(CFLAGS) -o $@ $^

# Build the object files from assembly
%.o: %.s
	$(AS) $(ASFLAGS) -o $@ $^

# Clean up after a build
clean:
	$(RM) $(OBJS4RM) *.s37 *.o *.a *.lst

