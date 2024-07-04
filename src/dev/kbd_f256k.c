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
#include "uart.h"
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

/*
 * Mapping of "codepoints" 0x80 - 0x98 (function keys, etc)
 * to ANSI escape codes
 */
const char * ansi_keys[] = {
    "1",    /* HOME */
    "2",    /* INS */
    "3",    /* DELETE */
    "4",    /* END */
    "5",    /* PgUp */
    "6",    /* PgDn */
    "A",    /* Up */
    "B",    /* Left */
    "C",    /* Right */
    "D",    /* Down */
    "11",   /* F1 */
    "12",   /* F2 */
    "13",   /* F3 */
    "14",   /* F4 */
    "15",   /* F5 */
    "17",   /* F6 */
    "18",   /* F7 */
    "19",   /* F8 */
    "20",   /* F9 */
    "21",   /* F10 */
    "23",   /* F11 */
    "24",   /* F12 */
    "30",   /* MONITOR */
    "31",   /* CTX SWITCH */
    "32"    /* MENU HELP */
};

/*
 * US keyboard layout scancode translation tables
 */

#define POUND 0x9c

const char kbd_256k_layout[] = {
    // Unmodified
    0x00, 0x1B, '1', '2', '3', '4', '5', '6',           /* 0x00 - 0x07 */
    '7', '8', '9', '0', '-', '=', 0x08, 0x09,           /* 0x08 - 0x0F */
    'q', 'w', 'e', 'r', 't', 'y', 'u', 'i',             /* 0x10 - 0x17 */
    'o', 'p', '[', ']', 0x0D, 0x00, 'a', 's',           /* 0x18 - 0x1F */
    'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',             /* 0x20 - 0x27 */
    0x27, '`', 0x00, '\\', 'z', 'x', 'c', 'v',          /* 0x28 - 0x2F */
    'b', 'n', 'm', ',', '.', '/', 0x00, '*',            /* 0x30 - 0x37 */
    0x00, ' ', 0x00, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E,      /* 0x38 - 0x3F */
    0x8F, 0x90, 0x91, 0x92, 0x93, 0x00, 0x00, 0x80,     /* 0x40 - 0x47 */
    0x86, 0x84, '-', 0x89, '5', 0x88, '+', 0x83,        /* 0x48 - 0x4F */
    0x87, 0x85, 0x81, 0x82, 0x96, 0x97, 0x98, 0x94,     /* 0x50 - 0x57 */
    0x95, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,     /* 0x58 - 0x5F */
    0x00, 0x00, 0x81, 0x80, 0x84, 0x82, 0x83, 0x85,     /* 0x60 - 0x67 */
    0x86, 0x89, 0x87, 0x88, '/', 0x0D, 0x00, 0x00,      /* 0x68 - 0x6F */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,     /* 0x70 - 0x77 */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,     /* 0x78 - 0x7F */

    // Shifted
    0x00, 0x1B, '!', '@', '#', '$', '%', '^',           /* 0x00 - 0x07 */
    '&', '*', '(', ')', '_', '+', 0x08, 0x09,           /* 0x08 - 0x0F */
    'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I',             /* 0x10 - 0x17 */
    'O', 'P', '{', '}', 0x0A, 0x00, 'A', 'S',           /* 0x18 - 0x1F */
    'D', 'F', 'G', 'H', 'J', 'K', 'L', ':',             /* 0x20 - 0x27 */
    0x22, '~', 0x00, '|', 'Z', 'X', 'C', 'V',           /* 0x28 - 0x2F */
    'B', 'N', 'M', '<', '>', '?', 0x00, 0x00,           /* 0x30 - 0x37 */
    0x00, ' ', 0x00, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E,      /* 0x38 - 0x3F */
    0x8F, 0x90, 0x91, 0x92, 0x93, 0x00, 0x00, 0x80,     /* 0x40 - 0x47 */
    0x86, 0x84, '-', 0x89, '5', 0x88, '+', 0x83,        /* 0x48 - 0x4F */
    0x87, 0x85, 0x81, 0x82, 0x96, 0x97, 0x98, 0x94,     /* 0x50 - 0x57 */
    0x95, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,     /* 0x58 - 0x5F */
    0x00, 0x00, 0x81, 0x80, 0x84, 0x82, 0x83, 0x85,     /* 0x60 - 0x67 */
    0x86, 0x89, 0x87, 0x88, '/', 0x0D, 0x00, 0x00,      /* 0x68 - 0x6F */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,     /* 0x70 - 0x77 */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,     /* 0x78 - 0x7F */

    // Control
    0x00, 0x1B, '1', '2', '3', '4', '5', 0x1E,          /* 0x00 - 0x07 */
    '7', '8', '9', '0', 0x1F, '=', 0x08, 0x09,          /* 0x08 - 0x0F */
    0x11, 0x17, 0x05, 0x12, 0x14, 0x19, 0x15, 0x09,     /* 0x10 - 0x17 */
    0x0F, 0x10, 0x1B, 0x1D, 0x0A, 0x00, 0x01, 0x13,     /* 0x18 - 0x1F */
    0x04, 0x06, 0x07, 0x08, 0x0A, 0x0B, 0x0C, ';',      /* 0x20 - 0x27 */
    0x22, '`', 0x00, '\\', 0x1A, 0x18, 0x03, 0x16,      /* 0x28 - 0x2F */
    0x02, 0x0E, 0x0D, ',', '.', 0x1C, 0x00, 0x00,       /* 0x30 - 0x37 */
    0x00, ' ', 0x00, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E,      /* 0x38 - 0x3F */
    0x8F, 0x90, 0x91, 0x92, 0x93, 0x00, 0x00, 0x80,     /* 0x40 - 0x47 */
    0x86, 0x84, '-', 0x89, '5', 0x88, '+', 0x83,        /* 0x48 - 0x4F */
    0x87, 0x85, 0x81, 0x82, 0x96, 0x97, 0x98, 0x94,     /* 0x50 - 0x57 */
    0x95, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,     /* 0x58 - 0x5F */
    0x00, 0x00, 0x81, 0x80, 0x84, 0x82, 0x83, 0x85,     /* 0x60 - 0x67 */
    0x86, 0x89, 0x87, 0x88, '/', 0x0D, 0x00, 0x00,      /* 0x68 - 0x6F */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,     /* 0x70 - 0x77 */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,     /* 0x78 - 0x7F */


    // Control-Shift
    0x00, 0x1B, '!', '@', '#', '$', '%', '^',           /* 0x00 - 0x07 */
    '&', '*', '(', ')', '_', '+', 0x08, 0x09,           /* 0x08 - 0x0F */
    0x11, 0x17, 0x05, 0x12, 0x14, 0x19, 0x15, 0x09,     /* 0x10 - 0x17 */
    0x0F, 0x10, 0x1B, 0x1D, 0x0A, 0x00, 0x01, 0x13,     /* 0x18 - 0x1F */
    0x04, 0x06, 0x07, 0x08, 0x0A, 0x0B, 0x0C, ';',      /* 0x20 - 0x27 */
    0x22, '`', 0x00, '\\', 0x1A, 0x18, 0x03, 0x16,      /* 0x28 - 0x2F */
    0x02, 0x0E, 0x0D, ',', '.', 0x1C, 0x00, 0x00,       /* 0x30 - 0x37 */
    0x00, ' ', 0x00, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E,      /* 0x38 - 0x3F */
    0x8F, 0x90, 0x91, 0x92, 0x93, 0x00, 0x00, 0x80,     /* 0x40 - 0x47 */
    0x86, 0x84, '-', 0x89, '5', 0x88, '+', 0x83,        /* 0x48 - 0x4F */
    0x87, 0x85, 0x81, 0x82, 0x96, 0x97, 0x98, 0x94,     /* 0x50 - 0x57 */
    0x95, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,     /* 0x58 - 0x5F */
    0x00, 0x00, 0x81, 0x80, 0x84, 0x82, 0x83, 0x85,     /* 0x60 - 0x67 */
    0x86, 0x89, 0x87, 0x88, '/', 0x0D, 0x00, 0x00,      /* 0x68 - 0x6F */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,     /* 0x70 - 0x77 */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,     /* 0x78 - 0x7F */

    // Capslock
    0x00, 0x1B, '1', '2', '3', '4', '5', '6',           /* 0x00 - 0x07 */
    '7', '8', '9', '0', '-', '=', 0x08, 0x09,           /* 0x08 - 0x0F */
    'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I',             /* 0x10 - 0x17 */
    'O', 'P', '[', ']', 0x0D, 0x00, 'A', 'S',           /* 0x18 - 0x1F */
    'D', 'F', 'G', 'H', 'J', 'K', 'L', ';',             /* 0x20 - 0x27 */
    0x27, '`', 0x00, '\\', 'Z', 'X', 'C', 'V',          /* 0x28 - 0x2F */
    'B', 'N', 'M', ',', '.', '/', 0x00, 0x00,           /* 0x30 - 0x37 */
    0x00, ' ', 0x00, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E,      /* 0x38 - 0x3F */
    0x8F, 0x90, 0x91, 0x92, 0x93, 0x00, 0x00, '7',      /* 0x40 - 0x47 */
    '8', '9', '-', '4', '5', '6', '+', '1',             /* 0x48 - 0x4F */
    '2', '3', '0', '.', 0x96, 0x97, 0x98, 0x94,         /* 0x50 - 0x57 */
    0x95, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,     /* 0x58 - 0x5F */
    0x00, 0x00, 0x81, 0x80, 0x84, 0x82, 0x83, 0x85,     /* 0x60 - 0x67 */
    0x86, 0x89, 0x87, 0x88, '/', 0x0D, 0x00, 0x00,      /* 0x68 - 0x6F */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,     /* 0x70 - 0x77 */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,     /* 0x78 - 0x7F */

    // Caps-Shift
    0x00, 0x1B, '!', '@', '#', '$', '%', '^',           /* 0x00 - 0x07 */
    '&', '*', '(', ')', '_', '+', 0x08, 0x09,           /* 0x08 - 0x0F */
    'q', 'w', 'e', 'r', 't', 'y', 'u', 'i',             /* 0x10 - 0x17 */
    'o', 'p', '{', '}', 0x0A, 0x00, 'a', 's',           /* 0x18 - 0x1F */
    'd', 'f', 'g', 'h', 'j', 'k', 'l', ':',             /* 0x20 - 0x27 */
    0x22, 0x00, 0x00, 0x00, 'z', 'x', 'c', 'v',         /* 0x28 - 0x2F */
    'b', 'n', 'm', '<', '>', '?', 0x00, 0x00,           /* 0x30 - 0x37 */
    0x00, ' ', 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,      /* 0x38 - 0x3F */
    0x8F, 0x90, 0x91, 0x92, 0x93, 0x00, 0x00, '7',      /* 0x40 - 0x47 */
    '8', '9', '-', '4', '5', '6', '+', '1',             /* 0x48 - 0x4F */
    '2', '3', '0', '.', 0x96, 0x97, 0x98,  0x94,         /* 0x50 - 0x57 */
    0x95, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,     /* 0x58 - 0x5F */
    0x00, 0x00, 0x81, 0x80, 0x84, 0x82, 0x83, 0x85,     /* 0x60 - 0x67 */
    0x86, 0x89, 0x87, 0x88, '/', 0x0D, 0x00, 0x00,      /* 0x68 - 0x6F */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,     /* 0x70 - 0x77 */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,     /* 0x78 - 0x7F */

    // ALT
    0x00, 0x1B, '1', '2', '3', POUND, '5', '6',         /* 0x00 - 0x07 */
    '~', '`', '|', '\\', '-', '=', 0x08, 0x09,          /* 0x08 - 0x0F */
    'q', 'w', 'e', 'r', 't', 'y', 'u', 'i',             /* 0x10 - 0x17 */
    'o', 'p', '[', ']', 0x0D, 0x00, 'a', 's',           /* 0x18 - 0x1F */
    'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',             /* 0x20 - 0x27 */
    0x27, 0x00, 0x00, 0x00, 'z', 'x', 'c', 'v',         /* 0x28 - 0x2F */
    'b', 'n', 'm', ',', '.', '/', 0x00, '*',            /* 0x30 - 0x37 */
    0x00, ' ', 0x00, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E,      /* 0x38 - 0x3F */
    0x8F, 0x90, 0x91, 0x92, 0x93, 0x00, 0x00, 0x80,     /* 0x40 - 0x47 */
    0x86, 0x84, '-', 0x89, '5', 0x88, '+', 0x83,        /* 0x48 - 0x4F */
    0x87, 0x85, 0x81, 0x82, 0x96, 0x97, 0x98, 0x94,     /* 0x50 - 0x57 */
    0x95, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,     /* 0x58 - 0x5F */
    0x00, 0x00, 0x81, 0x80, 0x84, 0x82, 0x83, 0x85,     /* 0x60 - 0x67 */
    0x86, 0x89, 0x87, 0x88, '/', 0x0D, 0x00, 0x00,      /* 0x68 - 0x6F */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,     /* 0x70 - 0x77 */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,     /* 0x78 - 0x7F */
};

