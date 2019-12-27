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

#include <linux/module.h>
#include <linux/etherdevice.h>
#include <linux/bcm_assert_locks.h>
#include <linux/workqueue.h>
#include "board.h"
#include "mac_drv.h"
#include "enet.h"
#include "port.h"
#include "bp_parsing.h"
#include "crossbar_dev.h"
#include "bcmenet_proc.h" 
#include "enet_dbg.h"
#ifdef RUNNER
#include "rdpa_api.h"
#endif
#ifdef CONFIG_BCM_PTP_1588
#include "ptp_1588.h"
#endif
#include <linux/kthread.h>
#include <linux/bcm_skb_defines.h>
#include <linux/rtnetlink.h>
#ifdef DT
#include <linux/of_address.h>
#include <linux/of_irq.h>
#include <linux/of_platform.h>
#endif
#ifdef PKTC
#include <osl.h>
#endif
#include <linux/if_vlan.h>      // for struct vlan_hdr
#include <linux/if_pppox.h>     // for struct pppoe_hdr
#include <linux/ppp_defs.h>     // for PPP_IPV6
#include <bcmnet.h>             // for check_arp_lcp_pkt()
#include "bcm/bcmswapitypes.h"  // for MAX_PRIORITY_VALUE

#ifdef SUPPORT_ETHTOOL
#include "bcmenet_ethtool.h"
extern const struct ethtool_ops enet_ethtool_ops;
#endif 

#ifdef CONFIG_BLOG
extern int bcm_tcp_v4_recv(pNBuff_t pNBuff, BlogFcArgs_t *fc_args);
#endif
#ifdef PHY_PON
extern phy_drv_t phy_drv_pon;
#endif

/* Flag indicates we're in Dying Gasp and powering down - don't clear once set */
static int dg_in_context=0;

fwdcb_t enet_fwdcb = NULL;

int enetx_weight_budget = 0;
static enetx_channel *enetx_channels;
/* Number of Linux interfaces currently opened */
static int open_count;

#if defined(CC_DROP_PRECEDENCE)
/* Drop precedence look up callback function. */
typedef unsigned int (*BCMENET_DP_LOOKUP_CB)(struct net_device *dev,
  unsigned char *data, unsigned int len);

static BCMENET_DP_LOOKUP_CB enet_dp_lookup_cb = NULL;

void bcmenet_register_dp_cb(BCMENET_DP_LOOKUP_CB dpcb) 
{
    enet_dp_lookup_cb = dpcb;
}

EXPORT_SYMBOL(bcmenet_register_dp_cb);

#endif /* CC_DROP_PRECEDENCE */

static void enetx_work_cb(struct work_struct *work)
{
    enetx_work_t *enetx_work = container_of(work, enetx_work_t, base_work);
    enetx_port_t *port = enetx_work->port;
    enetx_work_func_t func = enetx_work->func;

    func(port); 
    kfree(enetx_work);
}

int enetx_queue_work(enetx_port_t *port, enetx_work_func_t func)
{
    enetx_work_t *enetx_work = kmalloc(sizeof(enetx_work_t), GFP_ATOMIC);
    if (!enetx_work)
    {
        printk("enetx_queue_work: kmalloc failed to allocate work struct\n");
        return -1;
    }

    INIT_WORK(&enetx_work->base_work, enetx_work_cb);
    enetx_work->port = port;
    enetx_work->func = func;

    queue_work(system_unbound_wq, &enetx_work->base_work);

    return 0;
}

void set_mac_cfg_by_phy(enetx_port_t *p)
{
    mac_dev_t *mac_dev = p->p.mac;
    phy_dev_t *phy_dev = p->p.phy;
    mac_cfg_t mac_cfg = {};

    mac_dev_disable(mac_dev);
    mac_dev_cfg_get(mac_dev, &mac_cfg);

    if (phy_dev->speed == PHY_SPEED_10)
        mac_cfg.speed = MAC_SPEED_10;
    else if (phy_dev->speed == PHY_SPEED_100)
        mac_cfg.speed = MAC_SPEED_100;
    else if (phy_dev->speed == PHY_SPEED_1000)
        mac_cfg.speed = MAC_SPEED_1000;
    else if (phy_dev->speed == PHY_SPEED_2500)
        mac_cfg.speed = MAC_SPEED_2500;
    else if (phy_dev->speed == PHY_SPEED_5000)
        mac_cfg.speed = MAC_SPEED_5000;
    else if (phy_dev->speed == PHY_SPEED_10000)
        mac_cfg.speed = MAC_SPEED_10000;
    else
        mac_cfg.speed = MAC_SPEED_UNKNOWN;

    mac_cfg.duplex = phy_dev->duplex == PHY_DUPLEX_FULL ? MAC_DUPLEX_FULL : MAC_DUPLEX_HALF;

    mac_dev_cfg_set(mac_dev, &mac_cfg);
    if (phy_dev->link)
    {
        mac_dev_pause_set(mac_dev, phy_dev->pause_rx, phy_dev->pause_tx, p->dev ? p->dev->dev_addr : NULL);
        mac_dev_enable(mac_dev);
    }
}

void set_mac_eee_by_phy(enetx_port_t *p)
{
    mac_dev_t *mac_dev = p->p.mac;
    phy_dev_t *phy_dev = p->p.phy;
    int enabled = 0;

    if (phy_dev->link)
    {
        msleep(1000);
        phy_dev_eee_resolution_get(phy_dev, &enabled);
    }

    mac_dev_eee_set(mac_dev, enabled);
}

void phy_link_change_cb(void *ctx) /* ctx is a PORT_CLASS_PORT enetx_port_t */ 
{
    enetx_port_t *p = ctx;

    p->p.phy_last_change = (jiffies * 100) / HZ;

    if (p->p.mac)
    {
        /* Update mac cfg according to phy */
        set_mac_cfg_by_phy(p);

        /* Update mac eee according to phy */
        enetx_queue_work(p, set_mac_eee_by_phy);
    }

    if (p->dev)
    {
        /* Print new status to console */
        port_print_status(p);

        if (p->p.phy->link)
        {
            if(!netif_carrier_ok(p->dev))
            {
                netif_carrier_on(p->dev);
            }
        }
        else
        {
            if(netif_carrier_ok(p->dev))
            {
                netif_carrier_off(p->dev);
            }
        }
    }

    port_link_change(p, p->p.phy->link);
}

static inline void dumpHexData1(uint8_t *pHead, uint32_t len)
{
    uint32_t i;
    uint8_t *c = pHead;
    for (i = 0; i < len; ++i) {
        if (i % 16 == 0)
            printk("\n");
        printk("0x%02X, ", *c++);
    }
    printk("\n");
}


/* Called from platform ISR implementation */
inline int enetx_rx_isr(enetx_channel *chan)
{
    int i;

    enet_dbg_rx("rx_isr/priv %p\n", chan);

    for (i = 0; i < chan->rx_q_count; i++)
        enetxapi_queue_int_disable(chan, i);

    chan->rxq_cond = 1;
    wake_up_interruptible(&chan->rxq_wqh);

    return 0;
}

