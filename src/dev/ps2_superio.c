/**
 * General PS/2 Driver -- SuperIO edition
 */

#include <stdio.h>

#include "interrupt.h"
#include "ps2_reg.h"
#include "ps2_general.h"
#include "timers.h"
#include "txt_screen.h"

static long ps2_timeout = 120;
static bool ps2_has_two_channels = false;
static bool ps2_has_keyboard = false;
static bool ps2_has_mouse = false;

// TODO: dummy to be deleted
int kbd_getc() {
    return 0;
}

int ps2_wait_write(long timeout) {
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
    if (timeout != 0) {
        long timeout_ticks = timers_jiffies() + timeout;
        while ((*PS2_STATUS & PS2_STAT_OBF) == 0) {
            if (timers_jiffies() > timeout_ticks) {
                printf("PS/2: ps2_wait_read timeout [0x%02X]\n", *PS2_STATUS);
                return PS2_TIMEOUT;
            }
        }
    } else {
        while ((*PS2_STATUS & PS2_STAT_OBF) == 0) ;
    }

    return PS2_OK;
}

void ps2_flush_buffer() {
    printf("PS/2: flushing buffer... ");
    while ((*PS2_STATUS & PS2_STAT_OBF) == 1) {
        uint8_t dummy = *PS2_DATA_BUF;
        printf("[%02X] ", dummy);
    }
    printf("done.\n");
}

/**
 * Send a byte to the command channel (controller)
 * 
 * @param b the byte to send 
 * @param timeout the number of jiffies to wait for a timeout (0 is wait forever)
 * @return status (0 = success, negative number is an error)
 */
int ps2_send_cmd(uint8_t b, long timeout) {
    // Wait for the controller to be ready
    int result = ps2_wait_write(ps2_timeout);
    if (result) return result;

    // Write the byte to the controller
    *PS2_CMD_BUF = b;

    printf("PS/2: ps2_send_cmd: [0x%02X]\n", b);

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
    // Wait for the controller to be ready
    int result = ps2_wait_write(ps2_timeout);
    if (result) return result;

    // Write the byte to the controller
    *PS2_DATA_BUF = b;

    return PS2_OK;
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
    int result = ps2_wait_read(timeout);
    if (result) return result;

    // Read a byte from the port
    uint8_t result_byte = *PS2_DATA_BUF;
    printf("PS/2: ps2_read_data [0x%02X]\n", result_byte);
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
    printf("PS/2: ps2_send_cmd_with_data\n");
    if (ps2_send_cmd(command, timeout)) {
        return PS2_TIMEOUT;
    }
    printf("PS/2: ps2_send_cmd_with_data sent command\n");
    if (ps2_send_cmd_data(param, timeout)) {
        return PS2_TIMEOUT;
    }
    printf("PS/2: ps2_send_cmd_with_data sent data\n");

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
    printf("PS/2: ps2_send_cmd_expect_response\n");
    int result = ps2_send_cmd(command, timeout);
    if (result) return result;
    printf("PS/2: ps2_send_cmd_expect_response sent command\n");

    result = ps2_read_data(timeout);
    printf("PS/2: ps2_send_cmd_expect_response got response 0x%02X\n", result);
    return result;
}

void ps2_echo_keys() {
    int count = 0;

    txt_set_xy(0, 0, 1);
    printf("> ");

    do {
        if (*PS2_STATUS & PS2_STAT_OBF) {
            uint8_t code = *PS2_DATA_BUF;
            printf("%02X ", code);
            count++;
            if (count > 16) {
                txt_set_xy(0, 0, 1);
                printf("> ");
                count = 0;
            }
        }
    } while (1);
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
    int result = 0;

    printf("PS/2: ps2_init started\n");
    printf("PS/2: PS2_STATUS = 0x%08X\n", (uint32_t)PS2_STATUS);
    printf("PS/2: PS2_DATA_BUF = 0x%08X\n", (uint32_t)PS2_DATA_BUF);

    int_disable(0x08);
    int_disable(0x09);
    int_disable_all();

    // // Disable devices
    // if (ps2_send_cmd(PS2_CTRL_ENABLE_1, ps2_timeout) < 0) {
    //     printf("PS/2: timeout attempting to enable channel 1\n");
    //     return PS2_TIMEOUT;
    // }

    // ps2_echo_keys();
    
    // Disable devices
    if (ps2_send_cmd(PS2_CTRL_DISABLE_1, ps2_timeout) < 0) {
        printf("PS/2: timeout attempting to disable channel 1\n");
        return PS2_TIMEOUT;
    }
    if (ps2_send_cmd(PS2_CTRL_DISABLE_2, ps2_timeout)) {
        printf("PS/2: timeout attempting to disable channel 2\n");
        return PS2_TIMEOUT;
    }

    printf("PS/2: devices disabled\n");

    // Flush the output buffer
    ps2_flush_buffer();

    // Set the controller configuration byte
    // result = ps2_send_cmd_expect_response(PS2_CTRL_READCMD, ps2_timeout);
    // if (result < 0) {
    //     printf("PS/2: PS2_CTRL_READCMD timeout\n");
    //     return PS2_TIMEOUT;
    // }
    // uint8_t controller_orig = result & 0xff;
    // uint8_t controller = controller_orig & 0x04;
    if (ps2_send_cmd_with_data(PS2_CTRL_WRITECMD, 0x04, ps2_timeout) < 0) {
        printf("PS/2: ps2_send_cmd_with_data timeout\n");
        return PS2_TIMEOUT;
    }

    printf("PS/2: controller configuration set\n");

    ps2_flush_buffer();

    // Perform the controller selftest
    result = ps2_send_cmd_expect_response(PS2_CTRL_SELFTEST, ps2_timeout);
    if (result != PS2_RESP_OK) {
        printf("PS/2: controller selftest response: 0x%02X\n", result);
        return PS2_ERR_CONTROLLER;
    }

    printf("PS/2: controller passed self-test\n");

    // Check to see if there are two channels

    // Test channels

    // Enable channels

    // Reset devices

    return PS2_OK;
}



