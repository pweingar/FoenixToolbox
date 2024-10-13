/**
 * @file kbd_f256k.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2024-06-17
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "interrupt.h"
#include "log.h"
#include "ring_buffer.h"
#include "dev/kbd_f256k.h"
#include "F256/via_f256.h"
#include "F256/kbd_opt_f256.h"
#include "gabe_reg.h"
#include "simpleio.h"
#include "vicky_general.h"

//
// Constants
//

#define KBD_MATRIX_SIZE		9
#define KBD_COLUMNS			9
#define KBD_ROWS			8

/*
 * Modifier bit flags
 */

#define KBD_LOCK_SCROLL     0x01
#define KBD_LOCK_NUM        0x02
#define KBD_LOCK_CAPS       0x04
#define KBD_MOD_SHIFT       0x08
#define KBD_MOD_CTRL        0x10
#define KBD_MOD_ALT         0x20
#define KBD_MOD_OS          0x40
#define KBD_MOD_MENU        0x80

/*
 * Special key scan codes
 */

#define KEY_C 				0x2e
#define KEY_BREAK 			0x61

//
// Map a key's matrix position to its scan code (FoenixMCP scan codes are used here)
//

// 		PB0		PB1		PB2		PB3		PB4 	PB5		PB6	PB7			PB8
// PA0	DELETE	RETURN	LEFT	F7		F1		F3		F5	UP			DOWN
// PA1	3		W		A		4		Z		S		E	L-SHIFT	
// PA2	5		R		D		6		C		F		T	X	
// PA3	7		Y		G		8		B		H		U	V	
// PA4	9		I		J		0		M		K		O	N	
// PA5	-		P		L		CAPS	.		:		[	,	
// PA6	=		]		'		HOME	R-SHIFT	ALT		TAB	/			RIGHT
// PA7	1		BKSP	CTRL	2		SPACE	Foenix	Q	RUN/STOP

static const uint8_t kbd_scan_codes[KBD_ROWS][KBD_COLUMNS] = {
	{0x65, 0x1c, 0x69, 0x41, 0x3b, 0x3d, 0x3f, 0x68, 0x6a},
	{0x04, 0x11, 0x1e, 0x05, 0x2c, 0x1f, 0x12, 0x2a, 0x00},
	{0x06, 0x13, 0x20, 0x07, 0x2e, 0x21, 0x14, 0x2d, 0x00},
	{0x08, 0x15, 0x22, 0x09, 0x30, 0x23, 0x16, 0x2f, 0x00},
	{0x0a, 0x17, 0x24, 0x0b, 0x32, 0x25, 0x18, 0x31, 0x00},
	{0x0c, 0x19, 0x26, 0x3a, 0x34, 0x27, 0x1a, 0x33, 0x00},
	{0x0d, 0x1b, 0x28, 0x63, 0x36, 0x5c, 0x0f, 0x35, 0x6b},
	{0x02, 0x01, 0x1d, 0x03, 0x39, 0x5b, 0x10, 0x61, 0x00}
};


//
// Driver variables
//

static bool is_optical = false;
static uint16_t kbd_stat[KBD_MATRIX_SIZE];
static short counter = 0;
static uint8_t last_press = 0;
static t_word_ring scan_code_buffer;
static uint8_t modifiers = 0;
static bool break_pressed = 0;

/**
 * @brief Get the keys selected in a given row
 * 
 * @param row the number of the row to turn on (0 - 7)
 * @return uint8_t a bitfield representing the keys in that row (0 = released, 1 = pressed)
 */
static uint16_t kbd_get_columns(uint8_t row) {
	uint16_t result = 0;

	via1->pa = ~(0x01 << row);
	result = ((uint16_t)via1->pb | (((uint16_t)(via0->pb) & 0x0080) << 1)) ^ 0x1ff;
	
	return result;
}

/**
 * @brief Set or reset the modifier_flag in the modifiers variable, depending on if the key is pressed or not
 * 
 * @param modifier_flag the bit to change
 * @param is_pressed if true, key is pressed, if 0 key is released 
 */