extern struct sk_buff *skb_header_alloc(void);
static inline int rx_skb(FkBuff_t *fkb, enetx_port_t *port, enetx_rx_info_t *rx_info)
{
    struct net_device *dev = port->dev;
    struct sk_buff *skb;

    /* TODO: allocate from pool */
    skb = skb_header_alloc();
    if (unlikely(!skb))
    {
        enet_err("SKB allocation failure\n");
        INC_STAT_RX_DROP(port,rx_dropped_no_skb);
        return -1;
    }
    skb_headerinit((BCM_PKT_HEADROOM + rx_info->data_offset), 
#if defined(CC_NBUFF_FLUSH_OPTIMIZATION)
            SKB_DATA_ALIGN(fkb->len + BCM_SKB_TAILROOM + rx_info->data_offset),
#else
            BCM_MAX_PKT_LEN - rx_info->data_offset,
#endif
            skb, (uint8_t *)fkb->data, (RecycleFuncP)enetxapi_buf_recycle,(unsigned long) port->priv, fkb->blog_p);

    ETH_GBPM_TRACK_SKB( skb, GBPM_VAL_ALLOC, 0);
    ETH_GBPM_TRACK_SKB( skb, GBPM_VAL_RX, 0);

    skb_trim(skb,fkb->len);

    skb->recycle_flags &= SKB_NO_RECYCLE; /* no skb recycle,just do data recyle */
    skb->recycle_flags |= rx_info->extra_skb_flags;

    skb->priority = fkb->priority;
    skb->dev = dev;

    if (port->p.ops->rx_pkt_mod)
        port->p.ops->rx_pkt_mod(port, skb);
    
    skb->protocol = eth_type_trans(skb, dev);
#if defined(CONFIG_BCM_CSO)
    skb->ip_summed = fkb->rx_csum_verified; /* XXX: Make sure rx_csum_verified is 1/CHECKSUM_UNNECESSARY and not something else */
#endif

    if (port->n.set_channel_in_mark)
        skb->mark = SKBMARK_SET_PORT(skb->mark, rx_info->flow_id);

#if defined(CONFIG_BCM_PTP_1588) && defined(CONFIG_BCM_PON_XRDP)
    /* If the packet is 1588, ts32 should be extracted from the headroom */
    if (unlikely(rx_info->ptp_index == 1))
        ptp_1588_cpu_rx(skb, ntohl((uint32_t)*((uint32_t*)(fkb->data - PTP_TS_LEN))));
#endif

    ETH_GBPM_TRACK_SKB( skb, GBPM_VAL_EXIT, 0 );

    INC_STAT_DBG(port,rx_packets_netif_receive_skb);
    local_bh_disable();
    netif_receive_skb(skb);
    local_bh_enable();

    dev->last_rx = jiffies;

    return 0;
}

static inline void _free_fkb(FkBuff_t *fkb)
{
    fkb_flush(fkb, fkb->data, fkb->len, FKB_CACHE_FLUSH);
    enetxapi_fkb_databuf_recycle(fkb, (void *)(fkb->recycle_context));
}

/* Read up to budget packets from queue.
 * Return number of packets received on queue */
static inline int rx_pkt_from_q(int hw_q_id, int budget)
{
    int rc, count = 0;
    enetx_port_t *port;
    FkBuff_t *fkb;
    struct net_device *dev;
    enetx_rx_info_t rx_info;
#if defined(CONFIG_BLOG)
    BlogAction_t blog_action;
    BlogFcArgs_t fc_args;
#endif

    do
    {
        /* TODO: bulk processing */
        rc = enetxapi_rx_pkt(hw_q_id, &fkb, &rx_info);
        if (unlikely(rc))
            continue;

#ifdef ENET_DEBUG_RX_CPU_TRAFFIC
        if (unlikely(g_debug_mode))
        {
            count++;
            enetxapi_fkb_databuf_recycle(PDATA_TO_PFKBUFF(fkb, BCM_PKT_HEADROOM), NULL);
            continue;
        }               
#endif
        rcu_read_lock();
        root_sw->s.ops->mux_port_rx(root_sw, &rx_info, fkb, &port);
        if (unlikely(!port))
        {
            if (printk_ratelimit())
            {
                enet_err("failed to demux src_port %d on root: no such port\n", rx_info.src_port);
                /* enet_err("data<0x%08x> len<%u>", (int)fkb->data, fkb->len); */
                dumpHexData1(fkb->data, fkb->len);
            }
            _free_fkb(fkb);
            INC_STAT_RX_DROP(root_sw,rx_dropped_no_srcport);
            goto unlock;
        }

        dev = port->dev;
        if (unlikely(!dev))
        {
            if (printk_ratelimit())
                enet_err("no Linux interface attached to port %s\n", port->name);

            _free_fkb(fkb);
            INC_STAT_RX_DROP(port,rx_dropped_no_rxdev);
            goto unlock;
        }

        INC_STAT_RX_PKT_BYTES(port,fkb->len);
        INC_STAT_RX_Q(port,hw_q_id);

#if defined(CONFIG_BLOG)
        blog_action = blog_finit_args(fkb, dev, TYPE_ETH, port->n.set_channel_in_mark ? rx_info.flow_id :
            port->n.blog_chnl_rx, port->n.blog_phy, &fc_args);
        if (unlikely(blog_action == PKT_DROP))
        {
            enet_err("blog_finit return PKT_DROP %s\n", port->name);
            _free_fkb(fkb);
            INC_STAT_RX_DROP(port,rx_dropped_blog_drop);
            goto unlock;
        }

        /* packet consumed, proceed to next packet*/
        if (likely(blog_action == PKT_DONE))
        {
            INC_STAT_DBG(port,rx_packets_blog_done);
            count++;
            goto unlock;
        }

#ifndef CONFIG_BCM_PON_XRDP
        /* In RDP, wlan exception packets are trapped to enet driver */
        if (port->n.blog_phy == BLOG_WLANPHY && blog_action != PKT_NORM)
            fkb->blog_p->wl_hw_support.is_rx_hw_acc_en = 1;
#endif

        if (blog_action == PKT_TCP4_LOCAL)
        {
            INC_STAT_DBG(port,rx_packets_blog_done);
            bcm_tcp_v4_recv((void*)CAST_REAL_TO_VIRT_PNBUFF(fkb,FKBUFF_PTR), &fc_args);
            count++;
            goto unlock;
        }
#endif

        ///enet_dbgv("%s/%s/q%d len:%d\n", port->obj_name, dev->name, hw_q_id, fkb->len); /// one line rx info

        rc = rx_skb(fkb, port, &rx_info);
        count++;
unlock:
        rcu_read_unlock();
    }
    while (count < budget && likely(!rc));

    enet_dbg_rx("read from hw_rx_q %d count %d\n", hw_q_id, count);

    return count;
}

static inline int rx_pkt(enetx_channel *chan, int budget)
{
    int i, rc , count;

    /* Receive up to budget packets while Iterating over queues in channel by priority */
    for (count = 0, i = 0; i < chan->rx_q_count && count < budget; i++)
    {
        rc = rx_pkt_from_q(chan->rx_q[i], budget - count);
        count += rc;

#ifdef ENET_DEBUG_RX_CPU_TRAFFIC
        if(g_debug_mode)
        {
#ifdef CONFIG_ARM64
            g_debug_mode_pckt_rx += rc;
#else
            if(g_debug_mode_pckt_rx + rc > 0x100000000 -1) //2^32 - 1 = 0x100000000
                g_debug_mode_pckt_rx = 0x100000000 -1;
            else
                g_debug_mode_pckt_rx += rc;
#endif
        }
#endif
        /*do not continue process an empty queue*/
        if(rc == 0)
            continue;
    }

    return count;
}

int chan_thread_handler(void *data)
{
    int work = 0;
    int reschedule;
    int i;
    enetx_channel *chan = (enetx_channel *) data;

    while (1)
    {
        wait_event_interruptible(chan->rxq_wqh, chan->rxq_cond | kthread_should_stop());

        /*read budget from all queues of the channel*/
        work += rx_pkt(chan, enetx_weight_budget);
        reschedule = 0;

        /*if budget was not consumed then check if one of the
         * queues is full so thread will be reschedule - NAPI */
        if (work < enetx_weight_budget)
        {
            for (i = 0; i < chan->rx_q_count; i++)
            {
                if (enetxapi_queue_need_reschedule(chan, i))
                {
                    reschedule = 1;
                    break;
                }
            }
            /*enable interrupts again*/
            if (!reschedule)
            {
                work = 0;
                chan->rxq_cond = 0;
                for (i = 0; i < chan->rx_q_count; i++)
                {
                    enetxapi_queue_int_enable(chan, i);
                }
            }
        }
        else
        {
            work = 0;
            yield();
        }

    }

    return 0;
}

int enet_opened = 0;

static int enet_open(struct net_device *dev)
{
    enetx_channel *chan = enetx_channels;
    enetx_port_t *port = ((enetx_netdev *)netdev_priv(dev))->port;

    enet_opened++;
    if (port->port_class == PORT_CLASS_SW)
        return 0;

    enet_dbg("%s: opened\n", dev->name);

    if (open_count == 0)
    {
        while (chan)
        {
            int i;
            for (i = 0; i < chan->rx_q_count; i++)
                enetxapi_queue_int_enable(chan, i);

            chan = chan->next;
        }
    }

    open_count++;

    if (strcmp(dev->name, "eth0"))
        port_open(port);

    netif_start_queue(dev);

    return 0;
}

