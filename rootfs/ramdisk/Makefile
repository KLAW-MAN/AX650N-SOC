HOME_PATH		:= $(abspath $(shell pwd)/../..)
BUILD_PATH 		:= $(HOME_PATH)/build
KERNEL_PATH		:= $(HOME_PATH)/kernel/linux

include $(BUILD_PATH)/color.mk
include $(BUILD_PATH)/config.mak

OUT_DIR       		:= $(BUILD_PATH)/out/$(PROJECT)/objs
IMAGE_OUTDIR  		:= $(BUILD_PATH)/out/$(PROJECT)/images
OUT_ROOTFS		:= $(BUILD_PATH)/out/$(PROJECT)/rootfs
RAMDISK_ROOTFS		:= $(HOME_PATH)/rootfs/ramdisk/rootfs
SRC_ROOTFS_DIR		:= $(HOME_PATH)/rootfs
BUILD_ROOT_DIR	:= $(OUT_DIR)
ORIG_ROOTFS := rootfs
UBUNTU_ROOTFS := ubuntu_rootfs
ROOTFS_LIST := $(ORIG_ROOTFS)
ifeq ($(strip $(use_ubuntu_rootfs)),yes)
ROOTFS_LIST += $(UBUNTU_ROOTFS)
endif

ifeq ($(asan), yes)
CROSS_PATH     := $(abspath $(dir $(shell whereis $(CC)|awk '{print $$2}'))/..)
CROSS_NAME     := $(shell echo $(CROSS) | sed -e 's/-$$//g')
CROSS_LIB_PATH := $(CROSS_PATH)/$(CROSS_NAME)/lib64
endif

.PHONY: all rootfs minirootfs

default: rootfs

all: prepare rootfs
	@echo -e $(GREEN)"\nBuild Rootfs success!!\n" $(DONE)

prepare:
	@$(ECHO) -e $(GREEN) "Making $@..." $(DONE)
	@$(MKDIR) -p $(OUT_DIR)
	@$(MKDIR) -p $(BUILD_ROOT_DIR)/rootfs
	tar -zxpf $(SRC_ROOTFS_DIR)/ubuntu_rootfs.tar.gz -C $(BUILD_ROOT_DIR)
	@$(MKDIR) -p $(IMAGE_OUTDIR)

rootfs:
	@$(ECHO) -e $(GREEN) "Making $@..." $(DONE)
	@for ROOTFS_TARGET in $(ROOTFS_LIST);do \
	$(MKDIR) -p $(BUILD_ROOT_DIR)/$$ROOTFS_TARGET/proc; \
	$(MKDIR) -p $(BUILD_ROOT_DIR)/$$ROOTFS_TARGET/sys; \
	$(MKDIR) -p $(BUILD_ROOT_DIR)/$$ROOTFS_TARGET/mnt; \
	$(MKDIR) -p $(BUILD_ROOT_DIR)/$$ROOTFS_TARGET/sys; \
	$(MKDIR) -p $(BUILD_ROOT_DIR)/$$ROOTFS_TARGET/tmp; \
	$(MKDIR) -p $(BUILD_ROOT_DIR)/$$ROOTFS_TARGET/run; \
	$(MKDIR) -p $(BUILD_ROOT_DIR)/$$ROOTFS_TARGET/root; \
	$(MKDIR) -p $(BUILD_ROOT_DIR)/$$ROOTFS_TARGET/media; \
	$(MKDIR) -p $(BUILD_ROOT_DIR)/$$ROOTFS_TARGET/param; \
	$(MKDIR) -p $(BUILD_ROOT_DIR)/$$ROOTFS_TARGET/soc; \
	$(MKDIR) -p $(BUILD_ROOT_DIR)/$$ROOTFS_TARGET/opt; \
	done;

