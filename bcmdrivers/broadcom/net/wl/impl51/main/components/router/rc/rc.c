/*
 * Router rc control script
 *
 * Copyright (C) 2018, Broadcom. All Rights Reserved.
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 *
 * <<Broadcom-WL-IPTag/Open:>>
 *
 * $Id: rc.c 766522 2018-08-06 05:05:21Z $
 */

#ifndef LINUX_VERSION_CODE
#include <linux/version.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>
#include <syslog.h>
#include <signal.h>
#include <fcntl.h> /* for open */
#include <string.h>
#include <sys/klog.h>
#include <sys/types.h>
#include <sys/mount.h>
#include <sys/reboot.h>
#include <sys/stat.h>
#include <sys/sysmacros.h>
#include <sys/time.h>
#include <sys/utsname.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/utsname.h> /* for uname */
#include <net/if_arp.h>
#include <dirent.h>

#include <epivers.h>
#include <router_version.h>
#include <mtd.h>
#include <shutils.h>
#include <rc.h>
#include <netconf.h>
#include <nvparse.h>
#include <bcmdevs.h>
#include <bcmparams.h>
#include <bcmnvram.h>
#include <wlutils.h>
#include <ezc.h>
#include <pmon.h>
#include <bcmconfig.h>
#include <confmtd_utils.h>
#include <linux/version.h>
#include "ambitCfg.h"
#ifdef BCA_HNDROUTER
#include "ambitCfg.h"
#include "rc_patch.h"
#endif

#if defined(__CONFIG_WAPI__) || defined(__CONFIG_WAPI_IAS__)
#include <wapi_path.h>
#endif // endif
#if defined(__CONFIG_CIFS__)
#include <cifs_path.h>
#endif // endif

#ifdef __BRCM_GENERIC_IQOS__
#include "bcmIqosDef.h"
#endif
#include <etutils.h>

#ifdef BCA_HNDROUTER
#include <cms_image.h>
#include <bcm_imgif.h>
#include <bcm/bcmswapitypes.h>
#include <bcmnet.h>
#endif /* BCA_HNDROUTER */

/* foxconn added start, zacker, 09/17/2009, @wps_led */
#include <fcntl.h>
#include <wps_led.h>
/* foxconn added end, zacker, 09/17/2009, @wps_led */

/*fxcn added by dennis start,05/03/2012, fixed guest network can't reconnect issue*/
#define MAX_BSSID_NUM       4
#define MIN_BSSID_NUM       2
/*fxcn added by dennis end,05/03/2012, fixed guest network can't reconnect issue*/

#ifdef __CONFIG_NAT__
static void auto_bridge(void);
#endif	/* __CONFIG_NAT__ */

#include <sys/sysinfo.h> /* foxconn wklin added */
#ifdef __CONFIG_EMF__
extern void load_emf(void);
#endif /* __CONFIG_EMF__ */

#ifdef __CONFIG_DHDAP__
#define MAX_FW_PATH	512
#endif /* __CONFIG_DHDAP__ */

static void restore_defaults(void);
static void sysinit(void);
static void rc_signal(int sig);
#ifdef VLAN_SUPPORT
int getVlanRule(vlan_rule vlan[C_MAX_VLAN_RULE]);/* Foxconn modified by Max Ding, 11/22/2016 remove static, need use it in interface.c */
#endif
/*Foxconn add end, edward zhang, 2013/07/03*/
#ifdef BCA_HNDROUTER
static int nvram_erase();
static int bca_sys_upgrade(const char *path);
static int foxconn_sys_upgrade(const char *path, int size, int offset);
#endif /* BCA_HNDROUTER */

extern struct nvram_tuple router_defaults[];

extern void iqos_restore_defaults(void);



#define RESTORE_DEFAULTS() \
	(!nvram_match("restore_defaults", "0") || nvram_invmatch("os_name", "linux"))

#ifdef LINUX_2_6_36
static int
coma_uevent(void)
{
	char *modalias = NULL;
	char lan_ifname[32], *lan_ifnames, *next;

	modalias = getenv("MODALIAS");
	if (!strcmp(modalias, "platform:coma_dev")) {

		/* down WiFi adapter */
		lan_ifnames = nvram_safe_get("lan_ifnames");
		foreach(lan_ifname, lan_ifnames, next) {
			if (!strncmp(lan_ifname, "eth", 3)) {
				eval("wl", "-i", lan_ifname, "down");
			}
		}

		system("echo \"2\" > /proc/bcm947xx/coma");
	}
	return 0;
}
#endif /* LINUX_2_6_36 */

#if !defined(BCA_HNDROUTER)
static int
build_ifnames(char *type, char *names, int *size)
{
	char name[32], *next;
	int len = 0;
	int s;

	/* open a raw scoket for ioctl */
	if ((s = socket(AF_INET, SOCK_RAW, IPPROTO_RAW)) < 0)
		return -1;

	/*
	 * go thru all device names (wl<N> il<N> et<N> vlan<N>) and interfaces to
	 * build an interface name list in which each i/f name coresponds to a device
	 * name in device name list. Interface/device name matching rule is device
	 * type dependant:
	 *
	 *	wl:	by unit # provided by the driver, for example, if eth1 is wireless
	 *		i/f and its unit # is 0, then it will be in the i/f name list if
	 *		wl0 is in the device name list.
	 *	il/et:	by mac address, for example, if et0's mac address is identical to
	 *		that of eth2's, then eth2 will be in the i/f name list if et0 is
	 *		in the device name list.
	 *	vlan:	by name, for example, vlan0 will be in the i/f name list if vlan0
	 *		is in the device name list.
	 */
	foreach(name, type, next) {
		struct ifreq ifr;
		int i, unit;
		char var[32], *mac;
		unsigned char ea[ETHER_ADDR_LEN];

		/* vlan: add it to interface name list */
		if (!strncmp(name, "vlan", 4)) {
			/* append interface name to list */
			len += snprintf(&names[len], *size - len, "%s ", name);
			continue;
		}

		/* others: proceed only when rules are met */
		for (i = 1; i <= DEV_NUMIFS; i ++) {
			/* ignore i/f that is not ethernet */
			ifr.ifr_ifindex = i;
			if (ioctl(s, SIOCGIFNAME, &ifr))
				continue;
			if (ioctl(s, SIOCGIFHWADDR, &ifr))
				continue;
			if (ifr.ifr_hwaddr.sa_family != ARPHRD_ETHER)
				continue;
			if (!strncmp(ifr.ifr_name, "vlan", 4))
				continue;

			/* wl: use unit # to identify wl */
			if (!strncmp(name, "wl", 2)) {
				if (wl_probe(ifr.ifr_name) ||
				    wl_ioctl(ifr.ifr_name, WLC_GET_INSTANCE, &unit, sizeof(unit)) ||
				    unit != atoi(&name[2]))
					continue;
			}
			/* et/il: use mac addr to identify et/il */
			else if (!strncmp(name, "et", 2) || !strncmp(name, "il", 2)) {
				snprintf(var, sizeof(var), "%smacaddr", name);
				if (!(mac = nvram_get(var)) || !ether_atoe(mac, ea) ||
				    bcmp(ea, ifr.ifr_hwaddr.sa_data, ETHER_ADDR_LEN))
					continue;
			}
			/* mac address: compare value */
			else if (ether_atoe(name, ea) &&
				!bcmp(ea, ifr.ifr_hwaddr.sa_data, ETHER_ADDR_LEN))
				;
			/* others: ignore */
			else
				continue;

			/* append interface name to list */
			len += snprintf(&names[len], *size - len, "%s ", ifr.ifr_name);
		}
	}

	close(s);

	*size = len;
	return 0;
}
#else /* BCA_HNDROUTER */
/*****************************************************************************
*  FUNCTION:  build_ifnames
*  PURPOSE:   Go through all "ethx" net device interfaces and trying to find
*             wheather any of their names are matching to one of the names
*             from input list "types". If the match occur then add net device
*             interface name to the list "names".
*  PARAMETERS:
*      types (IN) - The list of the interface names for comparing with current
*                   existing net device interfaces in the system.
*      names (OUT) - The list of existing net device interfaces which matched
*                    to the names from "type".
*      size (IN/OUT) - (IN) The length of the list "type".
*                      (OUT) The length of the output list "names".
*
*  RETURNS:
*      0 - succeeded.
*      -1 - failed.
*  NOTES:
*****************************************************************************/
static int
build_ifnames(char *type, char *names, int *size)
{
	char name[32], *next;
	int len = 0;
	int s;
	char *et0macaddr;
	unsigned char ea[ETHER_ADDR_LEN];

	/* Get Ethernet switch base MAC address from NVRAM */
	if (!(et0macaddr = nvram_get("et0macaddr")) ||
		!ether_atoe(et0macaddr, ea))
		return -1;

	/* open a raw scoket for ioctl */
	if ((s = socket(AF_INET, SOCK_RAW, IPPROTO_RAW)) < 0)
		return -1;

	foreach(name, type, next) {
		struct ifreq ifr;
		int i, unit;

		for (i = 1; i <= DEV_NUMIFS; i ++) {
			/* ignore i/f that is not ethernet type ifc */
			ifr.ifr_ifindex = i;
			if (ioctl(s, SIOCGIFNAME, &ifr))
				continue;
			if (ioctl(s, SIOCGIFHWADDR, &ifr))
				continue;
			if (ifr.ifr_hwaddr.sa_family != ARPHRD_ETHER)
				continue;

			/* Identifying whether this is Wireless interface. */
			if (!strncmp(name, "wl", 2)) {
				unit = -1;
				if (!wl_probe(ifr.ifr_name) &&
				    !wl_ioctl(ifr.ifr_name, WLC_GET_INSTANCE, &unit, sizeof(unit)) &&
				    unit == atoi(&name[2])) {
					/* append interface name to the list */
					len += snprintf(&names[len], *size - len, "%s ", ifr.ifr_name);
				}
			}

			/* Identifying whether this is Ethernet interface. */
			if (!strncmp(name, "et", 2) && !strncmp(ifr.ifr_name, "eth", 3)) {
				/* "ifr.ifr_name" format is "ethX", where X=[1,2,3,4] */
				unit = atoi(&ifr.ifr_name[3]);
				if ((unit == atoi(&name[2])) && !bcmp(ea, ifr.ifr_hwaddr.sa_data, ETHER_ADDR_LEN)) {
					/* append interface name to the list. */
					len += snprintf(&names[len], *size - len, "%s ", ifr.ifr_name);
				}
			}
		}
	}

	close(s);

	*size = len;
	return 0;
}
#endif /* BCA_HNDROUTER */

#ifdef __CONFIG_WPS__
static void
wps_restore_defaults(void)
{
	/* cleanly up nvram for WPS */
	nvram_unset("wps_config_state");
	nvram_unset("wps_device_pin");
	nvram_unset("wps_proc_status");
	nvram_unset("wps_sta_pin");
	nvram_unset("wps_restart");
	nvram_unset("wps_config_method");
}
#endif /* __CONFIG_WPS__ */

static void
virtual_radio_restore_defaults(void)
{
	char tmp[100], prefix[] = "wlXXXXXXXXXXXXXXXXXXXXXXXXXXXX_mssid_";
	int i, j;

	nvram_unset("unbridged_ifnames");
	nvram_unset("ure_disable");

	/* Delete dynamically generated variables */
	for (i = 0; i < MAX_NVPARSE; i++) {
		sprintf(prefix, "wl%d_", i);
		nvram_unset(strcat_r(prefix, "vifs", tmp));
		nvram_unset(strcat_r(prefix, "ssid", tmp));
		nvram_unset(strcat_r(prefix, "guest", tmp));
		nvram_unset(strcat_r(prefix, "ure", tmp));
		nvram_unset(strcat_r(prefix, "ipconfig_index", tmp));
		nvram_unset(strcat_r(prefix, "nas_dbg", tmp));
		sprintf(prefix, "lan%d_", i);
		nvram_unset(strcat_r(prefix, "ifname", tmp));
		nvram_unset(strcat_r(prefix, "ifnames", tmp));
		nvram_unset(strcat_r(prefix, "gateway", tmp));
		nvram_unset(strcat_r(prefix, "proto", tmp));
		nvram_unset(strcat_r(prefix, "ipaddr", tmp));
		nvram_unset(strcat_r(prefix, "netmask", tmp));
		nvram_unset(strcat_r(prefix, "lease", tmp));
		nvram_unset(strcat_r(prefix, "stp", tmp));
		nvram_unset(strcat_r(prefix, "hwaddr", tmp));
		sprintf(prefix, "dhcp%d_", i);
		nvram_unset(strcat_r(prefix, "start", tmp));
		nvram_unset(strcat_r(prefix, "end", tmp));

		/* clear virtual versions */
		for (j = 0; j < 16; j++) {
			sprintf(prefix, "wl%d.%d_", i, j);
			nvram_unset(strcat_r(prefix, "ssid", tmp));
			nvram_unset(strcat_r(prefix, "ipconfig_index", tmp));
			nvram_unset(strcat_r(prefix, "guest", tmp));
			nvram_unset(strcat_r(prefix, "closed", tmp));
			nvram_unset(strcat_r(prefix, "wpa_psk", tmp));
			nvram_unset(strcat_r(prefix, "auth", tmp));
			nvram_unset(strcat_r(prefix, "wep", tmp));
			nvram_unset(strcat_r(prefix, "auth_mode", tmp));
			nvram_unset(strcat_r(prefix, "crypto", tmp));
			nvram_unset(strcat_r(prefix, "akm", tmp));
			nvram_unset(strcat_r(prefix, "hwaddr", tmp));
			nvram_unset(strcat_r(prefix, "bss_enabled", tmp));
			nvram_unset(strcat_r(prefix, "bss_maxassoc", tmp));
			nvram_unset(strcat_r(prefix, "wme_bss_disable", tmp));
			nvram_unset(strcat_r(prefix, "ifname", tmp));
			nvram_unset(strcat_r(prefix, "unit", tmp));
			nvram_unset(strcat_r(prefix, "ap_isolate", tmp));
			nvram_unset(strcat_r(prefix, "macmode", tmp));
			nvram_unset(strcat_r(prefix, "maclist", tmp));
			nvram_unset(strcat_r(prefix, "maxassoc", tmp));
			nvram_unset(strcat_r(prefix, "mode", tmp));
			nvram_unset(strcat_r(prefix, "radio", tmp));
			nvram_unset(strcat_r(prefix, "radius_ipaddr", tmp));
			nvram_unset(strcat_r(prefix, "radius_port", tmp));
			nvram_unset(strcat_r(prefix, "radius_key", tmp));
			nvram_unset(strcat_r(prefix, "key", tmp));
			nvram_unset(strcat_r(prefix, "key1", tmp));
			nvram_unset(strcat_r(prefix, "key2", tmp));
			nvram_unset(strcat_r(prefix, "key3", tmp));
			nvram_unset(strcat_r(prefix, "key4", tmp));
			nvram_unset(strcat_r(prefix, "wpa_gtk_rekey", tmp));
			nvram_unset(strcat_r(prefix, "nas_dbg", tmp));
			nvram_unset(strcat_r(prefix, "probresp_mf", tmp));

			nvram_unset(strcat_r(prefix, "bss_opmode_cap_reqd", tmp));
			nvram_unset(strcat_r(prefix, "mcast_regen_bss_enable", tmp));
			nvram_unset(strcat_r(prefix, "wmf_bss_enable", tmp));
			nvram_unset(strcat_r(prefix, "wmf_ucigmp_query", tmp));
			nvram_unset(strcat_r(prefix, "wmf_mcast_data_sendup", tmp));
			nvram_unset(strcat_r(prefix, "wmf_psta_disable", tmp));
			nvram_unset(strcat_r(prefix, "wmf_igmpq_filter", tmp));
			nvram_unset(strcat_r(prefix, "preauth", tmp));
			nvram_unset(strcat_r(prefix, "dwds", tmp));
			nvram_unset(strcat_r(prefix, "acs_dfsr_deferred", tmp));
			nvram_unset(strcat_r(prefix, "wet_tunnel", tmp));
			nvram_unset(strcat_r(prefix, "bridge", tmp));
			nvram_unset(strcat_r(prefix, "mfp", tmp));
			nvram_unset(strcat_r(prefix, "acs_dfsr_activity", tmp));
			nvram_unset(strcat_r(prefix, "acs_dfsr_immediate", tmp));
			nvram_unset(strcat_r(prefix, "wme", tmp));
			nvram_unset(strcat_r(prefix, "net_reauth", tmp));
			nvram_unset(strcat_r(prefix, "sta_retry_time", tmp));
			nvram_unset(strcat_r(prefix, "infra", tmp));

#ifdef __CONFIG_HSPOT__
			nvram_unset(strcat_r(prefix, "hsflag", tmp));
			nvram_unset(strcat_r(prefix, "hs2cap", tmp));
			nvram_unset(strcat_r(prefix, "opercls", tmp));
			nvram_unset(strcat_r(prefix, "anonai", tmp));
			nvram_unset(strcat_r(prefix, "wanmetrics", tmp));
			nvram_unset(strcat_r(prefix, "oplist", tmp));
			nvram_unset(strcat_r(prefix, "homeqlist", tmp));
			nvram_unset(strcat_r(prefix, "osu_ssid", tmp));
			nvram_unset(strcat_r(prefix, "osu_frndname", tmp));
			nvram_unset(strcat_r(prefix, "osu_uri", tmp));
			nvram_unset(strcat_r(prefix, "osu_nai", tmp));
			nvram_unset(strcat_r(prefix, "osu_method", tmp));
			nvram_unset(strcat_r(prefix, "osu_icons", tmp));
			nvram_unset(strcat_r(prefix, "osu_servdesc", tmp));
			nvram_unset(strcat_r(prefix, "concaplist", tmp));
			nvram_unset(strcat_r(prefix, "qosmapie", tmp));
			nvram_unset(strcat_r(prefix, "gascbdel", tmp));
			nvram_unset(strcat_r(prefix, "iwnettype", tmp));
			nvram_unset(strcat_r(prefix, "hessid", tmp));
			nvram_unset(strcat_r(prefix, "ipv4addr", tmp));
			nvram_unset(strcat_r(prefix, "ipv6addr", tmp));
			nvram_unset(strcat_r(prefix, "netauthlist", tmp));
			nvram_unset(strcat_r(prefix, "venuegrp", tmp));
			nvram_unset(strcat_r(prefix, "venuetype", tmp));
			nvram_unset(strcat_r(prefix, "venuelist", tmp));
			nvram_unset(strcat_r(prefix, "ouilist", tmp));
			nvram_unset(strcat_r(prefix, "3gpplist", tmp));
			nvram_unset(strcat_r(prefix, "domainlist", tmp));
			nvram_unset(strcat_r(prefix, "realmlist", tmp));
#endif /* __CONFIG_HSPOT__ */

#if defined(__CONFIG_VISUALIZATION__) && defined(CONFIG_VISUALIZATION_ENABLED)
		nvram_unset(strcat_r(prefix, "rrm", tmp));
#endif /* __CONFIG_VISUALIZATION__ && CONFIG_VISUALIZATION_ENABLED */
		}
	}
}

static void
set_psr_vars(int unit)
{
	int wl_unit = unit;
	int max_no_vifs = 0;
	int i;
	char *name;
	char *next = NULL;
	char cap[WLC_IOCTL_SMLEN];
	char caps[WLC_IOCTL_MEDLEN];
	char nv_param[NVRAM_MAX_PARAM_LEN];
	char nv_value[NVRAM_MAX_VALUE_LEN];
	char nv_interface[NVRAM_MAX_PARAM_LEN];

	/* Set the mode */
	sprintf(nv_param, "wl%d_mode", wl_unit);
	nvram_set(nv_param, "psr");
	/* Set the second radio as AP */
	sprintf(nv_param, "wl%d_mode", (wl_unit + 1));
	nvram_set(nv_param, "ap");

	nvram_set("wl_nband", "1");
	nvram_set("wl_phytype", "v");

	/* Get the number of VIFS */
	sprintf(nv_interface, "wl%d_ifname", wl_unit);
	name = nvram_safe_get(nv_interface);
	if (wl_iovar_get(name, "cap", (void *)caps, sizeof(caps))) {
		return;
	}

	sprintf(nv_interface, "wl%d_vifs", wl_unit);
	sprintf(nv_value, "wl%d.1", wl_unit);
	nvram_set(nv_interface, nv_value);

	foreach(cap, caps, next) {
		if (!strcmp(cap, "mbss16"))
			max_no_vifs = 16;
		else if (!strcmp(cap, "mbss4"))
			max_no_vifs = 4;
	}

	/* Enable the primary VIF */
	sprintf(nv_param, "wl%d.1_bss_enabled", wl_unit);
	nvram_set(nv_param, "1");

	/* Disable all VIFS wlX.2 onwards */
	for (i = 2; i < max_no_vifs; i++) {
		sprintf(nv_param, "wl%d.%d_bss_enabled", wl_unit, i);
		nvram_set(nv_param, "0");
	}

	/* Commit nvram variables */
	nvram_commit();
}

#ifdef __CONFIG_URE__
static void
ure_restore_defaults(int unit)
{
	char tmp[100], prefix[] = "wlXXXXXXXXXX_";
	struct nvram_tuple *t;

	sprintf(prefix, "wl%d.1_", unit);

	for (t = router_defaults; t->name; t++) {
		if (!strncmp(t->name, "wl_", 3)) {
			strcat_r(prefix, &t->name[3], tmp);
			nvram_unset(tmp);
		}
	}

	sprintf(prefix, "wl%d_ure", unit);
	nvram_unset(prefix);
}

