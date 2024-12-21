/**
 * @file sdc_f256.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2024-07-05
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include <stdint.h>
#include <stdio.h>

#include "log_level.h"
#define DEFAULT_LOG_LEVEL LOG_ERROR

#include "log.h"
#include "constants.h"
#include "errors.h"
#include "dev/block.h"
#include "indicators.h"
#include "interrupt.h"
#include "F256/sdc_spi.h"
#include "sdc_f256.h"

/* MMC/SD command (SPI mode) */
#define CMD0	(0)			/* GO_IDLE_STATE */
#define CMD1	(1)			/* SEND_OP_COND */
#define	ACMD41	(0x80+41)	/* SEND_OP_COND (SDC) */
#define CMD8	(8)			/* SEND_IF_COND */
#define CMD9	(9)			/* SEND_CSD */
#define CMD10	(10)		/* SEND_CID */
#define CMD12	(12)		/* STOP_TRANSMISSION */
#define CMD13	(13)		/* SEND_STATUS */
#define ACMD13	(0x80+13)	/* SD_STATUS (SDC) */
#define CMD16	(16)		/* SET_BLOCKLEN */
#define CMD17	(17)		/* READ_SINGLE_BLOCK */
#define CMD18	(18)		/* READ_MULTIPLE_BLOCK */
#define CMD23	(23)		/* SET_BLOCK_COUNT */
#define	ACMD23	(0x80+23)	/* SET_WR_BLK_ERASE_COUNT (SDC) */
#define CMD24	(24)		/* WRITE_BLOCK */
#define CMD25	(25)		/* WRITE_MULTIPLE_BLOCK */
#define CMD32	(32)		/* ERASE_ER_BLK_START */
#define CMD33	(33)		/* ERASE_ER_BLK_END */
#define CMD38	(38)		/* ERASE */
#define CMD55	(55)		/* APP_CMD */
#define CMD58	(58)		/* READ_OCR */

static t_sd_card_info sd0_card_info;
static t_sd_card_info sd1_card_info;

/**
 * @brief Transmit Busy Flag Check
 * 
 * @param sd pointer to the SPI device to check
 */
static void SD0_Wait_SDx_Busy(p_sdc_spi sd) {
	uint8_t i = (sd->ctrl & SDx_BUSY);
	do {
		i = (sd->ctrl & SDx_BUSY);
	} while (i == SDx_BUSY);
}

/**
 * @brief Transmit bytes to the card
 * 
 * @param sd pointer to the SPI device to write to
 * @param buff buffer of data to write
 * @param bc number of bytes to write
 */
static void SD0_Tx(p_sdc_spi sd, const uint8_t * buff, unsigned int bc) {
	do {
		uint8_t d = *buff++;	// Get a byte to be sent
    	sd->data = d; 			// Set the Data in the Transmit Register
    	SD0_Wait_SDx_Busy(sd);	// Wait for the transmit to be over with 
	} while (--bc);
}

/**
 * @brief Receive bytes from the card
 * 
 * @param sd pointer to the SPI device to read from
 * @param buff buffer of data to read into
 * @param bc number of bytes to read
 */
static void SD0_Rx(p_sdc_spi sd, uint8_t *buff, unsigned int bc) {
	do {
    	sd->data = 0xff;		// Set the Data in the Transmit Register
    	SD0_Wait_SDx_Busy(sd);	// Wait for the transmit to be over with 
		*buff++ = sd->data;		// Store a received byte
	} while (--bc);
}

/* Delay n microseconds (avr-gcc -Os) */
static void dly_us (unsigned int n)	{
	do {
    // Add Timer Routine for 1us (6x Clock @ 6.29Mhz)
    __asm("   nop\n"
          "   nop\n"
          "   nop\n"
          "   nop\n"
          "   nop\n"
          "   nop\n");
	} while (--n);
}

/*-----------------------------------------------------------------------*/
/* Wait for card ready Using - SPI Controler 0                           */
/*-----------------------------------------------------------------------*/

