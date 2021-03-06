// <:copyright-BRCM:2013:DUAL/GPL:standard
// 
//    Copyright (c) 2013 Broadcom 
//    All Rights Reserved
// 
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License, version 2, as published by
// the Free Software Foundation (the "GPL").
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// 
// A copy of the GPL is available at http://www.broadcom.com/licenses/GPLv2.php, or by
// writing to the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
// Boston, MA 02111-1307, USA.
// 
// :>
/*
 * vlan_action object header file.
 * This header file is generated automatically. Do not edit!
 */
#ifndef _RDPA_AG_VLAN_ACTION_H_
#define _RDPA_AG_VLAN_ACTION_H_

/** \addtogroup vlan_action
 * @{
 */


/** Get vlan_action type handle.
 *
 * This handle should be passed to bdmf_new_and_set() function in
 * order to create a vlan_action object.
 * \return vlan_action type handle
 */
bdmf_type_handle rdpa_vlan_action_drv(void);

/* vlan_action: Attribute types */
typedef enum {
    rdpa_vlan_action_attr_dir = 0, /* dir : MKRI : enum : Traffic Direction */
    rdpa_vlan_action_attr_index = 1, /* index : KRI : number : VLAN Action Index */
    rdpa_vlan_action_attr_action = 2, /* action : RW : aggregate vlan_action(rdpa_vlan_action_cfg_t) : Action and parameters */
} rdpa_vlan_action_attr_types;

/** vlan_action object key. */
typedef struct {
    rdpa_traffic_dir dir; /**< vlan_action: Traffic Direction */
    bdmf_number index; /**< vlan_action: VLAN Action Index */
} rdpa_vlan_action_key_t;


extern int (*f_rdpa_vlan_action_get)(const rdpa_vlan_action_key_t * key_, bdmf_object_handle *pmo);

/** Get vlan_action object by key.

 * This function returns vlan_action object instance by key.
 * \param[in] key_    Object key
 * \param[out] vlan_action_obj    Object handle
 * \return    0=OK or error <0
 */
int rdpa_vlan_action_get(const rdpa_vlan_action_key_t * key_, bdmf_object_handle *vlan_action_obj);

/** Get vlan_action/dir attribute.
 *
 * Get Traffic Direction.
 * \param[in]   mo_ vlan_action object handle or mattr transaction handle
 * \param[out]  dir_ Attribute value
 * \return 0 or error code < 0
 * The function can be called in task and softirq contexts.
 */
static inline int rdpa_vlan_action_dir_get(bdmf_object_handle mo_, rdpa_traffic_dir *dir_)
{
    bdmf_number _nn_;
    int _rc_;
    _rc_ = bdmf_attr_get_as_num(mo_, rdpa_vlan_action_attr_dir, &_nn_);
    *dir_ = (rdpa_traffic_dir)_nn_;
    return _rc_;
}


/** Set vlan_action/dir attribute.
 *
 * Set Traffic Direction.
 * \param[in]   mo_ vlan_action object handle or mattr transaction handle
 * \param[in]   dir_ Attribute value
 * \return 0 or error code < 0
 * The function can be called in task and softirq contexts.
 */
static inline int rdpa_vlan_action_dir_set(bdmf_object_handle mo_, rdpa_traffic_dir dir_)
{
    return bdmf_attr_set_as_num(mo_, rdpa_vlan_action_attr_dir, dir_);
}


/** Get vlan_action/index attribute.
 *
 * Get VLAN Action Index.
 * \param[in]   mo_ vlan_action object handle or mattr transaction handle
 * \param[out]  index_ Attribute value
 * \return 0 or error code < 0
 * The function can be called in task and softirq contexts.
 */
static inline int rdpa_vlan_action_index_get(bdmf_object_handle mo_, bdmf_number *index_)
{
    bdmf_number _nn_;
    int _rc_;
    _rc_ = bdmf_attr_get_as_num(mo_, rdpa_vlan_action_attr_index, &_nn_);
    *index_ = (bdmf_number)_nn_;
    return _rc_;
}


/** Set vlan_action/index attribute.
 *
 * Set VLAN Action Index.
 * \param[in]   mo_ vlan_action object handle or mattr transaction handle
 * \param[in]   index_ Attribute value
 * \return 0 or error code < 0
 * The function can be called in task and softirq contexts.
 */
static inline int rdpa_vlan_action_index_set(bdmf_object_handle mo_, bdmf_number index_)
{
    return bdmf_attr_set_as_num(mo_, rdpa_vlan_action_attr_index, index_);
}


/** Get vlan_action/action attribute.
 *
 * Get Action and parameters.
 * \param[in]   mo_ vlan_action object handle or mattr transaction handle
 * \param[out]  action_ Attribute value
 * \return 0 or error code < 0
 * The function can be called in task context only.
 */
static inline int rdpa_vlan_action_action_get(bdmf_object_handle mo_, rdpa_vlan_action_cfg_t * action_)
{
    return bdmf_attr_get_as_buf(mo_, rdpa_vlan_action_attr_action, action_, sizeof(*action_));
}


/** Set vlan_action/action attribute.
 *
 * Set Action and parameters.
 * \param[in]   mo_ vlan_action object handle or mattr transaction handle
 * \param[in]   action_ Attribute value
 * \return 0 or error code < 0
 * The function can be called in task context only.
 */
static inline int rdpa_vlan_action_action_set(bdmf_object_handle mo_, const rdpa_vlan_action_cfg_t * action_)
{
    return bdmf_attr_set_as_buf(mo_, rdpa_vlan_action_attr_action, action_, sizeof(*action_));
}

/** @} end of vlan_action Doxygen group */




#endif /* _RDPA_AG_VLAN_ACTION_H_ */