//
// Driver variables
//

static uint16_t kbd_stat[KBD_MATRIX_SIZE];
static short counter = 0;
static uint8_t last_press = 0;
static t_word_ring scan_code_buffer;
static t_word_ring char_buffer; 
static uint8_t modifiers = 0;
static bool break_pressed = 0;

/* Scan code to character lookup tables */

static char keys_unmodified[128];
static char keys_shift[128];
static char keys_control[128];
static char keys_control_shift[128];
static char keys_caps[128];
static char keys_caps_shift[128];
static char keys_alt[128];

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

/*
 * Catch special keys and convert them to their ANSI terminal codes
 *
 * Characters 0x80 - 0x98 are reserved for function keys, arrow keys, etc.
 * This function maps them to the ANSI escape codes
 *
 * Inputs:
 * modifiers = the current modifier bit flags (ALT, CTRL, META, etc)
 * c = the character found from the scan code.
 */
static unsigned char kbd_to_ansi(unsigned char modifiers, unsigned char c) {
    if ((c >= 0x80) && (c <= 0x98)) {
        /* The key is a function key or a special control key */
        const char * ansi_key = ansi_keys[c - 0x80];
        const char * sequence;
        short modifiers_after = 0;

        // Figure out if the modifiers come before or after the sequence code
        if (isdigit(ansi_key[0])) {
            // Sequence is numeric, modifiers come after
            modifiers_after = 1;
        }

        // After ESC, all sequences have [
        rb_word_put(&char_buffer, '[');

        if (modifiers_after) {
            // Sequence is numberic, get the expanded sequence and put it in the queue
            for (sequence = ansi_keys[c - 0x80]; *sequence != 0; sequence++) {
                rb_word_put(&char_buffer, *sequence);
            }
        }

        // Check to see if we need to send a modifier sequence
        if (modifiers & (KBD_MOD_SHIFT | KBD_MOD_CTRL | KBD_MOD_ALT | KBD_MOD_OS)) {
            unsigned char code_bcd;
            short modifier_code = 0;
            short i;

            if (modifiers_after) {
                // Sequence is numeric, so put modifiers after the sequence and a semicolon
                rb_word_put(&char_buffer, ';');
            }

            modifier_code = ((modifiers >> 3) & 0x1F) + 1;
            code_bcd = i_to_bcd(modifier_code);

            if (code_bcd & 0xF0) {
                rb_word_put(&char_buffer, ((code_bcd & 0xF0) >> 4) + '0');
            }
            rb_word_put(&char_buffer, (code_bcd & 0x0F) + '0');
        }

        if (!modifiers_after) {
            // Sequence is a letter code
            rb_word_put(&char_buffer, ansi_key[0]);
        } else {
            // Sequence is numeric, close it with a tilda
            rb_word_put(&char_buffer, '~');
        }

        return 0x1B;    /* Start the sequence with an escape */

    } else if (c == 0x1B) {
        /* ESC should be doubled, to distinguish from the start of an escape sequence */
        rb_word_put(&char_buffer, 0x1B);
        return c;

    } else {
        /* Not a special key: return the character unmodified */

        return c;
    }
}

