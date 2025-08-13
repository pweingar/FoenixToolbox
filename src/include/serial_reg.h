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

// #if MODEL == FOENIX_MODEL_FA2560K2

typedef struct com_ser_dev_s {
    uint8_t control;
    uint8_t reserved_1;
    uint8_t data;
    uint8_t reserved_2;
    uint16_t rxd_fifo_count;
    uint16_t txd_fifo_count;
} com_ser_dev_t, *com_ser_dev_p;

/**
 * COM1 -- First USB serial port for serial communications to a host computer
 */
#define SER_COM1            ((uint8_t *)0xffb18000)

/**
 * WIZFI wireless adapter -- This will be mapped to COM2
 */
#define SER_WIZFI           ((uint8_t *)0xffb16000)

// #endif

#endif