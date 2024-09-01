/*
 * Startup file for the Foenix  Toolbox
 */

#include "log_level.h"
#define DEFAULT_LOG_LEVEL LOG_INFO
#define LOG_CHANNEL LOG_CHANNEL_UART0

#include <ctype.h>
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
#elif MODEL == MODEL_FOENIX_F256 || MODEL == MODEL_FOENIX_F256K || MODEL == MODEL_FOENIX_F256K2
#include "cartridge.h"
#include "dev/txt_f256.h"
#include "dev/kbd_f256.h"
#include "dev/sdc_f256.h"
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
#include "dev/iec.h"
#include "iecll.h"
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
	INFO3("\n\rFoenix Toolbox v%d.%02d.%04d starting up...", VER_MAJOR, VER_MINOR, VER_BUILD);

	/* Fill out the system information */
	sys_get_information(&info);

    /* Initialize the memory system */
    mem_init(0x3d0000);

    // /* Hide the mouse */
    // mouse_set_visible(0);

    /* Initialize the text channels */
	INFO("Initializing the text system...");
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

#elif MODEL == MODEL_FOENIX_F256 || MODEL == MODEL_FOENIX_F256K || MODEL == MODEL_FOENIX_F256K2
	*vky_brdr_col_red = 0x80;
	*vky_brdr_col_green = 0x00;
	*vky_brdr_col_blue = 0x80;

	txt_f256_install();

	txt_init_screen(TXT_SCREEN_F256);
#else
#error Cannot identify screen setup
#endif

	INFO("Text system initialized.");

// 	// Initialize the bitmap system
// 	bm_init();
// 	INFO("Bitmap system initialized...");

    /* Initialize the indicators */
    ind_init();
    INFO("Indicators initialized");

    /* Initialize the interrupt system */
    int_init();
	INFO("Interrupts initialized");

    /* Mute the PSG */
    psg_mute_all();
	INFO("PSG Muted.");

    /* Initialize and mute the SID chips */
    sid_init_all();

// //     /* Initialize the Yamaha sound chips (well, turn their volume down at least) */
// //     ym_init();

    /* Initialize the CODEC */
    init_codec();
	INFO("CODEC initialized.");

    cdev_init_system();   // Initialize the channel device system
    INFO("Channel device system ready.");

    bdev_init_system();   // Initialize the channel device system
    INFO("Block device system ready.");

    if ((res = con_install())) {
        log_num(LOG_ERROR, "FAILED: Console installation", res);
    } else {
        INFO("Console installed.");
    }

// #if HAS_IDE
// 	iec_init();
// #endif

    /* Initialize the timers the MCP uses */
    timers_init();
	INFO("Timers initialized");

    /* Initialize the real time clock */
    // rtc_init();
	// INFO("Real time clock initialized");

    /* Enable all interrupts */
    int_enable_all();
    INFO("Interrupts enabled");

    // /* Play the SID test bong on the Gideon SID implementation */
    // sid_test_internal();

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

//     if ((res = ps2_init())) {
//         ERROR1("FAILED: PS/2 keyboard initialization", res);
//     } else {
//         log(LOG_INFO, "PS/2 keyboard initialized.");
//     }

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

t_file_info dir;
uint8_t buffer[512];

void dump(uint8_t * buffer, int count) {
	char char_buffer[17];

	printf("\n");

	short index = 0;
	for (int i = 0; i < count; i++) {
		if ((i > 0) && (i % 16 == 0)) {
			index = 0;
			char_buffer[16] = 0;
			printf(" %s\n", char_buffer);
		} else if (i > 0) {
			char c = buffer[i];
			printf("%02X ", c);

			if (isalpha(c) || isdigit(c)) {
				char_buffer[index++] = c;
			} else {
				char_buffer[index++] = '.';
			}
		}
	}

	printf(" %s\n", char_buffer);
}

union fatfs_date_u {
	struct {
		unsigned int day : 5;
		unsigned int month : 4;
		unsigned int year : 7;
	} s;
	short date;
};

union fatfs_time_u {
	struct {
		unsigned int second : 5;
		unsigned int minute : 6;
		unsigned int hour : 5;
	} s;
	short time;
};

void print_fatfs_datetime(short date, short time) {
	union fatfs_date_u fat_date;
	union fatfs_time_u fat_time;
	
	fat_date.date = date;
	fat_time.time = time;

	printf("%04d-%02d-%02d %02d:%02d ", fat_date.s.year + 1980, fat_date.s.month, fat_date.s.day, fat_time.s.hour, fat_time.s.minute);
}

void print_directory() {
	printf("\nDirectory for /sd0/\n");
	short fd = fsys_opendir("/sd0/");
	if (fd > -1) {
		INFO("fsys_opendir");

		short result = fsys_readdir(fd, &dir);
		while ((result == 0) && (dir.name[0] != 0)) {
			if (dir.name[0] == 0) {
				break;
			} else {
				if ((dir.attributes & FSYS_AM_SYS) == 0) {
					print_fatfs_datetime(dir.date, dir.time);
					printf(" %4ld ", dir.size);

					if (dir.attributes & FSYS_AM_DIR) {
						printf(" %s/\n", dir.name);
					} else {
						printf(" %s\n", dir.name);
					}
				}

				result = fsys_readdir(fd, &dir);
			}
		}

		fsys_closedir(fd);
		INFO("fsys_closedir");
	} else {
		ERROR1("Could not open directory %d", fd);
	}
}

void create_sample_file(const char * path) {
	printf("\nTrying to create: %s\n", path);
	short fd = fsys_open(path, FSYS_CREATE_ALWAYS | FSYS_WRITE);
	if (fd > 0) {
		char message[80];
		printf("Got channel #%d\n", fd);
		sprintf(message, "Hello, world!\n");
		short result = chan_write(fd, (uint8_t *)message, strlen(message));
		printf("Wrote %d characters.\n", result);
		fsys_close(fd);

	} else {
		printf("Could not create file: %d\n", fd);
	}
}

