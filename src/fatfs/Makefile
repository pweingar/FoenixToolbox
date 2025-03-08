
UNIT := F256K2

# Define OS-dependent variables

ifeq ($(OS),Windows_NT)
	RM = del /F/Q
else
	RM = rm -f
endif

# Define model-specific variables, including tools, source files, compiler flags, etc.

ifeq ($(UNIT),C256U)
	CPU=w65816
	SRCS_FOR_UNIT=c256_diskio.c
	CFLAGS_FOR_UNIT=-DMODEL=1 -DCPU=255 --target Foenix --code-model large --data-model large
	LDFLAGS_FOR_UNIT=C256/ld_lc_c256_u.scm clib-lc-ld.a
else ifeq ($(UNIT),C256U_PLUS)
	CPU=w65816
	SRCS_FOR_UNIT=c256_diskio.c
	CFLAGS_FOR_UNIT=-DMODEL=5 -DCPU=255 --target Foenix --code-model large --data-model large
	LDFLAGS_FOR_UNIT=C256/ld_lc_c256_fmx.scm clib-lc-ld.a --rtattr printf=medium
else ifeq ($(UNIT),C256_FMX)
	CPU=w65816
	SRCS_FOR_UNIT=c256_diskio.c
	CFLAGS_FOR_UNIT=-DMODEL=0 -DCPU=255 --target Foenix --code-model large --data-model large
	LDFLAGS_FOR_UNIT=C256/ld_lc_c256_fmx.scm clib-lc-ld.a 
else ifeq ($(UNIT),F256)
	CPU=w65816
	SRCS_FOR_UNIT=toolbox_bdev.c
	CFLAGS_FOR_UNIT=-DMODEL=2 -DCPU=255 --code-model large --data-model large --target f256

	ifeq ($(MEMORY),ROM)
		LDFLAGS_FOR_UNIT=C256/flash-f256.scm clib-lc-ld.a --rtattr printf=medium
	else
		LDFLAGS_FOR_UNIT=C256/ld_lc_f256.scm clib-lc-ld.a --rtattr printf=medium
	endif
else ifeq ($(UNIT),F256JR2)
	CPU=w65816
	SRCS_FOR_UNIT=toolbox_bdev.c
	CFLAGS_FOR_UNIT=-DMODEL=35 -DCPU=255 --code-model large --data-model large --target f256

	ifeq ($(MEMORY),ROM)
		LDFLAGS_FOR_UNIT=C256/flash-f256.scm clib-lc-ld.a --rtattr printf=medium
	else
		LDFLAGS_FOR_UNIT=C256/ld_lc_f256.scm clib-lc-ld.a --rtattr printf=medium
	endif
else ifeq ($(UNIT),F256K)
	CPU=w65816
	SRCS_FOR_UNIT=toolbox_bdev.c
	CFLAGS_FOR_UNIT=-DMODEL=18-DCPU=255 --code-model large --data-model large --target f256

	ifeq ($(MEMORY),ROM)
		LDFLAGS_FOR_UNIT=C256/flash-f256.scm clib-lc-ld.a --rtattr printf=medium
	else
		LDFLAGS_FOR_UNIT=C256/ld_lc_f256.scm clib-lc-ld.a --rtattr printf=medium
	endif
else ifeq ($(UNIT),F256K2)
	CPU=w65816
	SRCS_FOR_UNIT=toolbox_bdev.c
	CFLAGS_FOR_UNIT=-DMODEL=17 -DCPU=255 --code-model large --data-model large --target f256

	ifeq ($(MEMORY),ROM)
		LDFLAGS_FOR_UNIT=C256/flash-f256.scm clib-lc-ld.a --rtattr printf=medium
	else
		LDFLAGS_FOR_UNIT=C256/ld_lc_f256.scm clib-lc-ld.a --rtattr printf=medium
	endif
else ifeq ($(UNIT),A2560K)
	CPU=m68k

	SRCS_FOR_UNIT=toolbox_bdev.c
	CFLAGS_FOR_UNIT=-DMODEL=11 -DCPU=5 --code-model large --data-model large --target Foenix
endif

ifeq ($(CPU),w65816)
	CC=cc65816
	AS=as65816
	LD=ln65816
	AR=nlib
else ifeq ($(CPU),m68k)
	CC=cc68k
	AS=as68k
	LD=ln68k
	AR=nlib
endif

INCLUDES=-I.. -I../include
CFLAGS=$(INCLUDES) $(CFLAGS_FOR_UNIT) -l
ASFLAGS=$(INCLUDES)

SRCS = ff.c ffsystem.c ffunicode.c $(SRCS_FOR_UNIT)
OBJS = $(patsubst %.c,%.o,$(SRCS))
OBJS4RM = $(subst /,\\,$(OBJS))

.PHONY: all clean

all: fatfs.a

# Build the devices library file for this model
fatfs.a: $(OBJS)
	$(AR) $@ $^

# Build the object files from C
%.o: %.c
	$(CC) $(CFLAGS) -o $@ $^

# Clean up after a build
clean:
	$(RM) $(OBJS4RM) fatfs.a *.lst
