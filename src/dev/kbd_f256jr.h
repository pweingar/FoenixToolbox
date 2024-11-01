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

#include <stdbool.h>
#include "sys_macros.h"

/**
 * @brief Handle an IRQ to query the keyboard
 * 
 */
extern SYSTEMCALL void kbd_handle_irq();

/*
 * Try to retrieve the next scancode from the keyboard.
 *
 * Returns:
 *      The next scancode to be processed, 0 if nothing.
 */
extern unsigned short kbd_get_scancode();

/*
 * Check to see if a BREAK code has been pressed recently
 * If so, return true and reset the internal flag.
 *
 * BREAK will be RUN/STOP or CTRL-C on the F256K
 *
 * Returns:
 * true if a BREAK has been pressed since the last check
 */
extern bool kbd_break();

/*
 * Initialize the matrix keyboard
 *
 */
extern short kbd_sc_init();

#endif