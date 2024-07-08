/*------------------------------------------------------------------------/
/  Foolproof MMCv3/SDv1/SDv2 (in SPI mode) control module
/-------------------------------------------------------------------------/
/
/  Copyright (C) 2019, ChaN, all right reserved.
/
/ * This software is a free software and there is NO WARRANTY.
/ * No restriction on use. You can use, modify and redistribute it for
/   personal, non-profit or commercial products UNDER YOUR RESPONSIBILITY.
/ * Redistributions of source code must retain the above copyright notice.
/
/-------------------------------------------------------------------------/
  Features and Limitations:

  * Easy to Port Bit-banging SPI
    It uses only four GPIO pins. No complex peripheral needs to be used.

  * Platform Independent
    You need to modify only a few macros to control the GPIO port.

  * Low Speed
    The data transfer rate will be several times slower than hardware SPI.

  * No Media Change Detection
    Application program needs to perform a f_mount() after media change.

/-------------------------------------------------------------------------*/

#include <stdio.h>
#include "ff.h"		/* Obtains integer types for FatFs */
#include "f256xe_diskio.h"	/* Common include file for FatFs and disk I/O layer */
#include "../dev/rtc.h"

/*-------------------------------------------------------------------------*/
/* Platform dependent macros and functions needed to be modified           */
/*-------------------------------------------------------------------------*/
/* System Control Registers - to ger CD & WP from SD0 */
#define SD0_STAT      (*(volatile __far uint8_t *)0xF016A0)
#define SD0_STAT_CD   0x40  // When 1 = No Card, 0 = Card is Present
#define SD0_STAT_WP   0x80  // When 0 = Writeable, 1 = Card is Protected

/* SPI Controler 0 Registers - External Access (Front of Unit)*/
#define SD0_CTRL      (*(volatile __far uint8_t *)0xF01D00)
#define SD0_DATA			(*(volatile __far uint8_t *)0xF01D01)
/* SPI Controler 1 Registers - Internal Access (underneath of Unit - uSDCard) */
// Specific to the F256xE - Internal SDCard (Permanent Disk)
#define SD1_CTRL      (*(volatile __far uint8_t *)0xF01D80)
#define SD1_DATA			(*(volatile __far uint8_t *)0xF01D81)

#define SDx_CS				0x01		// 1 = Enable 
#define SDx_SLOW 			0x02		// 1 = Slow 400Khz, 0 = 25Mhz
#define SDx_BUSY			0x80		// 1 = Busy
// 
#define DEV_SD0		0	/* Frontal SDCard - Removable Media 0 - SDCARD */
#define DEV_SD1		1	/* Underneath SDCard - Permanent Media 1 - SDCARD */
//
/* MMC card type flags (MMC_GET_TYPE) */
#define CT_MMC3		0x01		/* MMC ver 3 */
#define CT_MMC4		0x02		/* MMC ver 4+ */
#define CT_MMC		0x03		/* MMC */
#define CT_SDC1		0x04		/* SD ver 1 */
#define CT_SDC2		0x08		/* SD ver 2+ */
#define CT_SDC		0x0C		/* SD */
#define CT_BLOCK	0x10		/* Block addressing */