/*
 * Try to get a character from the keyboard...
 *
 * Returns:
 *      the next character to be read from the keyboard (0 if none available)
 */
char kbd_getc() {
    if (!rb_word_empty(&char_buffer)) {
        // If there is a character waiting in the character buffer, return it...
        return (char)rb_word_get(&char_buffer);

    } else {
        // Otherwise, we need to check the scan code queue...
        unsigned short raw_code = kbd_get_scancode();
        while (raw_code != 0) {
            if ((raw_code & 0x80) == 0) {
                // If it's a make code, let's try to look it up...
                unsigned char modifiers = (raw_code >> 8) & 0xff;    // Get the modifiers
                unsigned char scan_code = raw_code & 0x7f;           // Get the base code for the key

                // Check the modifiers to see what we should lookup...

                if ((modifiers & (KBD_MOD_ALT | KBD_MOD_SHIFT | KBD_MOD_CTRL | KBD_LOCK_CAPS)) == 0) {
                    // No modifiers... just return the base character
                    return kbd_to_ansi(modifiers, keys_unmodified[scan_code]);

                } else if (modifiers & KBD_MOD_ALT) {
                    return kbd_to_ansi(modifiers, keys_alt[scan_code]);

                } else if (modifiers & KBD_MOD_CTRL) {
                    // If CTRL is pressed...
                    if (modifiers & KBD_MOD_SHIFT) {
                        // If SHIFT is also pressed, return CTRL-SHIFT form
                        return kbd_to_ansi(modifiers, keys_control_shift[scan_code]);

                    } else {
                        // Otherwise, return just CTRL form
                        return kbd_to_ansi(modifiers, keys_control[scan_code]);
                    }

                } else if (modifiers & KBD_LOCK_CAPS) {
                    // If CAPS is locked...
                    if (modifiers & KBD_MOD_SHIFT) {
                        // If SHIFT is also pressed, return CAPS-SHIFT form
                        return kbd_to_ansi(modifiers, keys_caps_shift[scan_code]);

                    } else {
                        // Otherwise, return just CAPS form
                        return kbd_to_ansi(modifiers, keys_caps[scan_code]);
                    }
                } else {
                    // SHIFT is pressed, return SHIFT form
                    return kbd_to_ansi(modifiers, keys_shift[scan_code]);
                }
            }

            // If we reach this point, it wasn't a useful scan-code...
            // So try to fetch another
            raw_code = kbd_get_scancode();
        }

        // If we reach this point, there are no useful scan codes
        return 0;
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
 * Set the keyboard translation tables
 *
 * The translation tables provided to the keyboard consist of eight
 * consecutive tables of 128 characters each. Each table maps from
 * the MAKE scan code of a key to its appropriate 8-bit character code.
 *
 * The tables included must include, in order:
 * - UNMODIFIED: Used when no modifier keys are pressed or active
 * - SHIFT: Used when the SHIFT modifier is pressed
 * - CTRL: Used when the CTRL modifier is pressed
 * - CTRL-SHIFT: Used when both CTRL and SHIFT are pressed
 * - CAPSLOCK: Used when CAPSLOCK is down but SHIFT is not pressed
 * - CAPSLOCK-SHIFT: Used when CAPSLOCK is down and SHIFT is pressed
 * - ALT: Used when only ALT is presse
 * - ALT-SHIFT: Used when ALT is pressed and either CAPSLOCK is down
 *   or SHIFT is pressed (but not both)
 *
 * Inputs:
 * tables = pointer to the keyboard translation tables
 */
short SYSTEMCALL kbd_layout(const char * tables) {
    short i;

    for (i = 0; i < 128; i++) {
        keys_unmodified[i] = tables[i];
        keys_shift[i] = tables[i + 128];
        keys_control[i] = tables[i + 256];
        // if (keys_control[i] == 0x03) {
        //     // We have set the scan code for CTRL-C?
        //     g_kbdmo_break_sc = i;
        // }
        // Check for CTRL-C
        keys_control_shift[i] = tables[i + 384];
        keys_caps[i] = tables[i + 512];
        keys_caps_shift[i] = tables[i + 640];
        keys_alt[i] = tables[i + 768];
    }

    return 0;
}

/*
 * Initialize the matrix keyboard
 *
 */
short kbd_init() {
	// Initialize VIA0 -- we'll just read from PB7
	via0->ddra = 0x00;
	via0->ddrb = 0x00;
	via0->acr = 0x00;
	via0->pcr = 0x00;
	via0->ier = 0x00;

	// Initialize VIA1 -- we'll write to all of PB
	via1->ddra = 0xff;
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

	// Set up the layout of the F256k keyboard
	kbd_layout(kbd_256k_layout);

	int_register(INT_VIA0, kbd_handle_irq);

	via0->acr = 0x40;								// Timer #0 in free running mode
	via0->ier = VIA_INT_TIMER1 | VIA_INT_IRQ;		// Allow timer #0 interrupts
	via0->t1c_l = 0xff;								// Set timer count
	via0->t1c_h = 0xff;

	int_enable(INT_VIA0);

	return 1;
}