static void
set_ure_vars(int unit)
{
	int wl_unit = unit;
	int travel_router;
	char prefix[] = "wlXXXXXXXXXX_";
	struct nvram_tuple *t;
	char nv_param[NVRAM_MAX_PARAM_LEN];
	char nv_value[NVRAM_MAX_VALUE_LEN];
	char nv_interface[NVRAM_MAX_PARAM_LEN];
	char os_interface[NVRAM_MAX_PARAM_LEN];
	int os_interface_size = sizeof(os_interface);
	char *temp = NULL;
	char interface_list[NVRAM_MAX_VALUE_LEN];
	int interface_list_size = sizeof(interface_list);
	char *wan0_ifname = "wan0_ifname";
	char *lan_ifnames = "lan_ifnames";
	char *wan_ifnames = "wan_ifnames";

	sprintf(prefix, "wl%d.1_", wl_unit);

	/* Clone default wl nvram settings to wl0.1 */
	for (t = router_defaults; t->name; t++) {
		if (!strncmp(t->name, "wl_", 3)) {
			strcat_r(prefix, &t->name[3], nv_param);
			nvram_set(nv_param, t->value);
		}
	}

	/* Overwrite some specific nvram settings */
	sprintf(nv_param, "wl%d_ure", wl_unit);
	nvram_set(nv_param, "1");

	sprintf(nv_param, "wl%d_vifs", wl_unit);
	sprintf(nv_value, "wl%d.1", wl_unit);
	nvram_set(nv_param, nv_value);

	sprintf(nv_param, "wl%d.1_unit", wl_unit);
	sprintf(nv_value, "%d.1", wl_unit);
	nvram_set(nv_param, nv_value);

	nvram_set("wl_ampdu_rr_rtylimit_tid", "3 3 3 3 3 3 3 3");
	nvram_set("wl_ampdu_rtylimit_tid", "7 7 7 7 7 7 7 7");
	sprintf(nv_param, "wl%d_ampdu_rr_rtylimit_tid", wl_unit);
	nvram_set(nv_param, "3 3 3 3 3 3 3 3");
	sprintf(nv_param, "wl%d_ampdu_rtylimit_tid", wl_unit);
	nvram_set(nv_param, "7 7 7 7 7 7 7 7");

	if (nvram_match("router_disable", "1"))
		travel_router = 0;
	else
		travel_router = 1;

	/* Set the wl modes for the primary wireless adapter
	 * and it's virtual interface
	 */
	sprintf(nv_param, "wl%d_mode", wl_unit);
	if (travel_router == 1) {
		nvram_set(nv_param, "sta");
		nvram_set("wl_mode", "sta");
	}
	else {
		nvram_set(nv_param, "wet");
		nvram_set("wl_mode", "wet");
	}

	sprintf(nv_param, "wl%d.1_mode", wl_unit);
	nvram_set(nv_param, "ap");

	if (travel_router == 1) {
		/* For URE with routing (Travel Router) we're using the STA part
		 * of our URE enabled radio as our WAN connection. So, we need to
		 * remove this interface from the list of bridged lan interfaces
		 * and set it up as the WAN device.
		 */
		temp = nvram_safe_get(lan_ifnames);
		strncpy(interface_list, temp, interface_list_size);

		/* Leverage build_ifnames() to get OS-dependent interface name.
		 * One white space is appended after build_ifnames(); need to
		 * remove it.
		 */
		sprintf(nv_interface, "wl%d", wl_unit);
		memset(os_interface, 0, os_interface_size);
		build_ifnames(nv_interface, os_interface, &os_interface_size);
		if (strlen(os_interface) > 1) {
			os_interface[strlen(os_interface) - 1] = '\0';
		}
		remove_from_list(os_interface, interface_list, interface_list_size);
		nvram_set(lan_ifnames, interface_list);

		/* Now remove the existing WAN interface from "wan_ifnames" */
		temp = nvram_safe_get(wan_ifnames);
		strncpy(interface_list, temp, interface_list_size);

		temp = nvram_safe_get(wan0_ifname);
		if (strlen(temp) != 0) {
			/* Stash this interface name in an nvram variable in case
			 * we need to restore this interface as the WAN interface
			 * when URE is disabled.
			 */
			nvram_set("old_wan0_ifname", temp);
			remove_from_list(temp, interface_list, interface_list_size);
		}

		/* Set the new WAN interface as the pimary WAN interface and add to
		 * the list wan_ifnames.
		 */
		nvram_set(wan0_ifname, os_interface);
		add_to_list(os_interface, interface_list, interface_list_size);
		nvram_set(wan_ifnames, interface_list);

		/* Now add the AP to the list of bridged lan interfaces */
		temp = nvram_safe_get(lan_ifnames);
		strncpy(interface_list, temp, interface_list_size);
		sprintf(nv_interface, "wl%d.1", wl_unit);

		/* Virtual interfaces that appear in NVRAM lists are ALWAYS stored
		 * as the NVRAM_FORM so we can add to list without translating.
		 */
		add_to_list(nv_interface, interface_list, interface_list_size);
		nvram_set(lan_ifnames, interface_list);
	}
	else {
		/* For URE without routing (Range Extender) we're using the STA
		 * as a WET device to connect to the "upstream" AP. We need to
		 * add our virtual interface(AP) to the bridged lan.
		 */
		temp = nvram_safe_get(lan_ifnames);
		strncpy(interface_list, temp, interface_list_size);

		sprintf(nv_interface, "wl%d.1", wl_unit);
		add_to_list(nv_interface, interface_list, interface_list_size);
		nvram_set(lan_ifnames, interface_list);
	}

	/* Make lan1_ifname, lan1_ifnames empty so that br1 is not created in URE mode. */
	nvram_set("lan1_ifname", "");
	nvram_set("lan1_ifnames", "");

	if (nvram_match("wl0_ure_mode", "wre")) {
		/* By default, wl0 bss is disabled in WRE mode */
		nvram_set("wl_bss_enabled", "0");
		nvram_set("wl0_bss_enabled", "0");
	}
}
#endif /* __CONFIG_URE__ */

#ifdef __CONFIG_NAT__
static void
auto_bridge(void)
{

	struct nvram_tuple generic[] = {
		{ "lan_ifname", "br0", 0 },
#ifdef BCA_HNDROUTER
#ifndef NO_ETHWAN
		{ "lan_ifnames", "eth1 eth2 eth3 eth4 eth5 eth6 eth7 eth8", 0 },
		{ "wan_ifname", "eth0", 0 },
		{ "wan_ifnames", "eth0", 0 },
#else
		{ "lan_ifnames", "eth0 eth1 eth2 eth3", 0 },
		{ "wan_ifname", "", 0 },
		{ "wan_ifnames", "", 0 },
#endif /* NO_ETHWAN */
#else
		{ "lan_ifnames", "eth0 eth2 eth3 eth4 eth5", 0 },
		{ "wan_ifname", "eth1", 0 },
		{ "wan_ifnames", "eth1", 0 },
#endif /* BCA_HNDROUTER */
		{ 0, 0, 0 }
	};
#ifdef __CONFIG_VLAN__
	struct nvram_tuple vlan[] = {
		{ "lan_ifname", "br0", 0 },
		{ "lan_ifnames", "vlan0 eth1 eth2 eth3", 0 },
		{ "wan_ifname", "vlan1", 0 },
		{ "wan_ifnames", "vlan1", 0 },
		{ 0, 0, 0 }
	};
#endif	/* __CONFIG_VLAN__ */
	struct nvram_tuple dyna[] = {
		{ "lan_ifname", "br0", 0 },
		{ "lan_ifnames", "", 0 },
		{ "wan_ifname", "", 0 },
		{ "wan_ifnames", "", 0 },
		{ 0, 0, 0 }
	};
	struct nvram_tuple generic_auto_bridge[] = {
		{ "lan_ifname", "br0", 0 },
		{ "lan_ifnames", "eth0 eth1 eth2 eth3 eth4 eth5", 0 },
		{ "wan_ifname", "", 0 },
		{ "wan_ifnames", "", 0 },
		{ 0, 0, 0 }
	};
#ifdef __CONFIG_VLAN__
	struct nvram_tuple vlan_auto_bridge[] = {
		{ "lan_ifname", "br0", 0 },
		{ "lan_ifnames", "vlan0 vlan1 eth1 eth2 eth3", 0 },
		{ "wan_ifname", "", 0 },
		{ "wan_ifnames", "", 0 },
		{ 0, 0, 0 }
	};
#endif	/* __CONFIG_VLAN__ */

	struct nvram_tuple dyna_auto_bridge[] = {
		{ "lan_ifname", "br0", 0 },
		{ "lan_ifnames", "", 0 },
		{ "wan_ifname", "", 0 },
		{ "wan_ifnames", "", 0 },
		{ 0, 0, 0 }
	};

	struct nvram_tuple *linux_overrides;
	struct nvram_tuple *t, *u;
	int auto_bridge = 0, i;
#ifdef __CONFIG_VLAN__
	uint boardflags;
#endif	/* __CONFIG_VLAN_ */
	char *landevs, *wandevs;
	char lan_ifnames[128], wan_ifnames[128];
	char dyna_auto_ifnames[128];
	char wan_ifname[32], *next;
	int len;
	int ap = 0;

	printf(" INFO : enter function auto_bridge()\n");

	if (!strcmp(nvram_safe_get("auto_bridge_action"), "1")) {
		auto_bridge = 1;
		cprintf("INFO: Start auto bridge...\n");
	} else {
		nvram_set("router_disable_auto", "0");
		cprintf("INFO: Start non auto_bridge...\n");
	}

	/* Delete dynamically generated variables */
	if (auto_bridge) {
		char tmp[100], prefix[] = "wlXXXXXXXXXX_";
		for (i = 0; i < MAX_NVPARSE; i++) {

			del_filter_client(i);
			del_forward_port(i);
#if !defined(AUTOFW_PORT_DEPRECATED)
			del_autofw_port(i);
#endif // endif

			snprintf(prefix, sizeof(prefix), "wan%d_", i);
			for (t = router_defaults; t->name; t ++) {
				if (!strncmp(t->name, "wan_", 4))
					nvram_unset(strcat_r(prefix, &t->name[4], tmp));
			}
		}
	}

	/*
	 * Build bridged i/f name list and wan i/f name list from lan device name list
	 * and wan device name list. Both lan device list "landevs" and wan device list
	 * "wandevs" must exist in order to preceed.
	 */
	if ((landevs = nvram_get("landevs")) && (wandevs = nvram_get("wandevs"))) {
		/* build bridged i/f list based on nvram variable "landevs" */
		len = sizeof(lan_ifnames);
		if (!build_ifnames(landevs, lan_ifnames, &len) && len)
			dyna[1].value = lan_ifnames;
		else
			goto canned_config;
		/* build wan i/f list based on nvram variable "wandevs" */
		len = sizeof(wan_ifnames);
		if (!build_ifnames(wandevs, wan_ifnames, &len) && len) {
			dyna[3].value = wan_ifnames;
			foreach(wan_ifname, wan_ifnames, next) {
				dyna[2].value = wan_ifname;
				break;
			}
		}
		else
			ap = 1;

		if (auto_bridge)
		{
			printf("INFO: lan_ifnames=%s\n", lan_ifnames);
			printf("INFO: wan_ifnames=%s\n", wan_ifnames);
			sprintf(dyna_auto_ifnames, "%s %s", lan_ifnames, wan_ifnames);
			printf("INFO: dyna_auto_ifnames=%s\n", dyna_auto_ifnames);
			dyna_auto_bridge[1].value = dyna_auto_ifnames;
			linux_overrides = dyna_auto_bridge;
			printf("INFO: linux_overrides=dyna_auto_bridge \n");
		}
		else
		{
			linux_overrides = dyna;
			printf("INFO: linux_overrides=dyna \n");
		}

	}
	/* override lan i/f name list and wan i/f name list with default values */
	else {
canned_config:
#ifdef __CONFIG_VLAN__
		boardflags = strtoul(nvram_safe_get("boardflags"), NULL, 0);
		if (boardflags & BFL_ENETVLAN) {
			if (auto_bridge)
			{
				linux_overrides = vlan_auto_bridge;
				printf("INFO: linux_overrides=vlan_auto_bridge \n");
			}
			else
			{
				linux_overrides = vlan;
				printf("INFO: linux_overrides=vlan \n");
			}
		} else {
#endif	/* __CONFIG_VLAN__ */
			if (auto_bridge)
			{
				linux_overrides = generic_auto_bridge;
				printf("INFO: linux_overrides=generic_auto_bridge \n");
			}
			else
			{
				linux_overrides = generic;
				printf("INFO: linux_overrides=generic \n");
			}
#ifdef __CONFIG_VLAN__
		}
#endif	/* __CONFIG_VLAN__ */
	}

		for (u = linux_overrides; u && u->name; u++) {
			nvram_set(u->name, u->value);
			printf("INFO: action nvram_set %s, %s\n", u->name, u->value);
			}

	/* Force to AP */
	if (ap)
		nvram_set("router_disable", "1");

	if (auto_bridge) {
		printf("INFO: reset auto_bridge flag.\n");
		nvram_set("auto_bridge_action", "0");
	}

	nvram_commit();
	cprintf("auto_bridge done\n");
}

#endif	/* __CONFIG_NAT__ */

static void
upgrade_defaults(void)
{
	char temp[100];
	int i;
	bool bss_enabled = TRUE;
	char *val;

	/* Check whether upgrade is required or not
	 * If lan1_ifnames is not found in NVRAM , upgrade is required.
	 */
	if (!nvram_get("lan1_ifnames") && !RESTORE_DEFAULTS()) {
		cprintf("NVRAM upgrade required.  Starting.\n");

		if (nvram_match("ure_disable", "1")) {
			nvram_set("lan1_ifname", "br1");
			nvram_set("lan1_ifnames", "wl0.1 wl0.2 wl0.3 wl1.1 wl1.2 wl1.3");
		}
		else {
			nvram_set("lan1_ifname", "");
			nvram_set("lan1_ifnames", "");
			for (i = 0; i < 2; i++) {
				snprintf(temp, sizeof(temp), "wl%d_ure", i);
				if (nvram_match(temp, "1")) {
					snprintf(temp, sizeof(temp), "wl%d.1_bss_enabled", i);
					nvram_set(temp, "1");
				}
				else {
					bss_enabled = FALSE;
					snprintf(temp, sizeof(temp), "wl%d.1_bss_enabled", i);
					nvram_set(temp, "0");
				}
			}
		}
		if (nvram_get("lan1_ipaddr")) {
			nvram_set("lan1_gateway", nvram_get("lan1_ipaddr"));
		}

		for (i = 0; i < 2; i++) {
			snprintf(temp, sizeof(temp), "wl%d_bss_enabled", i);
			nvram_set(temp, "1");
			snprintf(temp, sizeof(temp), "wl%d.1_guest", i);
			if (nvram_match(temp, "1")) {
				nvram_unset(temp);
				if (bss_enabled) {
					snprintf(temp, sizeof(temp), "wl%d.1_bss_enabled", i);
					nvram_set(temp, "1");
				}
			}

			snprintf(temp, sizeof(temp), "wl%d.1_net_reauth", i);
			val = nvram_get(temp);
			if (!val || (*val == 0))
				nvram_set(temp, nvram_default_get(temp));

			snprintf(temp, sizeof(temp), "wl%d.1_wpa_gtk_rekey", i);
			val = nvram_get(temp);
			if (!val || (*val == 0))
				nvram_set(temp, nvram_default_get(temp));
		}

		nvram_commit();

		cprintf("NVRAM upgrade complete.\n");
	}
}

#ifdef LINUX_2_6_36
/* Override the "0 5u" to "0 5" to backward compatible with old image */
static void
fa_override_vlan2ports()
{
	char port[] = "XXXX", *nvalue;
	char *next, *cur, *ports, *u;
	int len;

	ports = nvram_get("vlan2ports");
	nvalue = malloc(strlen(ports) + 2);
	if (!nvalue) {
		cprintf("Memory allocate failed!\n");
		return;
	}
	memset(nvalue, 0, strlen(ports) + 2);

	/* search last port include 'u' */
	for (cur = ports; cur; cur = next) {
		/* tokenize the port list */
		while (*cur == ' ')
			cur ++;
		next = strstr(cur, " ");
		len = next ? next - cur : strlen(cur);
		if (!len)
			break;
		if (len > sizeof(port) - 1)
			len = sizeof(port) - 1;
		strncpy(port, cur, len);
		port[len] = 0;

		/* prepare new value */
		if ((u = strchr(port, 'u')))
			*u = '\0';
		strcat(nvalue, port);
		strcat(nvalue, " ");
	}

	/* Remove last " " */
	len = strlen(nvalue);
	if (len) {
		nvalue[len-1] = '\0';
		nvram_set("vlan2ports", nvalue);
	}
	free(nvalue);
}

static void
fa_nvram_adjust()
{
	FILE *fp;
	int fa_mode;
	bool reboot = FALSE;

	if (RESTORE_DEFAULTS())
		return;

	fa_mode = atoi(nvram_safe_get("ctf_fa_mode"));
	switch (fa_mode) {
		case CTF_FA_BYPASS:
		case CTF_FA_NORMAL:
			break;
		default:
			fa_mode = CTF_FA_DISABLED;
			break;
	}

	if ((fp = fopen("/proc/fa", "r"))) {
		/* FA is capable */
		fclose(fp);

		if (FA_ON(fa_mode)) {
			char wan_ifnames[128];
			char wan_ifname[32], *next;
			int len, ret;

			cprintf("\nFA on.\n");

			/* Set et2macaddr, et2phyaddr as same as et0macaddr, et0phyaddr */
			if (!nvram_get("vlan2ports") || !nvram_get("wandevs"))  {
				cprintf("Insufficient envram, cannot do FA override\n");
				return;
			}

			/* adjusted */
			if (!strcmp(nvram_get("wandevs"), "vlan2") &&
			    !strchr(nvram_get("vlan2ports"), 'u'))
			    return;

			/* The vlan2ports will be change to "0 8u" dynamically by
			 * robo_fa_imp_port_upd. Keep nvram vlan2ports unchange.
			 */
			fa_override_vlan2ports();

			/* Override wandevs to "vlan2" */
			nvram_set("wandevs", "vlan2");
			/* build wan i/f list based on def nvram variable "wandevs" */
			len = sizeof(wan_ifnames);
			ret = build_ifnames("vlan2", wan_ifnames, &len);
			if (!ret && len) {
				/* automatically configure wan0_ too */
				nvram_set("wan_ifnames", wan_ifnames);
				nvram_set("wan0_ifnames", wan_ifnames);
				foreach(wan_ifname, wan_ifnames, next) {
					nvram_set("wan_ifname", wan_ifname);
					nvram_set("wan0_ifname", wan_ifname);
					break;
				}
			}
			cprintf("Override FA nvram...\n");
			reboot = TRUE;
		}
		else {
			cprintf("\nFA off.\n");
		}
	}
	else {
		/* FA is not capable */
		if (FA_ON(fa_mode)) {
			nvram_unset("ctf_fa_mode");
			cprintf("FA not supported...\n");
			reboot = TRUE;
		}
	}

	if (reboot) {
		nvram_commit();
		cprintf("FA nvram overridden, rebooting...\n");
		kill(1, SIGTERM);
	}
}

#define GMAC3_ENVRAM_BACKUP(name)				\
do {								\
	char *var, bvar[NVRAM_MAX_PARAM_LEN];			\
	if ((var = nvram_get(name)) != NULL) {			\
		snprintf(bvar, sizeof(bvar), "old_%s", name);	\
		nvram_set(bvar, var);				\
	}							\
} while (0)

/* Override GAMC3 nvram */
static void
gmac3_override_nvram()
{
	char var[32], *lists, *next;
	char newlists[NVRAM_MAX_PARAM_LEN];

	/* back up old embedded nvram */
	GMAC3_ENVRAM_BACKUP("et0macaddr");
	GMAC3_ENVRAM_BACKUP("et1macaddr");
	GMAC3_ENVRAM_BACKUP("et2macaddr");
	GMAC3_ENVRAM_BACKUP("et0mdcport");
	GMAC3_ENVRAM_BACKUP("et1mdcport");
	GMAC3_ENVRAM_BACKUP("et2mdcport");
	GMAC3_ENVRAM_BACKUP("et0phyaddr");
	GMAC3_ENVRAM_BACKUP("et1phyaddr");
	GMAC3_ENVRAM_BACKUP("et2phyaddr");
	GMAC3_ENVRAM_BACKUP("vlan1ports");
	GMAC3_ENVRAM_BACKUP("vlan2ports");
	GMAC3_ENVRAM_BACKUP("vlan1hwname");
	GMAC3_ENVRAM_BACKUP("vlan2hwname");
	GMAC3_ENVRAM_BACKUP("wandevs");

	/* change mac, mdcport, phyaddr */
	nvram_set("et2macaddr", nvram_get("et0macaddr"));
	nvram_set("et2mdcport", nvram_get("et0mdcport"));
	nvram_set("et2phyaddr", nvram_get("et0phyaddr"));
	nvram_set("et1mdcport", nvram_get("et0mdcport"));
	nvram_set("et1phyaddr", nvram_get("et0phyaddr"));
	nvram_set("et0macaddr", "00:00:00:00:00:00");
	nvram_set("et1macaddr", "00:00:00:00:00:00");

	/* change vlan ports */
	if (!(lists = nvram_get("vlan1ports"))) {
		cprintf("Default vlan1ports is not specified, override GMAC3 defaults...\n");
		nvram_set("vlan1ports", "1 2 3 4 5 7 8*");
	} else {
		strncpy(newlists, lists, sizeof(newlists));
		newlists[sizeof(newlists)-1] = '\0';

		/* search first port include '*' or 'u' and remove it */
		foreach(var, lists, next) {
			if (strchr(var, '*') || strchr(var, 'u')) {
				remove_from_list(var, newlists, sizeof(newlists));
				break;
			}
		}

		/* add port 5, 7 and 8* */
		add_to_list("5", newlists, sizeof(newlists));
		add_to_list("7", newlists, sizeof(newlists));
		add_to_list("8*", newlists, sizeof(newlists));
		nvram_set("vlan1ports", newlists);
	}

	if (!(lists = nvram_get("vlan2ports"))) {
		cprintf("Default vlan2ports is not specified, override GMAC3 defaults...\n");
		nvram_set("vlan2ports", "0 8u");
	} else {
		strncpy(newlists, lists, sizeof(newlists));
		newlists[sizeof(newlists)-1] = '\0';

		/* search first port include '*' or 'u' and remove it */
		foreach(var, lists, next) {
			if (strchr(var, '*') || strchr(var, 'u')) {
				remove_from_list(var, newlists, sizeof(newlists));
				break;
			}
		}

		/* add port 8u */
		add_to_list("8u", newlists, sizeof(newlists));
		nvram_set("vlan2ports", newlists);
	}

	/* change wandevs vlan hw name */
	nvram_set("wandevs", "et2");
	nvram_set("vlan1hwname", "et2");
	nvram_set("vlan2hwname", "et2");

	/* landevs should be have wl1 wl2 */

	/* set fwd_wlandevs from lan_ifnames */
	if (!(lists = nvram_get("lan_ifnames"))) {
		/* should not be happened */
		cprintf("lan_ifnames is not exist, override GMAC3 defaults...\n");
		nvram_set("fwd_wlandevs", "eth1 eth2 eth3");
	} else {
		strncpy(newlists, lists, sizeof(newlists));
		newlists[sizeof(newlists)-1] = '\0';

		/* remove ifname if it's not a wireless interrface */
		foreach(var, lists, next) {
			if (wl_probe(var)) {
				remove_from_list(var, newlists, sizeof(newlists));
				continue;
			}
		}
		nvram_set("fwd_wlandevs", newlists);
	}

	/* set fwddevs */
	nvram_set("fwddevs", "fwd0 fwd1");
}

#define GMAC3_ENVRAM_RESTORE(name)				\
do {								\
	char *var, bvar[NVRAM_MAX_PARAM_LEN];			\
	snprintf(bvar, sizeof(bvar), "old_%s", name);		\
	if ((var = nvram_get(bvar))) {				\
		nvram_set(name, var);				\
		nvram_unset(bvar);				\
	}							\
	else {							\
		nvram_unset(name);				\
	}							\
} while (0)

