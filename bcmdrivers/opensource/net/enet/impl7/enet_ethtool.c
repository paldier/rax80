/*
   <:copyright-BRCM:2017:DUAL/GPL:standard
   
      Copyright (c) 2017 Broadcom 
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
 */

#include <linux/netdevice.h>
#include <linux/ethtool.h>
#include "bcmnet.h"
#include "bcmenet_ethtool.h"
#include "enet.h"
#include "port.h"
#ifdef CONFIG_BCM_PTP_1588
extern int ptp_1588_get_ts_info(struct net_device *net, struct ethtool_ts_info *info);
#endif

static void enet_get_ethtool_stats(struct net_device *dev, struct ethtool_stats *stats, u64 *data)
{
    const struct rtnl_link_stats64 *ethStats;
    struct rtnl_link_stats64 temp;
    enetx_port_t *port = ((enetx_netdev *)netdev_priv(dev))->port;
    phy_dev_t *phy_dev;
    u64 speed = 0;

    if (!port || port->port_class != PORT_CLASS_PORT)
        return;

    ethStats = dev_get_stats(dev, &temp);
    
    data[ET_TX_BYTES] =     ethStats->tx_bytes;
    data[ET_TX_PACKETS] =   ethStats->tx_packets;
    data[ET_TX_ERRORS] =    ethStats->tx_errors;
    data[ET_RX_BYTES] =     ethStats->rx_bytes;
    data[ET_RX_PACKETS] =   ethStats->rx_packets;
    data[ET_RX_ERRORS] =    ethStats->rx_errors;

    /* Note: capacity is in bytes per second */
    phy_dev = port->p.phy;
    if (phy_dev && phy_dev->link)
    {
        switch (phy_dev->speed)
        {
        case PHY_SPEED_10:      speed =    10000000UL;  break;
        case PHY_SPEED_100:     speed =   100000000UL;  break;
        case PHY_SPEED_1000:    speed =  1000000000UL;  break;
        case PHY_SPEED_2500:    speed =  2500000000UL;  break;
        case PHY_SPEED_5000:    speed =  5000000000UL;  break;
        case PHY_SPEED_10000:   speed = 10000000000UL;  break;
        default:                speed = 0;
        }
    }
    data[ET_TX_CAPACITY] = speed / 8;
}