void read_sample_file(const char * path) {
	printf("\nContents of %s:\n", path);
	short fd = fsys_open(path, FSYS_READ);
	if (fd >= 0) {
		short c = 0;
		short status;
		do {
			c = chan_read_b(fd);
			chan_write_b(0, (uint8_t)c);
			status = chan_status(fd);
		} while ((status & CDEV_STAT_EOF) == 0);
		chan_close(fd);

	} else {
		printf("Could not open file: %d\n", fd);
	}
}

void test_sdc() {
	print_directory();
	
	printf("\nfsys_rename(\"/sd0/hello.txt\", \"/sd0/renamed.txt\")");
	fsys_rename("/sd0/hello.txt", "/sd0/renamed.txt");
	print_directory();

	printf("\nfsys_delete(\"/sd0/renamed.txt\")");
	fsys_delete("/sd0/renamed.txt");
	print_directory();

	printf("\nCreating /sd0/hello.txt\n");
	create_sample_file("/sd0/hello.txt");
	print_directory();

	read_sample_file("/sd0/test.txt");
	read_sample_file("/sd0/hello.txt");
}

void test_kbd_sc() {
	printf("> ");
	do {
		unsigned short scancode = kbd_get_scancode();
		if (scancode != 0) {
			printf("%04X ", scancode);
		}
	} while (!kbd_break());
	printf("\n\n");
}

void test_kbd() {
	printf("> ");
	do {
		kbd_handle_irq();
		char c = kbd_getc();
		if (c != 0) {
			txt_put(0, c);
		}
	} while (!kbd_break());
	printf("\n\n");
}

void test_psg() {
	long target_time = rtc_get_jiffies() + (long)(60 * 2);

	psg_tone(3, 0, 262);
	psg_tone(3, 1, 262 * 2);
	psg_tone(3, 2, 262 * 4);

	psg_attenuation(3, 0, 0);
	psg_attenuation(3, 1, 15);
	psg_attenuation(3, 2, 15);

	while (target_time > rtc_get_jiffies()) {
		;
	}

	psg_attenuation(3, 0, 15);
	psg_attenuation(3, 1, 15);
	psg_attenuation(3, 2, 15);
}

void test_sysinfo() {
	// 8 x 22 region
	t_rect region;
	region.size.height = 8;
	region.size.width = 23;
	region.origin.x = 80 - region.size.width;
	region.origin.y = 60 - region.size.height;

	txt_set_region(0, &region);
	
	printf("Foenix Retro Systems\n");
	printf("Model   %s\n", info.model_name);
	printf("CPU     %s\n", info.cpu_name);
	printf("Clock   %lu MHz\n", info.cpu_clock_khz / (long)1000);
	printf("Memory  %d MB\n", (int)(info.system_ram_size / ((long)1024 * (long)1024)));
	printf("FPGA    %04X %04X.%04X\n", info.fpga_model, info.fpga_version, info.fpga_subver);
	printf("Toolbox v%d.%02d.%04d\n", info.mcp_version, info.sub_model, info.mcp_build);

	region.size.width = 0;
	region.size.height = 0;
	region.origin.x = 0;
	region.origin.y = 0;

	txt_set_region(0, &region);
	txt_set_xy(0, 0, 0);
}

int main(int argc, char * argv[]) {
    short result;
    short i;
	char message[256];

    initialize();
	kbd_init();

	test_sysinfo();

	// printf("Foenix Toolbox v%d.%04d.%04d\n", VER_MAJOR, VER_MINOR, VER_BUILD);

	// const char * test_data = "Hello, world!";

	// volatile uint8_t * cartridge = ((volatile uint8_t *)0xf40000);

	// short cartridge_id = cart_id();
	// switch(cartridge_id) {
	// 	case CART_ID_NONE:
	// 		printf("No cartridge detected.\n");
	// 		break;

	// 	case CART_ID_RAM:
	// 		printf("RAM cartridge detected.\n");
	// 		break;

	// 	case CART_ID_FLASH:
	// 		printf("FLASH cartridge detected.\n");
	// 		// printf("Attempting to erase the flash cartridge.\n");
	// 		// cart_erase();
	// 		// printf("Attempting to program the flash cartridge.\n");
	// 		// for (int i = 0; i < strlen(test_data); i++) {
	// 		// 	cart_write(0xf40000 + i, test_data[i]);
	// 		// }

	// 		// for (int j = 0; j < strlen(test_data); j++) {
	// 		// 	txt_put(0, cartridge[j]);
	// 		// }

	// 		// printf("\n");
	// 		break;

	// 	default:
	// 		printf("Unable to determine whether a cartridge is present.\n");
	// 		break;
	// }

	// test_kbd();

	boot_screen();

	while (1) ;



	// printf("Initializing IEC\n");
	// result = iec_init();
	// if (result != 0) {
	// 	printf("Error initializing IEC.\n");
	// }

	// printf("Attempting to get status for IEC drive #8: ");
	// short n = iec_status(8, message, 256);
	// printf("\"%s\"\n", message);

	// printf("Attempting to write to the printer.\n");
	// iec_print(4, "\e1THIS IS PRINTED FROM AN F256K OVER THE IEC PORT!\r");

	// Attempt to start up the user code
    // log(LOG_INFO, "Looking for user startup code:");
	// boot_launch();

	printf("Done.\n");

#ifdef _CALYPSI_MCP_DEBUGGER
	extern int CalypsiDebugger(void);
	CalypsiDebugger(); // This will not return
#endif

    /* Infinite loop... */
    while (1) {
	};
}
