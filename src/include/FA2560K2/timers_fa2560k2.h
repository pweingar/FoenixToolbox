/**
 * @file timers_fa2560k2.h
 *
 * Define timer registers on the FA2560K2
 */

#ifndef __FA2560K2_TIMERS_H
#define __FA2560K2_TIMERS_H
//// $FFB0_4000		$FFB0_5FFF	(8K) - Timer Block

#include <stdint.h>

/** Timer control register 0: timers 0, 1, and 2 */
#define TIMER_TCR0      ((volatile uint32_t *)0xFFB04000)
#define TCR_ENABLE_0    0x00000001  /** Enable counter 0 */
#define TCR_CLEAR_0     0x00000002  /** Master clear of counter 0 */
#define TCR_LOAD_0      0x00000004  /** Master load of counter 0 */
#define TCR_CNTUP_0     0x00000008  /** Counter 0 Count up if 1, count down if 0 */
#define TCR_RECLR_0     0x00000010  /** Will Automatically clear and repeat the count */
#define TCR_RELOAD_0    0x00000020  /** Will Automatically Reload and repeat the count */
#define TCR_INE_0       0x00000080  /** Interrupt enable for counter 0 */

#define TCR_ENABLE_1    0x00000100  /** Enable counter 1 */
#define TCR_CLEAR_1     0x00000200  /** Master clear of counter 1 */
#define TCR_LOAD_1      0x00000400  /** Master load of counter 1 */
#define TCR_CNTUP_1     0x00000800  /** Counter 1 Count up if 1, count down if 0 */
#define TCR_RECLR_1     0x00001000  /** Will Automatically clear and repeat the count */
#define TCR_RELOAD_1    0x00002000  /** Will Automatically Reload and repeat the count */
#define TCR_INE_1       0x00008000  /** Interrupt enable for counter 1 */

#define TCR_ENABLE_2    0x00010000  /** Enable counter 2 */
#define TCR_CLEAR_2     0x00020000  /** Master clear of counter 2 */
#define TCR_LOAD_2      0x00040000  /** Master load of counter 2 */
#define TCR_CNTUP_2     0x00080000  /** Counter 2 Count up if 1, count down if 0 */
#define TCR_RECLR_2     0x00100000  /** Will Automatically clear and repeat the count */
#define TCR_RELOAD_2    0x00200000  /** Will Automatically Reload and repeat the count */
#define TCR_INE_2       0x00800000  /** Interrupt enable for counter 2 */

/** Timer control register 1: timers 3 and 4 */
#define TIMER_TCR1      ((volatile uint32_t *)0xFFB04004)
#define TCR_ENABLE_3    0x00000001  /** Enable counter 3 */
#define TCR_CLEAR_3     0x00000002  /** Master clear of counter 3 */
#define TCR_LOAD_3      0x00000004  /** Master load of counter 3 */
#define TCR_CNTUP_3     0x00000008  /** Counter 3 Count up if 1, count down if 0 */
#define TCR_RECLR_3     0x00000010  /** Enable reclear of timer 3 */
#define TCR_RELOAD_3    0x00000020  /** Enable reload of timer 3 */
#define TCR_INE_3       0x00000080  /** Interrupt enable for counter 3 */

#define TCR_ENABLE_4    0x00000100  /** Enable counter 4 */
#define TCR_CLEAR_4     0x00000200  /** Master clear of counter 4 */
#define TCR_LOAD_4      0x00000400  /** Master load of counter 4 */
#define TCR_CNTUP_4     0x00000800  /** Counter 4 Count up if 1, count down if 0 */
#define TCR_RECLR_4     0x00001000  /** Enable reclear of timer 4 */
#define TCR_RELOAD_4    0x00002000  /** Enable reload of timer 4 */
#define TCR_INE_4       0x00008000  /** Interrupt enable for counter 4 */

#define TCR_STAT_EQ0    0x08000000  /** Read only: timer 0 is equal to its comparison value */
#define TCR_STAT_EQ1    0x10000000  /** Read only: timer 1 is equal to its comparison value */
#define TCR_STAT_EQ2    0x20000000  /** Read only: timer 2 is equal to its comparison value */
#define TCR_STAT_EQ3    0x40000000  /** Read only: timer 3 is equal to its comparison value */

#define TIMER_VALUE_0   ((volatile uint32_t *)0xFFB04008)
#define TIMER_COMPARE_0 ((volatile uint32_t *)0xFFB0400C)
#define TIMER_VALUE_1   ((volatile uint32_t *)0xFFB04010)
#define TIMER_COMPARE_1 ((volatile uint32_t *)0xFFB04014)
#define TIMER_VALUE_2   ((volatile uint32_t *)0xFFB04018)
#define TIMER_COMPARE_2 ((volatile uint32_t *)0xFFB0401C)
#define TIMER_VALUE_3   ((volatile uint32_t *)0xFFB04020)
#define TIMER_COMPARE_3 ((volatile uint32_t *)0xFFB04024)
#define TIMER_VALUE_4   ((volatile uint32_t *)0xFFB04028)
#define TIMER_COMPARE_4 ((volatile uint32_t *)0xFFB0402C)

#endif
