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
 * Write a character to the screen directly
 */
extern void vky_txt_emit(char c);

#endif