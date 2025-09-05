#import "macros.h"

	.public sys_proc_exit
	.public sys_int_enable_all
	.public sys_int_disable_all
	.public sys_int_restore_all
	.public sys_int_disable
	.public sys_int_enable
	.public sys_int_register
	.public sys_int_pending
	.public sys_int_clear
	.public sys_get_info
	.public sys_chan_read_b
	.public sys_chan_read
	.public sys_chan_readline
	.public sys_chan_write_b
	.public sys_chan_write
	.public sys_chan_status
	.public sys_chan_flush
	.public sys_chan_seek
	.public sys_chan_ioctrl
	.public sys_chan_open
	.public sys_chan_close
	.public sys_chan_swap
	.public sys_chan_device
	.public sys_cdev_register
	.public sys_bdev_register
	.public sys_bdev_read
	.public sys_bdev_write
	.public sys_bdev_status
	.public sys_bdev_flush
	.public sys_bdev_ioctrl
	.public sys_fsys_open
	.public sys_fsys_close
	.public sys_fsys_opendir
	.public sys_fsys_closedir
	.public sys_fsys_readdir
	.public sys_fsys_findfirst
	.public sys_fsys_findnext
	.public sys_fsys_get_label
	.public sys_fsys_set_label
	.public sys_fsys_mkdir
	.public sys_fsys_delete
	.public sys_fsys_rename
	.public sys_fsys_load
	.public sys_fsys_register_loader
	.public sys_fsys_stat
	.public sys_mem_get_ramtop
	.public sys_mem_reserve
	.public sys_time_jiffies
	.public sys_rtc_set_time
	.public sys_rtc_get_time
	.public sys_kbd_scancode
	.public sys_err_message
	.public sys_kbd_layout
	.public sys_proc_run
	.public sys_txt_get_capabilities
	.public sys_txt_set_mode
	.public sys_txt_set_resolution
	.public sys_txt_setsizes
	.public sys_txt_set_xy
	.public sys_txt_get_xy
	.public sys_txt_get_region
	.public sys_txt_set_region
	.public sys_txt_set_color
	.public sys_txt_get_color
	.public sys_txt_set_cursor
	.public sys_txt_set_cursor_visible
	.public sys_txt_set_font
	.public sys_txt_get_sizes
	.public sys_txt_set_border
	.public sys_txt_set_border_color
	.public sys_txt_put
	.public sys_txt_print
	.public sys_kbd_handle_irq
	.public sys_reboot
	.public sys_proc_set_shell
	.public sys_proc_get_result
	.public sys_fsys_set_cwd
	.public sys_fsys_get_cwd

	.extern proc_exit
	.extern int_enable_all
	.extern int_disable_all
	.extern int_restore_all
	.extern int_disable
	.extern int_enable
	.extern int_register
	.extern int_pending
	.extern int_clear
	.extern sys_get_information
	.extern chan_read_b
	.extern chan_read
	.extern chan_readline
	.extern chan_write_b
	.extern chan_write
	.extern chan_status
	.extern chan_flush
	.extern chan_seek
	.extern chan_ioctrl
	.extern chan_open
	.extern chan_close
	.extern chan_swap
	.extern chan_device
	.extern cdev_register
	.extern bdev_register
	.extern bdev_read
	.extern bdev_write
	.extern bdev_status
	.extern bdev_flush
	.extern bdev_ioctrl
	.extern fsys_open
	.extern fsys_close
	.extern fsys_opendir
	.extern fsys_closedir
	.extern fsys_readdir
	.extern fsys_findfirst
	.extern fsys_findnext
	.extern fsys_get_label
	.extern fsys_set_label
	.extern fsys_mkdir
	.extern fsys_delete
	.extern fsys_rename
	.extern fsys_load
	.extern fsys_register_loader
	.extern fsys_stat
	.extern mem_get_ramtop
	.extern mem_reserve
	.extern timers_jiffies
	.extern rtc_set_time
	.extern rtc_get_time
	.extern kbd_get_scancode
	.extern err_message
	.extern kbd_layout
	.extern proc_run
	.extern txt_get_capabilities
	.extern txt_set_mode
	.extern txt_set_resolution
	.extern txt_setsizes
	.extern txt_set_xy
	.extern txt_get_xy
	.extern txt_get_region
	.extern txt_set_region
	.extern txt_set_color
	.extern txt_get_color
	.extern txt_set_cursor
	.extern txt_set_cursor_visible
	.extern txt_set_font
	.extern txt_get_sizes
	.extern txt_set_border
	.extern txt_set_border_color
	.extern txt_put
	.extern txt_print
	.extern kbd_handle_irq
	.extern reboot
	.extern proc_set_shell
	.extern proc_get_result
	.extern fsys_set_cwd
	.extern fsys_get_cwd

	.section jumptable,root