image:
#clean first
	@for ROOTFS_TARGET in $(ROOTFS_LIST); do \
	rm -rf $(BUILD_ROOT_DIR)/$$ROOTFS_TARGET/opt/*; \
	rm -rf $(BUILD_ROOT_DIR)/$$ROOTFS_TARGET/soc/*; \
	done;
	$(CP) -avf $(SRC_ROOTFS_DIR)/rootfs/* $(BUILD_ROOT_DIR)/rootfs;
	$(CP) -avf $(SRC_ROOTFS_DIR)/rootfs/usr/local/lib/* $(BUILD_ROOT_DIR)/ubuntu_rootfs/usr/local/lib
	$(CP) -avf $(SRC_ROOTFS_DIR)/rootfs/usr/local/bin/* $(BUILD_ROOT_DIR)/ubuntu_rootfs/usr/local/bin

# copy soc
	$(ECHO) -e $(GREEN) "make soc partiton" $(DONE);
	@for ROOTFS_TARGET in $(ROOTFS_LIST); do \
	$(MKDIR) -p $(BUILD_ROOT_DIR)/$$ROOTFS_TARGET/soc/lib; \
	$(MKDIR) -p $(BUILD_ROOT_DIR)/$$ROOTFS_TARGET/soc/ko; \
	$(MKDIR) -p $(BUILD_ROOT_DIR)/$$ROOTFS_TARGET/soc/spl; \
	cp -rf $(SRC_ROOTFS_DIR)/rootfs/opt/scripts $(BUILD_ROOT_DIR)/$$ROOTFS_TARGET/soc; \
	done;
ifeq ($(debug), yes)
	@for ROOTFS_TARGET in $(ROOTFS_LIST); do \
	cp -rf $(HOME_PATH)/msp/out/lib/*.debug $(BUILD_ROOT_DIR)/$$ROOTFS_TARGET/soc/lib; \
	cp -rf $(HOME_PATH)/kernel/osdrv/out/debug_ko/*.ko $(BUILD_ROOT_DIR)/$$ROOTFS_TARGET/soc/ko; \
	for debug in `find $(BUILD_ROOT_DIR)/$$ROOTFS_TARGET/soc/lib -name "*.debug"`;do mv $$debug `echo $$debug|sed 's/\.debug/\.so/'` ;done; \
	done
else
	@for ROOTFS_TARGET in $(ROOTFS_LIST);do \
	cp -rf $(HOME_PATH)/msp/out/lib/*.so* $(BUILD_ROOT_DIR)/$$ROOTFS_TARGET/soc/lib; \
	cp -rf $(HOME_PATH)/kernel/osdrv/out/ko/*.ko $(BUILD_ROOT_DIR)/$$ROOTFS_TARGET/soc/ko; \
	done;
endif

ifeq ($(asan), yes)
	@for ROOTFS_TARGET in $(ROOTFS_LIST);do \
	cp -rf $(CROSS_LIB_PATH)/libasan*.so* $(BUILD_ROOT_DIR)/$$ROOTFS_TARGET/soc/lib; \
	done;
endif

	@for ROOTFS_TARGET in $(ROOTFS_LIST); do \
	rm $(BUILD_ROOT_DIR)/$$ROOTFS_TARGET/etc/fstab*; \
	cp -rf $(RAMDISK_ROOTFS)/etc/* $(BUILD_ROOT_DIR)/$$ROOTFS_TARGET/etc; \
	cp -rf $(RAMDISK_ROOTFS)/soc/scripts/* $(BUILD_ROOT_DIR)/$$ROOTFS_TARGET/soc/scripts/; \
	done

ifneq ($(CMM_POOL_PARAM),)
	@for ROOTFS_TARGET in $(ROOTFS_LIST); do \
	sed -i "s/cmmpool*=.*/cmmpool=$(CMM_POOL_PARAM)/g" $(BUILD_ROOT_DIR)/$$ROOTFS_TARGET/soc/scripts/auto_load_all_drv.sh; \
	done;
endif

	@for ROOTFS_TARGET in $(ROOTFS_LIST); do \
	cp -rf $(IMAGE_OUTDIR)/spl_AX650_slave_signed.bin $(BUILD_ROOT_DIR)/$$ROOTFS_TARGET/soc/spl/; \
# copy opt ; \
	$(MKDIR) -p $(BUILD_ROOT_DIR)/$$ROOTFS_TARGET/opt/bin; \
	$(MKDIR) -p $(BUILD_ROOT_DIR)/$$ROOTFS_TARGET/opt/data; \
	$(MKDIR) -p $(BUILD_ROOT_DIR)/$$ROOTFS_TARGET/opt/etc; \
	$(MKDIR) -p $(BUILD_ROOT_DIR)/$$ROOTFS_TARGET/opt/lib; \
	cp -rf $(HOME_PATH)/msp/out/bin/* $(BUILD_ROOT_DIR)/$$ROOTFS_TARGET/opt/bin; \
	cp -rf $(HOME_PATH)/msp/out/data/* $(BUILD_ROOT_DIR)/$$ROOTFS_TARGET/opt/data; \
	cp -rf $(HOME_PATH)/msp/out/etc/* $(BUILD_ROOT_DIR)/$$ROOTFS_TARGET/opt/etc; \
	cp -rf $(HOME_PATH)/msp/out/lib/qt-5.15.7 $(BUILD_ROOT_DIR)/$$ROOTFS_TARGET/opt/lib/; \
	rm -rf $(BUILD_ROOT_DIR)/$$ROOTFS_TARGET/opt/scripts; \
	done;
ifeq ($(PROJECT),AX650_slave)
	@for ROOTFS_TARGET in $(ROOTFS_LIST); do \
	rm -rf $(BUILD_ROOT_DIR)/$$ROOTFS_TARGET/opt/etc/models/aiisp/*.axmodel; \
	done;
endif
ifeq ($(SUPPORT_VALGRIND), yes)
	@for ROOTFS_TARGET in $(ROOTFS_LIST); do \
	cp -rf $(VALGRIND_PATH)/bin/* $(BUILD_ROOT_DIR)/$$ROOTFS_TARGET/opt/bin; \
	cp -rf $(VALGRIND_PATH)/libexec $(BUILD_ROOT_DIR)/$$ROOTFS_TARGET/opt/; \
	done;
endif

# rootfs image
	@for ROOTFS_TARGET in $(ROOTFS_LIST); do \
	$(HOME_PATH)/tools/mkext4fs/make_ext4fs -l 2048M $(BUILD_PATH)/out/$(PROJECT)/images/$$ROOTFS_TARGET.ext4 $(BUILD_ROOT_DIR)/$$ROOTFS_TARGET; \
	$(HOME_PATH)/tools/mkext4fs/make_ext4fs -l 2048M -s $(BUILD_PATH)/out/$(PROJECT)/images/$$ROOTFS_TARGET\_sparse.ext4 $(BUILD_ROOT_DIR)/$$ROOTFS_TARGET; \
	done;

	@$(ECHO) -e $(GREEN) "make rootfs partiton" $(DONE)

install:
	@$(ECHO)

clean:
	@echo -e $(GREEN) "rootfs $@..." $(DONE)
	@for ROOTFS_TARGET in $(ROOTFS_LIST); do \
	rm -rf $(BUILD_ROOT_DIR)/$$ROOTFS_TARGET; \
	rm -rf $(BUILD_PATH)/out/$(PROJECT)/images/$$ROOTFS_TARGET.ext4; \
	rm -rf $(BUILD_PATH)/out/$(PROJECT)/images/$$ROOTFS_TARGET\_sparse.ext4; \
	done;
