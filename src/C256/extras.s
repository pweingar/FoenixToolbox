				.public restart_cli
				.public io_copy_down
				.public io_copy_up

				.extern proc_shell_address
            	.extern _Vfp
				.extern _Dp
            	.extern _DirectPageStart

#ifndef __CALYPSI_DATA_MODEL_SMALL__
              	.extern _NearBaseAddress
#endif

              	.section stack
				.section farcode

;
; void io_copy_down(uint16_t count, uint16_t dest, uint16_t src)
;
io_copy_down:	pha
				phx
				phy
				phb

				ldy dp:.tiny (_Dp)
				ldx dp:.tiny (_Dp+4)
				mvn #0xf0, #0xf0

				plb
				ply
				plx
				pla
				rtl

;
; void io_copy_up(uint16_t count, uint16_t dest, uint16_t src)
;
io_copy_up:		pha
				phx
				phy
				phb

				ldy dp:.tiny (_Dp)
				ldx dp:.tiny (_Dp+4)
				mvp #0xf0, #0xf0

				plb
				ply
				plx
				pla
				rtl

;
; Reset the stack to the initial value.
; Reset the direct page and data bank registers
; Transfer control to the registered shell address (if present)
;
restart_cli:	
				rep     #0x38         ; 16-bit registers, no decimal mode
				ldx     ##.sectionEnd stack
				txs                   ; set stack
				lda     ##_DirectPageStart
				tcd                   ; set direct page
#ifdef __CALYPSI_DATA_MODEL_SMALL__
				lda     ##0
#else
				lda     ##.word2 _NearBaseAddress
#endif
				stz     dp:.tiny(_Vfp+2)
				xba                   ; A upper half = data bank
				pha
				plb                   ; pop 8 dummy
				plb                   ; set data bank

				jsl proc_shell_address
				bra restart_cli
