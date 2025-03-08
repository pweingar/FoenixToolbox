/*
 * Definitions to access the PATA port on the A2560K
 */

#ifndef __PATA_A2560K_H
#define __PATA_A2560K_H

#include <stdint.h>

#define PATA_DATA_16         ((volatile uint16_t *)0xFEC00400)
#define PATA_DATA_8          ((volatile uint8_t *)0xFEC00400)
#define PATA_ERROR           ((volatile uint8_t *)0xFEC00402)
#define PATA_SECT_CNT        ((volatile uint8_t *)0xFEC00404)
#define PATA_SECT_SRT        ((volatile uint8_t *)0xFEC00406)
#define PATA_CLDR_LO         ((volatile uint8_t *)0xFEC00408)
#define PATA_CLDR_HI         ((volatile uint8_t *)0xFEC0040A)
#define PATA_HEAD            ((volatile uint8_t *)0xFEC0040C)
#define PATA_CMD_STAT        ((volatile uint8_t *)0xFEC0040E)

#endif
