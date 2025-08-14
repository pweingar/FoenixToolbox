/**
 * Common interface code for the internal serial interfaces on the F-series machines
 * 
 * This interface model is used by the USB serial ports, the MIDI chips, and the network interfaces
 */

#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "channel.h"
#include "errors.h"
#include "serial_common.h"
#include "sys_types.h"

#include "serial_reg.h"

/**
 * Table mapping channel device number to serial device port
 */
static uint8_t * ser_device[CDEV_DEVICES_MAX];

/**
 * Check to see if there is room in the transmit FIFO
 * 
 * @param dev pointer to the serial device
 * @return true if the transmit buffer is not full, false if it is
 */
static bool ser_can_txd(uint8_t * dev) {
    com_ser_dev_p uart = (com_ser_dev_p)dev;
    if (uart->txd_fifo_count < 2000) { // dev[SER_CONTROL] & SER_TXD_FIFO_EMPTY
        return true;
    } else {
        return false;
    }
}

/**
 * Check to see if there is data in the receive FIFO
 * 
 * @param dev pointer to the serial device
 * @return true if the receive buffer is not empty, false if it is
 */
static bool ser_can_rxd(uint8_t * dev) {
    com_ser_dev_p uart = (com_ser_dev_p)dev;
    if (uart->rxd_fifo_count == 0) { // dev[SER_CONTROL] & SER_RXD_FIFO_EMPTY
        return false;
    } else {
        return true;
    }
}

/**
 * Write a word to the data register
 * 
 * NOTE: if the TxD FIFO is full, this should just return. Therefore, the caller should be sure the FIFO has room
 * 
 * @param pointer to the serial device to write to
 */
static void ser_put(uint8_t * dev, uint8_t data) {
    if (ser_can_txd(dev)) {
        dev[SER_DATA] = data;
    }
}

/**
 * Attempt to read a word from the serial device
 * 
 * NOTE: in the case that the FIFO is empty, this function will return 0.
 *       Caller should check status before reading to be sure.
 * 
 * @param pointer to the serial device to read
 * @return the next word in the RxD FIFO (0 if none)
 */
static uint8_t ser_get(uint8_t * dev) {
    if (ser_can_rxd(dev)) {
        return dev[SER_DATA];
    } else {
        return 0;
    }
}

/**
 * Return the pointer to the serial device port for the given channel
 * 
 * @param channel pointer to the channel record
 * @return pointer to the common serial device record for the associated channel
 */
uint8_t * cdev_to_ser(p_channel channel) {
    return ser_device[channel->dev];
}

static short ser_open(t_channel * chan, const uint8_t * path, short mode) {
    uint8_t * dev = cdev_to_ser(chan);
    if (dev) {
        dev[SER_CONTROL] &= ~SER_RXD_SPEED;

        chan->data[0] = 0;      // Turn off CR -> CRLF translation
        return 0;
    } else {
        return DEV_ERR_BADDEV;
    }
}

/**
 * Read a byte from the serial device
 */
static short ser_read_b(p_channel chan) {
    uint8_t * dev = cdev_to_ser(chan);
    if (dev) {
        return ser_get(cdev_to_ser(chan));
    } else {
        return DEV_ERR_BADDEV;
    }
}

/**
 * Read a number of bytes from the serial device
 * 
 * @return the number of bytes written, negative number is an error
 */
static short ser_read(p_channel chan, uint8_t * buffer, short size) {
    int i = 0;
    uint8_t * dev = cdev_to_ser(chan);
    if (dev) {
        for (i = 0; i < size; i++) {
            if (ser_can_rxd(dev)) {
                buffer[i] = (uint8_t)ser_get(dev);
            } else {
                break;
            }
        }

        return i;

    } else {
        return DEV_ERR_BADDEV;
    }
}

/**
 * Read a line (of at most size bytes) from the common serial device
 * 
 * @param chan pointer to the channel record for the channel to read
 * @param buffer pointer to the byte buffer to write to
 * @param size the maximum number of bytes to read
 * @return the number of bytes written, negative number is an error
 */
