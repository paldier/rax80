/*
   Copyright (c) 2015 Broadcom
   All Rights Reserved

    <:label-BRCM:2015:DUAL/GPL:standard
    
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License, version 2, as published by
    the Free Software Foundation (the "GPL").
    
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    
    
    A copy of the GPL is available at http://www.broadcom.com/licenses/GPLv2.php, or by
    writing to the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
    Boston, MA 02111-1307, USA.
    
:>
*/



/* This is an automated file. Do not edit its contents. */


#include "bdmf_shell.h"
#include "rdd_map_auto.h"
#include "rdd_runner_reg_dump.h"
#include "rdd_runner_reg_dump_addrs.h"
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT US_TM_PD_FIFO_TABLE =
{
	16,
	{
		{ dump_RDD_PROCESSING_TX_DESCRIPTOR, 0x0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT RUNNER_GLOBAL_REGISTERS_INIT =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0x820 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT SERVICE_QUEUES_SCHEDULING_QUEUE_TABLE =
{
	8,
	{
		{ dump_RDD_SCHEDULING_QUEUE_DESCRIPTOR, 0x840 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT US_TM_TM_FLOW_CNTR_TABLE =
{
	1,
	{
		{ dump_RDD_TM_FLOW_CNTR_ENTRY, 0x880 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT BASIC_SCHEDULER_TABLE_US =
{
	16,
	{
		{ dump_RDD_BASIC_SCHEDULER_DESCRIPTOR, 0x900 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT SERVICE_QUEUES_RATE_LIMITER_TABLE =
{
	16,
	{
		{ dump_RDD_BASIC_RATE_LIMITER_DESCRIPTOR, 0xa00 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT US_TM_SCHEDULING_FLUSH_VECTOR =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0xb20 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT GHOST_REPORTING_GLOBAL_CFG =
{
	4,
	{
		{ dump_RDD_GHOST_REPORTING_GLOBAL_CFG_ENTRY, 0xb34 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT US_TM_BBH_TX_WAKE_UP_DATA_TABLE =
{
	8,
	{
		{ dump_RDD_BBH_TX_WAKE_UP_DATA_ENTRY, 0xb38 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT SERVICE_QUEUES_COMPLEX_SCHEDULER_TABLE =
{
	64,
	{
		{ dump_RDD_COMPLEX_SCHEDULER_DESCRIPTOR, 0xb40 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT REPORTING_QUEUE_ACCUMULATED_TABLE =
{
	8,
	{
		{ dump_RDD_REPORTING_ACCUMULATED_DATA, 0xb80 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT US_TM_SCHEDULING_QUEUE_TABLE =
{
	8,
	{
		{ dump_RDD_SCHEDULING_QUEUE_DESCRIPTOR, 0xc00 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT DS_TM_SCHEDULING_FLUSH_VECTOR =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0xea0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT ZERO_VALUE =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0xeb4 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT DS_TM_BBH_TX_WAKE_UP_DATA_TABLE =
{
	8,
	{
		{ dump_RDD_BBH_TX_WAKE_UP_DATA_ENTRY, 0xeb8 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT US_TM_BBH_TX_EGRESS_COUNTER_TABLE =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0xec0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT BBH_TX_EPON_WAKE_UP_DATA_TABLE =
{
	8,
	{
		{ dump_RDD_BBH_TX_WAKE_UP_DATA_ENTRY, 0xee8 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT XGPON_REPORT_TABLE =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0xef0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT BASIC_SCHEDULER_TABLE_DS =
{
	16,
	{
		{ dump_RDD_BASIC_SCHEDULER_DESCRIPTOR, 0xf00 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT DS_TM_PD_FIFO_TABLE =
{
	16,
	{
		{ dump_RDD_PROCESSING_TX_DESCRIPTOR, 0x1000 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT COMPLEX_SCHEDULER_TABLE =
{
	64,
	{
		{ dump_RDD_COMPLEX_SCHEDULER_DESCRIPTOR, 0x1500 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT SERVICE_QUEUES_PD_FIFO_TABLE =
{
	16,
	{
		{ dump_RDD_PROCESSING_TX_DESCRIPTOR, 0x1600 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT REPORTING_COUNTER_TABLE =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0x1700 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT US_TM_SCHEDULING_QUEUE_AGING_VECTOR =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0x17a0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT ONE_VALUE =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0x17b4 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT GENERAL_QUEUE_DYNAMIC_MNG_TABLE =
{
	8,
	{
		{ dump_RDD_QUEUE_DYNAMIC_MNG_ENTRY, 0x17b8 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT BBH_TX_EPON_INGRESS_COUNTER_TABLE =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x17c0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT EMAC_FLOW_CTRL_CTR_REP =
{
	8,
	{
		{ dump_RDD_BYTES_8, 0x17e8 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT OVERALL_RATE_LIMITER_TABLE =
{
	16,
	{
		{ dump_RDD_OVERALL_RATE_LIMITER_DESCRIPTOR, 0x17f0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT BASIC_RATE_LIMITER_TABLE_US =
{
	16,
	{
		{ dump_RDD_BASIC_RATE_LIMITER_DESCRIPTOR, 0x1800 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT DS_TM_SCHEDULING_QUEUE_TABLE =
{
	8,
	{
		{ dump_RDD_SCHEDULING_QUEUE_DESCRIPTOR, 0x1c00 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT DS_TM_SCHEDULING_QUEUE_AGING_VECTOR =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0x1ea0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT US_TM_BBH_TX_FIFO_BYTES_USED =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0x1eb4 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT MIRRORING_SCRATCH =
{
	8,
	{
		{ dump_RDD_BYTES_8, 0x1eb8 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT US_TM_TM_ACTION_PTR_TABLE =
{
	2,
	{
		{ dump_RDD_BYTES_2, 0x1ec0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT BUDGET_ALLOCATION_TIMER_VALUE =
{
	2,
	{
		{ dump_RDD_BYTES_2, 0x1ee2 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT US_TM_BB_DESTINATION_TABLE =
{
	2,
	{
		{ dump_RDD_BYTES_2, 0x1ee4 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT DS_TM_BB_DESTINATION_TABLE =
{
	2,
	{
		{ dump_RDD_BYTES_2, 0x1ee6 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT REPORT_BBH_TX_QUEUE_ID_TABLE =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0x1ee8 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT FLUSH_DISPATCHER_CREDIT_TABLE =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0x1ef0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT FLOW_CTRL_TIMER_VALUE =
{
	2,
	{
		{ dump_RDD_BYTES_2, 0x1efc },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT SERVICE_QUEUES_BUDGET_ALLOCATION_TIMER_VALUE =
{
	2,
	{
		{ dump_RDD_BYTES_2, 0x1efe },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT NATC_TBL_CFG =
{
	24,
	{
		{ dump_RDD_NATC_TBL_CONFIGURATION, 0x1f00 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT US_TM_CPU_TX_ABS_COUNTERS =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0x1f48 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT DS_TM_TM_FLOW_CNTR_TABLE =
{
	1,
	{
		{ dump_RDD_TM_FLOW_CNTR_ENTRY, 0x1fc8 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT US_TM_SCHEDULING_GLOBAL_FLUSH_CFG =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x1fcd },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT DS_TM_SCHEDULING_GLOBAL_FLUSH_CFG =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x1fce },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT SCHEDULING_FLUSH_GLOBAL_CFG =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x1fcf },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT DISPATCHER_CREDIT_TABLE =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0x1fd0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT TASK_IDX =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0x1fdc },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT US_TM_SCHEDULING_AGGREGATION_CONTEXT_VECTOR =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0x1fe0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT BBH_TX_US_FIFO_BYTES_THRESHOLD =
{
	2,
	{
		{ dump_RDD_BYTES_2, 0x1ff4 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT EMAC_FLOW_CTRL_VECTOR =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x1ff6 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT TM_vlan_stats_enable =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x1ff7 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT BBH_TX_EGRESS_REPORT_COUNTER_TABLE =
{
	8,
	{
		{ dump_RDD_BBH_TX_EGRESS_COUNTER_ENTRY, 0x1ff8 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT REPORTING_QUEUE_DESCRIPTOR_TABLE =
{
	16,
	{
		{ dump_RDD_REPORTING_QUEUE_DESCRIPTOR, 0x2000 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT DS_TM_CPU_TX_ABS_COUNTERS =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0x2410 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT REGISTERS_BUFFER =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0x2490 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT SERVICE_QUEUES_DISPATCHER_CREDIT_TABLE =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0x2510 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT CORE_ID_TABLE =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x251c },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT US_TM_FLUSH_AGGREGATION_TASK_DISABLE =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x251d },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT TX_MIRRORING_CONFIGURATION =
{
	2,
	{
		{ dump_RDD_MIRRORING_DESCRIPTOR, 0x251e },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT DS_TM_SCHEDULING_AGGREGATION_CONTEXT_VECTOR =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0x2520 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT DS_TM_FLUSH_AGGREGATION_TASK_DISABLE =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x2534 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT SRAM_DUMMY_STORE =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x2535 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT RATE_LIMIT_OVERHEAD =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x2536 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT BBH_TX_FIFO_SIZE =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x2537 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT US_TM_FIRST_QUEUE_MAPPING =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x2538 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT DS_TM_FIRST_QUEUE_MAPPING =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x2539 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT SERVICE_QUEUES_FIRST_QUEUE_MAPPING =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x253a },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT BBH_TX_INGRESS_COUNTER_TABLE =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x253b },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT DS_TM_TM_ACTION_PTR_TABLE =
{
	2,
	{
		{ dump_RDD_BYTES_2, 0x2540 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT TX_MIRRORING_DISPATCHER_CREDIT_TABLE =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0x2570 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT SERVICE_QUEUES_TM_ACTION_PTR_TABLE =
{
	2,
	{
		{ dump_RDD_BYTES_2, 0x2580 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT FPM_GLOBAL_CFG =
{
	12,
	{
		{ dump_RDD_FPM_GLOBAL_CFG, 0x25b0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT US_TM_UPDATE_FIFO_TABLE =
{
	4,
	{
		{ dump_RDD_UPDATE_FIFO_ENTRY, 0x25c0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT GHOST_REPORTING_QUEUE_STATUS_BIT_VECTOR_TABLE =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0x25e0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT US_TM_BBH_QUEUE_TABLE =
{
	4,
	{
		{ dump_RDD_BBH_QUEUE_DESCRIPTOR, 0x2600 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT SERVICE_QUEUES_RATE_LIMITER_VALID_TABLE =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0x26a0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT XGPON_REPORT_ZERO_SENT_TABLE =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x26b0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT DS_TM_UPDATE_FIFO_TABLE =
{
	4,
	{
		{ dump_RDD_UPDATE_FIFO_ENTRY, 0x26c0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT RATE_LIMITER_VALID_TABLE_US =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0x26e0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT EPON_UPDATE_FIFO_TABLE =
{
	4,
	{
		{ dump_RDD_UPDATE_FIFO_ENTRY, 0x2700 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT RATE_LIMITER_VALID_TABLE_DS =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0x2720 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT SERVICE_QUEUES_UPDATE_FIFO_TABLE =
{
	4,
	{
		{ dump_RDD_UPDATE_FIFO_ENTRY, 0x2740 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT EMAC_FLOW_CTRL_BUDGET =
{
	4,
	{
		{ dump_RDD_EMAC_FLOW_CTRL_BUDGET_ENTRY, 0x2780 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT BBH_TX_EPON_EGRESS_COUNTER_TABLE =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x27c0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT BASIC_RATE_LIMITER_TABLE_DS =
{
	16,
	{
		{ dump_RDD_BASIC_RATE_LIMITER_DESCRIPTOR, 0x2800 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT VLAN_TX_COUNTERS =
{
	8,
	{
		{ dump_RDD_PACKETS_AND_BYTES, 0x2c00 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT DS_TM_BBH_TX_EGRESS_COUNTER_TABLE =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x3040 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT DS_TM_BBH_QUEUE_TABLE =
{
	4,
	{
		{ dump_RDD_BBH_QUEUE_DESCRIPTOR, 0x3200 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT RUNNER_PROFILING_TRACE_BUFFER =
{
	4,
	{
		{ dump_RDD_TRACE_EVENT, 0x3400 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT SCT_FILTER =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0x3ffc },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT TX_FLOW_TABLE_1 =
{
	1,
	{
		{ dump_RDD_TX_FLOW_ENTRY, 0x0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT DIRECT_PROCESSING_EPON_CONTROL_SCRATCH_1 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x110 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT RUNNER_GLOBAL_REGISTERS_INIT_1 =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0x120 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT DIRECT_PROCESSING_PD_TABLE_1 =
{
	16,
	{
		{ dump_RDD_PROCESSING_RX_DESCRIPTOR, 0x140 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT VPORT_TO_FLOW_IDX_1 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x160 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT VPORT_CFG_TABLE_1 =
{
	8,
	{
		{ dump_RDD_VPORT_CFG_ENTRY, 0x180 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT LAYER2_GRE_TUNNEL_TABLE_1 =
{
	32,
	{
		{ dump_RDD_LAYER2_GRE_TUNNEL_ENTRY, 0x200 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT PBIT_TO_GEM_TABLE_1 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x380 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT CPU_RX_SCRATCHPAD_1 =
{
	8,
	{
		{ dump_RDD_BYTES_8, 0x400 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT DSCP_TO_PBITS_MAP_TABLE_1 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x600 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT CPU_RING_INTERRUPT_COUNTER_TABLE_1 =
{
	8,
	{
		{ dump_RDD_CPU_RING_INTERRUPT_COUNTER_ENTRY, 0x700 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT VPORT_TO_DSCP_TO_PBITS_TABLE_1 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x790 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT CPU_FEED_RING_RSV_TABLE_1 =
{
	2,
	{
		{ dump_RDD_BYTES_2, 0x7a0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT GLOBAL_DSCP_TO_PBITS_TABLE_1 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x7c0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT RX_FLOW_TABLE_1 =
{
	2,
	{
		{ dump_RDD_RX_FLOW_ENTRY, 0x800 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT DS_CPU_RX_METER_TABLE_1 =
{
	8,
	{
		{ dump_RDD_CPU_RX_METER_ENTRY, 0xa80 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT US_CPU_RX_METER_TABLE_1 =
{
	8,
	{
		{ dump_RDD_CPU_RX_METER_ENTRY, 0xb00 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT LAYER2_HEADER_COPY_MAPPING_TABLE_1 =
{
	4,
	{
		{ dump_RDD_LAYER2_HEADER_COPY_MAPPING_ENTRY, 0xb80 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT IPTV_CONFIGURATION_TABLE_1 =
{
	8,
	{
		{ dump_RDD_HW_IPTV_CONFIGURATION, 0xbf8 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT TCAM_IC_CMD_TABLE_1 =
{
	4,
	{
		{ dump_RDD_TCAM_IC_CMD, 0xc00 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT DUAL_STACK_LITE_TABLE_1 =
{
	64,
	{
		{ dump_RDD_DUAL_STACK_LITE_ENTRY, 0xe40 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT CSO_CONTEXT_TABLE_1 =
{
	108,
	{
		{ dump_RDD_CSO_CONTEXT_ENTRY, 0xe80 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT INGRESS_PACKET_BASED_MAPPING_1 =
{
	4,
	{
		{ dump_RDD_INGRESS_PACKET_BASED_MAPPING_ENTRY, 0xeec },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT FPM_POOL_NUMBER_MAPPING_TABLE_1 =
{
	1,
	{
		{ dump_RDD_FPM_POOL_NUMBER, 0xef0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT INGRESS_FILTER_PROFILE_TABLE_1 =
{
	8,
	{
		{ dump_RDD_INGRESS_FILTER_CTRL, 0xf00 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT POLICER_PARAMS_TABLE_1 =
{
	1,
	{
		{ dump_RDD_POLICER_PARAMS_ENTRY, 0xf80 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT LOOPBACK_QUEUE_TABLE_1 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0xfd0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT IPV4_HOST_ADDRESS_TABLE_1 =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0xfe0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT DS_PACKET_BUFFER_1 =
{
	512,
	{
		{ dump_RDD_PACKET_BUFFER, 0x1000 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT TC_TO_QUEUE_TABLE_1 =
{
	8,
	{
		{ dump_RDD_TC_TO_QUEUE_8, 0x2000 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT GENERAL_QUEUE_DYNAMIC_MNG_TABLE_1 =
{
	8,
	{
		{ dump_RDD_QUEUE_DYNAMIC_MNG_ENTRY, 0x2208 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT CPU_RECYCLE_INTERRUPT_COALESCING_TABLE_1 =
{
	16,
	{
		{ dump_RDD_CPU_INTERRUPT_COALESCING_ENTRY, 0x2210 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT TCAM_IC_HANDLER_TABLE_1 =
{
	2,
	{
		{ dump_RDD_TCAM_IC_HANDLER, 0x2220 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT DS_CPU_REASON_TO_METER_TABLE_1 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x2240 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT CPU_RX_COPY_PD_FIFO_TABLE_1 =
{
	16,
	{
		{ dump_RDD_PROCESSING_TX_DESCRIPTOR, 0x2280 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT US_CPU_REASON_TO_METER_TABLE_1 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x22c0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT EMAC_FLOW_CTRL_1 =
{
	16,
	{
		{ dump_RDD_EMAC_FLOW_CTRL_ENTRY, 0x2300 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT NATC_COUNTERS_GENERIC_CONTEXT_1 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x2350 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT CPU_RX_PSRAM_GET_NEXT_SCRATCHPAD_1 =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0x23d0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT SPDSVC_ANALYZER_PARAMS_TABLE_1 =
{
	28,
	{
		{ dump_RDD_SPDSVC_ANALYZER_PARAMS, 0x23e0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT ZERO_VALUE_1 =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0x23fc },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT PBIT_TO_QUEUE_TABLE_1 =
{
	8,
	{
		{ dump_RDD_PBIT_TO_QUEUE_8, 0x2400 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT PROCESSING_QUEUE_DYNAMIC_MNG_TABLE_1 =
{
	8,
	{
		{ dump_RDD_QUEUE_DYNAMIC_MNG_ENTRY, 0x2608 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT DHD_HW_CFG_1 =
{
	16,
	{
		{ dump_RDD_DHD_HW_CONFIGURATION, 0x2610 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT BRIDGE_CFG_TABLE_1 =
{
	8,
	{
		{ dump_RDD_BRIDGE_CFG, 0x2620 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT BRIDGE_LAN_MAC_1 =
{
	8,
	{
		{ dump_RDD_MAC_ADDRESS, 0x2638 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT IPV6_HOST_ADDRESS_CRC_TABLE_1 =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0x2640 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT REGISTERS_BUFFER_1 =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0x2680 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT NATC_TBL_CFG_1 =
{
	24,
	{
		{ dump_RDD_NATC_TBL_CONFIGURATION, 0x2700 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT RX_FLOW_CONTEXT_DDR_ADDR_1 =
{
	8,
	{
		{ dump_RDD_DEF_FLOW_CONTEXT_DDR_ADDR, 0x2748 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT INGRESS_FILTER_L2_REASON_TABLE_1 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x2750 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT TCAM_IC_CFG_TABLE_1 =
{
	16,
	{
		{ dump_RDD_TCAM_IC_CFG, 0x2760 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT SCRATCH_1 =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0x27e0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT RUNNER_PROFILING_TRACE_BUFFER_1 =
{
	4,
	{
		{ dump_RDD_TRACE_EVENT, 0x2800 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT CPU_RECYCLE_SRAM_PD_FIFO_1 =
{
	16,
	{
		{ dump_RDD_PROCESSING_TX_DESCRIPTOR, 0x2a00 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT TCPSPDTEST_ENGINE_REF_PKT_HDR_TABLE_1 =
{
	126,
	{
		{ dump_RDD_TCPSPDTEST_ENGINE_REF_PKT_HDR, 0x2b00 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT CPU_FEED_RING_INTERRUPT_COUNTER_MAX_1 =
{
	2,
	{
		{ dump_RDD_BYTES_2, 0x2b7e },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT TCPSPDTEST_ENGINE_TCB_TABLE_1 =
{
	56,
	{
		{ dump_RDD_TCPSPDTEST_ENGINE_TCB, 0x2b80 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT NULL_BUFFER_1 =
{
	8,
	{
		{ dump_RDD_BYTES_8, 0x2bb8 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT CPU_REASON_AND_VPORT_TO_METER_TABLE_1 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x2bc0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT DHD_RX_COMPLETE_0_DISPATCHER_CREDIT_TABLE_1 =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0x2bf0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT ONE_VALUE_1 =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0x2bfc },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT TCPSPDTEST_ENGINE_PKT_DROP_TABLE_1 =
{
	520,
	{
		{ dump_RDD_TCPSPDTEST_ENGINE_PKT_DROP, 0x2c00 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT TX_ABS_RECYCLE_COUNTERS_1 =
{
	8,
	{
		{ dump_RDD_TX_ABS_RECYCLE_COUNTERS_ENTRY, 0x2e08 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT DHD_RX_COMPLETE_1_DISPATCHER_CREDIT_TABLE_1 =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0x2e10 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT CPU_RX_INTERRUPT_SCRATCH_1 =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0x2e1c },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT DHD_RX_COMPLETE_2_DISPATCHER_CREDIT_TABLE_1 =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0x2e20 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT TASK_IDX_1 =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0x2e2c },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT DHD_TX_COMPLETE_0_DISPATCHER_CREDIT_TABLE_1 =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0x2e30 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT FORCE_DSCP_1 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x2e3c },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT CORE_ID_TABLE_1 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x2e3d },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT CPU_FEED_RING_INTERRUPT_THRESHOLD_1 =
{
	2,
	{
		{ dump_RDD_BYTES_2, 0x2e3e },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT SPDSVC_GEN_PARAMS_TABLE_1 =
{
	40,
	{
		{ dump_RDD_SPDSVC_GEN_PARAMS, 0x2e40 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT CPU_RECYCLE_INTERRUPT_ID_DDR_ADDR_1 =
{
	8,
	{
		{ dump_RDD_DDR_ADDRESS, 0x2e68 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT DHD_TX_COMPLETE_1_DISPATCHER_CREDIT_TABLE_1 =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0x2e70 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT CPU_FEED_RING_INTERRUPT_COUNTER_1 =
{
	2,
	{
		{ dump_RDD_BYTES_2, 0x2e7c },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT DHD_RX_POST_RING_SIZE_1 =
{
	2,
	{
		{ dump_RDD_DHD_RING_SIZE, 0x2e7e },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT CPU_VPORT_TO_METER_TABLE_1 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x2e80 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT CPU_RX_INTERRUPT_ID_DDR_ADDR_1 =
{
	8,
	{
		{ dump_RDD_DDR_ADDRESS, 0x2ea8 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT DHD_TX_COMPLETE_2_DISPATCHER_CREDIT_TABLE_1 =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0x2eb0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT DHD_RX_COMPLETE_RING_SIZE_1 =
{
	2,
	{
		{ dump_RDD_DHD_RING_SIZE, 0x2ebc },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT DHD_TX_COMPLETE_RING_SIZE_1 =
{
	2,
	{
		{ dump_RDD_DHD_RING_SIZE, 0x2ebe },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT FLOW_BASED_ACTION_PTR_TABLE_1 =
{
	2,
	{
		{ dump_RDD_BYTES_2, 0x2ec0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT RX_MIRRORING_CONFIGURATION_1 =
{
	2,
	{
		{ dump_RDD_MIRRORING_DESCRIPTOR, 0x2ee4 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT SRAM_DUMMY_STORE_1 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x2ee6 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT RATE_LIMIT_OVERHEAD_1 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x2ee7 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT TCPSPDTEST_ENGINE_CONN_INFO_TABLE_1 =
{
	8,
	{
		{ dump_RDD_TCPSPDTEST_ENGINE_CONN_INFO, 0x2ee8 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT CPU_RX_COPY_DISPATCHER_CREDIT_TABLE_1 =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0x2ef0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT INGRESS_QOS_DONT_DROP_RATIO_1 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x2efc },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT INGRESS_QOS_WAN_UNTAGGED_PRIORITY_1 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x2efd },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT MAC_TYPE_1 =
{
	1,
	{
		{ dump_RDD_MAC_TYPE_ENTRY, 0x2efe },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT IC_MCAST_ENABLE_1 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x2eff },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT RULE_BASED_ACTION_PTR_TABLE_1 =
{
	2,
	{
		{ dump_RDD_BYTES_2, 0x2f00 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT ECN_IPV6_REMARK_TABLE_1 =
{
	1,
	{
		{ dump_RDD_ECN_IPV6_REMARK_ENTRY, 0x2f22 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT CPU_REDIRECT_MODE_1 =
{
	1,
	{
		{ dump_RDD_CPU_REDIRECT_MODE_ENTRY, 0x2f23 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT IPTV_CLASSIFICATION_CFG_TABLE_1 =
{
	1,
	{
		{ dump_RDD_IPTV_CLASSIFICATION_CFG_ENTRY, 0x2f24 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT INGRESS_FILTER_1588_CFG_1 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x2f25 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT RX_MIRRORING_DIRECT_ENABLE_1 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x2f26 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT DHD_CPU_RECYCLE_INTERRUPT_ID_DDR_ADDR_1 =
{
	8,
	{
		{ dump_RDD_DDR_ADDRESS, 0x2f28 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT WAN_LOOPBACK_DISPATCHER_CREDIT_TABLE_1 =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0x2f30 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT IPTV_ACTION_PTR_TABLE_1 =
{
	2,
	{
		{ dump_RDD_BYTES_2, 0x2f40 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT DHD_FPM_THRESHOLDS_1 =
{
	2,
	{
		{ dump_RDD_BYTES_2, 0x2f68 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT SPDSVC_GEN_DISPATCHER_CREDIT_TABLE_1 =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0x2f70 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT VLAN_ACTION_GPE_HANDLER_PTR_TABLE_1 =
{
	2,
	{
		{ dump_RDD_BYTES_2, 0x2f80 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT SYSTEM_CONFIGURATION_1 =
{
	5,
	{
		{ dump_RDD_SYSTEM_CONFIGURATION_ENTRY, 0x2fa8 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT CPU_RECYCLE_RING_DESCRIPTOR_TABLE_1 =
{
	16,
	{
		{ dump_RDD_CPU_RING_DESCRIPTOR, 0x2fb0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT UPDATE_FIFO_TABLE_1 =
{
	4,
	{
		{ dump_RDD_UPDATE_FIFO_ENTRY, 0x2fc0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT CPU_RECYCLE_RING_INDEX_DDR_ADDR_TABLE_1 =
{
	8,
	{
		{ dump_RDD_DDR_ADDRESS, 0x2fe0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT CPU_INTERRUPT_COALESCING_TABLE_1 =
{
	16,
	{
		{ dump_RDD_CPU_INTERRUPT_COALESCING_ENTRY, 0x2ff0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT CPU_RING_DESCRIPTORS_TABLE_1 =
{
	16,
	{
		{ dump_RDD_CPU_RING_DESCRIPTOR, 0x3000 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT CPU_RX_COPY_UPDATE_FIFO_TABLE_1 =
{
	4,
	{
		{ dump_RDD_UPDATE_FIFO_ENTRY, 0x3100 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT CPU_FEED_RING_DESCRIPTOR_TABLE_1 =
{
	16,
	{
		{ dump_RDD_CPU_RING_DESCRIPTOR, 0x3120 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT CPU_RX_LOCAL_SCRATCH_1 =
{
	8,
	{
		{ dump_RDD_BYTES_8, 0x3130 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT TX_ABS_RECYCLE_UPDATE_FIFO_TABLE_1 =
{
	4,
	{
		{ dump_RDD_UPDATE_FIFO_ENTRY, 0x3140 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT FPM_GLOBAL_CFG_1 =
{
	12,
	{
		{ dump_RDD_FPM_GLOBAL_CFG, 0x3160 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT CPU_RECYCLE_RING_INTERRUPT_COUNTER_TABLE_1 =
{
	8,
	{
		{ dump_RDD_CPU_RING_INTERRUPT_COUNTER_ENTRY, 0x3170 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT CPU_RX_COPY_LOCAL_SCRATCH_1 =
{
	8,
	{
		{ dump_RDD_BYTES_8, 0x3178 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT PD_FIFO_TABLE_1 =
{
	16,
	{
		{ dump_RDD_PROCESSING_TX_DESCRIPTOR, 0x3180 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT TUNNELS_PARSING_CFG_1 =
{
	8,
	{
		{ dump_RDD_TUNNELS_PARSING_CFG, 0x31a0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT IPTV_CFG_TABLE_1 =
{
	8,
	{
		{ dump_RDD_IPTV_CFG, 0x31a8 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT DHD_DOORBELL_TX_COMPLETE_VALUE_1 =
{
	4,
	{
		{ dump_RDD_DHD_DOORBELL, 0x31b0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT IPTV_DDR_CTX_TABLE_ADDRESS_1 =
{
	8,
	{
		{ dump_RDD_IPTV_DDR_CTX_TABLE_ADDRESS, 0x31b8 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT TX_ABS_RECYCLE_PD_FIFO_TABLE_1 =
{
	16,
	{
		{ dump_RDD_PROCESSING_TX_DESCRIPTOR, 0x31c0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT DHD_DOORBELL_RX_COMPLETE_VALUE_1 =
{
	4,
	{
		{ dump_RDD_DHD_DOORBELL, 0x31e0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT NAT_CACHE_CFG_1 =
{
	7,
	{
		{ dump_RDD_NAT_CACHE_CFG, 0x31e8 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT DHD_DOORBELL_RX_POST_VALUE_1 =
{
	4,
	{
		{ dump_RDD_DHD_DOORBELL, 0x31f0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT NAT_CACHE_KEY0_MASK_1 =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0x31f8 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT DHD_COMPLETE_COMMON_RADIO_DATA_1 =
{
	112,
	{
		{ dump_RDD_DHD_COMPLETE_COMMON_RADIO_ENTRY, 0x3200 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT NATC_L2_VLAN_KEY_MASK_1 =
{
	2,
	{
		{ dump_RDD_BYTES_2, 0x3270 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT INGRESS_FILTER_CFG_1 =
{
	2,
	{
		{ dump_RDD_INGRESS_FILTER_CFG, 0x3278 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT VPORT_CFG_EX_TABLE_1 =
{
	2,
	{
		{ dump_RDD_VPORT_CFG_EX_ENTRY, 0x3280 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT CPU_REASON_TO_TC_1 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x32a0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT TC_TO_CPU_RXQ_1 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x32e0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT EXC_TC_TO_CPU_RXQ_1 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x3320 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT NATC_L2_TOS_MASK_1 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x3360 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT DHD_RX_COMPLETE_FLOW_RING_BUFFER_1 =
{
	32,
	{
		{ dump_RDD_DHD_RX_COMPLETE_DESCRIPTOR, 0x3380 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT VPORT_TO_CPU_OBJ_1 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x33c0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT DHD_RX_POST_FLOW_RING_BUFFER_1 =
{
	32,
	{
		{ dump_RDD_DHD_RX_POST_DESCRIPTOR, 0x3400 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT DHD_TX_COMPLETE_FLOW_RING_BUFFER_1 =
{
	16,
	{
		{ dump_RDD_DHD_TX_COMPLETE_DESCRIPTOR, 0x3440 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT DHD_CPU_RECYCLE_RING_DESCRIPTOR_TABLE_1 =
{
	16,
	{
		{ dump_RDD_CPU_RING_DESCRIPTOR, 0x3480 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT DHD_FPM_REPLY_1 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x34c0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT CPU_RECYCLE_INTERRUPT_SCRATCH_1 =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0x3500 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT CPU_RECYCLE_SHADOW_RD_IDX_1 =
{
	2,
	{
		{ dump_RDD_BYTES_2, 0x3580 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT CPU_RECYCLE_SHADOW_WR_IDX_1 =
{
	2,
	{
		{ dump_RDD_BYTES_2, 0x3600 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT SCT_FILTER_1 =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0x3ffc },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT CPU_TX_SCRATCHPAD_2 =
{
	8,
	{
		{ dump_RDD_BYTES_8, 0x0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT RX_FLOW_TABLE_2 =
{
	2,
	{
		{ dump_RDD_RX_FLOW_ENTRY, 0x400 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT VPORT_CFG_TABLE_2 =
{
	8,
	{
		{ dump_RDD_VPORT_CFG_ENTRY, 0x680 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT DSCP_TO_PBITS_MAP_TABLE_2 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x700 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT TCAM_IC_CMD_TABLE_2 =
{
	4,
	{
		{ dump_RDD_TCAM_IC_CMD, 0x800 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT GLOBAL_DSCP_TO_PBITS_TABLE_2 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0xa40 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT PBIT_TO_GEM_TABLE_2 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0xa80 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT DHD_BACKUP_INFO_CACHE_TABLE_2 =
{
	8,
	{
		{ dump_RDD_DHD_BACKUP_INFO_CACHE_ENTRY, 0xb00 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT LAYER2_HEADER_COPY_MAPPING_TABLE_2 =
{
	4,
	{
		{ dump_RDD_LAYER2_HEADER_COPY_MAPPING_ENTRY, 0xb80 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT IPTV_CONFIGURATION_TABLE_2 =
{
	8,
	{
		{ dump_RDD_HW_IPTV_CONFIGURATION, 0xbf8 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT TC_TO_QUEUE_TABLE_2 =
{
	8,
	{
		{ dump_RDD_TC_TO_QUEUE_8, 0xc00 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT GENERAL_QUEUE_DYNAMIC_MNG_TABLE_2 =
{
	8,
	{
		{ dump_RDD_QUEUE_DYNAMIC_MNG_ENTRY, 0xe08 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT VPORT_TO_DSCP_TO_PBITS_TABLE_2 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0xe10 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT RUNNER_GLOBAL_REGISTERS_INIT_2 =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0xe20 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT DUAL_STACK_LITE_TABLE_2 =
{
	64,
	{
		{ dump_RDD_DUAL_STACK_LITE_ENTRY, 0xe40 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT POLICER_PARAMS_TABLE_2 =
{
	1,
	{
		{ dump_RDD_POLICER_PARAMS_ENTRY, 0xe80 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT FPM_POOL_NUMBER_MAPPING_TABLE_2 =
{
	1,
	{
		{ dump_RDD_FPM_POOL_NUMBER, 0xed0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT TCAM_IC_HANDLER_TABLE_2 =
{
	2,
	{
		{ dump_RDD_TCAM_IC_HANDLER, 0xee0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT INGRESS_FILTER_PROFILE_TABLE_2 =
{
	8,
	{
		{ dump_RDD_INGRESS_FILTER_CTRL, 0xf00 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT NATC_COUNTERS_GENERIC_CONTEXT_2 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0xf80 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT DS_PACKET_BUFFER_2 =
{
	512,
	{
		{ dump_RDD_PACKET_BUFFER, 0x1000 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT PBIT_TO_QUEUE_TABLE_2 =
{
	8,
	{
		{ dump_RDD_PBIT_TO_QUEUE_8, 0x2000 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT REGISTERS_BUFFER_2 =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0x2208 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT PROCESSING_QUEUE_DYNAMIC_MNG_TABLE_2 =
{
	8,
	{
		{ dump_RDD_QUEUE_DYNAMIC_MNG_ENTRY, 0x2288 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT CPU_TX_FPM_POOL_NUMBER_MAPPING_TABLE_2 =
{
	1,
	{
		{ dump_RDD_FPM_POOL_NUMBER, 0x2290 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT SPDSVC_ANALYZER_PARAMS_TABLE_2 =
{
	28,
	{
		{ dump_RDD_SPDSVC_ANALYZER_PARAMS, 0x22a0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT INGRESS_PACKET_BASED_MAPPING_2 =
{
	4,
	{
		{ dump_RDD_INGRESS_PACKET_BASED_MAPPING_ENTRY, 0x22bc },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT WLAN_MCAST_DHD_STATION_CTX_TABLE_2 =
{
	1,
	{
		{ dump_RDD_WLAN_MCAST_DHD_STATION_CTX_ENTRY, 0x22c0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT DHD_POST_COMMON_RADIO_DATA_2 =
{
	104,
	{
		{ dump_RDD_DHD_POST_COMMON_RADIO_ENTRY, 0x2300 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT BRIDGE_LAN_MAC_2 =
{
	8,
	{
		{ dump_RDD_MAC_ADDRESS, 0x2368 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT DHD_FPM_POOL_NUMBER_MAPPING_TABLE_2 =
{
	1,
	{
		{ dump_RDD_FPM_POOL_NUMBER, 0x2370 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT WLAN_MCAST_DFT_LIST_SIZE_2 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x2380 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT WLAN_MCAST_DFT_LIST_ENTRY_SCRATCH_2 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x23c0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT RUNNER_PROFILING_TRACE_BUFFER_2 =
{
	4,
	{
		{ dump_RDD_TRACE_EVENT, 0x2400 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT WLAN_MCAST_COPY_SCRATCHPAD_2 =
{
	8,
	{
		{ dump_RDD_BYTES_8, 0x2600 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT WLAN_MCAST_DHD_STATION_TABLE_2 =
{
	8,
	{
		{ dump_RDD_WLAN_MCAST_DHD_STATION_ENTRY, 0x2800 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT LAYER2_GRE_TUNNEL_TABLE_2 =
{
	32,
	{
		{ dump_RDD_LAYER2_GRE_TUNNEL_ENTRY, 0x2a00 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT TCAM_IC_CFG_TABLE_2 =
{
	16,
	{
		{ dump_RDD_TCAM_IC_CFG, 0x2b80 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT TX_FLOW_TABLE_2 =
{
	1,
	{
		{ dump_RDD_TX_FLOW_ENTRY, 0x2c00 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT FPM_REPLY_2 =
{
	8,
	{
		{ dump_RDD_BYTES_8, 0x2d10 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT BRIDGE_CFG_TABLE_2 =
{
	8,
	{
		{ dump_RDD_BRIDGE_CFG, 0x2d20 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT RX_FLOW_CONTEXT_DDR_ADDR_2 =
{
	8,
	{
		{ dump_RDD_DEF_FLOW_CONTEXT_DDR_ADDR, 0x2d38 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT WLAN_MCAST_SCRATCHPAD_2 =
{
	8,
	{
		{ dump_RDD_BYTES_8, 0x2d40 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT NULL_BUFFER_2 =
{
	8,
	{
		{ dump_RDD_BYTES_8, 0x2d68 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT LOOPBACK_QUEUE_TABLE_2 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x2d70 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT FLOW_BASED_ACTION_PTR_TABLE_2 =
{
	2,
	{
		{ dump_RDD_BYTES_2, 0x2d80 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT ZERO_VALUE_2 =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0x2da4 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT CPU_RECYCLE_INTERRUPT_ID_DDR_ADDR_2 =
{
	8,
	{
		{ dump_RDD_DDR_ADDRESS, 0x2da8 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT CPU_RECYCLE_INTERRUPT_COALESCING_TABLE_2 =
{
	16,
	{
		{ dump_RDD_CPU_INTERRUPT_COALESCING_ENTRY, 0x2db0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT QUEUE_THRESHOLD_VECTOR_2 =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0x2dc0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT ONE_VALUE_2 =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0x2de4 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT CPU_RX_INTERRUPT_ID_DDR_ADDR_2 =
{
	8,
	{
		{ dump_RDD_DDR_ADDRESS, 0x2de8 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT DHD_HW_CFG_2 =
{
	16,
	{
		{ dump_RDD_DHD_HW_CONFIGURATION, 0x2df0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT CPU_RECYCLE_SRAM_PD_FIFO_2 =
{
	16,
	{
		{ dump_RDD_PROCESSING_TX_DESCRIPTOR, 0x2e00 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT EMAC_FLOW_CTRL_2 =
{
	16,
	{
		{ dump_RDD_EMAC_FLOW_CTRL_ENTRY, 0x2f00 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT INGRESS_FILTER_L2_REASON_TABLE_2 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x2f50 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT CPU_TX_EGRESS_DISPATCHER_CREDIT_TABLE_2 =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0x2f60 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT TASK_IDX_2 =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0x2f6c },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT CPU_TX_INGRESS_DISPATCHER_CREDIT_TABLE_2 =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0x2f70 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT FORCE_DSCP_2 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x2f7c },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT CPU_TX_vlan_stats_enable_2 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x2f7d },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT CORE_ID_TABLE_2 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x2f7e },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT SRAM_DUMMY_STORE_2 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x2f7f },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT RULE_BASED_ACTION_PTR_TABLE_2 =
{
	2,
	{
		{ dump_RDD_BYTES_2, 0x2f80 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT RX_MIRRORING_CONFIGURATION_2 =
{
	2,
	{
		{ dump_RDD_MIRRORING_DESCRIPTOR, 0x2fa2 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT RATE_LIMIT_OVERHEAD_2 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x2fa4 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT INGRESS_QOS_DONT_DROP_RATIO_2 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x2fa5 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT INGRESS_QOS_WAN_UNTAGGED_PRIORITY_2 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x2fa6 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT IC_MCAST_ENABLE_2 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x2fa7 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT DHD_CPU_RECYCLE_INTERRUPT_ID_DDR_ADDR_2 =
{
	8,
	{
		{ dump_RDD_DDR_ADDRESS, 0x2fa8 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT DHD_MCAST_DISPATCHER_CREDIT_TABLE_2 =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0x2fb0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT ECN_IPV6_REMARK_TABLE_2 =
{
	1,
	{
		{ dump_RDD_ECN_IPV6_REMARK_ENTRY, 0x2fbc },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT CPU_REDIRECT_MODE_2 =
{
	1,
	{
		{ dump_RDD_CPU_REDIRECT_MODE_ENTRY, 0x2fbd },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT IPTV_CLASSIFICATION_CFG_TABLE_2 =
{
	1,
	{
		{ dump_RDD_IPTV_CLASSIFICATION_CFG_ENTRY, 0x2fbe },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT INGRESS_FILTER_1588_CFG_2 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x2fbf },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT IPTV_ACTION_PTR_TABLE_2 =
{
	2,
	{
		{ dump_RDD_BYTES_2, 0x2fc0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT RX_MIRRORING_DIRECT_ENABLE_2 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x2fe2 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT DHD_BACKUP_BASE_ADDR_2 =
{
	8,
	{
		{ dump_RDD_BYTES_8, 0x2fe8 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT CPU_RECYCLE_RING_DESCRIPTOR_TABLE_2 =
{
	16,
	{
		{ dump_RDD_CPU_RING_DESCRIPTOR, 0x2ff0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT DHD_FLOW_RING_CACHE_CTX_TABLE_2 =
{
	16,
	{
		{ dump_RDD_DHD_FLOW_RING_CACHE_CTX_ENTRY, 0x3000 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT NATC_TBL_CFG_2 =
{
	24,
	{
		{ dump_RDD_NATC_TBL_CONFIGURATION, 0x3100 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT CPU_RECYCLE_RING_INDEX_DDR_ADDR_TABLE_2 =
{
	8,
	{
		{ dump_RDD_DDR_ADDRESS, 0x3148 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT WLAN_MCAST_DFT_ADDR_2 =
{
	8,
	{
		{ dump_RDD_PHYS_ADDR_64_PTR, 0x3158 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT CPU_FEED_RING_DESCRIPTOR_TABLE_2 =
{
	16,
	{
		{ dump_RDD_CPU_RING_DESCRIPTOR, 0x3160 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT FPM_GLOBAL_CFG_2 =
{
	12,
	{
		{ dump_RDD_FPM_GLOBAL_CFG, 0x3170 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT VLAN_ACTION_GPE_HANDLER_PTR_TABLE_2 =
{
	2,
	{
		{ dump_RDD_BYTES_2, 0x3180 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT DHD_FPM_THRESHOLDS_2 =
{
	2,
	{
		{ dump_RDD_BYTES_2, 0x31a8 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT SYSTEM_CONFIGURATION_2 =
{
	5,
	{
		{ dump_RDD_SYSTEM_CONFIGURATION_ENTRY, 0x31b0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT CPU_RECYCLE_RING_INTERRUPT_COUNTER_TABLE_2 =
{
	8,
	{
		{ dump_RDD_CPU_RING_INTERRUPT_COUNTER_ENTRY, 0x31b8 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT CPU_TX_EGRESS_UPDATE_FIFO_TABLE_2 =
{
	4,
	{
		{ dump_RDD_UPDATE_FIFO_ENTRY, 0x31c0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT TUNNELS_PARSING_CFG_2 =
{
	8,
	{
		{ dump_RDD_TUNNELS_PARSING_CFG, 0x31e0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT IPTV_CFG_TABLE_2 =
{
	8,
	{
		{ dump_RDD_IPTV_CFG, 0x31e8 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT IPTV_DDR_CTX_TABLE_ADDRESS_2 =
{
	8,
	{
		{ dump_RDD_IPTV_DDR_CTX_TABLE_ADDRESS, 0x31f0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT NAT_CACHE_CFG_2 =
{
	7,
	{
		{ dump_RDD_NAT_CACHE_CFG, 0x31f8 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT DHD_TX_POST_PD_FIFO_TABLE_2 =
{
	16,
	{
		{ dump_RDD_PROCESSING_TX_DESCRIPTOR, 0x3200 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT CPU_TX_INGRESS_UPDATE_FIFO_TABLE_2 =
{
	4,
	{
		{ dump_RDD_UPDATE_FIFO_ENTRY, 0x32c0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT NAT_CACHE_KEY0_MASK_2 =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0x32e0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT NATC_L2_VLAN_KEY_MASK_2 =
{
	2,
	{
		{ dump_RDD_BYTES_2, 0x32e8 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT INGRESS_FILTER_CFG_2 =
{
	2,
	{
		{ dump_RDD_INGRESS_FILTER_CFG, 0x32f0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT NATC_L2_TOS_MASK_2 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x32f8 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT DHD_TX_POST_FLOW_RING_BUFFER_2 =
{
	48,
	{
		{ dump_RDD_DHD_TX_POST_DESCRIPTOR, 0x3300 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT DHD_TX_POST_UPDATE_FIFO_TABLE_2 =
{
	4,
	{
		{ dump_RDD_UPDATE_FIFO_ENTRY, 0x3340 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT DHD_MCAST_UPDATE_FIFO_TABLE_2 =
{
	4,
	{
		{ dump_RDD_UPDATE_FIFO_ENTRY, 0x3380 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT CPU_TX_EGRESS_PD_FIFO_TABLE_2 =
{
	16,
	{
		{ dump_RDD_PROCESSING_TX_DESCRIPTOR, 0x33c0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT WLAN_MCAST_SSID_MAC_ADDRESS_TABLE_2 =
{
	8,
	{
		{ dump_RDD_WLAN_MCAST_SSID_MAC_ADDRESS_ENTRY, 0x3400 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT CPU_TX_INGRESS_PD_FIFO_TABLE_2 =
{
	16,
	{
		{ dump_RDD_PROCESSING_TX_DESCRIPTOR, 0x3480 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT DHD_MCAST_PD_FIFO_TABLE_2 =
{
	16,
	{
		{ dump_RDD_PROCESSING_TX_DESCRIPTOR, 0x34c0 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT VPORT_CFG_EX_TABLE_2 =
{
	2,
	{
		{ dump_RDD_VPORT_CFG_EX_ENTRY, 0x3500 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT DHD_DOORBELL_TX_POST_VALUE_2 =
{
	4,
	{
		{ dump_RDD_DHD_DOORBELL, 0x3520 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT DHD_BACKUP_INDEX_CACHE_2 =
{
	32,
	{
		{ dump_RDD_DHD_BACKUP_IDX_CACHE_TABLE, 0x3580 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT WLAN_MCAST_SSID_STATS_TABLE_2 =
{
	8,
	{
		{ dump_RDD_WLAN_MCAST_SSID_STATS_ENTRY, 0x3600 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT DHD_L2_HEADER_2 =
{
	1,
	{
		{ dump_RDD_BYTE_1, 0x3680 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT DHD_FLOW_RING_CACHE_LKP_TABLE_2 =
{
	2,
	{
		{ dump_RDD_DHD_FLOW_RING_CACHE_LKP_ENTRY, 0x3700 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT DHD_CPU_RECYCLE_RING_DESCRIPTOR_TABLE_2 =
{
	16,
	{
		{ dump_RDD_CPU_RING_DESCRIPTOR, 0x3780 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT CPU_RECYCLE_INTERRUPT_SCRATCH_2 =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0x3800 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT CPU_RECYCLE_SHADOW_RD_IDX_2 =
{
	2,
	{
		{ dump_RDD_BYTES_2, 0x3880 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT CPU_RECYCLE_SHADOW_WR_IDX_2 =
{
	2,
	{
		{ dump_RDD_BYTES_2, 0x3900 },
		{ 0, 0 },
	}
};
#endif
#if defined BCM6846
static DUMP_RUNNERREG_STRUCT SCT_FILTER_2 =
{
	4,
	{
		{ dump_RDD_BYTES_4, 0x3ffc },
		{ 0, 0 },
	}
};
#endif

TABLE_STRUCT RUNNER_TABLES[NUMBER_OF_TABLES] =
{
#if defined BCM6846
	{ "US_TM_PD_FIFO_TABLE", 1, CORE_0_INDEX, &US_TM_PD_FIFO_TABLE, 130, 1, 1 },
#endif
#if defined BCM6846
	{ "RUNNER_GLOBAL_REGISTERS_INIT", 1, CORE_0_INDEX, &RUNNER_GLOBAL_REGISTERS_INIT, 8, 1, 1 },
#endif
#if defined BCM6846
	{ "SERVICE_QUEUES_SCHEDULING_QUEUE_TABLE", 1, CORE_0_INDEX, &SERVICE_QUEUES_SCHEDULING_QUEUE_TABLE, 8, 1, 1 },
#endif
#if defined BCM6846
	{ "US_TM_TM_FLOW_CNTR_TABLE", 1, CORE_0_INDEX, &US_TM_TM_FLOW_CNTR_TABLE, 128, 1, 1 },
#endif
#if defined BCM6846
	{ "BASIC_SCHEDULER_TABLE_US", 1, CORE_0_INDEX, &BASIC_SCHEDULER_TABLE_US, 16, 1, 1 },
#endif
#if defined BCM6846
	{ "SERVICE_QUEUES_RATE_LIMITER_TABLE", 1, CORE_0_INDEX, &SERVICE_QUEUES_RATE_LIMITER_TABLE, 18, 1, 1 },
#endif
#if defined BCM6846
	{ "US_TM_SCHEDULING_FLUSH_VECTOR", 1, CORE_0_INDEX, &US_TM_SCHEDULING_FLUSH_VECTOR, 5, 1, 1 },
#endif
#if defined BCM6846
	{ "GHOST_REPORTING_GLOBAL_CFG", 1, CORE_0_INDEX, &GHOST_REPORTING_GLOBAL_CFG, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "US_TM_BBH_TX_WAKE_UP_DATA_TABLE", 1, CORE_0_INDEX, &US_TM_BBH_TX_WAKE_UP_DATA_TABLE, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "SERVICE_QUEUES_COMPLEX_SCHEDULER_TABLE", 1, CORE_0_INDEX, &SERVICE_QUEUES_COMPLEX_SCHEDULER_TABLE, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "REPORTING_QUEUE_ACCUMULATED_TABLE", 1, CORE_0_INDEX, &REPORTING_QUEUE_ACCUMULATED_TABLE, 16, 1, 1 },
#endif
#if defined BCM6846
	{ "US_TM_SCHEDULING_QUEUE_TABLE", 1, CORE_0_INDEX, &US_TM_SCHEDULING_QUEUE_TABLE, 84, 1, 1 },
#endif
#if defined BCM6846
	{ "DS_TM_SCHEDULING_FLUSH_VECTOR", 1, CORE_0_INDEX, &DS_TM_SCHEDULING_FLUSH_VECTOR, 5, 1, 1 },
#endif
#if defined BCM6846
	{ "ZERO_VALUE", 1, CORE_0_INDEX, &ZERO_VALUE, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "DS_TM_BBH_TX_WAKE_UP_DATA_TABLE", 1, CORE_0_INDEX, &DS_TM_BBH_TX_WAKE_UP_DATA_TABLE, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "US_TM_BBH_TX_EGRESS_COUNTER_TABLE", 1, CORE_0_INDEX, &US_TM_BBH_TX_EGRESS_COUNTER_TABLE, 40, 1, 1 },
#endif
#if defined BCM6846
	{ "BBH_TX_EPON_WAKE_UP_DATA_TABLE", 1, CORE_0_INDEX, &BBH_TX_EPON_WAKE_UP_DATA_TABLE, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "XGPON_REPORT_TABLE", 1, CORE_0_INDEX, &XGPON_REPORT_TABLE, 4, 1, 1 },
#endif
#if defined BCM6846
	{ "BASIC_SCHEDULER_TABLE_DS", 1, CORE_0_INDEX, &BASIC_SCHEDULER_TABLE_DS, 16, 1, 1 },
#endif
#if defined BCM6846
	{ "DS_TM_PD_FIFO_TABLE", 1, CORE_0_INDEX, &DS_TM_PD_FIFO_TABLE, 80, 1, 1 },
#endif
#if defined BCM6846
	{ "COMPLEX_SCHEDULER_TABLE", 1, CORE_0_INDEX, &COMPLEX_SCHEDULER_TABLE, 4, 1, 1 },
#endif
#if defined BCM6846
	{ "SERVICE_QUEUES_PD_FIFO_TABLE", 1, CORE_0_INDEX, &SERVICE_QUEUES_PD_FIFO_TABLE, 16, 1, 1 },
#endif
#if defined BCM6846
	{ "REPORTING_COUNTER_TABLE", 1, CORE_0_INDEX, &REPORTING_COUNTER_TABLE, 40, 1, 1 },
#endif
#if defined BCM6846
	{ "US_TM_SCHEDULING_QUEUE_AGING_VECTOR", 1, CORE_0_INDEX, &US_TM_SCHEDULING_QUEUE_AGING_VECTOR, 5, 1, 1 },
#endif
#if defined BCM6846
	{ "ONE_VALUE", 1, CORE_0_INDEX, &ONE_VALUE, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "GENERAL_QUEUE_DYNAMIC_MNG_TABLE", 1, CORE_0_INDEX, &GENERAL_QUEUE_DYNAMIC_MNG_TABLE, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "BBH_TX_EPON_INGRESS_COUNTER_TABLE", 1, CORE_0_INDEX, &BBH_TX_EPON_INGRESS_COUNTER_TABLE, 40, 1, 1 },
#endif
#if defined BCM6846
	{ "EMAC_FLOW_CTRL_CTR_REP", 1, CORE_0_INDEX, &EMAC_FLOW_CTRL_CTR_REP, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "OVERALL_RATE_LIMITER_TABLE", 1, CORE_0_INDEX, &OVERALL_RATE_LIMITER_TABLE, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "BASIC_RATE_LIMITER_TABLE_US", 1, CORE_0_INDEX, &BASIC_RATE_LIMITER_TABLE_US, 64, 1, 1 },
#endif
#if defined BCM6846
	{ "DS_TM_SCHEDULING_QUEUE_TABLE", 1, CORE_0_INDEX, &DS_TM_SCHEDULING_QUEUE_TABLE, 84, 1, 1 },
#endif
#if defined BCM6846
	{ "DS_TM_SCHEDULING_QUEUE_AGING_VECTOR", 1, CORE_0_INDEX, &DS_TM_SCHEDULING_QUEUE_AGING_VECTOR, 5, 1, 1 },
#endif
#if defined BCM6846
	{ "US_TM_BBH_TX_FIFO_BYTES_USED", 1, CORE_0_INDEX, &US_TM_BBH_TX_FIFO_BYTES_USED, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "MIRRORING_SCRATCH", 1, CORE_0_INDEX, &MIRRORING_SCRATCH, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "US_TM_TM_ACTION_PTR_TABLE", 1, CORE_0_INDEX, &US_TM_TM_ACTION_PTR_TABLE, 17, 1, 1 },
#endif
#if defined BCM6846
	{ "BUDGET_ALLOCATION_TIMER_VALUE", 1, CORE_0_INDEX, &BUDGET_ALLOCATION_TIMER_VALUE, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "US_TM_BB_DESTINATION_TABLE", 1, CORE_0_INDEX, &US_TM_BB_DESTINATION_TABLE, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "DS_TM_BB_DESTINATION_TABLE", 1, CORE_0_INDEX, &DS_TM_BB_DESTINATION_TABLE, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "REPORT_BBH_TX_QUEUE_ID_TABLE", 1, CORE_0_INDEX, &REPORT_BBH_TX_QUEUE_ID_TABLE, 2, 1, 1 },
#endif
#if defined BCM6846
	{ "FLUSH_DISPATCHER_CREDIT_TABLE", 1, CORE_0_INDEX, &FLUSH_DISPATCHER_CREDIT_TABLE, 3, 1, 1 },
#endif
#if defined BCM6846
	{ "FLOW_CTRL_TIMER_VALUE", 1, CORE_0_INDEX, &FLOW_CTRL_TIMER_VALUE, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "SERVICE_QUEUES_BUDGET_ALLOCATION_TIMER_VALUE", 1, CORE_0_INDEX, &SERVICE_QUEUES_BUDGET_ALLOCATION_TIMER_VALUE, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "NATC_TBL_CFG", 1, CORE_0_INDEX, &NATC_TBL_CFG, 3, 1, 1 },
#endif
#if defined BCM6846
	{ "US_TM_CPU_TX_ABS_COUNTERS", 1, CORE_0_INDEX, &US_TM_CPU_TX_ABS_COUNTERS, 32, 1, 1 },
#endif
#if defined BCM6846
	{ "DS_TM_TM_FLOW_CNTR_TABLE", 1, CORE_0_INDEX, &DS_TM_TM_FLOW_CNTR_TABLE, 5, 1, 1 },
#endif
#if defined BCM6846
	{ "US_TM_SCHEDULING_GLOBAL_FLUSH_CFG", 1, CORE_0_INDEX, &US_TM_SCHEDULING_GLOBAL_FLUSH_CFG, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "DS_TM_SCHEDULING_GLOBAL_FLUSH_CFG", 1, CORE_0_INDEX, &DS_TM_SCHEDULING_GLOBAL_FLUSH_CFG, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "SCHEDULING_FLUSH_GLOBAL_CFG", 1, CORE_0_INDEX, &SCHEDULING_FLUSH_GLOBAL_CFG, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "DISPATCHER_CREDIT_TABLE", 1, CORE_0_INDEX, &DISPATCHER_CREDIT_TABLE, 3, 1, 1 },
#endif
#if defined BCM6846
	{ "TASK_IDX", 1, CORE_0_INDEX, &TASK_IDX, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "US_TM_SCHEDULING_AGGREGATION_CONTEXT_VECTOR", 1, CORE_0_INDEX, &US_TM_SCHEDULING_AGGREGATION_CONTEXT_VECTOR, 5, 1, 1 },
#endif
#if defined BCM6846
	{ "BBH_TX_US_FIFO_BYTES_THRESHOLD", 1, CORE_0_INDEX, &BBH_TX_US_FIFO_BYTES_THRESHOLD, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "EMAC_FLOW_CTRL_VECTOR", 1, CORE_0_INDEX, &EMAC_FLOW_CTRL_VECTOR, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "TM_vlan_stats_enable", 1, CORE_0_INDEX, &TM_vlan_stats_enable, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "BBH_TX_EGRESS_REPORT_COUNTER_TABLE", 1, CORE_0_INDEX, &BBH_TX_EGRESS_REPORT_COUNTER_TABLE, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "REPORTING_QUEUE_DESCRIPTOR_TABLE", 1, CORE_0_INDEX, &REPORTING_QUEUE_DESCRIPTOR_TABLE, 65, 1, 1 },
#endif
#if defined BCM6846
	{ "DS_TM_CPU_TX_ABS_COUNTERS", 1, CORE_0_INDEX, &DS_TM_CPU_TX_ABS_COUNTERS, 32, 1, 1 },
#endif
#if defined BCM6846
	{ "REGISTERS_BUFFER", 1, CORE_0_INDEX, &REGISTERS_BUFFER, 32, 1, 1 },
#endif
#if defined BCM6846
	{ "SERVICE_QUEUES_DISPATCHER_CREDIT_TABLE", 1, CORE_0_INDEX, &SERVICE_QUEUES_DISPATCHER_CREDIT_TABLE, 3, 1, 1 },
#endif
#if defined BCM6846
	{ "CORE_ID_TABLE", 1, CORE_0_INDEX, &CORE_ID_TABLE, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "US_TM_FLUSH_AGGREGATION_TASK_DISABLE", 1, CORE_0_INDEX, &US_TM_FLUSH_AGGREGATION_TASK_DISABLE, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "TX_MIRRORING_CONFIGURATION", 1, CORE_0_INDEX, &TX_MIRRORING_CONFIGURATION, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "DS_TM_SCHEDULING_AGGREGATION_CONTEXT_VECTOR", 1, CORE_0_INDEX, &DS_TM_SCHEDULING_AGGREGATION_CONTEXT_VECTOR, 5, 1, 1 },
#endif
#if defined BCM6846
	{ "DS_TM_FLUSH_AGGREGATION_TASK_DISABLE", 1, CORE_0_INDEX, &DS_TM_FLUSH_AGGREGATION_TASK_DISABLE, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "SRAM_DUMMY_STORE", 1, CORE_0_INDEX, &SRAM_DUMMY_STORE, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "RATE_LIMIT_OVERHEAD", 1, CORE_0_INDEX, &RATE_LIMIT_OVERHEAD, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "BBH_TX_FIFO_SIZE", 1, CORE_0_INDEX, &BBH_TX_FIFO_SIZE, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "US_TM_FIRST_QUEUE_MAPPING", 1, CORE_0_INDEX, &US_TM_FIRST_QUEUE_MAPPING, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "DS_TM_FIRST_QUEUE_MAPPING", 1, CORE_0_INDEX, &DS_TM_FIRST_QUEUE_MAPPING, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "SERVICE_QUEUES_FIRST_QUEUE_MAPPING", 1, CORE_0_INDEX, &SERVICE_QUEUES_FIRST_QUEUE_MAPPING, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "BBH_TX_INGRESS_COUNTER_TABLE", 1, CORE_0_INDEX, &BBH_TX_INGRESS_COUNTER_TABLE, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "DS_TM_TM_ACTION_PTR_TABLE", 1, CORE_0_INDEX, &DS_TM_TM_ACTION_PTR_TABLE, 17, 1, 1 },
#endif
#if defined BCM6846
	{ "TX_MIRRORING_DISPATCHER_CREDIT_TABLE", 1, CORE_0_INDEX, &TX_MIRRORING_DISPATCHER_CREDIT_TABLE, 3, 1, 1 },
#endif
#if defined BCM6846
	{ "SERVICE_QUEUES_TM_ACTION_PTR_TABLE", 1, CORE_0_INDEX, &SERVICE_QUEUES_TM_ACTION_PTR_TABLE, 17, 1, 1 },
#endif
#if defined BCM6846
	{ "FPM_GLOBAL_CFG", 1, CORE_0_INDEX, &FPM_GLOBAL_CFG, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "US_TM_UPDATE_FIFO_TABLE", 1, CORE_0_INDEX, &US_TM_UPDATE_FIFO_TABLE, 8, 1, 1 },
#endif
#if defined BCM6846
	{ "GHOST_REPORTING_QUEUE_STATUS_BIT_VECTOR_TABLE", 1, CORE_0_INDEX, &GHOST_REPORTING_QUEUE_STATUS_BIT_VECTOR_TABLE, 5, 1, 1 },
#endif
#if defined BCM6846
	{ "US_TM_BBH_QUEUE_TABLE", 1, CORE_0_INDEX, &US_TM_BBH_QUEUE_TABLE, 40, 1, 1 },
#endif
#if defined BCM6846
	{ "SERVICE_QUEUES_RATE_LIMITER_VALID_TABLE", 1, CORE_0_INDEX, &SERVICE_QUEUES_RATE_LIMITER_VALID_TABLE, 4, 1, 1 },
#endif
#if defined BCM6846
	{ "XGPON_REPORT_ZERO_SENT_TABLE", 1, CORE_0_INDEX, &XGPON_REPORT_ZERO_SENT_TABLE, 10, 1, 1 },
#endif
#if defined BCM6846
	{ "DS_TM_UPDATE_FIFO_TABLE", 1, CORE_0_INDEX, &DS_TM_UPDATE_FIFO_TABLE, 8, 1, 1 },
#endif
#if defined BCM6846
	{ "RATE_LIMITER_VALID_TABLE_US", 1, CORE_0_INDEX, &RATE_LIMITER_VALID_TABLE_US, 4, 1, 1 },
#endif
#if defined BCM6846
	{ "EPON_UPDATE_FIFO_TABLE", 1, CORE_0_INDEX, &EPON_UPDATE_FIFO_TABLE, 8, 1, 1 },
#endif
#if defined BCM6846
	{ "RATE_LIMITER_VALID_TABLE_DS", 1, CORE_0_INDEX, &RATE_LIMITER_VALID_TABLE_DS, 4, 1, 1 },
#endif
#if defined BCM6846
	{ "SERVICE_QUEUES_UPDATE_FIFO_TABLE", 1, CORE_0_INDEX, &SERVICE_QUEUES_UPDATE_FIFO_TABLE, 8, 1, 1 },
#endif
#if defined BCM6846
	{ "EMAC_FLOW_CTRL_BUDGET", 1, CORE_0_INDEX, &EMAC_FLOW_CTRL_BUDGET, 5, 1, 1 },
#endif
#if defined BCM6846
	{ "BBH_TX_EPON_EGRESS_COUNTER_TABLE", 1, CORE_0_INDEX, &BBH_TX_EPON_EGRESS_COUNTER_TABLE, 8, 1, 1 },
#endif
#if defined BCM6846
	{ "BASIC_RATE_LIMITER_TABLE_DS", 1, CORE_0_INDEX, &BASIC_RATE_LIMITER_TABLE_DS, 64, 1, 1 },
#endif
#if defined BCM6846
	{ "VLAN_TX_COUNTERS", 1, CORE_0_INDEX, &VLAN_TX_COUNTERS, 129, 1, 1 },
#endif
#if defined BCM6846
	{ "DS_TM_BBH_TX_EGRESS_COUNTER_TABLE", 1, CORE_0_INDEX, &DS_TM_BBH_TX_EGRESS_COUNTER_TABLE, 5, 1, 1 },
#endif
#if defined BCM6846
	{ "DS_TM_BBH_QUEUE_TABLE", 1, CORE_0_INDEX, &DS_TM_BBH_QUEUE_TABLE, 5, 1, 1 },
#endif
#if defined BCM6846
	{ "RUNNER_PROFILING_TRACE_BUFFER", 1, CORE_0_INDEX, &RUNNER_PROFILING_TRACE_BUFFER, 128, 1, 1 },
#endif
#if defined BCM6846
	{ "SCT_FILTER", 1, CORE_0_INDEX, &SCT_FILTER, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "TX_FLOW_TABLE", 1, CORE_1_INDEX, &TX_FLOW_TABLE_1, 272, 1, 1 },
#endif
#if defined BCM6846
	{ "DIRECT_PROCESSING_EPON_CONTROL_SCRATCH", 1, CORE_1_INDEX, &DIRECT_PROCESSING_EPON_CONTROL_SCRATCH_1, 16, 1, 1 },
#endif
#if defined BCM6846
	{ "RUNNER_GLOBAL_REGISTERS_INIT", 1, CORE_1_INDEX, &RUNNER_GLOBAL_REGISTERS_INIT_1, 8, 1, 1 },
#endif
#if defined BCM6846
	{ "DIRECT_PROCESSING_PD_TABLE", 1, CORE_1_INDEX, &DIRECT_PROCESSING_PD_TABLE_1, 2, 1, 1 },
#endif
#if defined BCM6846
	{ "VPORT_TO_FLOW_IDX", 1, CORE_1_INDEX, &VPORT_TO_FLOW_IDX_1, 32, 1, 1 },
#endif
#if defined BCM6846
	{ "VPORT_CFG_TABLE", 1, CORE_1_INDEX, &VPORT_CFG_TABLE_1, 16, 1, 1 },
#endif
#if defined BCM6846
	{ "LAYER2_GRE_TUNNEL_TABLE", 1, CORE_1_INDEX, &LAYER2_GRE_TUNNEL_TABLE_1, 12, 1, 1 },
#endif
#if defined BCM6846
	{ "PBIT_TO_GEM_TABLE", 1, CORE_1_INDEX, &PBIT_TO_GEM_TABLE_1, 16, 8, 1 },
#endif
#if defined BCM6846
	{ "CPU_RX_SCRATCHPAD", 1, CORE_1_INDEX, &CPU_RX_SCRATCHPAD_1, 64, 1, 1 },
#endif
#if defined BCM6846
	{ "DSCP_TO_PBITS_MAP_TABLE", 1, CORE_1_INDEX, &DSCP_TO_PBITS_MAP_TABLE_1, 4, 64, 1 },
#endif
#if defined BCM6846
	{ "CPU_RING_INTERRUPT_COUNTER_TABLE", 1, CORE_1_INDEX, &CPU_RING_INTERRUPT_COUNTER_TABLE_1, 18, 1, 1 },
#endif
#if defined BCM6846
	{ "VPORT_TO_DSCP_TO_PBITS_TABLE", 1, CORE_1_INDEX, &VPORT_TO_DSCP_TO_PBITS_TABLE_1, 16, 1, 1 },
#endif
#if defined BCM6846
	{ "CPU_FEED_RING_RSV_TABLE", 1, CORE_1_INDEX, &CPU_FEED_RING_RSV_TABLE_1, 16, 1, 1 },
#endif
#if defined BCM6846
	{ "GLOBAL_DSCP_TO_PBITS_TABLE", 1, CORE_1_INDEX, &GLOBAL_DSCP_TO_PBITS_TABLE_1, 1, 64, 1 },
#endif
#if defined BCM6846
	{ "RX_FLOW_TABLE", 1, CORE_1_INDEX, &RX_FLOW_TABLE_1, 320, 1, 1 },
#endif
#if defined BCM6846
	{ "DS_CPU_RX_METER_TABLE", 1, CORE_1_INDEX, &DS_CPU_RX_METER_TABLE_1, 16, 1, 1 },
#endif
#if defined BCM6846
	{ "US_CPU_RX_METER_TABLE", 1, CORE_1_INDEX, &US_CPU_RX_METER_TABLE_1, 16, 1, 1 },
#endif
#if defined BCM6846
	{ "LAYER2_HEADER_COPY_MAPPING_TABLE", 1, CORE_1_INDEX, &LAYER2_HEADER_COPY_MAPPING_TABLE_1, 30, 1, 1 },
#endif
#if defined BCM6846
	{ "IPTV_CONFIGURATION_TABLE", 1, CORE_1_INDEX, &IPTV_CONFIGURATION_TABLE_1, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "TCAM_IC_CMD_TABLE", 1, CORE_1_INDEX, &TCAM_IC_CMD_TABLE_1, 9, 16, 1 },
#endif
#if defined BCM6846
	{ "DUAL_STACK_LITE_TABLE", 1, CORE_1_INDEX, &DUAL_STACK_LITE_TABLE_1, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "CSO_CONTEXT_TABLE", 1, CORE_1_INDEX, &CSO_CONTEXT_TABLE_1, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "INGRESS_PACKET_BASED_MAPPING", 1, CORE_1_INDEX, &INGRESS_PACKET_BASED_MAPPING_1, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "FPM_POOL_NUMBER_MAPPING_TABLE", 1, CORE_1_INDEX, &FPM_POOL_NUMBER_MAPPING_TABLE_1, 16, 1, 1 },
#endif
#if defined BCM6846
	{ "INGRESS_FILTER_PROFILE_TABLE", 1, CORE_1_INDEX, &INGRESS_FILTER_PROFILE_TABLE_1, 16, 1, 1 },
#endif
#if defined BCM6846
	{ "POLICER_PARAMS_TABLE", 1, CORE_1_INDEX, &POLICER_PARAMS_TABLE_1, 80, 1, 1 },
#endif
#if defined BCM6846
	{ "LOOPBACK_QUEUE_TABLE", 1, CORE_1_INDEX, &LOOPBACK_QUEUE_TABLE_1, 16, 1, 1 },
#endif
#if defined BCM6846
	{ "IPV4_HOST_ADDRESS_TABLE", 1, CORE_1_INDEX, &IPV4_HOST_ADDRESS_TABLE_1, 8, 1, 1 },
#endif
#if defined BCM6846
	{ "DS_PACKET_BUFFER", 1, CORE_1_INDEX, &DS_PACKET_BUFFER_1, 8, 1, 1 },
#endif
#if defined BCM6846
	{ "TC_TO_QUEUE_TABLE", 1, CORE_1_INDEX, &TC_TO_QUEUE_TABLE_1, 65, 1, 1 },
#endif
#if defined BCM6846
	{ "GENERAL_QUEUE_DYNAMIC_MNG_TABLE", 1, CORE_1_INDEX, &GENERAL_QUEUE_DYNAMIC_MNG_TABLE_1, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "CPU_RECYCLE_INTERRUPT_COALESCING_TABLE", 1, CORE_1_INDEX, &CPU_RECYCLE_INTERRUPT_COALESCING_TABLE_1, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "TCAM_IC_HANDLER_TABLE", 1, CORE_1_INDEX, &TCAM_IC_HANDLER_TABLE_1, 16, 1, 1 },
#endif
#if defined BCM6846
	{ "DS_CPU_REASON_TO_METER_TABLE", 1, CORE_1_INDEX, &DS_CPU_REASON_TO_METER_TABLE_1, 64, 1, 1 },
#endif
#if defined BCM6846
	{ "CPU_RX_COPY_PD_FIFO_TABLE", 1, CORE_1_INDEX, &CPU_RX_COPY_PD_FIFO_TABLE_1, 4, 1, 1 },
#endif
#if defined BCM6846
	{ "US_CPU_REASON_TO_METER_TABLE", 1, CORE_1_INDEX, &US_CPU_REASON_TO_METER_TABLE_1, 64, 1, 1 },
#endif
#if defined BCM6846
	{ "EMAC_FLOW_CTRL", 1, CORE_1_INDEX, &EMAC_FLOW_CTRL_1, 5, 1, 1 },
#endif
#if defined BCM6846
	{ "NATC_COUNTERS_GENERIC_CONTEXT", 1, CORE_1_INDEX, &NATC_COUNTERS_GENERIC_CONTEXT_1, 128, 1, 1 },
#endif
#if defined BCM6846
	{ "CPU_RX_PSRAM_GET_NEXT_SCRATCHPAD", 1, CORE_1_INDEX, &CPU_RX_PSRAM_GET_NEXT_SCRATCHPAD_1, 4, 1, 1 },
#endif
#if defined BCM6846
	{ "SPDSVC_ANALYZER_PARAMS_TABLE", 1, CORE_1_INDEX, &SPDSVC_ANALYZER_PARAMS_TABLE_1, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "ZERO_VALUE", 1, CORE_1_INDEX, &ZERO_VALUE_1, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "PBIT_TO_QUEUE_TABLE", 1, CORE_1_INDEX, &PBIT_TO_QUEUE_TABLE_1, 65, 1, 1 },
#endif
#if defined BCM6846
	{ "PROCESSING_QUEUE_DYNAMIC_MNG_TABLE", 1, CORE_1_INDEX, &PROCESSING_QUEUE_DYNAMIC_MNG_TABLE_1, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "DHD_HW_CFG", 1, CORE_1_INDEX, &DHD_HW_CFG_1, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "BRIDGE_CFG_TABLE", 1, CORE_1_INDEX, &BRIDGE_CFG_TABLE_1, 3, 1, 1 },
#endif
#if defined BCM6846
	{ "BRIDGE_LAN_MAC", 1, CORE_1_INDEX, &BRIDGE_LAN_MAC_1, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "IPV6_HOST_ADDRESS_CRC_TABLE", 1, CORE_1_INDEX, &IPV6_HOST_ADDRESS_CRC_TABLE_1, 16, 1, 1 },
#endif
#if defined BCM6846
	{ "REGISTERS_BUFFER", 1, CORE_1_INDEX, &REGISTERS_BUFFER_1, 32, 1, 1 },
#endif
#if defined BCM6846
	{ "NATC_TBL_CFG", 1, CORE_1_INDEX, &NATC_TBL_CFG_1, 3, 1, 1 },
#endif
#if defined BCM6846
	{ "RX_FLOW_CONTEXT_DDR_ADDR", 1, CORE_1_INDEX, &RX_FLOW_CONTEXT_DDR_ADDR_1, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "INGRESS_FILTER_L2_REASON_TABLE", 1, CORE_1_INDEX, &INGRESS_FILTER_L2_REASON_TABLE_1, 16, 1, 1 },
#endif
#if defined BCM6846
	{ "TCAM_IC_CFG_TABLE", 1, CORE_1_INDEX, &TCAM_IC_CFG_TABLE_1, 8, 1, 1 },
#endif
#if defined BCM6846
	{ "SCRATCH", 1, CORE_1_INDEX, &SCRATCH_1, 8, 1, 1 },
#endif
#if defined BCM6846
	{ "RUNNER_PROFILING_TRACE_BUFFER", 1, CORE_1_INDEX, &RUNNER_PROFILING_TRACE_BUFFER_1, 128, 1, 1 },
#endif
#if defined BCM6846
	{ "CPU_RECYCLE_SRAM_PD_FIFO", 1, CORE_1_INDEX, &CPU_RECYCLE_SRAM_PD_FIFO_1, 16, 1, 1 },
#endif
#if defined BCM6846
	{ "TCPSPDTEST_ENGINE_REF_PKT_HDR_TABLE", 1, CORE_1_INDEX, &TCPSPDTEST_ENGINE_REF_PKT_HDR_TABLE_1, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "CPU_FEED_RING_INTERRUPT_COUNTER_MAX", 1, CORE_1_INDEX, &CPU_FEED_RING_INTERRUPT_COUNTER_MAX_1, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "TCPSPDTEST_ENGINE_TCB_TABLE", 1, CORE_1_INDEX, &TCPSPDTEST_ENGINE_TCB_TABLE_1, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "NULL_BUFFER", 1, CORE_1_INDEX, &NULL_BUFFER_1, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "CPU_REASON_AND_VPORT_TO_METER_TABLE", 1, CORE_1_INDEX, &CPU_REASON_AND_VPORT_TO_METER_TABLE_1, 48, 1, 1 },
#endif
#if defined BCM6846
	{ "DHD_RX_COMPLETE_0_DISPATCHER_CREDIT_TABLE", 1, CORE_1_INDEX, &DHD_RX_COMPLETE_0_DISPATCHER_CREDIT_TABLE_1, 3, 1, 1 },
#endif
#if defined BCM6846
	{ "ONE_VALUE", 1, CORE_1_INDEX, &ONE_VALUE_1, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "TCPSPDTEST_ENGINE_PKT_DROP_TABLE", 1, CORE_1_INDEX, &TCPSPDTEST_ENGINE_PKT_DROP_TABLE_1, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "TX_ABS_RECYCLE_COUNTERS", 1, CORE_1_INDEX, &TX_ABS_RECYCLE_COUNTERS_1, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "DHD_RX_COMPLETE_1_DISPATCHER_CREDIT_TABLE", 1, CORE_1_INDEX, &DHD_RX_COMPLETE_1_DISPATCHER_CREDIT_TABLE_1, 3, 1, 1 },
#endif
#if defined BCM6846
	{ "CPU_RX_INTERRUPT_SCRATCH", 1, CORE_1_INDEX, &CPU_RX_INTERRUPT_SCRATCH_1, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "DHD_RX_COMPLETE_2_DISPATCHER_CREDIT_TABLE", 1, CORE_1_INDEX, &DHD_RX_COMPLETE_2_DISPATCHER_CREDIT_TABLE_1, 3, 1, 1 },
#endif
#if defined BCM6846
	{ "TASK_IDX", 1, CORE_1_INDEX, &TASK_IDX_1, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "DHD_TX_COMPLETE_0_DISPATCHER_CREDIT_TABLE", 1, CORE_1_INDEX, &DHD_TX_COMPLETE_0_DISPATCHER_CREDIT_TABLE_1, 3, 1, 1 },
#endif
#if defined BCM6846
	{ "FORCE_DSCP", 1, CORE_1_INDEX, &FORCE_DSCP_1, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "CORE_ID_TABLE", 1, CORE_1_INDEX, &CORE_ID_TABLE_1, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "CPU_FEED_RING_INTERRUPT_THRESHOLD", 1, CORE_1_INDEX, &CPU_FEED_RING_INTERRUPT_THRESHOLD_1, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "SPDSVC_GEN_PARAMS_TABLE", 1, CORE_1_INDEX, &SPDSVC_GEN_PARAMS_TABLE_1, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "CPU_RECYCLE_INTERRUPT_ID_DDR_ADDR", 1, CORE_1_INDEX, &CPU_RECYCLE_INTERRUPT_ID_DDR_ADDR_1, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "DHD_TX_COMPLETE_1_DISPATCHER_CREDIT_TABLE", 1, CORE_1_INDEX, &DHD_TX_COMPLETE_1_DISPATCHER_CREDIT_TABLE_1, 3, 1, 1 },
#endif
#if defined BCM6846
	{ "CPU_FEED_RING_INTERRUPT_COUNTER", 1, CORE_1_INDEX, &CPU_FEED_RING_INTERRUPT_COUNTER_1, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "DHD_RX_POST_RING_SIZE", 1, CORE_1_INDEX, &DHD_RX_POST_RING_SIZE_1, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "CPU_VPORT_TO_METER_TABLE", 1, CORE_1_INDEX, &CPU_VPORT_TO_METER_TABLE_1, 40, 1, 1 },
#endif
#if defined BCM6846
	{ "CPU_RX_INTERRUPT_ID_DDR_ADDR", 1, CORE_1_INDEX, &CPU_RX_INTERRUPT_ID_DDR_ADDR_1, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "DHD_TX_COMPLETE_2_DISPATCHER_CREDIT_TABLE", 1, CORE_1_INDEX, &DHD_TX_COMPLETE_2_DISPATCHER_CREDIT_TABLE_1, 3, 1, 1 },
#endif
#if defined BCM6846
	{ "DHD_RX_COMPLETE_RING_SIZE", 1, CORE_1_INDEX, &DHD_RX_COMPLETE_RING_SIZE_1, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "DHD_TX_COMPLETE_RING_SIZE", 1, CORE_1_INDEX, &DHD_TX_COMPLETE_RING_SIZE_1, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "FLOW_BASED_ACTION_PTR_TABLE", 1, CORE_1_INDEX, &FLOW_BASED_ACTION_PTR_TABLE_1, 18, 1, 1 },
#endif
#if defined BCM6846
	{ "RX_MIRRORING_CONFIGURATION", 1, CORE_1_INDEX, &RX_MIRRORING_CONFIGURATION_1, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "SRAM_DUMMY_STORE", 1, CORE_1_INDEX, &SRAM_DUMMY_STORE_1, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "RATE_LIMIT_OVERHEAD", 1, CORE_1_INDEX, &RATE_LIMIT_OVERHEAD_1, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "TCPSPDTEST_ENGINE_CONN_INFO_TABLE", 1, CORE_1_INDEX, &TCPSPDTEST_ENGINE_CONN_INFO_TABLE_1, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "CPU_RX_COPY_DISPATCHER_CREDIT_TABLE", 1, CORE_1_INDEX, &CPU_RX_COPY_DISPATCHER_CREDIT_TABLE_1, 3, 1, 1 },
#endif
#if defined BCM6846
	{ "INGRESS_QOS_DONT_DROP_RATIO", 1, CORE_1_INDEX, &INGRESS_QOS_DONT_DROP_RATIO_1, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "INGRESS_QOS_WAN_UNTAGGED_PRIORITY", 1, CORE_1_INDEX, &INGRESS_QOS_WAN_UNTAGGED_PRIORITY_1, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "MAC_TYPE", 1, CORE_1_INDEX, &MAC_TYPE_1, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "IC_MCAST_ENABLE", 1, CORE_1_INDEX, &IC_MCAST_ENABLE_1, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "RULE_BASED_ACTION_PTR_TABLE", 1, CORE_1_INDEX, &RULE_BASED_ACTION_PTR_TABLE_1, 17, 1, 1 },
#endif
#if defined BCM6846
	{ "ECN_IPV6_REMARK_TABLE", 1, CORE_1_INDEX, &ECN_IPV6_REMARK_TABLE_1, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "CPU_REDIRECT_MODE", 1, CORE_1_INDEX, &CPU_REDIRECT_MODE_1, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "IPTV_CLASSIFICATION_CFG_TABLE", 1, CORE_1_INDEX, &IPTV_CLASSIFICATION_CFG_TABLE_1, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "INGRESS_FILTER_1588_CFG", 1, CORE_1_INDEX, &INGRESS_FILTER_1588_CFG_1, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "RX_MIRRORING_DIRECT_ENABLE", 1, CORE_1_INDEX, &RX_MIRRORING_DIRECT_ENABLE_1, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "DHD_CPU_RECYCLE_INTERRUPT_ID_DDR_ADDR", 1, CORE_1_INDEX, &DHD_CPU_RECYCLE_INTERRUPT_ID_DDR_ADDR_1, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "WAN_LOOPBACK_DISPATCHER_CREDIT_TABLE", 1, CORE_1_INDEX, &WAN_LOOPBACK_DISPATCHER_CREDIT_TABLE_1, 3, 1, 1 },
#endif
#if defined BCM6846
	{ "IPTV_ACTION_PTR_TABLE", 1, CORE_1_INDEX, &IPTV_ACTION_PTR_TABLE_1, 17, 1, 1 },
#endif
#if defined BCM6846
	{ "DHD_FPM_THRESHOLDS", 1, CORE_1_INDEX, &DHD_FPM_THRESHOLDS_1, 3, 1, 1 },
#endif
#if defined BCM6846
	{ "SPDSVC_GEN_DISPATCHER_CREDIT_TABLE", 1, CORE_1_INDEX, &SPDSVC_GEN_DISPATCHER_CREDIT_TABLE_1, 3, 1, 1 },
#endif
#if defined BCM6846
	{ "VLAN_ACTION_GPE_HANDLER_PTR_TABLE", 1, CORE_1_INDEX, &VLAN_ACTION_GPE_HANDLER_PTR_TABLE_1, 17, 1, 1 },
#endif
#if defined BCM6846
	{ "SYSTEM_CONFIGURATION", 1, CORE_1_INDEX, &SYSTEM_CONFIGURATION_1, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "CPU_RECYCLE_RING_DESCRIPTOR_TABLE", 1, CORE_1_INDEX, &CPU_RECYCLE_RING_DESCRIPTOR_TABLE_1, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "UPDATE_FIFO_TABLE", 1, CORE_1_INDEX, &UPDATE_FIFO_TABLE_1, 8, 1, 1 },
#endif
#if defined BCM6846
	{ "CPU_RECYCLE_RING_INDEX_DDR_ADDR_TABLE", 1, CORE_1_INDEX, &CPU_RECYCLE_RING_INDEX_DDR_ADDR_TABLE_1, 2, 1, 1 },
#endif
#if defined BCM6846
	{ "CPU_INTERRUPT_COALESCING_TABLE", 1, CORE_1_INDEX, &CPU_INTERRUPT_COALESCING_TABLE_1, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "CPU_RING_DESCRIPTORS_TABLE", 1, CORE_1_INDEX, &CPU_RING_DESCRIPTORS_TABLE_1, 16, 1, 1 },
#endif
#if defined BCM6846
	{ "CPU_RX_COPY_UPDATE_FIFO_TABLE", 1, CORE_1_INDEX, &CPU_RX_COPY_UPDATE_FIFO_TABLE_1, 8, 1, 1 },
#endif
#if defined BCM6846
	{ "CPU_FEED_RING_DESCRIPTOR_TABLE", 1, CORE_1_INDEX, &CPU_FEED_RING_DESCRIPTOR_TABLE_1, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "CPU_RX_LOCAL_SCRATCH", 1, CORE_1_INDEX, &CPU_RX_LOCAL_SCRATCH_1, 2, 1, 1 },
#endif
#if defined BCM6846
	{ "TX_ABS_RECYCLE_UPDATE_FIFO_TABLE", 1, CORE_1_INDEX, &TX_ABS_RECYCLE_UPDATE_FIFO_TABLE_1, 8, 1, 1 },
#endif
#if defined BCM6846
	{ "FPM_GLOBAL_CFG", 1, CORE_1_INDEX, &FPM_GLOBAL_CFG_1, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "CPU_RECYCLE_RING_INTERRUPT_COUNTER_TABLE", 1, CORE_1_INDEX, &CPU_RECYCLE_RING_INTERRUPT_COUNTER_TABLE_1, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "CPU_RX_COPY_LOCAL_SCRATCH", 1, CORE_1_INDEX, &CPU_RX_COPY_LOCAL_SCRATCH_1, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "PD_FIFO_TABLE", 1, CORE_1_INDEX, &PD_FIFO_TABLE_1, 2, 1, 1 },
#endif
#if defined BCM6846
	{ "TUNNELS_PARSING_CFG", 1, CORE_1_INDEX, &TUNNELS_PARSING_CFG_1, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "IPTV_CFG_TABLE", 1, CORE_1_INDEX, &IPTV_CFG_TABLE_1, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "DHD_DOORBELL_TX_COMPLETE_VALUE", 1, CORE_1_INDEX, &DHD_DOORBELL_TX_COMPLETE_VALUE_1, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "IPTV_DDR_CTX_TABLE_ADDRESS", 1, CORE_1_INDEX, &IPTV_DDR_CTX_TABLE_ADDRESS_1, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "TX_ABS_RECYCLE_PD_FIFO_TABLE", 1, CORE_1_INDEX, &TX_ABS_RECYCLE_PD_FIFO_TABLE_1, 2, 1, 1 },
#endif
#if defined BCM6846
	{ "DHD_DOORBELL_RX_COMPLETE_VALUE", 1, CORE_1_INDEX, &DHD_DOORBELL_RX_COMPLETE_VALUE_1, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "NAT_CACHE_CFG", 1, CORE_1_INDEX, &NAT_CACHE_CFG_1, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "DHD_DOORBELL_RX_POST_VALUE", 1, CORE_1_INDEX, &DHD_DOORBELL_RX_POST_VALUE_1, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "NAT_CACHE_KEY0_MASK", 1, CORE_1_INDEX, &NAT_CACHE_KEY0_MASK_1, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "DHD_COMPLETE_COMMON_RADIO_DATA", 1, CORE_1_INDEX, &DHD_COMPLETE_COMMON_RADIO_DATA_1, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "NATC_L2_VLAN_KEY_MASK", 1, CORE_1_INDEX, &NATC_L2_VLAN_KEY_MASK_1, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "INGRESS_FILTER_CFG", 1, CORE_1_INDEX, &INGRESS_FILTER_CFG_1, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "VPORT_CFG_EX_TABLE", 1, CORE_1_INDEX, &VPORT_CFG_EX_TABLE_1, 16, 1, 1 },
#endif
#if defined BCM6846
	{ "CPU_REASON_TO_TC", 1, CORE_1_INDEX, &CPU_REASON_TO_TC_1, 64, 1, 1 },
#endif
#if defined BCM6846
	{ "TC_TO_CPU_RXQ", 1, CORE_1_INDEX, &TC_TO_CPU_RXQ_1, 64, 1, 1 },
#endif
#if defined BCM6846
	{ "EXC_TC_TO_CPU_RXQ", 1, CORE_1_INDEX, &EXC_TC_TO_CPU_RXQ_1, 64, 1, 1 },
#endif
#if defined BCM6846
	{ "NATC_L2_TOS_MASK", 1, CORE_1_INDEX, &NATC_L2_TOS_MASK_1, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "DHD_RX_COMPLETE_FLOW_RING_BUFFER", 1, CORE_1_INDEX, &DHD_RX_COMPLETE_FLOW_RING_BUFFER_1, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "VPORT_TO_CPU_OBJ", 1, CORE_1_INDEX, &VPORT_TO_CPU_OBJ_1, 40, 1, 1 },
#endif
#if defined BCM6846
	{ "DHD_RX_POST_FLOW_RING_BUFFER", 1, CORE_1_INDEX, &DHD_RX_POST_FLOW_RING_BUFFER_1, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "DHD_TX_COMPLETE_FLOW_RING_BUFFER", 1, CORE_1_INDEX, &DHD_TX_COMPLETE_FLOW_RING_BUFFER_1, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "DHD_CPU_RECYCLE_RING_DESCRIPTOR_TABLE", 1, CORE_1_INDEX, &DHD_CPU_RECYCLE_RING_DESCRIPTOR_TABLE_1, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "DHD_FPM_REPLY", 1, CORE_1_INDEX, &DHD_FPM_REPLY_1, 8, 1, 1 },
#endif
#if defined BCM6846
	{ "CPU_RECYCLE_INTERRUPT_SCRATCH", 1, CORE_1_INDEX, &CPU_RECYCLE_INTERRUPT_SCRATCH_1, 2, 1, 1 },
#endif
#if defined BCM6846
	{ "CPU_RECYCLE_SHADOW_RD_IDX", 1, CORE_1_INDEX, &CPU_RECYCLE_SHADOW_RD_IDX_1, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "CPU_RECYCLE_SHADOW_WR_IDX", 1, CORE_1_INDEX, &CPU_RECYCLE_SHADOW_WR_IDX_1, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "SCT_FILTER", 1, CORE_1_INDEX, &SCT_FILTER_1, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "CPU_TX_SCRATCHPAD", 1, CORE_2_INDEX, &CPU_TX_SCRATCHPAD_2, 128, 1, 1 },
#endif
#if defined BCM6846
	{ "RX_FLOW_TABLE", 1, CORE_2_INDEX, &RX_FLOW_TABLE_2, 320, 1, 1 },
#endif
#if defined BCM6846
	{ "VPORT_CFG_TABLE", 1, CORE_2_INDEX, &VPORT_CFG_TABLE_2, 16, 1, 1 },
#endif
#if defined BCM6846
	{ "DSCP_TO_PBITS_MAP_TABLE", 1, CORE_2_INDEX, &DSCP_TO_PBITS_MAP_TABLE_2, 4, 64, 1 },
#endif
#if defined BCM6846
	{ "TCAM_IC_CMD_TABLE", 1, CORE_2_INDEX, &TCAM_IC_CMD_TABLE_2, 9, 16, 1 },
#endif
#if defined BCM6846
	{ "GLOBAL_DSCP_TO_PBITS_TABLE", 1, CORE_2_INDEX, &GLOBAL_DSCP_TO_PBITS_TABLE_2, 1, 64, 1 },
#endif
#if defined BCM6846
	{ "PBIT_TO_GEM_TABLE", 1, CORE_2_INDEX, &PBIT_TO_GEM_TABLE_2, 16, 8, 1 },
#endif
#if defined BCM6846
	{ "DHD_BACKUP_INFO_CACHE_TABLE", 1, CORE_2_INDEX, &DHD_BACKUP_INFO_CACHE_TABLE_2, 16, 1, 1 },
#endif
#if defined BCM6846
	{ "LAYER2_HEADER_COPY_MAPPING_TABLE", 1, CORE_2_INDEX, &LAYER2_HEADER_COPY_MAPPING_TABLE_2, 30, 1, 1 },
#endif
#if defined BCM6846
	{ "IPTV_CONFIGURATION_TABLE", 1, CORE_2_INDEX, &IPTV_CONFIGURATION_TABLE_2, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "TC_TO_QUEUE_TABLE", 1, CORE_2_INDEX, &TC_TO_QUEUE_TABLE_2, 65, 1, 1 },
#endif
#if defined BCM6846
	{ "GENERAL_QUEUE_DYNAMIC_MNG_TABLE", 1, CORE_2_INDEX, &GENERAL_QUEUE_DYNAMIC_MNG_TABLE_2, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "VPORT_TO_DSCP_TO_PBITS_TABLE", 1, CORE_2_INDEX, &VPORT_TO_DSCP_TO_PBITS_TABLE_2, 16, 1, 1 },
#endif
#if defined BCM6846
	{ "RUNNER_GLOBAL_REGISTERS_INIT", 1, CORE_2_INDEX, &RUNNER_GLOBAL_REGISTERS_INIT_2, 8, 1, 1 },
#endif
#if defined BCM6846
	{ "DUAL_STACK_LITE_TABLE", 1, CORE_2_INDEX, &DUAL_STACK_LITE_TABLE_2, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "POLICER_PARAMS_TABLE", 1, CORE_2_INDEX, &POLICER_PARAMS_TABLE_2, 80, 1, 1 },
#endif
#if defined BCM6846
	{ "FPM_POOL_NUMBER_MAPPING_TABLE", 1, CORE_2_INDEX, &FPM_POOL_NUMBER_MAPPING_TABLE_2, 16, 1, 1 },
#endif
#if defined BCM6846
	{ "TCAM_IC_HANDLER_TABLE", 1, CORE_2_INDEX, &TCAM_IC_HANDLER_TABLE_2, 16, 1, 1 },
#endif
#if defined BCM6846
	{ "INGRESS_FILTER_PROFILE_TABLE", 1, CORE_2_INDEX, &INGRESS_FILTER_PROFILE_TABLE_2, 16, 1, 1 },
#endif
#if defined BCM6846
	{ "NATC_COUNTERS_GENERIC_CONTEXT", 1, CORE_2_INDEX, &NATC_COUNTERS_GENERIC_CONTEXT_2, 128, 1, 1 },
#endif
#if defined BCM6846
	{ "DS_PACKET_BUFFER", 1, CORE_2_INDEX, &DS_PACKET_BUFFER_2, 8, 1, 1 },
#endif
#if defined BCM6846
	{ "PBIT_TO_QUEUE_TABLE", 1, CORE_2_INDEX, &PBIT_TO_QUEUE_TABLE_2, 65, 1, 1 },
#endif
#if defined BCM6846
	{ "REGISTERS_BUFFER", 1, CORE_2_INDEX, &REGISTERS_BUFFER_2, 32, 1, 1 },
#endif
#if defined BCM6846
	{ "PROCESSING_QUEUE_DYNAMIC_MNG_TABLE", 1, CORE_2_INDEX, &PROCESSING_QUEUE_DYNAMIC_MNG_TABLE_2, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "CPU_TX_FPM_POOL_NUMBER_MAPPING_TABLE", 1, CORE_2_INDEX, &CPU_TX_FPM_POOL_NUMBER_MAPPING_TABLE_2, 16, 1, 1 },
#endif
#if defined BCM6846
	{ "SPDSVC_ANALYZER_PARAMS_TABLE", 1, CORE_2_INDEX, &SPDSVC_ANALYZER_PARAMS_TABLE_2, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "INGRESS_PACKET_BASED_MAPPING", 1, CORE_2_INDEX, &INGRESS_PACKET_BASED_MAPPING_2, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "WLAN_MCAST_DHD_STATION_CTX_TABLE", 1, CORE_2_INDEX, &WLAN_MCAST_DHD_STATION_CTX_TABLE_2, 64, 1, 1 },
#endif
#if defined BCM6846
	{ "DHD_POST_COMMON_RADIO_DATA", 1, CORE_2_INDEX, &DHD_POST_COMMON_RADIO_DATA_2, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "BRIDGE_LAN_MAC", 1, CORE_2_INDEX, &BRIDGE_LAN_MAC_2, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "DHD_FPM_POOL_NUMBER_MAPPING_TABLE", 1, CORE_2_INDEX, &DHD_FPM_POOL_NUMBER_MAPPING_TABLE_2, 16, 1, 1 },
#endif
#if defined BCM6846
	{ "WLAN_MCAST_DFT_LIST_SIZE", 1, CORE_2_INDEX, &WLAN_MCAST_DFT_LIST_SIZE_2, 64, 1, 1 },
#endif
#if defined BCM6846
	{ "WLAN_MCAST_DFT_LIST_ENTRY_SCRATCH", 1, CORE_2_INDEX, &WLAN_MCAST_DFT_LIST_ENTRY_SCRATCH_2, 64, 1, 1 },
#endif
#if defined BCM6846
	{ "RUNNER_PROFILING_TRACE_BUFFER", 1, CORE_2_INDEX, &RUNNER_PROFILING_TRACE_BUFFER_2, 128, 1, 1 },
#endif
#if defined BCM6846
	{ "WLAN_MCAST_COPY_SCRATCHPAD", 1, CORE_2_INDEX, &WLAN_MCAST_COPY_SCRATCHPAD_2, 64, 1, 1 },
#endif
#if defined BCM6846
	{ "WLAN_MCAST_DHD_STATION_TABLE", 1, CORE_2_INDEX, &WLAN_MCAST_DHD_STATION_TABLE_2, 64, 1, 1 },
#endif
#if defined BCM6846
	{ "LAYER2_GRE_TUNNEL_TABLE", 1, CORE_2_INDEX, &LAYER2_GRE_TUNNEL_TABLE_2, 12, 1, 1 },
#endif
#if defined BCM6846
	{ "TCAM_IC_CFG_TABLE", 1, CORE_2_INDEX, &TCAM_IC_CFG_TABLE_2, 8, 1, 1 },
#endif
#if defined BCM6846
	{ "TX_FLOW_TABLE", 1, CORE_2_INDEX, &TX_FLOW_TABLE_2, 272, 1, 1 },
#endif
#if defined BCM6846
	{ "FPM_REPLY", 1, CORE_2_INDEX, &FPM_REPLY_2, 2, 1, 1 },
#endif
#if defined BCM6846
	{ "BRIDGE_CFG_TABLE", 1, CORE_2_INDEX, &BRIDGE_CFG_TABLE_2, 3, 1, 1 },
#endif
#if defined BCM6846
	{ "RX_FLOW_CONTEXT_DDR_ADDR", 1, CORE_2_INDEX, &RX_FLOW_CONTEXT_DDR_ADDR_2, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "WLAN_MCAST_SCRATCHPAD", 1, CORE_2_INDEX, &WLAN_MCAST_SCRATCHPAD_2, 5, 1, 1 },
#endif
#if defined BCM6846
	{ "NULL_BUFFER", 1, CORE_2_INDEX, &NULL_BUFFER_2, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "LOOPBACK_QUEUE_TABLE", 1, CORE_2_INDEX, &LOOPBACK_QUEUE_TABLE_2, 16, 1, 1 },
#endif
#if defined BCM6846
	{ "FLOW_BASED_ACTION_PTR_TABLE", 1, CORE_2_INDEX, &FLOW_BASED_ACTION_PTR_TABLE_2, 18, 1, 1 },
#endif
#if defined BCM6846
	{ "ZERO_VALUE", 1, CORE_2_INDEX, &ZERO_VALUE_2, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "CPU_RECYCLE_INTERRUPT_ID_DDR_ADDR", 1, CORE_2_INDEX, &CPU_RECYCLE_INTERRUPT_ID_DDR_ADDR_2, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "CPU_RECYCLE_INTERRUPT_COALESCING_TABLE", 1, CORE_2_INDEX, &CPU_RECYCLE_INTERRUPT_COALESCING_TABLE_2, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "QUEUE_THRESHOLD_VECTOR", 1, CORE_2_INDEX, &QUEUE_THRESHOLD_VECTOR_2, 9, 1, 1 },
#endif
#if defined BCM6846
	{ "ONE_VALUE", 1, CORE_2_INDEX, &ONE_VALUE_2, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "CPU_RX_INTERRUPT_ID_DDR_ADDR", 1, CORE_2_INDEX, &CPU_RX_INTERRUPT_ID_DDR_ADDR_2, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "DHD_HW_CFG", 1, CORE_2_INDEX, &DHD_HW_CFG_2, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "CPU_RECYCLE_SRAM_PD_FIFO", 1, CORE_2_INDEX, &CPU_RECYCLE_SRAM_PD_FIFO_2, 16, 1, 1 },
#endif
#if defined BCM6846
	{ "EMAC_FLOW_CTRL", 1, CORE_2_INDEX, &EMAC_FLOW_CTRL_2, 5, 1, 1 },
#endif
#if defined BCM6846
	{ "INGRESS_FILTER_L2_REASON_TABLE", 1, CORE_2_INDEX, &INGRESS_FILTER_L2_REASON_TABLE_2, 16, 1, 1 },
#endif
#if defined BCM6846
	{ "CPU_TX_EGRESS_DISPATCHER_CREDIT_TABLE", 1, CORE_2_INDEX, &CPU_TX_EGRESS_DISPATCHER_CREDIT_TABLE_2, 3, 1, 1 },
#endif
#if defined BCM6846
	{ "TASK_IDX", 1, CORE_2_INDEX, &TASK_IDX_2, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "CPU_TX_INGRESS_DISPATCHER_CREDIT_TABLE", 1, CORE_2_INDEX, &CPU_TX_INGRESS_DISPATCHER_CREDIT_TABLE_2, 3, 1, 1 },
#endif
#if defined BCM6846
	{ "FORCE_DSCP", 1, CORE_2_INDEX, &FORCE_DSCP_2, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "CPU_TX_vlan_stats_enable", 1, CORE_2_INDEX, &CPU_TX_vlan_stats_enable_2, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "CORE_ID_TABLE", 1, CORE_2_INDEX, &CORE_ID_TABLE_2, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "SRAM_DUMMY_STORE", 1, CORE_2_INDEX, &SRAM_DUMMY_STORE_2, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "RULE_BASED_ACTION_PTR_TABLE", 1, CORE_2_INDEX, &RULE_BASED_ACTION_PTR_TABLE_2, 17, 1, 1 },
#endif
#if defined BCM6846
	{ "RX_MIRRORING_CONFIGURATION", 1, CORE_2_INDEX, &RX_MIRRORING_CONFIGURATION_2, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "RATE_LIMIT_OVERHEAD", 1, CORE_2_INDEX, &RATE_LIMIT_OVERHEAD_2, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "INGRESS_QOS_DONT_DROP_RATIO", 1, CORE_2_INDEX, &INGRESS_QOS_DONT_DROP_RATIO_2, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "INGRESS_QOS_WAN_UNTAGGED_PRIORITY", 1, CORE_2_INDEX, &INGRESS_QOS_WAN_UNTAGGED_PRIORITY_2, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "IC_MCAST_ENABLE", 1, CORE_2_INDEX, &IC_MCAST_ENABLE_2, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "DHD_CPU_RECYCLE_INTERRUPT_ID_DDR_ADDR", 1, CORE_2_INDEX, &DHD_CPU_RECYCLE_INTERRUPT_ID_DDR_ADDR_2, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "DHD_MCAST_DISPATCHER_CREDIT_TABLE", 1, CORE_2_INDEX, &DHD_MCAST_DISPATCHER_CREDIT_TABLE_2, 3, 1, 1 },
#endif
#if defined BCM6846
	{ "ECN_IPV6_REMARK_TABLE", 1, CORE_2_INDEX, &ECN_IPV6_REMARK_TABLE_2, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "CPU_REDIRECT_MODE", 1, CORE_2_INDEX, &CPU_REDIRECT_MODE_2, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "IPTV_CLASSIFICATION_CFG_TABLE", 1, CORE_2_INDEX, &IPTV_CLASSIFICATION_CFG_TABLE_2, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "INGRESS_FILTER_1588_CFG", 1, CORE_2_INDEX, &INGRESS_FILTER_1588_CFG_2, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "IPTV_ACTION_PTR_TABLE", 1, CORE_2_INDEX, &IPTV_ACTION_PTR_TABLE_2, 17, 1, 1 },
#endif
#if defined BCM6846
	{ "RX_MIRRORING_DIRECT_ENABLE", 1, CORE_2_INDEX, &RX_MIRRORING_DIRECT_ENABLE_2, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "DHD_BACKUP_BASE_ADDR", 1, CORE_2_INDEX, &DHD_BACKUP_BASE_ADDR_2, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "CPU_RECYCLE_RING_DESCRIPTOR_TABLE", 1, CORE_2_INDEX, &CPU_RECYCLE_RING_DESCRIPTOR_TABLE_2, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "DHD_FLOW_RING_CACHE_CTX_TABLE", 1, CORE_2_INDEX, &DHD_FLOW_RING_CACHE_CTX_TABLE_2, 16, 1, 1 },
#endif
#if defined BCM6846
	{ "NATC_TBL_CFG", 1, CORE_2_INDEX, &NATC_TBL_CFG_2, 3, 1, 1 },
#endif
#if defined BCM6846
	{ "CPU_RECYCLE_RING_INDEX_DDR_ADDR_TABLE", 1, CORE_2_INDEX, &CPU_RECYCLE_RING_INDEX_DDR_ADDR_TABLE_2, 2, 1, 1 },
#endif
#if defined BCM6846
	{ "WLAN_MCAST_DFT_ADDR", 1, CORE_2_INDEX, &WLAN_MCAST_DFT_ADDR_2, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "CPU_FEED_RING_DESCRIPTOR_TABLE", 1, CORE_2_INDEX, &CPU_FEED_RING_DESCRIPTOR_TABLE_2, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "FPM_GLOBAL_CFG", 1, CORE_2_INDEX, &FPM_GLOBAL_CFG_2, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "VLAN_ACTION_GPE_HANDLER_PTR_TABLE", 1, CORE_2_INDEX, &VLAN_ACTION_GPE_HANDLER_PTR_TABLE_2, 17, 1, 1 },
#endif
#if defined BCM6846
	{ "DHD_FPM_THRESHOLDS", 1, CORE_2_INDEX, &DHD_FPM_THRESHOLDS_2, 3, 1, 1 },
#endif
#if defined BCM6846
	{ "SYSTEM_CONFIGURATION", 1, CORE_2_INDEX, &SYSTEM_CONFIGURATION_2, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "CPU_RECYCLE_RING_INTERRUPT_COUNTER_TABLE", 1, CORE_2_INDEX, &CPU_RECYCLE_RING_INTERRUPT_COUNTER_TABLE_2, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "CPU_TX_EGRESS_UPDATE_FIFO_TABLE", 1, CORE_2_INDEX, &CPU_TX_EGRESS_UPDATE_FIFO_TABLE_2, 8, 1, 1 },
#endif
#if defined BCM6846
	{ "TUNNELS_PARSING_CFG", 1, CORE_2_INDEX, &TUNNELS_PARSING_CFG_2, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "IPTV_CFG_TABLE", 1, CORE_2_INDEX, &IPTV_CFG_TABLE_2, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "IPTV_DDR_CTX_TABLE_ADDRESS", 1, CORE_2_INDEX, &IPTV_DDR_CTX_TABLE_ADDRESS_2, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "NAT_CACHE_CFG", 1, CORE_2_INDEX, &NAT_CACHE_CFG_2, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "DHD_TX_POST_PD_FIFO_TABLE", 1, CORE_2_INDEX, &DHD_TX_POST_PD_FIFO_TABLE_2, 12, 1, 1 },
#endif
#if defined BCM6846
	{ "CPU_TX_INGRESS_UPDATE_FIFO_TABLE", 1, CORE_2_INDEX, &CPU_TX_INGRESS_UPDATE_FIFO_TABLE_2, 8, 1, 1 },
#endif
#if defined BCM6846
	{ "NAT_CACHE_KEY0_MASK", 1, CORE_2_INDEX, &NAT_CACHE_KEY0_MASK_2, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "NATC_L2_VLAN_KEY_MASK", 1, CORE_2_INDEX, &NATC_L2_VLAN_KEY_MASK_2, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "INGRESS_FILTER_CFG", 1, CORE_2_INDEX, &INGRESS_FILTER_CFG_2, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "NATC_L2_TOS_MASK", 1, CORE_2_INDEX, &NATC_L2_TOS_MASK_2, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "DHD_TX_POST_FLOW_RING_BUFFER", 1, CORE_2_INDEX, &DHD_TX_POST_FLOW_RING_BUFFER_2, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "DHD_TX_POST_UPDATE_FIFO_TABLE", 1, CORE_2_INDEX, &DHD_TX_POST_UPDATE_FIFO_TABLE_2, 8, 1, 1 },
#endif
#if defined BCM6846
	{ "DHD_MCAST_UPDATE_FIFO_TABLE", 1, CORE_2_INDEX, &DHD_MCAST_UPDATE_FIFO_TABLE_2, 8, 1, 1 },
#endif
#if defined BCM6846
	{ "CPU_TX_EGRESS_PD_FIFO_TABLE", 1, CORE_2_INDEX, &CPU_TX_EGRESS_PD_FIFO_TABLE_2, 2, 1, 1 },
#endif
#if defined BCM6846
	{ "WLAN_MCAST_SSID_MAC_ADDRESS_TABLE", 1, CORE_2_INDEX, &WLAN_MCAST_SSID_MAC_ADDRESS_TABLE_2, 16, 1, 1 },
#endif
#if defined BCM6846
	{ "CPU_TX_INGRESS_PD_FIFO_TABLE", 1, CORE_2_INDEX, &CPU_TX_INGRESS_PD_FIFO_TABLE_2, 2, 1, 1 },
#endif
#if defined BCM6846
	{ "DHD_MCAST_PD_FIFO_TABLE", 1, CORE_2_INDEX, &DHD_MCAST_PD_FIFO_TABLE_2, 2, 1, 1 },
#endif
#if defined BCM6846
	{ "VPORT_CFG_EX_TABLE", 1, CORE_2_INDEX, &VPORT_CFG_EX_TABLE_2, 16, 1, 1 },
#endif
#if defined BCM6846
	{ "DHD_DOORBELL_TX_POST_VALUE", 1, CORE_2_INDEX, &DHD_DOORBELL_TX_POST_VALUE_2, 16, 1, 1 },
#endif
#if defined BCM6846
	{ "DHD_BACKUP_INDEX_CACHE", 1, CORE_2_INDEX, &DHD_BACKUP_INDEX_CACHE_2, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "WLAN_MCAST_SSID_STATS_TABLE", 1, CORE_2_INDEX, &WLAN_MCAST_SSID_STATS_TABLE_2, 16, 1, 1 },
#endif
#if defined BCM6846
	{ "DHD_L2_HEADER", 1, CORE_2_INDEX, &DHD_L2_HEADER_2, 24, 1, 1 },
#endif
#if defined BCM6846
	{ "DHD_FLOW_RING_CACHE_LKP_TABLE", 1, CORE_2_INDEX, &DHD_FLOW_RING_CACHE_LKP_TABLE_2, 16, 1, 1 },
#endif
#if defined BCM6846
	{ "DHD_CPU_RECYCLE_RING_DESCRIPTOR_TABLE", 1, CORE_2_INDEX, &DHD_CPU_RECYCLE_RING_DESCRIPTOR_TABLE_2, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "CPU_RECYCLE_INTERRUPT_SCRATCH", 1, CORE_2_INDEX, &CPU_RECYCLE_INTERRUPT_SCRATCH_2, 2, 1, 1 },
#endif
#if defined BCM6846
	{ "CPU_RECYCLE_SHADOW_RD_IDX", 1, CORE_2_INDEX, &CPU_RECYCLE_SHADOW_RD_IDX_2, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "CPU_RECYCLE_SHADOW_WR_IDX", 1, CORE_2_INDEX, &CPU_RECYCLE_SHADOW_WR_IDX_2, 1, 1, 1 },
#endif
#if defined BCM6846
	{ "SCT_FILTER", 1, CORE_2_INDEX, &SCT_FILTER_2, 1, 1, 1 },
#endif
};
