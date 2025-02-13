/**
 * Low level functions for the A2560 machines
 */

#include <stdint.h>

#define VKY_TXT_CHAR_A	((volatile uint8_t *)0xfeca0000)
#define VKY_TXT_COLOR_A	((volatile uint8_t *)0xfeca8000)

// uint32_t __low_level_init() {
// 	// Return the code to make sure we stay in supervisor mode (Calypsi-dependent)
// 	return 1;
// }

/**
 * Handler for the illegal instruction exception
 */
__attribute__((interrupt(0x0010))) void illegal() {
	VKY_TXT_CHAR_A[10] = '!';
	VKY_TXT_COLOR_A[10] = 0xf0;

	while(1) ;
}