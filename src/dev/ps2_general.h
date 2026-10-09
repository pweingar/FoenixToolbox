/**
 * General driver support for PS/2 keyboard and mouse
 */

#ifndef __ps2_general__
#define __ps2_general__

#include <stdbool.h>
#include <stdint.h>

#define PS2_OK              0           // PS/2 function completed successfully
#define PS2_TIMEOUT         -1          // PS/2 function hit the timeout limit
#define PS2_ERR_CONTROLLER  -2          // PS/2 controller did not respond to initialization
#define PS2_NO_DEVICE       -3          // PS/2 port device did not respond to identification
#define PS2_SELF_TEST_FAIL  -4          // PS/2 device failed its self-test
#define PS2_CMD_ERROR       -5          // PS/2 device did not accept command

/**
 * Initialize the PS/2 interface
 * 
 * This function will test the PS/2 controller (if present), check to see how many ports are present, and
 * test devices on each port.
 * 
 * @return 0 on success, negative number on error
 */
extern int ps2_init();

/**
 * Send a byte to the command channel (controller)
 * 
 * @param b the byte to send 
 * @param timeout the number of jiffies to wait for a timeout (0 is wait forever)
 * @return status (0 = success, negative number is an error)
 */
extern int ps2_send_cmd(uint8_t b, long timeout);

/**
 * Send a data byte to the given port
 * 
 * @param port the port to send the byte to (0 or 1)
 * @param b the byte to send
 * @param timeout the number of jiffies to wait for a timeout (0 is wait forever)
 * @return status (0 = success, negative number is an error)
 */
extern int ps2_send_data(uint8_t port, uint8_t b, long timeout);

/**
 * Check to see if there is data waiting to be read from the PS/2 controller data buffer
 * 
 * @return true if ps2_read_data would return data, false otherwise
 */
extern bool ps2_has_data();

/**
 * Wait for data on the input port and return it.
 * 
 * @param timeout the number of jiffies to wait for a timeout (0 is wait forever)
 * @return the byte received (if >= 0) or an error code if negative
 */
extern int ps2_read_data(long timeout);

/**
 * Return the ID of the device on the given port as determined during initialization.
 * 
 * @param port the number of the port to check (0 or 1)
 * @return the ID of the device (0xffff means there was no device on that port, or the port is not supported)
 */
extern uint16_t ps2_device_id(uint8_t port);

/**
 * Check to see if there was a device on the given port
 * 
 * @param port the number of the port (0 or 1)
 * @return true if there was a device there when we initialized
 */
extern bool ps2_has_device(uint8_t port);

/**
 * Wait for data on the input port and return it.
 * 
 * NOTE: timeout should be > 0 for non-interrupt use. For responding to interrupts,
 *       timeout should be set to 0.
 * 
 * @param timeout the number of jiffies to wait for a timeout (0 is return immediately)
 * @return the byte received (if >= 0) or an error code if negative
 */
extern int ps2_read_data(long timeout);

/**
 * Clear out the FIFO for the keyboard
 */
extern void kbd_clear_fifo();

/**
 * Send a command to the keyboard
 * 
 * @param cmd the command to send
 * @return 0 on success, any other number is an error
 */
extern short kbd_send_cmd(uint8_t cmd);

/**
 * @brief Handle an IRQ to query the keyboard
 * 
 */
extern void kbd_handle_irq();

/**
 * Send a command to the mouse
 * 
 * @param cmd the command to send
 * @return 0 on success, any other number is an error
 */
extern short mouse_send_cmd(uint8_t cmd);

/**
 * @brief Handle an IRQ to query the mouse
 * 
 */
extern void mouse_handle_irq();

#endif