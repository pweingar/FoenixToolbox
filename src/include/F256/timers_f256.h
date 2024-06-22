/**
 * @file timers_c256.h
 *
 * Define timer registers on the C256
 */

#ifndef __C256_TIMERS_H
#define __C256_TIMERS_H

//
// TIMER_CTRL_* flags
//

#define TIMER_CTRL_EN		0x01	// Set to enable timer
#define TIMER_CTRL_SCLR		0x02	// Set to clear the timer for counting up
#define TIMER_CTRL_SLOAD	0x04	// Set to load the timer for counting down
#define TIMER_CTRL_CNT_UP	0x08	// Set to count up, clear to count down

//
// TIMER_CMP_* flags
//

#define TIMER_CMP_RECLR		0x01	// Set to reclear the register on reaching the comparison value
#define TIMER_CMP_RELOAD	0x02	// Set to reload the charge value on reaching 0

//
// Timer 0 -- Based on system clock (14318180Hz)
//

#define TIMER_CTRL_0    ((volatile __attribute__((far)) uint8_t *)0xf01650)
#define TIMER_CHG_L_0	((volatile __attribute__((far)) uint8_t *)0xf01651)
#define TIMER_CHG_M_0	((volatile __attribute__((far)) uint8_t *)0xf01652)
#define TIMER_CHG_H_0	((volatile __attribute__((far)) uint8_t *)0xf01653)
#define TIMER_CMPC_0	((volatile __attribute__((far)) uint8_t *)0xf01654)
#define TIMER_CMP_L_0	((volatile __attribute__((far)) uint8_t *)0xf01655)
#define TIMER_CMP_M_0	((volatile __attribute__((far)) uint8_t *)0xf01656)
#define TIMER_CMP_H_0	((volatile __attribute__((far)) uint8_t *)0xf01657)

//
// Timer 1 -- Based on start of frame clock (60/70 Hz)
//

#define TIMER_CTRL_1    ((volatile __attribute__((far)) uint8_t *)0xf01658)
#define TIMER_CHG_L_1	((volatile __attribute__((far)) uint8_t *)0xf01659)
#define TIMER_CHG_M_1	((volatile __attribute__((far)) uint8_t *)0xf0165a)
#define TIMER_CHG_H_1	((volatile __attribute__((far)) uint8_t *)0xf0165b)
#define TIMER_CMPC_1	((volatile __attribute__((far)) uint8_t *)0xf0165c)
#define TIMER_CMP_L_1	((volatile __attribute__((far)) uint8_t *)0xf0165d)
#define TIMER_CMP_M_1	((volatile __attribute__((far)) uint8_t *)0xf0165e)
#define TIMER_CMP_H_1	((volatile __attribute__((far)) uint8_t *)0xf0165f)

#endif
