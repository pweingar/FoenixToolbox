/**
 * @file kbd_f256.h
 * @author your name (you@domain.com)
 * @brief Common code for F256 keyboards to convert scancodes to characters
 * @version 0.1
 * @date 2024-07-12
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef __kbd_f256_h__
#define __kbd_f256_h__

#include "sys_general.h"

#if MODEL == MODEL_FOENIX_F256 || MODEL == MODEL_FOENIX_F256JR2
#include "kbd_f256jr.h"
#elif MODEL == MODEL_FOENIX_F256K || MODEL == MODEL_FOENIX_F256K2 || MODEL == MODEL_FOENIX_FA2560K2
#include "kbd_f256k.h"
#endif

#include "sys_macros.h"

/*
 * Try to get a character from the keyboard...
 *
 * Returns:
 *      the next character to be read from the keyboard (0 if none available)
 */
extern char kbd_getc();

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
extern SYSTEMCALL short kbd_layout(const char * tables);

/**
 * @brief Initialize the keyboard scancode to character converter.
 *
 */
extern void kbd_init();

#endif
