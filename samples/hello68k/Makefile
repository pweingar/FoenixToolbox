

# Define OS-dependent variables

ifeq ($(OS),Windows_NT)
	RM = del /F/Q
else
	RM = rm -f
endif

CPU=m68k
CFLAGS_FOR_UNIT=-DMODEL=11 -DCPU=5 --core 68010 --code-model large --data-model large
LDFLAGS_FOR_UNIT=a2560-ram.scm clib-68000-lc-ld.a --rtattr printf=medium --rom-code

CC=cc68k
AS=as68k
LD=ln68k
AR=nlib

INCLUDES=-I. -I./include
CFLAGS=$(INCLUDES) $(CFLAGS_FOR_UNIT) -l 
ASFLAGS=$(INCLUDES) --data-model large --code-model large -l

LDFLAGS=$(LDFLAGS_FOR_UNIT) --list-file toolbox.map

SRCS = hello.c # a2560-startup.s
OBJS = $(patsubst %.s,%.o,$(patsubst %.c,%.o,$(SRCS)))
OBJS4RM = $(subst /,\\,$(OBJS))
LIBS = 

.PHONY: clean

hello.s37: $(OBJS) $(LIBS)
	$(LD) $(LDFLAGS) --output-format s37 -o $@ $^

hello.pgz: $(OBJS) $(LIBS)
	$(LD) $(LDFLAGS) --output-format pgz -o $@ $^

# Build the object files from C
%.o: %.c
	$(CC) $(CFLAGS) -o $@ $^

# Build the object files from assembly
%.o: %.s
	$(AS) $(ASFLAGS) -o $@ $^

# Clean up after a build
clean:
	$(RM) $(OBJS4RM) hello.s37 *.lst *.map *.raw