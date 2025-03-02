/**
 * Support for the SuperIO chip.
 * 
 * Currently this is just used for initialization of the chip
 */

#ifndef __superio__
#define __superio__

#if HAS_SUPERIO

#include "superio_reg.h"

/*
 * Initialize the SuperIO registers
 */
extern void init_superio(void);

extern void unreset_lpc();

extern void configure_zones(); // This Init used to be done by the FPGA.

#endif
#endif