static void
gmac3_restore_nvram()
{
	/* back up old embedded nvram */
	GMAC3_ENVRAM_RESTORE("et0macaddr");
	GMAC3_ENVRAM_RESTORE("et1macaddr");
	GMAC3_ENVRAM_RESTORE("et2macaddr");
	GMAC3_ENVRAM_RESTORE("et0mdcport");
	GMAC3_ENVRAM_RESTORE("et1mdcport");
	GMAC3_ENVRAM_RESTORE("et2mdcport");
	GMAC3_ENVRAM_RESTORE("et0phyaddr");
	GMAC3_ENVRAM_RESTORE("et1phyaddr");
	GMAC3_ENVRAM_RESTORE("et2phyaddr");
	GMAC3_ENVRAM_RESTORE("vlan1ports");
	GMAC3_ENVRAM_RESTORE("vlan2ports");
	GMAC3_ENVRAM_RESTORE("vlan1hwname");
	GMAC3_ENVRAM_RESTORE("vlan2hwname");
	GMAC3_ENVRAM_RESTORE("wandevs");

	nvram_unset("fwd_wlandevs");
	nvram_unset("fwddevs");
}

static void
gmac3_restore_defaults()
{
	/* nvram variables will be changed when GMAC3 enabled */
	if (!strcmp(nvram_safe_get("wandevs"), "et2") &&
	    nvram_get("fwd_wlandevs") && nvram_get("fwddevs")) {
		gmac3_restore_nvram();
	}
}

static void
gmac3_nvram_adjust()
{
	int fa_mode;
	bool reboot = FALSE;
	bool gmac3_configured = FALSE;

	/* nvram variables will be changed when GMAC3 enabled */
	if (!strcmp(nvram_safe_get("wandevs"), "et2") &&
	    nvram_get("fwd_wlandevs") &&
	    nvram_get("fwddevs"))
		gmac3_configured = TRUE;

	fa_mode = atoi(nvram_safe_get("ctf_fa_mode"));
	if (fa_mode == CTF_FA_NORMAL || fa_mode == CTF_FA_BYPASS) {
		cprintf("GMAC3 based forwarding not compatible with ETFA - AuX port...\n");
		return;
	}

	if (et_capable(NULL, "gmac3")) {
		cprintf("\nGMAC3 on.\n");

		if (gmac3_configured)
			return;

		/* The vlan2ports will be change to "0 8u" dynamically by
		 * robo_fa_imp_port_upd. Keep nvram vlan2ports unchange.
		 */
		gmac3_override_nvram();

		cprintf("Override GMAC3 nvram...\n");
		reboot = TRUE;
	} else {
		cprintf("GMAC3 not supported...\n");

		if (nvram_get("gmac3_enable")) {
			nvram_unset("gmac3_enable");
			reboot = TRUE;
		}

		/* GMAC3 is not capable */
		if (gmac3_configured) {
			gmac3_restore_nvram();
			reboot = TRUE;
		}
	}

	if (reboot) {
		nvram_commit();
		cprintf("GMAC3 nvram overridden, rebooting...\n");
		kill(1, SIGTERM);
	}
}
#endif /* LINUX_2_6_36 */

#ifdef BCA_HNDROUTER
static void
enable_ethernet_intfs(char *ifnames, bool iswan)
{
	char name[32], *next;
	int s;
	struct ifreq ifr;
	struct ethswctl_data e_data;
	char tmp[64];

	if (!ifnames)
		return;

	/* open a raw scoket for ioctl */
	if ((s = socket(AF_INET, SOCK_RAW, IPPROTO_RAW)) < 0)
		return;

	foreach(name, ifnames, next) {
		/* filter out wl interfaces */
		if (!wl_probe(name))
			continue;
		/* every ethernet interface has a specific port id and we can use it to
		 * check the interface is an ethernet interface or not
		 */
		strncpy(ifr.ifr_name, "bcmsw", sizeof(ifr.ifr_name));
		strncpy(e_data.ifname, name, sizeof(e_data.ifname));
		e_data.op = ETHSWUNITPORT;
		ifr.ifr_data = (char *)&e_data;
		if (ioctl(s, SIOCETHSWCTLOPS, &ifr))
			continue;
		if (iswan) {
			snprintf(tmp, sizeof(tmp), "ethswctl -c wan -o enable -i %s", name);
			system(tmp);
		}
		snprintf(tmp, sizeof(tmp), "tmctl porttminit --devtype 0 --if %s --flag 1", name);
		system(tmp);
	}

	close(s);
}
#endif /* BCA_HNDROUTER */

#ifdef __CONFIG_FAILSAFE_UPGRADE_SUPPORT__
static void
failsafe_nvram_adjust(void)
{
	FILE *fp;
	char dev[PATH_MAX];
	bool found = FALSE, need_commit = FALSE;
	int i, partialboots;

	partialboots = atoi(nvram_safe_get(PARTIALBOOTS));
	if (partialboots != 0) {
		nvram_set(PARTIALBOOTS, "0");
		need_commit = TRUE;
	}

	if (nvram_get(BOOTPARTITION) != NULL) {
		/* get mtdblock device */
		if (!(fp = fopen("/proc/mtd", "r"))) {
			cprintf("Can't open /proc/mtd\n");
		} else {
			while (fgets(dev, sizeof(dev), fp)) {
				if (sscanf(dev, "mtd%d:", &i) && strstr(dev, LINUX_SECOND)) {
					found = TRUE;
					break;
				}
			}
			fclose(fp);

			/* The BOOTPARTITON was set, but linux2 partition can't be created.
			 * So unset BOOTPARTITION.
			 */
			if (found == FALSE) {
				cprintf("Unset bootpartition due to no linux2 partition found.\n");
				nvram_unset(BOOTPARTITION);
				need_commit = TRUE;
			}
		}
	}

	if (need_commit == TRUE)
		nvram_commit();
}
#endif /* __CONFIG_FAILSAFE_UPGRADE_SUPPORT__ */

#ifdef BCA_HNDROUTER
#define TEMP_KERNEL_NVRM_FILE "/var/.temp.kernel.nvram"
#define PRE_COMMIT_KERNEL_NVRM_FILE "/var/.kernel_nvram.setting.prec"
#define TEMP_KERNEL_NVRAM_FILE_NAME "/var/.kernel_nvram.setting.temp"
#define KERNEL_NVRAM_FILE_NAME "/data/.kernel_nvram.setting"

/*
 * Erasing router nvram
 * On success, zero is returned.
 * On error, last errno is returned.
 */
static int nvram_erase()
{
	int err = 0;
        /* Foxconn added start */
	FILE *fp = NULL;

	cprintf("Before erasing nvram, stop Kwilt at first\n");
	system("/tmp/media/nand/kwilt/hipplay/bin/busybox sh /tmp/media/nand/kwilt/hipplay/daemon stop");

	cprintf("Erasing nvram ...\n");
        /* Foxconn added end */

	if (access(PRE_COMMIT_KERNEL_NVRM_FILE, F_OK) != -1 &&
		unlink(PRE_COMMIT_KERNEL_NVRM_FILE) < 0) {
		cprintf("*** Failed to delete file %s. Error: %s\n",
				PRE_COMMIT_KERNEL_NVRM_FILE, strerror(errno));
		err = errno;
	}

	if (access(TEMP_KERNEL_NVRAM_FILE_NAME, F_OK) != -1 &&
		unlink(TEMP_KERNEL_NVRAM_FILE_NAME) < 0) {
		cprintf("*** Failed to delete file %s. Error: %s\n",
				TEMP_KERNEL_NVRAM_FILE_NAME, strerror(errno));
		err = errno;
	}

	if (access(TEMP_KERNEL_NVRM_FILE, F_OK) != -1 &&
		unlink(TEMP_KERNEL_NVRM_FILE) < 0) {
		cprintf("*** Failed to delete file %s. Error: %s\n",
				TEMP_KERNEL_NVRM_FILE, strerror(errno));
		err = errno;
	}

	if (access(KERNEL_NVRAM_FILE_NAME, F_OK) != -1 &&
		unlink(KERNEL_NVRAM_FILE_NAME) < 0) {
		cprintf("*** Failed to delete file %s. Error: %s\n",
				KERNEL_NVRAM_FILE_NAME, strerror(errno));
		err = errno;
	}

        /* Foxconn modified start */
	// Create erase check file
	fp = fopen("/data/kernel_nvram.erase", "w");
	if (fp == NULL) {
		cprintf("*** Failed to create erase check file Error: %s\n",
				strerror(errno));
		err = errno;
	}
        /* Foxconn modified end */
	sync();
	cprintf("Erasing nvram done\n");
	return err;
}

/*****************************************************************************
 * Image interface handler. Used for accessing/writing flash device.
 *****************************************************************************/
static IMGIF_HANDLE imgifHandle = NULL;

CmsImageFormat parseImgHdr(UINT8 *bufP, UINT32 bufLen)
{
   int result = CMS_IMAGE_FORMAT_FLASH;

   return result;
}

/*****************************************************************************
*  FUNCTION:  bca_sys_upgrade
*  PURPOSE:   Receiving an image content from httpd and writing to NAND flash.
*  PARAMETERS:
*      path (IN) - pipe file path.
*  RETURNS:
*      0 - succeeded.
*      errno - failed operation.
*  NOTES:
*       The calling sequence:
*  This function is called from the context of the /sbin/init programm process.
*  The /sbin/init process is "forked" by HTTPD process and it is the child of
*  HTTPD process.
*  The communication between HTTPD (parent) and bca_sys_upgrade is hold via pipe.
*****************************************************************************/
int
bca_sys_upgrade(const char *path)
{
	int ret = 0;
	pid_t pid = getpid();
	int imgsz, ulimgsz = 0;
	int r_count, w_count;
	FILE *fp = NULL;
	char *buf = NULL;
	uint bufsz = 0;
	imgif_flash_info_t flash_info;
	uint blknum = 0;

	/* Opening communication pipe between HTTPD parent process */
	if ((fp = fopen(path, "r")) == NULL) {
		cprintf("*** Filed open a file %s. Error: %s. Aborting \n",
				path, strerror(errno));
		ret = errno;
		goto fail;
	}

	/* HTTPD parent process supposed to send the image size. reading it. */
	r_count = safe_fread((void*)&imgsz, 1, sizeof(imgsz), fp);
	if (r_count < sizeof(imgsz)) {
		cprintf("*** Error(pid:%d): %s@%d Pipe read failed. Expected:%d,read:%d. Aborting\n",
			pid, __FUNCTION__, __LINE__, sizeof(imgsz), r_count);
		ret = EPIPE;
		goto fail;
	}
	cprintf("%s@%d(pid:%d): image size=%d.\n",
		__FUNCTION__, __LINE__, pid, imgsz);

	/* Initialize IMGIF context */
	imgifHandle = imgif_open(parseImgHdr, NULL);
	if (imgifHandle == NULL) {
		cprintf("*** Error(pid:%d): %s@%d Failed to create image ifc context. Aborting\n",
			pid, __FUNCTION__, __LINE__);
		ret = EIO;
		goto fail;
	}

#if defined(IMGIF_API_VERSION)
	/* query flash device information */
	if (imgif_get_flash_info(&flash_info) < 0) {
		cprintf("*** Error(pid:%d): %s@%d Failed to get flash info. Aborting\n",
			getpid(), __FUNCTION__, __LINE__);
		ret = EIO;
		goto fail;
	}
#else
	/* query flash device information */
	if (imgif_get_flash_info(imgifHandle, &flash_info) < 0) {
		cprintf("*** Error(pid:%d): %s@%d Failed to get flash info. Aborting\n",
			getpid(), __FUNCTION__, __LINE__);
		ret = EIO;
		goto fail;
	}
#endif // endif

	/* evaluate image size */
	if (((imgsz + CMS_IMAGE_OVERHEAD) > flash_info.flashSize) ||
	    (imgsz < CMS_IMAGE_MIN_LEN)) {
		ret = EINVAL;
		goto fail;
	}

	/* setting image upload buf size equals to flash block size */
        bufsz = flash_info.eraseSize;

	/* Allocating image upload buffer */
	if ((buf = malloc(bufsz)) == NULL) {
		cprintf("*** Error(pid:%d) %s@%d malloc failed. %s\n",
				getpid(), __FUNCTION__, __LINE__, strerror(errno));
		ret = errno;
		goto fail;
	}

	cprintf("\nUpgrading: ");
	/* uploading entire image by chunks */
	for (ulimgsz = 0, blknum = 1; ulimgsz < imgsz; ulimgsz += r_count, blknum++) {
		r_count = safe_fread((void*)buf, 1, bufsz, fp);
		if ((r_count < bufsz) && ((r_count + ulimgsz) != imgsz)) {
			/* This must be the last chunk, othrwise fail */
			cprintf("*** Error(pid:%d): %s@%d Pipe read failed. Expected:%d,read:%d. Aborting\n",
			pid, __FUNCTION__, __LINE__, bufsz, r_count);
			ret = EPIPE;
			goto fail;
		}
		/* Write chunk to the flash. */
		w_count = imgif_write(imgifHandle, (UINT8*)buf, r_count);
		if ((w_count < 0) || (w_count != r_count)) {
			cprintf("\nimgif_write() failed, towrite=%d, ret=%d",
				     w_count, r_count);
			ret = EIO;
			goto fail;
		}
		cprintf(".");
	}

 fail:
	if (buf != NULL)
		free(buf);
	if (fp != NULL)
		fclose(fp);
	if (imgifHandle != NULL) {
	  if (imgif_close(imgifHandle, (ret != 0)) == 0) {
	    if (ret == 0)
	      cprintf("\nDone. (written %d bytes, %d blocks with size %d\n",
		      ulimgsz, blknum, flash_info.eraseSize);
	  } else {
	    if (ret == 0)
	      cprintf("\n*** Fail to write the image\n");
	    ret = EIO;
	  }
	}

	return ret;
}

/* Foxconn added start */
int
foxconn_sys_upgrade(const char *path, int img_size, int offset)
{
	int ret = 0;
	pid_t pid = getpid();
	int imgsz, ulimgsz = 0;
	int r_count, w_count;
	FILE *fp = NULL;
	char *buf = NULL;
	uint bufsz = 0;
	imgif_flash_info_t flash_info;
	uint blknum = 0;

	if ((fp = fopen(path, "r")) == NULL) {
		cprintf("*** Filed open a file %s. Error: %s. Aborting \n",
				path, strerror(errno));
		ret = errno;
		goto fail;
	}

	imgsz = img_size - offset;
	fseek(fp, offset, SEEK_SET);

	/* Initialize IMGIF context */
	imgifHandle = imgif_open(parseImgHdr, NULL);
	if (imgifHandle == NULL) {
		cprintf("*** Error(pid:%d): %s@%d Failed to create image ifc context. Aborting\n",
			pid, __FUNCTION__, __LINE__);
		ret = EIO;
		goto fail;
	}

	/* query flash device information */
	if (imgif_get_flash_info(&flash_info) < 0) {
		cprintf("*** Error(pid:%d): %s@%d Failed to get flash info. Aborting\n",
			getpid(), __FUNCTION__, __LINE__);
		ret = EIO;
		goto fail;
	}

	/* evaluate image size */
	if (((imgsz + CMS_IMAGE_OVERHEAD) > flash_info.flashSize) ||
	    (imgsz < CMS_IMAGE_MIN_LEN)) {
		ret = EINVAL;
		goto fail;
	}

	/* setting image upload buf size equals to flash block size */
	bufsz = flash_info.eraseSize;

	/* Allocating image upload buffer */
	if ((buf = malloc(bufsz)) == NULL) {
		cprintf("*** Error(pid:%d) %s@%d malloc failed. %s\n",
				getpid(), __FUNCTION__, __LINE__, strerror(errno));
		ret = errno;
		goto fail;
	}

	cprintf("\nUpgrading: ");
	/* uploading entire image by chunks */
	for (ulimgsz = 0, blknum = 1; ulimgsz < imgsz; ulimgsz += r_count, blknum++) {
		r_count = safe_fread((void*)buf, 1, bufsz, fp);
		if ((r_count < bufsz) && ((r_count + ulimgsz) != imgsz)) {
			/* This must be the last chunk, othrwise fail */
			cprintf("*** Error(pid:%d): %s@%d Pipe read failed. Expected:%d,read:%d. Aborting\n",
			pid, __FUNCTION__, __LINE__, bufsz, r_count);
			ret = EPIPE;
			goto fail;
		}
		/* Write chunk to the flash. */
		w_count = imgif_write(imgifHandle, (UINT8*)buf, r_count);
		if ((w_count < 0) || (w_count != r_count)) {
			cprintf("\nimgif_write() failed, towrite=%d, ret=%d",
				     w_count, r_count);
			ret = EIO;
			goto fail;
		}
		cprintf(".");
	}

 fail:
	if (buf != NULL)
		free(buf);
	if (fp != NULL)
		fclose(fp);
	if (imgifHandle != NULL) {
	  if (imgif_close(imgifHandle, (ret != 0)) == 0) {
	    if (ret == 0)
	      cprintf("\nDone. (written %d bytes, %d blocks with size %d\n",
		      ulimgsz, blknum, flash_info.eraseSize);
	  } else {
	    if (ret == 0)
	      cprintf("\n*** Fail to write the image\n");
	    ret = EIO;
	  }
	}

	return ret;
}
/* Foxconn added end */
#endif /* BCA_HNDROUTER */

