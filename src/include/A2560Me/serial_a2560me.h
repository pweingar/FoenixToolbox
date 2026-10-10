/**
 * Register definitions for the common serial devices
 */

#ifndef __serial_me__
#define __serial_me__

#include "sys_general.h"

#define SER_CONTROL         0
#define SER_DATA            1
#define SER_RXD_COUNT       2
#define SER_TXD_COUNT       4

/**
 * Structure representing the common serial registers
 */
typedef struct com_ser_dev_s {
    uint8_t control;
    uint8_t data;
    uint16_t rxd_fifo_count;
    uint16_t txd_fifo_count;
} com_ser_dev_t, *com_ser_dev_p;

/**
 * A2560Me has a two USB UARTs for serial communications and a WizFi wireless network adapter
 */

/**
 * COM3 and 4 -- USB serial ports for serial communications to a host computer
 */
#define SER_COM3            ((uint8_t *)0xfec00b00)
#define SER_COM4            ((uint8_t *)0xfec00c00)

/**
 * WIZFI wireless adapter -- This will be mapped to CDEV_WIZFI
 */
#define SER_WIZFI           ((uint8_t *)0xffc00800)

#endif