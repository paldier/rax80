/***********************************************************************
 *
 * <:copyright-BRCM:2015:DUAL/GPL:standard
 * 
 *    Copyright (c) 2015 Broadcom 
 *    All Rights Reserved
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License, version 2, as published by
 * the Free Software Foundation (the "GPL").
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * 
 * A copy of the GPL is available at http://www.broadcom.com/licenses/GPLv2.php, or by
 * writing to the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 * 
 * :>
 *
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>	 /* needed to define getpid() */
#include <errno.h>
#include <signal.h>
#include <string.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <linux/if.h>
#include <bcmnet.h>
#include "cms_boardioctl.h"
#include <cms_mem.h>
#include <cms_msg.h>
#include <rdpa_types.h>
#include <bcm/bcmswapitypes.h>
#include <dirent.h>
#include <sched.h>
#include "cms_psp.h"
#include "wanconf.h"

int rdpaCtl_RdpaMwWanConf(void);
int rdpaCtl_time_sync_init(void);

#define WAN_CONF_ERROR       (-1)
#define WAN_CONF_SUCCESS      (0)

#define LAUNCH_EPON_APP_TRUE  (1)
#define LAUNCH_EPON_APP_FALSE (0)

#define EPONMAC_TRUE          (1)
#define EPONMAC_FALSE         (0)


#define GET_SCRATCHPAD_VALUE_AND_LENGTH(scratchpad_parameter, local_buffer, length_in_bytes) do     \
    {                                                                                               \
        length_in_bytes = cmsPsp_get(scratchpad_parameter, local_buffer, sizeof(local_buffer) - 1); \
        if (0 >= length_in_bytes)                                                                   \
        {                                                                                           \
            wc_log_err("unexpected data in %s, len=%d", scratchpad_parameter, length_in_bytes);     \
            return WAN_CONF_ERROR;                                                                  \
        }                                                                                           \
        local_buffer[length_in_bytes] = 0;                                                          \
    } while (0)


#if defined(BRCM_CMS_BUILD)
static CmsRet sendWanOpStateMsg(void *msgHandle, WanConfPhyType phyType, UBOOL8 opState);
#endif /* BRCM_CMS_BUILD */

int insmod_param(char *driver, char *param)
{
    char cmd[128];
    int ret;

    sprintf(cmd, "insmod %s %s", driver, param ? : "");
    ret = system(cmd);
    if (ret)
        wc_log_err("unable to load module: %s\n", cmd);

    return ret;
}

#if defined(BRCM_CMS_BUILD)
CmsRet smd_start_app(UINT32 wordData)
{
    void *msgBuf;
    CmsRet ret;
    CmsMsgHeader *msg;
    void *msgHandle;
    int pid;

    ret = cmsMsg_initWithFlags(EID_WANCONF, 0, &msgHandle);
    if (ret)
    {
        wc_log_err("message init failed ret=%d\n", ret);
        return ret;
    }

    msgBuf = cmsMem_alloc(sizeof(CmsMsgHeader), ALLOC_ZEROIZE);
    if (msgBuf == NULL)
    {
        wc_log_err("message allocation failed\n");
        return CMSRET_INTERNAL_ERROR;
    }

    msg = (CmsMsgHeader *)msgBuf;
    msg->src = EID_WANCONF;
    msg->dst = EID_SMD;
    msg->flags_event = FALSE;
    msg->type = CMS_MSG_START_APP;
    msg->wordData = wordData;
    msg->dataLength = 0;

    pid = (int)cmsMsg_sendAndGetReply(msgHandle, msg);
    if (pid == CMS_INVALID_PID)
    {
        wc_log_err("Failed to start app\n");
        return CMSRET_INTERNAL_ERROR;
    }

    if (wordData == EID_EPON_APP)
    {
        ret = sendWanOpStateMsg(msgHandle, WANCONF_PHY_TYPE_EPON, TRUE);
    }
    else if (wordData == EID_OMCID)
    {
        ret = sendWanOpStateMsg(msgHandle, WANCONF_PHY_TYPE_GPON, TRUE);
    }

    CMSMEM_FREE_BUF_AND_NULL_PTR(msgBuf);
    cmsMsg_cleanup(&msgHandle);

    return ret;
}
#endif /* BRCM_CMS_BUILD */