static int enet_stop(struct net_device *dev)
{
    enetx_channel *chan = enetx_channels;
    enetx_port_t *port = ((enetx_netdev *)netdev_priv(dev))->port;

    if (port->port_class == PORT_CLASS_SW)
        return 0;

    enet_dbg("%s: stopped\n", dev->name);

    netif_stop_queue(dev);

    if (strcmp(dev->name, "eth0"))
        port_stop(port);

    open_count--;

    if (open_count == 0)
    {
        while (chan)
        {
            int i;
            for (i = 0; i < chan->rx_q_count; i++)
                enetxapi_queue_int_disable(chan, i);

            chan = chan->next;
        }
    }

    return 0;
}

/* IEEE 802.3 Ethernet constant */
#define ETH_CRC_LEN             4

void inline get_mark_pNbuff(pNBuff_t *pNBuff, uint32_t **mark)
{
    void * pBuf = PNBUFF_2_PBUF(pNBuff);

     if ( IS_SKBUFF_PTR(pNBuff) )
        *mark = (uint32_t *)&(((struct sk_buff *)pBuf)->mark);
    else
        *mark = (uint32_t *)&(((FkBuff_t *)pBuf)->mark); 
}

int
enet_fwdcb_register(fwdcb_t fwdcb)
{
	if (fwdcb) {
		enet_fwdcb = fwdcb;
		return 0;
	}
	enet_fwdcb = NULL;
	return 0;
}
EXPORT_SYMBOL(enet_fwdcb_register);

// based on impl5\bcmenet.c:bcm63xx_enet_xmit()
static netdev_tx_t enet_xmit(pNBuff_t pNBuff, struct net_device *dev)
{
    enetx_port_t *port = NETDEV_PRIV(dev)->port;
    enetx_port_t *egress_port;
#if defined(PKTC)
    bool is_chained = FALSE;
    pNBuff_t pNBuff_next = NULL;
#endif
    int ret = 0;
    uint32_t *pMark;
    uint32_t len = 0, priority, rflags, mark = 0;
    uint8_t *data;
    dispatch_info_t dispatch_info = {};
    int hiPrioFlag=0;
    struct sk_buff *skb = NULL;
#ifdef CONFIG_BCM_BPM_BUF_TRACKING
    FkBuff_t *fkb = NULL;
#endif

	/* If Broadstream iqos enable, for WAN egress packets, need to call dev_queue_xmit */
    if (BROADSTREAM_IQOS_ENABLE() && pNBuff && (dev->priv_flags & IFF_WANDEV)) {
        if(IS_FKBUFF_PTR(pNBuff)) {
            /* From wl or enet driver rx */
			skb = bcm_iqoshdl_wrapper(dev, pNBuff);
            if (skb == NULL) {
			    goto drop_exit;
            }
			PKTSETFKBTOQMIT(skb);
		}
        else if(IS_SKBUFF_PTR(pNBuff)) {
            /* From dhd driver rx */
            skb = PNBUFF_2_SKBUFF(pNBuff);
            if (PKTISFCDONE(skb)) {
                PKTCLRFCDONE(skb);
                skb->dev = dev;
            }
			else
				goto normal_path;
        }
		else
		    goto drop_exit;

        /* 1. For broadstream iqos cb function.
         * 2. cb need to know it is fkb or skb
         */
        if (enet_fwdcb && enet_fwdcb(pNBuff, dev) == PKT_DROP) {
            nbuff_free(SKBUFF_2_PNBUFF(skb));
            return PKT_DROP;
        }
		local_bh_enable();
        dev_queue_xmit(skb);
		local_bh_disable();
		return 0;
    }

normal_path:

#if defined(PKTC)
    /* for PKTC, pNBuff is chained skb */
    if (IS_SKBUFF_PTR(pNBuff))
    {
        is_chained = PKTISCHAINED(pNBuff);
    }

    do {
#endif
        enet_dbg_tx("The physical port_id is %d (%s)\n", port->p.mac->mac_id, port->obj_name);

        get_mark_pNbuff(pNBuff, &pMark);
        INC_STAT_TX_Q_IN(port,SKBMARK_GET_Q_PRIO(*pMark));

        /* adjust tx priority q based on packet type (ARP, LCP) */ 
#ifdef PKTC
        if (!is_chained)
#endif
        {
            if (IS_SKBUFF_PTR(pNBuff))
            {
                skb = PNBUFF_2_SKBUFF(pNBuff);

                ETH_GBPM_TRACK_SKB( skb, GBPM_VAL_ENTER, 0 );
                ETH_GBPM_TRACK_SKB( skb, GBPM_VAL_TX, 0 );

                check_arp_lcp_pkt(skb->data, hiPrioFlag);
                if (hiPrioFlag)
                {
                    uint32_t hiPrioQ = MAX_PRIORITY_VALUE;
                    /* Give the highest possible priority to ARP/LCP packets */
#ifdef RUNNER
                    while ( hiPrioQ && BDMF_ERR_OK != rdpa_egress_tm_queue_exists(port->p.port_id, hiPrioQ) ) hiPrioQ--;
#endif                    
                    *pMark = SKBMARK_SET_Q_PRIO(*pMark, hiPrioQ);
                }
            }
        }
#ifdef CONFIG_BCM_BPM_BUF_TRACKING
        if (skb == NULL)
        {
            fkb = SKBUFF_2_PNBUFF(pNBuff);
            ETH_GBPM_TRACK_FKB( fkb, GBPM_VAL_ENTER, 0 );
            ETH_GBPM_TRACK_FKB( fkb, GBPM_VAL_TX, 0 );
        }
#endif
#ifdef SF2_DEVICE
        /* external switch queue remapping */
        if (port->p.ops->tx_q_remap)
        {
            uint32_t txq = port->p.ops->tx_q_remap(port,SKBMARK_GET_Q_PRIO((*pMark)));
            *pMark = SKBMARK_SET_Q_PRIO((*pMark), txq);
        }
#endif        
        //TODO_DSL? do we really need to modify mark in pNBuff? or just update dispatch_info.egress_queue is enough???

        if (nbuff_get_params_ext(pNBuff, &data, &len, &mark, &priority, &rflags) == NULL)
        {
            INC_STAT_TX_DROP(port,tx_dropped_bad_nbuff);
            return 0;
        }

        dispatch_info.channel = SKBMARK_GET_PORT(mark);
        dispatch_info.egress_queue = SKBMARK_GET_Q_PRIO(mark);

#ifdef NEXT_LEVEL_MUX_REQUIRED
        if (port->p.mux_port_tx)
        {
            port->p.mux_port_tx(port, pNBuff, &egress_port);
            if (unlikely(!egress_port))
            {
                enet_err("failed to mux %s/%s:%d\n", port->obj_name, port->p.parent_sw->obj_name, port->p.port_id);
                INC_STAT_TX_DROP(port->p.parent_sw,tx_dropped_mux_failed);/* XXX: is this ok ? no netdev here */
                return NETDEV_TX_OK;
            }
        }
        else
#endif
        {
            egress_port = port; /* "mux to root_sw" */
        }

        enet_dbg_tx("xmit from %s to %s/%s:%d\n", port->obj_name, egress_port->obj_name, egress_port->p.parent_sw->obj_name, egress_port->p.port_id);
        enet_dbg_tx("The egress queue is %d \n", dispatch_info.egress_queue);

#if defined(PKTC)
        if (is_chained)
        {
            pNBuff_next = PKTCLINK(pNBuff);
            PKTSETCLINK(pNBuff, NULL);
        }
#endif

#ifdef SF2_DEVICE
        /* if egress port on external switch with multiple IMP connection, select a runner port */
        if (egress_port->p.ops->tx_lb_imp)
            dispatch_info.lag_port = egress_port->p.ops->tx_lb_imp(egress_port, egress_port->p.mac->mac_id, data);
        ///enet_dbgv("%s/%s/q%d len:%d imp=%d\n", port->obj_name, port->dev->name, dispatch_info.egress_queue, len, dispatch_info.lag_port); /// one line tx info
#endif

#ifdef CONFIG_BLOG
        /* Pass to blog->fcache, so it can construct the customized fcache based execution stack */
        /* TODO: blog_chnl is based on network device attached to xmit port, not egress_port ? */

		if (IS_SKBUFF_PTR(pNBuff) && (PNBUFF_2_SKBUFF(pNBuff)->blog_p)) {
#ifdef PKTC
            if (!is_chained)
#endif
            {
                PNBUFF_2_SKBUFF(pNBuff)->blog_p->lag_port = dispatch_info.lag_port;
				if (BROADSTREAM_IQOS_ENABLE()) {
				    if ((dev->priv_flags & IFF_WANDEV) && !PKTISFKBTOQMIT(skb))
				   	    blog_emit(pNBuff, dev, TYPE_ETH, port->n.set_channel_in_mark ? dispatch_info.channel : port->n.blog_chnl, port->n.blog_phy);
				}
			    else
				   	blog_emit(pNBuff, dev, TYPE_ETH, port->n.set_channel_in_mark ? dispatch_info.channel : port->n.blog_chnl, port->n.blog_phy);
            }
        }
#endif /* CONFIG_BLOG */

#ifdef SF2_DEVICE
        /* if egress port is on external switch insert brcm tag as necessary */
        if (egress_port->p.ops->tx_pkt_mod)
            if (egress_port->p.ops->tx_pkt_mod(egress_port, &pNBuff, &data, &len, (1<< egress_port->p.mac->mac_id)))
                goto enet_xmit_cont;
#endif
        /* TODO: data demux should happen here */

        if (unlikely(len < ETH_ZLEN))
        {                
            nbuff_pad(pNBuff, ETH_ZLEN - len);
            if (IS_SKBUFF_PTR(pNBuff))
                (PNBUFF_2_SKBUFF(pNBuff))->len = ETH_ZLEN;
            len = ETH_ZLEN;
        }

        dispatch_info.drop_eligible = 0;

#if defined(CC_DROP_PRECEDENCE)
        if (enet_dp_lookup_cb != NULL)
        {
            dispatch_info.drop_eligible = enet_dp_lookup_cb(dev, data, len);
        }
#endif /* CC_DROP_PRECEDENCE */

        dispatch_info.pNBuff = pNBuff;
        dispatch_info.port = egress_port;
        if (unlikely(dg_in_context))
            dispatch_info.no_lock = 1;

#ifdef CONFIG_BCM_BPM_BUF_TRACKING
        if (skb != NULL)
        {
            ETH_GBPM_TRACK_SKB( skb, GBPM_VAL_EXIT, 0 );
        }
        else if (fkb != NULL)
        {
            ETH_GBPM_TRACK_FKB( fkb, GBPM_VAL_EXIT, 0 );
        }
#endif

        ret = egress_port->p.ops->dispatch_pkt(&dispatch_info);

        /* update stats */
        if (unlikely(ret))
        {
            INC_STAT_TX_DROP(egress_port,tx_dropped_dispatch);
        }
        else
        {
            INC_STAT_TX_PKT_BYTES(egress_port,len+ETH_CRC_LEN);
            INC_STAT_TX_Q_OUT(egress_port,dispatch_info.egress_queue);
        }

#ifdef SF2_DEVICE
enet_xmit_cont:
#endif
    
#if defined(PKTC)
        if (is_chained)
        {
            pNBuff = pNBuff_next;
        }

    } while (is_chained && pNBuff && IS_SKBUFF_PTR(pNBuff));
#endif

    return ret;

drop_exit:
    INC_STAT_TX_DROP(port,rx_packets_blog_done);
    nbuff_free(pNBuff);
	return 0;
}


