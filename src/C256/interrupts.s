;;;
;;; Assembly stubs for the interrupt handlers on the F256
;;;
;;; NOTE: this is necessary since the F256 has only 256 bytes of flash mapped
;;;       into bank 0, so we can have only the smallest amount of code there
;;;       that we can get away with.
;;;

				.extern int_handle_nmi
				.extern int_handle_irq
				.extern _Dp, _Vfp
              	.extern _DirectPageStart
				.extern _NearBaseAddress

				.section `$$interruptVector_0x00ffee`,text
				.word hw_handle_irq

				.section `$$interruptVector_0x00ffea`,text
				.word hw_handle_nmi
	
				.section code,root
hw_handle_irq:	rep #48
				pha
				phx
				phy
				pei dp:.tiny _Dp
				pei dp:.tiny (_Dp+2)
				pei dp:.tiny (_Dp+4)
				pei dp:.tiny (_Dp+6)
				pei dp:.tiny (_Dp+8)
				pei dp:.tiny (_Dp+10)
				pei dp:.tiny (_Dp+12)
				pei dp:.tiny (_Dp+14)
				pei dp:.tiny (_Dp+8)
				pei dp:.tiny (_Dp+10)

				; Save the caller's databank and direct page
				; TODO: this might change to being done only in the toolbox IRQ handler
				phd
				phb

				; Set the toolbox's direct page
				lda ##_DirectPageStart
				tcd

				; Set the toolbox's databank
				lda ##.word2 _NearBaseAddress
				stz dp:.tiny(_Vfp+2)
				xba									; A upper half = data bank
				pha
				plb                   				; pop 8 dummy
				plb                   				; set data bank

				; Call the interrupt handlers
				jsl int_handle_irq

				plb
				pld
				ply
				sty dp:.tiny (_Dp+10)
				ply
				sty dp:.tiny (_Dp+8)
				ply
				sty dp:.tiny (_Dp+14)
				ply
				sty dp:.tiny (_Dp+12)
				ply
				sty dp:.tiny (_Dp+10)
				ply
				sty dp:.tiny (_Dp+8)
				ply
				sty dp:.tiny (_Dp+6)
				ply
				sty dp:.tiny (_Dp+4)
				ply
				sty dp:.tiny (_Dp+2)
				ply
				sty dp:.tiny _Dp
				ply
				plx
				pla
				rti

hw_handle_nmi:	rep #48
				pha
				phx
				phy
				pei dp:.tiny _Dp
				pei dp:.tiny (_Dp+2)
				pei dp:.tiny (_Dp+4)
				pei dp:.tiny (_Dp+6)
				pei dp:.tiny (_Dp+8)
				pei dp:.tiny (_Dp+10)
				pei dp:.tiny (_Dp+12)
				pei dp:.tiny (_Dp+14)
				pei dp:.tiny (_Dp+8)
				pei dp:.tiny (_Dp+10)

				; Save the caller's databank and direct page
				; TODO: this might change to being done only in the toolbox IRQ handler
				phd
				phb

				; Set the toolbox's direct page
				lda ##_DirectPageStart
				tcd

				; Set the toolbox's databank
				lda ##.word2 _NearBaseAddress
				stz dp:.tiny(_Vfp+2)
				xba									; A upper half = data bank
				pha
				plb                   				; pop 8 dummy
				plb                   				; set data bank

				; Call the interrupt handlers
				jsl int_handle_nmi

				plb
				pld
				ply
				sty dp:.tiny (_Dp+10)
				ply
				sty dp:.tiny (_Dp+8)
				ply
				sty dp:.tiny (_Dp+14)
				ply
				sty dp:.tiny (_Dp+12)
				ply
				sty dp:.tiny (_Dp+10)
				ply
				sty dp:.tiny (_Dp+8)
				ply
				sty dp:.tiny (_Dp+6)
				ply
				sty dp:.tiny (_Dp+4)
				ply
				sty dp:.tiny (_Dp+2)
				ply
				sty dp:.tiny _Dp
				ply
				plx
				pla
				rti
