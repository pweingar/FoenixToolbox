/**
 * General PS/2 Driver -- SuperIO edition
 */

#include <stdio.h>

#include "log_level.h"
#define DEFAULT_LOG_LEVEL LOG_INFO

#include "log.h"
#include "interrupt.h"
#include "ps2_reg.h"
#include "ps2_general.h"
#include "timers.h"
#include "txt_screen.h"

#define CMD_REPEAT_MAX  3

static long ps2_timeout = 120;
static bool ps2_has_two_channels = false;
static bool ps2_has_keyboard = false;
static bool ps2_has_mouse = false;
static uint16_t ps2_keyboard_id = 0;
static uint16_t ps2_mouse_id = 0;

// void ps2_echo_keys() {
//     int count = 0;

//     txt_set_xy(0, 0, 1);
//     printf("> ");

//     do {
//         if (*PS2_STATUS & PS2_STAT_OBF) {
//             uint8_t code = *PS2_DATA_BUF;
//             printf("%02X ", code);
//             count++;
//             if (count > 16) {
//                 txt_set_xy(0, 0, 1);
//                 printf("> ");
//                 count = 0;
//             }
//         }
//     } while (1);
// }

int ps2_wait_write(long timeout) {
    TRACE("ps2_wait_write");
    if (timeout != 0) {
        long timeout_ticks = timers_jiffies() + timeout;
        while (*PS2_STATUS & PS2_STAT_IBF) {
            if (timers_jiffies() > timeout_ticks) return PS2_TIMEOUT;
        }
        return PS2_OK;
    } else {
        while (*PS2_STATUS & PS2_STAT_IBF) ;
        return PS2_OK;    
    }
}

int ps2_wait_read(long timeout) {
    TRACE("ps2_wait_read");
    if (timeout != 0) {
        long timeout_ticks = timers_jiffies() + timeout;
        while ((*PS2_STATUS & PS2_STAT_OBF) == 0) {
            if (timers_jiffies() > timeout_ticks) {
                DEBUG1("PS/2: ps2_wait_read timeout: status = 0x%02X", *PS2_STATUS);
                return PS2_TIMEOUT;
            }
        }
    } else {
        while ((*PS2_STATUS & PS2_STAT_OBF) == 0) ;
    }

    return PS2_OK;
}

void ps2_flush_buffer() {
    TRACE("ps2_flush_buffer");
    while ((*PS2_STATUS & PS2_STAT_OBF) == 1) {
        uint8_t dummy = *PS2_DATA_BUF;
    }
}

/**
 * Send a byte to the command channel (controller)
 * 
 * @param b the byte to send 
 * @param timeout the number of jiffies to wait for a timeout (0 is wait forever)
 * @return status (0 = success, negative number is an error)
 */
int ps2_send_cmd(uint8_t b, long timeout) {
    TRACE1("ps2_send_cmd: 0x%02X", b);

    // Wait for the controller to be ready
    int result = ps2_wait_write(ps2_timeout);
    if (result < 0) return result;

    // Write the byte to the controller
    *PS2_CMD_BUF = b;

    DEBUG1("ps2_send_cmd: command = 0x%02X", b);

    return PS2_OK;
}

/**
 * Send a byte to the data register (controller)
 * 
 * @param b the byte to send 
 * @param timeout the number of jiffies to wait for a timeout (0 is wait forever)
 * @return status (0 = success, negative number is an error)
 */
int ps2_send_cmd_data(uint8_t b, long timeout) {
    TRACE1("ps2_send_cmd_data: 0x%02X", b);

    // Wait for the controller to be ready
    int result = ps2_wait_write(ps2_timeout);
    if (result) return result;

    // Write the byte to the controller
    *PS2_DATA_BUF = b;

    return PS2_OK;
}

/**
 * Send a data byte to the given port
 * 
 * @param port the port to send the byte to (0 or 1)
 * @param b the byte to send
 * @param timeout the number of jiffies to wait for a timeout (0 is wait forever)
 * @return status (0 = success, negative number is an error)
 */
int ps2_send_data(uint8_t port, uint8_t b, long timeout) {
    TRACE2("ps2_send_data: %d <- 0x%02X", port, b);

    if (port == 1) {
        // Writing to channel B... send the command to direct the next byte to that port
        int result = ps2_send_cmd(MOUSE_CMD_PREFIX, timeout);
        if (result < 0) return result;
    }

    // Wait for the controller to be ready
    int result = ps2_wait_write(ps2_timeout);
    if (result < 0) return result;

    // Write the byte to the controller
    *PS2_DATA_BUF = b;

    return PS2_OK;
}

