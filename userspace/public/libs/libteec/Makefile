# Libteec makefile
OPTEE_VER := 3.0.0
OPTEE_TAR := $(OPTEE_VER).tar.gz
OPTEE_DIR := optee_client-$(OPTEE_VER)
OPTEE_OUT := $(OPTEE_DIR)/out/libteec

all: conditional_build

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


ifeq ($(strip $(BCM_OPTEE)),y)
conditional_build: libteec
clean: libteec_clean
else
conditional_build clean: ;
endif

$(OPTEE_DIR)/Makefile: $(OPTEE_TAR)
	@rm -rf $(OPTEE_DIR)
	@echo "Untarring source and overrides..."
	@tar xf $(OPTEE_TAR) 2> /dev/null
	@touch $@

libteec: $(OPTEE_DIR)/Makefile
	$(MAKE) -C $(OPTEE_DIR) build-libteec
	@mkdir -p $(INSTALL_DIR)/lib/
	@cp -a $(OPTEE_OUT)/libteec.so* $(INSTALL_DIR)/lib/
	@cp -a $(OPTEE_OUT)/libteec.so* $(BCM_FSBUILD_DIR)/public/lib/
	@cp -a $(OPTEE_OUT)/libteec.a   $(BCM_FSBUILD_DIR)/public/lib/
	@cp $(OPTEE_DIR)/public/*.h $(BCM_FSBUILD_DIR)/public/include/

libteec_clean:
	$(MAKE) -C $(OPTEE_DIR) clean-libteec
	@rm -f $(INSTALL_DIR)/lib/libteec.*

bcm_dorel_distclean: distclean

distclean:
	-rm -rf $(OPTEE_DIR)

