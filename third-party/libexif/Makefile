CUR_PATH		   := $(shell pwd)
HOME_PATH		   := $(CUR_PATH)/../..
ROOTFS_TARGET_PATH ?= $(HOME_PATH)/rootfs/rootfs/usr/local

include $(HOME_PATH)/build/color.mk
include $(HOME_PATH)/build/config.mak

all:
	@echo -e $(GREEN)"\nBuild libexif success!!\n"  $(DONE)


install:
	@mkdir -p $(ROOTFS_TARGET_PATH)/lib
	@cp -rf $(CUR_PATH)/lib/* $(ROOTFS_TARGET_PATH)/lib
	@echo -e $(GREEN)"\nInstall libexif success!!\n"  $(DONE)

clean:
	@rm -f $(ROOTFS_TARGET_PATH)/lib/libexif.so
	@rm -f $(ROOTFS_TARGET_PATH)/lib/libexif.so.12
	@rm -f $(ROOTFS_TARGET_PATH)/lib/libexif.so.12.3.4
	@rm -f $(ROOTFS_TARGET_PATH)/lib/libexif.a
	@rm -f $(ROOTFS_TARGET_PATH)/lib/libexif.la

	@echo -e $(GREEN)"\nClean libexif success!!\n"  $(DONE)







