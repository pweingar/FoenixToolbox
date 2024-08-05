/**
 * @file iec_port.c
 * @author your name (you@domain.com)
 * @brief Implement the low level control code for the IEC port on the F256
 * @version 0.1
 * @date 2024-07-19
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "iecll.h"
#include "dev/txt_screen.h"

short iec_open(uint8_t device, uint8_t channel, bool is_write, char * command) {
	return -1;
}

void iec_close(uint8_t device, uint8_t channel)  {
	iecll_listen(device);
	iecll_listen_sa(0xe0 | (channel & 0x0f));
	iecll_unlisten();
}

void iec_print(uint8_t device, char * message) {
	// Open the device, channel 1
	uint8_t channel = 1;
	iecll_listen(device);
	iecll_listen_sa(0x60 | channel);

	for (char *x = message; *x; x++) {
		iecll_out(*x);
	}

	iecll_unlisten();
	iec_close(4, channel);
}

/**
 * @brief Retrieve the raw status string from an IEC device
 * 
 * @param device number of the IEC device to query
 * @param buffer character buffer in which to write the status
 * @param count the maximum number of bytes to fetch
 * @return the number of bytes read from the device
 */
short iec_status(uint8_t device, char * buffer, short count) {
	short i = 0;

	// Zero out the buffer
	memset((void *)buffer, 0, count);

	// Open the device, channel 15
	txt_put(0, '-');
	iecll_talk(device);
	txt_put(0, '.');
	iecll_talk_sa(0x6f);
	txt_put(0, '.');

	// Copy the status bytes
	for (i = 0; i < count; i++) {
		uint8_t byte = iecll_in();
		txt_put(0, (char)byte);
		buffer[i] = (char)byte;
		if (iecll_eoi()) {
			break;
		}
	}
	buffer[i+1] = 0;

	iecll_untalk();

	return i;
}

/**
 * @brief Initialize the IEC port
 * 
 * @return short 0 on success, a negative number indicates an error
 */
short iec_init() {
	short result = iecll_ioinit();

	return result;
}