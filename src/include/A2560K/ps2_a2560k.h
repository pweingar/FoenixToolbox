#ifndef __PS2_A2560K_H
#define __PS2_A2560K_H

#include <stdint.h>

/*
 * Ports for the PS/2 keyboard and mouse on the A2560K
 */

#define PS2_STATUS          ((volatile uint8_t *)0xFEC02064)
#define PS2_CMD_BUF         ((volatile uint8_t *)0xFEC02064)
#define PS2_OUT_BUF         ((volatile uint8_t *)0xFEC02060)
#define PS2_INPT_BUF        ((volatile uint8_t *)0xFEC02060)
#define PS2_DATA_BUF        ((volatile uint8_t *)0xFEC02060)

#endif