#if !defined(DSL_RUNNER_DEVICE)
int enetxapi_post_parse(void)
{
    return 0;
}

static int _handle_mii(struct net_device *dev, struct ifreq *ifr, int cmd)
{
    struct mii_ioctl_data *mii = if_mii(ifr);
    enetx_port_t *port = ((enetx_netdev *)netdev_priv(dev))->port;
    
    if (port->port_class != PORT_CLASS_PORT || !port->p.phy)
        return -EINVAL;

    switch (cmd)
    {
        case SIOCGMIIPHY: /* Get address of MII PHY in use by dev */
            mii->phy_id = port->p.phy->addr;
            return 0;
        case SIOCGMIIREG: /* Read MII PHY register. */
            return phy_dev_read(port->p.phy, mii->reg_num & 0x1f, &mii->val_out) ? -EINVAL : 0;
        case SIOCSMIIREG: /* Write MII PHY register. */
            return phy_dev_write(port->p.phy, mii->reg_num & 0x1f, mii->val_in) ? -EINVAL : 0;
    }

    return -EINVAL;
}
#endif // !defined(DSL_RUNNER_DEVICE)

static struct rtnl_link_stats64 *enet_get_stats64(struct net_device *dev, struct rtnl_link_stats64 *net_stats)
{
    enetx_port_t *port = ((enetx_netdev *)netdev_priv(dev))->port;

    port_stats_get(port, net_stats);

    return net_stats;
}

#include "linux/if_bridge.h"
#include "linux/bcm_log.h"
static void enet_clr_stats(struct net_device *dev)
{
    enetx_port_t *port = ((enetx_netdev *)netdev_priv(dev))->port;

#if defined(CONFIG_BCM_KERNEL_BONDING)
    bcmFun_t *bcmFun = bcmFun_get(BCM_FUN_ID_BOND_CLR_SLAVE_STAT);
    if (bcmFun) bcmFun(dev);
#endif 

    port_stats_clear(port);
}

extern int enet_ioctl_compat(struct net_device *dev, struct ifreq *rq, int cmd);
/* Called with rtnl_lock */
static int enet_ioctl(struct net_device *dev, struct ifreq *ifr, int cmd)
{
    int rc;

    switch (cmd)
    {
        case SIOCGMIIPHY:
        case SIOCGMIIREG:
        case SIOCSMIIREG:
#if defined(DSL_RUNNER_DEVICE)
            return ioctl_handle_mii(dev, ifr, cmd);
#else
            return _handle_mii(dev, ifr, cmd);
#endif            
    }

#ifdef IOCTL_COMPAT
    rc = enet_ioctl_compat(dev, ifr, cmd);
    return rc;
#else
    return -EOPNOTSUPP;
#endif
}

static int enet_change_mtu(struct net_device *dev, int new_mtu)
{
    enetx_port_t *port = ((enetx_netdev *)netdev_priv(dev))->port;

    if (port->port_class != PORT_CLASS_PORT)
        return -EINVAL;

    if (new_mtu < ETH_ZLEN || new_mtu > ENET_MAX_MTU_PAYLOAD_SIZE)
        return -EINVAL;

    if (port_mtu_set(port, new_mtu))
        return -EINVAL;

    dev->mtu = new_mtu;

    return 0;
}

static int enet_set_mac_addr(struct net_device *dev, void *p)
{
    struct sockaddr *addr = p;

    if (netif_running(dev))
    {
        printk(KERN_WARNING "Warning: Setting MAC address of %s while it is ifconfig UP\n", dev->name);
        // return -EBUSY;
    }

    /* Don't do anything if there isn't an actual address change */
    if (memcmp(dev->dev_addr, addr->sa_data, dev->addr_len)) {
        kerSysReleaseMacAddress(dev->dev_addr);
        memmove(dev->dev_addr, addr->sa_data, dev->addr_len);
    }

    return 0;
}

#if defined(CONFIG_BCM_KERNEL_BONDING) && !defined(SUPPORT_ETHTOOL)
#error "SUPPORT_ETHTOOL/BUILD_ETHTOOL must be defined with CONFIG_BCM_KERNEL_BONDING"
#endif
#if defined(CONFIG_BCM_PTP_1588) && !defined(SUPPORT_ETHTOOL)
#error "SUPPORT_ETHTOOL/BUILD_ETHTOOL must be defined with CONFIG_BCM_PTP_1588"
#endif

static const struct net_device_ops enet_netdev_ops_port =
{
    .ndo_open = enet_open,
    .ndo_stop = enet_stop,
    .ndo_start_xmit = (HardStartXmitFuncP)enet_xmit,
    .ndo_do_ioctl = enet_ioctl,
    .ndo_get_stats64 = enet_get_stats64,
    .ndo_change_mtu = enet_change_mtu,
    .ndo_set_mac_address  = enet_set_mac_addr,
};

static const struct net_device_ops enet_netdev_ops_sw =
{
    .ndo_do_ioctl = enet_ioctl,
    .ndo_get_stats64 = enet_get_stats64,
    .ndo_set_mac_address  = enet_set_mac_addr,
};

