
UNIT := F256K

# Define OS-dependent variables

ifeq ($(OS),Windows_NT)
	RM = del /F/Q
else
	RM = rm -f
endif

# Define model-specific variables, including tools, source files, compiler flags, etc.

ifeq ($(UNIT),C256U)
	CC=cc65816
	AS=as65816
	AR=nlib

	SRCS_FOR_UNIT=txt_c256.c txt_evid.c indicators_c256.c interrupts_c256.c timers_c256.c
	CFLAGS_FOR_UNIT=-DMODEL=1 -DCPU=255 --target Foenix --code-model large --data-model large
else ifeq ($(UNIT),C256U_PLUS)
	CC=cc65816
	AS=as65816
	AR=nlib

	SRCS_FOR_UNIT=txt_c256.c txt_evid.c indicators_c256.c interrupts_c256.c timers_c256.c
	CFLAGS_FOR_UNIT=-DMODEL=5 -DCPU=255 --target Foenix --code-model large --data-model large
else ifeq ($(UNIT),C256_FMX)
	CC=cc65816
	AS=as65816
	AR=nlib

	SRCS_FOR_UNIT=txt_c256.c txt_evid.c indicators_c256.c interrupts_c256.c timers_c256.c
	CFLAGS_FOR_UNIT=-DMODEL=0 -DCPU=255 --target Foenix --code-model large --data-model large
else ifeq ($(UNIT),F256)
	CC=cc65816
	AS=as65816
	AR=nlib

	SRCS_FOR_UNIT=txt_f256.c kbd_f256.c kbd_f256jr.c indicators_c256.c interrupts_f256.c sdc_f256.c iec.c # timers_c256.c
	CFLAGS_FOR_UNIT=-DMODEL=2 -DCPU=255 --code-model large --data-model large --target f256
else ifeq ($(UNIT),F256K)
	CC=cc65816
	AS=as65816
	AR=nlib

	SRCS_FOR_UNIT=txt_f256.c kbd_f256.c kbd_f256k.c indicators_c256.c interrupts_f256.c sdc_f256.c iec.c # timers_c256.c 
	CFLAGS_FOR_UNIT=-DMODEL=17 -DCPU=255 --code-model large --data-model large --target f256
endif

INCLUDES=-I.. -I../include
CFLAGS=$(INCLUDES) $(CFLAGS_FOR_UNIT) -l
ASFLAGS=$(INCLUDES) -l

SRCS = block.c channel.c console.c fsys.c sprites.c tiles.c txt_screen.c rtc.c uart.c $(SRCS_FOR_UNIT) # pata.c bitmap.c dma.c ps2.c sdc.c 
OBJS = $(patsubst %.c,%.o,$(SRCS))
OBJS4RM = $(subst /,\\,$(OBJS))

.PHONY: all clean

all: devices.a

# Build the devices library file for this model
devices.a: $(OBJS)
	$(AR) $@ $^

# Build the object files from C
%.o: %.c
	$(CC) $(CFLAGS) -o $@ $^

# Clean up after a build
clean:
	$(RM) $(OBJS4RM) devices.a *.lst
