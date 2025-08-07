/*
 * Definitions for the interrupt controls for  the FA2560K2
 */

#include <calypsi/intrinsics68000.h>
#include <stdint.h>
#include <string.h>

#include "sys_types.h"
#include "features.h"
#include "interrupt.h"
#include "log.h"

#define INT_MAX_HANDLERS	48

/**
 * Macro to help declare the low-level interrupt handler and map it to the appropriate Toolbox
 * interrupt number.
 * 
 * GEN_HANDLER(my_int_foo, 0x1234, INT_FOO) will declare a handler like this:
 * 
 * __attribute__((interrupt(0x1234))) void my_int_foo() {
 *     int_process(INT_FOO);
 * }
 * 
 * int_process() is a Toolbox function which will acknowledge the pending interrupt in the
 * interrupt controller, lookup the associated interrupt handler, and call it if one is defined.
 * 
 */
#define GEN_HANDLER(name, vec, num) __attribute__((interrupt(vec))) void name() { int_process(num); }

//
// Interrupt Handler Vectors
//

static p_int_handler int_handlers[INT_MAX_HANDLERS];

//
// Mapping of FA2560K2 and A2560M interrupts to Toolbox interrupts
//

// Controller		Vector			Purpose				Toolbox Index	
// Group	Bit	Dec	Hex	Address							Name			Value
// 0		0	64	40	100			SOF					INT_SOF_A		0x00
// 			1	65	41	104			SOL					INT_SOL_A		0x01
// 			2	66	42	108			Reserved		
// 			3	67	43	10C			Reserved		
// 			4	68	44	110			Foenix Keyboard		INT_KBD_A2560K	0x11
// 			5	69	45	114			PS/2 Keyboard		INT_KBD_PS2		0x10
// 			6	70	46	118			PS/2 Mouse			INT_MOUSE		0x12
// 			7	71	47	11C			USB UART			INT_COM1		0x13
// 			15	79	4F	13C			RTC					INT_RTC			0x1f

// 1		0	80	50	140			DMA Channel 0		
// 			1	81	51	144			DMA Channel 1		
// 			2	82	52	148			DMA Channel 2		
// 			3	83	53	14C			DMA Channel 3		
// 			4	84	54	150			DMA Channel 4		
// 			5	85	55	154			DMA Channel 5		
// 			6	86	56	158			DMA Channel 6		
// 			7	87	57	15C			DMA Channel 7		
// 			8	88	58	160			SD Card Insert		INT_SDC_INS		0x21
// 			9	89	59	164			Timer 0				INT_TIMER0		0x18
// 			10	90	5A	168			Timer 1				INT_TIMER1		0x19
// 			11	91	5B	16C			Timer 2				INT_TIMER2		0x1a
// 			12	92	5C	170			Timer 3				INT_TIMER3		0x1b
// 			13	93	5D	174			VIA0				INT_VIA0		0x1d
// 			14	94	5E	178			VIA1				INT_VIA1		0x1e
// 			15	95	5F	17C			WizFi360 FIFO Rx	

// 2		0	96	60	180			WS6100		
// 			1	97	61	184			WizFi360 ???		
// 			2	98	62	188			Expansion Cartridge	INT_EXPANSION	0x06
// 			3	99	63	18C			Reserved		
// 			4	100	64	190			Reserved		
// 			5	101	65	194			Reserved		
// 			6	102	66	198			Reserved		
// 			7	103	67	19C			Reserved		
// 			8	104	68	1A0			SAM2695 MIDI		
// 			9	105	69	1A4			VS1053B MIDI		
// 			10	106	6A	1A8			OPL3 Timer			INT_OPL3		0x25
// 			11	107	6B	1AC			OPN2 Timer			INT_OPN2		0x24
// 			12	108	6C	1B0			OPM Timer			INT_OPM			0x23
// 			13	109	6D	1B4			PWM FIFO Empty		
// 			14	110	6E	1B8			DAC44K FIFO Empty 	INT_DAC0_PB		0x2f
// 			15	111	6F	1BC			DAC48K FIFO Empty	INT_DAC1_PB		0x2d