extern int port_by_netdev(struct net_device *dev, enetx_port_t **match);
#if defined(CONFIG_BCM_KERNEL_BONDING)
static uint32_t enet_get_sw_bonding_map(enetx_port_t *sw, uint16_t grp_no);
#endif

static int tr_rm_wan_map(enetx_port_t *port, void *_ctx)
{
    uint32_t *portMap = (uint32_t *)_ctx;

    if (port->dev && (port->dev->priv_flags & IFF_WANDEV))
        *portMap &= ~(1 <<port->p.mac->mac_id);
    return (*portMap == 0);
}

static int tr_sw_update_br_pbvlan(enetx_port_t *sw, void *_ctx)
{
    char *brName = (char *)_ctx;
    unsigned int brPort = 0xFFFFFFFF;
    struct net_device *dev;
    uint32_t portMap = 0;

    // if switch does not support pbvlan hw config, skip to next switch
    if (sw->s.ops->update_pbvlan == NULL)
        return 0;

    for(;;)
    {
        enetx_port_t *port;
#if defined(CONFIG_BCM_KF_NETFILTER)
        dev = bridge_get_next_port(brName, &brPort);
#else
        dev = NULL;
#endif
        if (dev == NULL)
            break;
        /* find root device */
        while( !netdev_path_is_root(dev) )
        {
            dev = netdev_path_next_dev(dev);
        }
#if defined(CONFIG_BCM_KERNEL_BONDING)
        /* Check with Bonding Driver the Master ID to get corresponding Ethernet Interfaces */
        if ( netif_is_bond_master(dev) )
        {
            int mstr_id = -1;
            BCM_BondingMasterInfo params;
            bcmFun_t *bcmFun = bcmFun_get(BCM_FUN_ID_BOND_GET_MSTR_INFO);
            if (bcmFun) {
                params.dev = dev;
                mstr_id = bcmFun(&params);
                if (mstr_id >= 0 && mstr_id < MAX_KERNEL_BONDING_GROUPS)
                {
                    portMap |= enet_get_sw_bonding_map(sw, mstr_id);
                }
            }
         }
        else
#endif /* defined(CONFIG_BCM_KERNEL_BONDING) */
        if (port_by_netdev(dev, &port)==0)
        {
            if (port->p.parent_sw == sw)
                portMap |= 1 <<port->p.mac->mac_id;
        }
    }

    /* Remove wanPort from portmap --- These ports are always isolated */
    _port_traverse_ports(sw, tr_rm_wan_map, PORT_CLASS_PORT, &portMap, 1);

    return sw->s.ops->update_pbvlan(sw, portMap);
}

static void bridge_update_pbvlan(char *brName)
{
    // traverse switch find ports associated with specified bridge
    port_traverse_ports(root_sw, tr_sw_update_br_pbvlan, PORT_CLASS_SW, brName);
}

void enet_update_pbvlan_all_bridge(void)
{
    char br_list[64];
    char *brName, *tracker;
    rcu_read_lock();
    bridge_get_br_list(br_list, sizeof(br_list));
    tracker = br_list;
    while (tracker)
    {
        brName = tracker;
        tracker = strchr(tracker,',');
        if(tracker)
        {
            *tracker = '\0';
            tracker++;
        }
        bridge_update_pbvlan(brName);
    }
    rcu_read_unlock();
}

#if defined(CONFIG_BCM_LOG)
/*
 * Wrapper function for other Kernel modules to check
 * if a given logical port is WAN or NOT.
 */
extern enetx_port_t *port_by_unit_port(int unit_port);

static int bcmenet_lag_port_get(void *dev)
{
    enetx_port_t *port = NETDEV_PRIV((struct net_device *)(dev))->port;
    enetx_port_t *egress_port;

#ifdef NEXT_LEVEL_MUX_REQUIRED
    if (port->p.mux_port_tx)
    {
        port->p.mux_port_tx(port, NULL, &egress_port);
        if (unlikely(!egress_port))
        {
            enet_err("failed to mux %s/%s:%d\n", port->obj_name, port->p.parent_sw->obj_name, port->p.port_id);

            return -1;
        }
    }
    else
#endif
    {
        egress_port = port; /* "mux to root_sw" */
    }

    if (egress_port->p.ops->tx_lb_imp)
    {
        return egress_port->p.ops->tx_lb_imp(egress_port, egress_port->p.mac->mac_id, NULL);
    }

    return 0;
}

static int bcmenet_is_bonded_lan_wan_port(void *ctxt)
{
    /* based on impl5\bcmenet.c:bcmenet_is_bonded_lan_wan_port() */
    int ret_val = 0;
#if defined(CONFIG_BCM_KERNEL_BONDING)
    int logical_port = *((int*)ctxt);
    enetx_port_t *port = port_by_unit_port(logical_port);

    if (port && port->p.bond_grp)
    {
        if (port->p.bond_grp->is_lan_wan_cfg &&
            port->p.bond_grp->lan_wan_port == port )
        {
            ret_val = 1;
        }
    }
#endif
    return ret_val;
}

static int bcmenet_is_wan_port(void *ctxt)
{
    int logical_port = *((int*)ctxt);
    enetx_port_t *port = port_by_unit_port(logical_port);
    
    return (port && !bcmenet_is_bonded_lan_wan_port(ctxt)) ? PORT_ROLE_IS_WAN(port) : 0;
}

#if defined(CONFIG_BCM_KERNEL_BONDING)

bond_info_t bond_grps[MAX_KERNEL_BONDING_GROUPS];

static int bcmenet_get_max_bonds(void *ctxt)
{
    return MAX_KERNEL_BONDING_GROUPS;
}

struct tr_bond_chk_data
{
    bond_info_t *grp;
    enetx_port_t *port;                 // port to be added/removed
    uint32_t pmap[BP_MAX_ENET_MACS];
    int lan_cnt, wan_cnt;
};

static int tr_bond_err_chk(enetx_port_t *port, void *_ctx)
{
    struct tr_bond_chk_data *data = (struct tr_bond_chk_data *)_ctx;

    if ((port->p.bond_grp == data->grp) || (port == data->port))
    {
        data->pmap[PORT_ON_ROOT_SW(port)?0:1] |= 1<< port->p.mac->mac_id;
        if (PORT_ROLE_IS_WAN(port) && data->grp->lan_wan_port != port)
            data->wan_cnt++;
        else
            data->lan_cnt++;
    }
    return 0;
}
/* Function to do error check before making changes to the bonding group */
static int enet_bonding_error_check(bond_info_t *grp, enetx_port_t *port)
{
    struct tr_bond_chk_data data = {};

    data.grp = grp;
    data.port = port;
    port_traverse_ports(root_sw, tr_bond_err_chk, PORT_CLASS_PORT, &data);

    if (data.wan_cnt > 1)
    {
        enet_err("Two WAN ports can't be bonded <0x%04x-0x%04x>\n", data.pmap[0], data.pmap[1]);
        return -1;
    }
    if (data.wan_cnt && data.lan_cnt > 1)
    {
        enet_err("More than one LAN port can't be bonded with WAN <0x%04x-0x%04x>\n", data.pmap[0], data.pmap[1]);
        return -1;
    }
    return 0;
}

struct tr_sw_bond_map_data
{
    bond_info_t *grp;
    uint32_t pmap;
};

static int tr_sw_bond_map(enetx_port_t *port, void *_ctx)
{
    struct tr_sw_bond_map_data *data = (struct tr_sw_bond_map_data *)_ctx;

    if (port->p.bond_grp == data->grp)
        data->pmap |= 1 << port->p.mac->mac_id;
    return 0;
}

static uint32_t enet_get_sw_bonding_map(enetx_port_t *sw, uint16_t grp_no)
{
    struct tr_sw_bond_map_data data = {};

    data.grp = &bond_grps[grp_no];
    // traverse current switch only
    _port_traverse_ports(sw, tr_sw_bond_map, PORT_CLASS_PORT, &data, 1);
    return data.pmap;
}

struct tr_bond_wan_cfg_data
{
    bond_info_t *grp;
    enetx_port_t *port;         // port to be added/removed
    enetx_port_t *lan_port;
    enetx_port_t *wan_port;
};

