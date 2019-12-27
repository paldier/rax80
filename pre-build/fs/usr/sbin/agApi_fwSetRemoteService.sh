#!/bin/sh

LOCKFILE="/tmp/agApi_fwSetRemoteService.sh.lock"
COUNTER=0
set -o noclobber
while ! echo $$ > $LOCKFILE; do
    echo "wait for agApi_fwSetRemoteService.sh finished"
	COUNTER=$((${COUNTER}+1))
    sleep 1
	if [ "$COUNTER" == "10" ]; then
		break;
	fi
done
set +o noclobber


RP_ENABLE="`nvram get fw_rsp_ping`"
USB_FTP_ENABLE="`nvram get usb_wan_ftp_enable`"
USB_FTP_PORT="`nvram get usb_wan_ftp_port`"
USB_HTTPS_ENABLE="`nvram get usb_wan_http_enable`"
USB_HTTPS_PORT="`nvram get usb_wan_http_port`"
MGMT_HTTPS_ENABLE="`nvram get http_rmenable`"
MGMT_HTTPS_PORT="`nvram get http_rmport`"
OPENVPN_ENABLE="`nvram get openvpnActive`"
OPENVPN_PORT="`nvram get openvpn_service_port`"
OPENVPN_PORT_TUN="`nvram get openvpn_service_port_tun`"
OPENVPN_PROTOCOL="`nvram get openvpn_protocol`"
OPENVPN_PROTOCOL_TUN="`nvram get openvpn_protocol_tun`"
OPENVPN_REDIRECT_GW="`nvram get openvpn_redirectGW`"
OPENVPN_TUN_IPADDR="`nvram get openvpn_tun_ipaddr`"
LAN_IFNAME="`nvram get lan_ifname`"
DHCP_START="`nvram get dhcp_start`"

WAN_PROTO="`nvram get wan_proto`"

LEAFP2P_ENABLE="`nvram get leafp2p_run`"

if [ "$WAN_PROTO" == "pppoe" ]; then
	WAN_IFNAME="ppp0"
elif [ "$WAN_PROTO" == "pptp" ]; then
	WAN_IFNAME="ppp0"
elif [ "$WAN_PROTO" == "l2tp" ]; then
	WAN_IFNAME="ppp0"
else
	WAN_IFNAME="`nvram get wan_ifname`"
fi

RET=`iptables -t filter -F REMOTE_MGMT_WAN_CHAIN`

if [ "$RP_ENABLE" == "1" ]; then
    RET=`iptables -A REMOTE_MGMT_WAN_CHAIN -i ${WAN_IFNAME} -p icmp --icmp-type echo-request -j ACCEPT`
else
    RET=`iptables -A REMOTE_MGMT_WAN_CHAIN -i ${WAN_IFNAME} -p icmp --icmp-type echo-request -j DROP`
fi

if [ "$USB_FTP_ENABLE" == "1" ]; then
    RET=`iptables -A REMOTE_MGMT_WAN_CHAIN -i ${WAN_IFNAME} -p tcp --dport ${USB_FTP_PORT} -j ACCEPT`
fi

if [ "$USB_HTTPS_ENABLE" == "1" ]; then
    RET=`iptables -A REMOTE_MGMT_WAN_CHAIN -i ${WAN_IFNAME} -p tcp --dport ${USB_HTTPS_PORT} -j ACCEPT`
fi

if [ "$MGMT_HTTPS_ENABLE" == "1" ]; then
    RET=`iptables -A REMOTE_MGMT_WAN_CHAIN -i ${WAN_IFNAME} -p tcp --dport ${MGMT_HTTPS_PORT} -j ACCEPT`
fi

if [ "$LEAFP2P_ENABLE" == "1" ]; then
    RET=`iptables -I REMOTE_MGMT_WAN_CHAIN 1 -i LeafNets -j ACCEPT`
fi


# Delete all OpenVPN rules
RET=`iptables -D FORWARD -j OPENVPN_FORWARD_CHAIN`
RET=`iptables -F OPENVPN_FORWARD_CHAIN`
RET=`iptables -t nat -F OPENVPN_POSTROUTING_CHAIN`

if [ "$OPENVPN_ENABLE" == "enable" ]; then
    RET=`iptables -A REMOTE_MGMT_WAN_CHAIN -i ${WAN_IFNAME} -p tcp --dport ${OPENVPN_PORT} -j ACCEPT`
    RET=`iptables -A REMOTE_MGMT_WAN_CHAIN -i ${WAN_IFNAME} -p udp --dport ${OPENVPN_PORT} -j ACCEPT`
    RET=`iptables -A REMOTE_MGMT_WAN_CHAIN -i tun0 -p ${OPENVPN_PROTOCOL_TUN} --dport ${OPENVPN_PORT} -j ACCEPT`
    RET=`iptables -A REMOTE_MGMT_WAN_CHAIN -i tap0 -p ${OPENVPN_PROTOCOL} --dport ${OPENVPN_PORT} -j ACCEPT`
	
    RET=`iptables -A REMOTE_MGMT_WAN_CHAIN -i ${WAN_IFNAME} -p tcp --dport ${OPENVPN_PORT_TUN} -j ACCEPT`
    RET=`iptables -A REMOTE_MGMT_WAN_CHAIN -i ${WAN_IFNAME} -p udp --dport ${OPENVPN_PORT_TUN} -j ACCEPT`
    RET=`iptables -A REMOTE_MGMT_WAN_CHAIN -i tun0 -p ${OPENVPN_PROTOCOL_TUN} --dport ${OPENVPN_PORT_TUN} -j ACCEPT`
    RET=`iptables -A REMOTE_MGMT_WAN_CHAIN -i tap0 -p ${OPENVPN_PROTOCOL} --dport ${OPENVPN_PORT_TUN} -j ACCEPT`
	
	# make LAN can be accessable via VPN	
    RET=`iptables -A REMOTE_MGMT_WAN_CHAIN -i tun0 -j ACCEPT`
	
	# Allow traffic initiated from VPN to access LAN
	RET=`iptables -I OPENVPN_FORWARD_CHAIN -i tun0 -o ${LAN_IFNAME} -s ${OPENVPN_TUN_IPADDR}/24 -d ${DHCP_START}/24 -m conntrack --ctstate NEW -j ACCEPT`
	
	if [ "$OPENVPN_REDIRECT_GW" == "all" ] || [ "$OPENVPN_REDIRECT_GW" == "auto" ]; then
		#Allow traffic initiated from VPN to access "the world"
		RET=`iptables -I OPENVPN_FORWARD_CHAIN -i tun0 -o ${WAN_IFNAME}  -s ${OPENVPN_TUN_IPADDR}/24 -m conntrack --ctstate NEW -j ACCEPT`

		#Masquerade traffic from VPN to "the world" -- done in the nat table
		RET=`iptables -t nat -I OPENVPN_POSTROUTING_CHAIN -o ${WAN_IFNAME}  -s ${OPENVPN_TUN_IPADDR}/24 -j MASQUERADE`
	fi

	RET=`iptables -I FORWARD -j OPENVPN_FORWARD_CHAIN`
fi

rm -f $LOCKFILE