/**
 * @brief Mapping of Toolbox interrupt numbers to FA2560 GABE group numbers (0xffff indicates an unassigned interrupt number)
 *
 */
const uint16_t g_int_group[] = {
	0x0000, 
	0x0000, 
	0xffff, 
	0xffff, 
	0xffff, 
	0xffff, 
	0x0002, 
	0xffff, 
	0xffff, 
	0xffff, 
	0xffff, 
	0xffff, 
	0xffff, 
	0xffff, 
	0xffff, 
	0xffff, 
	0x0000, 
	0x0000, 
	0x0000, 
	0x0000, 
	0xffff, 
	0xffff, 
	0xffff, 
	0xffff, 
	0x0001, 
	0x0001, 
	0x0001, 
	0x0001, 
	0xffff, 
	0x0001, 
	0x0001, 
	0x0000, 
	0xffff, 
	0x0001, 
	0xffff, 
	0x0002, 
	0x0002, 
	0x0002, 
	0xffff, 
	0xffff, 
	0xffff, 
	0xffff, 
	0xffff, 
	0xffff, 
	0xffff, 
	0x0002, 
	0xffff
};

/**
 * @brief Mapping of Toolbox interrupt numbers to FA2560 GABE mask numbers (0xffff indicates an unassigned interrupt number)
 *
 */
const uint16_t g_int_mask[] = {
	0x0001,
	0x0002,
	0xffff,
	0xffff,
	0xffff,
	0xffff,
	0x0004,
	0xffff,
	0xffff,
	0xffff,
	0xffff,
	0xffff,
	0xffff,
	0xffff,
	0xffff,
	0xffff,
	0x0020,
	0x0010,
	0x0040,
	0x0080,
	0xffff,
	0xffff,
	0xffff,
	0xffff,
	0x0200,
	0x0400,
	0x0800,
	0x1000,
	0xffff,
	0x2000,
	0x4000,
	0x0100,
	0xffff,
	0x0100,
	0xffff,
	0x1000,
	0x0800,
	0x0400,
	0xffff,
	0xffff,
	0xffff,
	0xffff,
	0xffff,
	0xffff,
	0xffff,
	0x8000,
	0xffff
};

/**
 * General routine for processing an interrupt
 *
 * 1. Clear the interrupt pending flag to indicate that we're processing it
 * 2. Call the interrupt handler registered for it (if any)
 */
void int_process(short int_number) {
	int_clear(int_number);
	if (int_handlers[int_number]) {
		int_handlers[int_number]();
	}
}

//
// Interrupt bindings
//

GEN_HANDLER(int_0x00, 0x0100, INT_SOF_A)		// Interrupt 0x00: Vicky SOF
GEN_HANDLER(int_0x01, 0x0104, INT_SOL_A)		// Interrupt 0x01: Vicky SOL
GEN_HANDLER(int_0x11, 0x0110, INT_KBD_A2560K)	// Interrupt 0x11: Foenix Keyboard
GEN_HANDLER(int_0x10, 0x0114, INT_KBD_PS2)		// Interrupt 0x10: PS/2 Keyboard
GEN_HANDLER(int_0x12, 0x0118, INT_MOUSE)		// Interrupt 0x12: PS/2 Mouse
GEN_HANDLER(int_0x13, 0x011c, INT_COM1)			// Interrupt 0x13: UART
GEN_HANDLER(int_0x1f, 0x013c, INT_RTC)			// Interrupt 0x1f: RTC

