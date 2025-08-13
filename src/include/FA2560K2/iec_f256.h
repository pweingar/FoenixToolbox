/**
 * Define the registers for the IEC (Commodore Serial) interface
 */

#ifndef __iec_f256_h__
#define __iec_f256_h__

#define IEC_CMD_LISTEN		0x20
#define IEC_CMD_TALK		0x40
#define IEC_CMD_OPENCH		0x60
#define IEC_CMD_CLOSE		0xE0
#define IEC_CMD_OPEN		0xF0
#define IEC_CMD_UNLISTEN	0x3F
#define IEC_CMD_UNTALK		0x5F

#define IEC_INPUT_PORT		0xf01680
#define IEC_DATA_i			0x01
#define IEC_CLK_i			0x02
#define IEC_ATN_i			0x10
#define IEC_SREQ_i			0x80

#define IEC_OUTPUT_PORT		0xf01681
#define IEC_DATA_o			0x01
#define IEC_CLK_o			0x02
#define IEC_ATN_o			0x10
#define IEC_RST_o			0x40
#define IEC_SREQ_o			0x80

#endif