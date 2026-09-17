#include <stdio.h>
#include <stdint.h>

#define BDEV_DEVICES_MAX    8

/*
 * Structure defining a block device's functions
 */

typedef struct s_dev_block {
    short number;           														// The number of the device (assigned by registration)
    char * name;            														// The name of the device
	void * data;																	// Device-specific data block
    short (*init)(struct s_dev_block *);        									// Initialize the device
    short (*read)(struct s_dev_block *, long lba, uint8_t * buffer, short size);	// Read a block from the device
    short (*write)(struct s_dev_block *, long lba, const uint8_t * buffer, short size);	// Write a block to the device
    short (*status)(struct s_dev_block *);      									// Get the status of the device
    short (*flush)(struct s_dev_block *);      										// Ensure that any pending writes to the device have been completed
    short (*ioctrl)(struct s_dev_block *, short command, unsigned char * buffer, short size);	// Issue a control command to the device
} t_dev_block, *p_dev_block;


t_dev_block g_block_devs[BDEV_DEVICES_MAX];

short bdev_init(short dev) {
    printf("bdev_init(%d)\n", (int)dev);

    short ret = -2;

    if (dev < BDEV_DEVICES_MAX) {
        printf("bdev_init: number within range\n");
        p_dev_block bdev = &g_block_devs[dev];
        if (bdev->number == dev) {
            ret = bdev->init(bdev);
            printf("bdev_init: called device init => %d\n", ret);
		} else {
            printf("bdev_init: dev (%d) != record (%d)\n", dev, bdev->number);
        }
    }

    printf("bdev_init returning %d\n", (int)ret);
    return ret;
}

int main(int argc, char * argv[]) {
    for (int i = 0; i < BDEV_DEVICES_MAX; i++) {
        g_block_devs[i].number = -1;
    }

    g_block_devs[0].number = 0;
    g_block_devs[0].name = "TEST0";

    g_block_devs[1].number = 1;
    g_block_devs[1].name = "TEST1";

    short result = bdev_init(1);
    if (result < 0) {
        printf("Failed to initialize\n");
    } else {
        printf("Initialized\n");
    }

    return 0;
}