/**
 * Check to see if there is data waiting to be read from the PS/2 controller data buffer
 * 
 * @return true if ps2_read_data would return data, false otherwise
 */
bool ps2_has_data() {
    if (*PS2_STATUS & PS2_STAT_OBF) {
        return true;
    } else {
        return false;
    }
}

/**
 * Wait for data on the input port and return it.
 * 
 * NOTE: timeout should be > 0 for non-interrupt use. For responding to interrupts,
 *       timeout should be set to 0.
 * 
 * @param timeout the number of jiffies to wait for a timeout (0 is return immediately)
 * @return the byte received (if >= 0) or an error code if negative
 */
int ps2_read_data(long timeout) {
    TRACE("ps2_read_data");

    if (timeout > 0) {
        int result = ps2_wait_read(timeout);
        if (result) return result;
    }

    // Read a byte from the port
    uint8_t result_byte = *PS2_DATA_BUF;
    return result_byte;
}

/**
 * Send a command to the PS/2 controller along with a parameter byte
 * 
 * @param command the command byte to send
 * @param param the data byte to send
 * @param timeout the number of jiffies to wait for a response
 * @return 0 success, negative number on error
 */
int ps2_send_cmd_with_data(uint8_t command, uint8_t param, long timeout) {
    TRACE2("ps2_send_cmd_with_data: command = 0x%02X, param = 0x%02X", command, param);

    if (ps2_send_cmd(command, timeout)) {
        return PS2_TIMEOUT;
    }

    if (ps2_send_cmd_data(param, timeout)) {
        return PS2_TIMEOUT;
    }

    return PS2_OK;
}

/**
 * Send a command to the controller without a parameter, wait for and return a response code
 * 
 * @param command the command byte to send
 * @param timeout the number of jiffies to wait for a response
 * @return the response to the command (< 0 represents an error talking to the controller)
 */
int ps2_send_cmd_expect_response(uint8_t command, long timeout) {
    TRACE1("ps2_send_cmd_expect_response: command = 0x%02X", command);

    int result = ps2_send_cmd(command, timeout);
    if (result < 0) return result;

    result = ps2_read_data(timeout);
    DEBUG1("ps2_send_cmd_expect_response got response 0x%02X", result);
    return result;
}

/**
 * Check the PS/2 controller to see if it has both channels
 * 
 * @return true if the second channel responded, false otherwise (might be a timeout)
 */
static bool ps2_check_two_channels() {
    TRACE("ps2_check_two_channels");

    int result = ps2_send_data(1, PS2_CTRL_ENABLE_2, ps2_timeout);
    if (result) {
        printf("ps2_check_two_channels: result = %d\n", result);
        return false;
    }

    // We don't care about the response here
    ps2_flush_buffer();

    result = ps2_send_cmd_expect_response(PS2_CTRL_READCMD, ps2_timeout);
    if (result < 0) {
        DEBUG("PS/2: PS2_CTRL_READCMD timeout");
        printf("PS/2: PS2_CTRL_READCMD timeout\n");
        return PS2_TIMEOUT;
    }

    printf("PS/2: PS2_CTRL_READCMD result = 0x%02X\n", result);

    if (result & 0x20) {
        printf("PS/2: PS2_CTRL_READCMD result had bit 0x10 set.\n");
        return false;
    } else {
        uint8_t ctrl_config = (uint8_t)result;
        result = ps2_send_cmd(PS2_CTRL_DISABLE_2, ps2_timeout);
        result = ps2_send_cmd_with_data(PS2_CTRL_WRITECMD, ctrl_config & 0xee, ps2_timeout);

        printf("PS/2: PS2_CTRL_READCMD result had bit 0x20 clear.\n");
        return true;
    }
}

/**
 * Run the channel test command provided and return true if there was an "OK" response
 * 
 * @param command the channel test command to run (0xAB, or 0xA9)
 * @return true if the channel tested OK, false otherwise
 */
static bool ps2_channel_tested(uint8_t command) {
    TRACE1("ps2_channel_tested: command = 0x%02X", command);
    printf("ps2_channel_tested: command = 0x%02X\n", command);
    int result = ps2_send_cmd_expect_response(command, ps2_timeout);
    if (result == 0) {
        return true;
    } else {
        printf("ps2_channel_tested: command = 0x%02X, result = 0x%02X\n", command, result);
        return false;
    }
}

