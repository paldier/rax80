/*
 * Broadcom Proprietary and Confidential. Copyright (C) 2016,
 * All Rights Reserved.
 * 
 * This is UNPUBLISHED PROPRIETARY SOURCE CODE of Broadcom;
 * the contents of this file may not be disclosed to third parties, copied
 * or duplicated in any form, in whole or in part, without the prior
 * written permission of Broadcom.
 *
 * $Id: $
 */
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <dirent.h>

#include <bcmnvram.h>
#include <shutils.h>
#include <confmtd_utils.h>

/* Foxconn added start, Sinclair, 1/13/16 */
#ifdef FOXCONN_ADDED
#include <stdbool.h>
//#include <sys/socket.h> // AF_INET
#include <sys/ioctl.h>    // SIOCGIFFLAGS
#include <errno.h>        // errno
#include <netinet/in.h>   // IPPROTO_IP
#include <net/if.h>       // IFF_*, ifreq
#endif  /* FOXCONN_ADDED */
/* Foxconn added end, Sinclair, 1/13/16 */

/* path */
/* Foxconn added start, Sinclair, 10/22/15 */
#ifdef FOXCONN_ADDED
#define RAMFS_CONFMTD_DIR       "/tmp/media/nand"
#define IQOS_CONFMTD_DIR        RAMFS_CONFMTD_DIR
#else  /* FOXCONN_ADDED */
#define IQOS_CONFMTD_DIR        RAMFS_CONFMTD_DIR"/iqos"
#endif  /* FOXCONN_ADDED */
/* Foxconn added end, Sinclair, 10/22/15 */
#define TREND_DIR		"/tmp/trend"
#define SRC_FILE_DIR		"/usr/sbin"

/* file */
#define QOS_CONF		"qos.conf"
#define SETUP_SH		"setup.sh"
#define QOS_SH			"qos.sh"
#if defined(BCA_HNDROUTER)
#define IQOS_SETUP_SH	"iqos-setup.sh"
#endif

#define PATH_QOS_CONF			TREND_DIR"/"QOS_CONF
#define PATH_QOS_CONF_TMP		TREND_DIR"/"QOS_CONF".tmp"
#define PATH_SETUP_SH			TREND_DIR"/"SETUP_SH
#if defined(BCA_HNDROUTER)
#define PATH_QOS_SH			TREND_DIR"/"IQOS_SETUP_SH
#else /* BCA_HNDROUTER */
#define PATH_QOS_SH			TREND_DIR"/"QOS_SH
#endif /* !BCA_HNDROUTER */
#define PATH_CONFMTD_QOS_CONF		IQOS_CONFMTD_DIR"/"QOS_CONF
#define PATH_SRC_FILE_QOS_CONF		SRC_FILE_DIR"/"QOS_CONF

/* Backup iQos configuration file
 *
 * @return      0 if success, -1 if fail.
 */
static int
iqos_conf_backup(void)
{
	DIR *dir;

	if (!(dir = opendir(RAMFS_CONFMTD_DIR)))
		mkdir(RAMFS_CONFMTD_DIR, 0777);
	else
		closedir(dir);

	if (!(dir = opendir(IQOS_CONFMTD_DIR)))
		mkdir(IQOS_CONFMTD_DIR, 0777);
	else
		closedir(dir);

	eval("cp", PATH_QOS_CONF, IQOS_CONFMTD_DIR);

        return 0;
//	return confmtd_backup();
}

/* Restore iQos configuration file
 *
 * @return      0 if success, -1 if fail.
 */
static int
iqos_conf_restore(void)
{
	char *conf_file = PATH_CONFMTD_QOS_CONF;
	struct stat tmp_stat;

	if (stat(conf_file, &tmp_stat) == 0) {
		eval("cp", conf_file, PATH_QOS_CONF);
		return 0;
	}


	return -1;
}

/* Place a file lock */
static int
file_lock(FILE *fp)
{
	int i;

	for (i = 0; i < 10; i++) {
		if (flock(fileno(fp), LOCK_EX | LOCK_NB))
			usleep(100000);
		else
			return 0;
	}

	fprintf(stderr, "file_lock: flock error\n");
	return -1;
}

