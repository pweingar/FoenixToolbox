;;;
;;; Test booting the F256k2e... 
;;;

			.cpu "65816"

vky_border_ctrl		= $f01004
vky_border_blue		= $f01005
vky_border_green	= $f01006
vky_border_red		= $f01007
vky_border_size_x	= $f01008
vky_border_size_y	= $f01009

led_pwr_blue		= $f016a7
led_pwr_green		= $f016a8
led_pwr_red			= $f016a9

* = $ff00

start:		clc
			xce

			sep #$30
			.as
			.xs

			lda #$01
			sta @l vky_border_ctrl

			lda #$ff
			sta @l vky_border_red
			sta @l led_pwr_red

			lda #0
			sta @l vky_border_blue
			sta @l led_pwr_blue
			sta @l vky_border_green
			sta @l led_pwr_green

			lda #16
			sta @l vky_border_size_x
			sta @l vky_border_size_y

loop:		nop
			bra loop

not_impl:	rti

* = $ffe4

hcop:		.word <>not_impl
hbrk:		.word <>not_impl
habort:		.word <>not_impl
hnmi:		.word <>not_impl
			.word 0
hirq:		.word <>not_impl

* = $fff4

ehcop:		.word <>not_impl
			.word 0
ehabort:	.word <>not_impl
ehnmi:		.word <>not_impl
hreset:		.word <>start
ehirq:		.word <>not_impl
