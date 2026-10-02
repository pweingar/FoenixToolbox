
# Define OS-dependent variables

ifeq ($(OS),Windows_NT)
	RM = del /F/Q
else
	RM = rm -f
endif

CC=cc68k
AS=as68k
LD=ln68k
AR=nlib

SRCS = ps2_test.c a2560_startup.s
OBJS = $(patsubst %.s,%.o,$(patsubst %.c,%.o,$(SRCS)))
OBJS4RM = $(subst /,\\,$(OBJS))
CFLAGS = -l
LDFLAGS = --cstartup=a2560

ps2_test.hex: $(OBJS) $(LIBS)
	$(LD) $(LDFLAGS) --output-format intel-hex ps2_test.scm -o $@ $^

# Build the object files from C
%.o: %.c
	$(CC) $(CFLAGS) -o $@ $^

# Build the object files from assembly
%.o: %.s
	$(AS) $(ASFLAGS) -o $@ $^