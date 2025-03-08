/**
 * Simple test routines... can be removed when everything is up and running
 */

#ifndef __tests__
#define __tests__

/**
 * Make sure we can read from the PATA hard drive
 */
extern void test_hd();

/**
 * Make sure we can read from the external SD card
 */
extern void test_sd0();

/**
 * Try to read and print a directory
 */
extern void test_dir(const char * path);

/**
 * Test the A2560K built-in keyboard
 */
extern void test_kbd_mo();

/**
 * Write a character to the screen directly
 */
extern void vky_txt_emit(char c);

#endif