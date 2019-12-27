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

#include "port.h"
#include "phy_drv.h"
#include "enet.h"
#include "crossbar_dev.h"
#include <proc_cmd.h>
extern int phy_devices_internal_index(phy_dev_t *phy_dev);
extern int mac_devices_internal_index(mac_dev_t *mac_dev);

static const char *port_type_string[] =
{
    FOREACH_PORT_TYPE(GENERATE_STRING)
};

#define PROC_DIR           "driver/enet"
#define CMD_PROC_FILE      "cmd"

static struct proc_dir_entry *proc_dir;
static struct proc_dir_entry *cmd_proc_file;



#ifdef ENET_DEBUG_RX_CPU_TRAFFIC
#define DEBUG_MODE_PROC_FILE        "debug_mode"

static struct proc_dir_entry *debug_cmd_proc_file;

#ifdef CONFIG_ARM64
uint64_t g_debug_mode_pckt_rx;
#else
uint32_t g_debug_mode_pckt_rx;
#endif

uint32_t g_debug_mode;
struct timeval g_start_time;
struct timeval g_end_time;

int debug_mode_set(int argc, char *argv[])
{
    char *usage = "Usage: set_debug_mode <[<mode 0|1>]";
    unsigned int debug_mode = 0xffffffff;
    int rc = 0;

    if (argc < 2)
        goto wrong_params;

    if (kstrtos32(argv[1], 10, &debug_mode))
        goto wrong_params;


    if (debug_mode > 1)
        goto wrong_params; 
    
    
    g_debug_mode = debug_mode;

    printk("\x1b[32m DEBUG_MODE[%d]: \x1b[0m\n",g_debug_mode);

    return rc;


wrong_params:
    pr_info("%s\n", usage);
    return rc;

}

