/**
 * Low level driver for the PATA hard drive.
 */

#ifndef __PATA_H
#define __PATA_H

#include <stdint.h>
#include "sys_types.h"

#define PATA_GET_SECTOR_COUNT   1
#define PATA_GET_SECTOR_SIZE    2
#define PATA_GET_BLOCK_SIZE     3
#define PATA_GET_DRIVE_INFO     4

#define PATA_SECTOR_SIZE        512         // Size of a block on the PATA

#define PATA_STAT_NOINIT        0x01        // PATA hard drive has not been initialized
#define PATA_STAT_PRESENT       0x02        // PATA hard drive is present

//
// Structures
//

typedef struct s_drive_info {
    uint16_t flags;
    char serial_number[18];
    char firmware_version[6];
    char model_name[38];
    uint16_t lba_enabled;
    union u1 {
        struct s1 {
            uint16_t lba_default_lo;
            uint16_t lba_default_hi;
        } lbaw;
        uint32_t lba_default;
    } l;
} t_drive_info, *p_drive_info;

//
// Install the PATA driver
//
// Returns:
//  0 on success, any negative number is an error code
//
extern short pata_install();

#endif
