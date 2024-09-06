#include "../../client/src/include/toolbox.h"

#include <string.h>

int main(int c, char * argv[]) {
	char * message = "Hello, Foenix Toolbox!\n";
	sys_chan_write(0, (uint8_t *)message, strlen(message));

	while (1) ;
}