static int tr_bond_wan_cfg(enetx_port_t *port, void *_ctx)
{
    struct tr_bond_wan_cfg_data *data = (struct tr_bond_wan_cfg_data *)_ctx;

    if ((port->p.bond_grp == data->grp) || (port == data->port))
    {
        if (PORT_ROLE_IS_WAN(port) && data->grp->lan_wan_port != port)
            data->wan_port = port;
        else
            data->lan_port = port;
    }

    return (data->wan_port && data->lan_port)? 1 : 0;
}

/* Function to configure the switch port as WAN port based on grouping */
static void bcmenet_do_wan_bonding_cfg_for_grp(uint16_t grp_no, uint16_t add_member, enetx_port_t *port)
{
    /* For the provided group and port, configuration is changed. Take care of any WAN port related configuration */
    struct tr_bond_wan_cfg_data data = {};
    bond_info_t *bond_grp = &bond_grps[grp_no];

    data.grp = bond_grp;
    data.port = port;
    port_traverse_ports(root_sw, tr_bond_wan_cfg, PORT_CLASS_PORT, &data);

    if (data.wan_port && data.lan_port) /* Both LAN & WAN are/were part of the group */
    {
        if (add_member ^ bond_grp->is_lan_wan_cfg)
        {
            /* modify lan port port_cap, so role_set won't fail */
            data.lan_port->p.port_cap = add_member? PORT_CAP_LAN_WAN : PORT_CAP_LAN_ONLY;
            port_netdev_role_set(data.lan_port, add_member? PORT_NETDEV_ROLE_WAN: PORT_NETDEV_ROLE_LAN, 1/*is_bond*/);
            /* also clear lan port ARL entries */
            if (add_member && data.lan_port->p.ops->fast_age)
                data.lan_port->p.ops->fast_age(data.lan_port);
            bond_grp->is_lan_wan_cfg = add_member;
            bond_grp->lan_wan_port = add_member ? data.lan_port : NULL;
        }
    }
}

static int tr_set_grp_blog_chnl(enetx_port_t *port, void *_ctx)
{
    bond_info_t *bond_grp = (bond_info_t *)_ctx;

    if ((port->p.bond_grp == bond_grp) && (bond_grp->blog_chnl_rx > port->n.blog_chnl))
        bond_grp->blog_chnl_rx = port->n.blog_chnl;
    return 0;
}
static int tr_set_port_blog_chnl_rx(enetx_port_t *port, void *_ctx)
{
    bond_info_t *bond_grp = (bond_info_t *)_ctx;

    if (port->p.bond_grp == bond_grp)
    {
        if (bond_grp->blog_chnl_rx == port->n.blog_chnl)
            netdev_path_set_hw_port(bond_grp->bond_dev, port->n.blog_chnl, port->n.blog_phy);
        port->n.blog_chnl_rx = bond_grp->blog_chnl_rx;
    }
    return 0;
}
static void update_bond_grp_blog_chnl_rx(bond_info_t *bond_grp)
{
    if (bond_grp->port_count == 0)
        return;
    /* find lowest blog_chnl_rx */
    bond_grp->blog_chnl_rx = 0xffffffff;
    port_traverse_ports(root_sw, tr_set_grp_blog_chnl, PORT_CLASS_PORT, bond_grp);
    enet_dbgv("bond_grp %d blog_chnl=%x\n", bond_grp->grp_idx, bond_grp->blog_chnl_rx);
    /* set all member ports with this this blog_chnl_rx value */
    port_traverse_ports(root_sw, tr_set_port_blog_chnl_rx, PORT_CLASS_PORT, bond_grp);
}

static int enet_update_bond_config(BCM_EnetBondingInfo *info_p, enetx_port_t *port)
{
    uint16_t grp_no = info_p->bonding_group_id;
    uint16_t add_member = info_p->is_join;
    int rc = 0;
    bond_info_t *bond_grp = &bond_grps[grp_no];

    if (grp_no >= MAX_KERNEL_BONDING_GROUPS)
    {
        return -1;
    }

    rc = enet_bonding_error_check(bond_grp, port);
    if (rc)
    {
        return rc;
    }
    if (add_member)
    {
        /* Check if already a member */
        if (port->p.bond_grp)
        {
            enet_err("%s already a member of bond group = %d\n", port->obj_name, grp_no);
            return 0;
        }
        if (bond_grp->port_count == 0)
            bond_grp->bond_dev = info_p->bond_dev;
        bond_grp->port_count++;
        bond_grp->grp_idx = grp_no;
        port->p.bond_grp = bond_grp;
    }
    else
    {
        /* Check if not already a member */
        if (!port->p.bond_grp)
        {
            enet_err("%s not a member of bond group = %d\n", port->obj_name, grp_no);
            return 0;
        }
        bond_grp->port_count--;
        port->p.bond_grp = NULL;
        port->n.blog_chnl_rx = port->n.blog_chnl;
        if (bond_grp->port_count == 0)
            bond_grp->bond_dev = NULL;
    }
    /* bonding group membeship changed, update blog_chnl_rx */
    update_bond_grp_blog_chnl_rx(bond_grp);

    /* Update HW Switch - restricting to only External switch for now */
    if (port->p.parent_sw->s.ops->config_trunk)
        rc = port->p.parent_sw->s.ops->config_trunk(port->p.parent_sw, port, grp_no, add_member);

    if (!rc)
    {
        bcmenet_do_wan_bonding_cfg_for_grp(grp_no, add_member?1:0, port);
    }

    if (bond_grp->port_count == 0) /* No more members in the bond group */
    {
        memset(bond_grp, 0, sizeof(*bond_grp));
    }

    return rc;
}

static int bcmenet_handle_bonding_change(void *ctxt)
{
    BCM_EnetBondingInfo *enetBondingInfo_p = (BCM_EnetBondingInfo*)ctxt;
    struct net_device *dev = enetBondingInfo_p->slave_dev;
    int print_once = 1;
    int err = 0;
    enetx_port_t *port;
    /* find root device */
    while( 1 )
    {
        if(netdev_path_is_root(dev))
        {
            break;
        }
        if (print_once && enetBondingInfo_p->is_join)
        {
            print_once = 0;
            /* One of the major issue with non-root device bonding is that if leaf device gets deleted/unregistered, Ethernet driver
               won't know which physical device it was associated with and will not remove the bonding configuration */
            enet_err("\n\n WARNING : Slave device <%s> is not a root device; Bonding must be done on physical interfaces.\n\n",dev->name);
        }
        /* slave_dev is on hold in Bonding driver -- don't put it back */
        if (dev != enetBondingInfo_p->slave_dev)
        {
            dev_put(dev);
        }
        dev = netdev_path_next_dev(dev);
        dev_hold(dev);
    }

    /* Check if this root device is managed by Ethernet Driver */
    if (port_by_netdev(dev, &port) == 0) 
    {
        err = enet_update_bond_config(enetBondingInfo_p, port);
    }
    else
    {
        enet_err("Slave Device <%s> Root Dev <%s> not managed by Ethernet Driver\n",enetBondingInfo_p->slave_dev->name,dev->name);
    }

    if (dev != enetBondingInfo_p->slave_dev)
    {
        dev_put(dev);
    }

    /* Based on sequence of operations, like:
       - remove ethernet interface (say eth1) from bridge => all other bridge ports will isolate this ethernet interface
       - add bond interface to bridge prior to adding eth1 to bond interface 
       - now add eth1 to bond interface, this will not trigger any bridge update notification and eth1 will be left out. 
      * to avoid above condition, better to update the pbvlan mapping on every bonding update, if bond interface is in bridge. */ 
    if (!err && enetBondingInfo_p->bond_dev->priv_flags & IFF_BRIDGE_PORT)
    {
        /* Would have been better to only update for the bridge this bond interface is part of ...
           but don't know any easy way to get the bridge from device. */
        enet_update_pbvlan_all_bridge();
    }

    return err;
}

#endif /* CONFIG_BCM_KERNEL_BONDING */
#endif /* CONFIG_BCM_LOG */

#ifdef CONFIG_BLOG
/*---------------------------------------------------------------------------
 * unsigned long bcmenet_get_xmit_mark(void *dev, int priority, unsigned long uMark)
 * Description:
 *    Bump the priority specified in input parameter.
 *    This function is for blog to raise TCP ACK TX priority. 
 *---------------------------------------------------------------------------
 */
