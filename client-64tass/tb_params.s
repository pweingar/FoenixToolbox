
;
; Allocate a number of bytes onto the stack for parameters
;
alloc		.macro count
			.switch \count
			.case 0
			.case 2
			pea #0		; 5
			.case 4
			pea #0		; 10
			pea #0
			.case 6
			pea #0		; 15
			pea #0
			pea #0
			.default
			sta #0,d

			tsc
			sec
			sbc #\count
			tcs

			lda #0,d
			.endswitch
			.endm

;
; Remove a number of bytes from the stack (previously alloc'ed)
;
free		.macro count
			.switch \count
			.case 0
			.case 2
			ply				; 5
			.case 4
			ply				; 10
			ply
			.case 6
			ply				; 15
			ply
			ply
			.default
			sta #0,d		; 4

			tsc				; 2
			clc				; 2
			adc #\count		; 3
			tcs				; 2

			lda #0,d		; 4 (17 total)
			.endswitch
			.endm

tb			.namespace


int_register	.namespace
				.virtual #1,s
handler:		.dword ?
				.endv
				.endn

;
; Channel Parameters
;

chan_read		.namespace
				.virtual #1,s
size:			.word ?
buffer			.dword ?
				.endv
				.endn

chan_readline	.namespace
				.virtual #1,s
size			.word ?
buffer			.dword ?
				.endv
				.endn

chan_write_b	.namespace
				.virtual #1,s
b:				.byte ?
				.endv
				.endn

chan_write		.namespace 
				.virtual #1,s
buffer			.dword ?
size			.word ?
				.endv
				.endn

chan_seek		.namespace
				.virtual #1,s
base			.word ?
position		.dword ?
				.endv
				.endn

chan_ioctrl		.namespace
				.virtual #1,s
size			.word ?
buffer			.dword ?
command			.word ?
				.endv
				.endn

chan_open		.namespace
				.virtual #1,s
mode			.word ?
path			.dword ?
				.endv
				.endn

chan_swap		.namespace
				.virtual #1,s
channel2		.word ?
				.endv
				.endn
			
			.endn