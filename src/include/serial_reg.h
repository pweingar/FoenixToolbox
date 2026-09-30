/**
 * Register definitions for the common serial devices
 */

#ifndef __serial_reg__
#define __serial_reg__

#include "sys_general.h"

#define SER_RXD_SPEED       0x01
#define SER_RXD_FIFO_EMPTY  0x02
#define SER_TXD_FIFO_EMPTY  0x04

#if MODEL == MODEL_FOENIX_FA2560K2
#include "FA2560K2/serial_fa2560k2.h"

#elif MODEL == MODEL_FOENIX_A2560ME
#include "A2560Me/serial_a2560me.h"

#endif

#endif