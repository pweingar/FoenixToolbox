/*
 * Startup file for the Foenix  Toolbox
 */

#include "log_level.h"
#define DEFAULT_LOG_LEVEL LOG_FATAL

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "features.h"
#include "sys_general.h"
#include "simpleio.h"
#include "log.h"
#include "boot.h"
#include "init.h"

#include "tests.h"
#include "test_cli.h"

extern unsigned long __memory_start;

int main(int argc, char * argv[]) {
    short result;
    short i;

    tb_init();
 	boot_screen();

#ifdef _CALYPSI_MCP_DEBUGGER
	extern int CalypsiDebugger(void);
	CalypsiDebugger(); // This will not return
#endif

    /* Infinite loop... */
    while (1) {
	};
}
