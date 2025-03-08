/*
 * Macros needed to support system calls
 */

#ifndef _sys_macros_h_
#define _sys_macros_h_

/*
 * Define the machine-specific system call function prefix
 */

#ifdef __CALYPSI_CORE_65816__
//
// System calls on the 65816 pass parameters primarily on stack using the Calypsi
// simple call convention and save/restore the direct page and data bank registers.
//
#define SYSTEMCALL __attribute__((simple_call)) __attribute__((saveds))

#elif __CALYPSI_TARGET_68000__
//
// System calls on the 65816 pass parameters primarily on stack using the Calypsi
// simple call convention and save/restore the direct page and data bank registers.
//
#define SYSTEMCALL 

#else
#define SYSTEMCALL
#endif

#endif