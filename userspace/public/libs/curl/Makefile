
libcurl: conditional_build 

CURR_DIR := $(shell pwd)
BUILD_DIR:=$(subst /userspace, /userspace,$(CURR_DIR))
BUILD_DIR:=$(word 1, $(BUILD_DIR))
include $(BUILD_DIR)/make.common

ifeq ($(strip $(DESKTOP_LINUX)),y)
BCM_BLT32_FLAGS = CFLAGS='$(BCM_LD_FLAGS)'
TOOLCHAIN_PREFIX=$(PROFILE_ARCH)-none-linux
endif

ifeq ($(strip $(BUILD_LIBCURL_WITH_SSL)),y)
CPPFLAGS += -I$(BCM_FSBUILD_DIR)/public/include
LDFLAGS += -L$(BCM_FSBUILD_DIR)/lib -L$(BCM_FSBUILD_DIR)/public/lib
BCM_BLT32_FLAGS += CPPFLAGS="$(CPPFLAGS)" LDFLAGS="$(LDFLAGS)" LIBS="-lssl -lcrypto" --with-ssl=$(BCM_FSBUILD_DIR)/lib -without-zlib
endif

ifeq ($(strip $(BUILD_LIBCURL_WITH_HTTP2)),y)
BCM_BLT32_FLAGS += --with-nghttp2 --with-ca-bundle=/etc/ssl/certs/cert.pem
endif


export LINUX_VER_STR TOOLCHAIN_PREFIX
PKG_CONFIG_PATH=$(BCM_FSBUILD_DIR)/public/lib/pkgconfig
export PKG_CONFIG_PATH

APP = curl-7.57.0
ifneq ($(strip $(BUILD_LIBCURL)),)
conditional_build: all
else
conditional_build:
	@echo "skipping libcurl (not configured)"
endif


libcurl/configure: $(APP).tar.gz
	if [ ! -e libcurl ]; then \
		mkdir -p libcurl; \
		cd libcurl ; tar --strip-components=1 -xzf ../$(APP).tar.gz; \
		touch -c libcurl/configure; \
	fi; 
	@echo "libcurl is untarred" 

check_config: libcurl/Makefile

libcurl/Makefile: libcurl/configure
	cd libcurl ; ./configure --prefix=$(BCM_FSBUILD_DIR)/public/ --host=$(TOOLCHAIN_PREFIX) $(BCM_BLT32_FLAGS);

all: check_config
	mkdir -p $(INSTALL_DIR)/lib/public/
	cd libcurl ; make
	cd libcurl ; make install
	cp -d $(BCM_FSBUILD_DIR)/public/lib/libcurl*.so* $(INSTALL_DIR)/lib/public/

clean:
	-rm -f $(INSTALL_DIR)/lib/public/libcurl*.so*
	-rm -rf libcurl

bcm_dorel_distclean: distclean

distclean:
	-rm -rf libcurl

shell:
	bash -i