GEN_HANDLER(int_0x21, 0x0160, INT_SDC_INS)		// Interrupt 0x21: SD Card insert
GEN_HANDLER(int_0x18, 0x0164, INT_TIMER0)		// Interrupt 0x18: Timer 0
GEN_HANDLER(int_0x19, 0x0168, INT_TIMER1)		// Interrupt 0x19: Timer 1
GEN_HANDLER(int_0x1a, 0x016c, INT_TIMER2)		// Interrupt 0x1a: Timer 2
GEN_HANDLER(int_0x1b, 0x0170, INT_TIMER3)		// Interrupt 0x1b: Timer 3
GEN_HANDLER(int_0x1d, 0x0174, INT_VIA0)			// Interrupt 0x1d: VIA0
GEN_HANDLER(int_0x1e, 0x0178, INT_VIA1)			// Interrupt 0x1e: VIA1

GEN_HANDLER(int_0x06, 0x0188, INT_EXPANSION)	// Interrupt 0x06: Expansion
GEN_HANDLER(int_0x25, 0x01a8, INT_OPL3_EXT)		// Interrupt 0x25: OPL3 Timer
GEN_HANDLER(int_0x24, 0x01ac, INT_OPN2_EXT)		// Interrupt 0x24: OPN2 Timer
GEN_HANDLER(int_0x23, 0x01b0, INT_OPM_INT)		// Interrupt 0x23: OPM Timer

GEN_HANDLER(int_0x2e, 0x01b8, INT_DAC0_PB)		// Interrupt 0x2e: DAC 44K playback done
GEN_HANDLER(int_0x2f, 0x01bc, INT_DAC1_PB)		// Interrupt 0x2f: DAC 48K playback done

// TODO: Add interrupt mappings for DMA, PWM, and network adapters

/*
 * Return the group number for the interrupt number
 */
uint16_t int_group(unsigned short n) {
	return g_int_group[n];
}

/*
 * Return the mask bit for the interrupt number
 */
uint16_t int_mask(unsigned short n) {
	return g_int_mask[n];
}

/*
 * Initialize the interrupt registers
 */
void int_init() {
	int i;

	// Clear all the interrupt handlers
	for (i = 0; i < INT_MAX_HANDLERS; i++) {
		int_handlers[i] = 0;
	}

    // At Reset, all of those already have those values
	// the Pol are @ 0x0000 and normally pending are reseted, but it is not impossible that some might be triggered during init

	*EDGE_GRP0 = 0xffff;
	*EDGE_GRP1 = 0xffff;
	*EDGE_GRP2 = 0xffff;

	*MASK_GRP0 = 0xffff;
	*MASK_GRP1 = 0xffff;
	*MASK_GRP2 = 0xffff;

	// Clear all the pending flags

	*PENDING_GRP0 = 0xffff;
	*PENDING_GRP1 = 0xffff;
	*PENDING_GRP2 = 0xffff;
}

/*
 * Enable all interrupts
 *
 * Returns:
 * a machine dependent representation of the interrupt masking prior to enabling
 */
SYSTEMCALL short int_enable_all() {
	// NOTE: this code uses Calypsi specific intrinsic functions
	//       and does a cast that may not be valid

	unsigned short status = __get_interrupt_state();
	__enable_interrupts();
	return status;
}

/*
 * Disable all interrupts
 *
 * Returns:
 * a machine dependent representation of the interrupt masking prior to disabling
 */
SYSTEMCALL short int_disable_all() {
	// NOTE: this code uses Calypsi specific intrinsic functions
	//       and does a cast that may not be valid

	unsigned short status = __get_interrupt_state();
	__disable_interrupts();
	return status;
}

/*
 * Restore interrupt masking state returned by a previous call to int_enable/int_disable
 *
 * NOTE: this is actually provided in the low level assembly
 *
 * Inputs:
 * state = a machine dependent representation of the interrupt masking to restore
 */
SYSTEMCALL void int_restore_all(short state) {
	// NOTE: this code uses Calypsi specific intrinsic functions
	//       and does a cast that may not be valid

	__restore_interrupt_state((__interrupt_state_t)state);
}

