/**
 * @file kbd_f256jr.h
 * @author your name (you@domain.com)
 * @brief Driver for the F256jr PS/2 keyboard
 * @version 0.1
 * @date 2024-06-17
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef __kbd_f256jr_h__
#define __kbd_f256jr_h__

#include "log_level.h"
#ifndef DEFAULT_LOG_LEVEL
    #define DEFAULT_LOG_LEVEL LOG_ERROR
#endif

#include <stdbool.h>

#include "interrupt.h"
#include "log.h"
#include "ps2_reg.h"
#include "ring_buffer.h"
#include "sys_macros.h"

//
// Constants
//

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
 * Special PS/2 byte codes
 */

#define KBD_PS2_SC_LSHIFT	0x12
#define KBD_PS2_SC_RSHIFT	0x59
#define KBD_PS2_SC_CAPS		0x58
#define KBD_PS2_SC_LCTRL	0x14
#define KBD_PS2_SC_RCTRL	0x14
#define KBD_PS2_SC_LALT		0x11
#define KBD_PS2_SC_RALT		0x11
#define KBD_PS2_SC_NUM		0x77
#define KBD_PS2_SC_LGUI		0x1f
#define KBD_PS2_SC_RGUI		0x27

/*
 * Special Foenix key scan codes
 */

#define KEY_C				0x2e
#define KEY_BREAK			0x61

/**
 * @brief Translation from base set 2 scancodes to Foenix scancodes
 * 
 */
const short kbd_ps2_default_sc[] = {
	0x00, 0x43, 0x00, 0x3f, 0x3d, 0x3b, 0x3c, 0x58, 0x00, 0x44, 0x42, 0x40, 0x3e, 0x0f, 0x29, 0x00, // 0x00 - 0x0f
	0x00, 0x38, 0x2a, 0x00, 0x1d, 0x10, 0x02, 0x00, 0x00, 0x00, 0x2c, 0x1f, 0x1e, 0x11, 0x03, 0x00, // 0x10 - 0x1f
	0x00, 0x2e, 0x2d, 0x20, 0x12, 0x05, 0x04, 0x00, 0x00, 0x39, 0x2f, 0x21, 0x14, 0x13, 0x06, 0x00, // 0x20 - 0x2f
	0x00, 0x31, 0x30, 0x23, 0x22, 0x15, 0x07, 0x00, 0x00, 0x00, 0x32, 0x24, 0x16, 0x08, 0x09, 0x00, // 0x30 - 0x3f
	0x00, 0x33, 0x25, 0x17, 0x18, 0x0b, 0x0a, 0x00, 0x00, 0x34, 0x35, 0x26, 0x27, 0x19, 0x0c, 0x00, // 0x40 - 0x4f
	0x00, 0x00, 0x28, 0x00, 0x1a, 0x0d, 0x00, 0x00, 0x3a, 0x36, 0x1c, 0x1b, 0x00, 0x2b, 0x00, 0x00, // 0x50 - 0x5f
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0e, 0x00, 0x00, 0x4f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 0x60 - 0x6f
	0x52, 0x53, 0x50, 0x4c, 0x4d, 0x48, 0x01, 0x45, 0x57, 0x4e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 0x70 - 0x7f
	0x00, 0x00, 0x00, 0x41, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 0x80 - 0x8f
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 0x90 - 0x9f
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 0xa0 - 0xaf
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 0xb0 - 0xbf
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 0xc0 - 0xcf
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 0xd0 - 0xdf
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 0xe0 - 0xef
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 0xf0 - 0xff
};

/**
 * @brief Translation from set 2 E0 prefixed scancodes to Foenix scancodes
 * 
 */
const short kbd_ps2_e0_sc[] = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 0x00 - 0x0f
	0x00, 0x5c, 0x00, 0x00, 0x5e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x5b, // 0x10 - 0x1f
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x5b, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x5d, // 0x20 - 0x2f
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 0x30 - 0x3f
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x6c, 0x00, 0x00, 0x00, 0x00, 0x00, // 0x40 - 0x4f
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x6d, 0x00, 0x00, 0x00, 0x00, 0x00, // 0x50 - 0x5f
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x66, 0x00, 0x69, 0x63, 0x00, 0x00, 0x00, // 0x60 - 0x6f
	0x62, 0x65, 0x6a, 0x00, 0x6b, 0x68, 0x00, 0x00, 0x00, 0x00, 0x67, 0x00, 0x00, 0x64, 0x00, 0x00, // 0x70 - 0x7f
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 0x80 - 0x8f
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 0x90 - 0x9f
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 0xa0 - 0xaf
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 0xb0 - 0xbf
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 0xc0 - 0xcf
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 0xd0 - 0xdf
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 0xe0 - 0xef
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 0xf0 - 0xff
};

