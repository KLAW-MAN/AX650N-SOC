CUR_PATH		   := $(shell pwd)
HOME_PATH		   := $(CUR_PATH)/../..
ROOTFS_TARGET_PATH := $(HOME_PATH)/rootfs/rootfs

include $(HOME_PATH)/build/color.mk
include $(HOME_PATH)/build/config.mak

all:
	@echo -e $(GREEN)"\nBuild optee success!!\n"  $(DONE)

install:
ifneq ($(strip $(SUPPORT_OPTEE)),false)
	@mkdir -p $(ROOTFS_TARGET_PATH)/lib
	@cp -d $(CUR_PATH)/libteec/lib/libteec.so* $(ROOTFS_TARGET_PATH)/usr/lib
	@mkdir -p $(ROOTFS_TARGET_PATH)/usr/sbin
	@cp -d $(CUR_PATH)/usr/sbin/tee-supplicant $(ROOTFS_TARGET_PATH)/usr/sbin
	@mkdir -p $(ROOTFS_TARGET_PATH)/etc/init.d
	@cp -d $(CUR_PATH)/etc/init.d/S30optee $(ROOTFS_TARGET_PATH)/etc/init.d
endif
	@echo -e $(GREEN)"\nInstall optee success!!\n"  $(DONE)

clean:
	@rm -f $(ROOTFS_TARGET_PATH)/usr/lib/libteec.so*
	@rm -f $(ROOTFS_TARGET_PATH)/usr/sbin/tee-supplicant
	@rm -f $(ROOTFS_TARGET_PATH)/etc/init.d/S30optee
	@echo -e $(GREEN)"\nClean optee success!!\n"  $(DONE)