static unsigned long bcmenet_get_xmit_mark(void *dev, int priority, unsigned long uMark)
{
    struct net_device *net_dev = (struct net_device *)dev;
    enetx_port_t *port = NULL;
    unsigned long uNewMark = uMark;
    int dataPrio = SKBMARK_GET_Q_PRIO(uMark); 
    int orgAckPrio = priority; 
    if (net_dev)
    {
        if (port_by_netdev(net_dev, &port) == 0)
        {
            if (port->p.ops->tx_q_remap)
            {
                uint32_t data_txq = port->p.ops->tx_q_remap(port,dataPrio);
                uint32_t ack_txq = port->p.ops->tx_q_remap(port,priority);

                do
                {
                    if (ack_txq > data_txq) break;
                    priority++;
                    ack_txq = port->p.ops->tx_q_remap(port,priority);
                } while (priority < MAX_PRIORITY_VALUE);
                if (ack_txq <= data_txq) priority = orgAckPrio;
            }
            uNewMark = SKBMARK_SET_Q_PRIO(uMark, priority);
        }
    }
    return uNewMark;
}  /* bcmenet_get_xmit_mark() */
#endif 

/* OAMPDU Ethernet dying gasp message */
static unsigned char dg_ethOam_frame[64] = {
    1, 0x80, 0xc2, 0, 0, 2,
    0, 0,    0,    0, 0, 0, /* Fill Src MAC at the time of sending, from dev */
    0x88, 0x9,
    3, /* Subtype */
    0, 0x52,    /* local stable, remote stable, dying gasp */
    1,          /* OAMPDU - information */
    'B', 'R', 'O', 'A', 'D', 'C', 'O', 'M',
    ' ', 'B', 'C', 'G',

};

/* Socket buffer and buffer pointer for msg */
static struct sk_buff *dg_skbp;

static void _enet_init_dg_skbp(void)
{
    /* Set up dying gasp buffer from packet transmit when we power down */
    dg_skbp = alloc_skb(64, GFP_ATOMIC);
    if (dg_skbp)
    {    
        memset(dg_skbp->data, 0, 64); 
        dg_skbp->len = 64;
        memcpy(dg_skbp->data, dg_ethOam_frame, sizeof(dg_ethOam_frame)); 
    }
}

static int tr_send_dg_pkt(enetx_port_t *port, void *_ctx)
{
    struct net_device *dev = port->dev;

    // Is this a ethernet WAN port?
    if (dev  && (dev->priv_flags & IFF_WANDEV) &&
        (port->port_type == PORT_TYPE_RUNNER_PORT || port->port_type == PORT_TYPE_SF2_PORT)) {
        int ret;

        /* Copy src MAC from dev into dying gasp packet */
        memcpy(dg_skbp->data + ETH_ALEN, dev->dev_addr, ETH_ALEN);

        /* Transmit dying gasp packet */
        ret = enet_xmit(SKBUFF_2_PNBUFF(dg_skbp), dev);
        printk("\n%s DG sent out on wan port %s (ret=%d)\n", __FUNCTION__, dev->name, ret);
        return 1;
    }
    return 0;
}

static void enet_switch_power_off(void *context)
{
    /* Indicate we are in a dying gasp context and can skip
       housekeeping since we're about to power down */
    dg_in_context = 1;

    if (dg_skbp == NULL) {
        enet_err("No DG skb to send \n");
        return;
    }
    port_traverse_ports(root_sw, tr_send_dg_pkt, PORT_CLASS_PORT, NULL);
}

int _enet_dev_flags_by_role(enetx_port_t *self)
{
    struct net_device *dev = self->dev;
    uint32_t flags = dev->priv_flags;

    if (self->n.port_netdev_role == PORT_NETDEV_ROLE_WAN)
    {
        flags |= IFF_WANDEV;
#ifdef NETDEV_HW_SWITCH
        flags &= ~IFF_HW_SWITCH;
#endif
    }
    else if (self->n.port_netdev_role == PORT_NETDEV_ROLE_LAN)
    {
        flags &= ~IFF_WANDEV;
#ifdef NETDEV_HW_SWITCH
        flags |= IFF_HW_SWITCH;
#endif
    }
    
    return flags;
}

void enet_dev_flags_update(enetx_port_t *self)
{
    self->dev->priv_flags = _enet_dev_flags_by_role(self);
}

void extsw_set_mac_address(uint8_t *addr);

int enet_dev_mac_set(enetx_port_t *p, int set)
{
    unsigned char macaddr[ETH_ALEN];
    int mac_group = 0;
    struct sockaddr sockaddr;

#ifdef SEPARATE_MAC_FOR_WAN_INTERFACES
    if (p->n.port_netdev_role == PORT_NETDEV_ROLE_WAN)
        mac_group = p->dev->ifindex;
#endif

#ifdef SEPARATE_MAC_FOR_LAN_INTERFACES
    if (p->n.port_netdev_role == PORT_NETDEV_ROLE_LAN)
        mac_group = p->dev->ifindex;
#endif

    if (set)
    {
        if (!is_zero_ether_addr(p->dev->dev_addr))
            return -1;

        kerSysGetMacAddress(macaddr, mac_group);
        memmove(sockaddr.sa_data, macaddr, ETH_ALEN);
        sockaddr.sa_family = p->dev->type;

        dev_set_mac_address(p->dev, &sockaddr);

#if defined(DSL_RUNNER_DEVICE)
        if (!IS_ROOT_SW(p) && !PORT_ON_ROOT_SW(p))
        {
            extsw_set_mac_address(p->dev->dev_addr);
        }
#endif // defined(DSL_RUNNER_DEVICE)
    }
    else
    {
        if (is_zero_ether_addr(p->dev->dev_addr))
            return -1;

        kerSysReleaseMacAddress(p->dev->dev_addr);
        memset(p->dev->dev_addr, 0, ETH_ALEN);
    }

    return 0;
}

int enet_create_netdevice(enetx_port_t *p)
{
    int rc;
    struct net_device *dev;
    enetx_netdev *ndev;

    /* TODO: point to port for private data */
    dev = alloc_etherdev(sizeof(enetx_netdev));
    if (!dev)
    {
        enet_err("failed to allocate etherdev for %s\n", p->name);
        return -1;
    }
        
    p->dev = dev;

    SET_MODULE_OWNER(dev);
    if (strlen(p->name))
        dev_alloc_name(dev, p->name);

    dev->priv_flags = _enet_dev_flags_by_role(p);

    dev->watchdog_timeo = 2 * HZ;
    netif_carrier_off(dev);
    netif_stop_queue(dev);

    ndev = netdev_priv(dev);
    ndev->port = p;

    if (p->port_class == PORT_CLASS_SW)
    {
        dev->priv_flags |= IFF_DONT_BRIDGE;
        dev->netdev_ops = &enet_netdev_ops_sw;
    }
    else if (p->port_class == PORT_CLASS_PORT)
    {
#ifdef CONFIG_BLOG
        netdev_path_set_hw_port(dev, p->n.blog_chnl, p->n.blog_phy);
        dev->clr_stats = enet_clr_stats;
#endif
        dev->netdev_ops = &enet_netdev_ops_port;
    }

#if defined(SUPPORT_ETHTOOL) || defined(CONFIG_BCM_PTP_1588)
    dev->ethtool_ops = &bcm63xx_enet_ethtool_ops;
    bcmenet_private_ethtool_ops = &enet_ethtool_ops;
#endif

#if defined(CONFIG_BCM_KF_EXTSTATS)
    dev->features |= NETIF_F_EXTSTATS; /* support extended statistics */
#endif

#if defined(CONFIG_BCM_PKTRUNNER_GSO)
     // GSO is supported only on LAN ports
     switch (p->p.port_cap)
     {
         case PORT_CAP_LAN_ONLY:
             dev->features       |= NETIF_F_SG | NETIF_F_IP_CSUM | NETIF_F_IPV6_CSUM | NETIF_F_TSO | NETIF_F_TSO6;
             dev->vlan_features  |= NETIF_F_SG | NETIF_F_IP_CSUM | NETIF_F_IPV6_CSUM | NETIF_F_TSO | NETIF_F_TSO6;
             break;
         default:
             dev->features       &= ~(NETIF_F_SG | NETIF_F_IP_CSUM | NETIF_F_IPV6_CSUM | NETIF_F_TSO | NETIF_F_TSO6 | NETIF_F_UFO);
             dev->vlan_features  &= ~(NETIF_F_SG | NETIF_F_IP_CSUM | NETIF_F_IPV6_CSUM | NETIF_F_TSO | NETIF_F_TSO6 | NETIF_F_UFO);
     }
 #endif

    dev->destructor = free_netdev;
    rc = register_netdevice(dev);
    if (rc)
    {
        enet_err("failed to register netdev for %s\n", p->obj_name);
        free_netdev(dev);
        p->dev = NULL;
    }
    else
    {
        enet_dbg("registered netdev %s for %s\n", dev->name, p->obj_name);
    }

    enet_dev_mac_set(p, 1);
    enet_change_mtu(dev, BCM_ENET_DEFAULT_MTU_SIZE);

    /* Carrier is always on when no PHY connected */
    if (p->port_class != PORT_CLASS_PORT || !p->p.phy || p->p.phy->link)
    {
        netif_carrier_on(dev);
        port_link_change(p, 1);
    }

#ifdef GPONDEF_CARRIER_ON_UPON_CREATE
    if (!strncmp(dev->name, "gpondef", strlen("gpondef"))) 
    {
        netif_carrier_on(dev);
        port_link_change(p, 1);
    }
#endif

    return rc;
}