/* Delay n microseconds (avr-gcc -Os) */
static void dly_us (UINT n)	{
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
/*--------------------------------------------------------------------------

   Module Private Functions

---------------------------------------------------------------------------*/

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


static DSTATUS SD0_Stat = STA_NOINIT;	/* Disk status */
static DSTATUS SD1_Stat = STA_NOINIT;	/* Disk status */
static BYTE SD0_CardType;			/* b0:MMC, b1:SDv1, b2:SDv2, b3:Block addressing */
static BYTE SD1_CardType;			/* b0:MMC, b1:SDv1, b2:SDv2, b3:Block addressing */

/*-----------------------------------------------------------------------*/
/* Transmit Busy Flag Check - SPI Controler 0                          */
/*-----------------------------------------------------------------------*/
static void SD0_Wait_SDx_Busy( void ) {
	unsigned char i;
	i = (SD0_CTRL & SDx_BUSY);
	do {
		i = (SD0_CTRL & SDx_BUSY);
	} while (i == SDx_BUSY);
}

/*-----------------------------------------------------------------------*/
/* Transmit bytes to the card - SPI Controler 0                          */
/*-----------------------------------------------------------------------*/
static void SD0_Tx ( const BYTE* buff,	UINT bc ) {
	BYTE d;
	do {
		d = *buff++;	/* Get a byte to be sent */
    SD0_DATA = d; // Set the Data in the Transmit Register
    SD0_Wait_SDx_Busy(); // Wait for the transmit to be over with 
	} while (--bc);
}

/*-----------------------------------------------------------------------*/
/* Receive bytes from the card - SPI Controler 0                         */
/*-----------------------------------------------------------------------*/
static void SD0_Rx ( BYTE *buff,	UINT bc	) {
	BYTE r;
	do {
    	SD0_DATA = 0xff; // Set the Data in the Transmit Register
    	SD0_Wait_SDx_Busy(); // Wait for the transmit to be over with 
			*buff++ = SD0_DATA;			/* Store a received byte */
	} while (--bc);
}

/*-----------------------------------------------------------------------*/
/* Wait for card ready Using - SPI Controler 0                           */
/*-----------------------------------------------------------------------*/
static int SD0_wait_ready (void) {
	BYTE d;
	UINT tmr;
	for (tmr = 5000; tmr; tmr--) {	/* Wait for ready in timeout of 500ms */
		SD0_Rx(&d, 1);
		if (d == 0xFF) break;
		dly_us(100);              // 100us
	}
	return tmr ? 1 : 0;
}

/*-----------------------------------------------------------------------*/
/* Deselect the card and release SPI bus - SPI Controler 0               */
/*-----------------------------------------------------------------------*/
static void SD0_deselect ( void ) {
	BYTE d;
  SD0_CTRL = SD0_CTRL & ~ SDx_CS; // SDx_CS = 0 ( Disabled ), SDx = 1 (Active)
	SD0_Rx(&d, 1);	/* Dummy clock (force DO hi-z for multiple slave SPI) */
}

/*-----------------------------------------------------------------------*/
/* Select the card and wait for ready - SPI Controler 0                  */
/*-----------------------------------------------------------------------*/
/* 1:OK, 0:Timeout */
static int SD0_select (void) {
	BYTE d;

  SD0_CTRL = SD0_CTRL | SDx_CS; // SDx_CS = 0 ( Disabled ), SDx = 1 (Active)
	SD0_Rx(&d, 1);	/* Dummy clock (force DO enabled) */
	
  if (SD0_wait_ready()) 
    return 1;	/* Wait for card ready */

	SD0_deselect();
	return 0;			/* Failed */
}

/*-----------------------------------------------------------------------*/
/* Receive a data packet from the card  - SPI Controler 0                */
/*-----------------------------------------------------------------------*/
/* 1:OK, 0:Failed */
static int SD0_Rx_datablock (	BYTE *buff,	UINT btr ) {
	BYTE d[2];
	UINT tmr;

	for (tmr = 1000; tmr; tmr--) {	/* Wait for data packet in timeout of 100ms */
		SD0_Rx(d, 1);
		if (d[0] != 0xFF) break;
		dly_us(100);    // 100us
	}

	if (d[0] != 0xFE) 
    return 0;		/* If not valid data token, return with error */

	SD0_Rx(buff, btr);			/* Receive the data block into buffer */
	SD0_Rx(d, 2);					/* Discard CRC */

	return 1;						/* Return with success */
}

/*-----------------------------------------------------------------------*/
/* Send a data packet to the card  - SPI Controler 0                     */
/*-----------------------------------------------------------------------*/
/* 1:OK, 0:Failed */
static int SD0_Tx_datablock (	const BYTE *buff,	BYTE token ) {
	BYTE d[2];

	if (!SD0_wait_ready()) 
    return 0;

	d[0] = token;
	SD0_Tx(d, 1);				/* Xmit a token */

	if (token != 0xFD) {		/* Is it data token? */
		SD0_Tx(buff, 512);	/* Xmit the 512 byte data block to MMC */
		SD0_Rx(d, 2);			/* Xmit dummy CRC (0xFF,0xFF) */
		SD0_Rx(d, 1);			/* Receive data response */
		if ((d[0] & 0x1F) != 0x05)	/* If not accepted, return with error */
			return 0;
	}

	return 1;
}

/*-----------------------------------------------------------------------*/
/* Send a command packet to the card                                     */
/*-----------------------------------------------------------------------*/
/* Returns command response (bit7==1:Send failed)*/
static BYTE SD0_Tx_cmd ( BYTE cmd, DWORD arg ) {
	BYTE n, d, buf[6];
	//printf("Processing Command: %d\r", cmd);
	if (cmd & 0x80) {	/* ACMD<n> is the command sequense of CMD55-CMD<n> */
		cmd &= 0x7F;
		n = SD0_Tx_cmd(CMD55, 0);
		if (n > 1)
      return n;
	}

	/* Select the card and wait for ready except to stop multiple block read */
	if (cmd != CMD12) {
		//printf("Processing Command: %d\r", cmd);		
		SD0_deselect();
		if (!SD0_select()) 
      return 0xFF;
	}

	/* Send a command packet */
	buf[0] = 0x40 | cmd;			/* Start + Command index */
	buf[1] = (BYTE)(arg >> 24);		/* Argument[31..24] */
	buf[2] = (BYTE)(arg >> 16);		/* Argument[23..16] */
	buf[3] = (BYTE)(arg >> 8);		/* Argument[15..8] */
	buf[4] = (BYTE)arg;				/* Argument[7..0] */
	n = 0x01;						/* Dummy CRC + Stop */

	if (cmd == CMD0) 
    n = 0x95;		/* (valid CRC for CMD0(0)) */
	if (cmd == CMD8) 
    n = 0x87;		/* (valid CRC for CMD8(0x1AA)) */
	
  buf[5] = n;
	SD0_Tx(buf, 6);

	/* Receive command response */
	if (cmd == CMD12) 
    SD0_Rx(&d, 1);	/* Skip a stuff byte when stop reading */
	
  n = 10;								/* Wait for a valid response in timeout of 10 attempts */
	do
		SD0_Rx(&d, 1);
	while ((d & 0x80) && --n);

	return d;			/* Return with the response value */
}

/*-----------------------------------------------------------------------*/
/* Transmit bytes to the card - SPI Controler 1                          */
/*-----------------------------------------------------------------------*/
static void SD1_Tx ( const BYTE* buff,	UINT bc ) {
	BYTE d;
	do {
		d = *buff++;	/* Get a byte to be sent */
    SD1_DATA = d; // Set the Data in the Transmit Register
    while ( SD1_CTRL & SDx_BUSY); // Wait for the transmit to be over with 
	} while (--bc);
}

/*-----------------------------------------------------------------------*/
/* Receive bytes from the card - SPI Controler 1                         */
/*-----------------------------------------------------------------------*/
static void SD1_Rx (	BYTE *buff,	UINT bc	) {
	BYTE r;
	do {
    SD1_DATA = 0xff; // Set the Data in the Transmit Register
    while ( SD1_CTRL & SDx_BUSY); // Wait for the transmit to be over with 
		*buff++ = SD1_DATA;			/* Store a received byte */
	} while (--bc);
}

/*-----------------------------------------------------------------------*/
/* Wait for card ready Using - SPI Controler 1                           */
/*-----------------------------------------------------------------------*/
static int SD1_wait_ready (void) {
	BYTE d;
	UINT tmr;
	for (tmr = 5000; tmr; tmr--) {	/* Wait for ready in timeout of 500ms */
		SD1_Rx(&d, 1);
		if (d == 0xFF) break;
		dly_us(100);              // 100us
	}
	return tmr ? 1 : 0;
}

/*-----------------------------------------------------------------------*/
/* Deselect the card and release SPI bus - SPI Controler 1               */
/*-----------------------------------------------------------------------*/
static void SD1_deselect ( void ) {
	BYTE d;
  	SD1_CTRL = SD1_CTRL & ~ SDx_CS; // SDx_CS = 0 ( Disabled ), SDx = 1 (Active)
	SD1_Rx(&d, 1);	/* Dummy clock (force DO hi-z for multiple slave SPI) */
}

/*-----------------------------------------------------------------------*/
/* Select the card and wait for ready - SPI Controler 1                  */
/*-----------------------------------------------------------------------*/
/* 1:OK, 0:Timeout */
static int SD1_select (void) {
	BYTE d;

  	SD1_CTRL = SD1_CTRL | SDx_CS; // SDx_CS = 0 ( Disabled ), SDx = 1 (Active)
	SD1_Rx(&d, 1);	/* Dummy clock (force DO enabled) */
	
  	if (SD1_wait_ready()) 
    	return 1;	/* Wait for card ready */

	SD1_deselect();
	return 0;			/* Failed */
}

/*-----------------------------------------------------------------------*/
/* Receive a data packet from the card  - SPI Controler 1                */
/*-----------------------------------------------------------------------*/
/* 1:OK, 0:Failed */
static int SD1_Rx_datablock (	BYTE *buff,	UINT btr ) {
	BYTE d[2];
	UINT tmr;

	for (tmr = 1000; tmr; tmr--) {	/* Wait for data packet in timeout of 100ms */
		SD1_Rx(d, 1);
		if (d[0] != 0xFF) break;
		dly_us(100);    // 100us
	}

	if (d[0] != 0xFE) 
    return 0;		/* If not valid data token, return with error */

	SD1_Rx(buff, btr);			/* Receive the data block into buffer */
	SD1_Rx(d, 2);					  /* Discard CRC */

	return 1;						    /* Return with success */
}

/*-----------------------------------------------------------------------*/
/* Send a data packet to the card  - SPI Controler 1                     */
/*-----------------------------------------------------------------------*/
/* 1:OK, 0:Failed */
static int SD1_Tx_datablock (	const BYTE *buff,	BYTE token ) {
	BYTE d[2];

	if (!SD1_wait_ready()) 
    return 0;

	d[0] = token;
	SD1_Tx(d, 1);				/* Xmit a token */

	if (token != 0xFD) {		/* Is it data token? */
		SD1_Tx(buff, 512);	/* Xmit the 512 byte data block to MMC */
		SD1_Rx(d, 2);			/* Xmit dummy CRC (0xFF,0xFF) */
		SD1_Rx(d, 1);			/* Receive data response */
		if ((d[0] & 0x1F) != 0x05)	/* If not accepted, return with error */
			return 0;
	}

	return 1;
}

/*-----------------------------------------------------------------------*/
/* Send a command packet to the card - SPI Controler 1                   */
/*-----------------------------------------------------------------------*/
/* Returns command response (bit7==1:Send failed)*/
static BYTE SD1_Tx_cmd ( BYTE cmd, DWORD arg ) {
	BYTE n, d, buf[6];
	
	//printf("Processing Command: %d\r", cmd);
	if (cmd & 0x80) {	/* ACMD<n> is the command sequense of CMD55-CMD<n> */
		cmd &= 0x7F;
		n = SD1_Tx_cmd(CMD55, 0);
		if (n > 1)
      return n;
	}

	/* Select the card and wait for ready except to stop multiple block read */
	if (cmd != CMD12) {
		//printf("Enabling CSn for Command: %d\r", cmd);
		SD1_deselect();
		if (!SD1_select()) {
			//printf("*** Enabling CSn Timed out ***\r");
      		return 0xFF;
			
		}
	}

	/* Send a command packet */
	buf[0] = 0x40 | cmd;			/* Start + Command index */
	buf[1] = (BYTE)(arg >> 24);		/* Argument[31..24] */
	buf[2] = (BYTE)(arg >> 16);		/* Argument[23..16] */
	buf[3] = (BYTE)(arg >> 8);		/* Argument[15..8] */
	buf[4] = (BYTE)arg;				/* Argument[7..0] */
	n = 0x01;						/* Dummy CRC + Stop */

	if (cmd == CMD0) 
    n = 0x95;		/* (valid CRC for CMD0(0)) */
	if (cmd == CMD8) 
    n = 0x87;		/* (valid CRC for CMD8(0x1AA)) */
	
  buf[5] = n;
	SD1_Tx(buf, 6);

	/* Receive command response */
	if (cmd == CMD12) 
    SD1_Rx(&d, 1);	/* Skip a stuff byte when stop reading */
	
  n = 10;								/* Wait for a valid response in timeout of 10 attempts */
	do
		SD1_Rx(&d, 1);
	while ((d & 0x80) && --n);

	return d;			/* Return with the response value */
}

/*--------------------------------------------------------------------------

   Public Functions

---------------------------------------------------------------------------*/

//#define SD0_STAT      (*(volatile __far uint8_t *)0xF016A0)
//#define SD0_STAT_CD   0x40  // When 1 = No Card, 0 = Card is Present
//#define SD0_STAT_WP   0x80  // When 1 = Writeable, 0 = Card is Protected
/*-----------------------------------------------------------------------*/
/* Get Disk Status                                                       */
/*-----------------------------------------------------------------------*/
/* Drive number (always 0) */
DSTATUS disk_status ( BYTE drv ) {

  if ( drv == DEV_SD0) {
  // CHeck for Card Present
    if ( SD0_STAT & SD0_STAT_CD )
      SD0_Stat = SD0_Stat | STA_NODISK;
    else 
      SD0_Stat = SD0_Stat & ~STA_NODISK;

    if ( SD0_STAT & SD0_STAT_WP )
      SD0_Stat = SD0_Stat | STA_PROTECT;
    else 
      SD0_Stat = SD0_Stat & ~STA_PROTECT;
    return SD0_Stat;
  }

  if ( drv == DEV_SD1) {
	//printf("Drive: %d, disk_status: %x\r", drv, SD0_Stat);
    return 0x00;	// There is always a card in the drive
  }

  return STA_NOINIT;


}



/*-----------------------------------------------------------------------*/
/* Initialize Disk Drive                                                 */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
	BYTE drv		/* Physical drive nmuber (0) */
)
{
	BYTE n, ty, cmd, buf[4];
	UINT tmr;
	DSTATUS s;

  if ( drv == DEV_SD0) {
	//printf("Init Drive %d\r", drv);	
	dly_us(10000);			/* 10ms */
    SD0_CTRL = SD0_CTRL | SDx_SLOW;   // Set the SPI in Slow Mode
	  for (n = 10; n; n--) {
	  	SD0_Rx(buf, 1);	/* Apply 80 dummy clocks and the card gets ready to receive command */
	  }
	  ty = 0;
	  if (SD0_Tx_cmd(CMD0, 0) == 1) {			/* Enter Idle state */
	  	if (SD0_Tx_cmd(CMD8, 0x1AA) == 1) {	/* SDv2? */
	  		SD0_Rx(buf, 4);							/* Get trailing return value of R7 resp */
	  		if (buf[2] == 0x01 && buf[3] == 0xAA) {		/* The card can work at vdd range of 2.7-3.6V */
	  			for (tmr = 1000; tmr; tmr--) {			/* Wait for leaving idle state (ACMD41 with HCS bit) */
	  				if (SD0_Tx_cmd(ACMD41, 1UL << 30) == 0) break;
	  				dly_us(1000);
	  			}
	  			if (tmr && SD0_Tx_cmd(CMD58, 0) == 0) {	/* Check CCS bit in the OCR */
	  				SD0_Rx(buf, 4);
	  				ty = (buf[0] & 0x40) ? CT_SDC2 | CT_BLOCK : CT_SDC2;	/* SDv2+ */
	  			}
	  		}
	  	} else {							/* SDv1 or MMCv3 */
	  		if (SD0_Tx_cmd(ACMD41, 0) <= 1) 	{
	  			ty = CT_SDC2; cmd = ACMD41;	/* SDv1 */
	  		} else {
	  			ty = CT_MMC3; cmd = CMD1;	/* MMCv3 */
	  		}
	  		for (tmr = 1000; tmr; tmr--) {			/* Wait for leaving idle state */
	  			if (SD0_Tx_cmd(cmd, 0) == 0) break;
	  			dly_us(1000);
	  		}
	  		if (!tmr || SD0_Tx_cmd(CMD16, 512) != 0)	/* Set R/W block length to 512 */
	  			ty = 0;
	  	}
	  }
      SD0_CTRL = SD0_CTRL & ~SDx_SLOW;   // Bring back the Fast Mode - 25Mhz

	  SD0_CardType = ty;
	  s = ty ? 0 : STA_NOINIT;
	  SD0_Stat = s;
	  //printf("SD0_CardType: %x\r", ty);	
	  //printf("SD0_Stat: %x\r", SD0_Stat);		  
	  SD0_deselect();

	  return s;
  }

  if ( drv == DEV_SD1) {
	//printf("Initializing Internal Drive\r");
	//printf("Init Drive %d\r", drv);		
	  dly_us(10000);			/* 10ms */

    SD1_CTRL = SD1_CTRL | SDx_SLOW;   // Set the SPI in Slow Mode

	for (n = 10; n; n--) {
		SD1_Rx(buf, 1);	/* Apply 80 dummy clocks and the card gets ready to receive command */
	} 
	  ty = 0;
	  if (SD1_Tx_cmd(CMD0, 0) == 1) {			/* Enter Idle state */
	  	if (SD1_Tx_cmd(CMD8, 0x1AA) == 1) {	/* SDv2? */
	  		SD1_Rx(buf, 4);							/* Get trailing return value of R7 resp */
	  		if (buf[2] == 0x01 && buf[3] == 0xAA) {		/* The card can work at vdd range of 2.7-3.6V */
	  			for (tmr = 1000; tmr; tmr--) {			/* Wait for leaving idle state (ACMD41 with HCS bit) */
	  				if (SD1_Tx_cmd(ACMD41, 1UL << 30) == 0) break;
	  				dly_us(1000);
	  			}
	  			if (tmr && SD1_Tx_cmd(CMD58, 0) == 0) {	/* Check CCS bit in the OCR */
	  				SD1_Rx(buf, 4);
	  				ty = (buf[0] & 0x40) ? CT_SDC2 | CT_BLOCK : CT_SDC2;	/* SDv2+ */
	  			}
	  		}
	  	} else {							/* SDv1 or MMCv3 */
	  		if (SD1_Tx_cmd(ACMD41, 0) <= 1) 	{
	  			ty = CT_SDC2; cmd = ACMD41;	/* SDv1 */
	  		} else {
	  			ty = CT_MMC3; cmd = CMD1;	/* MMCv3 */
	  		}
	  		for (tmr = 1000; tmr; tmr--) {			/* Wait for leaving idle state */
	  			if (SD1_Tx_cmd(cmd, 0) == 0) break;
	  			dly_us(1000);
	  		}
	  		if (!tmr || SD1_Tx_cmd(CMD16, 512) != 0)	/* Set R/W block length to 512 */
	  			ty = 0;
	  	}
	  }
    SD1_CTRL = SD1_CTRL & ~SDx_SLOW;   // Bring back the Fast Mode - 25Mhz
	  //printf("SD1_CardType: %x\r", ty);	
	  SD1_CardType = ty;
	  s = ty ? 0 : STA_NOINIT;
	  SD1_Stat = s;
	  //printf("SD1_Status: %x\r", SD1_Stat);	
	  SD1_deselect();

	  return s;
  }

	return RES_NOTRDY;


}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (	BYTE drv,	BYTE *buff,	LBA_t sector,	UINT count ) {
	BYTE cmd;
	DWORD sect = (DWORD)sector;

  if ( drv == DEV_SD0) {
	  if (disk_status(drv) & STA_NOINIT) 
      return RES_NOTRDY;

	  if (!(SD0_CardType & CT_BLOCK))
      sect *= 512;	/* Convert LBA to byte address if needed */

	  cmd = count > 1 ? CMD18 : CMD17;			/*  READ_MULTIPLE_BLOCK : READ_SINGLE_BLOCK */
	  if (SD0_Tx_cmd(cmd, sect) == 0) {
	  	do {
	  		if (!SD0_Rx_datablock(buff, 512)) 
          break;
	  		buff += 512;
	  	} while (--count);
	  	if (cmd == CMD18) 
        SD0_Tx_cmd(CMD12, 0);	/* STOP_TRANSMISSION */
	  }
	  SD0_deselect();
  }

  if ( drv == DEV_SD1) {
	  if (disk_status(drv) & STA_NOINIT) 
      return RES_NOTRDY;
	  
    if (!(SD1_CardType & CT_BLOCK)) 
      sect *= 512;	/* Convert LBA to byte address if needed */

	  cmd = count > 1 ? CMD18 : CMD17;			/*  READ_MULTIPLE_BLOCK : READ_SINGLE_BLOCK */
	  if (SD1_Tx_cmd(cmd, sect) == 0) {
	  	do {
	  		if (!SD1_Rx_datablock(buff, 512)) 
          break;
	  		buff += 512;
	  	} while (--count);
	  	if (cmd == CMD18) 
        SD1_Tx_cmd(CMD12, 0);	/* STOP_TRANSMISSION */
	  }
	  SD1_deselect();
  }  
  
  return count ? RES_ERROR : RES_OK;      
}