/**
 * @brief States of the keyboard engine
 * 
 */
enum kbd_state_e {
	kbd_state_default = 0,
	kbd_state_e0,
	kbd_state_e0f0,
	kbd_state_f0,
	kbd_state_e1,
	kbd_state_e114,
	kbd_state_e11477,
	kbd_state_e11477e1,
	kbd_state_e11477e1f0,
	kbd_state_e11477e1f014,
	kbd_state_e11477e1f014f0,
	kbd_state_e11477e1f014f077
};

//
// Module variables
//

static enum kbd_state_e kbd_state = kbd_state_default;
static t_word_ring scan_code_buffer;
static t_word_ring char_buffer; 
static uint8_t modifiers = 0;
static bool break_pressed = false;

//
// Code
//

static void kbd_send_cmd(uint8_t byte) {
	uint8_t status = 0;

	*PS2_OUT = byte;
	*PS2_CTRL |= PS2_CTRL_KBD_WR;

	do {
		status = *PS2_STAT;
	} while ((status & (PS2_STAT_KBD_ACK | PS2_STAT_KBD_NAK)) == 0);

	*PS2_CTRL &= ~PS2_CTRL_KBD_WR;
}

/**
 * @brief Enqueue the scancode into the keyboard scancode buffer with modifiers added
 * 
 * @param scancode the base scancode to enqueue
 * @param is_make true if the byte is a make code, false if it's a break
 */
static void kbd_enqueue_scancode(short scancode, bool is_make) {
	if (is_make) {
		// CTRL-C or CTRL-PAUSE trigger a break
		if (modifiers & KBD_MOD_CTRL) {
			if (scancode == KEY_C) {
				break_pressed = true;
			}
		} else if (modifiers & KBD_MOD_ALT) {
			if (scancode == KEY_BREAK) {
				break_pressed = true;
			}
		}
		rb_word_put(&scan_code_buffer, modifiers << 8 | (scancode & 0x00ff));

	} else {
		rb_word_put(&scan_code_buffer, modifiers << 8 | (scancode & 0x00ff) | 0x0080);
	}
}

/**
 * @brief Handle the special tracking for modifier and lock keys
 * 
 * @param modifier the modifier flag to either toggle or set/clear
 * @param is_make true if the byte is a make code, false if it's a break
 */
static void kbd_process_modifier(short modifier, bool is_make) {
	switch(modifier) {
		case KBD_MOD_SHIFT:
		case KBD_MOD_CTRL:
		case KBD_MOD_ALT:
			if (is_make) {
				modifiers |= modifier;
			} else {
				modifiers &= ~modifier;
			}
			break;

		case KBD_LOCK_CAPS:
		case KBD_LOCK_NUM:
			if (!is_make) {
				modifiers ^= modifier;
			}
			break;

		default:
			break;
	}
}

/**
 * @brief Handle a set 2 default scancode byte
 * 
 * @param byte_code the set 2 scancode to handle
 * @param is_make true if the byte is a make code, false if it's a break
 */
static void kbd_process_bytecode(uint8_t byte_code, bool is_make) {
	// Process modifier keys
	switch(byte_code) {
		case KBD_PS2_SC_LSHIFT:
		case KBD_PS2_SC_RSHIFT:
			// Shift keys
			kbd_process_modifier(KBD_MOD_SHIFT, is_make);
			break;

		case KBD_PS2_SC_CAPS:
			// Caps lock key
			kbd_process_modifier(KBD_LOCK_CAPS, is_make);
			break;

		case KBD_PS2_SC_LCTRL:
			// CTRL Key
			kbd_process_modifier(KBD_MOD_CTRL, is_make);
			break;

		case KBD_PS2_SC_LALT:
			// Alt Key
			kbd_process_modifier(KBD_MOD_ALT, is_make);
			break;

		case KBD_PS2_SC_NUM:
			// Number lock key
			kbd_process_modifier(KBD_LOCK_NUM, is_make);
			break;

		default:
			break;
	}

	// Translate PS/2 scancode to Foenix scancode
	short scancode = kbd_ps2_default_sc[byte_code];
	if (scancode) {
		kbd_enqueue_scancode(scancode, is_make);
	}
}

/**
 * @brief Handle a set 2 E0-prefixed scancode byte
 * 
 * @param byte_code the set 2 E0-prefixed scancode to handle
 * @param is_make true if the byte is a make code, false if it's a break
 */
