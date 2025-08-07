/*
 * Definitions for the interrupt controls for  the FA2560K2
 */

#include <calypsi/intrinsics68000.h>
#include <string.h>

#include "features.h"
#include "interrupt.h"
#include "log.h"

#define INT_MAX_HANDLERS	48

//
// Interrupt Handler Vectors
//

static p_int_handler int_handlers[INT_MAX_HANDLERS];

//
// Mapping of FA2560K2 and A2560M interrupts to Toolbox interrupts
//

/**
 * @brief Mapping of Toolbox interrupt numbers to FA2560 GABE group numbers (0xffff indicates an unassigned interrupt number)
 * 
 */
static uint16_t g_int_group[] = {
	0x0000, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, // 0x00 - 0x07
	0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, // 0x08 - 0x0f
    0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, // 0x10 - 0x17
	0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, // 0x18 - 0x1f
	0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, // 0x20 - 0x27
	0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff	// 0x28 - 0x2f
};

/**
 * @brief Mapping of Toolbox interrupt numbers to FA2560 GABE mask numbers (0xffff indicates an unassigned interrupt number)
 * 
 */
static uint16_t g_int_mask[] = {
	0x0001, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, // 0x00 - 0x07
	0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, // 0x08 - 0x0f
    0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, // 0x10 - 0x17
	0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, // 0x18 - 0x1f
	0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, // 0x20 - 0x27
	0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff	// 0x28 - 0x2f
};

//
// Interrupt bindings
//

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

// Interrupt 0x00: Vicky SOF
__attribute__((interrupt(0x0100))) void int_0x00() {
	int_process(0x00);
}

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
