				.public restart_cli

				.extern proc_shell_address

#ifdef __CALYPSI_DATA_MODEL_SMALL__
              	.extern _NearBaseAddress
#endif

				.section sstack
				.section stack

				.section code

#include "macros.h"

;
; Reset the stack to the initial value.
; Reset the near pointer to the default value (if needed)
; Transfer control to the registered shell address (if present)
;
restart_cli:	
              	move.l  #.sectionEnd stack + 1,a0
              	move.l  a0,usp
#ifdef __CALYPSI_DATA_MODEL_SMALL__
              	lea.l   _NearBaseAddress.l,a4
#endif

				call proc_shell_address
				jump restart_cli