int load_gpon_modules(rdpa_wan_type wan_type)
{
    int rc = WAN_CONF_SUCCESS;

    if (rdpa_wan_gpon == wan_type)
    {
        rc = insmod("/lib/modules/"KERNELVER"/extra/gponstack.ko");
    }
    else if (rdpa_wan_xgpon == wan_type)
    {
        rc = insmod("/lib/modules/"KERNELVER"/extra/ngponstack.ko");
    }

    rc = insmod("/lib/modules/"KERNELVER"/extra/bcmgpon.ko");

    rc = rc ? : system("cat /dev/rgs_logger &");
#if defined(BRCM_CMS_BUILD)
    if (!rc && (smd_start_app(EID_OMCID)) != CMSRET_SUCCESS)
    {
        wc_log_err("Failed to start omcid app\n");
        return WAN_CONF_ERROR;
    }
#endif /* BRCM_CMS_BUILD */

    return rc;
}

static int getPidByName(const char *name)
{
    DIR *dir;
    FILE *fp;
    struct dirent *dent;
    UBOOL8 found=FALSE;
    long pid;
    int  rc, p, i;
    int rval = CMS_INVALID_PID;
    char filename[BUFLEN_256];
    char processName[BUFLEN_256];
    char *endptr;

    if (NULL == (dir = opendir("/proc")))
    {
        wc_log_err("could not open /proc\n");
        return rval;
    }

    while ( (!found) && (NULL != (dent = readdir(dir))) )
    {
        /*
         * Each process has its own directory under /proc, the name of the
         * directory is the pid number.
         */
        if (DT_DIR != dent->d_type)
            continue;

        pid = strtol(dent->d_name, &endptr, 10);
        if (ERANGE != errno && endptr != dent->d_name)
        {
            snprintf(filename, sizeof(filename), "/proc/%ld/stat", pid);
            if (NULL == (fp = fopen(filename, "r")))
            {
                wc_log_err("could not open %s\n", filename);
            }
            else
            {
                /* Get the process name, format: 913 (consoled) */
                memset(processName, 0, sizeof(processName));
                rc = fscanf(fp, "%d (%s", &p, processName);
                fclose(fp);

                if (rc >= 2)
                {
                    i = strlen(processName);
                    if (i > 0)
                    {
                        /* strip out the trailing ) character */
                        if (')' == processName[i-1])
                            processName[i-1] = 0;
                    }
                }

                if (!strncmp(processName, name,strlen(name)))
                {
                    rval = pid;
                    found = TRUE;
                }
            }
        }
    }

    closedir(dir);

    return rval;
}

int load_epon_modules(int shall_launch_epon_app)
{
#define KTHREAD_NAME "EponMPCP"
    int pid;
    int rc = insmod_param("/lib/modules/"KERNELVER"/extra/bcmepon.ko", "epon_usr_init=1");
    struct sched_param sp = { .sched_priority = 10 };

    if (LAUNCH_EPON_APP_FALSE == shall_launch_epon_app)
        return rc;

#if defined(BRCM_CMS_BUILD)
    if (!rc && (smd_start_app(EID_EPON_APP) != CMSRET_SUCCESS))
    {
        wc_log_err("Failed to start eponapp\n");
        return WAN_CONF_ERROR;
    }
#endif /* BRCM_CMS_BUILD */

    pid = getPidByName(KTHREAD_NAME);
    if (pid > 0)
    {
        if (WAN_CONF_ERROR == sched_setscheduler(pid, SCHED_RR, &sp))
        {
            wc_log_err("failed to set kthread %s with scheduler RR\n", KTHREAD_NAME);
            return WAN_CONF_ERROR;
        }
    }
    else
    {
        wc_log_err("unable to find pid for kthread %s\n", KTHREAD_NAME);
    }

    return rc;
}

