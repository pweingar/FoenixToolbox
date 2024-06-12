/*
 * Startup file for the Foenix  Toolbox
 */

#include "log_level.h"
#define DEFAULT_LOG_LEVEL LOG_DEBUG
#define LOG_CHANNEL LOG_CHANNEL_CHANNEL_A

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "features.h"
#include "sys_general.h"
#include "simpleio.h"
#include "log.h"
#include "indicators.h"
#include "interrupt.h"
#include "gabe_reg.h"

#if HAS_SUPERIO
#include "superio.h"
#endif

#if MODEL == MODEL_FOENIX_A2560K
#include "dev/kbd_mo.h"
#endif
#if HAS_DUAL_SCREEN
#include "dev/txt_a2560k_a.h"
#include "dev/txt_a2560k_b.h"
#elif MODEL == MODEL_FOENIX_A2560U || MODEL == MODEL_FOENIX_A2560U_PLUS
#include "dev/txt_a2560u.h"
#elif MODEL == MODEL_FOENIX_C256U || MODEL == MODEL_FOENIX_C256U_PLUS || MODEL == MODEL_FOENIX_FMX
#include "dev/txt_c256.h"
#include "dev/txt_evid.h"
#endif

#include "syscalls.h"
#include "timers.h"
#include "boot.h"
#include "dev/bitmap.h"
#include "memory.h"
#include "dev/block.h"
#include "dev/channel.h"
#include "dev/console.h"
#include "dev/dma.h"
#include "dev/fdc.h"
#include "dev/fsys.h"
#include "dev/ps2.h"
#include "dev/rtc.h"
#include "dev/sdc.h"
#include "dev/txt_screen.h"

#include "dev/uart.h"
#include "snd/codec.h"
#include "snd/psg.h"
#include "snd/sid.h"
#include "snd/yamaha.h"
#include "vicky_general.h"
#include "fatfs/ff.h"
#include "rsrc/font/MSX_CP437_8x8.h"
#include "rsrc/bitmaps/splash_c256_u.h"

const char* VolumeStr[FF_VOLUMES] = { "sd0" };

extern unsigned long __memory_start;

void print_error(short channel, char * message, short code) {
	// TODO: bring back...
    // print(channel, message);
    // print(channel, ": ");
    // print_hex_16(channel, code);
    // print(channel, "\n");
}

t_sys_info info;

/*
 * Initialize the kernel systems.
 */
