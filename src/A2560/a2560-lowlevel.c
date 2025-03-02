/**
 * Low level functions for the A2560 machines
 */

#include <stdint.h>

#define VKY_TXT_CHAR_A	((volatile uint8_t *)0xfeca0000)
#define VKY_TXT_COLOR_A	((volatile uint8_t *)0xfeca8000)

uint32_t __low_level_init() {
	// Return the code to make sure we stay in supervisor mode (Calypsi-dependent)
	return 1;
}

/**
 * Handler for the bus exception
 */
__attribute__((interrupt(0x0008))) void bus_error() {
	VKY_TXT_CHAR_A[0] = 'B';
	VKY_TXT_COLOR_A[0] = 0xf0;
	VKY_TXT_CHAR_A[1] = '!';
	VKY_TXT_COLOR_A[1] = 0xf0;

	while(1) ;
}

/**
 * Handler for the addresss exception
 */
__attribute__((interrupt(0x0008))) void address_error() {
	VKY_TXT_CHAR_A[0] = 'A';
	VKY_TXT_COLOR_A[0] = 0xf0;
	VKY_TXT_CHAR_A[1] = '!';
	VKY_TXT_COLOR_A[1] = 0xf0;

	while(1) ;
}

/**
 * Handler for the illegal instruction exception
 */
__attribute__((interrupt(0x0010))) void illegal() {
	VKY_TXT_CHAR_A[0] = 'I';
	VKY_TXT_COLOR_A[0] = 0xf0;
	VKY_TXT_CHAR_A[1] = '!';
	VKY_TXT_COLOR_A[1] = 0xf0;

	while(1) ;
}

