/**
 * Root file for the Foenix Toolbox Bootstrap package
 */

#include <stdio.h>

#include "../src/init.h"

#include "boot.h"

int main(int argc, char * argv[]) {
    // Initialize the Toolbox
    tb_init();

    // Display the boot screen
 	boot_screen();

    /* Infinite loop... */
    while (1) {
	};
}