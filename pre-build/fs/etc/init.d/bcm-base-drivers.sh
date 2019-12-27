#!/bin/sh

trap "" 2


case "$1" in
	start)
		echo "Loading drivers and kernel modules... "
		echo

# Syntax
# line
# conditon
# condition
# for
# -BUILD_FEATURE_A
# will
# -!BUILD_FEATURE_B
# will
 
# UBUS
 
insmod /lib/modules/4.1.51/extra/rdp_fpm.ko  
insmod /lib/modules/4.1.51/extra/bdmf.ko  
 
#I2C 
 
# PON
 
# BPM
 
# RDPA
insmod /lib/modules/4.1.51/extra/rdpa_gpl.ko  
insmod /lib/modules/4.1.51/extra/rdpa_gpl_ext.ko  
insmod /lib/modules/4.1.51/extra/rdpa.ko  
 
# RDPA_User
insmod /lib/modules/4.1.51/extra/rdpa_usr.ko  
 
insmod /lib/modules/4.1.51/extra/rdpa_mw.ko  
 
# General
insmod /lib/modules/4.1.51/extra/chipinfo.ko  
 
# Ingress
# Must
 
# RDPA
insmod /lib/modules/4.1.51/extra/rdpa_cmd.ko  
 
insmod /lib/modules/4.1.51/extra/pktflow.ko  
insmod /lib/modules/4.1.51/extra/tdts.ko  
 
# enet
 
 
insmod /lib/modules/4.1.51/extra/bcm_enet.ko  
# moving
insmod /lib/modules/4.1.51/extra/cmdlist.ko  
insmod /lib/modules/4.1.51/extra/pktrunner.ko  
insmod /lib/modules/4.1.51/extra/nciTMSkmod.ko  
insmod /lib/modules/4.1.51/extra/bcmmcast.ko  
 
#load SATA/AHCI
insmod /lib/modules/4.1.51/kernel/drivers/ata/libata.ko  
insmod /lib/modules/4.1.51/kernel/drivers/ata/libahci.ko  
insmod /lib/modules/4.1.51/kernel/drivers/ata/ahci.ko  
insmod /lib/modules/4.1.51/extra/bcm_sata.ko  
insmod /lib/modules/4.1.51/kernel/drivers/ata/libahci_platform.ko  
insmod /lib/modules/4.1.51/kernel/drivers/ata/ahci_platform.ko  
 
# PCIe
insmod /lib/modules/4.1.51/extra/bcm_pcie_hcd.ko  
 
# pcie
 
# WLAN
insmod /lib/modules/4.1.51/extra/wfd.ko  
 
# NetXL
 
#Voice 
 
 
#load usb
insmod /lib/modules/4.1.51/kernel/drivers/usb/host/ehci-hcd.ko  
insmod /lib/modules/4.1.51/kernel/drivers/usb/host/ehci-platform.ko  
insmod /lib/modules/4.1.51/kernel/drivers/usb/host/ehci-pci.ko  
insmod /lib/modules/4.1.51/kernel/drivers/usb/host/ohci-hcd.ko  
insmod /lib/modules/4.1.51/kernel/drivers/usb/host/ohci-platform.ko  
insmod /lib/modules/4.1.51/kernel/drivers/usb/host/ohci-pci.ko  
insmod /lib/modules/4.1.51/kernel/drivers/usb/host/xhci-hcd.ko  
insmod /lib/modules/4.1.51/kernel/drivers/usb/host/xhci-plat-hcd.ko  
insmod /lib/modules/4.1.51/extra/bcm_usb.ko  
insmod /lib/modules/4.1.51/kernel/drivers/usb/class/usblp.ko  
insmod /lib/modules/4.1.51/kernel/drivers/usb/storage/usb-storage.ko  
insmod /lib/modules/4.1.51/kernel/drivers/usb/storage/uas.ko  
 
# other
 
insmod /lib/modules/4.1.51/extra/bcmvlan.ko  
insmod /lib/modules/4.1.51/extra/pwrmngtd.ko  
insmod /lib/modules/4.1.51/extra/bcmpdc.ko  
insmod /lib/modules/4.1.51/extra/bcmspu.ko  
 
insmod /lib/modules/4.1.51/extra/bcm_thermal.ko  
 
# presecure
 
# LTE
 
# MEMC
 
# Sysperf
# Must

 test -e /etc/rdpa_init.sh && /etc/rdpa_init.sh

# Enable the PKA driver.
 test -e /sys/devices/platform/bcm_pka/enable && echo 1 > /sys/devices/platform/bcm_pka/enable

exit 0
		;;

	stop)
		echo "removing bcm base drivers not implemented yet..."
		exit 1
		;;

	*)
		echo "bcmbasedrivers: unrecognized option $1"
		exit 1
		;;

esac


