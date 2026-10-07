/**
 * Code for PS/2 support on the F256 family
 */

#include <stdbool.h>

#include "errors.h"
#include "interrupt.h"
#include "log.h"
#include "ps2_reg.h"
#include "ring_buffer.h"
#include "sys_macros.h"
#include "timers.h"

//
// Constants
//

#define KBD_TIMEOUT			(60 * 5)
#define KBD_RETRIES			10

/**
 * Clear out the FIFO for the keyboard
 */
static void kbd_clear_fifo() {
	*PS2_CTRL |= PS2_CTRL_KBD_CLR;
	*PS2_CTRL &= ~PS2_CTRL_KBD_CLR;
}

/**
 * Send a command to the keyboard
 * 
 * @param cmd the command to send
 * @return 0 on success, any other number is an error
 */
static short kbd_send_cmd(uint8_t byte) {
	uint8_t status = 0;

	*PS2_OUT = byte;
	*PS2_CTRL |= PS2_CTRL_KBD_WR;

	long timeout = timers_jiffies() + KBD_TIMEOUT;

	do {
		if (timeout < timers_jiffies()) {
			*PS2_CTRL &= ~PS2_CTRL_KBD_WR;
			return DEV_TIMEOUT;
		}
		status = *PS2_STAT;
	} while ((status & (PS2_STAT_KBD_ACK | PS2_STAT_KBD_NAK)) == 0);

	*PS2_CTRL &= ~PS2_CTRL_KBD_WR;

	return 0;
}

/**
 * Wait for data on the input port and return it.
 * 
 * NOTE: timeout should be > 0 for non-interrupt use. For responding to interrupts,
 *       timeout should be set to 0.
 * 
 * @param timeout the number of jiffies to wait for a timeout (0 is return immediately)
 * @return the byte received (if >= 0) or an error code if negative
 */
int ps2_read_data(long timeout) {
    // Check to see if there is a keyboard bytecode waiting... process it if so
	while ((*PS2_STAT & PS2_STAT_KBD_EMP) == 0) {
		kbd_process_set2_bytecode(*PS2_KBD_IN);
	}
}