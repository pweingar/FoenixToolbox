;;;
;;; Bindings for the Foenix Toolbox public calls
;;;

	.public sys_proc_exit
	.public sys_int_enable_all
	.public sys_int_disable_all
	.public sys_int_disable
	.public sys_int_enable
	.public sys_int_register
	.public sys_int_pending
	.public sys_get_info
	.public sys_int_clear
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
	.public sys_chan_register
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
	.public sys_fsys_set_cwd
	.public sys_fsys_get_cwd
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
	.public sys_txt_setsizes
	.public sys_txt_set_xy
	.public sys_txt_get_xy
	.public sys_txt_get_region
	.public sys_txt_set_region
	.public sys_txt_set_color
	.public sys_txt_get_color
	.public sys_txt_set_cursor_visible
	.public sys_txt_set_font
	.public sys_txt_get_sizes
	.public sys_txt_set_border
	.public sys_txt_set_border_color
	.public sys_txt_put
	.public sys_txt_print

sys_proc_exit:                 .equlab 0xFFE000
sys_int_enable_all:            .equlab 0xFFE004
sys_int_disable_all:           .equlab 0xFFE008
sys_int_disable:               .equlab 0xFFE00C
sys_int_enable:                .equlab 0xFFE010
sys_int_register:              .equlab 0xFFE014
sys_int_pending:               .equlab 0xFFE018
sys_get_info:                  .equlab 0xFFE01C
sys_int_clear:                 .equlab 0xFFE020
sys_chan_read_b:               .equlab 0xFFE024
sys_chan_read:                 .equlab 0xFFE028
sys_chan_readline:             .equlab 0xFFE02C
sys_chan_write_b:              .equlab 0xFFE030
sys_chan_write:                .equlab 0xFFE034
sys_chan_status:               .equlab 0xFFE038
sys_chan_flush:                .equlab 0xFFE03C
sys_chan_seek:                 .equlab 0xFFE040
sys_chan_ioctrl:               .equlab 0xFFE044
sys_chan_open:                 .equlab 0xFFE048
sys_chan_close:                .equlab 0xFFE04C
sys_chan_swap:                 .equlab 0xFFE050
sys_chan_device:               .equlab 0xFFE054
sys_chan_register:             .equlab 0xFFE058
sys_bdev_register:             .equlab 0xFFE05C
sys_bdev_read:                 .equlab 0xFFE060
sys_bdev_write:                .equlab 0xFFE064
sys_bdev_status:               .equlab 0xFFE068
sys_bdev_flush:                .equlab 0xFFE06C
sys_bdev_ioctrl:               .equlab 0xFFE070
sys_fsys_open:                 .equlab 0xFFE074
sys_fsys_close:                .equlab 0xFFE078
sys_fsys_opendir:              .equlab 0xFFE07C
sys_fsys_closedir:             .equlab 0xFFE080
sys_fsys_readdir:              .equlab 0xFFE084
sys_fsys_findfirst:            .equlab 0xFFE088
sys_fsys_findnext:             .equlab 0xFFE08C
sys_fsys_get_label:            .equlab 0xFFE090
sys_fsys_set_label:            .equlab 0xFFE094
sys_fsys_mkdir:                .equlab 0xFFE098
sys_fsys_delete:               .equlab 0xFFE09C
sys_fsys_rename:               .equlab 0xFFE0A0
sys_fsys_set_cwd:              .equlab 0xFFE0A4
sys_fsys_get_cwd:              .equlab 0xFFE0A8
sys_fsys_load:                 .equlab 0xFFE0AC
sys_fsys_register_loader:      .equlab 0xFFE0B0
sys_fsys_stat:                 .equlab 0xFFE0B4
sys_mem_get_ramtop:            .equlab 0xFFE0B8
sys_mem_reserve:               .equlab 0xFFE0BC
sys_time_jiffies:              .equlab 0xFFE0C0
sys_rtc_set_time:              .equlab 0xFFE0C4
sys_rtc_get_time:              .equlab 0xFFE0C8
sys_kbd_scancode:              .equlab 0xFFE0CC
sys_err_message:               .equlab 0xFFE0D0
sys_kbd_layout:                .equlab 0xFFE0D4
sys_proc_run:                  .equlab 0xFFE0D8
sys_txt_get_capabilities:      .equlab 0xFFE0DC
sys_txt_set_mode:              .equlab 0xFFE0E0
sys_txt_setsizes:              .equlab 0xFFE0E4
sys_txt_set_xy:                .equlab 0xFFE0E8
sys_txt_get_xy:                .equlab 0xFFE0EC
sys_txt_get_region:            .equlab 0xFFE0F0
sys_txt_set_region:            .equlab 0xFFE0F4
sys_txt_set_color:             .equlab 0xFFE0F8
sys_txt_get_color:             .equlab 0xFFE0FC
sys_txt_set_cursor_visible:    .equlab 0xFFE100
sys_txt_set_font:              .equlab 0xFFE104
sys_txt_get_sizes:             .equlab 0xFFE108
sys_txt_set_border:            .equlab 0xFFE10C
sys_txt_set_border_color:      .equlab 0xFFE110
sys_txt_put:                   .equlab 0xFFE114
sys_txt_print:                 .equlab 0xFFE118