/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/
//DRESULT disk_write (
//	BYTE drv,			/* Physical drive nmuber (0) */
//	const BYTE *buff,	/* Pointer to the data to be written */
//	LBA_t sector,		/* Start sector number (LBA) */
//	UINT count			/* Sector count (1..128) */
//)
DRESULT disk_write ( BYTE drv, const BYTE *buff, LBA_t sector, UINT count ) {
	DWORD sect = (DWORD)sector;

  if ( drv == DEV_SD0) {
	  if (disk_status(drv) & STA_NOINIT) 
      return RES_NOTRDY;

	  if (!(SD0_CardType & CT_BLOCK)) sect *= 512;	/* Convert LBA to byte address if needed */

	  if (count == 1) {	/* Single block write */
	  	if ((SD0_Tx_cmd(CMD24, sect) == 0) && SD0_Tx_datablock(buff, 0xFE))
	  		count = 0;
	  }
	  else {				/* Multiple block write */
	  	if (SD0_CardType & CT_SDC) 
        SD0_Tx_cmd(ACMD23, count);

	  	if (SD0_Tx_cmd(CMD25, sect) == 0) {	/* WRITE_MULTIPLE_BLOCK */
	  		do {
	  			if (!SD0_Tx_datablock(buff, 0xFC)) 
            break;
	  			buff += 512;
	  		} while (--count);

	  		if (!SD0_Tx_datablock(0, 0xFD))	/* STOP_TRAN token */
	  			count = 1;
	  	}
	  }
	  SD0_deselect();
  }

  if ( drv == DEV_SD1) {
	  if (disk_status(drv) & STA_NOINIT) 
      return RES_NOTRDY;

	  if (!(SD1_CardType & CT_BLOCK)) sect *= 512;	/* Convert LBA to byte address if needed */

	  if (count == 1) {	/* Single block write */
	  	if ((SD1_Tx_cmd(CMD24, sect) == 0) && SD1_Tx_datablock(buff, 0xFE))
	  		count = 0;
	  }
	  else {				/* Multiple block write */
	  	if (SD1_CardType & CT_SDC) 
        SD1_Tx_cmd(ACMD23, count);

	  	if (SD1_Tx_cmd(CMD25, sect) == 0) {	/* WRITE_MULTIPLE_BLOCK */
	  		do {
	  			if (!SD1_Tx_datablock(buff, 0xFC)) 
            break;
	  			buff += 512;
	  		} while (--count);
	  		
        if (!SD1_Tx_datablock(0, 0xFD))	/* STOP_TRAN token */
	  			count = 1;
	  	}
	  }
	  SD1_deselect();
  }  
  
  return count ? RES_ERROR : RES_OK;
}