static void
restore_defaults(void)
{
	struct nvram_tuple generic[] = {
		{ "lan_ifname", "br0", 0 },
#ifdef BCA_HNDROUTER
#ifndef NO_ETHWAN
		{ "lan_ifnames", "eth1 eth2 eth3 eth4 eth5 eth6 eth7 eth8", 0 },
		{ "wan_ifname", "eth0", 0 },
		{ "wan_ifnames", "eth0", 0 },
#else
		{ "lan_ifnames", "eth0 eth1 eth2 eth3", 0 },
		{ "wan_ifname", "", 0 },
		{ "wan_ifnames", "", 0 },
#endif /* NO_ETHWAN */
#else
		{ "lan_ifnames", "eth0 eth2 eth3 eth4 eth5", 0 },
		{ "wan_ifname", "eth1", 0 },
		{ "wan_ifnames", "eth1", 0 },
#endif /* BCA_HNDROUTER */
		{ "lan1_ifname", "br1", 0 },
		{ "lan1_ifnames", "wl0.1 wl0.2 wl0.3 wl1.1 wl1.2 wl1.3", 0 },
		{ 0, 0, 0 }
	};
#ifdef __CONFIG_VLAN__
	struct nvram_tuple vlan[] = {
		{ "lan_ifname", "br0", 0 },
		{ "lan_ifnames", "vlan0 eth1 eth2 eth3", 0 },
		{ "wan_ifname", "vlan1", 0 },
		{ "wan_ifnames", "vlan1", 0 },
		{ "lan1_ifname", "br1", 0 },
		{ "lan1_ifnames", "wl0.1 wl0.2 wl0.3 wl1.1 wl1.2 wl1.3", 0 },
		{ 0, 0, 0 }
	};
#endif	/* __CONFIG_VLAN__ */
	struct nvram_tuple dyna[] = {
		{ "lan_ifname", "br0", 0 },
		{ "lan_ifnames", "", 0 },
		{ "wan_ifname", "", 0 },
		{ "wan_ifnames", "", 0 },
		{ "lan1_ifname", "br1", 0 },
		{ "lan1_ifnames", "wl0.1 wl0.2 wl0.3 wl1.1 wl1.2 wl1.3", 0 },
		{ 0, 0, 0 }
	};

	struct nvram_tuple sta_mode[] = {
		{ "lan_ifname", "", 0 },
		{ "lan_ifnames", "", 0 },
		{ "wan_ifname", "", 0 },
		{ "wan_ifnames", "", 0 },
		{ "lan1_ifname", "", 0 },
		{ "lan1_ifnames", "wl0.1 wl0.2 wl0.3 wl1.1 wl1.2 wl1.3", 0 },
		{ 0, 0, 0 }
	};

	struct nvram_tuple psta_mode[] = {
		{ "lan_ifname", "br0", 0 },
		{ "lan_ifnames", "", 0 },
		{ "wan_ifname", "", 0 },
		{ "wan_ifnames", "", 0 },
		{ "wl_mode", "psta", 0 },
		{ "lan_dhcp", "0", 0},
		{ "router_disable", "0", 0 },
		{ 0, 0, 0 }
	};

	struct nvram_tuple psr_mode[] = {
		{ "lan_ifname", "br0", 0 },
		{ "lan_ifnames", "", 0 },
		{ "wan_ifname", "", 0 },
		{ "wan_ifnames", "", 0 },
		{ "wl_mode", "psr", 0 },
		{ "lan_dhcp", "0", 0 },
		{ "router_disable", "0", 0 },
		{ 0, 0, 0 }
	};

	struct nvram_tuple *linux_overrides = generic;
	struct nvram_tuple *t, *u;
	int restore_defaults, i;
#ifdef __CONFIG_VLAN__
	uint boardflags;
#endif	/* __CONFIG_VLAN_ */
	char *landevs, *wandevs;
	char lan_ifnames[128], wan_ifnames[128];
	char wan_ifname[32], *next;
	char lan_ifname[32];
	char lan1_ifname[32];
	int len;
	int ap = 0;
	int j;

	/* Restore defaults if told to or OS has changed */
	restore_defaults = RESTORE_DEFAULTS();

	if (restore_defaults) {
                /* Foxconn added start */
		// Remove erase check file
		if (unlink("/data/kernel_nvram.erase") < 0)
			cprintf("Fail to remove erase check file...");
                /* Foxconn added end */
		cprintf("Restoring defaults...");
	}

	/* Delete dynamically generated variables */
	if (restore_defaults) {
		char tmp[100], prefix[] = "wlXXXXXXXXXX_";
		for (i = 0; i < MAX_NVPARSE; i++) {
#ifdef __CONFIG_NAT__
			del_filter_client(i);
			del_forward_port(i);
#if !defined(AUTOFW_PORT_DEPRECATED)
			del_autofw_port(i);
#endif // endif
#endif	/* __CONFIG_NAT__ */
			snprintf(prefix, sizeof(prefix), "wl%d_", i);
			for (t = router_defaults; t->name; t ++) {
				if (!strncmp(t->name, "wl_", 3))
					nvram_unset(strcat_r(prefix, &t->name[3], tmp));
			}
#ifdef __CONFIG_NAT__
			snprintf(prefix, sizeof(prefix), "wan%d_", i);
			for (t = router_defaults; t->name; t ++) {
				if (!strncmp(t->name, "wan_", 4))
					nvram_unset(strcat_r(prefix, &t->name[4], tmp));
			}
#endif	/* __CONFIG_NAT__ */

			/* Delete dynamically generated NVRAMs for Traffic Mgmt & DWM */
			snprintf(prefix, sizeof(prefix), "wl%d_", i);
			for (j = 0; j < MAX_NUM_TRF_MGMT_RULES; j++) {
				del_trf_mgmt_port(prefix, j);
			}
			for (j = 0; j < MAX_NUM_TRF_MGMT_DWM_RULES; j++) {
				del_trf_mgmt_dwm(prefix, j);
			}

#ifdef __CONFIG_DHDAP__
			/* Delete dynamically generated NVRAMs for DHDAP */
			snprintf(tmp, sizeof(tmp), "wl%d_cfg_maxassoc", i);
			nvram_unset(tmp);
#endif // endif
		}
#if defined(LINUX_2_6_36) && defined(__CONFIG_TREND_IQOS__)
		iqos_restore_defaults();
#endif /* LINUX_2_6_36 && __CONFIG_TREND_IQOS__ */
#ifdef __CONFIG_WPS__
		wps_restore_defaults();
#endif /* __CONFIG_WPS__ */
#ifdef __CONFIG_WAPI_IAS__
		nvram_unset("as_mode");
#endif /* __CONFIG_WAPI_IAS__ */
		virtual_radio_restore_defaults();
#ifdef __CONFIG_URE__
		if (nvram_match("wl0_ure_mode", "wre") ||
		    nvram_match("wl0_ure_mode", "ure")) {
			ure_restore_defaults(0);
		}
#endif /* __CONFIG_URE__ */
#ifdef LINUX_2_6_36
		/* Delete dynamically generated variables */
                /* Foxconn deledted start */
		//gmac3_restore_defaults();
                /* Foxconn deleted end */
#endif // endif
		/* unset nvrams listed in local array of below function */
		unset_generic_nvrams();
	}

	/*
	 * Build bridged i/f name list and wan i/f name list from lan device name list
	 * and wan device name list. Both lan device list "landevs" and wan device list
	 * "wandevs" must exist in order to preceed.
	 */

	if (!strcmp(nvram_safe_get("devicemode"), "sta")) {
		int val;
		int lan_if = 0;
		if ((landevs = nvram_get("landevs"))) {
			/* Build the OS interface names */
			len = sizeof(lan_ifnames);
			if (!build_ifnames(landevs, lan_ifnames, &len) && len)
				sta_mode[1].value = lan_ifnames;
			else
				goto canned_config;
			foreach(lan_ifname, lan_ifnames, next) {
				/* Looking for primary wl interface */
				if (!wl_ioctl(lan_ifname, WLC_GET_MAGIC, &val, sizeof(val))) {
					sta_mode[0].value = lan_ifname;
					break;
				}
			}
			foreach(lan1_ifname, lan_ifnames, next) {
				/* Looking for secondary wl interface if any */
				if (!wl_ioctl(lan1_ifname, WLC_GET_MAGIC, &val, sizeof(val))) {
					/* Skip to the next WLAN interface if there is one */
					if (lan_if) {
						sta_mode[4].value = lan1_ifname;
						break;
					}
					lan_if++;
				}
			}
			linux_overrides = sta_mode;
		} else
			goto canned_config;
	} else if (!strcmp(nvram_safe_get("devicemode"), "psta")) {
		/* Get the lan and wan if names to configure */
		/* Note: In the event of misconfiguration, we still need to reach the router
		 * via HTTP or Telnet. Hence, check for landevs "or" wandevs.
		 */
		if ((landevs = nvram_get("landevs"))) {
			/* build bridged i/f list based on nvram variable "landevs" */
			len = sizeof(lan_ifnames);
			if (!build_ifnames(landevs, lan_ifnames, &len) && len)
				psta_mode[1].value = lan_ifnames;
			else
				goto canned_config;
		}

		/* build wan i/f list based on nvram variable "wandevs" */
		if ((wandevs = nvram_get("wandevs"))) {
			len = sizeof(wan_ifnames);
			if (!build_ifnames(wandevs, wan_ifnames, &len) && len) {
				psta_mode[3].value = wan_ifnames;
				foreach(wan_ifname, wan_ifnames, next) {
					psta_mode[2].value = wan_ifname;
					break;
				}
			}
		}
		if (landevs || wandevs) {
			linux_overrides = psta_mode;
		} else
			goto canned_config;
	} else if (!strcmp(nvram_safe_get("devicemode"), "psr")) {
		/* Get the lan and wan ifnames to configure */
		/* Note: In the event of misconfiguration, we still need to reach the router
		 * via HTTP or Telnet. Hence, check for landevs "or" wandevs.
		 */
		if ((landevs = nvram_get("landevs"))) {
			/* build bridged i/f list based on nvram variable "landevs" */
			len = sizeof(lan_ifnames);
			if (!build_ifnames(landevs, lan_ifnames, &len) && len) {
				char nv_interface[NVRAM_MAX_PARAM_LEN];
				int wl_unit = 0;
				sprintf(nv_interface, "wl%d.1", wl_unit);
				add_to_list(nv_interface, lan_ifnames, sizeof(lan_ifnames));
				psr_mode[1].value = lan_ifnames;
			} else
				goto canned_config;
		}

		if ((wandevs = nvram_get("wandevs"))) {
			/* build wan i/f list based on nvram variable "wandevs" */
			len = sizeof(wan_ifnames);
			if (!build_ifnames(wandevs, wan_ifnames, &len) && len) {
				psr_mode[3].value = wan_ifnames;
				foreach(wan_ifname, wan_ifnames, next) {
					psr_mode[2].value = wan_ifname;
					break;
				}
			}
		} else
			ap = 1;
		if (landevs || wandevs) {
			linux_overrides = psr_mode;
		} else
			goto canned_config;
	} else if ((landevs = nvram_get("landevs")) && (wandevs = nvram_get("wandevs"))) {
		/* build bridged i/f list based on nvram variable "landevs" */
		len = sizeof(lan_ifnames);
		if (!build_ifnames(landevs, lan_ifnames, &len) && len)
			dyna[1].value = lan_ifnames;
		else
			goto canned_config;

		/* build wan i/f list based on nvram variable "wandevs" */
		len = sizeof(wan_ifnames);
		if (!build_ifnames(wandevs, wan_ifnames, &len) && len) {
			dyna[3].value = wan_ifnames;
			foreach(wan_ifname, wan_ifnames, next) {
				dyna[2].value = wan_ifname;
				break;
			}
		}
		else
			ap = 1;
		linux_overrides = dyna;
	}
	/* override lan i/f name list and wan i/f name list with default values */
	else {
canned_config:
#ifdef __CONFIG_VLAN__
		boardflags = strtoul(nvram_safe_get("boardflags"), NULL, 0);
		if (boardflags & BFL_ENETVLAN)
			linux_overrides = vlan;
		else
#endif	/* __CONFIG_VLAN__ */
			linux_overrides = generic;
	}

	/* Check if nvram version is set, but old */
	if (nvram_get("nvram_version")) {
		int old_ver, new_ver;

		old_ver = atoi(nvram_get("nvram_version"));
		new_ver = atoi(NVRAM_SOFTWARE_VERSION);
		if (old_ver < new_ver) {
			cprintf("NVRAM: Updating from %d to %d\n", old_ver, new_ver);
			nvram_set("nvram_version", NVRAM_SOFTWARE_VERSION);
		}
	}

	/* Restore defaults */
	for (t = router_defaults; t->name; t++) {
		if (restore_defaults || !nvram_get(t->name)) {
			for (u = linux_overrides; u && u->name; u++) {
				if (!strcmp(t->name, u->name)) {
					nvram_set(u->name, u->value);
					break;
				}
			}
			if (!u || !u->name) {
				nvram_set(t->name, nvram_default_get(t->name));
			}
		}
	}

	/* Force to AP */
#if 0 /* foxconn wklin removed, 10/22/2008 */
	if (ap)
		nvram_set("router_disable", "1");
#endif

	/* Always set OS defaults */
	nvram_set("os_name", "linux");
	nvram_set("os_version", ROUTER_VERSION_STR);
	nvram_set("os_date", __DATE__);
	/* Always set WL driver version! */
	nvram_set("wl_version", EPI_VERSION_STR);

	nvram_set("is_modified", "0");
	nvram_set("ezc_version", EZC_VERSION_STR);

	if (restore_defaults) {
		FILE *fp;
		char memdata[256] = {0};
		uint memsize = 0;
		char pktq_thresh[8] = {0};
		char et_pktq_thresh[8] = {0};

		if ((fp = fopen("/proc/meminfo", "r")) != NULL) {
			/* get memory count in MemTotal = %d */
			while (fgets(memdata, 255, fp) != NULL) {
				if (strstr(memdata, "MemTotal") != NULL) {
					sscanf(memdata, "MemTotal:        %d kB", &memsize);
					break;
				}
			}
			fclose(fp);
		}

		if (memsize <= 32768) {
			/* Set to 512 as long as onboard memory <= 32M */
			sprintf(pktq_thresh, "512");
			sprintf(et_pktq_thresh, "512");
		}
		else if (memsize <= 65536) {
			/* We still have to set the thresh to prevent oom killer */
			sprintf(pktq_thresh, "1024");
			sprintf(et_pktq_thresh, "1536");
		}
		else {
			sprintf(pktq_thresh, "1024");
			/* Adjust the et thresh to 3300 as long as memory > 64M.
			 * The thresh value is based on benchmark test.
			 */
			sprintf(et_pktq_thresh, "3300");
		}

		nvram_set("wl_txq_thresh", pktq_thresh);
		nvram_set("et_txq_thresh", et_pktq_thresh);
#if defined(__CONFIG_USBAP__)
		nvram_set("wl_rpcq_rxthresh", pktq_thresh);
#endif // endif
		/* foxconn modified start, zacker, 08/06/2010 */
		/* Create a new value to inform loaddefault in "read_bd" */
		nvram_set("load_defaults", "1");
		eval("read_bd"); /* foxconn wklin added, 10/22/2008 */
		/* finished "read_bd", unset load_defaults flag */
#ifdef CONFIG_PRODUCT_ALIAS
		if(strstr(nvram_safe_get("hwver"),AMBIT_PRODUCT_NAME_ALIAS))
		{
			nvram_set("wps_modelname",AMBIT_PRODUCT_NAME_ALIAS);
			nvram_set("media_name","ReadyDLNA: "AMBIT_PRODUCT_NAME_ALIAS);
			nvram_set("system_name",AMBIT_PRODUCT_NAME_ALIAS);
			nvram_set("friendly_name",AMBIT_PRODUCT_NAME_ALIAS);
			nvram_set("wps_modelnum",AMBIT_PRODUCT_NAME_ALIAS);
			nvram_set("wps_device_name",AMBIT_PRODUCT_NAME_ALIAS);
			nvram_set("l2tp_conn_id",AMBIT_PRODUCT_NAME_ALIAS);
		}
#endif
		nvram_unset("load_defaults");

		/* foxconn modified end, zacker, 08/06/2010 */
		/* Foxconn add start, Tony W.Y. Wang, 04/06/2010 */
#ifdef SINGLE_FIRMWARE
		if (nvram_match("sku_name", "NA"))
		acosNvramConfig_setPAParam(0);
		else
		acosNvramConfig_setPAParam(1);
#else
		#ifdef FW_VERSION_NA
			acosNvramConfig_setPAParam(0);
		#else
			acosNvramConfig_setPAParam(1);
		#endif
#endif
		/* Foxconn add end, Tony W.Y. Wang, 04/06/2010 */

	}

#ifdef __CONFIG_URE__
	if (restore_defaults) {
		if (nvram_match("wl0_ure_mode", "ure") ||
		    nvram_match("wl0_ure_mode", "wre")) {
			nvram_set("ure_disable", "0");
			nvram_set("router_disable", "1");
			/* Set ure related nvram settings */
			set_ure_vars(0);
		}
	}
#endif /* __CONFIG_URE__ */

#ifdef BCM_DRSDBD
	if (restore_defaults)
	{
		nvram_set("drsdbd_restore_defaults", "1");
	}
#endif /* BCM_DRSDBD */
	if (restore_defaults) {
		/* We readjust once we start the wl driver */
		if (nvram_match("devicemode", "psr")) {
			nvram_set("wl0_mode", "ap");
			nvram_set("wl1_mode", "ap");
		}
	}

	/* Commit values */
	if (restore_defaults) {
		nvram_commit();
		sync();         /* Foxconn added start pling 12/25/2006 */
		cprintf("done\n");
	}
}

#ifdef __CONFIG_NAT__
static void
set_wan0_vars(void)
{
	int unit;
	char tmp[100], prefix[] = "wanXXXXXXXXXX_";

	/* check if there are any connections configured */
	for (unit = 0; unit < MAX_NVPARSE; unit ++) {
		snprintf(prefix, sizeof(prefix), "wan%d_", unit);
		if (nvram_get(strcat_r(prefix, "unit", tmp)))
			break;
	}
	/* automatically configure wan0_ if no connections found */
	if (unit >= MAX_NVPARSE) {
		struct nvram_tuple *t;
		char *v;

		/* Write through to wan0_ variable set */
		snprintf(prefix, sizeof(prefix), "wan%d_", 0);
		for (t = router_defaults; t->name; t ++) {
			if (!strncmp(t->name, "wan_", 4)) {
				if (nvram_get(strcat_r(prefix, &t->name[4], tmp)))
					continue;
				v = nvram_get(t->name);
				nvram_set(tmp, v ? v : t->value);
			}
		}
		nvram_set(strcat_r(prefix, "unit", tmp), "0");
		nvram_set(strcat_r(prefix, "desc", tmp), "Default Connection");
		nvram_set(strcat_r(prefix, "primary", tmp), "1");
	}
}
#endif	/* __CONFIG_NAT__ */

#if defined(LINUX26)

#define FS_JFFS2		0x1
#define FS_YAFFS2		0x2
#define FS_JFFS2_ENAB(x)	(x & FS_JFFS2)
#define FS_YAFFS2_ENAB(x)	(x & FS_YAFFS2)

#define JFFS2_MAGIC		0x1985

static int
nand_fs_check(uint32 *flag)
{
	FILE *fp;
	char buf[NAME_MAX];

	if (!(fp = fopen("/proc/filesystems", "r")))
		return -1;

	while (fgets(buf, sizeof(buf), fp) != NULL) {
		if (strstr(buf, "jffs2")) {
			*flag |= FS_JFFS2;
		} else if (strstr(buf, "yaffs2")) {
			*flag |= FS_YAFFS2;
		}
	}

	fclose(fp);

	return 0;
}

static int
read_jffs2_magic(uint32 flag, uint16 *magic)
{
	if (FS_YAFFS2_ENAB(flag)) {
		int fd;
		int readbytes;

		/* get oob free areas data */
		if ((fd = open("/proc/brcmnand", O_RDONLY)) < 0)
			return -1;

		readbytes = read(fd, (char*)magic, sizeof(*magic));

		close(fd);

		if (readbytes != sizeof(*magic)) {
			fprintf(stderr, "Read brcmnand oob failed, "
				"want %d but got %d bytes\n", sizeof(*magic), readbytes);
			return -1;
		}
	} else {
		*magic = JFFS2_MAGIC;
	}

	return 0;
}

static int
nand_fs_mtd_mount(const char *mtd, const char *jpath)
{
	FILE *fp;
	char *dpath = NULL, dev[PATH_MAX];
	int i, ret = -1;
	uint32 fs_flag = 0;
	uint16 magic = 0;

	/* get mtdblock device */
	if (!(fp = fopen("/proc/mtd", "r")))
		return -1;

	while (fgets(dev, sizeof(dev), fp)) {
		if (sscanf(dev, "mtd%d:", &i) && strstr(dev, mtd)) {
			snprintf(dev, sizeof(dev), "/dev/mtdblock%d", i);
			dpath = dev;
			break;
		}
	}
	fclose(fp);

	/* check if we have MTD partition */
	if (dpath == NULL)
		return -1;

	/* create mount directory */
	if (mkdir(jpath, 0777) != 0 && errno != EEXIST)
		return -1;

	/* check filesystem support for nand */
	if ((nand_fs_check(&fs_flag) != 0) || fs_flag == 0)
		return -1;

	if (read_jffs2_magic(fs_flag, &magic) != 0)
		return -1;

	if ((magic == JFFS2_MAGIC) && FS_JFFS2_ENAB(fs_flag)) {
		if (mount(dpath, jpath, "jffs2", 0, NULL)) {
			fprintf(stderr, "Mount nflash MTD jffs2 partition %s to %s failed\n",
				dpath, jpath);
			goto erase_and_mount;
		}

		return 0;
	}

	if (FS_YAFFS2_ENAB(fs_flag)) {
		if (mount(dpath, jpath, "yaffs2", 0, NULL)) {
			fprintf(stderr, "Mount nflash MTD yaffs2 partition %s to %s failed\n",
				dpath, jpath);
			goto erase_and_mount;
		}

		/* Force creation of YAFFS2 checkpoint (YAFFS2 run-time state snapshot) */
		sync();

		return 0;
	}

erase_and_mount:
	if ((ret = mtd_erase(mtd))) {
		fprintf(stderr, "Erase nflash MTD partition %s failed %d\n", dpath, ret);
		return ret;
	}

	ret = -1;

	if (FS_YAFFS2_ENAB(fs_flag)) {
		if ((ret = mount(dpath, jpath, "yaffs2", 0, NULL)) != 0) {
			fprintf(stderr, "Mount nflash MTD yaffs2 partition %s to %s failed\n",
				dpath, jpath);
		} else {
			/* Force creation of YAFFS2 checkpoint (YAFFS2 run-time state snapshot) */
			sync();
		}
	}

	if (FS_JFFS2_ENAB(fs_flag) && ret != 0) {
		if ((ret = mount(dpath, jpath, "jffs2", 0, NULL)) != 0) {
			fprintf(stderr, "Mount nflash MTD jffs2 partition %s to %s failed\n",
				dpath, jpath);
		}
	}

	return ret;
}
#endif /* LINUX26 */

/*  Add support for crashlogging using MTD OOPS
*/
/* Name of flash partition to put/pull the logs */
#define WLENT_MTDOOPS_PARTITION_NAME	"mtdoops"
/* Max size of the logs to write  */
#define WLENT_MTDOOPS_RECORD_SIZE	4096

static void
start_mtdoops_crash_logging(void)
{
	char mtdoptions[200];
	struct utsname name;

	/* Enable kernel reboot on panic (2.6 and 3.10) */
	uname(&name);

	/* Install mtdoops module with our options */
	snprintf(mtdoptions, sizeof(mtdoptions),
	         "insmod /lib/modules/%s/kernel/drivers/mtd/mtdoops.ko record_size=%d mtddev=%s",
	         name.release, WLENT_MTDOOPS_RECORD_SIZE, WLENT_MTDOOPS_PARTITION_NAME);
	system(mtdoptions);
}

/* Foxconn added start */
#ifdef BCA_HNDROUTER
#define WATCHDOG_MIN_LX4x 4000
static void start_hw_wdt(void)
{
	int wdt = atoi(nvram_safe_get("watchdog"));

	/* arm the hw watchdog timer */
	if (wdt) {
		char tmp[100];

		wdt = (wdt > WATCHDOG_MIN_LX4x) ? wdt : WATCHDOG_MIN_LX4x;
		/* convert to secs */
		wdt /= 1000;
		dprintf("wdt:%d\n", wdt);

		snprintf(tmp, sizeof(tmp), "wdtctl -d -t %d start", wdt);
		system(tmp);
	}
}
#endif /* BCA_HNDROUTER */
/* Foxconn added end */

void
get_chipinfo(uint *chipid, uint *chiprev, uint *chippkg)
{
	FILE *fp = NULL;
	char buf[128];

	*chipid = 0;
	*chiprev = 0;
	*chippkg = 0;

	/* Get PlatSoc CHIP info */
	if ((fp = fopen("/proc/bcm_chipinfo", "r")) != NULL) {
		while (fgets(buf, sizeof(buf), fp) != NULL) {
			if (strstr(buf, "ChipID") != NULL) {
				sscanf(buf, "ChipID: 0x%x", chipid);
				continue;
			}
			if (strstr(buf, "ChipRevision") != NULL) {
				sscanf(buf, "ChipRevision: 0x%x", chiprev);
				continue;
			}
			if (strstr(buf, "PackageOption") != NULL) {
				sscanf(buf, "PackageOption: 0x%x", chippkg);
				continue;
			}
		}
		fclose(fp);
	}
}

/* This function updates the nvram radio_dmode_X to NIC/DGL depending on driver mode */
static void
wl_driver_mode_update(void)
{
	int unit = -1;
	int i = 0;
	char ifname[16] = {0};

	/* Search for existing wl devices with eth prefix and the max unit number used */
	for (i = 0; i <= DEV_NUMIFS; i++) {
		snprintf(ifname, sizeof(ifname), "eth%d", i);
		if (!wl_probe(ifname)) {
			int unit = -1;
			char mode_str[128];
			char *mode = "NIC";

#ifdef __CONFIG_DHDAP__
			mode = dhd_probe(ifname) ? "NIC" : "DGL";
#endif // endif

			if (!wl_ioctl(ifname, WLC_GET_INSTANCE, &unit,
					sizeof(unit))) {
				sprintf(mode_str, "wlradio_dmode_%d", unit);
				if (strcmp(nvram_safe_get(mode_str), mode) != 0) {
					printf("%s: Setting %s = %s\n",
							__FUNCTION__, mode_str, mode);
					nvram_set(mode_str, mode);
				}
			}
		}
	}

	/* Search for existing wl devices with wl prefix and the max unit number used */
	for (i = 0; i <= DEV_NUMIFS; i++) {
		snprintf(ifname, sizeof(ifname), "wl%d", i);
		if (!wl_probe(ifname)) {
			char mode_str[128];
			char *mode = "NIC";

#ifdef __CONFIG_DHDAP__
			mode = dhd_probe(ifname) ? "NIC" : "DGL";
#endif // endif

			if (!wl_ioctl(ifname, WLC_GET_INSTANCE, &unit,
					sizeof(unit))) {
				sprintf(mode_str, "wlradio_dmode_%d", i);
				if (strcmp(nvram_get(mode_str), mode) != 0) {
					printf("%s: Setting %s = %s\n",
							__FUNCTION__, mode_str, mode);
					nvram_set(mode_str, mode);
				}
			}
		}
	}
}

#ifdef BCA_HNDROUTER
/* This function updates the affinity of wl NIC driver kernel thread */
static void
wl_thread_affinity_update(void)
{
	int i = 0;
	pid_t pid_wl;
	char pid[16] = {0};
	char process_name[16];

#define MAX_RADIO_NUM 3
	for (i = 0; i < MAX_RADIO_NUM; i++) {
		sprintf(process_name, "wl%d-kthrd", i);
		if ((pid_wl = get_pid_by_thrd_name(process_name)) <= 0)
			continue;
		sprintf(pid, "%d", pid_wl);
		eval("taskset", "-p", "0xf", pid);
	}
}
#endif /* BCA_HNDROUTER */

static int noconsole = 0;