/**
 * Attempt to reset the device attached to a given port.
 * 
 * @param port the number of the port to get the reset command (0 or 1)
 * @param dev_id pointer to the 16-bit variable in which to store any PS/2 device ID received
 * @return 0 on success, negative number on error
 */
static int ps2_reset_device(uint8_t port, uint16_t * dev_id) {
    TRACE1("ps2_reset_device: port = %d", port);

    short ack_count = 0;
    *dev_id = 0;

    int result = ps2_send_data(port, 0xff, ps2_timeout);
    if (result) return result;

    // Process the response from the device as flexibly as possible
    // Device may ACK with 0xFA 0xAA or with 0xAA 0xFA
    // Device can respond with a PS/2 device ID of 1 or 2 bytes (but it might not)

    while (1) {
        result = ps2_read_data(ps2_timeout);
        if ((result == 0xfa) || (result == 0xaa)) {
            ack_count++;
        } else {
            // Got something besides an ACK... fall out of the loop to check it out
            break;
        }
    }

    // TODO: we should probably be on the lookout for the RESEND response and handle it.

    if (result < 0) {
        // We got a timeout... either there was no acknowledgement, or no ID
        if (ack_count > 0) {
            // We got an acknowledgement, but no ID. Return success but don't set an ID
            return 0;
        } else {
            // We did not get ANY response, return the timeout
            return result;
        }

    } else if ((result == 0xfc) || (result == 0xfd)) {
        // We got a self-test error... return it
        return PS2_SELF_TEST_FAIL;

    } else {
        // Check for an optional device ID byte
        int id_byte_0 = result;
        if (id_byte_0 < 0) {
            // No ID returned... return OK, but don't set an ID
            return 0;
        }

        // Check for an optional second device ID byte
        int id_byte_1 = ps2_read_data(ps2_timeout);
        if (id_byte_1 < 0) {
            // Single byte ID
            *dev_id = id_byte_0 & 0x00ff;
        } else {
            // Two byte ID
            *dev_id = ((id_byte_1 & 0x00ff) << 8) | (id_byte_0 & 0x00ff);
        }

        return 0;
    }
}

/**
 * Return the ID of the device on the given port as determined during initialization.
 * 
 * @param port the number of the port to check (0 or 1)
 * @return the ID of the device (0xffff means there was no device on that port, or the port is not supported)
 */
uint16_t ps2_device_id(uint8_t port) {
    switch(port) {
        case 0:
            if (ps2_has_keyboard) {
                return ps2_keyboard_id;
            }
            break;

        case 1:
            if (ps2_has_mouse) {
                return ps2_mouse_id;
            }
            break;

        default:
            break;
    }

    return 0xffff;
}

/**
 * Check to see if there was a device on the given port
 * 
 * @param port the number of the port (0 or 1)
 * @return true if there was a device there when we initialized
 */
bool ps2_has_device(uint8_t port) {
    switch(port) {
        case 0:
            return ps2_has_keyboard;

        case 1:
            return ps2_has_mouse;

        default:
            return false;
    }
}

/**
 * Clear out the FIFO for the keyboard
 */
void kbd_clear_fifo() {
    ps2_flush_buffer();
}

/**
 * Send a command to the keyboard
 * 
 * @param cmd the command to send
 * @return 0 on success, any other number is an error
 */
short kbd_send_cmd(uint8_t cmd) {
    int count = CMD_REPEAT_MAX;

    while (count > 0) {
        ps2_flush_buffer();

        int result = ps2_send_data(0, cmd, ps2_timeout);
        if (result < 0) return result;

        printf("KBD SENT: %02X ", cmd);

        int response = ps2_read_data(ps2_timeout);
        printf("REPLY: %02X\n", response);
        if (response < 0) {
            return response;

        } else if (response == 0xfa) {
                // We got an ACK... return OK
                return PS2_OK;

        } else {
            // Got errors and retry
            count--;
        }
    }

    return PS2_CMD_ERROR;
}

/**
 * Send a command to the mouse
 * 
 * @param cmd the command to send
 * @return 0 on success, any other number is an error
 */
short mouse_send_cmd(uint8_t cmd) {
    return ps2_send_data(1, cmd, ps2_timeout);
}

/**
 * Initialize the PS/2 interface
 * 
 * This function will test the PS/2 controller (if present), check to see how many ports are present, and
 * test devices on each port.
 * 
 * @return 0 on success, negative number on error
 */
