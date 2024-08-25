			.cpu "65816"

vky_text = $f04000
vky_mst_ctrl_0 = $f01000

* = $f40000

header:		.byte $f8, $16				; Signature
			.byte 0						; Version
			.dword start				; Starting address
			.dword boot_icon_pixels		; Boot Icon pixels (0 = none)
			.dword boot_icon_clut		; CLUT address (0 = none)

			.null "sample_boot"

;
; The code for the sample
;

start:		sep #$20
			.as
			rep #$10
			.xl

			; Switch to text only mode

			lda #$01
			sta vky_mst_ctrl_0

			; Fill the text screen with blanks

			lda #' '
			ldx #0
clrloop1:	sta @l vky_text,x
			inx
			cpx #$2000
			bne clrloop1

			; Write the message to the screen

			ldx #0
putloop:	lda @l message,x
			beq done
			sta @l vky_text,x
			inx
			bra putloop

done:		nop
			bra done

message:	.null 'Hello, world!'

; Include the boot icon and boot icon's CLUT to use on the boot screen
 
.include "boot_clut.s"
.include "boot_icon.s"