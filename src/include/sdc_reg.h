/*
 * Include file to access the SDC port on the Foenix machine
 */

#ifndef __SDC_REG_H
#define __SDC_REG_H

#include <stdint.h>

#include "sys_types.h"
#include "sys_general.h"

/**
 * @brief Structure to manage the SPI driven SDC ports
 *
 */
#if (MODEL != MODEL_FOENIX_FA2560K2) && (MODEL != MODEL_FOENIX_A2560ME)
//
// NOTE: [A2560Me] delay registers are set at reset by the FPGA. These set the divisor for the low speed
//       SPI clock. Here are details from Stefany:
//
//       * SD0: is controlled from the main FPGA and the running clock for the SPI is 100.5Mhz and
//         the overall state-machine will divide that clock by 4, so in high-speed mode, the SPI Clock
//         is actually 25.125Mhz. So, the value of that new register to have a clock between
//         100Khz - 400Khz for the slow-mode is 80.
// 
//       * SD1: is the same as SD0.
//
//       * Now, in the case of SD2, the SPI Controller is inside the 10M02 FPGA (the north bridge) and
//         the main clock for the CPU is the local bus Clock which is 50.250Mhz, so the value of this
//         new register is 40... So, for the RECORD, do not forget about this.
//

typedef struct s_sdc_spi {
	uint8_t ctrl;
	uint8_t data;
	uint8_t delay;			// Delay for low speed mode (default set by FPGA)
	uint8_t delay_copy;		// Copy of delay
} t_sdc_spi, *p_sdc_spi;
#endif

#if MODEL == MODEL_FOENIX_A2560K
#include "A2560K/sdc_a2560k.h"

#elif MODEL == MODEL_FOENIX_A2560X || MODEL == MODEL_FOENIX_GENX
#include "A2560X/sdc_a2560x.h"

#elif MODEL == MODEL_FOENIX_A2560ME
#include "A2560Me/sdc_a2560me.h"

#elif MODEL == MODEL_FOENIX_A2560U || MODEL == MODEL_FOENIX_A2560U_PLUS
#include "A2560U/sdc_a2560u.h"

#elif MODEL == MODEL_FOENIX_FMX || MODEL == MODEL_FOENIX_C256U || MODEL == MODEL_FOENIX_C256U_PLUS
#include "C256/sdc_c256.h"

#elif MODEL_FOENIX_F256_GEN
#include "F256/sdc_spi.h"

#elif MODEL == MODEL_FOENIX_FA2560K2
#include "FA2560K2/sdc_fa2560k.h"

#endif

/*
 * Bit definitions
 */

// SDC_TRANS_TYPE_REG
#define SDC_TRANS_DIRECT      0x00   // 00 = Direct Access
#define SDC_TRANS_INIT_SD     0x01   // 01 = Init SD
#define SDC_TRANS_READ_BLK    0x02   // 10 = RW_READ_BLOCK (512 Bytes)
#define SDC_TRANS_WRITE_BLK   0x03   // 11 = RW_WRITE_SD_BLOCK

// SDC_TRANS_CONTROL_REG
#define SDC_TRANS_START       0x01
// SDC_TRANS_STATUS_REG
#define SDC_TRANS_BUSY        0x01   //  1= Transaction Busy
// SDC_TRANS_ERROR_REG
#define SDC_TRANS_INIT_NO_ERR   0x00   // Init Error Report [1:0]
#define SDC_TRANS_INIT_CMD0_ERR 0x01
#define SDC_TRANS_INIT_CMD1_ERR 0x02

#define SDC_TRANS_RD_NO_ERR     0x00   // Read Error Report [3:2]
#define SDC_TRANS_RD_CMD_ERR    0x04
#define SDC_TRANS_RD_TOKEN_ERR  0x08

#define SDC_TRANS_WR_NO_ERR     0x00   // Write Report Error  [5:4]
#define SDC_TRANS_WR_CMD_ERR    0x10
#define SDC_TRANS_WR_DATA_ERR   0x20
#define SDC_TRANS_WR_BUSY_ERR   0x30

#endif
