
#include <stdint.h>

#define VKY_TXT_CHAR_A	((volatile char *)0xfeca0000)
#define VKY_TXT_COLOR_A	((volatile uint8_t *)0xfeca8000)

uint16_t vky_txt_pos = 0;

void vky_txt_emit(char c) {
	VKY_TXT_CHAR_A[vky_txt_pos] = c;
	VKY_TXT_COLOR_A[vky_txt_pos++] = 0xf0;
}

int main(int argc, char * argv[]) {
	vky_txt_pos = 0;

	vky_txt_emit('H');
	vky_txt_emit('e');
	vky_txt_emit('l');
	vky_txt_emit('l');
	vky_txt_emit('o');

	while(1) ;
}