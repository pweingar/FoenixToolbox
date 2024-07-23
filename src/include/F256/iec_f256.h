/**
 * Define the registers for the IEC (Commodore Serial) interface
 */

#ifndef __iec_f256_h__
#define __iec_f256_h__

#include <stdint.h>

#define IEC_CMD_LISTEN		0x20
#define IEC_CMD_TALK		0x40
#define IEC_CMD_OPENCH		0x60
#define IEC_CMD_CLOSE		0xE0
#define IEC_CMD_OPEN		0xF0
#define IEC_CMD_UNLISTEN	0x3F
#define IEC_CMD_UNTALK		0x5F

#define IEC_IN				((volatile uint8_t *)0xf01680)
#define IEC_OUT				((volatile uint8_t *)0xf01681)

#define IEC_PIN_DATA		0x01
#define IEC_PIN_CLK			0x02
#define IEC_PIN_ATN			0x10
#define IEC_PIN_SREQ		0x80

// TODO: fill out with the actual registers

#endif