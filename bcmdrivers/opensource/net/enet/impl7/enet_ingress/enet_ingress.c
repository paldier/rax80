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
 *  Created on: Apr/2017
 *      Author: ido@broadcom.com
 */
#include <linux/module.h>
#include <linux/etherdevice.h>
#include <linux/bcm_assert_locks.h>
#include "enet.h"
#include "enet_dbg.h"
#include "rdpa_api.h"
#include <linux/kthread.h>

int enetx_weight_budget = 0;
static enetx_channel *enetx_channels;

typedef struct enetx_port_t *p;

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

netdev_tx_t enet_xmit(pNBuff_t pNBuff, uint32_t port_id, uint32_t flow_id)
{
    rdpa_cpu_tx_info_t info = {};

    info.method = rdpa_cpu_tx_ingress;
    info.port = port_id;
    info.cpu_port = rdpa_cpu_host;
    info.drop_precedence = 0;
    info.flags = 0;

    if (rdpa_if_is_wan(port_id))
        info.x.wan.flow = flow_id;

    return rdpa_cpu_send_sysb(pNBuff, &info);
}

extern struct sk_buff *skb_header_alloc(void);
static inline int rx_skb(FkBuff_t *fkb, enetx_rx_info_t *rx_info)
{
    struct sk_buff *skb;

    skb = skb_header_alloc();
    if (unlikely(!skb))
    {
        enet_err("SKB allocation failure\n");
        return -1;
    }
    skb_headerinit((BCM_PKT_HEADROOM + rx_info->data_offset), 
#if defined(CC_NBUFF_FLUSH_OPTIMIZATION)
            SKB_DATA_ALIGN(fkb->len + BCM_SKB_TAILROOM + rx_info->data_offset),
#else
            BCM_MAX_PKT_LEN - rx_info->data_offset,
#endif
            skb, (uint8_t *)fkb->data, (RecycleFuncP)enetxapi_buf_recycle,(unsigned long) 0, fkb->blog_p);

    skb_trim(skb,fkb->len);

    skb->recycle_flags &= SKB_NO_RECYCLE; /* no skb recycle,just do data recyle */
    skb->recycle_flags |= rx_info->extra_skb_flags;

    skb->priority = fkb->priority;

    /* Do work here !
     * i.e learn MAC; etc
     */

    enet_xmit(skb, rx_info->src_port, rx_info->flow_id);

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
    FkBuff_t *fkb;
    enetx_rx_info_t rx_info;

    do
    {
        rc = enetxapi_rx_pkt(hw_q_id, &fkb, &rx_info);
        if (unlikely(rc))
            continue;

        rc = rx_skb(fkb, &rx_info);

        count++; 
    }
    while (count < budget && likely(!rc));

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
        wait_event_interruptible(chan->rxq_wqh, chan->rxq_cond);
        if (kthread_should_stop())
            break;

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

static void __exit_refok bcm_enet_exit(void)
{
    enetxapi_queues_uninit(&enetx_channels);
}
module_exit(bcm_enet_exit);

static int enet_open(void)
{
    enetx_channel *chan = enetx_channels;

    while (chan)
    {
        int i;
        for (i = 0; i < chan->rx_q_count; i++)
            enetxapi_queue_int_enable(chan, i);

        chan = chan->next;
    }

    return 0;
}

static int filter_cfg(void)
{
    rdpa_filter_global_cfg_t global_cfg;
    bdmf_object_handle filter;

    if (rdpa_filter_get(&filter))
        return -1;

    rdpa_filter_global_cfg_get(filter, &global_cfg);
    global_cfg.cpu_bypass = 1;
    rdpa_filter_global_cfg_set(filter, &global_cfg);

    bdmf_put(filter);

    return 0;
}

int __init bcm_enet_init(void)
{
    int rc = -1;

    if (BCM_SKB_ALIGNED_SIZE != skb_aligned_size())
    {
        enet_err("skb_aligned_size mismatch. Need to recompile enet module\n");
        return -ENOMEM;
    }

    if (filter_cfg())
        goto exit;

    if (enetxapi_queues_init(&enetx_channels))
        goto exit;

    enet_open();
    rc = 0;

exit:
    if (rc)
    {
        enet_err("Failed to inititialize, exiting\n");
        bcm_enet_exit();
    }

    return rc;
}

module_init(bcm_enet_init);

MODULE_DESCRIPTION("BCM internal cpu_tx_ingress testing network driver");
MODULE_LICENSE("GPL");

void phy_link_change_cb(void *ctx)
{
}

void enet_remove_netdevice(enetx_port_t *p)
{
}

int enet_create_netdevice(enetx_port_t *p)
{
    return 0;
}

void enet_dev_role_update(enetx_port_t *self)
{
}

void dynamic_meters_init(bdmf_object_handle cpu_obj, int watch_qid)
{
}

void dynamic_meters_uninit(bdmf_object_handle cpu_obj)
{
}

void ptp_1588_uninit(void)
{
}

int ptp_1588_init(void)
{
    return 0;
}

int is_pkt_ptp_1588(pNBuff_t pNBuff, rdpa_cpu_tx_info_t *info, char **ptp_offset)
{
    return 0;
}

int ptp_1588_cpu_send_sysb(bdmf_sysb sysb, rdpa_cpu_tx_info_t *info, char *ptp_header)
{
    return 0;
}

int enet_dev_mac_set(enetx_port_t *p, int set)
{
    return 0;
}

void enet_update_pbvlan_all_bridge(void)
{
}

void enet_dev_flags_update(enetx_port_t *self)
{
}

