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
 ****************************************************************************
 * File Name  : bcm_i2c.c
 *
 * Description: This file contains the platform dependent cod for detecting 
 *    and adding i2c device to the system
 ***************************************************************************/

#include <linux/types.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/i2c.h>
#include <linux/delay.h>
#include <linux/workqueue.h>
#include <linux/kthread.h>
#include <linux/sched.h>

#include <bcm_intr.h>
#include <boardparms.h>
#include <board.h>
#include <bcmsfp_i2c.h>

#include <linux/proc_fs.h>
#include <linux/seq_file.h>

#define MAX_I2C_ADDR         8

#define BCM_I2C_DEBUG
#ifdef BCM_I2C_DEBUG
#define BCM_I2C_LOG(fmt, args...) printk("bcm_i2c: %s " fmt,  __FUNCTION__, ##args)
#else
#define BCM_I2C_LOG(fmt, args...)
#endif

/* Work around for BCM96858XREF boardid, used by both SFF and SFP modes: OpticalModulePresence should not be set in SFF */
#ifdef CONFIG_BCM96858
#define SFFXREF6858WA
#endif

enum test_module {
    test_sfp_sff,
    test_sfp_sff_pmd,
};

struct bcm_i2c_platform_data {
    struct i2c_client* i2c_clients[MAX_I2C_ADDR];
    /* sfp related stuff */
    int sfp_status;
    unsigned short sfp_intr_num;
    unsigned short sfp_mod_abs_gpio;
    int sfp_polling;
    int sfp_intr;
} bcm_i2c_platform_data;

struct sfp_work {
	struct delayed_work dwork;
	int    bus;
} sfp_work;

static struct bcm_i2c_platform_data bcm_i2c_data[MAX_I2C_BUS];

static struct i2c_board_info sfp_board_info[MAX_SFP_I2C_ADDR]  = {
    {   /* SFP EEPROM client */
        I2C_BOARD_INFO("sfp_eeprom", SFP_I2C_EEPROM_ADDR),
    },
    {  	/* SFP diagnostic & monitor cient */
        I2C_BOARD_INFO("sfp_diag", SFP_I2C_DIAG_ADDR),
    },
    {	/* SFP Eth PHY client*/
	I2C_BOARD_INFO("sfp_phy", SFP_I2C_PHY_ADDR),
    },
};

#if defined(CONFIG_BCM_PON) || defined(CONFIG_BCM963158)
static struct i2c_board_info pmd_board_info[MAX_PMD_I2C_ADDR]  = {
    {   /* pmd reg client */
        I2C_BOARD_INFO("pmd_reg", PMD_I2C_REG_ADDR),
    },
    {  	/* SFP diagnostic & monitor cient */
        I2C_BOARD_INFO("pmd_iram", PMD_I2C_IRAM_ADDR),
    },
    {	/* SFP client on address 0x52, not sure what it is*/
	I2C_BOARD_INFO("pmd_dram", PMD_I2C_DRAM_ADDR),
    },
};
#endif

static struct sfp_work sfp_delay_work[MAX_I2C_BUS];

static struct blocking_notifier_head sfp_notifier;
static DECLARE_RWSEM(sfp_sts_sem);

static int probe_i2c_client(int bus, enum test_module test);

static int gponOpticsType = -1;

static int remove_sfp_i2c_client(int bus)
{
    int i, removed = 0;
    struct bcm_i2c_platform_data* pdata = &bcm_i2c_data[bus];

    for (i = 0; i < MAX_SFP_I2C_ADDR; i++)
    {
        if (pdata->i2c_clients[i])
        {
            removed = 1;
            i2c_unregister_device(pdata->i2c_clients[i]);
        }

        pdata->i2c_clients[i] = NULL;
    }

    if (removed)
    {
        down_write(&sfp_sts_sem);
        pdata->sfp_status = SFP_STATUS_REMOVED;
        up_write(&sfp_sts_sem);

        blocking_notifier_call_chain(&sfp_notifier, SFP_STATUS_REMOVED, (void*)((uintptr_t)&bus));
    }

    return 0;
}

