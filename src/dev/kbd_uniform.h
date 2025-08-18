/**
 * Uniform keyboard interface
 */

#ifndef __kbd_uniform__
#define __kbd_uniform__

#include <stdint.h>

//
// Types
//

struct kbd_instance_s;

typedef struct kbd_drv_s {
    short id;                                                               // The ID for this driver
    char * name;                                                            // The name of this type of keyboard

    short (*count)();                                                       // Return the number of this type of keyboard attached
    void * (*get_instance)(short n);                                        // Get the nth connected keyboard of this type
    
    short (*init)(struct kbd_instance_s * instance);                        // Initialize a particular keyboard
    short (*enable)(struct kbd_instance_s * instance);                      // Enable a particular keyboard
    short (*disable)(struct kbd_instance_s * instance);                     // Disable a particular keyboard
    short (*set_status)(struct kbd_instance_s * instance, uint8_t status);  // Set the status LEDs
} kbd_drv_t, *kbd_drv_p;

typedef struct kbd_instance_s {
    short id;                   // A unique ID for this specific keyboard
    short drv_id;               // The ID for the keyboard's driver
    short connection_type;      // Mechanical, Optical, PS/2, USB
    short model;                // A2560K, F256K Mechanical, FNX-KBD79, etc.
    short port;                 // Implementation specific number to address the physical keyboard
    void * data;                // Private, instance-specific data
} kbd_instance_t, *kbd_instance_p;

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
extern void kbd_set_status(uint8_t status);

/**
 * Get the next ASCII character from the keyboard queue
 * 
 * @return the ASCII character corresponding to the next key pressed (0 if none)
 */
extern char kbd_getc();

/**
 * Get the next Toolbox scancode from the queue
 * 
 * @return the next scancode in the queue (0 if none)
 */
extern uint16_t kbd_get_scancode();

/**
 * Add a scancode to the keyboard buffer
 * 
 * NOTE: it is expect that this routine would be called by individual keyboard drivers
 * 
 * @param scancode the scancode to add
 */
extern void kbd_enqueue(uint16_t scancode);

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
extern short kbd_set_layout(const uint8_t * tables);

/**
 * Initialize the uniform keyboard module
 * 
 * @return 0 on success, negative number on error
 */
extern short kbd_init();

#endif