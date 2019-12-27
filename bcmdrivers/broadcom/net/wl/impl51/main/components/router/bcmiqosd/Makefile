#
# bcmiqosd Makefile
#
# Broadcom Proprietary and Confidential. Copyright (C) 2016,
# All Rights Reserved.
# 
# This is UNPUBLISHED PROPRIETARY SOURCE CODE of Broadcom;
# the contents of this file may not be disclosed to third parties, copied
# or duplicated in any form, in whole or in part, without the prior
# written permission of Broadcom.
#
# $Id: $
#
export INSTALLDIR := $(BUILDDIR)/targets/$(PROFILE)/fs
export TARGETDIR := $(BUILDDIR)/targets/$(PROFILE)/fs

CFLAGS += -I. -I$(TOP)/shared -I$(SRCBASE)/include -Wall

CFLAGS	+= -s
CFLAGS  += -O2

LDFLAGS	+= -L$(TOP)/nvram -L$(PLATFORMDIR)/install/nvram/usr/lib -lnvram -L$(TOP)/shared -L$(PLATFORMDIR)/install/shared/usr/lib -lshared \
	-L$(TOP)/libbcmcrypto -L$(INSTALLDIR)/libbcmcrypto/usr/lib -lbcmcrypto
LDFLAGS += -L$(TOP)/libbcm -L$(INSTALLDIR)/libbcm/usr/lib -lbcm
LDFLAGS += $(EXTRA_LDFLAGS)
#LDFLAGS	+= -lpthread

# Foxconn added, Sinclair, 10/20/15
CFLAGS += -DFOXCONN_ADDED
LDFLAGS += -lgcc_s

all: bcmiqosd

clean:
	rm -f *.o bcmiqosd

install: all
	install -d $(INSTALLDIR)/usr/sbin
	install bcmiqosd $(INSTALLDIR)/usr/sbin
	$(STRIP) $(INSTALLDIR)/usr/sbin/bcmiqosd

bcmiqosd: bcmiqosd.o