/*
 * Disable an interrupt by masking it
 *
 * Interrupt number is made by the group number and number within the group.
 * For instance, the RTC interrupt would be 0x1F and the Channel A SOF interrupt would be 0x00.
 *
 * Inputs:
 * n = the number of the interrupt: n[7..4] = group number, n[3..0] = individual number.
 */
SYSTEMCALL void int_disable(unsigned short n) {
	/* Find the group (the relevant interrupt mask register) for the interrupt */
	unsigned short group = int_group(n);

	/* Find the mask for the interrupt */
	unsigned short mask = int_mask(n);

	if ((group != 0xffff) && (mask != 0xffff)) {
		// Only set the mask if the mask and group numbers are valid
		uint8_t new_mask = MASK_GRP0[group] | mask;

		/* Set the mask bit for the interrupt in the correct MASK register */
		MASK_GRP0[group] = new_mask;
	}

}

/*
 * Enable an interrupt
 *
 * Interrupt number is made by the group number and number within the group.
 * For instance, the RTC interrupt would be 0x1F and the Channel A SOF interrupt would be 0x00.
 * And interrupt number of 0xFF specifies that all interrupts should be disabled.
 *
 * Inputs:
 * n = the number of the interrupt: n[7..4] = group number, n[3..0] = individual number.
 */
SYSTEMCALL void int_enable(unsigned short n) {
	/* Find the group (the relevant interrupt mask register) for the interrupt */
	unsigned short group = int_group(n);

	/* Find the mask for the interrupt */
	unsigned short mask = int_mask(n);

	if ((group != 0xffff) && (mask != 0xffff)) {
		// Only set the mask if the mask and group numbers are valid
		uint8_t new_mask = MASK_GRP0[group] & ~mask;

		/* Clear the mask bit for the interrupt in the correct MASK register */
		MASK_GRP0[group] = new_mask;
	}
}

/*
 * Register a handler for a given interrupt.
 *
 * Inputs:
 * n = the number of the interrupt: n[7..4] = group number, n[3..0] = individual number.
 * handler = pointer to the interrupt handler to register
 *
 * Returns:
 * the pointer to the previous interrupt handler
 */
SYSTEMCALL p_int_handler int_register(unsigned short n, p_int_handler handler) {
	p_int_handler old_handler = 0;

	if (n < INT_MAX_HANDLERS) {
		old_handler = int_handlers[n];
		int_handlers[n] = handler;
	}

	return old_handler;
}

/*
 * Return true (non-zero) if an interrupt is pending for the given interrupt
 *
 * Inputs:
 * n = the number of the interrupt: n[7..4] = group number, n[3..0] = individual number.
 *
 * Returns:
 * non-zero if interrupt n is pending, 0 if not
 */
SYSTEMCALL short int_pending(unsigned short n) {
	/* Find the group (the relevant interrupt mask register) for the interrupt */
	unsigned short group = int_group(n);

	/* Find the mask for the interrupt */
	unsigned short mask = int_mask(n);

	if ((group != 0xffff) && (mask != 0xffff)) {
		// Only query the pending mask if the mask and group numbers are valid
		return (PENDING_GRP0[group] & mask);

	} else {
		// If the mask or group number are invalid, just return false
		return 0;
	}
}

/*
 * Acknowledge an interrupt (clear out its pending flag)
 *
 * Inputs:
 * n = the number of the interrupt: n[7..4] = group number, n[3..0] = individual number.
 */
SYSTEMCALL void int_clear(unsigned short n) {
	/* Find the group (the relevant interrupt mask register) for the interrupt */
	unsigned short group = int_group(n);

	/* Find the mask for the interrupt */
	unsigned short mask = int_mask(n);

	if ((group != 0xffff) && (mask != 0xffff)) {
		// Only set the mask if the mask and group numbers are valid
		uint8_t new_mask = PENDING_GRP0[group] | mask;

		/* Set the bit for the interrupt to mark it as cleared */
		PENDING_GRP0[group] = new_mask;
	}
}
