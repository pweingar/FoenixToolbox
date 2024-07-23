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

#include <stdint.h>
#include <calypsi/intrinsics65816.h>

#include "F256/iec_f256.h"
#include "errors.h"

/**
 * @brief Wait for 20 microseconds
 * 
 */
static void sleep_20us() {
	__asm(
	    "   phx"
        "   ldx ##20"
		"1$ dex"
        "   bne 1$"
        "   plx"
	);
}

/**
 * @brief Wait for 100 microseconds
 * 
 */
static void sleep_100us() {
	__asm(
		"   phx"
		"   ldx #5"
		"1$ jsl sleep_20us"
		"   dex"
		"   bne 1$"
		"   plx"
	);
}

/**
 * @brief Wait for 300 microseconds
 * 
 */
static void sleep_300us() {
	sleep_100us();
	sleep_100us();
	sleep_100us();
}

/**
 * @brief Wait for 1 millisecond
 * 
 */
static void sleep_1ms() {
	sleep_300us();
	sleep_300us();
	sleep_300us();
}

/**
 * @brief Assert pins on the IEC port
 * 
 * @param pins mask bits of the pins to assert
 */
static void iec_assert(uint8_t pins) {
	*IEC_OUT &= ~pins;
}

/**
 * @brief Release pins on the IEC port
 * 
 * @param pins mask bits of the pins to release
 */
static void iec_release(uint8_t pins) {
	*IEC_OUT |= pins;
}

/**
 * @brief Assert and release pins in one transaction
 * 
 * @param assert_pins mask bits of pins to assert
 * @param release_pins mask bits of pins to release
 */
static void iec_assert_release(uint8_t assert_pins, uint8_t release_pins) {
	*IEC_OUT = (*IEC_OUT & ~assert_pins) | release_pins;
}

/**
 * @brief Read a pin on the IEC port, given its mask bit
 * 
 * @param pin mask bit of the pin to read
 * @return uint8_t 0 = pin released, 1 = pin asserted
 */
static uint8_t iec_read(uint8_t pin) {
	uint8_t v1 = *IEC_IN;
	uint8_t v2 = *IEC_IN;
	while (v1 != v2)  {
		v1 = v2;
		v2 = *IEC_IN;
	}
	return ((v2 & pin == 0) ? 1 : 0);
}

static short iec_send_common(uint8_t data, bool eoi) {
	sleep_300ms();

	iec_release(IEC_PIN_CLK | IEC_PIN_DATA);

	// Wait for the DATA line to be released by all the listeners
	// NOTE: we may need to use Gadget's clever DATA hack
	do {
		sleep_20us();
	} while(iec_read(IEC_PIN_DATA));

	if (eoi) {
		// If EOI, we wait until the listener ACKs the EOI
		while (!iec_read(IEC_PIN_DATA)) ;

		// Thgen wait for the DATA 
		do {
			sleep_20us();
		} while(iec_read(IEC_PIN_DATA));
	}

	// Actually send the bits...

	sleep_20us();
	for (short bit = 0; bit < 8; bit++) {
		iec_assert(IEC_PIN_CLOCK);
		if (data & 0x01) {
			iec_assert(IEC_PIN_DATA);
		} else {
			iec_release(IEC_PIN_DATA);
		}
		data = data >> 1;

		sleep_20us();
		sleep_20us();
		iec_release(IEC_PIN_CLOCK);
		sleep_20us();
	}

	iec_release(IEC_PIN_DATA);
	iec_assert(IEC_PIN_CLK);

	for (short count = 0; count < 50; count++) {
		if (iec_read(IEC_PIN_DATA)) {
			break;
		}
		sleep_20us();
	}

	return 0;
}

static short iec_atn_common(uint8_t data) {
	__interrupt_state_t state = __get_interrupt_state();
	__disable_interrupts;

	iec_assert_release((IEC_PIN_ATN | IEC_PIN_CLK), IEC_PIN_DATA);
	sleep_1ms();

	if (iec_read(IEC_PIN_DATA)) {
		iec_release(IEC_PIN_ATN);
		__restore_interrupt_state(state);
		return ERR_GENERAL;
	}

	short result = iec_send_common(data, false);
	__restore_interrupt_state(state);
	return result;
}

static short iec_atn_release(uint8_t data) {
	iec_atn_common(data);
	iec_release(IEC_PIN_ATN);
	sleep_20us();
	sleep_20us();
	sleep_20us();
	iec_release(IEC_PIN_CLK | IEC_PIN_DATA);
}

static uint8_t iec_read_b() {
	bool is_eoi = false;
	while(iec_read(IEC_PIN_CLK)) {
		// TODO: detect EOI here
		;
	}

	iec_release(IEC_PIN_DATA);

	while(iec_read(IEC_PIN_CLK)) ;

	uint8_t data = 0;
	for (short count = 0; count < 8; count++) {
		data = data >> 1;

		while(iec_read(IEC_PIN_CLK)) ;
		
		if (iec_read(IEC_PIN_DATA)) {
			data |= 0x80;
		}

		while(!iec_read(IEC_PIN_CLK)) ;
	}

	iec_assert(IEC_PIN_DATA);
	sleep_20us();
	iec_release(IEC_PIN_DATA);

	if (eoi) {
		// We have the last byte, wait for the talker to turn around the bus
		while (iec_read(IEC_PIN_CLK)) ;

		// Take back control of the bus
		iec_assert(IEC_PIN_CLK);
	}

	return data;
}

/**
 * @brief Send the TALK command to an IEC device
 * 
 * @param device the number of the IEC device to make a talker
 */
void iec_talk(uint8_t device) {
	return iec_atn_common(IEC_CMD_TALK | (device & 0x0f));
}

/**
 * @brief Send the secondary address for the talk command
 * 
 * @param channel the channel for the secondary address
 */
static void iec_talk_sa(uint8_t channel) {
	iec_atn_release_release(channel & 0x0f);

	// Turn around control of the bus to the talker
	iec_assert(IEC_PIN_DATA);
	iec_release(IEC_PIN_CLK);

	// Wait for acknowledgement that we have a talker
	while(!iec_read(IEC_PIN_CLK)) ;
}

/**
 * @brief Send the UNTALK command
 * 
 */
void iec_untalk() {
	iec_atn_release(IEC_CMD_UNTALK);
}

/**
 * @brief Send the UNLISTEN command
 * 
 */
static void iec_unlisten() {
	iec_atn_release(IEC_CMD_UNLISTEN);
}

/**
 * @brief Retrieve the raw status string from an IEC device
 * 
 * @param device number of the IEC device to query
 * @param buffer character buffer in which to write the status
 * @param count the maximum number of bytes to fetch
 */
void iec_status(uint8_t device, char * buffer, short count) {
	iec_talk(device);
	iec_talk_sa(IEC_CMD_OPENCH | 0x0f);

	// TODO: handle the EOI

	for (short i = 0; i < count; i++) {
		char c = iec_read_b();
		buffer[i] = c;
	}

	iec_untalk();
}

/**
 * @brief Initialize the IEC port
 * 
 * @return short 0 on success, a negative number indicates an error
 */
short iec_init() {
	iec_assert_release(IEC_PIN_CLK, (IEC_PIN_ATN | IEC_PIN_DATA | IEC_PIN_SREQ));
	sleep_1ms();
	sleep_1ms();
	sleep_1ms();

	if (iec_read(IEC_PIN_ATN) || iec_read(IEC_PIN_SREQ)) {
		return ERR_NOT_READY;
	} else {
		return 0;
	}
}