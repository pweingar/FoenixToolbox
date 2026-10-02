/*
 * Code to support the timers
 */

#ifndef __TIMERS_H
#define __TIMERS_H

#include "sys_macros.h"

/*
 * Initialize the timers and their interrupts
 */
extern void timers_init();

/*
 * Return the number of jiffies (1/60 of a second) since last reset time
 */
extern SYSTEMCALL long timers_jiffies();

/**
 * Set the multiplier used by the system clock based timers based on the CPU clock speed.
 */
extern void timers_set_sys_timers();

/**
 * Reset the microsecond timer to 0
 */
extern void timers_reset_usec();

/**
 * Wait for N microseconds (approximately)
 * 
 * NOTE: this will make use of TIMER2 on the A2560 models, which is based on the system clock
 * 
 * @param n the number of microseconds to wait
 */
extern void timers_wait_usec(unsigned int n);


/**
 * Return the current multiplier for the micro-second timer
 */
extern int timers_get_multiplier();

/**
 * Get the current microsecond timer count
 * 
 * @return the number of clock cycles
 */
extern uint32_t timers_get_usec();

#endif
