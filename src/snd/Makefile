
UNIT := F256K2

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

	SRCS_FOR_UNIT=psg.c sid.c codec_c256.c
	CFLAGS_FOR_UNIT=-DMODEL=1 -DCPU=255 --target Foenix --code-model large --data-model large
else ifeq ($(UNIT),C256U_PLUS)
	CC=cc65816
	AS=as65816
	AR=nlib

	SRCS_FOR_UNIT=psg.c sid.c codec_c256.c
	CFLAGS_FOR_UNIT=-DMODEL=5 -DCPU=255 --target Foenix --code-model large --data-model large
else ifeq ($(UNIT),C256_FMX)
	CC=cc65816
	AS=as65816
	AR=nlib

	SRCS_FOR_UNIT=psg.c sid.c codec_c256.c
	CFLAGS_FOR_UNIT=-DMODEL=0 -DCPU=255 --target Foenix --code-model large --data-model large
else ifeq ($(UNIT),F256)
	CC=cc65816
	AS=as65816
	AR=nlib

	SRCS_FOR_UNIT=psg.c codec_c256.c sid.c
	CFLAGS_FOR_UNIT=-DMODEL=2 -DCPU=255 --code-model large --data-model large --target f256
else ifeq ($(UNIT),F256JR2)
	CC=cc65816
	AS=as65816
	AR=nlib

	SRCS_FOR_UNIT=psg.c codec_c256.c sid.c
	CFLAGS_FOR_UNIT=-DMODEL=35 -DCPU=255 --code-model large --data-model large --target f256
else ifeq ($(UNIT),F256K)
	CC=cc65816
	AS=as65816
	AR=nlib

	SRCS_FOR_UNIT=psg.c codec_c256.c sid.c
	CFLAGS_FOR_UNIT=-DMODEL=18 -DCPU=255 --code-model large --data-model large --target f256
else ifeq ($(UNIT),F256K2)
	CC=cc65816
	AS=as65816
	AR=nlib

	SRCS_FOR_UNIT=psg.c codec_c256.c sid.c yamaha.c
	CFLAGS_FOR_UNIT=-DMODEL=17 -DCPU=255 --code-model large --data-model large --target f256
else ifeq ($(UNIT),A2560K)
	CC=cc68k
	AS=as68k
	AR=nlib

	SRCS_FOR_UNIT=psg.c codec.c sid.c yamaha.c
	CFLAGS_FOR_UNIT=-DMODEL=11 -DCPU=5 --code-model large --data-model large --target Foenix
endif

INCLUDES=-I.. -I../include
CFLAGS=$(INCLUDES) $(CFLAGS_FOR_UNIT) -l
ASFLAGS=$(INCLUDES)

SRCS = $(SRCS_FOR_UNIT)
OBJS = $(patsubst %.c,%.o,$(SRCS))
OBJS4RM = $(subst /,\\,$(OBJS))

.PHONY: all clean

all: sound.a

# Build the devices library file for this model
sound.a: $(OBJS)
	@echo $(OBJS)
	$(AR) $@ $^

# Build the object files from C
%.o: %.c
	$(CC) $(CFLAGS) -o $@ $^

# Clean up after a build
clean:
	$(RM) $(OBJS4RM) sound.a *.lst
