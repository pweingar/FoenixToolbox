	.extern __program_start

	.section header

signature:	.byte	0xf8, 0x16
version:	.byte	0
start:		.long	__program_start
icon:		.long	0
clut:		.long	0
name:		.asciz	"imageview"