int ps2_init() {
    TRACE("ps2_init");

    int result = 0;
    
    // Disable devices
    if (ps2_send_cmd(PS2_CTRL_DISABLE_1, ps2_timeout) < 0) {
        DEBUG("PS/2: timeout attempting to disable channel 1");
        return PS2_TIMEOUT;
    }
    if (ps2_send_cmd(PS2_CTRL_DISABLE_2, ps2_timeout)) {
        DEBUG("PS/2: timeout attempting to disable channel 2");
        return PS2_TIMEOUT;
    }

    INFO("PS/2: devices disabled");

    // Flush the output buffer
    ps2_flush_buffer();

    // Set the controller configuration byte
    result = ps2_send_cmd_expect_response(PS2_CTRL_READCMD, ps2_timeout);
    if (result < 0) {
        DEBUG("PS/2: PS2_CTRL_READCMD timeout");
        return PS2_TIMEOUT;
    }
    uint8_t controller_orig = result & 0xff;
    uint8_t controller = controller_orig & 0x04;
    if (ps2_send_cmd_with_data(PS2_CTRL_WRITECMD, 0x04, ps2_timeout) < 0) {
        DEBUG("PS/2: ps2_send_cmd_with_data timeout");
        return PS2_TIMEOUT;
    }

    INFO("PS/2: controller configuration set");

    ps2_flush_buffer();

    // Perform the controller selftest
    result = ps2_send_cmd_expect_response(PS2_CTRL_SELFTEST, ps2_timeout);
    if (result != PS2_RESP_OK) {
        INFO1("PS/2: controller selftest response: 0x%02X", result);
        return PS2_ERR_CONTROLLER;
    }

    INFO("PS/2: controller passed self-test");

    // Check to see if there are two channels
    ps2_has_two_channels = true; // ps2_check_two_channels();

    if (ps2_has_two_channels) {
        printf("PS/2: system has two ports\n");
    } else {
        printf("PS/2: system has only one port\n");
    }

    // Test channels

    // Make sure the buffer is clear
    ps2_flush_buffer();

    if (ps2_channel_tested(PS2_CTRL_KBDTEST)) {
        ps2_has_keyboard = true;
        INFO("PS/2: system has a keyboard");
        printf("PS/2: system has a keyboard\n");
    } else {
        printf("PS/2: system does not have a keyboard.\n");
    }

    if (ps2_has_two_channels) {
        if (ps2_channel_tested(PS2_CTRL_MOUSETEST)) {
            ps2_has_mouse = true;
            printf("PS/2: system has a mouse.\n");
        } else {
            ps2_has_mouse = false;
            printf("PS/2: system does not have a mouse.\n");
        }
    }

    // Enable channels

    // Make sure the buffer is clear
    ps2_flush_buffer();

    result = ps2_send_cmd(PS2_CTRL_ENABLE_1, ps2_timeout);
    if (result) return result;

    if (ps2_has_two_channels) {
        result = ps2_send_cmd(PS2_CTRL_ENABLE_2, ps2_timeout);
        if (result) return result;
    }

    // Reset devices

    // Make sure the buffer is clear
    ps2_flush_buffer();

    if (ps2_has_keyboard) {
        result = ps2_reset_device(0, &ps2_keyboard_id);
        if (result) {
            // Keyboard did not respond... mark that we don't really have one
            ps2_has_keyboard = false;
            ps2_keyboard_id = 0;
            printf("PS/2: keyboard did not reset.\n");
        } else {
            printf("PS/2: keyboard ID: 0x%04X\n", ps2_keyboard_id);
        }
    }

    if (ps2_has_mouse) {
        result = ps2_reset_device(1, &ps2_mouse_id);
        if (result) {
            // Mouse did not respond... mark that we don't really have one
            ps2_has_mouse = false;
            ps2_mouse_id = 0;
            printf("PS/2: mouse did not reset.\n");
        } else {
            printf("PS/2: mouse ID: 0x%04X\n", ps2_mouse_id);
        }
    }

    if (ps2_has_mouse) {
        // Set the controller configuration byte to enable both ports and translation
        if (ps2_send_cmd_with_data(PS2_CTRL_WRITECMD, 0x07, ps2_timeout) < 0) {
            DEBUG("PS/2: ps2_send_cmd_with_data timeout while enabling both ports");
            return PS2_TIMEOUT;
        }  
    } else {
        // Set the controller configuration byte to enable port #1 and translation
        if (ps2_send_cmd_with_data(PS2_CTRL_WRITECMD, 0x05, ps2_timeout) < 0) {
            DEBUG("PS/2: ps2_send_cmd_with_data timeout while enabling just port 0");
            return PS2_TIMEOUT;
        }   
    }

    return PS2_OK;
}
