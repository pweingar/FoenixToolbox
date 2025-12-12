/**
 * Low level functions for the A2560 machines
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "../dev/indicators.h"
#include "../dev/txt_screen.h"

const short panic_screen = 0;
const short panic_msg_x = 0;
const short panic_msg_y = 0;

/**
 * Program counter at the time of an exception
 */
uint32_t except_pc = 0;

/**
 * Effective address that caused an exception
 */
uint32_t except_addr = 0;

static char except_line[80];

/**
 * Print the panic message to the screen
 */
static void panic_print() {
    ind_set(IND_POWER, IND_ERROR);
    txt_init_screen(panic_screen);
    txt_set_xy(panic_screen, panic_msg_x, panic_msg_y);
    txt_print(panic_screen, except_line);

    while(1) ;
}

uint32_t __low_level_init() {
	// Return the code to make sure we stay in supervisor mode (Calypsi-dependent)
    except_pc = 0;
    except_addr = 0;
    except_line[0] = 0;
	return 1;
}

/**
 * Handler for the bus exception
 */
__attribute__((interrupt(0x0008))) void bus_error() {
    __asm(
        " move.l (18,sp),except_pc"
    );

    sprintf(except_line, "Bus error at 0x%08X\n", except_pc);
    panic_print();
}

/**
 * Handler for the addresss exception
 */
__attribute__((interrupt(0x000c))) void address_error() {
    __asm(
        " move.l (18,sp),except_pc.l\n"
        " move.l (24,sp),except_addr.l\n"
    );

    sprintf(except_line, "Address error at 0x%08X [0x%08X]\n", except_pc, except_addr);
    panic_print();
}

/**
 * Handler for the illegal instruction exception
 */
__attribute__((interrupt(0x0010))) void illegal_error() {
    __asm(
        " move.l (18,sp),except_pc.l"
    );

    sprintf(except_line, "Illegal instruction at 0x%08X\n", except_pc);
    panic_print();
}

/**
 * Handler for the divide by zero exception
 */
__attribute__((interrupt(0x0014))) void div0_error() {
    __asm(
        " move.l (18,sp),except_pc.l"
    );

    sprintf(except_line, "Divide by zero at 0x%08X\n", except_pc);
    panic_print();
}

/**
 * Handler for the privilege exception
 */
__attribute__((interrupt(0x0020))) void priv_error() {
    __asm(
        " move.l (18,sp),except_pc.l"
    );

    sprintf(except_line, "Privilege error at 0x%08X\n", except_pc);
    panic_print();
}