static int is_sfp_plugin(unsigned short gpio)
{
    int gpioActHigh, plugin = 0;
    unsigned int value;

    gpioActHigh = gpio & BP_ACTIVE_LOW ? 0 : 1;
    value = kerSysGetGpioValue(gpio);

    if ((value&&gpioActHigh) || (!value&&!gpioActHigh))
        plugin = 1;

    return plugin;
}

static void check_sfp_status(int bus)
{
    struct bcm_i2c_platform_data *pdata = &bcm_i2c_data[bus];

    if (is_sfp_plugin(pdata->sfp_mod_abs_gpio))
    {
        if (pdata->sfp_polling && pdata->sfp_status == SFP_STATUS_INSERTED)
            return;

        if (pdata->sfp_status != SFP_STATUS_REMOVED)
            BUG();

        BCM_I2C_LOG("sfp plugged in on bus %d, add clients...\n", bus);
        probe_i2c_client(bus, test_sfp_sff);
    }
    else
    {
        if (pdata->sfp_polling && pdata->sfp_status == SFP_STATUS_REMOVED)
            return;

        if (pdata->sfp_status != SFP_STATUS_INSERTED)
            BUG();
        
        BCM_I2C_LOG("sfp plugged out on bus %d, remove clients...\n", bus);
        remove_sfp_i2c_client(bus);
    }
}

static void sfp_cb(struct work_struct *work_arg)
{
    struct delayed_work *delay_work;
    struct sfp_work *sfp_delay_work;

    delay_work = container_of(work_arg, struct delayed_work, work);
    sfp_delay_work = container_of(delay_work, struct sfp_work, dwork);

    check_sfp_status(sfp_delay_work->bus);
}

static irqreturn_t sfp_isr(int irq, void *arg)
{
    int bus = (int)((uintptr_t)arg);
    
    /* BCM_I2C_LOG("sfp_isr interrupt called irq %d for i2c bus %d!\n", irq, bus); */
    if( bus < MAX_I2C_BUS )   
        schedule_delayed_work(&sfp_delay_work[bus].dwork, HZ/3); 
    
#if !defined(CONFIG_ARM) && !defined(CONFIG_ARM64)
    BcmHalInterruptEnable(irq);
#else
    BcmHalExternalIrqClear(irq);
#endif

    return IRQ_HANDLED;
}

static int _probe(struct i2c_adapter * adap, unsigned short addr, char buf[], int len)
{
    struct i2c_msg msg[2];
    int ret, try = 0;

    while (try < 3)
    {
        msg[0].addr = msg[1].addr = addr;
        msg[0].flags = msg[1].flags = 0;

        msg[0].len = 1;
        buf[0] = 0x0; /* Offset to read from */
        msg[0].buf = buf;

        msg[1].flags |= I2C_M_RD;
        msg[1].len = len;
        msg[1].buf = buf;

        ret = i2c_transfer(adap, msg, 2);
        /* BCM_I2C_LOG("try %d own probe for client addr 0x%x ret %d reg 0 0x%x\n", try, addr, ret, buf[0]); */
        if (ret == 2)
            break;

        try++;
        msleep(50);
    }

    return ret == 2;
}

static int sfp_probe(struct i2c_adapter * adap, unsigned short addr)
{
    char buf[2];
    int len;

    if (addr == SFP_I2C_PHY_ADDR)
        len = 0x2; /* PHY Register is 16 bit */
    else
        len = 0x1;

    return _probe(adap, addr, buf, len);
}

#if defined(CONFIG_BCM_PON) || defined(CONFIG_BCM963158)
static int pmd_probe(struct i2c_adapter * adap, unsigned short addr)
{
#define PMD_DEV_MATCH_LEN 4
    char buf[PMD_DEV_MATCH_LEN], pmd_device[][PMD_DEV_MATCH_LEN] = {
        { 0x08, 0x68, 0x90, 0x10 },
        { 0x08, 0x68, 0x91, 0x00 },
        {},
    };
    int i = 0;

    if (!_probe(adap, addr, buf, sizeof(buf)))
        return 0;

    do
    {
        if (!memcmp(pmd_device[i], buf, sizeof(buf)))
        {
            gponOpticsType = BCM_I2C_PON_OPTICS_TYPE_PMD;
            return 1;
        }
    } while (*pmd_device[++i]);

    return 0;
}
#endif

