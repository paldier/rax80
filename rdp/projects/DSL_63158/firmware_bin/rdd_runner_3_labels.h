#ifndef IMAGE_3_CODE_ADDRESSES
#define IMAGE_3_CODE_ADDRESSES

#define image_3_start_task_initialization_task		(0x18)
#define image_3_initialization_task		(0x18)
#define image_3_start_task_update_fifo_read_1st_wakeup_request		(0x18DC)
#define image_3_update_fifo_read_1st_wakeup_request		(0x18DC)
#define image_3_start_task_tx_task_1st_wakeup_request		(0x1C0)
#define image_3_tx_task_1st_wakeup_request		(0x1C0)
#define image_3_start_task_budget_allocator_1st_wakeup_request		(0x7FC)
#define image_3_budget_allocator_1st_wakeup_request		(0x7FC)
#define image_3_start_task_debug_routine		(0xD4)
#define image_3_debug_routine		(0xD4)
#define image_3_start_task_ovl_budget_allocator_1st_wakeup_request		(0xB14)
#define image_3_ovl_budget_allocator_1st_wakeup_request		(0xB14)
#define image_3_start_task_flush_task_1st_wakeup_request		(0x11B4)
#define image_3_flush_task_1st_wakeup_request		(0x11B4)
#define image_3_start_task_dhd_tx_complete_wakeup_request		(0x12B0)
#define image_3_dhd_tx_complete_wakeup_request		(0x12B0)
#define image_3_start_task_dhd_rx_complete_wakeup_request		(0x15A0)
#define image_3_dhd_rx_complete_wakeup_request		(0x15A0)
#define image_3_start_task_wan_tx_ddr_read_1st_wakeup_request		(0xDE8)
#define image_3_wan_tx_ddr_read_1st_wakeup_request		(0xDE8)
#define image_3_start_task_wan_tx_psram_write_1st_wakeup_request		(0xF98)
#define image_3_wan_tx_psram_write_1st_wakeup_request		(0xF98)
#define image_3_debug_routine_handler		(0xC)
#define image_3_scheduling_update_status		(0x450)
#define image_3_scheduling_action_not_valid		(0x59C)
#define image_3_basic_scheduler_update_dwrr		(0x6C0)
#define image_3_complex_scheduler_update_dwrr_basic_schedulers		(0x7C8)
#define image_3_complex_scheduler_update_dwrr_queues		(0x7D8)
#define image_3_basic_rate_limiter_complex_scheduler		(0xBE8)
#define image_3_basic_rate_limiter_basic_scheduler_no_cs		(0xC20)
#define image_3_basic_rate_limiter_queue_with_cs_bs		(0xC54)
#define image_3_basic_rate_limiter_queue_with_bs		(0xCA8)
#define image_3_ovl_rate_limiter		(0xCE4)
#define image_3_complex_rate_limiter_queue_sir		(0xCF8)
#define image_3_complex_rate_limiter_queue_pir		(0xD34)
#define image_3_complex_rate_limiter_basic_scheduler_sir		(0xD70)
#define image_3_complex_rate_limiter_basic_scheduler_pir		(0xDAC)

#endif