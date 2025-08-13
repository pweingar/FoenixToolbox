#ifndef __PS2_FA2560K_H
#define __PS2_FA2560K_H

#include <stdint.h>

/*
 * Ports for the PS/2 keyboard and mouse on the FA2560K
 */

#define PS2_CTRL      		((volatile __attribute__((far)) uint8_t *)0xffb10000)
#define PS2_CTRL_KBD_WR		0x02
#define PS2_CTRL_MOUSE_WR	0x08
#define PS2_CTRL_KBD_CLR	0x10
#define PS2_CTRL_MOUSE_CLR	0x20

#define PS2_OUT     		((volatile __attribute__((far)) uint8_t *)0xffb10042)
#define PS2_KBD_IN    		((volatile __attribute__((far)) uint8_t *)0xffb10044)
#define PS2_MOUSE_IN   		((volatile __attribute__((far)) uint8_t *)0xffb10046)
#define PS2_STAT      		((volatile __attribute__((far)) uint8_t *)0xffb10048)
#define PS2_STAT_KBD_ACK	0x80	// 1: Keyboard success
#define PS2_STAT_KBD_NAK	0x40	// 1: Keyboard error
#define PS2_STAT_MOUSE_ACK	0x20	// 1: Mouse success
#define PS2_STAT_MOUSE_NAK	0x10	// 1: Mouse error
#define PS2_STAT_MOUSE_EMP	0x02	// 1: Mouse FIFO empty
#define PS2_STAT_KBD_EMP	0x01	// 1: Keyboard FIFO empty

#endif