static int probe_sfp_sff(struct i2c_adapter *i2c_adap, struct bcm_i2c_platform_data* pdata)
{
    int i, found = 0;
#ifdef SFFXREF6858WA
    int plugged_in = 0;
    static int first = 1;

    if (first)
        plugged_in = is_sfp_plugin(pdata->sfp_mod_abs_gpio);
#endif

    for (i = 0; i < MAX_SFP_I2C_ADDR && sfp_board_info[i].addr; i++)
    {
        pdata->i2c_clients[i] = i2c_new_probed_device(i2c_adap, &sfp_board_info[i], I2C_ADDRS(sfp_board_info[i].addr), sfp_probe);
        if (pdata->i2c_clients[i])
        {
            found = 1;
            BCM_I2C_LOG("i2c device at address 0x%x detected\n", sfp_board_info[i].addr);

#ifdef SFFXREF6858WA
            if (first && !plugged_in)
                pdata->sfp_intr = 0;
#endif
        }
    }

#ifdef SFFXREF6858WA
    first = 0;
#endif

    return found;
}

static int probe_pmd(struct i2c_adapter *i2c_adap, struct bcm_i2c_platform_data* pdata)
{
#if defined(CONFIG_BCM_PON) || defined(CONFIG_BCM963158)
    unsigned short rstn;

    /* Probe only if entry exists in BP */
    if (BpGetGpioPmdReset(&rstn) == BP_SUCCESS) {
        pdata->i2c_clients[0] = i2c_new_probed_device(i2c_adap, &pmd_board_info[0], I2C_ADDRS(pmd_board_info[0].addr), pmd_probe);
        if (pdata->i2c_clients[0])
        {
            pdata->i2c_clients[1] = i2c_new_device(i2c_adap, &pmd_board_info[1]);
            pdata->i2c_clients[2] = i2c_new_device(i2c_adap, &pmd_board_info[2]);
            BCM_I2C_LOG("PMD i2c device added.\n");
            return 1;
        }
    }
#endif

    return 0;
}

static int probe_i2c_client(int bus, enum test_module test)
{
    int found = 0;
    struct i2c_adapter *i2c_adap;
    struct bcm_i2c_platform_data* pdata = &bcm_i2c_data[bus];

    if (pdata->i2c_clients[0])
        BUG();

    i2c_adap = i2c_get_adapter(bus);
    if (!i2c_adap)
        return -1;

    if (test == test_sfp_sff_pmd)
        found = probe_pmd(i2c_adap, pdata);

    if (!found)
        found = probe_sfp_sff(i2c_adap, pdata);
    
    i2c_put_adapter(i2c_adap);

    if (found)
    {
        down_write(&sfp_sts_sem);
        pdata->sfp_status = SFP_STATUS_INSERTED;
        up_write(&sfp_sts_sem);
    
        blocking_notifier_call_chain(&sfp_notifier, SFP_STATUS_INSERTED, (void*)((uintptr_t)&bus));
    }

    return 0;
}

#ifdef CONFIG_BP_PHYS_INTF
static int find_sfp_i2c_bus_in_intf(int bus, unsigned short intf_type)
{
    int i, intf_num;
    unsigned short mgmt_type, mgmt_bus_num;

    if((intf_num = BpGetPhyIntfNumByType(intf_type))) {
        for( i = 0; i < intf_num; i++ ) {
            if( BpGetIntfMgmtType(intf_type, i, &mgmt_type ) == BP_SUCCESS && mgmt_type == BP_INTF_MGMT_TYPE_I2C ) {
                if( BpGetIntfMgmtBusNum(intf_type, i, &mgmt_bus_num) == BP_SUCCESS && mgmt_bus_num == bus ) {
                    return i; 
	        }
            }
        }
    }

    return -1;
}
#else
#define BP_INTF_TYPE_xPON 1 /* Dummy value so code below will have less #ifdef */
#endif