static void kbd_process_modifier(uint8_t modifier_flag, bool is_pressed) {
	if (is_pressed) {
		modifiers |= modifier_flag;
	} else {
		modifiers &= ~(modifier_flag);
	}
}

/**
 * @brief Turn the caps lock LED on or off
 * 
 * @param cap_en true for on, false for off
 */
static void kbd_set_caps_led(bool cap_en) {
	if (cap_en) {
		*GABE_MSTR_CTRL |= GABE_CAP_EN;
	} else {
		*GABE_MSTR_CTRL &= (~GABE_CAP_EN);
	}
}

/**
 * @brief Handle a key MAKE or BREAK event... set up for auto repeat and queue the MAKE/BREAK scan code
 * 
 * @param column the number of the column (0 - 8)
 * @param row the number of the row (0 - 7)
 * @param is_pressed TRUE, the key is down... FALSE, the key is up
 */
static void kbd_process_key(short column, short row, bool is_pressed) {
	uint8_t scan_code = kbd_scan_codes[row][column];

	switch(scan_code) {
		case 0x2a:
		case 0x36:
			// Left or right SHIFT
			kbd_process_modifier(KBD_MOD_SHIFT, is_pressed);
			break;
		
		case 0x3a:
			// CAPS... flip KBD_LOCK_CAPS bit on MAKE, ignore BREAK
			if (is_pressed) {
				modifiers = modifiers ^ KBD_LOCK_CAPS;
				kbd_set_caps_led(modifiers & KBD_LOCK_CAPS);
			}
			break;

		case 0x1d:
			// Control key
			kbd_process_modifier(KBD_MOD_CTRL, is_pressed);
			break;

		case 0x5c:
			// ALT key
			kbd_process_modifier(KBD_MOD_ALT, is_pressed);
			break;

		case 0x5b:
			// FOENIX key
			kbd_process_modifier(KBD_MOD_OS, is_pressed);
			break;

		default:
			break;
	}

	if (scan_code != 0) {
		if (is_pressed == 0) {
			if (last_press == scan_code) {
				// If we released the last key pressed, remove it from the typematic variables
				last_press = 0;
			}

			// Convert the scan code to a BREAK scan code
			scan_code |= 0x80;
		} else {
			// Set the key press information for the key press event
			counter = 0;
			last_press = scan_code;
		}

		// Set the break flag if the RUN/STOP or CTRL-C key was pressed
		if ((scan_code == KEY_BREAK) ||
			((scan_code == KEY_C) && ((modifiers & KBD_MOD_CTRL) != 0))) {
			break_pressed = true;
		}

		if (!rb_word_full(&scan_code_buffer)) {
			rb_word_put(&scan_code_buffer, (modifiers << 8) | scan_code);
		}
	}
}

/*
 * Try to retrieve the next scancode from the keyboard.
 *
 * Returns:
 *      The next scancode to be processed, 0 if nothing.
 */
SYSTEMCALL unsigned short kbd_get_scancode() {
	if (rb_word_empty(&scan_code_buffer)) {
		return 0;
	} else {
		return rb_word_get(&scan_code_buffer);
	}
}

/**
 * @brief Scan the state of the keys on the mechanical keyboard
 * 
 */
void kbd_scan_mechanical() {
	uint8_t ifr = via0->ifr;
	uint8_t counter_low = via0->t1c_l;

	for (uint8_t row = 0; row < 8; row++) {
		// Check each column to see if any key is pressed
		uint16_t columns_stat = kbd_get_columns(row);
		uint16_t columns_eor = kbd_stat[row] ^ columns_stat;
		if (columns_eor != 0) {
			short column = 0;

			kbd_stat[row] = columns_stat;
			while (columns_eor != 0) {
				if (columns_eor & 0x01) {
					// Current key changed
					kbd_process_key(column, row, columns_stat & 0x01);
				}

				columns_stat = columns_stat >> 1;
				columns_eor = columns_eor >> 1;
				column++;
			}
		}
	}
}

/**
 * @brief Query the optical keyboard to get the scanned keyboard matrix (if it has any pending data)
 * 
 */
