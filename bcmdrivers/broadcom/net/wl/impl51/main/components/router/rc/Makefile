#
# Router rc control scripts Makefile
#
# Copyright (C) 2018, Broadcom. All Rights Reserved.
#
# Permission to use, copy, modify, and/or distribute this software for any
# purpose with or without fee is hereby granted, provided that the above
# copyright notice and this permission notice appear in all copies.
#
# THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
# WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
# MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
# SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
# WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
# OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
# CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
#
#
# <<Broadcom-WL-IPTag/Open:>>
#
# $Id: Makefile 758512 2018-04-19 05:26:58Z $
#

include $(TOP)/.config

ifneq (2_4,$(LINUX_VERSION))
ifneq (2_6,$(LINUX_VERSION))
CFLAGS += -DLINUX26 -DLINUX_2_6_36 -DAUTOFW_PORT_DEPRECATED
else
CFLAGS += -DLINUX26
endif

ifneq ($(strip $(BCA_HNDROUTER)),)
BB_BASE_PATH = $(BUILD_DIR)/userspace/gpl/apps/busybox
BB_HEADER_PATH = $(firstword $(dir $(shell find $(BB_BASE_PATH) -name  Config.h)))
ifeq ($(strip $(BB_HEADER_PATH)),)
$(error "Could not found busybox header path in $(BB_BASE_PATH)")
else
CFLAGS += -I$(BB_HEADER_PATH)
endif
CFLAGS += -I$(BUILD_DIR)/userspace/public/include
CFLAGS += -I$(BUILD_DIR)/userspace/public/include/linux
CFLAGS += -I$(INC_BRCMDRIVER_PUB_PATH)/$(BRCM_BOARD)
ifeq ($(strip $(BUILD_ETHWAN)),)
# GMAC builds do not support WAN
CFLAGS += -DNO_ETHWAN
endif
else
ifneq ($(CONFIG_STBAP),y)
CFLAGS += -I$(LINUXDIR)/include -I$(TOP)/busybox-1.x
endif # CONFIG_STBAP
endif # BCA_HNDROUTER

ifeq ($(BCA_HND_EAP),y)
CFLAGS += -DBCA_HND_EAP
endif

CFLAGS += -I. -I$(TOP)/shared -I$(SRCBASE)/include ${WLAN_StdIncPathA} -I$(SRCBASE)/../components/shared -I$(SRCBASE)/../components/wlioctl/include -I$(SRCBASE)/../components/proto/include -I$(SRCBASE)/common/include -Wall  -I$(SRCBASE)/../ap/acos/include
else # 2.4
CFLAGS += -I. -I$(TOP)/shared -I$(TOP)/busybox -I$(SRCBASE)/include ${WLAN_StdIncPathA} -I$(SRCBASE)/../components/shared -I$(SRCBASE)/common/include -Wall
endif

CFLAGS += -I$(SRCBASE)/../components/math/include -I$(SRCBASE)/../components/math/src

CFLAGS += $(if $(WLAN_ComponentIncPathA),$(WLAN_ComponentIncPathA),$(addprefix -I,$(wildcard $(SRCBASE)/shared/bcmwifi/include)))
CFLAGS += -I$(TOP)/dpsta

ifeq ($(FW_TYPE),NA)
export CFLAGS += -DFW_VERSION_NA
endif

ifeq ($(PROFILE),AX6000)
CFLAGS += -DRESTART_ALL_PROCESSES
CFLAGS += -DET_CMD_NEW
endif

#CFLAGS	+= -g -DDEBUG
CFLAGS	+= -s
ifeq ($(CONFIG_RTR_OPTIMIZE_SIZE),y)
CFLAGS  += -Os
else
CFLAGS  += -O2
endif
LDFLAGS	+= -L$(TOP)/nvram -L$(PLATFORMDIR)/install/nvram/usr/lib -lnvram -L$(TOP)/shared -L$(PLATFORMDIR)/install/shared/usr/lib -lshared \
	-L$(TOP)/libbcmcrypto -L$(INSTALLDIR)/libbcmcrypto/usr/lib -lbcmcrypto
