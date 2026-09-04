/*
 * Definitions for the interrupt controls
 */

#include <string.h>

#include "features.h"
#include "interrupt.h"
#include "calypsi/intrinsics68000.h"

//
// Interrupt numbers (group and number) for A2560me interrupts
//

// // Interrupt group 0
// #define INT_SOF             0x00    // Vicky Start of Frame - There is no 2 Channels in the A2560Me
// #define INT_SOL             0x01    // Vicky Start of Line - There is no 2 Channels in the A2560Me
// #define INT_VICKY1          0x02    // Other Possible Graphic Core Engine Interrupts to be defined later - Reserved
// #define INT_VICKY2          0x03    // Other Possible Graphic Core Engine Interrupts to be defined later - Reserved
// #define INT_COM3_USB        0x07    // New Internal UART Connected to USB-C
// #define INT_KBD_PS2         0x08    // PS/2 Keyboard
// #define INT_MOUSE           0x09    // PS/2 Mouse
// #define INT_COM1            0x0A    // COM1
// #define INT_COM2            0x0B    // COM2
// #define INT_FDC             0x0C    // Floppy controller
// #define INT_LPT             0x0D    // Parallel port
// #define INT_MIDI            0x0E    // MPU-401 / MIDI
// #define INT_RTC             0x0F    // Real Time Clock

// // Interrupt group 1
// #define INT_DMA_CH0         0x10    // DMA Channel 0
// #define INT_DMA_CH1         0x11    // DMA Channel 1
// #define INT_DMA_CH2         0x12    // DMA Channel 2
// #define INT_DMA_CH3         0x13    // DMA Channel 3
// #define INT_DMA_CH4         0x14    // DMA Channel 4
// #define INT_DMA_CH5         0x15    // DMA Channel 5
// #define INT_DMA_CH6         0x16    // DMA Channel 6
// #define INT_DMA_CH7         0x17    // DMA Channel 7
// #define INT_SDC_INS   	   0x18
// #define INT_TIMER0          0x19    // Timer 0, Clocked with the CPU Clock (Local BUS @ 33Mhz / 50Mhz)
// #define INT_TIMER1          0x1A    // Timer 1, Clocked with the CPU Clock (Local BUS @ 33Mhz / 50Mhz)
// #define INT_TIMER2          0x1B    // Timer 2, Clocked with the CPU Clock (Local BUS @ 33Mhz / 50Mhz)
// #define INT_TIMER3          0x1C    // Timer 3, Clocked with the SOF - 60Hz in the case of the A2560Me
// #define INT_WIZFI_FIFO      0x1F    // There is a WIZFI360 RX FIFO IRQ in the A2560Me - this is a FIFO IRQ

// // Interrupt group 2
// #define INT_W6100           0x20    // W6100 Ethernet Chip IRQ
// #define INT_WIZFI_MOD       0x21    // WizFi360 Module IRQ - This is the Interrupt from the module itself
// #define INT_PCIE            0x23    // A2560M PCI Express
// #define INT_USB_OTG         0x24    // USB On the GO
// #define INT_RSVD_0          0x25    // Reserved
// #define INT_RSVD_1          0x26    // Reserved
// #define INT_RSVD_2          0x27    // Reserved
// #define INT_SAM2695_FIFO    0x28    // Beatrix 0 - This is a FIFO Emtpy/Full Interrupt
// #define INT_VS1053B_FIFO    0x29    // Beatrix 1 - This is a FIFO Emtpy/Full Interrupt
// #define INT_OPL3            0x2A    // Beatrix 2
// #define INT_OPM2            0x2B    // Beatrix 3
// #define INT_OPN             0x2C    // Reserved
// #define INT_DAC1_FIFO       0x2E    // DAC0 Playback Done (44.1K) - This will be a FIFO Almost Emtpy Interrupt
// #define INT_DAC0_FIFO       0x2F    // DAC1 Playback Done (48K) - This will be a FIFO Almost Emtpy Interrupt

#define MAX_HANDLERS 48

p_int_handler g_int_handler[MAX_HANDLERS];

/*
 * Enable all interrupts
 *
 * NOTE: this is actually provided in the low level assembly
 *
 * Returns:
 * a machine dependent representation of the interrupt masking prior to enabling
 */
SYSTEMCALL short int_enable_all() {
	__interrupt_state_t state = __get_interrupt_state();
	__enable_interrupts();
	return (short)state;
}

/*
 * Disable all interrupts
 *
 * NOTE: this is actually provided in the low level assembly
 *
 * Returns:
 * a machine dependent representation of the interrupt masking prior to disabling
 */
SYSTEMCALL short int_disable_all() {
	__interrupt_state_t state = (short)__get_interrupt_state();
	__disable_interrupts();
	return (short)state;
}

/*
 * Return the group number for the interrupt number
 *
 * For the m68000 machines, this will just be the high nibble of the number
 */
