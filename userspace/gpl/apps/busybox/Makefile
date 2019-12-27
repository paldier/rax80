
all dynamic install: conditional_build

#
# Set our CommEngine directory (by splitting the pwd into two words
# at /userspace and taking the first word only).
# Then include the common defines under CommEngine.
# You do not need to modify this part.
#
CURR_DIR := $(shell pwd)
BUILD_DIR:=$(subst /userspace, /userspace,$(CURR_DIR))
BUILD_DIR:=$(word 1, $(BUILD_DIR))

include $(BUILD_DIR)/make.common

export INC_KERNEL_BASE

APP = busybox-1.28.3


.PHONY: check_untar_patch_configure conditional_build check_versions $(APP)/.config $(APP)/Config.h

ifneq ($(strip $(BUILD_BUSYBOX)),)
#conditional_build: $(APP) $(APP)_vanilla
conditional_build: $(APP) 

CMS_BUILD=0
BUILD_BUSYBOX_STATIC_FLAG=n
ifneq ($(strip $(BUILD_BRCM_CMS)),)
CMS_BUILD=1
BUILD_BUSYBOX_STATIC_FLAG=y
endif


# Depend on tar file
# Use $(APP)/Makefile to mark tar progress is completed
$(APP)/Makefile: $(APP).tar.bz2 Makefile
#	rm -rf $(APP)
#	mkdir $(APP)
#	cd $(APP) && tar --strip-components=1 -xkf ../$(APP).tar.bz2
#	@echo "$(APP) is untarred"
#	if [ $(CMS_BUILD) -eq 1 ]; then \
#		echo "*********************Applying patches to $(APP)"; \
#		patch -p1 -b -s -d$(APP) < $(APP).patch; \
#	fi
#	touch $@

# Depend on $(APP)/Makefile so that tar progress should be done only once
$(APP)/.config: $(APP)/Makefile $(wildcard config.d/*.conf)
	cat bbconfig-router_AX6000 > $(APP)/.config
#	cat brcm.config > $(APP)/.config
#	for i in config.d/[0-9]*.conf ; \
		do echo "config script $$i" ; \
		BUSYBOX_VANILLA_BUILD=n BRCM_EXTRA_PATH=$(BRCM_BUSYBOX_EXTRA_PATH) perl $$i $(APP)/.config; \
	done
	for i in config.d/[0-9]*.conf ; \
		do echo "config script $$i" ; \
		BUSYBOX_VANILLA_BUILD=n perl $$i $(APP)/.config; \
	done
	@echo "$(APP) is configured"

$(APP)/Config.h: $(APP)/.config
	$(MAKE) -C $(APP) include/autoconf.h
	cd $(APP) && cp -f include/autoconf.h Config.h

ifneq ($(strip $(BUILD_TCPSPDTEST)),)
TCPSPDTEST_LIBS = tcpspdtest_genl
endif

# Depend on $(APP)/.config so that make progress can continue where it is stopped before
$(APP): $(APP)/.config $(APP)/Config.h
	BRCM_LIBS="cms_msg cms_util bcm_flashutil bcm_crc dl bcm_boardctl resolv $(TCPSPDTEST_LIBS)" $(MAKE) -C $(APP) prepare
	BRCM_LIBS="cms_msg cms_util bcm_flashutil bcm_crc dl bcm_boardctl resolv $(TCPSPDTEST_LIBS)" $(MAKE) -C $(APP) install
	@echo "Done building $(APP)"

# Depend on tar file
# Use $(APP)/Makefile to mark tar progress is completed
$(APP)_vanilla/Makefile: $(APP).tar.bz2 Makefile
	rm -rf $(APP)_vanilla
	mkdir $(APP)_vanilla
	cd $(APP)_vanilla && tar --strip-components=1 -xkf ../$(APP).tar.bz2
	@echo "$(APP)_vanilla is untarred"
	touch $@

# Depend on $(APP)_vanilla/configure so that tar progress should be done only once
$(APP)_vanilla/.config: $(APP)_vanilla/Makefile $(wildcard config.d/*.conf)
#	cat bbconfig-router_AX6000 > $(APP)_vanilla/.config
	cat brcm.config > $(APP)_vanilla/.config
	for i in config.d/[0-9]*.conf ; \
		do echo "config script $$i" ; \
		BUSYBOX_VANILLA_BUILD=y BUILD_BUSYBOX_STATIC=$(BUILD_BUSYBOX_STATIC_FLAG) perl $$i $(APP)_vanilla/.config; \
	done
	@echo "$(APP)_vanilla is configured"

$(APP)_vanilla/Config.h: $(APP)_vanilla/.config
	$(MAKE) -C $(APP)_vanilla include/autoconf.h
	cd $(APP)_vanilla && cp -f include/autoconf.h Config.h

# Depend on check_config so that make progress can continue where it is stopped before
$(APP)_vanilla: $(APP)_vanilla/.config $(APP)_vanilla/Config.h
	@echo "Building vanilla"
	$(MAKE) -C $(APP)_vanilla prepare
	$(MAKE) -C $(APP)_vanilla install
	@echo "Done building $(APP)_vanilla"
else
conditional_build: sanity_check
	@echo "skipping $(APP) (not configured)"
endif

# NOTE: make clean from within app does not do a proper job, so wiping out
# entire directory to ensure consistency.
clean:
	#rm -rf $(APP) $(APP)_vanilla
	cd $(APP) ; make clean
	#cd $(APP)_vanilla ; make clean
	rm -rf $(APP)_vanilla

# The next line is a hint to our release scripts
# GLOBAL_RELEASE_SCRIPT_CALL_DISTCLEAN
distclean: clean

bcm_dorel_distclean: distclean

shell:
	bash -i