void kbd_scan_optical() {
	if ((KBD_OPTICAL->status & KBD_OPT_STAT_EMPTY) == 0) {
		// If there is data pending in the keyboard buffer...

		// Read in all the bytes available and fill out the column matrix
		uint16_t count = KBD_OPTICAL->count;
		for (int x = 0; x < count; x += 2) {
			// Get two data bytes
			uint8_t byte_hi = KBD_OPTICAL->data;
			uint8_t byte_low = KBD_OPTICAL->data;

			// Separate out the row number and the column status bits
			uint8_t row = (byte_hi >> 4) & 0x07;
			uint16_t columns_stat = ((uint16_t)byte_hi << 8 | (uint16_t)byte_low) & 0x01ff;

			// Determine which columns have changed on this row
			uint16_t columns_eor = kbd_stat[row] ^ columns_stat;
			if (columns_eor != 0) {
				// If columns have changed...
				short column = 0;

				// Record the new column values
				kbd_stat[row] = columns_stat;
				while (columns_eor != 0) {
					if (columns_eor & 0x01) {
						// Current key changed
						kbd_process_key(column, row, columns_stat & 0x01);
					}

					columns_stat = columns_stat >> 1;
					columns_eor = columns_eor >> 1;
					column++;
				}
			}
		}
	}
}

/**
 * @brief Handle an IRQ to query the keyboard
 * 
 */
void kbd_handle_irq() {
	// The scanning process is different depending on the keyboard type:
	//
	// The mechanical keyboard is a simple matrix of keys, and we need to scan it to see
	// which keys are pressed and which are not.
	//
	// The optical keyboard does more processing for us. If it detects a change, it will
	// indicate that it has data for us and then let us read the entire state of the matrix
	// as a sequence of bytes.

	if (is_optical) {
		kbd_scan_optical();
	} else {
		kbd_scan_mechanical();
	}
}

/*
 * Check to see if a BREAK code has been pressed recently
 * If so, return true and reset the internal flag.
 *
 * BREAK will be RUN/STOP or CTRL-C on the F256K
 *
 * Returns:
 * true if a BREAK has been pressed since the last check
 */
bool kbd_break() {
	bool result = break_pressed;
	break_pressed = 0;
	return result;
}

/*
 * Initialize the matrix keyboard
 *
 */
short kbd_sc_init() {
	if (KBD_OPTICAL->status & KBD_OPT_STAT_MECH) {
		is_optical = false;
	} else {
		is_optical = true;
	}

	// Initialize VIA0 -- we'll just read from PB7
	via0->ddra = 0x00;
	via0->ddrb = 0x00;
	via0->acr = 0x00;
	via0->pcr = 0x00;
	via0->ier = 0x00;

	// Initialize VIA1 -- we'll write to all of PB only if the keyboard is mechanical
	if (is_optical) {
		// For an optical keyboard, make it read only just to be safe
		via1->ddra = 0x00;
	} else {
		via1->ddra = 0xff;
	}
	via1->ddrb = 0x00;
	via1->acr = 0x00;
	via1->pcr = 0x00;
	via1->ier = 0x00;

	// Clear the modifiers and intialize the caps lock LED to off
	modifiers = 0;
	kbd_set_caps_led(0);

	// Initialize the keyboard status to nothing pressed
	for (short i = 0; i < KBD_MATRIX_SIZE; i++) {
		kbd_stat[i] = 0;
	}

	// Set up and clear out the buffer for the scan codes
	rb_word_init(&scan_code_buffer);

	// int_register(INT_VIA0, kbd_handle_irq);
	int_register(INT_SOF_A, kbd_handle_irq);

	// via0->acr = 0x40;								// Timer #0 in free running mode
	// via0->ier = VIA_INT_TIMER1 | VIA_INT_IRQ;		// Allow timer #0 interrupts
	// via0->t1c_l = 0xff;								// Set timer count
	// via0->t1c_h = 0xff;

	// int_enable(INT_VIA0);
	int_enable(INT_SOF_A);

	return 1;
}