/* Foxconn added start, Sinclair, 1/13/16 */
#ifdef FOXCONN_ADDED
/* Check the network interface is UP or not
 *
 * @return      true if interface is up, false if otherwise
 */
bool is_itf_is_up_or_not(char *ifname) {
    bool ret_val = false;
    int socId = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
    if (socId < 0) {
        printf("%s(%d): Socket failed (%d)\n",  __func__, __LINE__, errno);
        goto __out;
    }

    struct ifreq if_req;
    (void) strncpy(if_req.ifr_name, ifname, sizeof(if_req.ifr_name));
    int rv = ioctl(socId, SIOCGIFFLAGS, &if_req);
    close(socId);

    if ( rv == -1) {
        printf("%s(%d): ioctl failed (%d)\n",  __func__, __LINE__, errno);
        goto __out;
    }

    if (if_req.ifr_flags & IFF_UP) {
        ret_val = true;
    }

__out:
    return ret_val;
}
#endif  /* FOXCONN_ADDED */
/* Foxconn added end, Sinclair, 1/13/16 */

int
main(int argc, char **argv)
{
	struct stat file_stat;
	FILE *fp1, *fp2;
	char line[256], download[32], upload[32];
	int retry = 0, download_len, upload_len;
	char *wan_ifname;
	char *lan_ifname;
	char *wan_proto;
	const char *const_down_str = "ceil_down=";
	const char *const_up_str = "ceil_up=";

	/* Filter out incorrect argument */
	if ((argc != 2) || ((strncmp(argv[1], "stop", strlen("stop")) != 0) &&
		(strncmp(argv[1], "start", strlen("start")) != 0) &&
#ifdef FOXCONN_ADDED
		(strncmp(argv[1], "pause", strlen("pause")) != 0) &&
#endif  /* FOXCONN_ADDED */
		(strncmp(argv[1], "restart", strlen("restart")) != 0))) {
		return -1;
	}

	if (daemon(1, 1) == -1) {
		fprintf(stderr, "error from daemonize.\n");
		return -1;
	}

	/* Retrieve WAN interface */
/* Foxconn added start, Sinclair, 10/22/15@ for IPv4 WAN */
#ifdef FOXCONN_ADDED
	if (strcmp(argv[1], "pause") == 0) {
		/* Stop iQos traffic control */
		eval(PATH_QOS_SH, "stop", wan_ifname, lan_ifname);
		return 0;
	}

    char tmp_ifname[32] = {0};
    wan_proto = nvram_safe_get("wan_proto");

#   ifdef MOBILE_PRODUCT
    if (nvram_match("wwan_wan_type", "0")) {
        strncat(tmp_ifname, nvram_safe_get("wan1_ifname"), sizeof(tmp_ifname));
    }
    else if (nvram_match("wwan_wan_type", "2")) {
        /* Failover mode */
        strncat(tmp_ifname, nvram_safe_get("current_ifname"), sizeof(tmp_ifname));
    }
    else
#   endif  /* MOBILE_PRODUCT */
    /*foxconn Han edited, 05/13/2016 pppoe should use eth0 as wan_dev*/
    if (strcmp(wan_proto, "pppoe")==0 || strcmp(wan_proto, "pptp")==0 || strcmp(wan_proto, "l2tp")==0) 
    {
        strncat(tmp_ifname, nvram_safe_get("pppoe_ifname"), sizeof(tmp_ifname));
    } else {
        strncat(tmp_ifname, nvram_safe_get("wan_ifname"), sizeof(tmp_ifname));
    }

    /*foxconn Han edited start, 05/09/2016 
     * traffic shaping not work when iptv/vlan enabled
     * due to CTF support for iptv/vlan, traffic will pass between eth0 and br0 won't go through vlan10*/
    if(strncmp(tmp_ifname,"eth0", 4)) /*only check when wan_ifname != eth0*/
    {
        if (nvram_match ("enable_vlan", "enable") || nvram_match("iptv_enabled","1"))
        {
            printf("\nbcmiqosd: when vlan or iptv enabled force to use eth0 as wan ifname!\n\n");
            sprintf(tmp_ifname,"eth0");
        }
    }
    /*foxconn Han edited end, 05/09/2016*/


#   ifdef MOBILE_PRODUCT
    if (is_itf_is_up_or_not(tmp_ifname)==false && strcmp(argv[1], "stop")!=0) {
        snprintf(tmp_ifname, sizeof(tmp_ifname), "%s", nvram_safe_get("wan_ifname"));
    }
#   endif  /* MOBILE_PRODUCT */

#if 0
    if (nvram_match("ipv6_proto","pppoe")) {
        strncat(tmp_ifname, ",ppp1", sizeof(tmp_ifname));
    } else if (nvram_match("ipv6_proto","6to4")) {
        strncat(tmp_ifname, ",sit0", sizeof(tmp_ifname));
    } else if (nvram_match("ipv6_proto","auto")) {
        strncat(tmp_ifname, ",sit0,ppp1", sizeof(tmp_ifname));
    }
#endif

    wan_ifname = &tmp_ifname;
    printf("%s(%d) wan_ifname=%s\n", __func__, __LINE__, wan_ifname);
#else   /* FOXCONN_ADDED */
	wan_proto = nvram_safe_get("wan0_proto");

	if (strcmp(wan_proto, "pppoe") == 0) {
		wan_ifname = nvram_safe_get("wan0_pppoe_ifname");
	} else {
		wan_ifname = nvram_safe_get("wan0_ifname");
	}
#endif  /* FOXCONN_ADDED */
/* Foxconn added end, Sinclair, 10/22/15@ for IPv6 WAN */


	/* Retrieve LAN interface */
	lan_ifname = nvram_safe_get("lan_ifname");

	if (strcmp(argv[1], "stop") == 0) {
		/* Stop iQos */
#if !defined(BCA_HNDROUTER)
		eval(PATH_SETUP_SH, "stop", wan_ifname, lan_ifname);
		eval(PATH_QOS_SH, "stop");
		eval("rm", PATH_QOS_CONF);
#else
		eval(PATH_QOS_SH, "stop");
#endif

		return 0;
	}

#if !defined(BCA_HNDROUTER) /* RAMFS_CONFMTD not support in 4908 */
	if (nvram_match("broadstream_iqos_default_conf", "1")) {

        /*foxconn Han edited start, 05/11/2016 prevent keep using default configuration*/
        nvram_set("broadstream_iqos_default_conf","0");
        nvram_commit();
        /*foxconn Han edited end, 05/11/2016*/

//		eval("cp", PATH_SRC_FILE_QOS_CONF, PATH_QOS_CONF);
		iqos_conf_backup();
	} else {
		if (iqos_conf_restore()) {
//			eval("cp", PATH_SRC_FILE_QOS_CONF, PATH_QOS_CONF);
			iqos_conf_backup();
		}
	}
#else
//	eval("cp", PATH_SRC_FILE_QOS_CONF, PATH_QOS_CONF);
#endif


	/* Execute WAN port bandwidth detection */
	if (nvram_match("broadstream_iqos_wan_bw_auto", "1")) {
		/* In case router is not ready to connect to internet
		 * and test whether speedtest server is reachable.
		 */
		do {
			sleep(1);
			eval("curl", "-s", "-o", "/tmp/speedtest-test",
				"http://www.speedtest.net/speedtest-config.php");
			if (stat("/tmp/speedtest-test", &file_stat)) {
				if (retry > 3) {
					fprintf(stderr, "speedtest server is not reachable\n");
					return -1;
				} else {
					retry++;
					continue;
				}
			}
			eval("rm", "/tmp/speedtest-test");
			break;
		} while (1);

		/* Execute speedtest_cli to test download and upload bandwidth */
		eval("speedtest_cli", "1", "3", "1", "2");

		if (stat("/tmp/speedtest_download_result", &file_stat)) {
			fprintf(stderr, "stat file /tmp/speedtest_download_result error\n");
			return -1;
		}
		if (stat("/tmp/speedtest_upload_result", &file_stat)) {
			fprintf(stderr, "stat file /tmp/speedtest_upload_result error\n");
			return -1;
		}

		/* Retrieve download bandwidth obtained by speedtest_cli */
		if (!(fp1 = fopen("/tmp/speedtest_download_result", "r"))) {
			perror("fopen /tmp/speedtest_download_result");
			return errno;
		}

		fgets(line, 255, fp1);
		if (strncmp(line, "ceil_down=", strlen("ceil_down=")) != 0) {
			fprintf(stderr, "incorrect content: /tmp/speedtest_download_result\n");
			fclose(fp1);
			eval("rm", "/tmp/speedtest_download_result");
			return -1;
		}

		download_len = (int)(strstr(line, "kbps") - line - strlen("ceil_down="));
		if (download_len >= sizeof(download)) {
			fprintf(stderr, "incorrect content: /tmp/speedtest_download_result\n");
			fclose(fp1);
			eval("rm", "/tmp/speedtest_download_result");
			return -1;
		}
		strncpy(download, &line[strlen("ceil_down=")], download_len);
		download[download_len] = '\0';

		fclose(fp1);

		/* Retrieve dupload bandwidth obtained by speedtest_cli */
		if (!(fp1 = fopen("/tmp/speedtest_upload_result", "r"))) {
			perror("fopen /tmp/speedtest_upload_result");
			return errno;
		}

		fgets(line, 255, fp1);
		if (strncmp(line, "ceil_up=", strlen("ceil_up=")) != 0) {
			fprintf(stderr, "incorrect content: /tmp/speedtest_upload_result\n");
			fclose(fp1);
			eval("rm", "/tmp/speedtest_upload_result");
			return -1;
		}

		upload_len = (int)(strstr(line, "kbps") - line - strlen("ceil_up="));
		if (upload_len >= sizeof(upload)) {
			fprintf(stderr, "incorrect content: /tmp/speedtest_upload_result\n");
			fclose(fp1);
			eval("rm", "/tmp/speedtest_upload_result");
			return -1;
		}
		strncpy(upload, &line[strlen("ceil_up=")], upload_len);
		upload[upload_len] = '\0';

		fclose(fp1);

		eval("cp", PATH_QOS_CONF, PATH_QOS_CONF_TMP);

		if (!(fp1 = fopen(PATH_QOS_CONF, "w"))) {
			perror("fopen "PATH_QOS_CONF);
			return errno;
		}

		if (!(fp2 = fopen(PATH_QOS_CONF_TMP, "r"))) {
			fclose(fp1);
			perror("fopen "PATH_QOS_CONF_TMP);
			return errno;
		}

		if (file_lock(fp1)) {
			fclose(fp1);
			fclose(fp2);

			eval("cp", PATH_QOS_CONF_TMP, PATH_QOS_CONF);
			eval("rm", PATH_QOS_CONF_TMP);
			return -1;
		}

		/* Update testing results of speedtest_cli to configuraton file of iQos */
		while (fgets(line, 255, fp2)) {
			if (strncmp(line, "ceil_down=", strlen("ceil_down=")) == 0) {
				fprintf(fp1, "%s%s%s\n", const_down_str, download, "kbps");
			} else if (strncmp(line, "ceil_up=", strlen("ceil_up=")) == 0) {
				fprintf(fp1, "%s%s%s\n", const_up_str, upload, "kbps");
			} else {
				fputs(line, fp1);
			}
		}

		flock(fileno(fp1), LOCK_UN | LOCK_NB);

		fclose(fp1);
		fclose(fp2);
		eval("rm", PATH_QOS_CONF_TMP);
#if !defined(BCA_HNDROUTER) /* RAMFS_CONFMTD not support in 4908 */		
		iqos_conf_backup();
#endif		
	}
    /* Foxconn added start, Sinclair, 10/22/15@ for NTGR's QoS setting */
#ifdef FOXCONN_ADDED
    else {
        unsigned long uplink_bw=0, downlink_bw=0;
        if (nvram_match("qos_manual","1")) {
            downlink_bw = strtoul(nvram_get("qos_bw_downlink_manual"), NULL, 10);
            uplink_bw = strtoul(nvram_get("qos_bw_uplink_manual"), NULL, 10);
        } else {
            downlink_bw = strtoul(nvram_get("qos_bw_downlink_ookla"), NULL, 10);
            uplink_bw = strtoul(nvram_get("qos_bw_uplink_ookla"), NULL, 10);
        }
        printf("%s(%d) dl=%lu, ul=%lu\n", 
                    __func__, __LINE__, downlink_bw/8000, uplink_bw/8000);

        /* The content below reuse from line 274~312 */
		eval("cp", PATH_QOS_CONF, PATH_QOS_CONF_TMP);

		if (!(fp1 = fopen(PATH_QOS_CONF, "w"))) {
			perror("fopen "PATH_QOS_CONF);
			return errno;
		}

		if (!(fp2 = fopen(PATH_QOS_CONF_TMP, "r"))) {
			fclose(fp1);
			perror("fopen "PATH_QOS_CONF_TMP);
			return errno;
		}

		if (file_lock(fp1)) {
			fclose(fp1);
			fclose(fp2);

			eval("cp", PATH_QOS_CONF_TMP, PATH_QOS_CONF);
			eval("rm", PATH_QOS_CONF_TMP);
			return -1;
		}

		/* Update testing results of speedtest_cli to configuraton file of iQos */
		while (fgets(line, 255, fp2)) {
			if (strncmp(line, "ceil_down=", strlen("ceil_down=")) == 0) {
				fprintf(fp1, "%s%lu%s\n", const_down_str, downlink_bw/8000, "kbps");
			} else if (strncmp(line, "ceil_up=", strlen("ceil_up=")) == 0) {
				fprintf(fp1, "%s%lu%s\n", const_up_str, uplink_bw/8000, "kbps");
			} else {
				fputs(line, fp1);
			}
		}

		flock(fileno(fp1), LOCK_UN | LOCK_NB);

		fclose(fp1);
		fclose(fp2);
		eval("rm", PATH_QOS_CONF_TMP);
		iqos_conf_backup();
        /* The content above reuse from line 274~312 */
    }
#endif  /* FOXCONN_ADDED */
    /* Foxconn added end, Sinclair, 10/22/15@ for NTGR's QoS setting */

	/* Start|restart iQos */
	if (strcmp(argv[1], "start") == 0) {
		iqos_conf_restore();
    if (nvram_match("broadstream_iqos_enable", "1"))    	
    {
		    eval(PATH_SETUP_SH, "restart", wan_ifname, lan_ifname);
		}
		else
		{
		    eval(PATH_SETUP_SH, "stop", wan_ifname, lan_ifname);
    }

#ifdef FOXCONN_ADDED
#if !defined(BCA_HNDROUTER)		
        if (nvram_match("broadstream_iqos_enable", "1")) {
 		    eval(PATH_QOS_SH, "start");
        }
        else {
		    eval(PATH_QOS_SH, "stop");
        }		
#endif
#else   /* FOXCONN_ADDED */
#if defined(BCA_HNDROUTER)
        if (nvram_match("broadstream_iqos_enable", "1")) {
   		      eval(PATH_QOS_SH, "start");
	      }
	      else
	      {
   		      eval(PATH_QOS_SH, "config");
	      }
#endif

#endif  /* FOXCONN_ADDED */
	} else if (strcmp(argv[1], "restart") == 0) {
#ifdef FOXCONN_ADDED
//        eval(PATH_QOS_SH, "stop");
		eval(PATH_SETUP_SH, "restart", wan_ifname, lan_ifname);
#endif  /* FOXCONN_ADDED */
//		eval(PATH_SETUP_SH, "restart", wan_ifname, lan_ifname);
#ifdef FOXCONN_ADDED

#if !defined(BCA_HNDROUTER)
        if (nvram_match("broadstream_iqos_enable", "1")) {
		    eval(PATH_QOS_SH, "start");
        }
        else {
		    eval(PATH_QOS_SH, "stop");
        }
#endif
//		eval(PATH_QOS_SH, "restart");
#else   /* FOXCONN_ADDED */
		eval(PATH_QOS_SH, "restart");
#endif  /* FOXCONN_ADDED */
	}
    printf("%s(%d) wan_ifname=%s\n", __func__, __LINE__, wan_ifname);

	return 0;
}