/**
 * @brief Wait for card ready Using
 * 
 * @param sd pointer to the SPI device
 * @return int 
 */
static int SD0_wait_ready (p_sdc_spi sd) {
	uint8_t d;
	int tmr;

	for (tmr = 5000; tmr; tmr--) {	// Wait for ready in timeout of 500ms
		SD0_Rx(sd, &d, 1);
		if (d == 0xFF) break;
		dly_us(100);              	// 100us
	}
	return tmr ? 1 : 0;
}


/**
 * @brief Deselect the card and release SPI bus
 * 
 * @param sd pointer to the SPI device
 */
static void SD0_deselect(p_sdc_spi sd) {
	uint8_t d;

	ind_set(IND_SDC, IND_OFF);

	sd->ctrl = sd->ctrl & ~SDx_CS;	// SDx_CS = 0 ( Disabled ), SDx = 1 (Active)
	SD0_Rx(sd, &d, 1);				// Dummy clock (force DO hi-z for multiple slave SPI)
}

/*-----------------------------------------------------------------------*/
/* Select the card and wait for ready - SPI Controler 0                  */
/*-----------------------------------------------------------------------*/
/*  */

/**
 * @brief Select the card and wait for ready
 * 
 * @param sd pointer to the SPI device
 * @return int 1:OK, 0:Timeout
 */
static int SD0_select(p_sdc_spi sd) {
	uint8_t d;

	ind_set(IND_SDC, IND_ON);

  	sd->ctrl = sd->ctrl | SDx_CS; 	// SDx_CS = 0 ( Disabled ), SDx = 1 (Active)
	SD0_Rx(sd, &d, 1);				// Dummy clock (force DO enabled)
	
  	if (SD0_wait_ready(sd)) {		// Wait for card ready
    	return 1;	
	}

	SD0_deselect(sd);
	return 0;						// Failed
}

/**
 * @brief Receive a data packet from the card
 * 
 * @param sd pointer to the SPI device
 * @param buff buffer of bytes to read into
 * @param btr number of bytes to transfer
 * @return int 1 on success, 0 on failure
 */
static int SD0_Rx_datablock (p_sdc_spi sd, uint8_t * buff, unsigned int btr) {
	uint8_t d[2];
	int tmr;

	for (tmr = 1000; tmr; tmr--) {	// Wait for data packet in timeout of 100ms
		SD0_Rx(sd, d, 1);
		if (d[0] != 0xFF) {
			break;
		}
		dly_us(100);    			// 100us
	}

	if (d[0] != 0xFE) {				// If not valid data token, return with error
    	return 0;		
	}

	SD0_Rx(sd, buff, btr);			// Receive the data block into buffer
	SD0_Rx(sd, d, 2);				// Discard CRC

	return 1;						// Return with success
}

/**
 * @brief Send a data packet to the card
 * 
 * @param sd pointer to the SPI device
 * @param buff buffer of bytes to write to the card
 * @param token token byte to transmit
 * @return int 1 on success, 0 on failure
 */
static int SD0_Tx_datablock (p_sdc_spi sd, const uint8_t *buff, uint8_t token) {
	uint8_t d[2];

	if (!SD0_wait_ready(sd)) {
    	return 0;
	}

	d[0] = token;
	SD0_Tx(sd, d, 1);					// Xmit a token

	if (token != 0xFD) {				// Is it data token?
		SD0_Tx(sd, buff, 512);			// Xmit the 512 byte data block to MMC
		SD0_Rx(sd, d, 2);				// Xmit dummy CRC (0xFF,0xFF)
		SD0_Rx(sd, d, 1);				// Receive data response
		if ((d[0] & 0x1F) != 0x05) {	// If not accepted, return with error
			return 0;
		}
	}

	return 1;
}

/*-----------------------------------------------------------------------*/
/* Send a command packet to the card                                     */
/*-----------------------------------------------------------------------*/
/* Returns command response (bit7==1:Send failed)*/