static int enet_ethtool_get_settings(struct net_device *dev, struct ethtool_cmd *ecmd)
{
    enetx_port_t *port = ((enetx_netdev *)netdev_priv(dev))->port;
    phy_dev_t *phy_dev;
    uint32_t caps = 0;
    uint32_t lp_caps = 0;
    uint32_t supported = 0;

    if (!port || port->port_class != PORT_CLASS_PORT)
        return -1;

    phy_dev = port->p.phy;
    if (!phy_dev)
    {
        enet_err("%s has no PHY connect\n", dev->name);
        return -1;
    }
    phy_dev = cascade_phy_get_last_active(phy_dev);

    if (cascade_phy_dev_caps_get(phy_dev, CAPS_TYPE_ADVERTISE, &caps) ||
        cascade_phy_dev_caps_get(phy_dev, CAPS_TYPE_LP_ADVERTISED,  &lp_caps) ||
        cascade_phy_dev_caps_get(phy_dev, CAPS_TYPE_SUPPORTED, &supported))
        return -1;

    ecmd->advertising = 0;
    if (caps & PHY_CAP_AUTONEG)
    {
        ecmd->autoneg = AUTONEG_ENABLE;
        ecmd->advertising |= ADVERTISED_Autoneg;
    }
    else
    {
        ecmd->autoneg = AUTONEG_DISABLE;
    }

    ecmd->phy_address = phy_dev->addr;

    if (caps & PHY_CAP_10000)       ecmd->advertising |= ADVERTISED_10000baseT_Full;
    if (caps & PHY_CAP_2500)        ecmd->advertising |= ADVERTISED_2500baseX_Full;
    if (caps & PHY_CAP_1000_FULL)   ecmd->advertising |= ADVERTISED_1000baseT_Full;
    if (caps & PHY_CAP_1000_HALF)   ecmd->advertising |= ADVERTISED_1000baseT_Half;
    if (caps & PHY_CAP_100_FULL)    ecmd->advertising |= ADVERTISED_100baseT_Full;
    if (caps & PHY_CAP_100_HALF)    ecmd->advertising |= ADVERTISED_100baseT_Half;
    if (caps & PHY_CAP_10_FULL)     ecmd->advertising |= ADVERTISED_10baseT_Full;
    if (caps & PHY_CAP_10_HALF)     ecmd->advertising |= ADVERTISED_10baseT_Half;
    if (caps & PHY_CAP_PAUSE)       ecmd->advertising |= ADVERTISED_Pause;
    if (caps & PHY_CAP_PAUSE_ASYM)  ecmd->advertising |= ADVERTISED_Asym_Pause;

    if (lp_caps & PHY_CAP_AUTONEG)     ecmd->lp_advertising |= ADVERTISED_Autoneg;
    if (lp_caps & PHY_CAP_10000)       ecmd->lp_advertising |= ADVERTISED_10000baseT_Full;
    if (lp_caps & PHY_CAP_2500)        ecmd->lp_advertising |= ADVERTISED_2500baseX_Full;
    if (lp_caps & PHY_CAP_1000_FULL)   ecmd->lp_advertising |= ADVERTISED_1000baseT_Full;
    if (lp_caps & PHY_CAP_1000_HALF)   ecmd->lp_advertising |= ADVERTISED_1000baseT_Half;
    if (lp_caps & PHY_CAP_100_FULL)    ecmd->lp_advertising |= ADVERTISED_100baseT_Full;
    if (lp_caps & PHY_CAP_100_HALF)    ecmd->lp_advertising |= ADVERTISED_100baseT_Half;
    if (lp_caps & PHY_CAP_10_FULL)     ecmd->lp_advertising |= ADVERTISED_10baseT_Full;
    if (lp_caps & PHY_CAP_10_HALF)     ecmd->lp_advertising |= ADVERTISED_10baseT_Half;
    if (lp_caps & PHY_CAP_PAUSE)       ecmd->lp_advertising |= ADVERTISED_Pause;
    if (lp_caps & PHY_CAP_PAUSE_ASYM)  ecmd->lp_advertising |= ADVERTISED_Asym_Pause;

    if (supported & PHY_CAP_10000)       ecmd->supported |= SUPPORTED_10000baseT_Full;
    if (supported & PHY_CAP_2500)        ecmd->supported |= SUPPORTED_2500baseX_Full;
    if (supported & PHY_CAP_1000_FULL)   ecmd->supported |= SUPPORTED_1000baseT_Full;
    if (supported & PHY_CAP_1000_HALF)   ecmd->supported |= SUPPORTED_1000baseT_Half;
    if (supported & PHY_CAP_100_FULL)    ecmd->supported |= SUPPORTED_100baseT_Full;
    if (supported & PHY_CAP_100_HALF)    ecmd->supported |= SUPPORTED_100baseT_Half;
    if (supported & PHY_CAP_10_FULL)     ecmd->supported |= SUPPORTED_10baseT_Full;
    if (supported & PHY_CAP_10_HALF)     ecmd->supported |= SUPPORTED_10baseT_Half;

    ecmd->supported |= SUPPORTED_Pause | SUPPORTED_Asym_Pause | SUPPORTED_Autoneg;
    // TODO: need to get supported link mode from phy
    ecmd->supported |= SUPPORTED_TP;
    if ((phy_dev->meta_id & MAC_IFACE) == MAC_IF_SERDES )
        ecmd->supported |= SUPPORTED_FIBRE;

    ecmd->duplex = 0xff;
    switch(phy_dev->speed)
    {
    case PHY_SPEED_10000:   ecmd->speed = SPEED_10000; break;
    case PHY_SPEED_2500:    ecmd->speed = SPEED_2500; break;
    case PHY_SPEED_1000:    ecmd->speed = SPEED_1000; break;
    case PHY_SPEED_100:     ecmd->speed = SPEED_100; break;
    case PHY_SPEED_10:      ecmd->speed = SPEED_10; break;
    case 0:
        break;
    default:
        WARN_ONCE(1, "Unknown ethernet speed (%d)\n", phy_dev->speed);
        return -1;
    }

    if(ecmd->duplex == 0xff) ecmd->duplex = (phy_dev->duplex == PHY_DUPLEX_FULL)? DUPLEX_FULL: DUPLEX_HALF;

  return 0;
}

static int enet_ethtool_set_settings(struct net_device *dev, struct ethtool_cmd *ecmd)
{
    enetx_port_t *port = ((enetx_netdev *)netdev_priv(dev))->port;
    phy_dev_t *phy_dev;
    phy_speed_t speed;

    if (!port || port->port_class != PORT_CLASS_PORT)
        return -1;

    phy_dev = port->p.phy;
    if (!phy_dev)
    {
        enet_err("%s has no PHY connect\n", dev->name);
        return -1;
    }

    if (ecmd->autoneg == AUTONEG_ENABLE)
    {
        speed = PHY_SPEED_AUTO;
    }
    else switch(ecmd->speed)
    {
        case SPEED_10000:   speed = PHY_SPEED_10000; break;
        case SPEED_2500:    speed = PHY_SPEED_2500; break;
        case SPEED_1000:    speed = PHY_SPEED_1000; break;
        case SPEED_100:     speed = PHY_SPEED_100; break;
        case SPEED_10:      speed = PHY_SPEED_10; break;
        default:
            WARN_ONCE(1, "Unknown Ethernet Speed Requested: (%d)Mbps\n", ecmd->speed);
            return -1;
    }

    return cascade_phy_dev_speed_set(phy_dev, speed,
        (ecmd->duplex == DUPLEX_FULL)? PHY_DUPLEX_FULL:PHY_DUPLEX_HALF);
}