sys_proc_exit:                	jump proc_exit
sys_int_enable_all:           	jump int_enable_all
sys_int_disable_all:          	jump int_disable_all
sys_int_restore_all:          	jump int_restore_all
sys_int_disable:              	jump int_disable
sys_int_enable:               	jump int_enable
sys_int_register:             	jump int_register
sys_int_pending:              	jump int_pending
sys_int_clear:                	jump int_clear
sys_get_info:                 	jump sys_get_information
sys_chan_read_b:              	jump chan_read_b
sys_chan_read:                	jump chan_read
sys_chan_readline:            	jump chan_readline
sys_chan_write_b:             	jump chan_write_b
sys_chan_write:               	jump chan_write
sys_chan_status:              	jump chan_status
sys_chan_flush:               	jump chan_flush
sys_chan_seek:                	jump chan_seek
sys_chan_ioctrl:              	jump chan_ioctrl
sys_chan_open:                	jump chan_open
sys_chan_close:               	jump chan_close
sys_chan_swap:                	jump chan_swap
sys_chan_device:              	jump chan_device
sys_cdev_register:            	jump cdev_register
sys_bdev_register:            	jump bdev_register
sys_bdev_read:                	jump bdev_read
sys_bdev_write:               	jump bdev_write
sys_bdev_status:              	jump bdev_status
sys_bdev_flush:               	jump bdev_flush
sys_bdev_ioctrl:              	jump bdev_ioctrl
sys_fsys_open:                	jump fsys_open
sys_fsys_close:               	jump fsys_close
sys_fsys_opendir:             	jump fsys_opendir
sys_fsys_closedir:            	jump fsys_closedir
sys_fsys_readdir:             	jump fsys_readdir
sys_fsys_findfirst:           	jump fsys_findfirst
sys_fsys_findnext:            	jump fsys_findnext
sys_fsys_get_label:           	jump fsys_get_label
sys_fsys_set_label:           	jump fsys_set_label
sys_fsys_mkdir:               	jump fsys_mkdir
sys_fsys_delete:              	jump fsys_delete
sys_fsys_rename:              	jump fsys_rename
sys_fsys_load:                	jump fsys_load
sys_fsys_register_loader:     	jump fsys_register_loader
sys_fsys_stat:                	jump fsys_stat
sys_mem_get_ramtop:           	jump mem_get_ramtop
sys_mem_reserve:              	jump mem_reserve
sys_time_jiffies:             	jump timers_jiffies
sys_rtc_set_time:             	jump rtc_set_time
sys_rtc_get_time:             	jump rtc_get_time
sys_kbd_scancode:             	jump kbd_get_scancode
sys_err_message:              	jump err_message
sys_kbd_layout:               	jump kbd_layout
sys_proc_run:                 	jump proc_run
sys_txt_get_capabilities:     	jump txt_get_capabilities
sys_txt_set_mode:             	jump txt_set_mode
sys_txt_set_resolution:       	jump txt_set_resolution
sys_txt_setsizes:             	jump txt_setsizes
sys_txt_set_xy:               	jump txt_set_xy
sys_txt_get_xy:               	jump txt_get_xy
sys_txt_get_region:           	jump txt_get_region
sys_txt_set_region:           	jump txt_set_region
sys_txt_set_color:            	jump txt_set_color
sys_txt_get_color:            	jump txt_get_color
sys_txt_set_cursor:           	jump txt_set_cursor
sys_txt_set_cursor_visible:   	jump txt_set_cursor_visible
sys_txt_set_font:             	jump txt_set_font
sys_txt_get_sizes:            	jump txt_get_sizes
sys_txt_set_border:           	jump txt_set_border
sys_txt_set_border_color:     	jump txt_set_border_color
sys_txt_put:                  	jump txt_put
sys_txt_print:                	jump txt_print
sys_kbd_handle_irq:           	jump kbd_handle_irq
sys_reboot:                   	jump reboot
sys_proc_set_shell:           	jump proc_set_shell
sys_proc_get_result:          	jump proc_get_result
sys_fsys_set_cwd:             	jump fsys_set_cwd
sys_fsys_get_cwd:             	jump fsys_get_cwd
