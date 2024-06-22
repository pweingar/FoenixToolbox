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

#include "dev/kbd_f256k.h"
#include "F256/via_f256.h"
#include <stdbool.h>
#include <stdint.h>

//
// Constants
//

#define KBD_MATRIX_SIZE		9
#define KBD_COLUMNS			9
#define KBD_ROWS			8

//
// Map a key's matrix position to its scan code (FoenixMCP scan codes are used here)
//

static const uint8_t kbd_scan_codes[KBD_COLUMNS][KBD_ROWS] = {
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}
};

//
// Driver variables
//

static uint8_t kbd_stat[KBD_MATRIX_SIZE];
static short counter = 0;
static uint8_t last_press = 0;

// 		PB0	PB1		PB2		PB3			PB4		PB5	PB6	PB7		PB8
// PA7	1	<==		CTRL	RUN			SPC		F	Q	2
// PA1	3	W		A		L-SHIFT		Z		S	E	4
// PA2	5	R		D		X			C		F	T	6
// PA3	7	Y		G		V			B		H	U	8
// PA4	9	I		J		N			M		K	O	0
// PA5	+	P		L		,			.		:	@ 	CAPS
// PA6	-	*		;		/			R-SHIFT	ALT	TAB	HOME	RIGHT
// PA0	DEL	RETURN	LEFT	UP			F1		F3	F5	F7		DOWN

/**
 * @brief Get the keys selected in a given column
 * 
 * @param column the number of the column (0 - 8)
 * @return uint8_t a bitfield representing the keys in that column (0 = released, 1 = pressed)
 */
static uint8_t kbd_get_rows(short column) {
	uint8_t result = 0x00;

	if (column > 7) {
		via0->pb = 0x80;
		result = via1->pa;
		via0->pb = 0;
	} else {
		via1->pb = 0x01 << column;
		result = via1->pa;
		via1->pb = 0;
	}

	return result;
}

/**
 * @brief Handle a key MAKE or BREAK event... set up for auto repeat and queue the MAKE/BREAK scan code
 * 
 * @param column the number of the column (0 - 8)
 * @param row the number of the row (0 - 7)
 * @param is_pressed TRUE, the key is down... FALSE, the key is up
 */
static void kbd_process_key(short column, short row, bool is_pressed) {
	uint8_t scan_code = kbd_scan_codes[column][row];
	if (scan_code != 0) {
		if (!is_pressed) {
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

		// TODO: queue the scan code
	}
}

/**
 * @brief Handle an IRQ to query the keyboard
 * 
 */
void kbd_handle_irq() {
	for (short column = 0; column < 8; column++) {
		// Check each column to see if any key is pressed
		uint8_t rows_stat = kbd_get_rows(column);
		uint8_t rows_eor = kbd_stat[column] ^ rows_stat;
		if (rows_eor != 0) {
			short row = 0;

			kbd_stat[column] = rows_stat;
			while (rows_eor != 0) {
				if (rows_eor & 0x01) {
					// Current key changed
					kbd_process_key(column, row, rows_stat && 0x01); 
				}

				rows_stat = rows_stat >> 1;
				rows_eor = rows_eor >> 1;
				row++;
			}
		}
	}
}

/*
 * Initialize the matrix keyboard
 *
 */
short kbd_init() {
	// Initialize VIA0 -- we'll just write to PB7
	via0->ddra = 0x00;
	via0->ddrb = 0x80;
	via0->acr = 0x00;
	via0->pcr = 0x00;
	via0->ier = 0x00;

	// Initialize VIA1 -- we'll write to all of PB
	via1->ddra = 0x00;
	via1->ddrb = 0xff;
	via1->acr = 0x00;
	via1->pcr = 0x00;
	via1->ier = 0x00;

	for (short i = 0; i < KBD_MATRIX_SIZE; i++) {
		kbd_stat[i] = 0;
	}
}