/**
 * @brief Send a command packet to the card
 * 
 * @param sd pointer to the SPI device
 * @param cmd command to send
 * @param arg 
 * @return uint8_t command response (bit7==1:Send failed)
 */
static uint8_t SD0_Tx_cmd (p_sdc_spi sd, uint8_t cmd, uint32_t arg) {
	uint8_t n, d, buf[6];

	if (cmd & 0x80) {	/* ACMD<n> is the command sequense of CMD55-CMD<n> */
		cmd &= 0x7F;
		n = SD0_Tx_cmd(sd, CMD55, 0);
		if (n > 1) {
      		return n;
		}
	}

	/* Select the card and wait for ready except to stop multiple block read */
	if (cmd != CMD12) {	
		SD0_deselect(sd);
		if (!SD0_select(sd)) {
			return 0xFF;
		}
	}

	/* Send a command packet */
	buf[0] = 0x40 | cmd;			/* Start + Command index */
	buf[1] = (uint8_t)(arg >> 24);	/* Argument[31..24] */
	buf[2] = (uint8_t)(arg >> 16);	/* Argument[23..16] */
	buf[3] = (uint8_t)(arg >> 8);	/* Argument[15..8] */
	buf[4] = (uint8_t)arg;			/* Argument[7..0] */
	n = 0x01;						/* Dummy CRC + Stop */

	if (cmd == CMD0) {
		n = 0x95;					/* (valid CRC for CMD0(0)) */
	}
	if (cmd == CMD8) {
    	n = 0x87;					/* (valid CRC for CMD8(0x1AA)) */
	}
	
  	buf[5] = n;
	SD0_Tx(sd, buf, 6);

	/* Receive command response */
	if (cmd == CMD12) 
    SD0_Rx(sd, &d, 1);				/* Skip a stuff byte when stop reading */
	
	n = 10;							/* Wait for a valid response in timeout of 10 attempts */
	do {
		SD0_Rx(sd, &d, 1);
	} while ((d & 0x80) && --n);

	return d;						/* Return with the response value */
}

/**
 * @brief pointer to the device driver structure for this device
 * 
 * @param dev initialize the device
 * @return short 0 for success, negative number for error
 */
static short sdc_init(p_dev_block dev) {
	p_sd_card_info card = (p_sd_card_info)dev->data;
	p_sdc_spi sd = card->reg;
	uint8_t n, cmd, buf[4];
	int tmr;
	short s;

	
	dly_us(10000);			/* 10ms */
    sd->ctrl |= SDx_SLOW;   // Set the SPI in Slow Mode
	for (n = 10; n; n--) {
		SD0_Rx(sd, buf, 1);	// Apply 80 dummy clocks and the card gets ready to receive command
	}

	card->type = 0;
	if (SD0_Tx_cmd(sd, CMD0, 0) == 1) {			/* Enter Idle state */
	  	if (SD0_Tx_cmd(sd, CMD8, 0x1AA) == 1) {	/* SDv2? */
	  		SD0_Rx(sd, buf, 4);							/* Get trailing return value of R7 resp */
	  		if (buf[2] == 0x01 && buf[3] == 0xAA) {		/* The card can work at vdd range of 2.7-3.6V */
	  			for (tmr = 1000; tmr; tmr--) {			/* Wait for leaving idle state (ACMD41 with HCS bit) */
	  				if (SD0_Tx_cmd(sd, ACMD41, 1UL << 30) == 0) {
						break;
					}
	  				dly_us(1000);
	  			}
	  			if (tmr && SD0_Tx_cmd(sd, CMD58, 0) == 0) {	/* Check CCS bit in the OCR */
	  				SD0_Rx(sd, buf, 4);
	  				card->type = (buf[0] & 0x40) ? (CT_SDC2 | CT_BLOCK) : CT_SDC2;	/* SDv2+ */
	  			}
	  		}

	  	} else {							/* SDv1 or MMCv3 */
	  		if (SD0_Tx_cmd(sd, ACMD41, 0) <= 1) {
				/* SDv1 */
	  			card->type = CT_SDC2;
				cmd = ACMD41;	

	  		} else {
				/* MMCv3 */
	  			card->type = CT_MMC3;
				cmd = CMD1;	
	  		}

			/* Wait for leaving idle state */
	  		for (tmr = 1000; tmr; tmr--) {			
	  			if (SD0_Tx_cmd(sd, cmd, 0) == 0) {
					break;
				}
	  			dly_us(1000);
	  		}

			/* Set R/W block length to 512 */
	  		if (!tmr || SD0_Tx_cmd(sd, CMD16, 512) != 0) {	
	  			card->type = 0;
			}
	  	}
	}

	sd->ctrl &= ~SDx_SLOW;   // Bring back the Fast Mode - 25Mhz
	card->status = card->type ? 0 : SDC_STAT_NOINIT;

	INFO1("SD0_CardType: %x", card->type);	
	INFO1("SD0_Stat: %x", card->status);
	
	SD0_deselect(sd);

	return card->status;
}

