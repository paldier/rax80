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


#include "rdd_ag_flow_ctrl.h"

int rdd_ag_flow_ctrl_emac_flow_ctrl_vector_set(uint8_t bits)
{
    RDD_BYTE_1_BITS_WRITE_G(bits, RDD_EMAC_FLOW_CTRL_VECTOR_ADDRESS_ARR, 0);

    return BDMF_ERR_OK;
}

int rdd_ag_flow_ctrl_emac_flow_ctrl_vector_get(uint8_t *bits)
{
    RDD_BYTE_1_BITS_READ_G(*bits, RDD_EMAC_FLOW_CTRL_VECTOR_ADDRESS_ARR, 0);

    return BDMF_ERR_OK;
}

int rdd_ag_flow_ctrl_emac_flow_ctrl_budget_budget_set(uint32_t _entry, uint32_t budget)
{
    if(_entry >= RDD_EMAC_FLOW_CTRL_BUDGET_SIZE)
         return BDMF_ERR_PARM;

    RDD_EMAC_FLOW_CTRL_BUDGET_ENTRY_BUDGET_WRITE_G(budget, RDD_EMAC_FLOW_CTRL_BUDGET_ADDRESS_ARR, _entry);

    return BDMF_ERR_OK;
}

int rdd_ag_flow_ctrl_emac_flow_ctrl_budget_budget_get(uint32_t _entry, uint32_t *budget)
{
    if(_entry >= RDD_EMAC_FLOW_CTRL_BUDGET_SIZE)
         return BDMF_ERR_PARM;

    RDD_EMAC_FLOW_CTRL_BUDGET_ENTRY_BUDGET_READ_G(*budget, RDD_EMAC_FLOW_CTRL_BUDGET_ADDRESS_ARR, _entry);

    return BDMF_ERR_OK;
}