static void kbd_process_e0_bytecode(uint8_t byte_code, bool is_make) {
	// Process modifier keys
	switch(byte_code) {
		case KBD_PS2_SC_LGUI:
		case KBD_PS2_SC_RGUI:
			// GUI key
			kbd_process_modifier(KBD_MOD_OS, is_make);
			break;

		case KBD_PS2_SC_RCTRL:
			// CTRL Key
			kbd_process_modifier(KBD_MOD_CTRL, is_make);
			break;

		case KBD_PS2_SC_RALT:
			// Alt Key
			kbd_process_modifier(KBD_MOD_ALT, is_make);
			break;

		default:
			break;
	}

	// Translate PS/2 scancode with the E0 prefix to Foenix scancode
	short scancode = kbd_ps2_e0_sc[byte_code];
	if (scancode) {
		kbd_enqueue_scancode(scancode, is_make);
	}
}

/**
 * @brief Process the bytecodes that come in from the PS/2 keyboard. Walk through the state machine for E0, F0, and E1 prefixes
 * 
 * @param byte_code the raw bytes from the PS/2 interface
 */
static void kbd_process_set2_bytecode(uint8_t byte_code) {
	switch(kbd_state) {
		case kbd_state_default:
			switch(byte_code) {
				case 0xe0:
					kbd_state = kbd_state_e0;
					break;

				case 0xf0:
					kbd_state = kbd_state_f0;
					break;

				case 0xe1:
					kbd_state = kbd_state_e1;
					break;

				default:
					kbd_process_bytecode(byte_code, true);
					break;
			}
			break;

		case kbd_state_e0:
			if (byte_code == 0xf0) {
				kbd_state = kbd_state_e0f0;
			} else {
				kbd_process_e0_bytecode(byte_code, true);
				kbd_state = kbd_state_default;
			}
			break;

		case kbd_state_f0:
			kbd_process_bytecode(byte_code, false);
			kbd_state = kbd_state_default;
			break;

		case kbd_state_e0f0:
			kbd_process_e0_bytecode(byte_code, false);
			kbd_state = kbd_state_default;
			break;

		case kbd_state_e1:
			if (byte_code == 0x14) {
				kbd_state = kbd_state_e114;
			} else {
				kbd_state = kbd_state_default;
			}
			break;

		case kbd_state_e114:
			if (byte_code == 0x77) {
				kbd_state = kbd_state_e11477;
			} else {
				kbd_state = kbd_state_default;
			}
			break;

		case kbd_state_e11477:
			if (byte_code == 0xe1) {
				kbd_state = kbd_state_e11477e1;
			} else {
				kbd_state = kbd_state_default;
			}
			break;

		case kbd_state_e11477e1:
			if (byte_code == 0xf0) {
				kbd_state = kbd_state_e11477e1f0;
			} else {
				kbd_state = kbd_state_default;
			}
			break;

		case kbd_state_e11477e1f0:
			if (byte_code == 0x14) {
				kbd_state = kbd_state_e11477e1f014;
			} else {
				kbd_state = kbd_state_default;
			}
			break;

		case kbd_state_e11477e1f014:
			if (byte_code == 0xf0) {
				kbd_state = kbd_state_e11477e1f014f0;
			} else {
				kbd_state = kbd_state_default;
			}
			break;

		case kbd_state_e11477e1f014f0:
			if (byte_code == 0x77) {
				kbd_enqueue_scancode(0x61, true);
			}
			kbd_state = kbd_state_default;
			break;

		default:
			kbd_state = kbd_state_default;
			break;
	}
}

/**
 * @brief Handle an IRQ to query the keyboard
 * 
 */
SYSTEMCALL void kbd_handle_irq() {
	// Check to see if there is a keyboard bytecode waiting... process it if so
	if ((*PS2_STAT & PS2_STAT_KBD_EMP) == 0) {
		kbd_process_set2_bytecode(*PS2_KBD_IN);
	}
}

/*
 * Try to retrieve the next scancode from the keyboard.
 *
 * Returns:
 *      The next scancode to be processed, 0 if nothing.
 */
unsigned short kbd_get_scancode() {
	if (!rb_word_empty(&scan_code_buffer)) {
		return rb_word_get(&scan_code_buffer);
	} else {
		return 0;
	}
}

/*
 * Check to see if a BREAK code has been pressed recently
 * If so, return true and reset the internal flag.
 *
 * BREAK will be CTRL-PAUSE or CTRL-C on the F256jr
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
	// Initialize the keyboard buffers
	rb_word_init(&scan_code_buffer);
	rb_word_init(&char_buffer);

	// Initialize the state of the keyboard
	kbd_state = kbd_state_default;
	modifiers = 0;
	break_pressed = false;

	// Register and enable the PS/2 interrupt handler
	// int_register(INT_KBD_PS2, kbd_handle_irq);
	// int_enable(INT_KBD_PS2);

	return 0;
}

#endif