int debug_mode_get_pps(int argc, char *argv[])
{
    char *usage = "Usage: get_pps";
    int rc = 0;
#ifdef CONFIG_ARM64
    uint64_t time_diff;
#else
    uint32_t time_diff;
#endif
    
    if (argc < 1)
        goto wrong_params;
    
    do_gettimeofday(&g_end_time);
    
    time_diff = (g_end_time.tv_sec - g_start_time.tv_sec) + (g_end_time.tv_usec - g_start_time.tv_usec) / 1000000;

#ifdef CONFIG_ARM64
    printk("\x1b[32m DEBUG_MODE: pckt_rx[%lld] end_time[%ldsec] start_time[%ldsec] PPS[%lld]\x1b[0m\n", 
#else
    printk("\x1b[32m DEBUG_MODE: pckt_rx[%d] end_time[%ldsec] start_time[%ldsec] PPS[%d]\x1b[0m\n", 
#endif
           g_debug_mode_pckt_rx,
           g_end_time.tv_sec,
           g_start_time.tv_sec,
           g_debug_mode_pckt_rx / time_diff);

    memcpy(&g_start_time, &g_end_time, sizeof(struct timespec)); 
    g_debug_mode_pckt_rx = 0;
    
    return rc;                      


wrong_params:
    pr_info("%s\n", usage);
    return rc;
}

static struct proc_cmd_ops debug_mode_command_entries[] = 
{
    { .name = "set_debug_mode",  .do_command = debug_mode_set },
    { .name = "get_pps_measure", .do_command = debug_mode_get_pps},
};

static struct proc_cmd_table debug_mode_command_table = 
{
    .module_name = "debug_mode_masters",
    .size = sizeof(debug_mode_command_entries) / sizeof(debug_mode_command_entries[0]),
    .ops = debug_mode_command_entries
};
#endif

static int port_proc_cmd_status(int argc, char *argv[])
{
    int id;

    if (argc < 2)
        goto Error;

    if (kstrtos32(argv[1], 10, &id) || id < 0 || id >= 18)
        goto Error;

    /* XXX: Do something with port here */

    return 0;

Error:
    printk("Usage: status <id>\n");
    return 0;
}

int crossbar_info_by_phy(phy_dev_t *phy_dev, int *crossbar_id, int *internal_endpoint, int *external_endpoint);

static int tr_port_list(enetx_port_t *p, void *_ctx)
{
    char *class_str;
    int mac_internal_index, ext_ep, int_ep, phy_internal_index;
    
    switch (p->port_class)
    {
        case PORT_CLASS_SW: class_str = "SW"; break;
        case PORT_CLASS_PORT: class_str = "PORT"; break;
        case PORT_CLASS_PORT_UNDEF: class_str = "UNDEF"; break;
        case PORT_CLASS_PORT_DETECT: class_str = "DTCT"; break;
        default: class_str = "ERR";
    }

    printk("| %4s ", class_str);
    printk("| %9s  ", p->obj_name);
    printk("| %7s  ", p->dev ? p->dev->name : "-");
    printk("| %s     ", !p->is_init ? "y" : " ");
    printk("| %13s  ", port_type_string[p->port_type] + sizeof("PORT_TYPE_") - 1);

    if (p->port_class == PORT_CLASS_PORT ||
        p->port_class == PORT_CLASS_PORT_UNDEF ||
        p->port_class == PORT_CLASS_PORT_DETECT)
    {
        printk("| %9s  ", p->p.parent_sw ? p->p.parent_sw->obj_name : "");
        printk("| %02d  ", p->p.port_id);
        
        if (p->p.phy)
        {
            if (phy_is_crossbar(p->p.phy))
            {
                crossbar_info_by_phy(get_active_phy(p->p.phy), NULL, &ext_ep, &int_ep);
                    printk("| I%02d ", int_ep);
            }
            else
            {
                phy_internal_index = phy_devices_internal_index(p->p.phy);
                if (phy_internal_index >= 0)
                    printk("| %02d  ", phy_internal_index);
            }
        }
        else
            printk("| %2s  ", "");
        
        mac_internal_index = p->p.mac ? mac_devices_internal_index(p->p.mac) : -1;
        if (mac_internal_index >= 0)
            printk("| %02d  ", mac_internal_index);
        else
            printk("| %2s  ", "");

        if (p->p.ops->print_priv)
            printk("| %17s  ", p->p.ops->print_priv(p));
        else
            printk("| %17s  ", "");
    }
    if (p->port_class == PORT_CLASS_SW)
    {
        printk("| %9s  ", p->s.parent_port ? p->s.parent_port->obj_name : "");
        printk("| %2s  ", "");
        printk("| %2s  ", "");
        printk("| %2s  ", "");
        if (p->s.ops->print_priv)
            printk("| %17s  ", p->s.ops->print_priv(p));
        else
            printk("| %17s  ", "");
    }

    printk("|\n");

    return 0;
}

static int port_proc_cmd_list(int argc, char *argv[])
{
    printk("|===========================================================================================================|\n");
    printk("| Clss |  Name      | Dev Name | !Init |  Type          |  Parent    |PrtID| PHY | MAC | Extra              |\n");
    printk("|===========================================================================================================|\n");
        
    port_traverse_ports(root_sw, tr_port_list, PORT_CLASS_SW | PORT_CLASS_PORT | PORT_CLASS_PORT_DETECT | PORT_CLASS_PORT_UNDEF, NULL);

    printk("|===========================================================================================================|\n");

    return 0;
}
  
static struct proc_cmd_ops command_entries[] = {
    { .name = "status", .do_command = port_proc_cmd_status},
    { .name = "list",   .do_command = port_proc_cmd_list},
};

static struct proc_cmd_table port_command_table = {
    .module_name = "ENET",
    .size = ARRAY_SIZE(command_entries),
    .ops = command_entries,
};

void enet_proc_exit(void)
{
    if (cmd_proc_file) 
    {
        remove_proc_entry(CMD_PROC_FILE, proc_dir);
        cmd_proc_file = NULL;
    }	
#ifdef ENET_DEBUG_RX_CPU_TRAFFIC
    if (debug_cmd_proc_file) 
    {
        remove_proc_entry(DEBUG_MODE_PROC_FILE, proc_dir);
        debug_cmd_proc_file = NULL;
    }
#endif
    if (proc_dir)
    {
        remove_proc_entry(PROC_DIR, NULL);
        proc_dir = NULL;
    }	
}

int __init enet_proc_init(void)
{
    proc_dir = proc_mkdir(PROC_DIR, NULL);
    if (!proc_dir) 
    {
        pr_err("Failed to create PROC directory %s.\n",
            PROC_DIR);
        goto error;
    }

    cmd_proc_file = proc_create_cmd(CMD_PROC_FILE, proc_dir,
        &port_command_table);
    if (!cmd_proc_file) 
    {
        pr_err("Failed to create %s\n", CMD_PROC_FILE);
        goto error;
    }

#ifdef ENET_DEBUG_RX_CPU_TRAFFIC
    debug_cmd_proc_file = proc_create_cmd(DEBUG_MODE_PROC_FILE, proc_dir, &debug_mode_command_table);
    if (!debug_cmd_proc_file) 
    {
        pr_err("Failed to create %s\n", DEBUG_MODE_PROC_FILE);
        goto error;
    }

    g_debug_mode = 0;
    g_debug_mode_pckt_rx = 0;
    do_gettimeofday(&g_start_time);
#endif

    return 0;

error:
    if (proc_dir)
        enet_proc_exit();

    return -EIO;
}

