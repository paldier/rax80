############################################################################################
#
#	Makefile For curl
#
#	Author: Kathy Lai
#	Date: 2014.04.02
#	
#	Description:
#		It will be used by xAgent implementation.
#		We will use the version curl-7.36.0 for xAgent.
#
#############################################################################################
include ../config.mk
include ../config.in

ifeq ($(CONFIG_CLOUD_XAGENT_CONF),y)
CURL_DIR=curl-7.57.0
SUBDIRS := curl-7.57.0
endif

CURL_DIR_PATH=$(shell pwd)

HOST="arm-linux"
HOST_COMPILE=$(HOST)-
ifeq ($(LINUXDIR),$(BASEDIR)/kernel/linux-4.1)
CROSS_COMPILE=arm-buildroot-linux-gnueabi-
else
CROSS_COMPILE=arm-uclibc-linux-2.6.36-
endif
OPENSSL_DIR=$(GPLTOPDIR)/openssl
.PHONY: curl libcurl

all: curl_conf sub_clean curl curl_preinstall dist_clean libcurl_conf sub_clean libcurl

curl_conf:
	ln -sf $(CURL_DIR) curl
	cd $(CURL_DIR); \
	./configure --host=$(HOST)  CC=$(CROSS_COMPILE)gcc CFLAGS="$(CFLAGS) -I$(OPENSSL_DIR)/include" LDFLAGS="$(LDFLAGS) -L$(OPENSSL_DIR)" --prefix=$(TARGETDIR) --disable-shared --with-ssl=$(OPENSSL_DIR) --disable-manual --disable-proxy --without-zlib \
		 				--disable-cookies --disable-ipv6 --disable-dict --disable-file --disable-ftp --disable-gopher \
						--disable-imap --disable-pop3 --disable-smtp --disable-telnet --disable-tftp --disable-rtsp  
curl:	
	touch $(CURL_DIR)/Makefile
	(cd $(CURL_DIR); make) || exit 1;

curl_preinstall:
	cp $(CURL_DIR_PATH)/$(CURL_DIR)/src/curl $(CURL_DIR_PATH)/binary
	
	 
libcurl_conf:
	cd $(CURL_DIR); \
	./configure --host=$(HOST)  CC=$(CROSS_COMPILE)gcc  CFLAGS="$(CFLAGS) -I$(OPENSSL_DIR)/include" LDFLAGS="$(LDFLAGS) -L$(OPENSSL_DIR)" --prefix=$(TARGETDIR) --enable-shared --with-ssl=$(OPENSSL_DIR) --disable-manual --disable-proxy --without-zlib \
						--disable-cookies --disable-ipv6 --disable-dict --disable-file --disable-ftp --disable-gopher \
						--disable-imap --disable-pop3 --disable-smtp --disable-telnet --disable-tftp --disable-rtsp  
libcurl:
	(cd $(CURL_DIR); make) || exit 1;
	cp $(CURL_DIR_PATH)/$(CURL_DIR)/lib/.libs/libcurl.* $(CURL_DIR_PATH)/binary	

sub_clean:
	for i in ${SUBDIRS}; do \
		(cd $$i; make clean); \
	done
	
dist_clean:
	for i in ${SUBDIRS}; do \
		(cd $$i; make distclean) || exit 1; \
	done
	
clean: curl_conf sub_clean libcurl_conf sub_clean
	rm -rf $(CURL_DIR_PATH)/binary/*
	rm -rf $(TARGETDIR)/sbin/curl
	rm -rf $(TARGETDIR)/usr/lib/libcurl.*
install:
#	$(CROSS_COMPILE)strip $(CURL_DIR_PATH)/binary/curl
	install -d $(TARGETDIR)/sbin/
	install -m 755 $(CURL_DIR_PATH)/binary/curl $(TARGETDIR)/sbin
	install -m 755 $(CURL_DIR_PATH)/binary/libcurl.*  $(TARGETDIR)/usr/lib

