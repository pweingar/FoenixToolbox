/**
 * @file iec_port.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2024-07-22
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef __iec_h__
#define __iec_h__

#include <stdint.h>

extern void iec_print(uint8_t device, char * message);

/**
 * @brief Retrieve the raw status string from an IEC device
 * 
 * @param device number of the IEC device to query
 * @param buffer character buffer in which to write the status
 * @param count the maximum number of bytes to fetch
 * @return the number of bytes read from the device
 */
extern short iec_status(uint8_t device, char * buffer, short count);

/**
 * @brief Initialize the IEC port
 * 
 * @return short 0 on success, a negative number indicates an error
 */
extern short iec_init();

#endif
