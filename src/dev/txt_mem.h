/**
 * Driver for the memory based text screen for the F256JRX and F256K2X
 */

#ifndef __txt_mem__
#define __txt_mem__

#define TXT_SCREEN_MEM_F256 1

/**
 * Initialize and install the driver
 *
 * @return 0 on success, any other number is an error
 */
extern short txt_mem_install();

/**
 * Initialize and install the driver
 *
 * @return 0 on success, any other number is an error
 */
// extern short txt_mem_f256_install();

extern void txt_mem_test();

#endif