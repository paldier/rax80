/*
 * This is a module which is used for logging packets.
 */

/* (C) 1999-2001 Paul `Rusty' Russell
 * (C) 2002-2004 Netfilter Core Team <coreteam@netfilter.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt
#include <linux/module.h>
#include <linux/spinlock.h>
#include <linux/skbuff.h>
#include <linux/if_arp.h>
#include <linux/ip.h>
#include <net/ipv6.h>
#include <net/icmp.h>
#include <net/udp.h>
#include <net/tcp.h>
#include <net/route.h>

#include <linux/netfilter.h>
#include <linux/netfilter/x_tables.h>
#include <linux/netfilter/xt_LOG.h>
#include <linux/netfilter_ipv6/ip6_tables.h>
#include <net/netfilter/nf_log.h>

#define LOG_CMD "/usr/sbin/log_cli"
#define GetIpHeaderLen(ptIp) (((ptIp->ihl)) * 4)
#define GetIpPayload(ptIp) ((char *)ptIp + GetIpHeaderLen(ptIp))


static char *envp[] = {
             "HOME=/",
             "PATH=/sbin:/bin:/usr/sbin:/usr/bin",
             NULL
};

static int get_ip(char *src_ip, struct sk_buff *skb)
{
    unsigned char a[4];
    struct iphdr *pIpHdr = (struct iphdr*)skb_network_header(skb);
    
	memcpy(a, (unsigned char*)&(pIpHdr->saddr), 4);
	sprintf(src_ip, "%d.%d.%d.%d", a[0], a[1], a[2], a[3]);
	return 0;
}

static int get_dst_ip(char *dst_ip, struct sk_buff *skb)
{
    unsigned char a[4];
    struct iphdr *pIpHdr = (struct iphdr*)skb_network_header(skb);
    
	memcpy(a, (unsigned char*)&(pIpHdr->daddr), 4);
	sprintf(dst_ip, "%d.%d.%d.%d", a[0], a[1], a[2], a[3]);
	return 0;
}

static int get_id(char *id, struct sk_buff *skb)
{
    struct iphdr *pIpHdr = (struct iphdr*)skb_network_header(skb);
    
	sprintf(id, "%u", ntohs(pIpHdr->id));
	return 0;
}

static int get_ip_len(char *ip_len, struct sk_buff *skb)
{
    struct iphdr *pIpHdr = (struct iphdr*)skb_network_header(skb);
    
	sprintf(ip_len, "%u", ntohs(pIpHdr->tot_len));
	return 0;
}

static int get_tcp_seq(char *tcp_seq, struct sk_buff *skb)
{
    struct iphdr *pIpHdr = (struct iphdr*)skb_network_header(skb);
    
    struct tcphdr *th;
    th = (struct tcphdr *)GetIpPayload(pIpHdr);	    
    sprintf(tcp_seq, "%u", th->seq);
	return 0;
}

static int get_tcp_ack(char *tcp_ack, struct sk_buff *skb)
{
    struct iphdr *pIpHdr = (struct iphdr*)skb_network_header(skb);
    
    struct tcphdr *th;
    th = (struct tcphdr *)GetIpPayload(pIpHdr);	    
    sprintf(tcp_ack, "%u", th->ack_seq);
	return 0;
}

static int get_tcp_len(char *tcp_len, struct sk_buff *skb)
{ 
    sprintf(tcp_len, "%lu", sizeof(struct tcphdr));
	return 0;
}


static int get_src_port(char *src_port, struct sk_buff *skb)
{
    struct iphdr *iph = (struct iphdr*)skb_network_header(skb);

    if (iph->protocol == IPPROTO_TCP)
    {
	    struct tcphdr *th;
        th = (struct tcphdr *)GetIpPayload(iph);	    
        sprintf(src_port, "%u", ntohs(th->source));
    }
    else if (iph->protocol == IPPROTO_UDP)
    {
	    struct udphdr *uh;
        uh = (struct udphdr *)GetIpPayload(iph);	    
        sprintf(src_port, "%u", ntohs(uh->source));
    }
	return 0;
}

static int get_dst_port(char *dst_port, struct sk_buff *skb)
{
    struct iphdr *iph = (struct iphdr*)skb_network_header(skb);

    if (iph->protocol == IPPROTO_TCP)
    {
	    struct tcphdr *th;
        th = (struct tcphdr *)GetIpPayload(iph);
        sprintf(dst_port, "%u", ntohs(th->dest));
    }
    else if (iph->protocol == IPPROTO_UDP)
    {
	    struct udphdr *uh;
        uh = (struct udphdr *)GetIpPayload(iph);	    
        sprintf(dst_port, "%u", ntohs(uh->dest));
    }
	return 0;
}

static int log_DOS_ATTACK(char *service, struct sk_buff *skb)
{
     int ret;
     static char src_ip[20];
     static char src_port[20];
     static char blk_serv[64];
     
     static char *argv [6];
     
     strcpy(blk_serv, service+11);
     get_ip(src_ip, skb);
     get_src_port(src_port, skb);

     argv [0] = LOG_CMD;
     argv [1] = "DOS_ATTACK";
     argv [2] = blk_serv;
     argv [3] = src_ip;
     argv [4] = src_port;
     argv [5] = NULL;
     printk("%s %s %s %s %s\n", argv[0], argv[1], argv[2], argv[3], argv[4]);     
     ret = call_usermodehelper(argv[0], argv, envp, UMH_NO_WAIT);
     
     //printk("ret: %d\n", ret);

     return ret;
}

static int log_BLOCK_SERVICE(char *service, struct sk_buff *skb)
{
     int ret;
     static char src_ip[20];
     static char blk_serv[64];
     
     static char *argv [6];
     
     strcpy(blk_serv, service+14);
     get_ip(src_ip, skb);
     //printk(KERN_EMERG "log_BLOCK_SERVICE\n");

     argv [0] = LOG_CMD;
     argv [1] = "BLOCK_SERVICE";
     argv [2] = blk_serv;
     argv [3] = src_ip;
     argv [4] = NULL;
     argv [5] = NULL;
     printk("%s %s %s %s\n", argv[0], argv[1], argv[2], argv[3]);     
     ret = call_usermodehelper(argv[0], argv, envp, UMH_NO_WAIT);
     
     //printk("ret: %d\n", ret);

     return ret;
}

static int log_BLOCK_SITE(char *service, struct sk_buff *skb)
{
    int ret=0;
    static char src_ip[20];
    static char blk_word[128];
    static char action[16];

    static char *argv [6];

    if(strncmp(service+11, "block", 5)==0)	 
        strcpy(action, "block");
    else
        strcpy(action, "allow");

    strcpy(blk_word, service+17);
    get_ip(src_ip, skb);
    //printk(KERN_EMERG "log_BLOCK_SITE\n");

     argv [0] = LOG_CMD;
     argv [1] = "BLOCK_SITE";
     argv [2] = blk_word;
     argv [3] = src_ip;
     argv [4] = action;
     argv [5] = NULL;
     
    printk("%s %s %s %s %s\n", argv[0], argv[1], argv[2], argv[3], argv[4]);     
     ret = call_usermodehelper(argv[0], argv, envp, UMH_NO_WAIT);
     
     //printk(KERN_EMERG "ret: %d\n", ret);

     return ret;
}

static int send_BLOCK_page(char *service, struct sk_buff *skb)
{
    int ret=0;
    static char src_ip[20];
    static char dst_ip[20];
    static char src_port[20];
    static char dst_port[20];
    static char id[20];
    static char ip_len[16];
    static char tcp_seq[16];
    static char tcp_ack[16];
    static char tcp_len[16];

    static char *argv [11];

    if(strncmp(service+11, "block", 5)==0)	 
        ;
    else
        return 0;

    get_ip(src_ip, skb);
    get_dst_ip(dst_ip, skb);
    get_src_port(src_port, skb);
    get_dst_port(dst_port, skb);
    get_id(id, skb);
    get_ip_len(ip_len, skb);
    get_tcp_seq(tcp_seq, skb);
    get_tcp_ack(tcp_ack, skb);
    get_tcp_len(tcp_len, skb);
    //printk(KERN_EMERG "log_BLOCK_SITE\n");

     argv [0] = LOG_CMD;
     argv [1] = "BLOCK_PAGE";
     argv [2] = src_ip;
     argv [3] = src_port;
     argv [4] = id;
     argv [5] = dst_ip;
     argv [6] = ip_len;
     argv [7] = tcp_seq;
     argv [8] = tcp_ack;
     argv [9] = tcp_len;
     argv [10] = NULL;
     
    printk("%s %s %s %s %s %s %s %s %s %s\n", argv[0], argv[1], argv[2], argv[3], argv[4], \
        argv[5], argv[6], argv[7], argv[8], argv[9]);     
     ret = call_usermodehelper(argv[0], argv, envp, UMH_NO_WAIT);
     
     //printk(KERN_EMERG "ret: %d\n", ret);

     return ret;
}

static int log_PORT_FORWARD(char *service, struct sk_buff *skb)
{
    int ret=0;
    static char src_ip[20];
    static char dst_ip[20];
    static char src_port[20];
    static char dst_port[20];

    static char *argv [7];

    get_ip(src_ip, skb);
    get_dst_ip(dst_ip, skb);
    get_src_port(src_port, skb);
    get_dst_port(dst_port, skb);
    
    //printk(KERN_EMERG "log_PORT_FORWARD\n");

     argv [0] = LOG_CMD;
     argv [1] = "PORT_FORWARD";
     argv [2] = src_ip;
     argv [3] = src_port;
     argv [4] = dst_ip;
     argv [5] = dst_port;
     argv [6] = NULL;
     
     printk("%s %s %s %s %s %s\n", argv[0], argv[1], argv[2], argv[3], argv[4], argv[5]);     
     ret = call_usermodehelper(argv[0], argv, envp, UMH_NO_WAIT);
     
     //printk(KERN_EMERG "ret: %d\n", ret);

     return ret;
}

static unsigned int
log_tg(struct sk_buff *skb, const struct xt_action_param *par)
{
	const struct xt_log_info *loginfo = par->targinfo;
	struct nf_loginfo li;
	struct net *net = dev_net(par->in ? par->in : par->out);

	li.type = NF_LOG_TYPE_LOG;
	li.u.log.level = loginfo->level;
	li.u.log.logflags = loginfo->logflags;

    if(strncmp(loginfo->prefix, "BLOCK_SERVICE", 13)==0)
    {
    	log_BLOCK_SERVICE((char*)loginfo->prefix, skb);
    }
    else if(strncmp(loginfo->prefix, "BLOCK_SITE", 10)==0)
    {
    	log_BLOCK_SITE((char*)loginfo->prefix, skb);
    	send_BLOCK_page((char*)loginfo->prefix, skb);
    }
    else if(strncmp(loginfo->prefix, "PORT_FORWARD", 12)==0)
    {
    	log_PORT_FORWARD((char*)loginfo->prefix, skb);
    }
    else if(strncmp(loginfo->prefix, "DOS_ATTACK", 10)==0)
    {
    	log_DOS_ATTACK((char*)loginfo->prefix, skb);
    }
    	
	nf_log_packet(net, par->family, par->hooknum, skb, par->in, par->out,
		      &li, "%s", loginfo->prefix);
	return XT_CONTINUE;
}

static int log_tg_check(const struct xt_tgchk_param *par)
{
	const struct xt_log_info *loginfo = par->targinfo;

	if (par->family != NFPROTO_IPV4 && par->family != NFPROTO_IPV6)
		return -EINVAL;

	if (loginfo->level >= 8) {
		pr_debug("level %u >= 8\n", loginfo->level);
		return -EINVAL;
	}

	if (loginfo->prefix[sizeof(loginfo->prefix)-1] != '\0') {
		pr_debug("prefix is not null-terminated\n");
		return -EINVAL;
	}

	return nf_logger_find_get(par->family, NF_LOG_TYPE_LOG);
}

static void log_tg_destroy(const struct xt_tgdtor_param *par)
{
	nf_logger_put(par->family, NF_LOG_TYPE_LOG);
}

static struct xt_target log_tg_regs[] __read_mostly = {
	{
		.name		= "LOG",
		.family		= NFPROTO_IPV4,
		.target		= log_tg,
		.targetsize	= sizeof(struct xt_log_info),
		.checkentry	= log_tg_check,
		.destroy	= log_tg_destroy,
		.me		= THIS_MODULE,
	},
#if IS_ENABLED(CONFIG_IP6_NF_IPTABLES)
	{
		.name		= "LOG",
		.family		= NFPROTO_IPV6,
		.target		= log_tg,
		.targetsize	= sizeof(struct xt_log_info),
		.checkentry	= log_tg_check,
		.destroy	= log_tg_destroy,
		.me		= THIS_MODULE,
	},
#endif
};

static int __init log_tg_init(void)
{
	return xt_register_targets(log_tg_regs, ARRAY_SIZE(log_tg_regs));
}

static void __exit log_tg_exit(void)
{
	xt_unregister_targets(log_tg_regs, ARRAY_SIZE(log_tg_regs));
}

module_init(log_tg_init);
module_exit(log_tg_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Netfilter Core Team <coreteam@netfilter.org>");
MODULE_AUTHOR("Jan Rekorajski <baggins@pld.org.pl>");
MODULE_DESCRIPTION("Xtables: IPv4/IPv6 packet logging");
MODULE_ALIAS("ipt_LOG");
MODULE_ALIAS("ip6t_LOG");