static void
sysinit(void)
{
#ifndef __CONFIG_STBAP__
	struct utsname name;
	time_t tm = 0;
#ifdef BCA_HNDROUTER
	char *crash_log_backup_mtd;
	char *crash_log_backup_dir;
#endif /* BCA_HNDROUTER */

	struct utsname unamebuf;
	char *lx_rel;
#endif /* __CONFIG_STBAP__ */

	char buf[PATH_MAX];
	struct stat tmp_stat;
	char *loglevel;
#ifndef __CONFIG_STBAP__
	/* Use uname() to get the system's hostname */
	uname(&unamebuf);
	lx_rel = unamebuf.release;

#ifdef BCA_HNDROUTER
	setenv("PATH",
		"/sbin:/bin:/usr/sbin:/usr/bin:/opt/sbin:/opt/bin:/opt/usr/sbin:/opt/usr/bin",
		1);
	cprintf("Starting bcm_boot_launcher ...\n");
	system("bcm_boot_launcher start");
	/* Foxconn added start */
	start_hw_wdt();
	if (is_en_load_default()) {
		nvram_set("restore_defaults", "1");
		system("burndisdefault");
	}
	/* Foxconn added end */
#endif /* BCA_HNDROUTER */

	if (memcmp(lx_rel, "2.6", 3) == 0) {
		int fd;
		if ((fd = open("/dev/console", O_RDWR)) < 0) {
			if (memcmp(lx_rel, "2.6.36", 6) == 0) {
				mount("devfs", "/dev", "devtmpfs", MS_MGC_VAL, NULL);
			}
			else {
				mount("devfs", "/dev", "tmpfs", MS_MGC_VAL, NULL);
				if (mknod("/dev/console", S_IRWXU|S_IFCHR, makedev(5, 1)) < 0 &&
					errno != EEXIST) {
					perror("filesystem node /dev/console not created");
				}
			}
		}
		else {
			close(fd);
		}
	}

	/* /proc */
	mount("proc", "/proc", "proc", MS_MGC_VAL, NULL);
#ifdef LINUX26
	mount("sysfs", "/sys", "sysfs", MS_MGC_VAL, NULL);
#endif /* LINUX26 */

#ifndef BCA_HNDROUTER
	/* /tmp */
	mount("ramfs", "/tmp", "ramfs", MS_MGC_VAL, NULL);
#endif /* !BCA_HNDROUTER */

	/* /var */
	if (mkdir("/tmp/var", 0777) < 0 && errno != EEXIST) perror("/tmp/var not created");
	if (mkdir("/var/lock", 0777) < 0 && errno != EEXIST) perror("/var/lock not created");
	if (mkdir("/var/log", 0777) < 0 && errno != EEXIST) perror("/var/log not created");
	if (mkdir("/var/run", 0777) < 0 && errno != EEXIST) perror("/var/run not created");
	if (mkdir("/var/tmp", 0777) < 0 && errno != EEXIST) perror("/var/tmp not created");
	if (mkdir("/tmp/media", 0777) < 0 && errno != EEXIST) perror("/tmp/media not created");

#if 0 /* Foxconn removed */
#ifdef __CONFIG_HSPOT__
	if (mkdir(RAMFS_CONFMTD_DIR, 0777) < 0 && errno != EEXIST) {
		perror("/tmp/confmtd not created.");
	}
	
	if (mkdir(RAMFS_CONFMTD_DIR"/hspot", 0777) < 0 && errno != EEXIST) {
		perror("/tmp/confmtd/hspot not created.");
	}
#endif /* __CONFIG_HSPOT__ */

#ifdef __CONFIG_DHDAP__
	if (mkdir(RAMFS_CONFMTD_DIR, 0777) < 0 && errno != EEXIST) {
		perror("/tmp/confmtd not created.");
	}
#endif /* __CONFIG_DHDAP__ */

#ifdef BCM_DRSDBD
	if (mkdir(RAMFS_CONFMTD_DIR, 0777) < 0 && errno != EEXIST) {
		perror("/tmp/confmtd not created.");
	}
	if (mkdir(RAMFS_CONFMTD_DIR"/drsdbd", 0777) < 0 && errno != EEXIST) {
		perror("/tmp/confmtd/drsdbd not created.");
	}
#endif /* BCM_DRSDBD */
#endif /*Foxconn removed*/


#if defined(LINUX26)
#ifdef BCA_HNDROUTER
	crash_log_backup_mtd = nvram_get("crash_log_backup_mtd");
	crash_log_backup_dir = nvram_get("crash_log_backup_dir");
	if (crash_log_backup_mtd && crash_log_backup_dir)
		nand_fs_mtd_mount(crash_log_backup_mtd, crash_log_backup_dir);
#else
	nand_fs_mtd_mount("brcmnand", "/tmp/media/nand");
#endif /* BCA_HNDROUTER */
#endif /* LINUX26 */

	confmtd_restore();
	/* Foxconn added start by Kathy, 10/14/2013 @ Facebook WiFi */
	mkdir("/tmp/fbwifi", 0777);
	/* Foxconn added end by Kathy, 10/14/2013 @ Facebook WiFi */
#ifdef __CONFIG_UTELNETD__
	/* If kernel enable unix908 pty then we have to make following things. */
	if (mkdir("/dev/pts", 0777) < 0 && errno != EEXIST) perror("/dev/pts not created");
	if (mount("devpts", "/dev/pts", "devpts", MS_MGC_VAL, NULL) == 0) {
		/* pty master */
		if (mknod("/dev/ptmx", S_IRWXU|S_IFCHR, makedev(5, 2)) < 0 && errno != EEXIST)
			perror("filesystem node /dev/ptmx not created");
	} else {
		rmdir("/dev/pts");
	}
#endif	/* LINUX2636 && __CONFIG_UTELNETD__ */

#ifdef __CONFIG_SAMBA__
#ifndef BCA_HNDROUTER
	/* Add Samba Stuff */
	if (mkdir("/tmp/samba", 0777) < 0 && errno != EEXIST) {
		perror("/tmp/samba not created");
	}
	if (mkdir("/tmp/samba/lib", 0777) < 0 && errno != EEXIST) {
		perror("/tmp/samba/lib not created");
	}
	if (mkdir("/tmp/samba/private", 0777) < 0 && errno != EEXIST) {
		perror("/tmp/samba/private not created");
	}
	if (mkdir("/tmp/samba/var", 0777) < 0 && errno != EEXIST) {
		perror("/tmp/samba/var not created");
	}
	if (mkdir("/tmp/samba/var/locks", 0777) < 0 && errno != EEXIST) {
		perror("/tmp/samba/var/locks not created");
	}
#endif /* !BCA_HNDROUTER */
#if defined(LINUX_2_6_36)
	/* To improve SAMBA upload performance */
	reclaim_mem_earlier();
#endif /* LINUX_2_6_36 */
#endif /* __CONFIG_SAMBA__ */

#ifdef BCMQOS
	if (mkdir("/tmp/qos", 0777) < 0 && errno != EEXIST) perror("/tmp/qos not created");
#endif // endif
#endif /* __CONFIG_STBAP__ */

	/* Setup console */
#ifdef __CONFIG_STBAP__
	noconsole = 1;
#else
	if (console_init())
		noconsole = 1;
#endif /* __CONFIG_STBAP__ */

#ifndef __CONFIG_STBAP__
#ifdef LINUX26
	if (mkdir("/dev/shm", 0777) < 0 && errno != EEXIST) perror("/dev/shm not created");
	eval("/sbin/hotplug2", "--coldplug");
#endif /* LINUX26 */
#endif /* __CONFIG_STBAP__ */

	if ((loglevel = nvram_get("console_loglevel")))
		klogctl(8, NULL, atoi(loglevel));
	else
		klogctl(8, NULL, 1);

	/* Modules */
#ifdef __CONFIG_STBAP__
	snprintf(buf, sizeof(buf), "/lib/modules");
#else
	uname(&name);
	snprintf(buf, sizeof(buf), "/lib/modules/%s", name.release);
#endif /* __CONFIG_STBAP__ */
	if (stat("/proc/modules", &tmp_stat) == 0 &&
	    stat(buf, &tmp_stat) == 0) {
#ifndef __CONFIG_STBAP__
		char module[80], *modules, *next;
#endif // endif

		/* foxconn modified start, zacker, 08/06/2010 */
		/* Restore defaults if necessary */
		nvram_set ("wireless_restart", "1");
        restore_defaults();
		
		/* Foxconn Bob added start on 11/12/2014, force enable DFS */
		nvram_set("fcc_dfs_ch_enable", "1");
		nvram_set("ce_dfs_ch_enable", "1");
		nvram_set("telec_dfs_ch_enable", "1");
		/* Foxconn Bob added end on 11/12/2014, force enable DFS */


        /* For 4500 IR-159. by MJ. 2011.07.04  */
        /* Foxconn added start pling 02/11/2011 */
        /* WNDR4000 IR20: unset vifs NVRAM and let
         * bcm_wlan_util.c to reconstruct them if
         * necessary. move to here since they should be
         * done before read_bd */
        nvram_unset("wl0_vifs");
        nvram_unset("wl1_vifs");
        #if defined(INCLULDE_2ND_5G_RADIO) 
        nvram_unset("wl2_vifs");
        #endif
        /* Foxconn added end pling 02/11/2011 */

        /* Read ethernet MAC, RF params, etc */
		eval("read_bd");
        /* foxconn modified end, zacker, 08/06/2010 */

		/* Load ctf */

        /* Foxconn added start Bob 10/30/2014 */
        /* Make sure ctf_disable value is correct after dynamic enable/disable CTF function is introduced */
        if (nvram_match("enable_vlan", "1"))
            nvram_set("ctf_disable", "1");
        else
            nvram_set("ctf_disable", "0");
        /* Foxconn added end Bob 10/30/2014 */

		if (!nvram_match("ctf_disable", "1"))
			eval("insmod", "ctf");

#if defined(__CONFIG_WAPI__) || defined(__CONFIG_WAPI_IAS__)
		if (stat(WAPI_DIR, &tmp_stat) != 0) {
			if (mkdir(WAPI_DIR, 0777) < 0 && errno != EEXIST) {
				perror("WAPI_DIR not created");
			}
			if (mkdir(WAPI_WAI_DIR, 0777) < 0 && errno != EEXIST) {
				perror("WAPI_WAI_DIR not created");
			}
			if (mkdir(WAPI_AS_DIR, 0777) < 0 && errno != EEXIST) {
				perror("WAPI_AS_DIR not created");
			}
		}
#endif /* __CONFIG_WAPI__ || __CONFIG_WAPI_IAS__ */
#if defined(__CONFIG_CIFS__)
		if (stat(CIFS_DIR, &tmp_stat) != 0) {
			if (mkdir(CIFS_DIR, 0777) < 0 && errno != EEXIST) {
				perror("CIFS_DIR not created");
			}
			eval("cp", "/usr/sbin/cs_cfg.txt", CIFS_DIR);
			eval("cp", "/usr/sbin/cm_cfg.txt", CIFS_DIR);
			eval("cp", "/usr/sbin/pwd_list.txt", CIFS_DIR);
		}
#endif /* __CONFIG_CIFS__ */

/* #ifdef BCMVISTAROUTER */
#ifdef __CONFIG_IPV6__
		eval("insmod", "ipv6");
#endif /* __CONFIG_IPV6__ */
/* #endif */

#ifndef __CONFIG_STBAP__
#if defined(__CONFIG_EMF__) && !defined(BCA_HNDROUTER)
		/* Load the EMF & IGMP Snooper modules */
		load_emf();
#endif /*  __CONFIG_EMF__ && !BCA_HNDROUTER */
    /* Bob added start to avoid sending unexpected dad, 09/16/2009 */
#ifdef INCLUDE_IPV6
		if (nvram_match("ipv6ready","1"))
		{
			system("echo 0 > /proc/sys/net/ipv6/conf/default/dad_transmits");
		}else{
		/* Foxconn added start pling 12/06/2010 */
		/* By default ipv6_spi is inserted to system to drop all packets. */
		/*Foxconn modify start by Hank for change ipv6_spi path in rootfs 08/27/2012*/

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4, 1, 13))
        char cmd_buf[128];
		if (nvram_match("enable_ap_mode","1"))
            sprintf(cmd_buf, "/sbin/insmod %s/ipv6_spi.ko working_mode=\"ap\"", KERNEL_MODULE_PATH);
        else
            sprintf(cmd_buf, "/sbin/insmod %s/ipv6_spi.ko", KERNEL_MODULE_PATH);
        
        system(cmd_buf);    
#else        
		if (nvram_match("enable_ap_mode","1"))
			system("/sbin/insmod /lib/modules/2.6.36.4brcmarm+/kernel/lib/ipv6_spi.ko working_mode=\"ap\"");
		else
			system("/sbin/insmod /lib/modules/2.6.36.4brcmarm+/kernel/lib/ipv6_spi.ko");
#endif			
		/*Foxconn modify end by Hank for change ipv6_spi path in rootfs 08/27/2012*/
		/* Foxconn added end pling 12/06/2010 */
		}
#endif
    /* Bob added end to avoid sending unexpected dad, 09/16/2009 */
        
		/* Foxconn added start pling 09/02/2010 */
		/* Need to initialise switch related NVRAM before 
		 * insert ethernet module. */

		/* Load kernel modules. Make sure dpsta is loaded before wl
		 * due to symbol dependency.
		 */
#ifdef __CONFIG_GMAC3__
    

    if(!nvram_get("gmac3_enable"))
        nvram_set("gmac3_enable", "1");
    	

    if((strlen(nvram_get("gmac3_enable"))==0) || (strcmp(nvram_get("gmac3_enable"),"1")==0))
    {
        nvram_set("wandevs", "et2");
        nvram_set("et0macaddr", "00:00:00:00:00:00");
        nvram_set("et0mdcport", "0");
        nvram_set("et0phyaddr", "30");


        nvram_set("et1macaddr", "00:00:00:00:00:00");
        nvram_set("et1mdcport", "0");
        nvram_set("et1phyaddr", "30");
        nvram_set("et2macaddr", acosNvramConfig_get("lan_hwaddr"));
        nvram_set("et2mdcport", "0");
        nvram_set("et2phyaddr", "30");

        nvram_set("vlan1hwname", "et2");
        nvram_set("vlan2hwname", "et2");

        nvram_set("landevs", "vlan1 wl0 wl1 wl2 wl0.1 wl1.1 wl2.1");
        nvram_set("fwd_cpumap", "d:x:2:169:1 d:l:5:169:1 d:u:5:163:0");
        nvram_set("fwd_wlandevs", "eth1 eth2 eth3");
        nvram_set("fwddevs", "fwd0 fwd1");
    }
    else
    {
        nvram_set("wandevs", "et0");
        nvram_set("et0macaddr", acosNvramConfig_get("lan_hwaddr"));
        nvram_set("et0mdcport", "0");
        nvram_set("et0phyaddr", "30");
        nvram_unset("et1macaddr");
        nvram_unset("et2macaddr");
      	nvram_unset("fwd_wlandevs");

  	    nvram_unset("fwddevs");
        nvram_set("vlan1hwname", "et0");
        nvram_set("vlan2hwname", "et0");

    }	
    nvram_set("lan_ifname", "br0");


#endif

#ifdef __CONFIG_IGMP_SNOOPING__
		config_switch();
		//if (nvram_match("enable_vlan", "enable")) 
				config_iptv_params();
#endif
		/* Foxconn added end pling 09/02/2010 */

        /* foxconn added start by Bob 12/12/2013, BRCM suggest not to enable rxchain power save */
        nvram_set("wl_rxchain_pwrsave_enable", "0");
        nvram_unset("wl0_rxchain_pwrsave_enable");
        nvram_unset("wl1_rxchain_pwrsave_enable");

#ifdef AX6000 /*foxconn Han edited, 03/02/2018, disband5grp only needed for tri-band projects*/
		nvram_unset("3:disband5grp");
		nvram_unset("2:disband5grp");
#else /* !AX6000*/
#ifdef BCA_HNDROUTER
		nvram_set("3:disband5grp","0x7");
		nvram_set("2:disband5grp","0x18");
#else /* !BCA_HNDROUTER */
		nvram_set("0:disband5grp","0x7");
		nvram_set("2:disband5grp","0x18");
#endif /* !BCA_HNDROUTER */		
#endif /* AX6000*/
        /* foxconn added end by Bob 12/12/2013, BRCM suggest not to enable rxchain power save */

        /* Foxconn Bob added start 01/26/2015, clear all FXCN defined DFS related nvram flag */
        nvram_unset("eth1_dfs_OOC");
        nvram_unset("eth2_dfs_OOC");
        nvram_unset("eth3_dfs_OOC");
        nvram_unset("eth1_dfs_detected");
        nvram_unset("eth2_dfs_detected");
        nvram_unset("eth3_dfs_detected");
        /* Foxconn Bob added end 01/26/2015, clear all FXCN defined DFS related nvram flag */
        
        /* Foxconn Bob added start 07/24/2015, force disable PMF to fix IOT issue with Nexus 5 */
#ifdef MFP		
        disable_mfp();
#endif		
        /* Foxconn Bob added end 07/24/2015, force disable PMF to fix IOT issue with Nexus 5 */
        
        /* Foxconn Bob added start on 08/03/2015, workaround for dongle trap issue */
#ifdef BCA_HNDROUTER
#else /* !BCA_HNDROUTER */
        nvram_set("0:cpuclk","800");
        nvram_set("1:cpuclk","800");
        nvram_set("2:cpuclk","800");
#endif /* !BCA_HNDROUTER */		
        /* Foxconn Bob added end on 08/03/2015, workaround for dongle trap issue */

        /* Foxconn Bob added start on 09/15/2015, correct WMM parameters */
        
        nvram_set("wl_wme_sta_be", "15 1023 3 0 0 off off");
        nvram_set("wl_wme_sta_bk", "15 1023 7 0 0 off off");
        nvram_set("wl_wme_sta_vi", "7 15 2 6016 3008 off off");
        nvram_set("wl_wme_sta_vo", "3 7 2 3264 1504 off off");
        nvram_set("wl_wme_ap_be", "15 63 3 0 0 off off");
        nvram_set("wl_wme_ap_bk", "15 1023 7 0 0 off off");
        nvram_set("wl_wme_ap_vi", "7 15 1 6016 3008 off off");
        nvram_set("wl_wme_ap_vo", "3 7 1 3264 1504 off off");
        
        nvram_set("wl0_wme_sta_be", "15 1023 3 0 0 off off");
        nvram_set("wl0_wme_sta_bk", "15 1023 7 0 0 off off");
        nvram_set("wl0_wme_sta_vi", "7 15 2 6016 3008 off off");
        nvram_set("wl0_wme_sta_vo", "3 7 2 3264 1504 off off");
        nvram_set("wl0_wme_ap_be", "15 63 3 0 0 off off");
        nvram_set("wl0_wme_ap_bk", "15 1023 7 0 0 off off");
        nvram_set("wl0_wme_ap_vi", "7 15 1 6016 3008 off off");
        nvram_set("wl0_wme_ap_vo", "3 7 1 3264 1504 off off");
        
        nvram_set("wl1_wme_sta_be", "15 1023 3 0 0 off off");
        nvram_set("wl1_wme_sta_bk", "15 1023 7 0 0 off off");
        nvram_set("wl1_wme_sta_vi", "7 15 2 6016 3008 off off");
        nvram_set("wl1_wme_sta_vo", "3 7 2 3264 1504 off off");
        nvram_set("wl1_wme_ap_be", "15 63 3 0 0 off off");
        nvram_set("wl1_wme_ap_bk", "15 1023 7 0 0 off off");
        nvram_set("wl1_wme_ap_vi", "7 15 1 6016 3008 off off");
        nvram_set("wl1_wme_ap_vo", "3 7 1 3264 1504 off off");
        
        #if defined(INCLULDE_2ND_5G_RADIO) 
        nvram_set("wl2_wme_sta_be", "15 1023 3 0 0 off off");
        nvram_set("wl2_wme_sta_bk", "15 1023 7 0 0 off off");
        nvram_set("wl2_wme_sta_vi", "7 15 2 6016 3008 off off");
        nvram_set("wl2_wme_sta_vo", "3 7 2 3264 1504 off off");
        nvram_set("wl2_wme_ap_be", "15 63 3 0 0 off off");
        nvram_set("wl2_wme_ap_bk", "15 1023 7 0 0 off off");
        nvram_set("wl2_wme_ap_vi", "7 15 1 6016 3008 off off");
        nvram_set("wl2_wme_ap_vo", "3 7 1 3264 1504 off off");
        #endif
        /* Foxconn Bob added end on 09/15/2015, correct WMM parameters */
       
#ifndef BCA_HNDROUTER   /*R8000P gbsd not do this setting*/
        nvram_set("gbsd_wait_rssi_intf_idx", "2");  /* Foxconn Bob added on 10/14/2015 to force gbsd rssi interface to wl2 */
#endif

#ifdef __CONFIG_DHDAP__
#ifdef BCA_HNDROUTER
#ifdef BCA_HND_EAP
		/* For EAP builds, NIC mode is needed by default. Exclude dhd */
		modules = nvram_get("kernel_mods") ? : "hnd emf igs wl";
#else
		modules = nvram_get("kernel_mods") ? : "hnd emf igs dpsta dhd wl";
#endif /* BCA_HND_EAP */
#else
#ifdef BCM_MOCA
		modules = nvram_get("kernel_mods") ? : "et bmoca bcm57xx dpsta dhd wl";
#else
		modules = nvram_get("kernel_mods") ? : "et bcm57xx dpsta dhd wl";
#endif /* BCM_MOCA */
#endif /* BCA_HNDROUTER */
#else
#ifdef BCA_HNDROUTER
		modules = nvram_get("kernel_mods") ? : "hnd emf igs dpsta wl";
#else
#ifdef BCM_MOCA
		modules = nvram_get("kernel_mods") ? : "et bmoca bcm57xx dpsta wl";
#else
		modules = nvram_get("kernel_mods") ? : "et bcm57xx dpsta wl";
#endif /* BCM_MOCA */
#endif /* BCA_HNDROUTER */
#endif /* __CONFIG_DHDAP__ */

		printf("==== modules = %s \n", modules);
		foreach(module, modules, next) {
			char tmp[100];
#ifdef __CONFIG_DHDAP__
			/* For DHD, additional module params have to be passed. */
			if ((strcmp(module, "dhd") == 0) || (strcmp(module, "wl") == 0)) {
				int	i = 0, maxwl_eth = 0, maxunit = -1;
				int	unit = -1;
				char ifname[16] = {0};
				char instance_base[128];

				/* Search for existing wl devices and the max unit number used */
				for (i = 1; i <= DEV_NUMIFS; i++) {
					snprintf(ifname, sizeof(ifname), "eth%d", i);
					if (!wl_probe(ifname)) {
						if (!wl_ioctl(ifname, WLC_GET_INSTANCE, &unit,
							sizeof(unit))) {
							maxwl_eth = i;
							maxunit = (unit > maxunit) ? unit : maxunit;
						}
					}
				}
				snprintf(instance_base, sizeof(instance_base), "instance_base=%d",
					maxunit + 1);

#ifdef BCA_HNDROUTER
				snprintf(tmp, sizeof(tmp), "insmod %s/extra/%s.ko %s", buf, module, instance_base);
#else
				snprintf(tmp, sizeof(tmp), "insmod %s %s", module, instance_base);
#endif // endif

				system(tmp);
			} else
#endif /* __CONFIG_DHDAP__ */
			{
#ifdef BCA_HNDROUTER
				snprintf(tmp, sizeof(tmp), "insmod %s/extra/%s.ko", buf, module);
#else
				snprintf(tmp, sizeof(tmp), "insmod %s", module);
#endif // endif
				system(tmp);
			}
			printf("insmod %s Done\n", module);

		}

#if defined(BCA_HNDROUTER) && defined(PORT_BONDING)
		{
            char tmp[256];
            snprintf(tmp, sizeof(tmp), "insmod %s/kernel/drivers/net/bonding/bonding.ko "
                "miimon=1000 mode=4 ad_select=2 xmit_hash_policy=1 all_slaves_active=1", buf);
            system(tmp);
		}
