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

#include "log.h"
#include "ring_buffer.h"
#include "dev/kbd_f256k.h"
#include "F256/via_f256.h"
#include <stdbool.h>
#include <stdint.h>

#include <stdio.h>
#include "uart.h"

//
// Constants
//

#define KBD_MATRIX_SIZE		9
#define KBD_COLUMNS			9
#define KBD_ROWS			8

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

static uint8_t kbd_stat[KBD_MATRIX_SIZE];
static short counter = 0;
static uint8_t last_press = 0;
static t_word_ring scan_code_buffer; 

/**
 * @brief Get the keys selected in a given column
 * 
 * @param column the number of the column (0 - 8)
 * @return uint8_t a bitfield representing the keys in that column (0 = released, 1 = pressed)
 */
static uint8_t kbd_get_rows(short column) {
	uint8_t result = 0x00;

	if (column > 7) {
		via0->pb = 0x00;
		result = via1->pa;
		via0->pb = 0x80;
	} else {
		via1->pb = ~(0x01 << column);
		result = via1->pa;
		via1->pb = 0xff;
	}

	char message[80];
	sprintf(message, "Row: %02X, %02X", result, column);
	uart_writeln(0, message);
	
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
	uint8_t scan_code = kbd_scan_codes[row][column];
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

		if (!rb_word_full(&scan_code_buffer)) {
			rb_word_put(&scan_code_buffer, (unsigned short)scan_code);
		}
	}
}

/*
 * Try to retrieve the next scancode from the keyboard.
 *
 * Returns:
 *      The next scancode to be processed, 0 if nothing.
 */
unsigned short kbd_get_scancode() {
	if (rb_word_empty(&scan_code_buffer)) {
		return 0;
	} else {
		return rb_word_get(&scan_code_buffer);
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

	// Set up and clear out the buffer for the scan codes
	rb_word_init(&scan_code_buffer);

	return 1;
}