/**
 * @brief Read a block from the device
 * 
 * @param dev pointer to the device driver structure for this device
 * @param lba the LBA number of the sector to read
 * @param buffer buffer to write
 * @param size number of bytes to try to read
 * @return short the number of bytes read (negative number for error)
 */
static short sdc_read(p_dev_block dev, long lba, uint8_t * buffer, short size) {
	p_sd_card_info card = (p_sd_card_info)dev->data;
	p_sdc_spi sd = card->reg;
	uint8_t cmd;
	short count = size % 512 + 1;

	if (card->status & SDC_STAT_NOINIT) {
    	return ERR_NOT_READY;
	}

	if (!(card->type & CT_BLOCK)) {
		lba *= 512;	/* Convert LBA to byte address if needed */
	}

	cmd = (count > 1) ? CMD18 : CMD17;			/*  READ_MULTIPLE_BLOCK : READ_SINGLE_BLOCK */
	if (SD0_Tx_cmd(sd, cmd, (uint32_t)lba) == 0) {
		do {
			if (!SD0_Rx_datablock(sd, buffer, 512)) {
				break;
			}
			buffer += 512;
		} while (--count);

	  	if (cmd == CMD18) {
			SD0_Tx_cmd(sd, CMD12, 0);	/* STOP_TRANSMISSION */
		}
	}
	SD0_deselect(sd);

	return size;
}

/**
 * @brief Write a block to the device
 * 
 * @param dev pointer to the device driver structure for this device
 * @param lba the LBA number of the sector to write
 * @param buffer buffer to read
 * @param size number of bytes to try to write
 * @return short the number of bytes write (negative number for error)
 */
static short sdc_write(p_dev_block dev, long lba, const uint8_t * buffer, short size) {
	p_sd_card_info card = (p_sd_card_info)dev->data;
	p_sdc_spi sd = card->reg;
	uint8_t cmd;
	short count = size % 512 + 1;

	if (card->status & SDC_STAT_NOINIT) {
    	return ERR_NOT_READY;
	}

	if (!(card->type & CT_BLOCK)) {
		/* Convert LBA to byte address if needed */
		lba *= 512;
	}

	if (count == 1) {
		/* Single block write */
		if ((SD0_Tx_cmd(sd, CMD24, lba) == 0) && SD0_Tx_datablock(sd, buffer, 0xFE)) {
			count = 0;
		}

	} else {				
		/* Multiple block write */
	  	if (card->type & CT_SDC) {
        	SD0_Tx_cmd(sd, ACMD23, count);
		}

	  	if (SD0_Tx_cmd(sd, CMD25, lba) == 0) {	/* WRITE_MULTIPLE_BLOCK */
	  		do {
	  			if (!SD0_Tx_datablock(sd, buffer, 0xFC)) { 
            		break;
				}
	  			buffer += 512;
	  		} while (--count);

	  		if (!SD0_Tx_datablock(sd, 0, 0xFD)) {
				/* STOP_TRAN token */
	  			count = 1;
			}
	  	}
	}
	SD0_deselect(sd);

	return size;
}

