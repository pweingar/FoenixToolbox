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
#include "dev/indicators.h"
#include "interrupt.h"
#include "gabe_reg.h"
#include "superio.h"

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
#elif MODEL == MODEL_FOENIX_F256 || MODEL == MODEL_FOENIX_F256K || MODEL == MODEL_FOENIX_F256K2 || MODEL == MODEL_FOENIX_F256JR2
#include "cartridge.h"
#include "dev/txt_f256.h"
#include "dev/kbd_f256.h"
#elif MODEL == MODEL_FOENIX_FA2560K2
#include "FA2560K2/vkyii_legacy_Channelb.h"
#include "dev/txt_fa2560k2.h"
#endif

#include "syscalls.h"
#include "timers.h"
#include "boot.h"
#include "memory.h"
#include "dev/block.h"
#include "dev/channel.h"
#include "dev/console.h"
#include "dev/dma.h"

// Reference the PATA/IDE driver, if needed
#if HAS_PATA
#include "dev/pata.h"
#endif

// // Reference the floppy drive driver, if needed
// #if HAS_FLOPPY
// #include "dev/fdc.h"
// #endif

// Reference the correct SDC driver
#if HAS_SDC_SPI
#include "dev/sdc_spi.h"
#else
#include "dev/sdc.h"
#endif

#include "dev/fsys.h"
#include "dev/iec.h"
#include "iecll.h"
#include "dev/ps2.h"
#include "dev/rtc.h"
#include "dev/txt_screen.h"
#include "dev/uart.h"
#include "snd/codec.h"
#include "snd/psg.h"
#include "snd/sid.h"
#if HAS_OPN || HAS_OPM || HAS_OPL3
#include "snd/yamaha.h"
#endif

#include "vicky_general.h"
#include "fatfs/ff.h"
#include "rsrc/font/MSX_CP437_8x8.h"

#include "tests.h"

// The list of drives for FATFS
#if HAS_PATA
// Machines with an IDE/PATA interface have an internal hard drive
const char* VolumeStr[FF_VOLUMES] = { "sd0", "hd0" };
#else
// Otherwise, machines have an internal SD card
const char* VolumeStr[FF_VOLUMES] = { "sd0", "sd1" };
#endif

extern unsigned long __memory_start;

t_sys_info info;

/*
 * Initialize the kernel systems.
 */