/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/
//DRESULT disk_ioctl (
//	BYTE drv,		/* Physical drive nmuber (0) */
//	BYTE ctrl,		/* Control code */
//	void *buff		/* Buffer to send/receive control data */
//)
DRESULT disk_ioctl ( BYTE drv, BYTE ctrl,	void *buff ) {
	DRESULT res;
	BYTE n, csd[16];
	DWORD cs;

  if ( drv == DEV_SD0) {
	  if (disk_status(drv) & STA_NOINIT) 
      return RES_NOTRDY;	/* Check if card is in the socket */

	  res = RES_ERROR;
	  switch (ctrl) {
	  	
      case CTRL_SYNC :		/* Make sure that no pending write process */
	  		if (SD0_select()) 
          res = RES_OK;
	  		break;

	  	case GET_SECTOR_COUNT :	/* Get number of sectors on the disk (DWORD) */
	  		if ((SD0_Tx_cmd(CMD9, 0) == 0) && SD0_Rx_datablock(csd, 16)) {
	  			if ((csd[0] >> 6) == 1) {	/* SDC ver 2.00 */
	  				cs = csd[9] + ((WORD)csd[8] << 8) + ((DWORD)(csd[7] & 63) << 16) + 1;
	  				*(LBA_t*)buff = cs << 10;
	  			} 
          else {					/* SDC ver 1.XX or MMC */
	  				n = (csd[5] & 15) + ((csd[10] & 128) >> 7) + ((csd[9] & 3) << 1) + 2;
	  				cs = (csd[8] >> 6) + ((WORD)csd[7] << 2) + ((WORD)(csd[6] & 3) << 10) + 1;
	  				*(LBA_t*)buff = cs << (n - 9);
	  			}
	  			res = RES_OK;
	  		}
	  		break;

	  	case GET_BLOCK_SIZE :	/* Get erase block size in unit of sector (DWORD) */
	  		*(DWORD*)buff = 128;
	  		res = RES_OK;
	  		break;

	  	default:
	  		res = RES_PARERR;
	  }
	  SD0_deselect();
  }

  if ( drv == DEV_SD1) {
	  if (disk_status(drv) & STA_NOINIT) 
      return RES_NOTRDY;	/* Check if card is in the socket */

	  res = RES_ERROR;
	  
    switch (ctrl) {
	  	case CTRL_SYNC :		/* Make sure that no pending write process */
	  		if (SD1_select()) 
          res = RES_OK;
	  		break;

	  	case GET_SECTOR_COUNT :	/* Get number of sectors on the disk (DWORD) */
	  		if ((SD1_Tx_cmd(CMD9, 0) == 0) && SD1_Rx_datablock(csd, 16)) {
	  			if ((csd[0] >> 6) == 1) {	/* SDC ver 2.00 */
	  				cs = csd[9] + ((WORD)csd[8] << 8) + ((DWORD)(csd[7] & 63) << 16) + 1;
	  				*(LBA_t*)buff = cs << 10;
	  			} 
          else {					/* SDC ver 1.XX or MMC */
	  				n = (csd[5] & 15) + ((csd[10] & 128) >> 7) + ((csd[9] & 3) << 1) + 2;
	  				cs = (csd[8] >> 6) + ((WORD)csd[7] << 2) + ((WORD)(csd[6] & 3) << 10) + 1;
	  				*(LBA_t*)buff = cs << (n - 9);
	  			}
	  			res = RES_OK;
	  		}
	  		break;

	  	case GET_BLOCK_SIZE :	/* Get erase block size in unit of sector (DWORD) */
	  		*(DWORD*)buff = 128;
	  		res = RES_OK;
	  		break;

	  	default:
	  		res = RES_PARERR;
	  }
	  SD1_deselect();

  }  
	return res;
}

DWORD get_fattime (void)
{
	t_time time;
	rtc_get_time(&time);

    return (DWORD)(time.year - 80) << 25 |
           (DWORD)(time.month + 1) << 21 |
           (DWORD)time.day << 16 |
           (DWORD)time.hour << 11 |
           (DWORD)time.minute << 5 |
           (DWORD)time.second >> 1;
}
