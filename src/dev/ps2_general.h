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
 * Wait for data on the input port and return it.
 * 
 * @param timeout the number of jiffies to wait for a timeout (0 is wait forever)
 * @return the byte received (if >= 0) or an error code if negative
 */
extern int ps2_read_data(long timeout);

#endif