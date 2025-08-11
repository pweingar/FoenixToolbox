/**
 * Register definitions for the common serial devices
 */

#ifndef __serial_reg__
#define __serial_reg__

#include "sys_general.h"

//
// Offsets within a uint8_t array to the common serial registers
//

#define SER_CONTROL         0
#define SER_DATA            2
#define SER_RXD_COUNT       4
#define SER_TXD_COUNT       6

#define SER_RXD_SPEED       0x01
#define SER_RXD_FIFO_EMPTY  0x02
#define SER_TXD_FIFO_EMPTY  0x04

#if MODEL == FOENIX_MODEL_FA2560K2

/**
 * COM1 -- First USB serial port for serial communications to a host computer
 */
#define SER_COM1            ((uint8_t *)0xffb18000)

#endif

#endif