static short ser_readline(p_channel chan, uint8_t * buffer, short size) {
    int i = 0;
    uint8_t * dev = cdev_to_ser(chan);
    if (dev) {
        for (i = 0; i < size; i++) {
            if (ser_can_rxd(dev)) {
                char c = (char)ser_get(dev);
                if ((c == '\n') || (c == '\r')) {
                    break;
                } else {
                    buffer[i] = (uint8_t)c;
                }
            } else {
                break;
            }
        }

        return i;

    } else {
        return DEV_ERR_BADDEV;
    }
}

/**
 * Write a byte to the serial device
 * 
 * @param chan pointer to the channel record for the channel we're writing
 * @param data the byte to write
 * @return 0 on success, negative number is an error
 */
static short ser_write_b(p_channel chan, uint8_t data) {
    uint8_t * dev = cdev_to_ser(chan);
    if (dev) {
        while (!ser_can_txd(dev)) ;
        ser_put(dev, data);
        if ((data == '\r') && chan->data[0]) {
            // If we should translate CR to CRLF, and we have a CR... send a LF
            ser_put(dev, '\n');
        }
        return 0;

    } else {
        return DEV_ERR_BADDEV;
    }
}

/**
 * Write a number of bytes to the common serial device
 * 
 * @param chan pointer to the channel record for the channel to write
 * @param data pointer to the bytes to write
 * @param count the number of bytes to write
 * @return the number of bytes actually written, negative number is an error
 */
static short ser_write(p_channel chan, const uint8_t * data, short count) {
    short i = 0;
    uint8_t * dev = cdev_to_ser(chan);
    if (dev) {
        for (i = 0; i < count; i++) {
            while (!ser_can_txd(dev)) ;
            ser_put(dev, data[i]);
            if ((data == '\r') && chan->data[0]) {
                // If we should translate CR to CRLF, and we have a CR... send a LF
                ser_put(dev, '\n');
            }
        }

        return i;
    } else {
        return DEV_ERR_BADDEV;
    }
}
/**
 * Check and return the status of the common serial device
 * 
 * @param chan pointer to the channel record for the channel to query
 * @return the status bitfield for the device, negative number is an error
 */
static short ser_status(p_channel chan) {
    uint8_t * dev = cdev_to_ser(chan);
    if (dev) {
        short result = 0;
        if (ser_can_rxd(dev)) result |= CDEV_STAT_READABLE;
        if (ser_can_txd(dev)) result |= CDEV_STAT_WRITABLE;
        return result;

    } else {
        return DEV_ERR_BADDEV;
    }
}

short ser_ioctrl(p_channel chan, short command, uint8_t * buffer, short size) {
    switch(command) {
        case IOCTRL_CRLF_XLATE_ON:
            chan->data[0] = 1;
            break;

        case IOCTRL_CRLF_XLATE_OFF:
            chan->data[0] = 0;
            break;

        default:
            break;
    }

    return 0;
}

/**
 * Install a common serial interface device as a channel device
 * 
 * @param port pointer to the first register of the common serial device
 * @param cdev_number the channel device number that should be registered for this device
 * @param name the user-readable name for the channel device (e.g. "COM1")
 * @return 0 on success, negative number on failure
 */
short ser_install(uint8_t * port, short cdev_number, char * name) {
    t_dev_chan device;

    // Map this channel device number to this port
    ser_device[cdev_number] = (uint8_t *)port;

    device.name = name;
    device.number = cdev_number;

    device.init = 0;
    device.open = ser_open;
    device.close = 0;

    device.read = ser_read;
    device.read_b = ser_read_b;
    device.readline = ser_readline;
    device.write = ser_write;
    device.write_b = ser_write_b;

    device.status = ser_status;
    device.flush = 0;
    device.ioctrl = ser_ioctrl;

    return cdev_register(&device);
}


/**
 * Install all the standard common serial interface devices on this machine
 * 
 * @return 0 on success, negative number on failure
 */
short ser_install_all() {
    short result = 0;

#ifdef SER_COM1
    // Install the first USB Serial COM port
    result = ser_install(SER_COM1, CDEV_COM1, "COM1");
    if (result) {
        return result;
    }
#endif

#ifdef SER_WIZFI
    // Install the first USB Serial COM port
    result = ser_install(SER_WIZFI, CDEV_COM2, "WIZFI");
    if (result) {
        return result;
    }
#endif

    return result;
}

/**
 * Initialize the common serial device system
 */
void ser_init() {
    for (short i = 0; i < CDEV_DEVICES_MAX; i++) {
        ser_device[i] = 0;
    }
}