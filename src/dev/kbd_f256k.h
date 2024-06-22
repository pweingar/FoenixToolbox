/**
 * @file kbd_f256k.h
 * @author your name (you@domain.com)
 * @brief Driver for the F256K and F256K2 matrix keyboard
 * @version 0.1
 * @date 2024-06-17
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef __kbd_f256k_h__
#define __kbd_f256k_h__

/**
 * @brief Handle an IRQ to query the keyboard
 * 
 */
extern void kbd_handle_irq();

/*
 * Initialize the matrix keyboard
 *
 */
extern short kbd_init();

#endif