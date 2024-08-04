/**
 * @file ieecll.h
 * @author your name (you@domain.com)
 * @brief Declare the functions that provide low-level access to the IEC port
 * @version 0.1
 * @date 2024-07-27
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef __iecll_h__
#define __iecll_h__

#include <stdint.h>

/**
 * @brief Initialize the IEC interface
 * 
 * @return short 0 on success, -1 if no devices found
 */
extern short iecll_ioinit();

/**
 * @brief Send a TALK command to a device
 * 
 * @param device the number of the device to become the talker
 * @return short 
 */
extern short iecll_talk(uint8_t device);

/**
 * @brief Send the secondary address to the TALK command, release ATN, and turn around control of the bus
 * 
 * @param secondary_address the secondary address to send
 * @return short 
 */
extern short iecll_talk_sa(uint8_t secondary_address);

/**
 * @brief Send a LISTEN command to a device
 * 
 * @param device 
 * @return short the number of the device to become the listener
 */
extern short iecll_listen(uint8_t device);

/**
 * @brief Send the secondary address to the LISTEN command and release ATN
 * 
 * @param secondary_address the secondary address to send
 * @return short 
 */
extern short iecll_listen_sa(uint8_t secondary_address);

/**
 * @brief Send the UNTALK command to all devices and drop ATN
 * 
 */
extern void iecll_untalk();

/**
 * @brief Send the UNLISTEN command to all devices
 * 
 */
extern void iecll_unlisten();

/**
 * @brief Try to get a byte from the IEC bus
 * 
 * NOTE: EOI flag is set, if this character is the last to be read from the active talker.
 * 
 * @return uint8_t the byte read
 */
extern uint8_t iecll_in();

/**
 * @brief Check to see if the last byte read was an EOI byte
 * 
 * @return short 0 if not EOI, any other number if EOI
 */
extern short iecll_eoi();

/**
 * @brief Send a byte to the IEC bus. Actually sends the previous byte and queues the current byte.
 * 
 * @param byte the byte to send
 */
extern void iecll_out(uint8_t byte);

/**
 * @brief Assert and release the reset line on the IEC bus
 * 
 */
extern void iecll_reset();

#endif