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
 * Wait for N microseconds (approximately)
 * 
 * NOTE: this will make use of TIMER2 on the A2560 models, which is based on the system clock
 * 
 * @param n the number of microseconds to wait
 */
extern void timer_wait_usec(unsigned int n);

#endif