int create_bcmenet_vport(char *ifname, int op, rdpa_if port)
{
    struct ifreq ifr;
    int err, skfd;
    struct ethctl_data ethctl;

    memset(&ethctl, 0x0, sizeof(struct ethctl_data));
    if ((skfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        printf("socket open error\n");
        return WAN_CONF_ERROR;
    }

    strcpy(ifr.ifr_name, "bcmsw");
    if ((err = ioctl(skfd, SIOCGIFINDEX, &ifr)) < 0 )
    {
        printf("bcmsw interface does not exist");
        goto exit;
    }

    ethctl.op = op;
    ethctl.val = port;
    ifr.ifr_data = (void *)&ethctl;
    err = ioctl(skfd, SIOCETHCTLOPS, &ifr);
    if (err < 0 )
        printf("Error %d bcmenet gbe wan port\n", err);

    strcpy(ifname, ethctl.ifname);

exit:
    close(skfd);
    return err;
}

int get_enet_wan_port(char *ifname, unsigned int len)
{
    struct ifreq ifr;
    int err, skfd;
    struct ethswctl_data ethswctl;

    memset(&ethswctl, 0x0, sizeof(struct ethswctl_data));
    if ((skfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        printf("socket open error\n");
        return WAN_CONF_ERROR;
    }

    strcpy(ifr.ifr_name, "bcmsw");
    if ((err = ioctl(skfd, SIOCGIFINDEX, &ifr)) < 0 )
    {
        printf("bcmsw interface does not exist");
        goto exit;
    }

    ethswctl.up_len.uptr = ifname;
    ethswctl.up_len.len = len;
    ifr.ifr_data = (void *)&ethswctl;
    err = ioctl(skfd, SIOCGWANPORT, &ifr);
    if (err < 0 )
        printf("Error %d bcmenet gbe wan port\n", err);

exit:
    close(skfd);
    return err;
}

#if defined(BRCM_CMS_BUILD)
void gbeSendPostMdmMsg(void *msgHandle)
{
    char buf[sizeof(CmsMsgHeader) + sizeof(MdmPostActNodeInfo)] = {0};
    CmsRet cmsReturn;
    CmsMsgHeader *msgHdr = (CmsMsgHeader *)buf;
    MdmPostActNodeInfo *msgBody = (MdmPostActNodeInfo *)(msgHdr + 1);

    msgHdr->dst = EID_SSK;
    msgHdr->src = EID_WANCONF;
    msgHdr->type = CMS_MSG_MDM_POST_ACTIVATING;
    msgHdr->flags_event = 1;
    msgHdr->dataLength = sizeof(MdmPostActNodeInfo);
    msgBody->subType = MDM_POST_ACT_TYPE_FILTER;

    /* Attempt to send CMS response message & test result. */
    cmsReturn = cmsMsg_send(msgHandle, msgHdr);
    if (CMSRET_SUCCESS != cmsReturn)
    {
        wc_log_err("Send message failure, cmsResult: %d", cmsReturn);
    }
    else
    {
        wc_log_err("Sent Wanconf App Indication to SSK");
    }
}

static int sendEnablePort(char *if_name)
{
    char buf[sizeof(CmsMsgHeader) + IFNAMESIZ]={0};
    CmsMsgHeader *msg=(CmsMsgHeader *) buf;
    char *msg_ifname = (char *)(msg+1);
    CmsRet ret;
    void *msgHandle;

    if (strlen(if_name) > IFNAMESIZ -1)
        return WAN_CONF_ERROR;

    ret = cmsMsg_initWithFlags(EID_WANCONF, 0, &msgHandle);
    if (ret)
        return ret;

    msg->type = CMS_MSG_WAN_PORT_ENABLE;
    msg->src = EID_WANCONF;
    msg->dst = EID_SSK;
    msg->flags_event = 1;
    msg->dataLength = IFNAMESIZ;

    strcpy(msg_ifname, if_name);

    if (CMSRET_SUCCESS != (ret = cmsMsg_send(msgHandle, msg)))
    {
        wc_log_err("could not send out CMS_MSG_WAN_PORT_ENABLE, ret=%d", ret);
        return WAN_CONF_ERROR;
    }
    gbeSendPostMdmMsg(msgHandle);

    cmsMsg_cleanup(&msgHandle);

    return WAN_CONF_SUCCESS;
}

static CmsRet sendWanOpStateMsg(void *msgHandle, WanConfPhyType phyType, UBOOL8 opState)
{
    CmsRet ret = CMSRET_SUCCESS;
    char buf[sizeof(CmsMsgHeader) + sizeof(WanConfPhyOpStateMsgBody)] = {0};
    CmsMsgHeader *msg = (CmsMsgHeader*)buf;
    WanConfPhyOpStateMsgBody *info;

    msg->type = CMS_MSG_WAN_PORT_SET_OPSTATE;
    msg->src = EID_WANCONF;
    msg->dst = EID_SSK;
    msg->flags_request = 1;
    msg->flags_response = 0;
    msg->flags_event = 0;
    msg->dataLength = sizeof(WanConfPhyOpStateMsgBody);
    msg->wordData = 0;

    info = (WanConfPhyOpStateMsgBody*)&(buf[sizeof(CmsMsgHeader)]);
    info->phyType = phyType;
    info->opState = opState;

    /* Wait for reply to serialize the processing. */
    ret = cmsMsg_sendAndGetReply(msgHandle, msg);
    if (CMSRET_SUCCESS != ret)
    {
        wc_log_err("cmsMsg_sendAndGetReply(CMS_MSG_WAN_PORT_SET_OPSTATE) failed, ret=%d", ret);
    }

    return ret;
}
#endif

static int get_is_epon_mac(int *is_epon_mac)
{
    char buf[16];
    int count;

    GET_SCRATCHPAD_VALUE_AND_LENGTH(RDPA_WAN_OEMAC_PSP_KEY, buf, count);
    
    if (!strncasecmp(buf, RDPA_WAN_OEMAC_VALUE_EPONMAC, strlen(RDPA_WAN_OEMAC_VALUE_EPONMAC)))
        *is_epon_mac = EPONMAC_TRUE;
    else
        *is_epon_mac = EPONMAC_FALSE;

    return WAN_CONF_SUCCESS;
}

static int get_shall_launch_epon_app(int *shall_launch_epon_app)
{
    char buf[16];
    int count;

    *shall_launch_epon_app = LAUNCH_EPON_APP_TRUE;

    GET_SCRATCHPAD_VALUE_AND_LENGTH(RDPA_WAN_TYPE_PSP_KEY, buf, count);
    
    if (!strncasecmp(buf, RDPA_WAN_TYPE_VALUE_GBE, strlen(RDPA_WAN_TYPE_VALUE_GBE)))
    {
        int is_epon_mac, rc;

        rc = get_is_epon_mac(&is_epon_mac);
        if (WAN_CONF_ERROR == rc)
            return WAN_CONF_ERROR;

        if (EPONMAC_TRUE == is_epon_mac)
            *shall_launch_epon_app = LAUNCH_EPON_APP_FALSE;
    }

    return WAN_CONF_SUCCESS;
}

int main(int argc __attribute__((unused)), char *argv[] __attribute__((unused)))
{
    pid_t childPid = fork();
    char buf[16];
    int rc, shall_launch_epon_app;
    rdpa_wan_type wan_type = rdpa_wan_none;
#ifndef CONFIG_BCM963158
    int count;
#endif
    /* BP_BOARD_ID_LEN(16) defined in boardparms.h */
    char boardid[16];

    if (childPid < 0) /* Failed to fork */
        return WAN_CONF_ERROR;

    if (childPid != 0) /* Father always exists */
        return WAN_CONF_SUCCESS;

#ifdef CONFIG_BCM963158
    wan_type = rdpa_wan_gpon;
#else
    GET_SCRATCHPAD_VALUE_AND_LENGTH(RDPA_WAN_TYPE_PSP_KEY, buf, count);

    wc_log("cmsPsp_get: rdpaWanType=%s\n", buf);

    if (!strcasecmp(buf, RDPA_WAN_TYPE_VALUE_GPON))
        wan_type = rdpa_wan_gpon;
    else if (!strcasecmp(buf, RDPA_WAN_TYPE_VALUE_EPON))
    {
        char wan_rate_buf[16];
#define RATE_STR_LEN 2
#define PSP_RATE_STR_LEN 4

        wan_type = rdpa_wan_epon;
        GET_SCRATCHPAD_VALUE_AND_LENGTH(RDPA_WAN_RATE_PSP_KEY, wan_rate_buf, count);
        if ((count >= PSP_RATE_STR_LEN) && (!strncasecmp(&wan_rate_buf[RATE_STR_LEN], RDPA_WAN_RATE_10G, RATE_STR_LEN)))
            wan_type = rdpa_wan_xepon;
    }
    else if (!strcasecmp(buf, RDPA_WAN_TYPE_VALUE_AE))
        wan_type = rdpa_wan_gbe;
    else if (!strcasecmp(buf, RDPA_WAN_TYPE_VALUE_GBE))
    {
        int is_epon_mac;
        rc = get_is_epon_mac(&is_epon_mac);
        if (WAN_CONF_ERROR == rc)
            return WAN_CONF_ERROR;

        if (EPONMAC_TRUE == is_epon_mac)
            wan_type = rdpa_wan_xepon;
        else
            wan_type = rdpa_wan_gbe;
    }
    else if (!strcasecmp(buf, RDPA_WAN_TYPE_VALUE_XGPON1) || !strcasecmp(buf, RDPA_WAN_TYPE_VALUE_NGPON2) || !strcasecmp(buf, RDPA_WAN_TYPE_VALUE_XGS))
        wan_type = rdpa_wan_xgpon;
    else if (!strcasecmp(buf, RDPA_WAN_TYPE_VALUE_AUTO))
    {
        rc = try_wan_type_detect_and_set(&wan_type);
        if (DETECTION_ERROR == rc)
            return WAN_CONF_ERROR;
    }
    else
    {
        wc_log_err("Unknown wan set in scratchpad %s = %s\n", RDPA_WAN_TYPE_PSP_KEY, buf);
        return WAN_CONF_ERROR;
    }
#endif
        
    /* Must be loaded even when not in PMD module because of symbol dep. */
    rc = insmod("/lib/modules/"KERNELVER"/extra/pmd.ko");
    if (rc)
    {
        wc_log_err("Error loading PMD module");
        return WAN_CONF_ERROR;
    }
    
    /* Loaded here because of dependency of bcm_i2c_pon_optics_type_get */
    rc = insmod("/lib/modules/"KERNELVER"/extra/laser_dev.ko");
    if (rc)
    {
        wc_log_err("Error loading Laser module");
        return WAN_CONF_ERROR;
    }

    rc = rdpaCtl_RdpaMwWanConf();
    if (rc)
    {
        wc_log("Failed to call rdpa_mw ioctl (rc=%d)\n", rc);
        return rc;
    }

    switch (wan_type)
    {
        case rdpa_wan_gpon:
        case rdpa_wan_xgpon:
            rc = load_gpon_modules(wan_type);
            break;
        case rdpa_wan_epon:
        case rdpa_wan_xepon:
            rc = get_shall_launch_epon_app(&shall_launch_epon_app);
            if (WAN_CONF_ERROR == rc)
                return WAN_CONF_ERROR;

            rc = load_epon_modules(shall_launch_epon_app);
            /*epon ae mode*/
            if (LAUNCH_EPON_APP_FALSE == shall_launch_epon_app)
            {
                char ifname[IFNAMESIZ];
                ifname[0] = '\0';

                rc = create_bcmenet_vport(ifname, ETHCREATEEPONAEVPORT, rdpa_wan_type_to_if(wan_type));
#if defined(BRCM_CMS_BUILD)
                rc = rc ? : sendEnablePort(ifname);
#endif /* BRCM_CMS_BUILD */
            }
            break;
        case rdpa_wan_gbe:
            {
                char ifname[IFNAMESIZ];
                ifname[0] = '\0';

                /* Get the enet wan port if already configured in rdpa */
                rc = get_enet_wan_port(ifname, IFNAMESIZ);
                if (rc)
                    goto exit;

                if (ifname[0] == '\0')
                    rc = create_bcmenet_vport(ifname, ETHINITWAN, rdpa_wan_type_to_if(wan_type));

#if defined(BRCM_CMS_BUILD)
                rc = rc ? : sendEnablePort(ifname);
#endif /* BRCM_CMS_BUILD */
                break;
            }
        default:
            wc_log_err("Unsupported wanconf type set in scratchpad %s = %s\n", RDPA_WAN_TYPE_PSP_KEY, buf);
            return WAN_CONF_ERROR;
    }

    if (CMSRET_SUCCESS != (rc = devCtl_boardIoctl(BOARD_IOCTL_GET_ID, 0, boardid, sizeof(boardid), 0, NULL)))
    {
         wc_log_err("Could not get Board Id");
         goto exit;
    }
    else if (!strcmp(boardid,"963158REF2"))
    {
         /* Workaround for a REF2 hardware issue */
         /* Set rx_pmd_dp_invert bit(RX PMD Datapath Invert Control) in SerDes */
         if (WAN_CONF_ERROR == system("serdesctrl reg 0xd0d3 0x1"))
         {
             wc_log_err("Failed to execute command - serdesctrl reg 0xd0d3 0x1");
         }
         else
         {
             printf("REF2 SerDes Workaround applied successfully\n");
         }
    }


exit:
    if (rc)
    {
        wc_log("wanconf was not loaded successfully (rc=%d)\n", rc);
        return rc;
    }

    rc = rdpaCtl_time_sync_init();
    if (rc)
    {
        wc_log("Failed to call rdpaCtl_time_sync_init ioctl (rc=%d)\n", rc);
        return rc;
    }

    return WAN_CONF_SUCCESS;
}