/**
 * @brief Get the status of the device
 * 
 * @param dev pointer to the device driver structure for this device
 * @return short the status of the driver
 */
static short sdc_status(p_dev_block dev) {
	p_sd_card_info card = (p_sd_card_info)dev->data;

	return card->status;
}

/**
 * @brief Ensure that any pending writes to teh device have been completed
 * 
 * @param dev pointer to the device driver structure for this device
 * @return 0 on success, negative number for error
 */
static short sdc_flush(p_dev_block dev) {
	return 0;
}

/**
 * @brief Issue a control command to the device
 * 
 * @param dev pointer to the device driver structure for this device
 * @param command 
 * @param buffer 
 * @param size 
 * @return short 
 */
static short sdc_ioctrl(p_dev_block dev, short command, unsigned char * buffer, short size) {
	p_sd_card_info card = (p_sd_card_info)dev->data;
	p_sdc_spi sd = card->reg;

	uint8_t n, csd[16];
	uint32_t cs;

	if (card->status & SDC_STAT_NOINIT) {
		return ERR_NOT_READY;	/* Check if card is in the socket */
	} 
      
	short res = ERR_GENERAL;
	switch (command) {
		case IOCTRL_CTRL_SYNC:
			/* Make sure that no pending write process */		
			if (SD0_select(sd)) {
				res = 0;
			}
			break;

		case IOCTRL_GET_SECTOR_COUNT:
			/* Get number of sectors on the disk (DWORD) */
			if ((SD0_Tx_cmd(sd, CMD9, 0) == 0) && SD0_Rx_datablock(sd, csd, 16)) {
				if ((csd[0] >> 6) == 1) {	/* SDC ver 2.00 */
					cs = csd[9] + ((uint32_t)csd[8] << 8) + ((uint32_t)(csd[7] & 63) << 16) + 1;
					*(uint32_t *)buffer = cs << 10;
				} else {					/* SDC ver 1.XX or MMC */
					n = (csd[5] & 15) + ((csd[10] & 128) >> 7) + ((csd[9] & 3) << 1) + 2;
					cs = (csd[8] >> 6) + ((uint16_t)csd[7] << 2) + ((uint16_t)(csd[6] & 3) << 10) + 1;
					*(uint32_t*)buffer = cs << (n - 9);
				}
				res = 0;
			}
			break;

			case IOCTRL_GET_SECTOR_SIZE:
				/* Get erase block size in unit of sector (DWORD) */
				*(uint32_t*)buffer = 128;
				res = 0;
				break;

			default:
				res = ERR_BAD_ARGUMENT;
	}
	SD0_deselect(sd);

	return 0;
}

//
// Install the SDC driver
//
short sdc_install() {
    t_dev_block dev;                    // bdev_register copies the data, so we'll allocate this on the stack

    INFO("sdc_install");

    /* Install an interrupt handler to catch insertion of a card */
    // int_register(INT_SDC_INS, sdc_handler);
    // int_enable(INT_SDC_INS);

	sd0_card_info.reg = SD0_REG;
	sd0_card_info.status = 0;
	sd0_card_info.type = 0;

	sd1_card_info.reg = SD1_REG;
	sd1_card_info.status = 0;
	sd1_card_info.type = 0;

    dev.number = BDEV_SD0;
    dev.name = "SD0";
	dev.data = &sd0_card_info;
    dev.init = sdc_init;
    dev.read = sdc_read;
    dev.write = sdc_write;
    dev.flush = sdc_flush;
    dev.status = sdc_status;
    dev.ioctrl = sdc_ioctrl;

    short result = bdev_register(&dev);
	if (result == 0) {
		dev.number = BDEV_SD1;
		dev.name = "SD1";
		dev.data = &sd1_card_info;
		
		result = bdev_register(&dev);
	}

	return result;
}