LDFLAGS += -L$(TOP)/libbcm -L$(INSTALLDIR)/libbcm/usr/lib -lbcm
ifeq ($(LINUXDIR),$(BUILD_DIR)/kernel/linux-4.1)
CFLAGS  += -I$(BUILD_DIR)/userspace/ap/acos/include
LDFLAGS += -L$(ACOSTOPDIR)/shared -lacos_shared 
LDFLAGS += -L$(BUILD_DIR)/targets/$(PROFILE)/fs.install/lib  -L$(BUILD_DIR)/targets/$(PROFILE)/fs.install/usr/lib -lnvram -lwlcsm 
CFLAGS  += -I$(BUILD_DIR)/bcmdrivers/broadcom/net/wl/impl51/main/src/router/shared 
LDFLAGS += -L$(BUILD_DIR)/bcmdrivers/broadcom/net/wl/impl51/main/src/router/shared 
vpath %.c $(BUILD_DIR)/bcmdrivers/broadcom/net/wl/impl51/main/src/router/shared 
CFLAGS  += -I$(BUILD_DIR)/bcmdrivers/broadcom/net/wl/impl51/main/src/include
CFLAGS  += -I$(BUILD_DIR)/bcmdrivers/broadcom/net/wl/impl51/main/src/router/dpsta
CFLAGS  += -I$(ACOSTOPDIR)/include
CFLAGS  += -I$(LINUXDIR)/include/uapi
ifneq ($(strip $(BCA_HNDROUTER)),)
CFLAGS  += -I$(ACOSTOPDIR)/arno_nat_cli/include
LDFLAGS += -L$(ACOSTOPDIR)/arno_nat_cli -lnat
else
CFLAGS  += -I$(ACOSTOPDIR)/acos_nat_cli/include
LDFLAGS += -L$(ACOSTOPDIR)/acos_nat_cli -lnat
endif
else
CFLAGS	+= -I$(SRCBASE)/../ap/acos/shared
LDFLAGS	+= -L$(SRCBASE)/../ap/acos/shared -L$(SRCBASE)/../ap/acos/acos_nat_cli -L$(TARGETDIR)/usr/lib -lacos_shared -lnat
endif
LDFLAGS += -lcrypt
CFLAGS += -DFXCN_PATCH

#Foxconn Silver added start, 2011/12/21, for kernel_ntfs
ifneq ($(PROFILE),)	# PROFILE eq R6300v2 || R6250
CFLAGS += -DUSE_KERNEL_NTFS
endif
#Foxconn Silver added end, 2011/12/21, for kernel_ntfs
LDFLAGS += $(EXTRA_LDFLAGS)

ifeq ($(BUILD_WFA_APUT),1)
CFLAGS	+= -DCAPI_AP
endif

ifeq ($(FW_TYPE),TWC)
CFLAGS += -DBUILD_TWC
endif
ifeq ($(CONFIG_WAPI_IAS),y)
CFLAGS += -DRC_BUILDTIME=$(shell date -d "now" +"%s")
endif

OBJS := rc.o init.o interface.o network.o services.o udhcpc.o http.o stats.o bcmutils.o mtd.o usb_hotplug.o pmon.o bcm_math.o
OBJS += rc_patch.o
ifeq ($(CONFIG_BCMQOS),y)
OBJS += qos.o
endif

ifeq ($(CONFIG_NAT),y)
OBJS += firewall.o
LDFLAGS += -L$(TOP)/netconf -L$(PLATFORMDIR)/install/netconf/usr/lib -lnetconf
endif

ifeq ($(CONFIG_NAT),y)
OBJS += ppp.o
endif

ifeq ($(RWL),1)
CFLAGS += -DRWL_SOCKET
endif

ifeq ($(CONFIG_WCN),y)
OBJS += usb_hotplug.o
endif



ifneq ($(PROFILE),)  # R6300v2 || R6250
OBJS += hotplug_usb2.o
endif

ifeq ($(BCA_HNDROUTER),y)
LDFLAGS += -L$(TARGETDIR)/lib -lethctl
endif

vpath %.c $(TOP)/shared $(SRCBASE)/shared $(SRCBASE)/../components/math/src

all: rc

clean:
	rm -f *.o rc .*.depend

install: all
	install -d $(INSTALLDIR)/sbin
	install rc $(INSTALLDIR)/sbin
	$(STRIP) $(INSTALLDIR)/sbin/rc
ifeq ($(strip $(BCA_HNDROUTER)),)
	cd $(INSTALLDIR)/sbin && ln -sf rc write_cfe
endif
	cd $(INSTALLDIR)/sbin && ln -sf rc write
	cd $(INSTALLDIR)/sbin && ln -sf rc mtd_isbad
	cd $(INSTALLDIR)/sbin && ln -sf rc mtd_markbad
	cd $(INSTALLDIR)/sbin && ln -sf rc mtd_write_oob
	cd $(INSTALLDIR)/sbin && ln -sf rc mtd_read_oob
	cd $(INSTALLDIR)/sbin && ln -sf rc mtd_write_page
	cd $(INSTALLDIR)/sbin && ln -sf rc init
	cd $(INSTALLDIR)/sbin && ln -sf rc stats
	cd $(INSTALLDIR)/sbin && ln -sf rc hotplug
	cd $(INSTALLDIR)/sbin && ln -sf rc preinit
	cd $(INSTALLDIR)/sbin && ln -sf rc erase

rc: $(OBJS)
	$(CC) -o $@ $(OBJS) $(LDFLAGS) -lgcc_s

%.o: .%.depend
.%.depend: %.c
	$(CC) $(CFLAGS) -M $< > $@

-include $(OBJS:%.o=.%.depend)