#endif /* BCA_HNDROUTER && PORT_BONDING */

		/* Update the nvram settings with the appropriate driver
		 * mode (nic/dongle) for each of the wlan radios.
		 */
		wl_driver_mode_update();

		hotplug_usb_init();
#endif /* __CONFIG_STBAP__ */

#ifdef __CONFIG_USBAP__
		eval("insmod", "usbcore");
        /* Foxconn, [MJ] start, we can't insert usb-storage easiler than
         * automount being started. */
#if 0
		eval("insmod", "usb-storage");
        /* Foxconn, [MJ], for debugging. */
        cprintf("--> insmod usb-storage.\n");
#endif
        /* Foxconn, [MJ] end, we can't insert usb-storage easiler than
         * automount being started. */
		{
			char	insmod_arg[128];
			int	i = 0, maxwl_eth = 0, maxunit = -1;
			char	ifname[16] = {0};
			int	unit = -1;
			char arg1[20] = {0};
			char arg2[20] = {0};
			char arg3[20] = {0};
			char arg4[20] = {0};
			char arg5[20] = {0};
			char arg6[20] = {0};
			char arg7[20] = {0};
			const int wl_wait = 3;	/* max wait time for wl_high to up */

			/* Save QTD cache params in nvram */
			sprintf(arg1, "log2_irq_thresh=%d", atoi(nvram_safe_get("ehciirqt")));
			sprintf(arg2, "qtdc_pid=%d", atoi(nvram_safe_get("qtdc_pid")));
			sprintf(arg3, "qtdc_vid=%d", atoi(nvram_safe_get("qtdc_vid")));
			sprintf(arg4, "qtdc0_ep=%d", atoi(nvram_safe_get("qtdc0_ep")));
			sprintf(arg5, "qtdc0_sz=%d", atoi(nvram_safe_get("qtdc0_sz")));
			sprintf(arg6, "qtdc1_ep=%d", atoi(nvram_safe_get("qtdc1_ep")));
			sprintf(arg7, "qtdc1_sz=%d", atoi(nvram_safe_get("qtdc1_sz")));

			eval("insmod", "ehci-hcd", arg1, arg2, arg3, arg4, arg5,
				arg6, arg7);

			/* Search for existing PCI wl devices and the max unit number used.
			 * Note that PCI driver has to be loaded before USB hotplug event.
			 * This is enforced in rc.c
			 */
			for (i = 1; i <= DEV_NUMIFS; i++) {
				sprintf(ifname, "eth%d", i);
				if (!wl_probe(ifname)) {
					if (!wl_ioctl(ifname, WLC_GET_INSTANCE, &unit,
						sizeof(unit))) {
						maxwl_eth = i;
						maxunit = (unit > maxunit) ? unit : maxunit;
					}
				}
			}

			/* Set instance base (starting unit number) for USB device */
			sprintf(insmod_arg, "instance_base=%d", maxunit + 1);
			eval("insmod", "wl_high", insmod_arg);

			/* Hold until the USB/HSIC interface is up (up to wl_wait sec) */
			sprintf(ifname, "eth%d", maxwl_eth + 1);
			i = wl_wait;
			while (wl_probe(ifname) && i--) {
				sleep(1);
			}
			if (!wl_ioctl(ifname, WLC_GET_INSTANCE, &unit, sizeof(unit)))
				cprintf("wl%d is up in %d sec\n", unit, wl_wait - i);
			else
				cprintf("wl%d not up in %d sec\n", unit, wl_wait);
		}
#ifdef LINUX26
		mount("usbdeffs", "/proc/bus/usb", "usbfs", MS_MGC_VAL, NULL);
#else
		mount("none", "/proc/bus/usb", "usbdevfs", MS_MGC_VAL, NULL);
#endif /* LINUX26 */
#endif /* __CONFIG_USBAP__ */

#ifdef __CONFIG_WCN__
		modules = "scsi_mod sd_mod usbcore usb-ohci usb-storage fat vfat msdos";
		foreach(module, modules, next){
            /* Foxconn, [MJ] for debugging. */
            cprintf("--> insmod %s\n", ,module);
			eval("insmod", module);
		}
#endif // endif

#ifdef __CONFIG_SOUND__
		modules = "soundcore snd snd-timer snd-page-alloc snd-pcm snd-pcm-oss "
		        "snd-soc-core i2c-core i2c-algo-bit i2c-gpio snd-soc-bcm947xx-i2s "
		        "snd-soc-bcm947xx-pcm snd-soc-wm8750 snd-soc-wm8955 snd-soc-bcm947xx";
		foreach(module, modules, next)
			eval("insmod", module);
		mknod("/dev/dsp", S_IRWXU|S_IFCHR, makedev(14, 3));
		if (mkdir("/dev/snd", 0777) < 0 && errno != EEXIST) perror("/dev/snd not created");
		mknod("/dev/snd/controlC0", S_IRWXU|S_IFCHR, makedev(116, 0));
		mknod("/dev/snd/pcmC0D0c", S_IRWXU|S_IFCHR, makedev(116, 24));
		mknod("/dev/snd/pcmC0D0p", S_IRWXU|S_IFCHR, makedev(116, 16));
		mknod("/dev/snd/timer", S_IRWXU|S_IFCHR, makedev(116, 33));
#endif // endif

#ifndef __CONFIG_STBAP__
#ifdef LINUX_2_6_36
		/* To combat hotplug event lost because it could possibly happen before
		 * Rootfs is mounted or rc (preinit) is invoked during kernel boot-up with
		 * USB device attached.
		 */
		{
			uint chipid = 0, chiprev = 0, chippkg = 0;

			get_chipinfo(&chipid, &chiprev, &chippkg);

			/* 53573/53574/47189 only supports USB2.0 */
			modules = nvram_get("hci_mods") ? :
				((BCM53573_CHIP(chipid)) ?
				"ehci-hcd ohci-hcd" : "xhci-hcd ehci-hcd ohci-hcd");
		}

		foreach(module, modules, next)
			eval("insmod", module);
#endif /* LINUX_2_6_36 */
#endif /* __CONFIG_STBAP__ */

#if (defined(LINUX_2_6_36) || defined(BCA_HNDROUTER)) && defined(__CONFIG_TREND_IQOS__)
		{
			struct stat file_stat;

			system("echo 300 > /proc/sys/net/ipv4/netfilter/ip_conntrack_udp_timeout");
			system("echo 300 > /proc/sys/net/ipv4/netfilter/ip_conntrack_udp_timeout_stream");
			system("echo 1800 > /proc/sys/net/ipv4/netfilter/ip_conntrack_tcp_timeout_established");
			system("echo 131072 > /proc/sys/net/ipv4/netfilter/ip_conntrack_max");
            /* contrack table turning for ACOSNAT end */
            nvram_set("brcm_speedtest",   "0");
            /* Foxconn modified start, Sinclair, 10/22/2015@ BRCM_GENERIC_IQOS */
#ifdef __BRCM_GENERIC_IQOS__
			if (stat("/usr/sbin/qos.conf", &file_stat) == 0) {

				if (mkdir(IQOS_RUNTIME_FOLDER, 0777) < 0 && errno != EEXIST)
    				perror("IQOS_RUNTIME_FOLDER not created");
				else {
					eval("cp", "/usr/sbin/tdts_rule_agent", "/tmp/trend");
					eval("cp", "/usr/sbin/setup.sh", "/tmp/trend");
					eval("cp", "/usr/sbin/rule.trf", "/tmp/trend");
					eval("cp", "/usr/sbin/app_patrol_all.conf", "/tmp/trend");
					eval("cp", "/usr/sbin/app_patrol_clear.conf", "/tmp/trend");
					eval("cp", "/usr/sbin/app_patrol.conf", "/tmp/trend");
					eval("cp", "/usr/sbin/sample.bin", "/tmp/trend");
					eval("cp", "/usr/sbin/qos.conf", "/tmp/trend");
					eval("cp", "/usr/sbin/moni.sh", "/tmp/trend");
					eval("cp", "/usr/sbin/patrol_tq.conf", "/tmp/trend");
					// copy preload file
					eval("cp", "/lib/modules/4.1.51/extra/tdts.ko", "/tmp/trend");
					eval("cp", "/lib/modules/4.1.51/extra/tdts_udb.ko", "/tmp/trend");
					eval("cp", "/lib/modules/4.1.51/extra/tdts_udbfw.ko", "/tmp/trend");
					eval("cp", "/usr/sbin/iqos-setup.sh", "/tmp/trend");
					eval("cp", "/usr/sbin/clean-cache.sh", "/tmp/trend");
					eval("cp", "/usr/sbin/shn_ctrl", "/tmp/trend");
					eval("cp", "/usr/sbin/tcd", "/tmp/trend");
					eval("cp", "/usr/sbin/TmToNtgr_dev_mapping", "/tmp/trend");
				}
			}
#else  /* BRCM_GENERIC_IQOS */
			if (stat("/usr/sbin/qosd.conf", &file_stat) == 0) {
				if (mkdir("/tmp/trend", 0777) < 0 && errno != EEXIST)
					perror("/tmp/trend not created");
				else {
					eval("cp", "/lib/modules/IDP.ko", "/tmp/trend");
					eval("cp", "/lib/modules/bw_forward.ko", "/tmp/trend");
					eval("cp", "/lib/modules/tc_cmd.ko", "/tmp/trend");
					eval("cp", "/usr/sbin/bwdpi-rule-agent", "/tmp/trend");
					eval("cp", "/usr/sbin/rule.trf", "/tmp/trend");
					eval("cp", "/usr/sbin/setup.sh", "/tmp/trend");
					eval("cp", "/usr/sbin/upgrade.sh", "/tmp/trend");
					eval("cp", "/usr/sbin/qosd.conf", "/tmp/trend");
					eval("cp", "/usr/sbin/idpfw", "/tmp/trend");
					eval("cp", "/usr/sbin/tmdbg", "/tmp/trend");
					eval("cp", "/usr/sbin/TmToNtgr_dev_mapping", "/tmp/trend");
					eval("cp", "/usr/sbin/rule.version", "/tmp/trend");
				}
			}
#endif  /* BRCM_GENERIC_IQOS */
            /* Foxconn modified end, Sinclair, 10/22/2015@ BRCM_GENERIC_IQOS */
		}
#if defined(BCA_HNDROUTER)
        // Init iQos database once to get version content
        system("cp /data/qos.conf /tmp/trend");
        system("cp /data/rule.trf /tmp/trend");
        system("cp /data/TmToNtgr_dev_mapping /tmp/trend");
        system("cp /data/TMDPI/archive/* /tmp/trend");
        system("cd /tmp/trend; tdts_rule_agent -g");
        nvram_set("iqos_init_once","1");
        system("cd /tmp/trend; ./setup.sh stop;"); /*foxconn Han edited, 07/19/2018 remove tdts.ko first prevent setup.sh start failed*/
//        system("cd /tmp/trend; ./setup.sh start; cat /proc/ips_info > /tmp/trend/eng.version; ./setup.sh stop;");
#endif
#endif /* LINUX_2_6_36 && __CONFIG_TREND_IQOS__ */
	}

#ifdef BCA_HND_EAP
	/* Runner is enabled by default. Disable it unless specifically enabled in NVRAM */
	if (!nvram_match("eap_accel_enable", "1"))
	{
		eval("/bin/runner", "disable");
		eval("/bin/fcctl", "disable");
	}
#endif // endif

#ifndef __CONFIG_STBAP__
	if (memcmp(lx_rel, "2.6.36", 6) == 0) {
		int fd;
		if ((fd = open("/proc/irq/163/smp_affinity", O_RDWR)) >= 0) {
			close(fd);

			if (et_capable(NULL, "gmac3")) {
				char *fwd_cpumap;

				/* Place network interface vlan1/eth0 on CPU hosting 5G upper */
				fwd_cpumap = nvram_get("fwd_cpumap");

				if (fwd_cpumap == NULL) {
					/* BCM4709acdcrh: Network interface GMAC on Core#0
					 *    [5G+2G:163 on Core#0] and [5G:169 on Core#1].
					 *    Bind et2:vlan1:eth0:181 to Core#0
					 *    Note, USB3 xhci_hcd's irq#112 binds Core#1
					 *    bind eth0:181 to Core#1 impacts USB3 performance
					 */
					system("echo 1 > /proc/irq/181/smp_affinity");

				} else {

					char cpumap[32], *next;

					foreach(cpumap, fwd_cpumap, next) {
						char mode, chan;
						int band, irq, cpu;

						/* Format: mode:chan:band#:irq#:cpu# */
						if (sscanf(cpumap, "%c:%c:%d:%d:%d",
						           &mode, &chan, &band, &irq, &cpu) != 5) {
							break;
						}
						if (cpu > 1) {
							break;
						}
						/* Find the single 5G upper */
						if ((chan == 'u') || (chan == 'U')) {
							char command[128];
							snprintf(command, sizeof(command),
							    "echo %d > /proc/irq/181/smp_affinity",
							    1 << cpu);
							system(command);
							break;
						}
					}
				}

			} else { /* ! GMAC3 enabled */

				if (!nvram_match("txworkq", "1")) {
					system("echo 2 > /proc/irq/163/smp_affinity");
					system("echo 2 > /proc/irq/169/smp_affinity");
				}
			}

			system("echo 2 > /proc/irq/112/smp_affinity");
		}
	}
#ifdef BCA_HNDROUTER
	wl_thread_affinity_update();
#endif // endif
    /*foxconn Han edited, 09/18/2018 default disable fc when nmrp_test_mode == 1, for MFG testing*/
    if(nvram_match("nmrp_test_mode","1"))
    {
        system("echo \"fc disable for nmrp_test_mode=1\" > /dev/console");
        system("fc disable");
    }
	if (nvram_match("enable_ap_mode", "1") || nvram_match("enable_sta_mode", "1"))
		system("fc config --accel-mode 0"); //For circle, change accel from L2&L3 to L3 mode when AP/Bridge mode
	system("echo 20480 > /proc/sys/vm/min_free_kbytes");    /*Bob added on 09/05/2013, Set min free memory to 20Mbytes in case allocate memory failed */
	
	/* Set a sane date */
	stime(&tm);

#endif /* __CONFIG_STBAP__ */

	dprintf("done\n");
}

/* States */
enum {
	RESTART,
	STOP,
	START,
	TIMER,
	IDLE,
	WSC_RESTART,
	WLANRESTART, /* Foxconn added by EricHuang, 11/24/2006 */
	PPPSTART    /* Foxconn added by EricHuang, 01/09/2008 */
};
static int state = START;
static int signalled = -1;

/* foxconn added start, zacker, 05/20/2010, @spec_1.9 */
static int next_state = IDLE;

static int
next_signal(void)
{
	int tmp_sig = next_state;
	next_state = IDLE;
	return tmp_sig;
}
/* foxconn added end, zacker, 05/20/2010, @spec_1.9 */

/* Signal handling */
static void
rc_signal(int sig)
{
	if (state == IDLE) {	
		if (sig == SIGHUP) {
			dprintf("signalling RESTART\n");
			signalled = RESTART;
		}
		else if (sig == SIGUSR2) {
			dprintf("signalling START\n");
			signalled = START;
		}
		else if (sig == SIGINT) {
			dprintf("signalling STOP\n");
			signalled = STOP;
		}
		else if (sig == SIGALRM) {
			dprintf("signalling TIMER\n");
			signalled = TIMER;
		}
		else if (sig == SIGUSR1) {
			dprintf("signalling WSC RESTART\n");
			signalled = WSC_RESTART;
		}
		/* Foxconn modified start by EricHuang, 01/09/2008 */
		else if (sig == SIGQUIT) {
			printf("%s %d\n", __func__, __LINE__);
			dprintf("signalling WLANRESTART\n");
			signalled = WLANRESTART;
		}
		else if (sig == SIGILL) {
			signalled = PPPSTART;
		}
		/* Foxconn modified end by EricHuang, 01/09/2008 */
	}
	/* foxconn added start, zacker, 05/20/2010, @spec_1.9 */
	else if (next_state == IDLE)
	{
		if (sig == SIGHUP) {
			dprintf("signalling RESTART\n");
			next_state = RESTART;
		}
		else if (sig == SIGUSR2) {
			dprintf("signalling START\n");
			next_state = START;
		}
		else if (sig == SIGINT) {
			dprintf("signalling STOP\n");
			next_state = STOP;
		}
		else if (sig == SIGALRM) {
			dprintf("signalling TIMER\n");
			next_state = TIMER;
		}
		else if (sig == SIGUSR1) {
			dprintf("signalling WSC RESTART\n");
			next_state = WSC_RESTART;
		}
		else if (sig == SIGQUIT) {
			printf("signalling WLANRESTART\n");
			next_state = WLANRESTART;
		}
		else if (sig == SIGILL) {
			next_state = PPPSTART;
		}
	}
	/* foxconn added end, zacker, 05/20/2010, @spec_1.9 */
}

/* Get the timezone from NVRAM and set the timezone in the kernel
 * and export the TZ variable
 */
static void
set_timezone(void)
{
	/* Foxconn added start */
	time_t now;
	struct tm gm, local;
	struct timezone tz;
	struct timeval *tvp = NULL;
	/* Foxconn added end */

	/* Export TZ variable for the time libraries to
	 * use.
	 */
	setenv("TZ", nvram_safe_get("time_zone"), 1);

	/* Foxconn added start */
	/* Update kernel timezone */
	time(&now);
	gmtime_r(&now, &gm);
	localtime_r(&now, &local);
	tz.tz_minuteswest = (mktime(&gm) - mktime(&local)) / 60;
	settimeofday(tvp, &tz);
	/* Foxconn added end */

#if defined(__CONFIG_WAPI__) || defined(__CONFIG_WAPI_IAS__)
#ifndef	RC_BUILDTIME
#define	RC_BUILDTIME	1252636574
#endif // endif
	{
		struct timeval tv = {RC_BUILDTIME, 0};

		time(&now);
		if (now < RC_BUILDTIME)
			settimeofday(&tv, &tz);
	}
#endif /* __CONFIG_WAPI__ || __CONFIG_WAPI_IAS__ */
}

#ifdef __CONFIG_STBAP__
static void
rc_init(void)
{
	(void)rc_signal;
	(void)restore_defaults;

	/* Basic initialization */
	sysinit();
	/* Convert deprecated variables */
	convert_deprecated();

	/* Upgrade NVRAM variables to MBSS mode */
	upgrade_defaults();

	/* Always set OS defaults */
	nvram_set("os_name", "linux");
	nvram_set("os_version", ROUTER_VERSION_STR);
	nvram_set("os_date", __DATE__);

#ifdef __CONFIG_NAT__
	/* Auto Bridge if neccessary */
	if (!strcmp(nvram_safe_get("auto_bridge"), "1"))
	{
		auto_bridge();
	}
	/* Setup wan0 variables if necessary */
	set_wan0_vars();
#endif	/* __CONFIG_NAT__ */

#if defined(WLTEST) && defined(RWL_SOCKET)
	/* Shorten TCP timeouts to prevent system from running slow with rwl */
	system("echo \"10 10 10 10 3 3 10 10 10 10\">/proc/sys/net/ipv4/ip_conntrack_tcp_timeouts");
#endif /* WL_TEST && RWL_SOCKET */

#ifdef __CONFIG_FAILSAFE_UPGRADE_SUPPORT__
	failsafe_nvram_adjust();
#endif // endif
}

static void
rc_start(void)
{
#ifdef __CONFIG_VLAN__
	uint boardflags;

	/* Get boardflags to see if VLAN is supported */
	boardflags = strtoul(nvram_safe_get("boardflags"), NULL, 0);
#endif	/* __CONFIG_VLAN__ */

	pmon_init();
	{ /* Set log level on restart */
		char *loglevel;
		int loglev = 8;

		if ((loglevel = nvram_get("console_loglevel"))) {
			loglev = atoi(loglevel);
		}
		klogctl(8, NULL, loglev);
		if (loglev < 7) {
			printf("WARNING: console log level set to %d\n", loglev);
		}
	}

	set_timezone();
#ifdef __CONFIG_VLAN__
	if (boardflags & BFL_ENETVLAN)
		start_vlan();
#endif	/* __CONFIG_VLAN__ */
	start_lan();
	start_services();
#ifdef __CONFIG_NAT__
	start_wan();
#endif	/* __CONFIG_NAT__ */
#ifdef __CONFIG_IGMP_PROXY__
	start_igmp_proxy();
#endif /* __CONFIG_IGMP_PROXY__ */
	start_wl();
#ifdef __CONFIG_SOUND__
#ifdef __CONFIG_AMIXER__
	/* Start the Mixer */
	start_amixer();
#endif /* __CONFIG_AMIXER__ */
#endif /* __CONFIG_SOUND__ */
#ifdef __CONFIG_MDNSRESPONDER__
	/* start mdns responder */
	start_mdns();
	sleep(3);
#endif /* __CONFIG_MDNSRESPONDER__ */
#ifdef __CONFIG_AIRPLAY__
	/* Start Airplay */
	start_airplay();
#endif /* __CONFIG_AIRPLAY__ */
}

static void
rc_stop(void)
{
#ifdef __CONFIG_VLAN__
	uint boardflags;

	/* Get boardflags to see if VLAN is supported */
	boardflags = strtoul(nvram_safe_get("boardflags"), NULL, 0);
#endif	/* __CONFIG_VLAN__ */

	pmon_init();
	stop_services();
#ifdef __CONFIG_IGMP_PROXY__
	stop_igmp_proxy();
#endif /* __CONFIG_IGMP_PROXY__ */
#ifdef __CONFIG_NAT__
	stop_wan();
#endif	/* __CONFIG_NAT__ */
	stop_lan();
#ifdef __CONFIG_VLAN__
	if (boardflags & BFL_ENETVLAN)
		stop_vlan();
#endif	/* __CONFIG_VLAN__ */
#ifdef __CONFIG_SOUND__
#ifdef __CONFIG_AMIXER__
	stop_amixer();
#endif /* __CONFIG_AMIXER__ */
#ifdef __CONFIG_AIRPLAY__
	stop_airplay();
#endif /* __CONFIG_AIRPLAY__ */
#endif /* __CONFIG_SOUND__ */
#ifdef __CONFIG_MDNSRESPONDER__
	stop_mdns();
#endif /* __CONFIG_MDNSRESPONDER__ */
}

/* Settop box command handler */
static int
proc_stb(char *cmd)
{
	if (!strcmp(cmd,"init")) {
		dprintf("INIT\n");
		rc_init();
	} else if (!strcmp(cmd,"start")) {
		dprintf("START\n");
		rc_start();
	} else if (!strcmp(cmd,"stop")) {
		dprintf("STOP\n");
		rc_stop();
	} else if (!strcmp(cmd,"restart")) {
		dprintf("RESTART\n");
		rc_stop();
		rc_start();
	} else {
		fprintf(stderr, "Unknown command %s.  Usage: rc [init|start|stop|restart]\n", cmd);
		return EINVAL;
	}

	return 0;
}
#endif /* __CONFIG_STBAP__ */
/* Timer procedure.Gets time from the NTP servers once every timer interval
 * Interval specified by the NVRAM variable timer_interval
 */
#if defined(BCA_HNDROUTER) && defined(PORT_BONDING)
static int do_timer_counter = 0;
static bool bonding_drv_enabled = FALSE;
static bool bonding_failsafe_disable = FALSE;

