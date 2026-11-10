/**
 * @file kbd_ps2.h
 * @author your name (you@domain.com)
 * @brief Driver for the PS/2 keyboard
 * @version 0.1
 * @date 2024-06-17
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef __kbd_ps2__
#define __kbd_ps2__

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

/**
 * Enable to disable keyboard scanning
 * 
 * @param is_enabled true if scanning should be enabled false otherwise
 * @return 0 on success, negative number on error
 */
extern short kbd_enable(bool is_enabled);

/**
 * Set the status of the keyboad status LEDS (caps, scroll, and numlock)
 * 
 * status is a bitfield:
 * - 0x01 = SCROLL LOCK
 * - 0x02 = NUM LOCK
 * - 0x04 = CAPS LOCK
 * 
 * NOTE: keyboard scanning should be turned off before calling this
 * 
 * @param status a bitfield describing which LEDs should be on
 * @return 0 on success, negative number on error
 */
extern short kbd_set_status(uint8_t status);

#endif