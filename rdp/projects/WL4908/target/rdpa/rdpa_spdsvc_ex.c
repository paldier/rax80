/*
* <:copyright-BRCM:2014:DUAL/GPL:standard
* 
*    Copyright (c) 2014 Broadcom 
*    All Rights Reserved
* 
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License, version 2, as published by
* the Free Software Foundation (the "GPL").
* 
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
* 
* 
* A copy of the GPL is available at http://www.broadcom.com/licenses/GPLv2.php, or by
* writing to the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
* Boston, MA 02111-1307, USA.
* 
* :>
*/

/*
 * rdpa_spdsvc_ex.c
 * RDPA Speed Service RDP interface
 */
#include <bdmf_dev.h>
#include <rdpa_api.h>
#include "rdpa_int.h"
#include <rdd.h>
#include <rdd_data_structures.h>
#include "rdpa_spdsvc_ex.h"
#include "rdpa_rdd_inline.h"

bdmf_error_t rdpa_rdd_spdsvc_gen_config(const rdpa_spdsvc_generator_t *generator_p)
{
    return rdd_spdsvc_config(generator_p->kbps, generator_p->mbs,
                            generator_p->copies, generator_p->total_length);
}

/* Get generator counters and running status. Analyzer counters remain unchanged */
bdmf_error_t rdpa_rdd_spdsvc_gen_get_result(uint8_t *running_p,
                                            uint32_t *tx_packets_p,
                                            uint32_t *tx_discards_p)
{
    return rdd_spdsvc_get_tx_result(running_p,
                                    tx_packets_p,
                                    tx_discards_p);
}

bdmf_error_t rdpa_rdd_spdsvc_gen_terminate(void)
{
#if defined(OREN)
    return BDMF_ERR_STATE;
#else
    return rdd_spdsvc_terminate();
#endif
}

bdmf_error_t rdpa_rdd_spdsvc_analyzer_config(void)
{
#if defined(DSL_63138) || defined(DSL_63148) || defined(WL4908)

    rdd_spdsvc_reset_rx_time();

#else

#if defined(OREN)
    RDD_SPEED_SERVICE_RX_TIMESTAMPS_DTS *spdsvc_rx_timestamps_ptr;

    spdsvc_rx_timestamps_ptr = (RDD_SPEED_SERVICE_RX_TIMESTAMPS_DTS *)
                (DEVICE_ADDRESS(RUNNER_COMMON_1_OFFSET) + SPEED_SERVICE_RX_TIMESTAMPS_TABLE_ADDRESS);

    RDD_SPEED_SERVICE_RX_TIMESTAMPS_START_TS_WRITE(0, spdsvc_rx_timestamps_ptr);
    RDD_SPEED_SERVICE_RX_TIMESTAMPS_LAST_TS_WRITE(0, spdsvc_rx_timestamps_ptr);
#endif

#endif
    return BDMF_ERR_OK;
}

bdmf_error_t rdpa_rdd_spdsvc_analyzer_get_rx_time(uint32_t *rx_time_us)
{
#if defined(DSL_63138) || defined(DSL_63148) || defined(WL4908)

    rdd_spdsvc_get_rx_time(rx_time_us);

#else

#if defined(OREN)
    uint32_t timestamp_start, timestamp_last;
    RDD_SPEED_SERVICE_RX_TIMESTAMPS_DTS *spdsvc_rx_timestamps_ptr;

    spdsvc_rx_timestamps_ptr = (RDD_SPEED_SERVICE_RX_TIMESTAMPS_DTS *)
        (DEVICE_ADDRESS(RUNNER_COMMON_1_OFFSET) + SPEED_SERVICE_RX_TIMESTAMPS_TABLE_ADDRESS);

    RDD_SPEED_SERVICE_RX_TIMESTAMPS_START_TS_READ(timestamp_start, spdsvc_rx_timestamps_ptr);
    RDD_SPEED_SERVICE_RX_TIMESTAMPS_LAST_TS_READ(timestamp_last, spdsvc_rx_timestamps_ptr);
    *rx_time_us = timestamp_last - timestamp_start;
#else
    *rx_time_us = 0;
#endif

#endif
    return BDMF_ERR_OK;
}


