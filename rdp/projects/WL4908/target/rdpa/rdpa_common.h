/*
* <:copyright-BRCM:2013:DUAL/GPL:standard
* 
*    Copyright (c) 2013 Broadcom 
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


#ifndef _RDPA_COMMON_H_
#define _RDPA_COMMON_H_

#include "rdpa_types.h"
#include "bdmf_dev.h"
#include "bdmf_errno.h"
#include "rdpa_port.h"

bdmf_error_t rdpa_obj_get(struct bdmf_object **rdpa_objs, int max_rdpa_objs_num, int index, struct bdmf_object **mo);
int rdpa_dir_index_get_next(rdpa_dir_index_t *dir_index, bdmf_index max_index);
void replace_ownership(bdmf_object_handle current_mo, bdmf_object_handle new_mo, bdmf_object_handle owner);

#if defined(XRDP)
void rdpa_common_update_cntr_results_uint32(void *stat_buf, void *accumulative_stat_buf, uint32_t stat_offset_in_bytes, uint32_t cntr_result);

typedef enum
{
    rdpa_stat_pckts_id,
    rdpa_stat_bytes_id
} rdpa_stat_elements_ids_t;

static inline uint32_t _get_rdpa_stat_offset(rdpa_stat_elements_ids_t id)
{
    uint32_t offset_in_bytes[rdpa_stat_bytes_id + 1] =
    {
        offsetof(rdpa_stat_t, packets),
        offsetof(rdpa_stat_t, bytes)
    };

    return offset_in_bytes[id];
}
#endif

typedef struct queue_id_info
{
    int rc_id;                                        /* rdd rate controller index */
    int queue;                                        /* physical queue in  RDD  */
    int channel;                                      /* physical channel in RDD   */
} queue_info_t;

typedef enum
{
    rdpa_reserved_option_0,        /* bit 0 is reserved */
    rdpa_ecn_ipv6_remarking_option /* bit 1 set ECN IPV6 option*/
} rdpa_system_options_types;

#ifdef BDMF_DRIVER
/*
 * Enum tables for framework CLI access
 */
extern const bdmf_attr_enum_table_t rdpa_if_enum_table;
extern const bdmf_attr_enum_table_t rdpa_lan_wan_if_enum_table;
extern const bdmf_attr_enum_table_t rdpa_lan_wan_wlan_if_enum_table;
extern const bdmf_attr_enum_table_t rdpa_lan_or_cpu_if_enum_table;
extern const bdmf_attr_enum_table_t rdpa_wlan_ssid_enum_table;
extern const bdmf_attr_enum_table_t rdpa_emac_enum_table;
extern const bdmf_attr_enum_table_t rdpa_wan_emac_enum_table;
extern const bdmf_attr_enum_table_t rdpa_wan_type_enum_table;
extern const bdmf_attr_enum_table_t rdpa_forward_action_enum_table;
extern const bdmf_attr_enum_table_t rdpa_filter_action_enum_table;
extern const bdmf_attr_enum_table_t rdpa_traffic_dir_enum_table;
extern const bdmf_attr_enum_table_t rdpa_port_frame_allow_enum_table;
extern const bdmf_attr_enum_table_t rdpa_qos_method_enum_table;
extern const bdmf_attr_enum_table_t rdpa_ip_version_enum_table;
extern const bdmf_attr_enum_table_t rdpa_forward_mode_enum_table;
extern const bdmf_attr_enum_table_t rdpa_classify_mode_enum_table;
extern const bdmf_attr_enum_table_t rdpa_disc_prty_enum_table;
extern const bdmf_attr_enum_table_t rdpa_flow_dest_enum_table;
extern const bdmf_attr_enum_table_t rdpa_ip_class_method_enum_table;
extern const bdmf_attr_enum_table_t rdpa_cpu_reason_enum_table;
extern const bdmf_attr_enum_table_t rdpa_epon_mode_enum_table;
extern const bdmf_attr_enum_table_t rdpa_ic_act_vect_enum_table;
extern const bdmf_attr_enum_table_t rdpa_ic_dei_command_enum_table;
extern const bdmf_attr_enum_table_t rdpa_bpm_buffer_size_enum_table;
extern const bdmf_attr_enum_table_t rdpa_ic_trap_reason_enum_table;
extern const bdmf_attr_enum_table_t rdpa_filter_enum_table;
extern const bdmf_attr_enum_table_t rdpa_speed_type_enum_table;
extern const bdmf_attr_enum_table_t rdpa_protocol_filters_table;
extern const bdmf_attr_enum_table_t rdpa_tc_enum_table;
extern const bdmf_attr_enum_table_t rdpa_l2_flow_key_exclude_enum_table;
#endif /* #ifdef BDMF_DRIVER */

#endif
