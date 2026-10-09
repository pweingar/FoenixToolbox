/*
 * Declarations for the interrupt system on the A2560Me
 */

#ifndef __INTERRUPT_A2560ME__
#define __INTERRUPT_A2560ME__

#include <stdint.h>
#include "sys_general.h"
#include "sys_macros.h"
#include "sys_types.h"

/*
 * Interrupt control registers
 */

#define PENDING_GRP0 		((volatile uint16_t *)0xFEC00100)
#define PENDING_GRP1 		((volatile uint16_t *)0xFEC00102)
#define PENDING_GRP2 		((volatile uint16_t *)0xFEC00104)

#define POL_GRP0 			((volatile uint16_t *)0xFEC00108)
#define POL_GRP1 			((volatile uint16_t *)0xFEC0010A)
#define POL_GRP2 			((volatile uint16_t *)0xFEC0010C)

#define EDGE_GRP0 			((volatile uint16_t *)0xFEC00110)
#define EDGE_GRP1 			((volatile uint16_t *)0xFEC00112)
#define EDGE_GRP2 			((volatile uint16_t *)0xFEC00114)

#define MASK_GRP0 			((volatile uint16_t *)0xFEC00118)
#define MASK_GRP1 			((volatile uint16_t *)0xFEC0011A)
#define MASK_GRP2 			((volatile uint16_t *)0xFEC0011C)

/**
 * Interrupt assignments on the A2560Me
 * 
 * Group 0
 * Number   Function
 * 00       Start of Frame
 * 01       Start of Line
 * 02 - 06  Reserved
 * 07       USB UART COM3
 * 08       Super I/O Keyboard
 * 09       Super I/O Mouse
 * 0A       Super I/O COM1
 * 0B       Super I/O COM2
 * 0C       Super I/O LPT
 * 0D       Super I/O Floppy
 * 0E       Super I/O MPU-401
 * 0F       RTC
 * 
 * Group 1
 * 10       VDMA (DDR3 Blitter)
 * 11       MDMA
 * 12 - 16  DMA Channels 2 - 6
 * 17       SDMA
 * 18       Front SD Card Insertion
 * 19       TIMER0
 * 1A       TIMER1
 * 1B       TIMER2
 * 1C       TIMER3
 * 1D - 1E  Reserved
 * 1F       WizFi UART: Receive FIFO not empty
 *
 * Group 2
 * 20       LAN9221 Ethernet
 * 21       WizFi module IRQ pin
 * 22       Reserved
 * 23       PCI Express
 * 24       USB UART COM4
 * 25       IDE/SATA Bridge
 * 26       TRINITY Joystick
 * 27       Reserved
 * 28       SAM2695 MIDI Receive FIFO
 * 29       VS1053 MIDI receive FIFO
 * 2A       OPL3 Timer
 * 2B       OPN2 Timer
 * 2C       OPM Timer
 * 2D       PWM FIFO almost empty
 * 2E       DAC 44.1 kHz FIFO
 * 2F       DAC 48 kHz FIFO
 */

/*
 * Define standard interrupt numbers to be used for enabling, disabling an interrupt or setting its handler
 */

#define INT_SOF             0x00    /* Vicky Start of Frame */
#define INT_SOL             0x01    /* Vicky Start of Line */
#define INT_RESERVED_1      0x02
#define INT_RESERVED_2      0x03
#define INT_RESERVED_3      0x04
#define INT_RESERVED_4      0x05
#define INT_RESERVED_5      0x06
#define INT_COM3            0x07
#define INT_KBD_PS2         0x08    /* Super I/O PS/2 Keyboard */
#define INT_MOUSE           0x09    /* Super I/O PS/2 Mouse */
#define INT_COM1            0x0A    /* Super I/O COM1 */
#define INT_COM2            0x0B    /* Super I/O COM2 */
#define INT_LPT1            0x0C    /* Super I/O LPT */
#define INT_FDC             0x0D    /* Super I/O Floppy */
#define INT_MIDI            0x0E    /* Super I/O MIDI */
#define INT_RTC             0x0F    /* Real Time Clock */

#define INT_VDMA            0x10    /* DDR3 VMDA (Blitter) */
#define INT_MDMA            0x11    /* SRAM <-> DDR3 DMA */
#define INT_RESERVED_6      0x12
#define INT_RESERVED_7      0x13
#define INT_RESERVED_8      0x14
#define INT_RESERVED_9      0x15
#define INT_RESERVED_A      0x16
#define INT_SDMA            0x17    /* SDMA End Transfer */
#define INT_SDC_INS         0x18    /* Front SDC Insertion */
#define INT_TIMER0          0x19    /* Timer 0, Clocked with the CPU Clock */
#define INT_TIMER1          0x1A    /* Timer 1, Clocked with the CPU Clock */
#define INT_TIMER2          0x1B    /* Timer 2, Clocked with the CPU Clock */
#define INT_TIMER3          0x1C    /* Timer 3, Clocked with the SOF */
#define INT_RESERVED_B		0x1D
#define INT_RESERVED_C 		0x1E
#define INT_WIZFI_FIFO      0x1F    /* WizFi: Receive FIFO not empty */

#define INT_LAN9221         0x20    /* LAN9221 Ethernet Adapter */
#define INT_WIZFI_IRQ       0x21    /* WizFi IRQ Pin */
#define INT_PCIE            0x22    /* PCI Express Interrupt */
#define INT_OPM_INT         0x23    /* Internal OPM */
#define INT_COM4            0x24    /* USB UART COM4 */
#define INT_PATA            0x25    /* IDE/PATA Hard drive interrupt */
#define INT_JOYSTICK        0x26    /* Trinity Joystick */
#define INT_RESERVED_D      0x27    /* Reserved */
#define INT_SAM2695         0x28    /* SAM2695 MIDI Receive FIFO */
#define INT_VS1053          0x29    /* VS1053 MIDI Receive FIFO */
#define INT_OPL3            0x2A    /* OPL3 Timer */
#define INT_OPN2            0x2B    /* OPN2 Timer */
#define INT_OPM             0x2C    /* OPM Timer */
#define INT_PWM             0x2D    /* PWM FIFO almost empty */
#define INT_DAC_44          0x2E    /* DAC 44.1 kHz FIFO */
#define INT_DAC_48          0x2F    /* DAC 48 kHz FIFO */

#endif
