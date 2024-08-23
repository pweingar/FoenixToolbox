			.cpu "65816"

vky_text = $f04000
vky_mst_ctrl_0 = $f01000

* = $002000

header:		.text "PGX"
			.byte $01			; Version 0, CPU 65816
			.byte 0
			.byte `start
			.byte >start
			.byte <start

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