unsigned short int_group(unsigned short n) {
	return ((n >> 4) & 0x0f);
}

/*
 * Return the mask bit for the interrupt number
 *
 * For the m68000 machines, this will just be the bit corresponding to the lower nibble
 */
unsigned short int_mask(unsigned short n) {
	return (1 << (n & 0x0f));
}

/*
 * Initialize the interrupt registers
 */
void int_init() {
	int i;

	memset(g_int_handler, 0, sizeof(g_int_handler));

    // At Reset, all of those already have those values
	// the Pol are @ 0x0000 and normally pending are reseted, but it is not impossible that some might be triggered during init
	*EDGE_GRP0 = 0xFFFF;
	*EDGE_GRP1 = 0xFFFF;
	*EDGE_GRP2 = 0xFFFF;

	*MASK_GRP0 = 0xFFFF;
	*MASK_GRP1 = 0xFFFF;
	*MASK_GRP2 = 0xFFFF;
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
	unsigned short new_mask = MASK_GRP0[group] | mask;

	/* Set the mask bit for the interrupt in the correct MASK register */
	MASK_GRP0[group] = new_mask;
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
	unsigned short new_mask = MASK_GRP0[group] & ~mask;

	/* Clear the mask bit for the interrupt in the correct MASK register */
	MASK_GRP0[group] = new_mask;
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
	if (n < MAX_HANDLERS) {
		p_int_handler old_handler = g_int_handler[n];
		g_int_handler[n] = handler;
	    return old_handler;
	} else {
		return 0;
	}
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

	/* Set the mask bit for the interrupt in the correct MASK register */
	return (PENDING_GRP0[group] & mask);
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
	unsigned short new_mask = PENDING_GRP0[group] | mask;

	/* Set the bit for the interrupt to mark it as cleared */
	PENDING_GRP0[group] = new_mask;
}

/**
 * Handler for VICKY interrupts
 * 
 * This interrupt is on auto vector level 6
 */
__attribute__((interrupt(0x0078))) void int_vicky() {
	unsigned short n;
	unsigned short mask;
	unsigned short pending = *PENDING_GRP0 & 0x000f;

	/* Acknowledge all the pending interrupts:
	 * NOTE: we have to do this, even if there is no handler for the interrupt */
	*PENDING_GRP0 = 0x000f;
	if (pending != 0) {
		for (n = 0, mask = 1; n < 4; n++, mask <<= 1) {
			if (pending & mask) {
				p_int_handler handler = g_int_handler[n];

				if (handler) {
					/* If we got a handler, call it */
					handler();
				}
			}
		}
	}
}

/* See FoenixMCP/src/m68k/startup_m68k.s
  #define INT_PS2KBD_VECN           0x40
  #define INT_PS2MOUSE_VECN         0x41
  #define INT_COM1_VECN             0x42
  #define INT_COM2_VECN             0x43
  #define INT_LPT_VECN              0x44
  #define INT_FDC_VECN              0x45
  #define INT_MIDI_VECN             0x46
  #define INT_BQ4802LY_VECN         0x47

  #define INT_SDCARD_INSERT_VECN    0x50
  #define INT_TIMER0_VECN           0x51 / Timer 0, Clocked with the CPU Clock /
  #define INT_TIMER1_VECN           0x52 / Timer 1, Clocked with the CPU Clock /
  #define INT_TIMER2_VECN           0x53 / Timer 2, Clocked with the CPU Clock /
  #define INT_TIMER3_VECN           0x54 / Timer 3, Clocked with the SOF /

  #define INT_VIA0_VECN             0x55
  #define INT_VIA1_VECN             0x56
  #define INT_WIZFI360_FIFO_RX_VECN 0x57
  #define INT_WS6100_VECN           0x58
  #define INT_WIZFI360_VECN         0x59
*/

/**
 * Forward an interrupt to its correct handler
 * 
 * This will be called by the routine registered with the CPU's interrupt vectors
 * for non-auto vector interrupts.
 * 
 * @param n the number of the interrupt: n[7..4] = group number, n[3..0] = individual number.
 */
inline void int_dispatch(unsigned short n) {
	p_int_handler handler = g_int_handler[n];
	if (handler) {
		// If there is a handler for this interrupt, call it
		handler();
	}

	// Acknowledge that the interrupt was processed
	int_clear(n);
}

/**
 * CPU interrupt routine for PS/2 keyboard (CPU vector 0x40)
 */
__attribute__((interrupt(0x0100))) void int_handle_40() {
	int_dispatch(INT_KBD_PS2);
}

/**
 * CPU interrupt routine for PS/2 mouse (CPU vector 0x41)
 */
__attribute__((interrupt(0x0104))) void int_handle_41() {
	int_dispatch(INT_MOUSE);
}

// /**
//  * CPU interrupt routine for interrupt #00 (CPU vector 0x00)
//  */
// __attribute__((interrupt(0x0000))) void int_handle_00() {
// 	int_dispatch(0x00);
// }