
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

APP = gptfdisk-1.0.3

.PHONY: check_untar_patch_configure conditional_build check_versions 

ifneq ($(strip $(BUILD_GPTFDISK)),)
conditional_build: $(APP)
else
conditional_build: sanity_check
	@echo "skipping $(APP) (not configured)"

endif

$(APP): sanity_check
	(tar xkzf $(APP).tar.gz 2> /dev/null || true)
	echo "Applying patches to $(APP)" ; \
	patch -p1 -b -N -s -d$(APP) < $(APP).patch ; \
	$(MAKE) -C $(APP) sgdisk LDFLAGS=-L$(BCM_FSBUILD_DIR)/public/lib CFLAGS=-I$(BCM_FSBUILD_DIR)/public/include CXXFLAGS=-I$(BCM_FSBUILD_DIR)/public/include
	cp $(APP)/sgdisk $(INSTALL_DIR)/bin/


clean:
	-make -C  $(APP) clean
	-rm -f $(INSTALL_DIR)/bin/sgdisk
	-rm -rf $(APP)