int
do_timer(void)
{
	int ntp_interval = atoi(nvram_safe_get("timer_interval"));
	DIR *proc_bond_dir;

	dprintf("%d\n", ntp_interval);

	/* Bonding Failsafe if neccessary */
	if (!bonding_failsafe_disable && bonding_drv_enabled)
			bonding_failsafe();

	if (ntp_interval == 0) {
			alarm(1);
			return 0;
		}

	do_timer_counter++;
	if (do_timer_counter == ntp_interval) {
			/* Report stats */
			if (nvram_invmatch("stats_server", "")) {
						char *stats_argv[] = { "stats", nvram_get("stats_server"), NULL };
						_eval(stats_argv, NULL, 5, NULL);
					}

			start_ntpc();

			do_timer_counter = 0;
		}
	alarm(1);

	return 0;
}
#else
int
do_timer(void)
{
	int interval = atoi(nvram_safe_get("timer_interval"));

	dprintf("%d\n", interval);

	if (interval == 0)
		return 0;

	/* Report stats */
	if (nvram_invmatch("stats_server", "")) {
		char *stats_argv[] = { "stats", nvram_get("stats_server"), NULL };
		_eval(stats_argv, NULL, 5, NULL);
	}

	/* Sync time */
	start_ntpc();

	alarm(interval);

	return 0;
}
#endif /* BCA_HNDROUTER && PORT_BONDING */

static void
run_rc_local(void)
{
	char *cmd;
	struct stat tmp_stat;

	if ((cmd = nvram_get("rclocal")) != NULL &&
		stat(cmd, &tmp_stat) == 0) {
		system(cmd);
	}
}

#if defined(BCA_HNDROUTER) && defined(PORT_BONDING)
#define PROC_BONDING_DIR        "/proc/net/bonding"
#endif /* BCA_HNDROUTER && PORT_BONDING */

/* Main loop */
static void
main_loop(void)
{
	int defaults;
#ifdef CAPI_AP
	static bool start_aput = TRUE;
#endif // endif
	sigset_t sigset;
	pid_t shell_pid = 0;
	/* Foxconn added start */
#if defined(DUAL_TRI_BAND_HW_SUPPORT)	
   char hwver[32]={"R7800"};
#endif
	/* Foxconn added end */
#ifdef __CONFIG_VLAN__
	uint boardflags;
#endif // endif
#if defined(BCA_HNDROUTER) && defined(PORT_BONDING)
	DIR *proc_bond_dir;
#endif /* BCA_HNDROUTER && PORT_BONDING */

	defaults = RESTORE_DEFAULTS();

	/* Basic initialization */
	sysinit();



    /* Foxconn Bob added start on 03/30/3015, add a nvram hwver to indicate hw verion */
    #if defined(DUAL_TRI_BAND_HW_SUPPORT)
	bd_read_hwver(hwver,sizeof(hwver));


    /*foxconn Han edited, 07/02/2015 add for hwver and hwrev, don't save too many words in hwver*/
    hwver[5] = 0;

	hwver[31]=0;
	nvram_set("hwver", hwver);
	#endif
	/* Foxconn Bob added end on 03/30/3015, add a nvram hwver to indicate hw verion */


	/* Foxconn added start pling 06/26/2014 */
	/* R8000 TD99, Link down/up WAN ethernet for Comcast modem IPv6 compatibility issue*/
    abDisableWanEthernetPort();
	/* Foxconn added end pling 06/26/2014 */

	/* Foxconn added start pling 03/20/2014 */
	/* Router Spec Rev 12: disable/enable ethernet interface when dhcp server start */
	eval("landown");
	/* Foxconn added end pling 03/20/2014 */


	/* Add loopback */
	config_loopback();
	/* Restore defaults if necessary */
	//restore_defaults(); /* foxconn removed, zacker, 08/06/2010, move to sysinit() */


	/* Convert deprecated variables */
	convert_deprecated();

	/* Upgrade NVRAM variables to MBSS mode */
	upgrade_defaults();

    /* Read ethernet MAC, etc */
    //eval("read_bd"); /* foxconn removed, zacker, 08/06/2010, move to sysinit() */
    /* foxconn wklin added end, 10/22/2008 */

    /* Reset some wps-related parameters */
    nvram_set("wps_start",   "none");
    /* foxconn added start, zacker, 05/20/2010, @spec_1.9 */
    nvram_set("wps_status", "0"); /* start_wps() */
    nvram_set("wps_proc_status", "0");
    /* foxconn added end, zacker, 05/20/2010, @spec_1.9 */
    
    /* Foxconn Perry added start, 2011/05/13, for IPv6 router advertisment prefix information */
    /* reset IPv6 obsolete prefix information after reboot */
    nvram_set("radvd_lan_obsolete_ipaddr", "");
    nvram_set("radvd_lan_obsolete_ipaddr_length", "");
    nvram_set("radvd_lan_new_ipaddr", "");
    nvram_set("radvd_lan_new_ipaddr_length", "");
    /* Foxconn Perry added end, 2011/05/13, for IPv6 router advertisment prefix information */

    /* Foxconn added start, zacker, 06/17/2010, @new_tmp_lock */
    /* do this in case "wps_aplockdown_forceon" is set to "1" for tmp_lock
     * purpose but then there are "nvram_commit" and "reboot" action
     */
    if (nvram_match("wsc_pin_disable", "1"))
        nvram_set("wps_aplockdown_forceon", "1");
    else
        nvram_set("wps_aplockdown_forceon", "0");
    /* Foxconn added end, zacker, 06/17/2010, @new_tmp_lock */

    /* Foxconn add start, Max Ding, 02/26/2010 */
#ifdef RESTART_ALL_PROCESSES
    if (!nvram_match("restart_all_processes", "1"))
        nvram_set("wan_init_status", "first");
    nvram_unset("restart_all_processes");
#endif
    /* Foxconn add end, Max Ding, 02/26/2010 */
    eval("acos_init_once");
	/* Basic initialization */
	//sysinit();
#ifdef BCM_MOCA
	/* Downlod MoCa FW */
	eval("/usr/sbin/mocad", "-D");
#endif /* BCM_MOCA */

#ifndef __CONFIG_STBAP__
	/* Setup signal handlers */
	signal_init();
	signal(SIGHUP, rc_signal);
	signal(SIGUSR2, rc_signal);
	signal(SIGINT, rc_signal);
	signal(SIGALRM, rc_signal);
	signal(SIGUSR1, rc_signal);	
	signal(SIGQUIT, rc_signal); /* Foxconn added by EricHuang, 11/24/2006 */
	signal(SIGILL, rc_signal); //ppp restart
	sigemptyset(&sigset);

	if (nvram_match("rc_interrupt", "on")) {
		/* Give user a chance to run a shell before bringing up the rest of the system */
		if (!noconsole)
			run_shell(1, 0);
	}

	/* Get boardflags to see if VLAN is supported */
#ifdef __CONFIG_VLAN__
	boardflags = strtoul(nvram_safe_get("boardflags"), NULL, 0);
#endif	/* __CONFIG_VLAN__ */


#if 0 /* foxconn modified, wklin 10/22/2008, move the the start of this function */
	/* Add loopback */
	config_loopback();
#endif /* __CONFIG_STBAP__ */

	/* Convert deprecated variables */
	convert_deprecated();

	/* Upgrade NVRAM variables to MBSS mode */
	upgrade_defaults();

#ifndef __CONFIG_STBAP__
	/* Restore defaults if necessary */
	restore_defaults();

    /* Foxconn added start pling 06/20/2007 */
    /* Read board data again, since the "restore_defaults" action
     * above will overwrite some of our settings */
    eval("read_bd");
    /* Foxconn added end pling 06/20/2006 */
#endif /* 0 */
#ifdef LINUX_2_6_36
	/* Ajuest FA NVRAM variables */
	//fa_nvram_adjust();

	/* Ajuest GMAC3 NVRAM variables */
//	gmac3_nvram_adjust();
#endif // endif
#else
	/* Always set OS defaults */
	nvram_set("os_name", "linux");
	nvram_set("os_version", ROUTER_VERSION_STR);
	nvram_set("os_date", __DATE__);
#endif /* __CONFIG_STBAP__ */

#ifdef __CONFIG_NAT__
	/* Auto Bridge if neccessary */
	if (!strcmp(nvram_safe_get("auto_bridge"), "1"))
	{
		auto_bridge();
	}
	/* Setup wan0 variables if necessary */
	set_wan0_vars();
#endif	/* __CONFIG_NAT__ */

#if defined(WLTEST) && defined(RWL_SOCKET)
	/* Shorten TCP timeouts to prevent system from running slow with rwl */
	system("echo \"10 10 10 10 3 3 10 10 10 10\">/proc/sys/net/ipv4/ip_conntrack_tcp_timeouts");
#endif /* WL_TEST && RWL_SOCKET */

	/* Start up MTD OOPS crash logging */
	start_mtdoops_crash_logging();

#ifdef __CONFIG_FAILSAFE_UPGRADE_SUPPORT__
	failsafe_nvram_adjust();
#endif // endif


    /* Foxconn added start pling 07/13/2009 */
    /* create the USB semaphores */
#ifdef SAMBA_ENABLE
    usb_sem_init(); //[MJ] for 5G crash
#endif
    /* Foxconn added end pling 07/13/2009 */

#ifdef BCA_HNDROUTER
    /* Foxconn modify start, Max Ding, 11/22/2016 for vlan case when "wan_ifnames = vlanxx" */
    //enable_ethernet_intfs(nvram_get("wan_ifnames"), TRUE);
    //enable_ethernet_intfs(nvram_get("lan_ifnames"), FALSE);
    enable_ethernet_intfs("eth0", TRUE);
    enable_ethernet_intfs("eth1 eth2 eth3 eth4", FALSE);
#if (defined AX6000)
    enable_ethernet_intfs(LAN5_IF_NAME_NUM, FALSE);
#endif
    /* Foxconn modify end, Max Ding, 11/22/2016 */
#ifdef PORT_BONDING
	proc_bond_dir = opendir(PROC_BONDING_DIR);
	if (proc_bond_dir) {
		bonding_drv_enabled = TRUE;
		close(proc_bond_dir);
	}
#endif /* PORT_BONDING */
#endif /* BCA_HNDROUTER */

	run_rc_local();

	/* Loop forever */
	for (;;) {
		switch (state) {
		case RESTART:
			dprintf("RESTART\n");
			/* Fall through */
			/* Foxconn added start pling 06/14/2007 */
            /* When vista finished configuring this router (wl0_wps_config_state: 0->1),
             * then we come here to restart WLAN 
             */
            stop_wps();
			stop_nas();
            stop_eapd();
			stop_bcmupnp();
			stop_wlan();
				stop_bsd();

			/*Foxconn add start by Hank 06/14/2012*/
			/*Enable 2.4G auto channel detect, kill acsd for stop change channel*/
			//if((nvram_match("wla_channel", "0") || nvram_match("wlg_channel", "0")) && nvram_match("enable_sta_mode","0"))
			if(nvram_match("enable_sta_mode","0"))
				stop_acsd();
			/*Foxconn add end by Hank 06/14/2012*/

    	    convert_wlan_params();  /* For WCN , added by EricHuang, 12/21/2006 */
            sleep(2);               /* Wait some time for wsc, etc to terminate */

            /* if "unconfig" to "config" mode, force it to built-in registrar and proxy mode */
            /* added start by EricHuang, 11/04/2008 */
            if ( nvram_match("wps_status", "0") ) //restart wlan for wsc
            {
                nvram_set("lan_wps_reg", "enabled");
                nvram_set("wl_wps_reg", "enabled");
                nvram_set("wl0_wps_reg", "enabled");
#if (defined INCLUDE_DUAL_BAND)
                nvram_set("wl1_wps_reg", "enabled");
#if defined(INCLULDE_2ND_5G_RADIO) 
                nvram_set("wl2_wps_reg", "enabled");
#endif
#endif
                /* Foxconn modify start, Max Ding, 08/28/2010 for NEW_BCM_WPS */
                /* New NVRAM to BSP 5.22.83.0, 'wlx_wps_config_state' not used anymore. */
                //printf("restart -- wl0_wps_config_state=%s\n", nvram_get("wl0_wps_config_state"));
                //nvram_set("wl_wps_config_state", nvram_get("wl0_wps_config_state"));
                if ( nvram_match("lan_wps_oob", "enabled") )
                {
                    nvram_set("wl_wps_config_state", "0");
                    nvram_set("wl0_wps_config_state", "0");
#if (defined INCLUDE_DUAL_BAND)
                    nvram_set("wl1_wps_config_state", "0");
#if defined(INCLULDE_2ND_5G_RADIO) 
                    nvram_set("wl2_wps_config_state", "0");
#endif
#endif
                }
                else
                {
                    nvram_set("wl_wps_config_state", "1");
                    nvram_set("wl0_wps_config_state", "1");
#if (defined INCLUDE_DUAL_BAND)
                    nvram_set("wl1_wps_config_state", "1");
#if defined(INCLULDE_2ND_5G_RADIO) 
                    nvram_set("wl2_wps_config_state", "1");
#endif                    
#endif
                }
                /* Foxconn modify end, Max Ding, 08/28/2010 */
            }
            /* added end by EricHuang, 11/04/2008 */
            
            /* hide unnecessary warnings (Invaid XXX, out of range xxx etc...)*/
            {
                #include <fcntl.h>
                int fd1, fd2;
                fd1 = dup(2);
                fd2 = open("/dev/null", O_WRONLY);
                close(2);
                dup2(fd2, 2);
                close(fd2);
                start_wlan(); //<-- to hide messages generated here
                close(2);
                dup2(fd1, 2);
                close(fd1);
            }
            
            save_wlan_time();          
            start_bcmupnp();
            start_wps();            /* Foxconn modify by aspen Bai, 08/01/2008 */
            start_eapd();           /* Foxconn modify by aspen Bai, 10/08/2008 */
            start_nas();            /* Foxconn modify by aspen Bai, 08/01/2008 */
            if(nvram_match("enable_sta_mode","0"))
				start_acsd();
            sleep(2);               /* Wait for WSC to start */
            /* Foxconn add start by aspen Bai, 09/10/2008 */
            /* Must call it when start wireless */
            start_wl();
            /* Foxconn add end by aspen Bai, 09/10/2008 */
			/*Foxconn add start by Antony 06/16/2013 Start the bandsteering*/

    
      if((strcmp(nvram_safe_get("wla_ssid"),nvram_safe_get("wlg_ssid") )!=0))
          nvram_set("enable_band_steering", "0");      	

      if(strcmp(nvram_safe_get("wla_secu_type"),nvram_safe_get("wlg_secu_type") )!=0)
          nvram_set("enable_band_steering", "0");      	

      if(strcmp(nvram_safe_get("wla_secu_type"),"None") || strcmp(nvram_safe_get("wlg_secu_type"),"None"))
      {
          if(strcmp(nvram_safe_get("wla_passphrase"),nvram_safe_get("wlg_passphrase"))!=0) 
              nvram_set("enable_band_steering", "0");
      }
			if(nvram_match("enable_band_steering", "1") && nvram_match("wla_wlanstate", "Enable")&& nvram_match("wlg_wlanstate", "Enable"))
				start_bsd();
			/*Foxconn add end by Antony 06/16/2013*/
            #if defined(INCLULDE_2ND_5G_RADIO)
			if(nvram_match("wl_5g_bandsteering", "1") && nvram_match("wlg_wlanstate", "Enable")&& nvram_match("wlh_wlanstate", "Enable"))
				start_bsd();
            #endif

			/*Foxconn add start by Hank 06/14/2012*/
			/*Enable 2.4G auto channel detect, call acsd to start change channel*/
			//if((nvram_match("wla_channel", "0") || nvram_match("wlg_channel", "0")) && nvram_match("enable_sta_mode","0"))
			
			/*Foxconn add end by Hank 06/14/2012*/
            nvram_commit();         /* Save WCN obtained parameters */

			/* foxconn modified start, zacker, 05/20/2010, @spec_1.9 */
			//state = IDLE;
			state = next_signal();
			/* foxconn modified end, zacker, 05/20/2010, @spec_1.9 */

		
			break;
			/* Foxconn added end pling 06/14/2007 */

		case STOP:
			dprintf("STOP\n");
			pmon_init();
#if defined(BCA_HNDROUTER) && defined(PORT_BONDING)
			if (bonding_drv_enabled)
				stop_bonding();
#endif /* BCA_HNDROUTER && PORT_BONDING */
			stop_services();
#if defined(BCA_HNDROUTER) && defined(MCPD_PROXY)
			stop_mcpd_proxy();
#endif /* BCA_HNDROUTER && MCPD_PROXY */
#ifdef __CONFIG_NAT__
			stop_wan();
#endif	/* __CONFIG_NAT__ */

            if(nvram_match ("wireless_restart", "1"))
            {
                stop_wps();
                stop_nas();
                stop_eapd(); 
    			stop_bsd();
            }
            stop_bcmupnp();
			
			stop_lan();
#ifdef __CONFIG_VLAN__
			if (boardflags & BFL_ENETVLAN)
				stop_vlan();
#endif	/* __CONFIG_VLAN__ */
#ifdef __CONFIG_SOUND__
#ifdef __CONFIG_SALSA__
#ifdef __CONFIG_AMIXER__
			stop_amixer();
#endif /* __CONFIG_AMIXER__ */
#endif /* __CONFIG_SALSA__ */
#ifdef __CONFIG_AIRPLAY__
			stop_airplay();
#endif /* __CONFIG_AIRPLAY__ */
#ifdef __CONFIG_ALSACTL__
			stop_alsactl();
#endif /* __CONFIG_ALSACTL__ */
#endif /* __CONFIG_SOUND__ */
#ifdef __CONFIG_MDNSRESPONDER__
			stop_mdns();
#endif /* __CONFIG_MDNSRESPONDER__ */
			if (state == STOP) {
				/* foxconn modified start, zacker, 05/20/2010, @spec_1.9 */
				//state = IDLE;
				state = next_signal();
				/* foxconn modified end, zacker, 05/20/2010, @spec_1.9 */

				break;
			}
			/* Fall through */
		case START:
			dprintf("START\n");
			pmon_init();
			/* foxconn added start, zacker, 01/13/2012, @iptv_igmp */
#ifdef CONFIG_RUSSIA_IPTV
			if (!nvram_match("wla_repeater", "1")
#if (defined INCLUDE_DUAL_BAND)
				&& !nvram_match("wlg_repeater", "1")
#endif
				)
			{
				/* don't do this in cgi since "rc stop" need to do cleanup */
				config_iptv_params();
				/* always do this to active new vlan settings */
				active_vlan();
			}
#endif /* CONFIG_RUSSIA_IPTV */

#if (defined INCLUDE_QOS) || (defined __CONFIG_IGMP_SNOOPING__)
            if (!nvram_match("gmac3_enable", "1"))
			    config_switch_reg();
#endif
			/* foxconn added end, zacker, 01/13/2012, @iptv_igmp */
			
			if ( nvram_match("debug_port_mirror", "1"))
            {
#if defined(ET_CMD_NEW)
                system("ethswctl -c regaccess -v 0x0210 -d 0x8000 -l 2");
                system("ethswctl -c regaccess -v 0x0212 -d 0x110 -l 2");
                system("ethswctl -c regaccess -v 0x021C -d 0x110 -l 2");
#else
                system("et robowr 0x02 0x10 0x8000");
                system("et robowr 0x02 0x12 0x110");
                system("et robowr 0x02 0x1C 0x110");
#endif
            }
            
#if defined(R8000) || defined(BCA_HNDROUTER)
        system("et -i eth0 robowr 0x4 0x4 0");   /* Bob added on 07/17/2014, to enable STP forward */

        /*foxconn Han edited start, 07/31/2018 per CS4843779 add below commands for switch DVT test*/
        system("ethswctl -c hw-switching -o enable");
        system("ethswctl -c regaccess -v 0x0021 -d 0x0041 -l 2");
        system("ethswctl -c pause -p 0 -v 2");
        system("ethswctl -c pause -p 1 -v 2");
        system("ethswctl -c pause -p 2 -v 2");
        system("ethswctl -c pause -p 3 -v 2");
        system("ethswctl -c pause -p 7 -v 2");
        /*foxconn Han edited end, 07/31/2018 per CS4843779 add below commands for switch DVT test*/

#ifdef AX6000 /*AX6000 don't need dual_5g_band*/
		nvram_unset("dual_5g_band");
#else /*AX6000*/ /*AX6000 don't need dual_5g_band*/
#error "not define AX6000"
		if ( nvram_match("wla_region", "5"))
		    nvram_set("dual_5g_band","1");
		else
		    nvram_set("dual_5g_band","1");
#endif /*!AX6000*/
#endif
		/*foxconn added start, water, 12/21/09*/
#ifdef RESTART_ALL_PROCESSES
		if ( nvram_match("restart_all_processes", "1") )
		{
			restore_defaults();
			eval("read_bd");

            /* Foxconn Bob added start 07/24/2015, force disable PMF to fix IOT issue with Nexus 5 */
#ifdef MFP			
            disable_mfp();
#endif			
            /* Foxconn Bob added end 07/24/2015, force disable PMF to fix IOT issue with Nexus 5 */

			convert_deprecated();
			/* Foxconn add start, Max Ding, 03/03/2010 */

#if (defined BCM5325E) || (defined BCM53125) && !defined(BCA_HNDROUTER)
			system("/usr/sbin/et robowr 0x34 0x00 0x00e0");
#endif
			/* Foxconn add end, Max Ding, 03/03/2010 */
#if !defined(U12H245) && !defined(U12H264) && !defined(U12H268)
			if(acosNvramConfig_match("emf_enable", "1") )
			{
    			system("insmod emf");
    			system("insmod igs");
    			system("insmod wl");
			}
#endif			
		}
#endif

			{ /* Set log level on restart */
				char *loglevel;
				int loglev = 8;

				if ((loglevel = nvram_get("console_loglevel"))) {
					loglev = atoi(loglevel);
				}
				klogctl(8, NULL, loglev);
				if (loglev < 7) {
					printf("WARNING: console log level set to %d\n", loglev);
				}
			}

			set_timezone();
#ifdef __CONFIG_VLAN__
#ifdef BCA_HNDROUTER
            if (acosNvramConfig_match("enable_vlan", "enable")) /* Foxconn added by Max Ding, 11/22/2016 */
#else
			if (boardflags & BFL_ENETVLAN)
#endif
            {
				start_vlan();
            }
#endif	/* __CONFIG_VLAN__ */
            /* wklin modified start, 10/23/2008 */
            /* hide unnecessary warnings (Invaid XXX, out of range xxx etc...)*/
            {
                #include <fcntl.h>
                int fd1, fd2;
                fd1 = dup(2);
                fd2 = open("/dev/null", O_WRONLY);
                close(2);
                dup2(fd2, 2);
                close(fd2);
                start_lan(); //<-- to hide messages generated here
                if(nvram_match ("wireless_restart", "1"))
                    start_wlan(); //<-- need it to bring up 5G interface
                close(2);
                dup2(fd1, 2);
                close(fd1);
            }


            if (nvram_match("wla_repeater", "1")
#if (defined INCLUDE_DUAL_BAND)
            || nvram_match("wlg_repeater", "1")
#endif
            )
            {
                /* if repeater mode, del vlan1 from br0 and disable vlan */
#ifdef BCM4716
                system("/usr/sbin/brctl delif br0 vlan0");
                system("/usr/sbin/et robowr 0x34 0x00 0x00");
#else
                /*foxconn modified start, water, 01/07/10, @lan pc ping DUT failed when repeater mode & igmp enabled*/
                //system("/usr/sbin/brctl delif br0 vlan1");
                //system("/usr/sbin/et robowr 0x34 0x00 0x00");
#ifdef IGMP_PROXY
                if (!nvram_match("igmp_proxying_enable", "1"))
#endif
                {
                system("/usr/sbin/brctl delif br0 vlan1");
                system("/usr/sbin/et robowr 0x34 0x00 0x00");
                }
                /*foxconn modified end, water, 01/07/10*/
#endif
            }
            /* wklin modified end, 10/23/2008 */           
            save_wlan_time();
			start_bcmupnp();
			start_wps();
            if(nvram_match ("wireless_restart", "1"))
            {
                start_eapd();
                start_nas();
                if(nvram_match("enable_sta_mode","0") )
				    start_acsd();
                sleep(2);
#if defined(BCA_HNDROUTER) && defined(MCPD_PROXY)
                start_mcpd_proxy();
#endif /* BCA_HNDROUTER && MCPD_PROXY */
			start_wl();
//			start_services();
#if defined(BCA_HNDROUTER) && defined(PORT_BONDING)
			if (bonding_drv_enabled) {
				if (!strcmp(nvram_safe_get("bonding_failsafe_disable"), "1"))
					bonding_failsafe_disable = TRUE;
				else
					bonding_failsafe_disable = FALSE;

				start_bonding();
			}
#endif /* BCA_HNDROUTER && PORT_BONDING */
#if 0 //def __CONFIG_NAT__
			start_wan();
#endif	/* __CONFIG_NAT__ */

                if((strcmp(nvram_safe_get("wla_ssid"),nvram_safe_get("wlg_ssid") )!=0))
                    nvram_set("enable_band_steering", "0");      	

                if(strcmp(nvram_safe_get("wla_secu_type"),nvram_safe_get("wlg_secu_type") )!=0)
                    nvram_set("enable_band_steering", "0");      	

                if(strcmp(nvram_safe_get("wla_secu_type"),"None") || strcmp(nvram_safe_get("wlg_secu_type"),"None"))
                {
                    if(strcmp(nvram_safe_get("wla_passphrase"),nvram_safe_get("wlg_passphrase"))!=0) 
                        nvram_set("enable_band_steering", "0");
                }

			    if(nvram_match("enable_band_steering", "1") && nvram_match("wla_wlanstate", "Enable")&& nvram_match("wlg_wlanstate", "Enable"))
				    start_bsd();
                
                #if defined(INCLULDE_2ND_5G_RADIO)
			    if(nvram_match("wl_5g_bandsteering", "1") && nvram_match("wlh_wlanstate", "Enable")&& nvram_match("wlg_wlanstate", "Enable"))
				start_bsd();
				#endif
            }
            /* Now start ACOS services */

            /*foxconn Han edited, 10/02/2015*/
            //isDhdReady();
            /* Foxconn added start pling 06/26/2014 */
            /* R8000 TD99, Link down/up WAN ethernet for Comcast modem IPv6 compatibility issue*/
            abEnableWanEthernetPort();
            /* Foxconn added end pling 06/26/2014 */


            eval("acos_init");
            eval("acos_service", "start");


                /* Start wsc if it is in 'unconfiged' state, and if PIN is not disabled */
            if(nvram_match ("wireless_restart", "1"))
            {
                if (nvram_match("wl0_wps_config_state", "0") && !nvram_match("wsc_pin_disable", "1"))
                {
                    /* if "unconfig" to "config" mode, force it to built-in registrar and proxy mode */
                    nvram_set("wl_wps_reg", "enabled");
                    nvram_set("wl0_wps_reg", "enabled");
                    nvram_set("wps_proc_status", "0");
                    nvram_set("wps_method", "1");
                    //nvram_set("wps_config_command", "1");
                }

                /* Foxconn added start pling 03/30/2009 */
                /* Fix antenna diversiy per Netgear Bing's request */
#if 0//(!defined WNR3500v2VCNA)        // pling added 04/10/2009, vnca don't want fixed antenna
            eval("wl", "down");
            eval("wl", "nphy_antsel", "0x02", "0x02", "0x02", "0x02");
            eval("wl", "up");
#endif
                /* Foxconn added end pling 03/30/2009 */
                //eval("wl", "interference", "2");    // pling added 03/27/2009, per Netgear Fanny request

#if ( (defined SAMBA_ENABLE) || (defined HSDPA) )
                if (!acosNvramConfig_match("wla_wlanstate", "Enable") || acosNvramConfig_match("wifi_on_off", "0"))
                {/*water, 05/15/2009, @disable wireless, router will reboot continually*/
                 /*on WNR3500L, WNR3500U, MBR3500, it was just a work around..*/
                    eval("wl", "down");
                }
#endif
            }
            nvram_set ("wireless_restart", "1");

			if (defaults) {
				if (!(strcmp(nvram_safe_get("devicemode"), "psr"))) {
					set_psr_vars(0);
				}
			}

			if (strcmp(nvram_safe_get("wl_mode"), "psr") != 0) {
				/* If we are not in PSTA Repeater mode start the sound config */
#ifdef __CONFIG_SOUND__
#ifdef __CONFIG_ALSACTL__
				start_alsactl();
#endif /* __CONFIG_ALSACTL__ */
#ifdef __CONFIG_SALSA__
#ifdef __CONFIG_AMIXER__
				/* Start the Mixer */
				start_amixer();
#endif /* __CONFIG_AMIXER__ */
#endif /* __CONFIG_SALSA__ */
#endif /* __CONFIG_SOUND__ */

#ifdef __CONFIG_MDNSRESPONDER__
				/* Start Airplay */
				start_mdns();
				sleep(3);
#endif /* __CONFIG_MDNSRESPONDER__ */
#ifdef __CONFIG_AIRPLAY__
				start_airplay();
#endif /* __CONFIG_AIRPLAY__ */
			}
			/* Fall through */
		case TIMER:
            /* Foxconn removed start pling 07/12/2006 */
#if 0
			dprintf("TIMER\n");
			do_timer();
#endif
            /* Foxconn removed end pling 07/12/2006 */
			/* Fall through */
		case IDLE:
			dprintf("IDLE\n");
			/* foxconn modified start, zacker, 05/20/2010, @spec_1.9 */
			//state = IDLE;
			state = next_signal();
			if (state != IDLE)
				break;
			/* foxconn modified end, zacker, 05/20/2010, @spec_1.9 */
#ifdef CAPI_AP
			if (start_aput == TRUE) {
				system("/usr/sbin/wfa_aput_all&");
				start_aput = FALSE;
			}
#endif /* CAPI_AP */

			/* foxconn added start, zacker, 09/17/2009, @wps_led */
			if (nvram_match("wps_start",   "none"))
			    /* Foxconn add modified, Tony W.Y. Wang, 12/03/2009 */
				//send_wps_led_cmd(WPS_LED_BLINK_OFF, 0);
				if (acosNvramConfig_match("dome_led_status", "ON"))
                    send_wps_led_cmd(WPS_LED_BLINK_OFF, 3);
                else if (acosNvramConfig_match("dome_led_status", "OFF"))
                    send_wps_led_cmd(WPS_LED_BLINK_OFF, 2);
			/* foxconn added end, zacker, 09/17/2009, @wps_led */

			/* Wait for user input or state change */
			while (signalled == -1) {
				if (!noconsole && (!shell_pid || kill(shell_pid, 0) != 0))
					shell_pid = run_shell(0, 1);
				else {

					sigsuspend(&sigset);
				}
#ifdef LINUX26
#ifdef USBAP
				system("echo 4096 > /proc/sys/vm/min_free_kbytes");
#endif // endif
				/*Foxconn modify start by Hank 07/31/2013*/
				/*for speed up USB3.0 throughput*/
				system("echo 1 > /proc/sys/vm/drop_caches");
				//system("echo 4096 > /proc/sys/vm/min_free_kbytes");
				/*Foxconn modify end by Hank 07/31/2013*/
#elif defined(__CONFIG_SHRINK_MEMORY__)
				eval("cat", "/proc/shrinkmem");
#endif	/* LINUX26 */
			}
			state = signalled;
			signalled = -1;
			break;

		case WSC_RESTART:
			dprintf("WSC_RESTART\n");
			/* foxconn modified start, zacker, 05/20/2010, @spec_1.9 */
			//state = IDLE;
			state = next_signal();
			/* foxconn modified end, zacker, 05/20/2010, @spec_1.9 */
			stop_wps();    /* Foxconn modify by aspen Bai, 08/01/2008 */
			start_wps();    /* Foxconn modify by aspen Bai, 08/01/2008 */
			break;

            /* Foxconn added start pling 06/14/2007 */
            /* We come here only if user press "apply" in Wireless GUI */
		case WLANRESTART:
		    dprintf("WLANRESTART\n");
            stop_wps(); 
		    stop_nas();
            stop_eapd();
            stop_bcmupnp();

			/*Foxconn add start by Antony 06/16/2013*/
				stop_bsd();
			/*Foxconn add end by Antony 06/16/2013*/
            
			stop_wlan();
            
			/*Foxconn add start by Hank 06/14/2012*/
			/*Enable 2.4G auto channel detect, kill acsd stop change channel*/
			//if((nvram_match("wla_channel", "0") || nvram_match("wlg_channel", "0")) && nvram_match("enable_sta_mode","0"))
			if(nvram_match("enable_sta_mode","0"))
	            stop_acsd();
			/*Foxconn add end by Hank 06/14/2012*/
			eval("read_bd");    /* sync foxconn and brcm nvram params */
                       
            /* Foxconn Bob added start 07/24/2015, force disable PMF to fix IOT issue with Nexus 5 */
#ifdef MFP			
            disable_mfp();
#endif			
            /* Foxconn Bob added end 07/24/2015, force disable PMF to fix IOT issue with Nexus 5 */

            /* wklin modified start, 01/29/2007 */
            /* hide unnecessary warnings (Invaid XXX, out of range xxx etc...)*/
            {
                #include <fcntl.h>
                int fd1, fd2;
                fd1 = dup(2);
                fd2 = open("/dev/null", O_WRONLY);
                close(2);
                dup2(fd2, 2);
                close(fd2);
                start_wlan(); //<-- to hide messages generated here
                close(2);
                dup2(fd1, 2);
                close(fd1);
            }
            /* wklin modified end, 01/29/2007 */
            #if 0
            /* Foxconn add start, Tony W.Y. Wang, 03/25/2010 @Single Firmware Implementation */
            if (nvram_match("sku_name", "NA"))
            {
                printf("set wl country and power of NA\n");
                eval("wl", "country", "Q1/15");
                /* Foxconn modify start, Max Ding, 12/27/2010 "US/39->US/8" for open DFS band 2&3 channels */
                //eval("wl", "-i", "eth2", "country", "US/39");
                eval("wl", "-i", "eth2", "country", "Q1/15");
                /* Foxconn modify end, Max Ding, 12/27/2010 */
                /* Foxconn remove start, Max Ding, 12/27/2010 fix time zone bug for NA sku */
                //nvram_set("time_zone", "-8");
                /* Foxconn remove end, Max Ding, 12/27/2010 */
                nvram_set("wla_region", "11");
                nvram_set("wla_temp_region", "11");
                nvram_set("wl_country", "Q1");
                nvram_set("wl_country_code", "Q1");
                nvram_set("ver_type", "NA");
            }
            /*
            else if (nvram_match("sku_name", "WW"))
            {
                printf("set wl country and power of WW\n");
                eval("wl", "country", "EU/5");
                eval("wl", "-i", "eth2", "country", "EU/5");
                nvram_set("time_zone", "0");
                nvram_set("wla_region", "5");
                nvram_set("wla_temp_region", "5");
                nvram_set("wl_country", "EU5");
                nvram_set("wl_country_code", "EU5");
                nvram_set("ver_type", "WW");
            }
            */
            /* Foxconn add end, Tony W.Y. Wang, 03/25/2010 @Single Firmware Implementation */
            #endif
            
            save_wlan_time();
            start_bcmupnp();
            start_wps();
            start_eapd();
            start_nas();
            if(nvram_match("enable_sta_mode","0") )
			//if((nvram_match("wla_channel", "0") || nvram_match("wlg_channel", "0")) && nvram_match("enable_sta_mode","0"))
				start_acsd();
            sleep(2);           /* Wait for WSC to start */
            start_wl();
#ifdef ARP_PROTECTION
            config_arp_table();
#endif 
		/*Foxconn add start by Antony 06/16/2013 Start the bandsteering*/
    
      if((strcmp(nvram_safe_get("wla_ssid"),nvram_safe_get("wlg_ssid") )!=0))
          nvram_set("enable_band_steering", "0");      	

      if(strcmp(nvram_safe_get("wla_secu_type"),nvram_safe_get("wlg_secu_type") )!=0)
          nvram_set("enable_band_steering", "0");      	

      if(strcmp(nvram_safe_get("wla_secu_type"),"None") || strcmp(nvram_safe_get("wlg_secu_type"),"None"))
      {
          if(strcmp(nvram_safe_get("wla_passphrase"),nvram_safe_get("wlg_passphrase"))!=0) 
              nvram_set("enable_band_steering", "0");
      }

			if(nvram_match("enable_band_steering", "1") && nvram_match("wla_wlanstate", "Enable")&& nvram_match("wlg_wlanstate", "Enable"))
				start_bsd();
            
            #if defined(INCLULDE_2ND_5G_RADIO)
			if(nvram_match("wl_5g_bandsteering", "1") && nvram_match("wlh_wlanstate", "Enable")&& nvram_match("wlg_wlanstate", "Enable"))
				start_bsd();
		    #endif
			/*Foxconn add end by Antony 06/16/2013*/

            /* Start wsc if it is in 'unconfiged' state */
            if (nvram_match("wl0_wps_config_state", "0") && !nvram_match("wsc_pin_disable", "1"))
            {
                /* if "unconfig" to "config" mode, force it to built-in registrar and proxy mode */
                nvram_set("wl_wps_reg", "enabled");
                nvram_set("wl0_wps_reg", "enabled");
                nvram_set("wps_proc_status", "0");
                nvram_set("wps_method", "1");
                //nvram_set("wps_config_command", "1");
            }
			/* foxconn modified start, zacker, 05/20/2010, @spec_1.9 */
			//state = IDLE;
			state = next_signal();
			/* foxconn modified end, zacker, 05/20/2010, @spec_1.9 */
		    break;
            /* Foxconn added end pling 06/14/2007 */
        /* Foxconn added start by EricHuang, 01/09/2008 */
		case PPPSTART:
		{
            //char *pptp_argv[] = { "pppd", NULL };
            char *pptp_argv[] = { "pppd", "file", "/tmp/ppp/options", NULL };

		    _eval(pptp_argv, NULL, 0, NULL);
		    
		    /* foxconn modified start, zacker, 05/20/2010, @spec_1.9 */
		    //state = IDLE;
		    state = next_signal();
		    /* foxconn modified end, zacker, 05/20/2010, @spec_1.9 */
		    break;
		}
		/* Foxconn added end by EricHuang, 01/09/2008 */
		    
		default:
			dprintf("UNKNOWN\n");
			return;
		}
	}
}

