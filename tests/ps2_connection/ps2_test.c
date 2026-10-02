#include <stdint.h>
#include <calypsi/intrinsics68000.h>

/** Text Matrix for Channel B */
#define TEXT_MATRIX     ((volatile uint8_t *)0xFECA0000)

/** Color Matrix for Channel B */
#define COLOR_MATRIX    ((volatile uint8_t *)0xFECA8000)

#define PS2_DATA        ((volatile uint8_t *)0xFEC02060)
#define PS2_CMD         ((volatile uint8_t *)0xFEC02064)
#define PS2_STATUS      ((volatile uint8_t *)0xFEC02064)
#define PS2_STAT_OBF    0x01
#define PS2_STAT_IBF    0x02

/**
 * Wait for the PS2 controller to be ready to receive data to its input buffer.
 */
void ps2_wait_in() {
    while ((*PS2_STATUS & PS2_STAT_IBF) != 0) ;
}

/**
 * Wait for the PS2 controller to have data ready to read from its output bufffer.
 */
void ps2_wait_out() {
    while ((*PS2_STATUS & PS2_STAT_OBF) == 0) ;
}

/**
 * Send a command to the PS/2 input buffer.
 */
void ps2_send_cmd(uint8_t b) {
    ps2_wait_in();
    *PS2_CMD = b;
}

/**
 * Send a data byte to the PS/2 input buffer.
 */
void ps2_send_data(uint8_t b) {
    ps2_wait_in();
    *PS2_DATA = b;
}

/**
 * Get a byte from the PS/2 output buffer.
 * 
 * @return the byte read
 */
uint8_t ps2_get_data() {
    ps2_wait_out();
    return *PS2_DATA;
}

const char hex_digits[] = "0123456789ABCDEF";

/**
 * Write the hex display code to the screen
 */
void display_code(short offset, uint8_t code) {
    const uint8_t color = 0x0f;

    TEXT_MATRIX[offset] = hex_digits[code >> 4];
    TEXT_MATRIX[offset+1] = hex_digits[code & 0x0f];
    COLOR_MATRIX[offset] = color;
    COLOR_MATRIX[offset+1] = color;
}

int main(int argc, char argv[]) {
    // We don't want anything grabbing out data
    __disable_interrupts();

    display_code(0, 0xde);
    display_code(2, 0xad);

    do {
        uint8_t code = ps2_get_data();
        display_code(4, code);
    } while (1);

    return 0;
}