void initialize() {
    long target_jiffies;
    int i;
    short res;

    /* Setup logging early */
    log_init();
	log_setlevel(DEFAULT_LOG_LEVEL);

	/* Fill out the system information */
	sys_get_information(&info);

    /* Initialize the memory system */
    mem_init(0x3d0000);

    /* Hide the mouse */
    mouse_set_visible(0);

    /* Initialize the text channels */
    txt_init();
#if HAS_DUAL_SCREEN
    txt_a2560k_a_install();
    txt_a2560k_b_install();
    log(LOG_INFO, "Initializing screens...");
    txt_init_screen(TXT_SCREEN_A2560K_A);
    txt_init_screen(TXT_SCREEN_A2560K_B);

#elif MODEL == MODEL_FOENIX_A2560U || MODEL == MODEL_FOENIX_A2560U_PLUS
    txt_a2560u_install();
    txt_init_screen(TXT_SCREEN_A2560U);

#elif MODEL == MODEL_FOENIX_C256U || MODEL == MODEL_FOENIX_C256U_PLUS || MODEL == MODEL_FOENIX_FMX
	// Install and initialize the main screen text driver
	txt_c256_install();
	txt_init_screen(TXT_SCREEN_C256);

	// If the EVID card is plugged in, install and initialize the EVID driver
	if (info.screens > 1) {
		short result = txt_evid_install();
		txt_init_screen(TXT_SCREEN_EVID);
	}

#else
#error Cannot identify screen setup
#endif

	INFO("Text system initialized...");

	// Initialize the bitmap system
	bm_init();
	INFO("Bitmap system initialized...");

    /* Initialize the indicators */
    ind_init();
    INFO("Indicators initialized");

    /* Initialize the interrupt system */
    int_init();
	INFO("Interrupts initialized");

    /* Mute the PSG */
    psg_mute_all();

    /* Initialize and mute the SID chips */
    sid_init_all();

//     /* Initialize the Yamaha sound chips (well, turn their volume down at least) */
//     ym_init();

    /* Initialize the CODEC */
    init_codec();

    cdev_init_system();   // Initialize the channel device system
    INFO("Channel device system ready.");

    bdev_init_system();   // Initialize the channel device system
    INFO("Block device system ready.");

    if ((res = con_install())) {
        log_num(LOG_ERROR, "FAILED: Console installation", res);
    } else {
        INFO("Console installed.");
    }

    /* Initialize the timers the MCP uses */
    timers_init();
	INFO("Timers initialized");

    /* Initialize the real time clock */
    rtc_init();
	INFO("Real time clock initialized");

    target_jiffies = sys_time_jiffies() + 300;     /* 5 seconds minimum */
    DEBUG1("target_jiffies assigned: %d", target_jiffies);

    /* Enable all interrupts */
    int_enable_all();
    TRACE("Interrupts enabled");

//     /* Play the SID test bong on the Gideon SID implementation */
//     sid_test_internal();

    // if ((res = pata_install())) {
    //     log_num(LOG_ERROR, "FAILED: PATA driver installation", res);
    // } else {
    //     INFO("PATA driver installed.");
    // }

    if ((res = sdc_install())) {
        ERROR1("FAILED: SDC driver installation %d", res);
    } else {
        INFO("SDC driver installed.");
    }

#if HAS_FLOPPY
    if ((res = fdc_install())) {
        ERROR1("FAILED: Floppy drive initialization %d", res);
    } else {
        INFO("Floppy drive initialized.");
    }
#endif

    // At this point, we should be able to call into to console to print to the screens

    if ((res = ps2_init())) {
        print_error(0, "FAILED: PS/2 keyboard initialization", res);
    } else {
        log(LOG_INFO, "PS/2 keyboard initialized.");
    }

#if MODEL == MODEL_FOENIX_A2560K
    if ((res = kbdmo_init())) {
        log_num(LOG_ERROR, "FAILED: A2560K built-in keyboard initialization", res);
    } else {
        log(LOG_INFO, "A2560K built-in keyboard initialized.");
    }
#endif

#if HAS_PARALLEL_PORT
    if ((res = lpt_install())) {
        log_num(LOG_ERROR, "FAILED: LPT installation", res);
    } else {
        log(LOG_INFO, "LPT installed.");
    }
#endif

#if HAS_MIDI_PORTS
    if ((res = midi_install())) {
        log_num(LOG_ERROR, "FAILED: MIDI installation", res);
    } else {
        log(LOG_INFO, "MIDI installed.");
    }
#endif

//     if (res = uart_install()) {
//         log_num(LOG_ERROR, "FAILED: serial port initialization", res);
//     } else {
//         log(LOG_INFO, "Serial ports initialized.");
//     }

    if ((res = fsys_init())) {
        log_num(LOG_ERROR, "FAILED: file system initialization", res);
    } else {
        INFO("File system initialized.");
    }
}

int main(int argc, char * argv[]) {
    short result;
    short i;
	char message[256];

    initialize();

	// Attempt to start up the user code
    log(LOG_INFO, "Looking for user startup code:");
	boot_launch();

	printf("Done.\n");

#ifdef _CALYPSI_MCP_DEBUGGER
	extern int CalypsiDebugger(void);
	CalypsiDebugger(); // This will not return
#endif

    /* Infinite loop... */
    while (1) {
	};
}
