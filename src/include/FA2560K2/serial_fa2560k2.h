/**
 * Register definitions for the common serial devices
 */

#ifndef __serial_fa2560k2__
#define __serial_fa2560k2__

#include "sys_general.h"

#define SER_CONTROL         0
#define SER_DATA            2
#define SER_RXD_COUNT       4
#define SER_TXD_COUNT       6

/**
 * Structure representing the common serial registers
 */
typedef struct com_ser_dev_s {
    uint8_t control;
    uint8_t reserved_1;
    uint8_t data;
    uint8_t reserved_2;
    uint16_t rxd_fifo_count;
    uint16_t txd_fifo_count;
} com_ser_dev_t, *com_ser_dev_p;

/**
 * FA2560K2 has a single USB UART for serial communications and a WizFi wireless network adapter
 */

/**
 * COM1 -- First USB serial port for serial communications to a host computer
 */
#define SER_COM1            ((uint8_t *)0xffb18000)

/**
 * WIZFI wireless adapter -- This will be mapped to CDEV_WIZFI
 */
#define SER_WIZFI           ((uint8_t *)0xffb16000)

#endif