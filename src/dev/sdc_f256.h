/**
 * Definitions support low level SDC device driver for the F256
 */

#ifndef __SDC_F256_H
#define __SDC_F256_H

#include "F256/sdc_spi.h"
#include "sys_types.h"

//
// Definitions for GABE's internal SD card controller
//

#define SDC_SECTOR_SIZE         512         // Size of a block on the SDC

#define SDC_STAT_NOINIT         0x01        // SD has not been initialized
#define SDC_STAT_PRESENT        0x02        // SD is present
#define SDC_STAT_PROTECTED      0x04        // SD is write-protected

/* MMC card type flags (MMC_GET_TYPE) */
#define CT_MMC3					0x01		/* MMC ver 3 */
#define CT_MMC4					0x02		/* MMC ver 4+ */
#define CT_MMC					0x03		/* MMC */
#define CT_SDC1					0x04		/* SD ver 1 */
#define CT_SDC2					0x08		/* SD ver 2+ */
#define CT_SDC					0x0C		/* SD */
#define CT_BLOCK				0x10		/* Block addressing */

typedef struct s_sd_card_info {
	p_sdc_spi reg;
	uint8_t type;
	uint8_t status;
} t_sd_card_info, *p_sd_card_info;

//
// Install the SDC driver
//
extern short sdc_install();

#endif
