#include "../../client/src/include/toolbox.h"

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

t_file_info file_info;

void myprint(const char * text) {
	int length = strlen(text);
	sys_chan_write(0, (const uint8_t *)text, length);
}

int main(int c, char * argv[]) {
	char line[80];

	myprint("BMP ImageViewer for the Foenix F256\n");

	short dir = sys_fsys_opendir("/sd0");
	if (dir >= 0) {
		bool keep_looping = true;
		while (keep_looping) {
			short result = sys_fsys_readdir(dir, &file_info);
			if (result == 0) {
				if (file_info.name[0] != 0) {
					sprintf(line, "%s\n", file_info.name);
					myprint(line);

				} else {
					keep_looping = false;
				}

			} else {
				sprintf(line, "Could not read directory: %d\n", dir);
				myprint(line);
				keep_looping = false;
			}
		}
		sys_fsys_closedir(dir);


	} else {
		sprintf(line, "Could not open directory: %d\n", dir);
		myprint(line);
	}

	while (1) ;
}