static int get_i2c_bus_num(int idx)
{
#ifdef CONFIG_BP_PHYS_INTF
    unsigned short bus = 0;
    if( BpGetIntfPortNum(BP_INTF_TYPE_I2C, idx, &bus) != BP_SUCCESS ) {
        BCM_I2C_LOG("No port/bus number define i2c interface %d!!!\n", idx);
        return 0;
    }
    return (int)bus;
#else
    return idx;
#endif
}

static int get_num_of_i2c_bus(void) 
{
    int num_i2c_bus;

#ifdef CONFIG_BP_PHYS_INTF
    num_i2c_bus = BpGetPhyIntfNumByType(BP_INTF_TYPE_I2C);
#else
    /* assume only one i2c bus for the old style board parameter */
    num_i2c_bus = 1;
#endif

    return num_i2c_bus;
}

static int sfp_polling_func(void * arg) 
{
    int i, num_i2c_bus, bus;
    struct bcm_i2c_platform_data* pdata;

    num_i2c_bus = get_num_of_i2c_bus();
    while(1)
    {
        for (i = 0; i < num_i2c_bus; i++)
        {
            bus = get_i2c_bus_num(i);
            if( bus >= MAX_I2C_BUS )
                continue;

            pdata = &bcm_i2c_data[bus];
            if (pdata->sfp_polling)
                check_sfp_status(bus);
        }

        msleep(300);
    }

    return 0;
}

static int create_poll_thread;

static int create_sfp_polling_thread(void)
{
    struct task_struct *thread;

    thread = kthread_run(sfp_polling_func, NULL, "sfp_polling");
    if (!thread)
    {
        BCM_I2C_LOG("Failed to polling create thread.\n");
        return -1;
	}
   
    return 0;
}

static int prepare_pmd_sff_detection(int bus)
{
#if defined(CONFIG_BCM_PON) || defined(CONFIG_BCM963158)
    unsigned short intf_type, txen, rstn;
#ifdef CONFIG_BP_PHYS_INTF
    int intf_idx;
    
    if (bcm_i2c_sfp_get_intf(bus, &intf_type, &intf_idx) != 0)
        return -1;
#else
    intf_type = BP_INTF_TYPE_xPON;
#endif

    /* Some SFP need VDDT to power on */
    if (intf_type != BP_INTF_TYPE_xPON)
        return 0;

    if (BpGetPonTxEnGpio(&txen) == BP_SUCCESS )
    {
        kerSysSetGpioDir(txen);
        kerSysSetGpioState(txen, kGpioActive);
    }

    /* PMD device need taken out of reset */
    if (BpGetGpioPmdReset(&rstn) == BP_SUCCESS)
    {
        kerSysSetGpioDir(rstn);
        kerSysSetGpioState(rstn, kGpioActive);
        kerSysSetGpioState(rstn, kGpioInactive);
    }
#endif

    return 0;
}