int
main(int argc, char **argv)
{
#ifdef BCA_HNDROUTER
	char *init_alias = "init";
#elif defined(LINUX26)
	char *init_alias = "preinit";
#else
	char *init_alias = "init";
#endif // endif
	char *base = strrchr(argv[0], '/');

	base = base ? base + 1 : argv[0];

	/* init */
	if (strstr(base, init_alias)) 	
	{
		mount("devfs", "/dev", "tmpfs", MS_MGC_VAL, NULL);
		/* Michael added */
//        mknod("/dev/nvram", S_IRWXU|S_IFCHR, makedev(252, 0));
/*        mknod("/dev/mtdblock16", S_IRWXU|S_IFBLK, makedev(31, 16));
        mknod("/dev/mtdblock17", S_IRWXU|S_IFBLK, makedev(31, 17));
        mknod("/dev/mtd16", S_IRWXU|S_IFCHR, makedev(90, 32));
        mknod("/dev/mtd16ro", S_IRWXU|S_IFCHR, makedev(90, 33));
        mknod("/dev/mtd17", S_IRWXU|S_IFCHR, makedev(90, 34));
        mknod("/dev/mtd17ro", S_IRWXU|S_IFCHR, makedev(90, 35));*/
		/* Michael ended */
		mknod("/dev/console", S_IRWXU|S_IFCHR, makedev(5, 1));
		mknod("/dev/aglog", S_IRWXU|S_IFCHR, makedev(AGLOG_MAJOR_NUM, 0));
		mknod("/dev/wps_led", S_IRWXU|S_IFCHR, makedev(WPS_LED_MAJOR_NUM, 0));
#ifdef __CONFIG_UTELNETD__
		mkdir("/dev/pts", 0777);	
		mknod("/dev/pts/ptmx", S_IRWXU|S_IFCHR, makedev(5, 2));
		mknod("/dev/pts/0", S_IRWXU|S_IFCHR, makedev(136, 0));
		mknod("/dev/pts/1", S_IRWXU|S_IFCHR, makedev(136, 1));
#endif	/* __CONFIG_UTELNETD__ */
		/* Foxconn added start pling 12/26/2011, for WNDR4000AC */
#if (defined GPIO_EXT_CTRL)
		mknod("/dev/ext_led", S_IRWXU|S_IFCHR, makedev(EXT_LED_MAJOR_NUM, 0));
#endif
		/* Foxconn added end pling 12/26/2011 */

		main_loop();
		return 0;
	}

	/* Set TZ for all rc programs */
	setenv("TZ", nvram_safe_get("time_zone"), 1);

	/* rc [stop|start|restart ] */
	if (strstr(base, "rc")) {
		if (argv[1]) {
#ifdef __CONFIG_STBAP__
			return proc_stb(argv[1]);
#else
			if (strncmp(argv[1], "start", 5) == 0)
				return kill(1, SIGUSR2);
			else if (strncmp(argv[1], "stop", 4) == 0)
				return kill(1, SIGINT);
			else if (strncmp(argv[1], "restart", 7) == 0)
				return kill(1, SIGHUP);
#endif /* __CONFIG_STBAP__ */
		    /* Foxconn added start by EricHuang, 11/24/2006 */
		    else if (strcmp(argv[1], "wlanrestart") == 0) {
		        return kill(1, SIGQUIT);
		    }
		    /* Foxconn added end by EricHuang, 11/24/2006 */
		} else {
			fprintf(stderr, "usage: rc [start|stop|restart|wlanrestart]\n");
			return EINVAL;
		}
	}

#ifdef __CONFIG_NAT__
	/* ppp */
	else if (strstr(base, "ip-up"))
		return ipup_main(argc, argv);
	else if (strstr(base, "ip-down"))
		return ipdown_main(argc, argv);

	/* udhcpc [ deconfig bound renew ] */
	else if (strstr(base, "udhcpc"))
		return udhcpc_wan(argc, argv);
#endif	/* __CONFIG_NAT__ */

#if 0 /* foxconn wklin removed, 05/14/2009 */
	/* ldhclnt [ deconfig bound renew ] */
	else if (strstr(base, "ldhclnt"))
		return udhcpc_lan(argc, argv);

	/* stats [ url ] */
	else if (strstr(base, "stats"))
		return http_stats(argv[1] ? : nvram_safe_get("stats_server"));
#endif

	/* erase [device] */
	else if (strstr(base, "erase")) {
#ifdef BCA_HNDROUTER
	if (argv[1] && (!strcmp(argv[1], "nvram"))) {
			return nvram_erase();
#else /* !BCA_HNDROUTER */
		/* foxconn modified, zacker, 07/09/2010 */
		/*
	if (argv[1] && ((!strcmp(argv[1], "boot")) ||
		 (!strcmp(argv[1], "linux")) ||
		 (!strcmp(argv[1], "linux2")) ||
		 (!strcmp(argv[1], "rootfs")) ||
		 (!strcmp(argv[1], "rootfs2")) ||
		 (!strcmp(argv[1], "brcmnand")) ||
		 (!strcmp(argv[1], "confmtd")) ||
		 (!strcmp(argv[1], WLENT_MTDOOPS_PARTITION_NAME)) ||
		 (!strcmp(argv[1], "nvram")))) {
		*/
		if (argv[1]) {
			return mtd_erase(argv[1]);
#endif /* BCA_HNDROUTER */
	} else {
			fprintf(stderr, "usage: erase [device]\n");
			return EINVAL;
		}
	}

	/* write [path] [device] */
	else if (strstr(base, "write")) {
#ifdef BCA_HNDROUTER
		if (argc >= 4)
			return foxconn_sys_upgrade(argv[1], atoi(argv[2]), atoi(argv[3]));
			//return bca_sys_upgrade(argv[1]);
		else {
			cprintf("%s@%d *** Error argc=%d\n", __FUNCTION__, __LINE__, argc);
			return EINVAL;
		}
#else /* !BCA_HNDROUTER */
		if (argc >= 3)
			return mtd_write(argv[1], argv[2]);
		else {
			fprintf(stderr, "usage: write [path] [device]\n");
			return EINVAL;
		}
#endif /* BCA_HNDROUTER */
	}

	/* hotplug [event] */
	else if (strstr(base, "hotplug")) {
		if (argc >= 2) {
			if (!strcmp(argv[1], "net"))
				return hotplug_net();
		/*foxconn modified start, water, @usb porting, 11/11/2008*/
/*#ifdef __CONFIG_WCN__
			else if (!strcmp(argv[1], "usb"))
				return hotplug_usb();
#endif*/
        /*for mount usb disks, 4m board does not need these codes.*/
#if (defined SAMBA_ENABLE || defined HSDPA) /* Foxconn add, FredPeng, 03/16/2009 @HSDPA */
			/* else if (!strcmp(argv[1], "usb"))
				return usb_hotplug(); */
				/*return hotplug_usb();*/
			else if (!strcmp(argv[1], "block"))
                return hotplug_block(); /* wklin modified, 02/09/2011 */
#endif
#if defined(LINUX_2_6_36)
			else if (!strcmp(argv[1], "platform"))
				return coma_uevent();
#endif /* LINUX_2_6_36 */
            /* Foxconn added start pling 10/05/2012 */
            /* For USB LED after Kcode printer detection */
#if (defined INCLUDE_USB_LED)
            else
            {
                char *driver = getenv("PHYSDEVDRIVER");

                //printf("hotplug else case driver=%s \n",driver);
                if (driver && strstr(driver, "NetUSB"))
                {
                    hotplug_NetUSB();
                }
            }
#endif
            /* Foxconn added end pling 10/05/2012 */

		} else {
			fprintf(stderr, "usage: hotplug [event]\n");
			return EINVAL;
		}
	}

	return EINVAL;
}
