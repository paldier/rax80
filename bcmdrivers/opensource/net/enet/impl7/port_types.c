/*
   <:copyright-BRCM:2015:DUAL/GPL:standard
   
      Copyright (c) 2015 Broadcom 
      All Rights Reserved
   
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

/*
 *  Created on: Nov/2015
 *      Author: ido@broadcom.com
 */

#include "port.h"

#ifdef RUNNER
#include "runner.h"
#endif
#ifdef SF2_DEVICE
#include "sf2.h"
#endif
#ifdef BRCM_FTTDP
#include "g9991.h"
#endif
#include "runner_wifi.h"

#ifdef VLANTAG
extern sw_ops_t port_vlan_sw;
extern port_ops_t port_vlan_port;
#endif

#ifdef ENET_DMA
extern sw_ops_t port_dummy_sw;
extern port_ops_t port_dma_port;
#endif

static int dbg_port_count, dbg_sw_count, dbg_port_detect_count;

int _assign_port_class(enetx_port_t *port, port_type_t port_type)
{
    sw_ops_t *sw_ops;
    port_ops_t *port_ops;
    port_class_t port_class;

    switch (port_type)
    {
#ifdef RUNNER
        case PORT_TYPE_RUNNER_SW:
            sw_ops = &port_runner_sw;
            port_class = PORT_CLASS_SW;
            break;
        case PORT_TYPE_RUNNER_PORT:
            port_ops = &port_runner_port;
            port_class = PORT_CLASS_PORT;
            break;
        case PORT_TYPE_RUNNER_DETECT:
            port_class = PORT_CLASS_PORT_DETECT;
            break;
#endif
#ifdef SF2_DEVICE
        case PORT_TYPE_SF2_SW:
            sw_ops = &port_sf2_sw;
            port_class = PORT_CLASS_SW;
            break;
        case PORT_TYPE_SF2_PORT:
            port_ops = &port_sf2_port;
            port_class = PORT_CLASS_PORT;
            break;
#endif
#ifdef ENET_RUNNER_WIFI
        case PORT_TYPE_RUNNER_WIFI:
            port_ops = &port_runner_wifi;
            port_class = PORT_CLASS_PORT;
            break;
#endif
#ifdef EPON
        case PORT_TYPE_RUNNER_EPON:
            port_ops = &port_runner_epon;
            port_class = PORT_CLASS_PORT;
            break;
#endif
#ifdef GPON
        case PORT_TYPE_RUNNER_GPON:
            port_ops = &port_runner_gpon;
            port_class = PORT_CLASS_PORT;
            break;
#endif
#ifdef BRCM_FTTDP
        case PORT_TYPE_G9991_SW:
            sw_ops = &port_g9991_sw;
            port_class = PORT_CLASS_SW;
            break;
        case PORT_TYPE_G9991_PORT:
            port_ops = &port_g9991_port;
            port_class = PORT_CLASS_PORT;
            break;
        case PORT_TYPE_G9991_ES_PORT:
            port_ops = &port_g9991_es_port;
            port_class = PORT_CLASS_PORT;
            break;
#endif
#ifdef VLANTAG
        case PORT_TYPE_VLAN_SW:
            sw_ops = &port_vlan_sw;
            port_class = PORT_CLASS_SW;
            break;
        case PORT_TYPE_VLAN_PORT:
            port_ops = &port_vlan_port;
            port_class = PORT_CLASS_PORT;
            break;
#endif
#ifdef ENET_DMA
        case PORT_TYPE_DIRECT_RGMII:
            sw_ops = &port_dummy_sw;
            port_class = PORT_CLASS_SW;
            break;
        case PORT_TYPE_GENERIC_DMA:
            port_ops = &port_dma_port;
            port_class = PORT_CLASS_PORT;
            break;
#endif
        default:
            enet_err("failed to create port type %d\n", port_type);
            return -1;
    }

    port->port_class = port_class;
    port->port_type = port_type;

    /* object name for easy debugging */
    if (port->port_class == PORT_CLASS_SW)
    {
        port->s.ops = sw_ops;
        snprintf(port->obj_name, IFNAMSIZ, "sw%d", dbg_sw_count++);
    }
    else if (port->port_class == PORT_CLASS_PORT)
    {
        port->p.ops = port_ops;
        snprintf(port->obj_name, IFNAMSIZ, "port%d", dbg_port_count++);
    }
    else if (port->port_class == PORT_CLASS_PORT_DETECT)
    {
        snprintf(port->obj_name, IFNAMSIZ, "portdetect%d", dbg_port_detect_count++);
    }

    enet_dbg("set %s type %d\n", port->obj_name, port_type);
            
    return 0;
}