void initialize() {
    long target_jiffies;
    int i;
    short res;

#if HAS_SUPERIO
	// First thing... make sure that the SuperIO is initialized
    unreset_lpc();
    configure_zones(); // This Init used to be done by the FPGA.
	init_superio();
#endif

    /* Setup logging early */
    log_init();
    log_setlevel(DEFAULT_LOG_LEVEL);

	INFO3("\n\rFoenix Toolbox v%d.%02d.%04d starting up...", VER_MAJOR, VER_MINOR, VER_BUILD);

	/* Fill out the system information */
	sys_get_information(&info);

    /* Initialize the memory system */
    mem_init();

    // /* Hide the mouse */
    // mouse_set_visible(0);

    /* Initialize the text channels */
    txt_init();
#if HAS_DUAL_SCREEN
    txt_a2560k_a_install();
    txt_a2560k_b_install();
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

#elif MODEL == MODEL_FOENIX_F256 || MODEL == MODEL_FOENIX_F256K || MODEL == MODEL_FOENIX_F256K2 || MODEL == MODEL_FOENIX_F256JR2
	txt_f256_install();
	txt_init_screen(TXT_SCREEN_F256);

#elif MODEL == MODEL_FOENIX_FA2560K2
    txt_fa2560k2_install();
    txt_init_screen(TXT_SCREEN_FA2560K2);

#else
#error Cannot identify screen setup
#endif

	INFO("Text system initialized.");
	INFO1("Top of memory: %lx", mem_get_ramtop());

    /* Initialize the indicators */
    ind_init();
    INFO("Indicators initialized");

    /* Initialize the interrupt system */
    int_init();
	INFO("Interrupts initialized");

    /* Mute the PSG */
    psg_mute_all();
	INFO("PSG initialized.");

    /* Initialize and mute the SID chips */
    sid_init_all();
	INFO("SID chips initialized.");

#if HAS_OPN || HAS_OPM || HAS_OPL3
    /* Initialize the Yamaha sound chips (well, turn their volume down at least) */
    ym_init();
	INFO("Yamaha initialized.");
#endif

    /* Initialize the CODEC */
    init_codec();
	INFO("CODEC initialized.");

    cdev_init_system();   // Initialize the channel device system
    INFO("Channel device system ready.");

    bdev_init_system();   // Initialize the channel device system
    INFO("Block device system ready.");

    if ((res = con_install())) {
		ERROR1("FAILED: Console installation", res);
    } else {
        INFO("Console installed.");
    }

#if HAS_IEC
	iec_init();
#endif

    /* Initialize the timers the MCP uses */
    timers_init();
	INFO("Timers initialized");

    /* Initialize the real time clock */
    rtc_init();
	INFO("Real time clock initialized");

    /* Enable all interrupts */
    int_enable_all();
    INFO("Interrupts enabled");

    /* Play the SID test bong on the Gideon SID implementation */
    printf("\e[1;3HTesting SID...\n");
    sid_test_internal();
	INFO("SID boot bong played.");
    printf("SID boot bong played.\n");

#if HAS_PATA
    if ((res = pata_install())) {
        log_num(LOG_ERROR, "FAILED: PATA driver installation", res);
    } else {
        INFO("PATA driver installed.");
    }
#endif

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

//     // At this point, we should be able to call into to console to print to the screens

//     // if ((res = ps2_init())) {
//     //     ERROR1("FAILED: PS/2 keyboard initialization", res);
//     // } else {
//     //     log(LOG_INFO, "PS/2 keyboard initialized.");
//     // }

// 	// Initialize the keyboard
// 	kbd_init();
// 	INFO("Keyboard initialized");

// #if MODEL == MODEL_FOENIX_A2560K
//     if ((res = kbdmo_init())) {
//         log_num(LOG_ERROR, "FAILED: A2560K built-in keyboard initialization", res);
//     } else {
//         log(LOG_INFO, "A2560K built-in keyboard initialized.");
//     }
// #endif

#if HAS_PARALLEL_PORT
    if ((res = lpt_install())) {
        log_num(LOG_ERROR, "FAILED: LPT installation", res);
    } else {
        log(LOG_INFO, "LPT installed.");
    }
#endif

// #if HAS_MIDI_PORTS
//     if ((res = midi_install())) {
//         log_num(LOG_ERROR, "FAILED: MIDI installation", res);
//     } else {
//         log(LOG_INFO, "MIDI installed.");
//     }
// #endif

//     if ((res = uart_install()) != 0) {
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

void int_sof_test() {
    VKY3_B_TEXT_MATRIX[0]++;
}

int main(int argc, char * argv[]) {
    short result;
    short i;
    char message[256];

    initialize();

	printf("Foenix Toolbox v%d.%02d.%04d\n", VER_MAJOR, VER_MINOR, VER_BUILD);
    printf("Model: %s\n", info.model_name);
    int clock_MHz = (int)(info.cpu_clock_khz / 1000L);
	printf("CPU:   %s at %d MHz\n", info.cpu_name, clock_MHz);

    int_register(INT_SOF_A, int_sof_test);
    int_enable(INT_SOF_A);

    // // test_hd();
    // test_sd0();
    // test_sd1();

    printf("\nTesting FATFS:\n");
    test_dir("/sd0");
    printf("\n");
    test_dir("/sd1");

    long jiffies = timers_jiffies();
    do {
        long current = timers_jiffies();
        if (current >= jiffies + 60) {
            t_time time;
            rtc_get_time(&time);
            printf("\e[40;1H%02d:%02d:%02d", time.hour, time.minute, time.second);
            jiffies = current;
        }
    } while (1);

    // printf("\n\nShould display boot screen here.\n");

 	// boot_screen();

#ifdef _CALYPSI_MCP_DEBUGGER
	extern int CalypsiDebugger(void);
	CalypsiDebugger(); // This will not return
#endif

    /* Infinite loop... */
    while (1) {
	};
}
