/*
 * Shell-like utility functions
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
 * $Id: wlif_utils.h 766767 2018-08-14 02:20:48Z $
 */

#ifndef _wlif_utils_h_
#define _wlif_utils_h_

#include "bcmwifi_channels.h"
#include "ethernet.h"

#ifndef IFNAMSIZ
#define IFNAMSIZ 16
#endif // endif

#ifndef ETHER_ADDR_LEN
#define ETHER_ADDR_LEN 6
#endif // endif

#define WLIFU_MAX_NO_BRIDGE		2
#define WLIFU_MAX_NO_WAN		2

#define MAX_USER_KEY_LEN	80			/* same as NAS_WKSP_MAX_USER_KEY_LEN */
#define MAX_SSID_LEN		32			/* same as DOT11_MAX_SSID_LEN */

#ifdef __CONFIG_RSDB__
/* Please keep the below enum in sync with wlc_rsdb_modes in wlc_rsdb.h */
enum wlif_rsdb_modes {
	WLIF_RSDB_MODE_AUTO = AUTO,
	WLIF_RSDB_MODE_2X2,
	WLIF_RSDB_MODE_RSDB,
	WLIF_RSDB_MODE_80P80,
	WLIF_RSDB_MODE_MAX
};
#endif /* __CONFIG_RSDB__ */

typedef struct wsec_info_s {
	int unit;					/* interface unit */
	int ibss;					/* IBSS vs. Infrastructure mode */
	int gtk_rekey_secs;		/* group key rekey interval */
	int wep_index;			/* wep key index */
	int ssn_to;				/* ssn timeout value */
	int debug;				/* verbose - 0:no | others:yes */
	int preauth;				/* preauth */
	int auth_blockout_time;	/* update auth blockout retry interval */
	unsigned int auth;	/* shared key authentication optional (0) or required (1) */
	unsigned int akm;			/* authentication mode */
	unsigned int wsec;			/* encryption */
	unsigned int flags;			/* flags */
	char osifname[IFNAMSIZ];	/* interface name */
	unsigned char ea[ETHER_ADDR_LEN];			/* interface hw address */
	unsigned char remote[ETHER_ADDR_LEN];	/* wds remote address */
	unsigned short radius_port;				/* radius server port number */
	char ssid[MAX_SSID_LEN + 1];				/* ssid info */
	char psk[MAX_USER_KEY_LEN + 1];			/* user-supplied psk passphrase */
	char *secret;				/* user-supplied radius secret */
	char *wep_key;			/* user-supplied wep key */
	char *radius_addr;		/* radius server address */
	char *nas_id;			/* nas mac address */
} wsec_info_t;

/* Struct for common bss-trans action frame data. */
typedef struct wl_wlif_bss_trans_data {
	uint8 rclass;			/* Rclass */
	chanspec_t chanspec;		/* Channel */
	struct ether_addr bssid;	/* Target bssid. */
	struct ether_addr addr;		/* Sta addr. */
	int timeout;			/* Timeout to clear mac from maclist. */
} wl_wlif_bss_trans_data_t;

#define WLIFU_WSEC_SUPPL			0x00000001	/* role is supplicant */
#define WLIFU_WSEC_AUTH			0x00000002	/* role is authenticator */
#define WLIFU_WSEC_WDS			0x00000004	/* WDS mode */

#define WLIFU_AUTH_RADIUS			0x20	/* same as nas_mode_t RADIUS in nas.h */

/* get wsec return code */
#define WLIFU_WSEC_SUCCESS			0
#define WLIFU_ERR_INVALID_PARAMETER	1
#define WLIFU_ERR_NOT_WL_INTERFACE	2
#define WLIFU_ERR_NOT_SUPPORT_MODE	4
#define WLIFU_ERR_WL_REMOTE_HWADDR	3
#define WLIFU_ERR_WL_WPA_ROLE		5

/* BSS transition return code */
#define WLIFU_BSS_TRANS_RESP_ACCEPT	0
#define WLIFU_BSS_TRANS_RESP_REJECT	1
#define WLIFU_BSS_TRANS_RESP_UNKNOWN	2

/* NVRAM names */
#define WLIFU_NVRAM_BSS_TRANS_NO_DEAUTH	"bss_trans_no_deauth"

extern int get_wlname_by_mac(unsigned char *mac, char *wlname);
extern char *get_ifname_by_wlmac(unsigned char *mac, char *name);
extern int get_wsec(wsec_info_t *info, unsigned char *mac, char *osifname);
extern bool wl_wlif_is_psta(char *ifname);
extern bool wl_wlif_is_dwds(char *ifname);
extern int wl_wlif_get_chip_cap(char *ifname, char *cap);
#ifdef __CONFIG_RSDB__
extern int wl_wlif_get_rsdb_mode();
#endif /* __CONFIG_RSDB__ */
extern bool wl_wlif_is_wet_ap(char *ifname);

/* wlif lib handle. */
typedef void wl_wlif_hdl;

/* Callback handler for ioctl calls. */
typedef int (*callback_hndlr)(char *ifname, int cmd, void *buf, int len, void *data);

/*
 * Init routine for wlif handle
 * Which allocates and initializes memory for wlif handle.
 * Params:
 * @uschd_hdl: Uschd handle.
 * @ifname: Interface name.
 * @ioctl_hndlr: Module specific ioctl handler routine.
 * @data: The data will be passed in ioctl_hndler callback function.
 */
extern wl_wlif_hdl* wl_wlif_init(void *uschd_hdl, char *ifname,
	callback_hndlr ioctl_hndlr, void *data);

/*
 * Deinit routine to free memory for wlif handle.
 * Params:
 * @hdl: Wlif handle.
 */
extern void wl_wlif_deinit(wl_wlif_hdl *hdl);

/*
 * BSS-Trans routine for sending act frame and receiving bss-trans response
 * if event_fd is invalid lib will not get the bss-trans response.
 * Params:
 * @hdl: wlif lib handle.
 * @data: BSS-Trans action frame data.
 * @event_fd: Socket descriptor to receive bss-trans resp.
 */
extern int wl_wlif_do_bss_trans(wl_wlif_hdl *hdl, wl_wlif_bss_trans_data_t *data,
	int event_fd);

/*
 * Routine for setting macmode and maclist for valid timeout
 * a timer will be registered to unblock the sta.
 * Params:
 * @hdl: Wlif lib handle.
 * @addr: Sta mac address.
 * @timeout: Timeout interval.
 */
extern int wl_wlif_block_mac(wl_wlif_hdl *hdl, struct ether_addr addr,
	int timeout);

/*
 * Routine for removing sta entry from maclist.
 * Params:
 * @hdl: wlif lib handle.
 * @addr: Sta mac address.
 * @flag: Bitflag for sta count and macmode.
 */
extern int wl_wlif_unblock_mac(wl_wlif_hdl *hdl, struct ether_addr addr, int flag);

extern int get_bridge_by_ifname(char *ifname, char **brname);
extern int wl_wlif_wds_ap_ifname(char *ifname, char *apname);
#endif /* _wlif_utils_h_ */
