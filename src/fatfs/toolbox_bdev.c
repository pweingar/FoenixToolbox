/**
 * @file toolbox_bdev.h
 * @author your name (you@domain.com)
 * @brief Connector module to allow FatFS to use Foenix Toolkit block devices
 * @version 0.1
 * @date 2024-07-06
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "log_level.h"
#define DEFAULT_LOG_LEVEL LOG_ERROR

#include <stdio.h>
#include <ctype.h>

#include "log.h"
#include "toolbox_bdev.h"
#include "constants.h"
#include "errors.h"
#include "dev/block.h"

/**
 * @brief Convert Toolkit Block Device result codes to FatFS result codes
 * 
 * @param result Toolkit result code
 * @return DRESULT a corresponding result code
 */
static DRESULT bdev_to_fatfs(short result) {
	if (result < 0) {
		switch(result) {
			case DEV_WRITEPROT:
				return RES_WRPRT;

			case ERR_NOT_READY:
				return RES_NOTRDY;

			case ERR_BAD_ARGUMENT:
				return RES_PARERR;

			default:
				return RES_ERROR;
		}
	}

	return RES_OK;
}

DSTATUS disk_initialize(BYTE pdrv) {
	return (DSTATUS)bdev_init(pdrv);
}

DSTATUS disk_status(BYTE pdrv) {
	DSTATUS result = (DSTATUS)bdev_status(pdrv);
	INFO1("disk_status: %02X", result);
	return result;
}

static void sector_dump(uint8_t * buffer, int count) {
	char char_buffer[17];

	printf("Sector:\n");

	short index = 0;
	for (int i = 0; i < count; i++) {
		char c = buffer[i];

		if ((i & 0xf) == 0) {
			printf("\n");
		}

		printf("%02X ", c);
	}
}

DRESULT disk_read(BYTE pdrv, BYTE* buff, LBA_t sector, UINT count) {
	short result = bdev_read(pdrv, sector, buff, 512 * count);
	INFO2("disk_read: sector #%u result %d", sector, result);
	// sector_dump(buff, 512);
	
	return bdev_to_fatfs(result);
}

DRESULT disk_write(BYTE pdrv, const BYTE* buff, LBA_t sector, UINT count) {
	short result = bdev_write(pdrv, sector, buff, 512 * count);
	INFO2("disk_write: sector #%u result %d", sector, result);
	return bdev_to_fatfs(result);
}

DRESULT disk_ioctl(BYTE pdrv, BYTE cmd, void* buff) {
	short result = bdev_ioctrl(pdrv, cmd, buff, 0);
	INFO1("disk_ioctl: %d", result);
	return bdev_to_fatfs(result);
}
