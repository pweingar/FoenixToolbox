/**
 * @file kbd_opt_fa2560k.h
 * @brief Define the optical keyboard registers
 * @version 0.1
 * @date 2024-09-15
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef __kbd_opt_fa2560k_h__
#define __kbd_opt_fa2560k_h__

#include <stdint.h>

/**
 * @brief Structure representing the FA2560K optical keyboard
 *
 * NOTE: data bytes are read consecutively and represent 16-bit words.
 *       The 4 most significant bits encode the row number (0 - 7).
 *       The 9 least significant bits contain the column bitfield (a 1
 *       indicates the key at that column and row is currently pressed).
 *
 */
typedef struct s_kbd_optical {
	uint16_t data;					// Data bytes to read from the keyboard
	uint16_t status;				// Indicate keyboard type and if the buffer is empty
	uint16_t count;					// Number of bytes in the keyboard buffer
} t_kbd_optical, *p_kbd_optical;

/*
 * Status bit flags
 */
#define KBD_OPT_STAT_MECH	0x80
#define KBD_OPT_STAT_EMPTY	0x01

#define KBD_OPTICAL ((volatile p_kbd_optical)0xffb1e000)

#endif
