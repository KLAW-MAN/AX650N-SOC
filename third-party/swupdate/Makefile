CUR_PATH		   := $(shell pwd)
HOME_PATH		   := $(CUR_PATH)/../..
ROOTFS_TARGET_PATH := $(HOME_PATH)/rootfs/rootfs/opt/swupdate

include $(HOME_PATH)/build/color.mk
include $(HOME_PATH)/build/config.mak

all:
	@echo -e $(GREEN)"\nBuild swupdate success!!\n"  $(DONE)


install:
	@mkdir -p $(ROOTFS_TARGET_PATH)/bin
	@cp -rf $(CUR_PATH)/bin/* $(ROOTFS_TARGET_PATH)/bin
	@mkdir -p $(ROOTFS_TARGET_PATH)/lib
	@cp -rf $(CUR_PATH)/lib/* $(ROOTFS_TARGET_PATH)/lib
	@cp -rf $(CUR_PATH)/v2 $(ROOTFS_TARGET_PATH)/
	@cp -rf $(CUR_PATH)/etc/swupdate.cfg $(ROOTFS_TARGET_PATH)/../../etc/
	@echo -e $(GREEN)"\nInstall swupdate success!!\n"  $(DONE)

clean:
	@rm -rf $(ROOTFS_TARGET_PATH)
	@rm -rf $(ROOTFS_TARGET_PATH)/../../etc/swupdate.cfg

	@echo -e $(GREEN)"\nClean swupdate success!!\n"  $(DONE)







