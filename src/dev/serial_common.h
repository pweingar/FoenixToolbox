/**
 * Common interface code for the internal serial interfaces on the F-series machines
 * 
 * This interface model is used by the USB serial ports, the MIDI chips, and the network interfaces
 */

#ifndef __serial_common__
#define __serial_common__

#include <stdint.h>

/**
 * Initialize the common serial device system
 */
extern void ser_init();

/**
 * Install a common serial interface device as a channel device
 * 
 * @param port pointer to the first register of the common serial device
 * @param cdev_number the channel device number that should be registered for this device
 * @param name the user-readable name for the channel device (e.g. "COM1")
 * @return 0 on success, negative number on failure
 */
extern short ser_install(uint8_t * port, short cdev_number, char * name);

/**
 * Install all the standard common serial interface devices on this machine
 * 
 * @return 0 on success, negative number on failure
 */
extern short ser_install_all();

#endif