LIB = libbdmf.so
default: dynamic


libbdmf.so:
	cp libbdmf.so_$(PROFILE).save libbdmf.so

dynamic: generic_private_lib_install

clean:
	rm -f libbdmf.so

CURR_DIR := $(shell pwd)
BUILD_DIR := $(subst /userspace, /userspace,$(CURR_DIR))
BUILD_DIR := $(word 1, $(BUILD_DIR))
include $(BUILD_DIR)/make.common
