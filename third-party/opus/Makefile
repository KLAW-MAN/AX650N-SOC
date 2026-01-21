CUR_PATH		   := $(shell pwd)
HOME_PATH		   := $(CUR_PATH)/../..
ROOTFS_TARGET_PATH ?= $(HOME_PATH)/rootfs/rootfs/usr/local

include $(HOME_PATH)/build/color.mk
include $(HOME_PATH)/build/config.mak

all:
	@echo -e $(GREEN)"\nBuild opus success!!\n"  $(DONE)


install:
	@mkdir -p $(ROOTFS_TARGET_PATH)/lib
	@cp -rf $(CUR_PATH)/lib/* $(ROOTFS_TARGET_PATH)/lib
	@echo -e $(GREEN)"\nInstall opus success!!\n"  $(DONE)

clean:
	@rm -f $(ROOTFS_TARGET_PATH)/lib/libopus.so
	@rm -f $(ROOTFS_TARGET_PATH)/lib/libopus.so.0
	@rm -f $(ROOTFS_TARGET_PATH)/lib/libopus.so.0.8.0

	@echo -e $(GREEN)"\nClean opus success!!\n"  $(DONE)







