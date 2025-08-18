/**
 * Uniform keyboard interface
 */

#include "kbd_uniform.h"

#include "ring_buffer.h"

//
// Module variables
//

static t_word_ring kbu_buffer;

//
// Functions
//

/**
 * Set the status LEDs on all attached keyboards
 * 
 * The bits of the status byte are assigned as in the PS/2 standard
 * - status[0] = scroll lock
 * - status[1] = number lock
 * - status[2] = caps lock
 * 
 * @param the status byte
 */
void kbd_set_status(uint8_t status) {
    // TODO: implement status LEDs
}

/**
 * Get the next ASCII character from the keyboard queue
 * 
 * @return the ASCII character corresponding to the next key pressed (0 if none)
 */
char kbd_getc() {
    char result = 0;

    // TODO: implement retrieving ASCII code

    return result;
}

/**
 * Get the next Toolbox scancode from the queue
 * 
 * @return the next scancode in the queue (0 if none)
 */
uint16_t kbd_get_scancode() {
    short result = 0;

    if (rb_word_empty(&kbu_buffer)) {
        return 0;
    } else {
        return rb_word_get(&kbu_buffer);
    }

    return result;
}

/**
 * Add a scancode to the keyboard buffer
 * 
 * NOTE: it is expect that this routine would be called by individual keyboard drivers
 * 
 * @param scancode the scancode to add
 */
void kbd_enqueue(uint16_t scancode) {
    if (!rb_word_full(&kbu_buffer)) {
        rb_word_put(&kbu_buffer, scancode);
    }
}

/**
 * Set the keyboard translation tables
 *
 * The translation tables provided to the keyboard consist of eight
 * consecutive tables of 128 characters each. Each table maps from
 * the MAKE scan code of a key to its appropriate 8-bit character code.
 *
 * The tables included must include, in order:
 * - UNMODIFIED: Used when no modifier keys are pressed or active
 * - SHIFT: Used when the SHIFT modifier is pressed
 * - CTRL: Used when the CTRL modifier is pressed
 * - CTRL-SHIFT: Used when both CTRL and SHIFT are pressed
 * - CAPSLOCK: Used when CAPSLOCK is down but SHIFT is not pressed
 * - CAPSLOCK-SHIFT: Used when CAPSLOCK is down and SHIFT is pressed
 * - ALT: Used when only ALT is presse
 * - ALT-SHIFT: Used when ALT is pressed and either CAPSLOCK is down
 *   or SHIFT is pressed (but not both)
 * 
 * @param tables pointer to the translation tables
 * @return 0 on success, negative number on error
 */
short kbd_set_layout(const uint8_t * tables) {
    short result = 0;

    // TODO: implement set layout

    return result;
}

/**
 * Initialize the uniform keyboard module
 * 
 * @return 0 on success, negative number on error
 */
short kbd_init() {
    short result = 0;

    // Intialize the scancode ringbuffer
    rb_word_init(&kbu_buffer);

    return result;
}