#define BCM_ENET_DRV_VERSION "7.0"  // enet impl7

const char bcmenet_fullname[] = "Broadcom Ethernet Interface";
const char bcmenet_version[] = BCM_ENET_DRV_VERSION;

static void enet_ethtool_get_drvinfo(struct net_device *dev, struct ethtool_drvinfo *info)
{
    enetx_port_t *port = ((enetx_netdev *)netdev_priv(dev))->port;

    if (!port || port->port_class != PORT_CLASS_PORT)
        return;

    strcpy(info->driver, bcmenet_fullname);
    strcpy(info->version, bcmenet_version);
    strcpy(info->fw_version, "N/A");
    info->n_stats = ET_MAX;
    info->n_priv_flags = ET_PF_MAX;
}

static void enet_ethtool_get_pauseparam(struct net_device *dev, struct ethtool_pauseparam *info)
{
    enetx_port_t *port = ((enetx_netdev *)netdev_priv(dev))->port;
    int rx_enable, tx_enable;

    if (!port || port->port_class != PORT_CLASS_PORT)
        return;

    if (port_pause_get(port, &rx_enable, &tx_enable))
        return;
    info->rx_pause = rx_enable;
    info->tx_pause = tx_enable;
    info->autoneg  = 1;         // TODO: hardcoded to true, need to get from hw
}

static int enet_ethtool_set_pauseparam(struct net_device *dev, struct ethtool_pauseparam *info)
{
    enetx_port_t *port = ((enetx_netdev *)netdev_priv(dev))->port;

    if (!port || port->port_class != PORT_CLASS_PORT)
        return -1;

    return port_pause_set(port, info->rx_pause, info->tx_pause);
}

#if defined(CONFIG_BCM_ENERGY_EFFICIENT_ETHERNET)
static int enet_ethtool_get_eee(struct net_device *dev, struct ethtool_eee *info)
{
    enetx_port_t *port = ((enetx_netdev *)netdev_priv(dev))->port;
    phy_dev_t *phy_dev;
    int enabled;

    if (!port || port->port_class != PORT_CLASS_PORT)
        return -1;

    phy_dev = port->p.phy;
    if (!phy_dev)
    {
        enet_err("%s has no PHY connect\n", dev->name);
        return -1;
    }
    phy_dev = cascade_phy_get_last_active(phy_dev);

    phy_dev_eee_get(phy_dev, &enabled);
    info->eee_enabled = enabled;
    phy_dev_eee_resolution_get(phy_dev, &enabled);
    info->eee_active = enabled;

    info->supported = SUPPORTED_TP; // need to be non-zero to display above values;
    return 0;
}

static int enet_ethtool_set_eee(struct net_device *dev, struct ethtool_eee *info)
{
   enetx_port_t *port = ((enetx_netdev *)netdev_priv(dev))->port;
    phy_dev_t *phy_dev;

    if (!port || port->port_class != PORT_CLASS_PORT)
        return -1;

    phy_dev = port->p.phy;
    if (!phy_dev)
    {
        enet_err("%s has no PHY connect\n", dev->name);
        return -1;
    }

    return cascade_phy_dev_eee_set(phy_dev, info->eee_enabled ? 1: 0);
}
#endif

const struct ethtool_ops enet_ethtool_ops =
{
    .get_drvinfo  =         enet_ethtool_get_drvinfo,
    .get_settings =         enet_ethtool_get_settings,
    .set_settings =         enet_ethtool_set_settings,
    .get_ethtool_stats =    enet_get_ethtool_stats,
    .get_pauseparam =       enet_ethtool_get_pauseparam,
    .set_pauseparam =       enet_ethtool_set_pauseparam,
#if defined(CONFIG_BCM_ENERGY_EFFICIENT_ETHERNET)
    .get_eee =              enet_ethtool_get_eee,
    .set_eee =              enet_ethtool_set_eee,
#endif
#ifdef CONFIG_BCM_PTP_1588
    .get_ts_info = ptp_1588_get_ts_info,
#endif
};