void enet_remove_netdevice(enetx_port_t *p)
{
    enet_dbg("unregister_netdevice: %s\n", p->dev->name);
    
    enet_dev_mac_set(p, 0);

    /* XXX: Should syncronize_net even when one port is removed ? */
    unregister_netdevice(p->dev);

    p->dev = NULL;
}

#if defined(CONFIG_BCM_KF_NETFILTER)
static int bridge_notifier(struct notifier_block *nb, unsigned long event, void *brName);
struct notifier_block br_notifier = {
    .notifier_call = bridge_notifier,
};

static int bridge_notifier(struct notifier_block *nb, unsigned long event, void *brName)
{
    switch (event)
    {
        case BREVT_IF_CHANGED:
            bridge_update_pbvlan(brName);
            break;
    }
    return NOTIFY_DONE;
}

static int bridge_stp_handler(struct notifier_block *nb, unsigned long event, void *portInfo);
static struct notifier_block br_stp_handler = {
    .notifier_call = bridge_stp_handler,
};

extern int get_port_by_if_name(char *ifname, enetx_port_t **port);

static int bridge_stp_handler(struct notifier_block *nb, unsigned long event, void *portInfo)
{
    struct stpPortInfo *pInfo = (struct stpPortInfo *)portInfo;
    enetx_port_t *port;

    switch (event)
    {
    case BREVT_STP_STATE_CHANGED:
        {

            if (get_port_by_if_name(&pInfo->portName[0], &port))
                break;

            if (port->p.ops->stp_set)
                port->p.ops->stp_set(port, STP_MODE_UNCHANGED, pInfo->stpState);
            break;
        }
    }
    return NOTIFY_DONE;
}
#endif /* CONFIG_BCM_KF_NETFILTER */

/* WARNING: __exit_refok suppresses warnings from CONFIG_DEBUG_SECTION_MISMATCH
 *          This should only be called from __init or __exit functions.
 */
static void __exit_refok bcm_enet_exit(void)
{
    synchronize_net();

    enet_proc_exit();

    if (root_sw->dev)
        kerSysDeregisterDyingGaspHandler(root_sw->dev->name);

#if defined(CONFIG_BCM_LOG)
    bcmFun_dereg(BCM_FUN_ID_ENET_IS_WAN_PORT);
    bcmFun_dereg(BCM_FUN_ID_ENET_LAG_PORT_GET);
    bcmFun_dereg(BCM_FUN_ID_ENET_IS_BONDED_LAN_WAN_PORT);
#if defined(CONFIG_BCM_KERNEL_BONDING)
    bcmFun_dereg(BCM_FUN_ID_ENET_MAX_BONDS);
    bcmFun_dereg(BCM_FUN_ID_ENET_BONDING_CHANGE);
#endif /* CONFIG_BCM_KERNEL_BONDING */
#endif

#ifdef CONFIG_BLOG
   blog_eth_get_tx_mark_fn = (blog_eth_get_tx_mark_t)NULL;
#endif

#if defined(CONFIG_BCM_KF_NETFILTER)
    /* Unregister bridge notifier hooks */
    unregister_bridge_notifier(&br_notifier);
    unregister_bridge_stp_notifier(&br_stp_handler);
#endif

    enetxapi_queues_uninit(&enetx_channels);
    rtnl_lock();
    sw_free(&root_sw);
    rtnl_unlock();
}
#ifndef DT
module_exit(bcm_enet_exit);
#endif

static int tr_power_down(enetx_port_t *port, void *_ctx)
{
    if (port->p.phy)
        phy_dev_power_set(port->p.phy, 0);

    return 0;
}

int __init bcm_enet_init(void)
{
    int rc;

    if (BCM_SKB_ALIGNED_SIZE != skb_aligned_size())
    {
        enet_err("skb_aligned_size mismatch. Need to recompile enet module\n");
        return -ENOMEM;
    }

    rc = mac_drivers_set();
    if (rc)
        goto exit;

    rc = phy_drivers_set();
    if (rc)
        goto exit;

    rc = bp_parse();
    if (rc)
        goto exit;

    rc = crossbar_finalize();
    if (rc)
        goto exit;

    enetxapi_post_parse();
    
    rc = mac_drivers_init();
    if (rc)
        goto exit;

    rc = phy_drivers_init();
#ifdef PHY_PON
    rc |= phy_driver_set(&phy_drv_pon);
#endif
    if (rc)
        goto exit;

    rc = port_traverse_ports(root_sw, tr_power_down, PORT_CLASS_PORT_DETECT, NULL);
    if (rc)
        goto exit;

    rtnl_lock();
    rc = sw_init(root_sw);
    rtnl_unlock();
    if (rc)
        goto exit;

    rc = enetxapi_queues_init(&enetx_channels);
    if (rc)
        goto exit;

    enetxapi_post_config();

    _enet_init_dg_skbp();
    /* Set up dying gasp handler */
    kerSysRegisterDyingGaspHandler(root_sw->dev->name, &enet_switch_power_off, root_sw->dev);

#if defined(CONFIG_BCM_LOG)
    bcmFun_reg(BCM_FUN_ID_ENET_IS_WAN_PORT, bcmenet_is_wan_port);
    bcmFun_reg(BCM_FUN_ID_ENET_LAG_PORT_GET, bcmenet_lag_port_get);
    bcmFun_reg(BCM_FUN_ID_ENET_IS_BONDED_LAN_WAN_PORT, bcmenet_is_bonded_lan_wan_port);
#if defined(CONFIG_BCM_KERNEL_BONDING)
    bcmFun_reg(BCM_FUN_ID_ENET_MAX_BONDS, bcmenet_get_max_bonds);
    bcmFun_reg(BCM_FUN_ID_ENET_BONDING_CHANGE, bcmenet_handle_bonding_change);
#endif /* CONFIG_BCM_KERNEL_BONDING */
#endif

#ifdef CONFIG_BLOG
   blog_eth_get_tx_mark_fn = bcmenet_get_xmit_mark;
#endif

#if defined(CONFIG_BCM_KF_NETFILTER)
    /* Register bridge notifier hooks */
    register_bridge_stp_notifier(&br_stp_handler);
    register_bridge_notifier(&br_notifier);
#endif

    rc = enet_proc_init();

exit:
    if (rc)
    {
        enet_err("Failed to inititialize, exiting\n");
        bcm_enet_exit();
        BUG();
    }

    return rc;
}

#ifdef DT
extern struct of_device_id enetxapi_of_platform_enet_table[];

static struct platform_driver enetxapi_of_platform_enet_driver = {
    .driver = {
        .name = "of_bcmenet",
        .of_match_table = enetxapi_of_platform_enet_table,
    },
    .probe = enetxapi_of_platform_enet_probe,
    .remove = enetxapi_of_platform_enet_remove,
};

module_platform_driver(enetxapi_of_platform_enet_driver);
#else
module_init(bcm_enet_init);
#endif

MODULE_DESCRIPTION("BCM internal ethernet network driver");
MODULE_LICENSE("GPL");

