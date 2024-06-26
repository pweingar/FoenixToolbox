/**
 * @file via_f256.h
 * @author your name (you@domain.com)
 * @brief Register definitions for the VIA 65C22 chips
 * @version 0.1
 * @date 2024-06-17
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef __via_f256_h__
#define __via_f256_h__

#include <stdint.h>

typedef struct via_s {
	uint8_t pb;				// Port B data
	uint8_t pa;				// Port A data
	uint8_t ddrb;			// Port B data-direction
	uint8_t ddra;			// Port a data-direction

	uint8_t t1c_l;			// Timer 1 Counter/Latch low-byte
	uint8_t t1c_h;			// Timer 1 Counter high-byte
	uint8_t t1l_l;			// Timer 1 Latch low-byte
	uint8_t t1l_h;			// Timer 1 Latch high-byte

	uint8_t t2c_l;			// Timer 2 Counter low-byte
	uint8_t t2c_h;			// Timer 2 Counter high-byte

	uint8_t sr;				// Shift register

	uint8_t acr;			// Auxiliary Control Register
	uint8_t pcr;			// Peripheral Control Register
	uint8_t ifr;			// Interrupt Flag Register
	uint8_t ier;			// Interrupt Enable Register

	uint8_t pa_nh;			// Port A -- No Handshake
} via_t, *via_p;

//
// VIA Interrupt Flags
//

#define VIA_INT_CA2 0x01
#define VIA_INT_CA1 0x02
#define VIA_INT_SR 0x04
#define VIA_INT_CB2 0x08
#define VIA_INT_CB1 0x10
#define VIA_INT_TIMER2 0x20
#define VIA_INT_TIMER1 0x40
#define VIA_INT_IRQ 0x80

#define via0 ((volatile __attribute__((far)) via_p)0xf01c00)
#define via1 ((volatile __attribute__((far)) via_p)0xf01b00)

#endif