static int prepare_sfp_detection(int bus)
{
    struct bcm_i2c_platform_data *pdata = &bcm_i2c_data[bus];
    unsigned short intf_type;
#ifdef CONFIG_BP_PHYS_INTF
    int intf_idx;
#endif

#ifdef CONFIG_BP_PHYS_INTF
    if (bcm_i2c_sfp_get_intf(bus, &intf_type, &intf_idx) != 0)
        return 0;
        
    BCM_I2C_LOG("i2c bus %d used by intf type %d idx %d\n", bus, intf_type, intf_idx);
    /* check if SFP use interrupt */
    if (BpGetOpticalModulePresenceExtIntr(intf_type, intf_idx, &pdata->sfp_intr_num) == BP_SUCCESS && pdata->sfp_intr_num != BP_EXT_INTR_NONE) {
#else
    intf_type = BP_INTF_TYPE_xPON;
    if (BpGetOpticalModulePresenceExtIntr(&pdata->sfp_intr_num) == BP_SUCCESS && pdata->sfp_intr_num != BP_EXT_INTR_NONE) {
#endif
#ifdef CONFIG_BP_PHYS_INTF
       if( BpGetOpticalModulePresenceExtIntrGpio(intf_type, intf_idx, &pdata->sfp_mod_abs_gpio) == BP_SUCCESS && pdata->sfp_mod_abs_gpio != BP_GPIO_NONE) {
#else
       if( BpGetOpticalModulePresenceExtIntrGpio(&pdata->sfp_mod_abs_gpio) == BP_SUCCESS && pdata->sfp_mod_abs_gpio != BP_GPIO_NONE ) {
#endif
            create_poll_thread = pdata->sfp_intr = 1;
            BCM_I2C_LOG("i2c bus %d sfp detection using interrupt on gpio %d\n", bus, pdata->sfp_mod_abs_gpio&BP_GPIO_NUM_MASK);
        }
    } else {
        /* check if it has sfp mod_abs gpio pin for polling */
#ifdef CONFIG_BP_PHYS_INTF
        if (BpGetSfpModDetectGpio(intf_type, intf_idx, &pdata->sfp_mod_abs_gpio) == BP_SUCCESS ) {
#else
        if (BpGetSfpDetectGpio(&pdata->sfp_mod_abs_gpio) == BP_SUCCESS ) {
#endif
            BCM_I2C_LOG("i2c bus %d sfp detection using polling\n", bus);
            create_poll_thread = pdata->sfp_polling = 1;
        }
    }

    if (pdata->sfp_polling || pdata->sfp_intr)
    {
        kerSysSetGpioDirInput(pdata->sfp_mod_abs_gpio);
        if (is_sfp_plugin(pdata->sfp_mod_abs_gpio))
            BCM_I2C_LOG("sfp plugged in when power on, add clients...\n");
    }

    /* if not using interrupt/polling but board has xPON optical module soldered down, then
       interface is always present, assuming only have one xPON or SGMII interface in the board */
    if (pdata->sfp_intr == 0 && pdata->sfp_polling == 0 && intf_type == BP_INTF_TYPE_xPON)
        BCM_I2C_LOG("i2c bus %d interface soldered on board...\n", bus);

    if (pdata->sfp_intr)
    {
        INIT_DELAYED_WORK(&sfp_delay_work[bus].dwork, sfp_cb);
        sfp_delay_work[bus].bus = bus;
        ext_irq_connect(pdata->sfp_intr_num, (void *)((uintptr_t)bus), (FN_HANDLER)sfp_isr);
    }

    return 0;
}

#if defined(CONFIG_BCM_PON) || defined(CONFIG_BCM963158)
static int proc_show(struct seq_file *m, void *v)
{
    seq_printf(m, "%d\n", gponOpticsType);
    return 0;
}

static int proc_open(struct inode *inode, struct  file *file)
{
      return single_open(file, proc_show, NULL);
}

static const struct file_operations proc_fops = {
    .owner = THIS_MODULE,
    .open = proc_open,
    .read = seq_read,
    .llseek = seq_lseek,
    .release = single_release,
};

static __init int create_proc_entry(void)
{
    if (!proc_create(BCM_I2C_PROC_DIR, 0, NULL, &proc_fops))
    {
        BCM_I2C_LOG("Failed to create proc entry\n");
        return -ENOMEM;
    }

    return 0;
}
#endif

static __init int bcm_add_i2c(void)
{
    int ret = -1, i, num_i2c_bus, bus;

    BLOCKING_INIT_NOTIFIER_HEAD(&sfp_notifier);
    
#if defined(CONFIG_BCM_PON) || defined(CONFIG_BCM963158)
    gponOpticsType = BCM_I2C_PON_OPTICS_TYPE_LEGACY;

    if (create_proc_entry())
        goto exit;
#endif

    num_i2c_bus = get_num_of_i2c_bus();
    for (i = 0; i < num_i2c_bus; i++)
    {
        bus = get_i2c_bus_num(i);
        if( bus >= MAX_I2C_BUS )
            continue;

        if (prepare_sfp_detection(bus))
            goto exit;

        if (prepare_pmd_sff_detection(bus))
            goto exit;

        if (probe_i2c_client(bus, test_sfp_sff_pmd))
            goto exit;
    }

    ret = 0;
    /* Polling thread only if mechanism to detect SFP is supported */
    if (create_poll_thread)
        ret = create_sfp_polling_thread();

exit:
    if (ret)
        BCM_I2C_LOG("Error in loading module\n");
    
    return ret;
}

int bcm_i2c_sfp_register_notifier(struct notifier_block* nb)
{
    return blocking_notifier_chain_register(&sfp_notifier, nb);
}
EXPORT_SYMBOL(bcm_i2c_sfp_register_notifier);

int bcm_i2c_sfp_unregister_notifier(struct notifier_block* nb)
{
    return blocking_notifier_chain_unregister(&sfp_notifier, nb);
}
EXPORT_SYMBOL(bcm_i2c_sfp_unregister_notifier);

int bcm_i2c_sfp_get_status(int bus)
{
    int status = SFP_STATUS_INVALID;

    if( bus < MAX_I2C_BUS )  {
        down_read(&sfp_sts_sem);
        status = bcm_i2c_data[bus].sfp_status;
        up_read(&sfp_sts_sem);
    }

    return status;
}
EXPORT_SYMBOL(bcm_i2c_sfp_get_status);

#ifdef CONFIG_BP_PHYS_INTF
/*Given the interface type and index, find the i2c bus number */
int bcm_i2c_sfp_get_bus_num(unsigned short intf_type, int intf_idx, int *bus)
{
    int ret = -1;
    unsigned short mgmt_type, bus_num;

    if( BpGetIntfMgmtType(intf_type, intf_idx, &mgmt_type ) == BP_SUCCESS && 
         mgmt_type == BP_INTF_MGMT_TYPE_I2C ) 
    {
        if( BpGetIntfMgmtBusNum(intf_type, intf_idx, &bus_num) == BP_SUCCESS ) {
            *bus = bus_num;
            ret = 0;
        }
    }

    return ret;
}
EXPORT_SYMBOL(bcm_i2c_sfp_get_bus_num);

/*Given the i2c bus number, find interface type and index */
int bcm_i2c_sfp_get_intf(int bus, unsigned short* intf_type, int* intf_idx)
{
    /* Only xPON, SGMII interface has sfp */
    *intf_idx = find_sfp_i2c_bus_in_intf(bus, BP_INTF_TYPE_xPON);
    if (*intf_idx != - 1)
    {
        *intf_type = BP_INTF_TYPE_xPON;
        return 0;
    }

    *intf_idx = find_sfp_i2c_bus_in_intf(bus, BP_INTF_TYPE_SGMII);
    if (*intf_idx != - 1)
    {
        *intf_type = BP_INTF_TYPE_SGMII;
        return 0;
    }

    return -1;
}
EXPORT_SYMBOL(bcm_i2c_sfp_get_intf);
#endif

int bcm_i2c_pon_optics_type_get(unsigned short *pusValue)
{
    *pusValue = gponOpticsType;
    if (gponOpticsType == -1)
        return BP_VALUE_NOT_DEFINED;

    return BP_SUCCESS;
}
EXPORT_SYMBOL(bcm_i2c_pon_optics_type_get);

static void bcm_remove_i2c(void)
{
    int bus;

#if defined(CONFIG_BCM_PON) || defined(CONFIG_BCM963158)
    remove_proc_entry(BCM_I2C_PROC_DIR, NULL);
#endif

    for (bus = 0; bus < MAX_I2C_BUS; bus++)
        remove_sfp_i2c_client(bus);
}

module_init(bcm_add_i2c);
module_exit(bcm_remove_i2c);

MODULE_LICENSE("GPL");
