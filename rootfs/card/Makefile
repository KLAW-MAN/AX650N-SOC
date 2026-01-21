HOME_PATH		:= $(abspath $(shell pwd)/../..)
BUILD_PATH 		:= $(HOME_PATH)/build
KERNEL_PATH		:= $(HOME_PATH)/kernel/linux

include $(BUILD_PATH)/color.mk
include $(BUILD_PATH)/config.mak

OUT_DIR		:= $(BUILD_PATH)/out/$(PROJECT)/objs
IMAGE_OUTDIR	:= $(BUILD_PATH)/out/$(PROJECT)/images
OUT_ROOTFS	:= $(BUILD_PATH)/out/$(PROJECT)/rootfs
CARD_ROOTFS_DIR	:= $(HOME_PATH)/rootfs/card
LOCAL_ROOTFS	:= $(CARD_ROOTFS_DIR)/rootfs
SRC_ROOTFS_DIR	:= $(HOME_PATH)/rootfs
BUILD_ROOT_DIR	:= $(OUT_DIR)
COPY_FILE	:= $(CARD_ROOTFS_DIR)/scripts/copy_files.py
PROJECT_MAK	:= $(BUILD_PATH)/projects/$(PROJECT).mak
DEB_VERSION	:= $(shell echo $(SDK_VERSION) | sed 's/^V//; s/_.*//')

#rootfs
ROOTFS_BASE_DIR	:= $(HOME_PATH)
DST_ROOTFS_DIR	:= $(OUT_DIR)/rootfs
ROOTFS_IMG	:= $(IMAGE_OUTDIR)/rootfs.ext4
ROOTFS_FILELIST	:= $(CARD_ROOTFS_DIR)/filelist/rootfs_filelist.txt
#soc
SOC_BASE_DIR	:= $(HOME_PATH)
DST_SOC_DIR	:= $(OUT_DIR)/rootfs/soc
SOC_FILELIST	:= $(CARD_ROOTFS_DIR)/filelist/soc_filelist.txt
#opt
OPT_BASE_DIR	:= $(HOME_PATH)
DST_OPT_DIR	:= $(OUT_DIR)/rootfs/opt
OPT_FILELIST	:= $(CARD_ROOTFS_DIR)/filelist/opt_filelist.txt

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
	@python3 $(CARD_ROOTFS_DIR)/scripts/load_board_config.py
	@$(ECHO) -e $(GREEN) "Making $@..." $(DONE)
	@$(MKDIR) -p $(OUT_DIR)
	@$(MKDIR) -p $(BUILD_ROOT_DIR)/rootfs
	@$(MKDIR) -p $(IMAGE_OUTDIR)

rootfs:
	@$(ECHO) -e $(GREEN) "Making $@..." $(DONE)
	@$(MKDIR) -p $(BUILD_ROOT_DIR)/rootfs/proc
	@$(MKDIR) -p $(BUILD_ROOT_DIR)/rootfs/sys
	@$(MKDIR) -p $(BUILD_ROOT_DIR)/rootfs/mnt
	@$(MKDIR) -p $(BUILD_ROOT_DIR)/rootfs/sys
	@$(MKDIR) -p $(BUILD_ROOT_DIR)/rootfs/tmp
	@$(MKDIR) -p $(BUILD_ROOT_DIR)/rootfs/run
	@$(MKDIR) -p $(BUILD_ROOT_DIR)/rootfs/root
	@$(MKDIR) -p $(BUILD_ROOT_DIR)/rootfs/media
	@$(MKDIR) -p $(BUILD_ROOT_DIR)/rootfs/param
	@$(MKDIR) -p $(BUILD_ROOT_DIR)/rootfs/soc
	@$(MKDIR) -p $(BUILD_ROOT_DIR)/rootfs/opt

image:
	@rm -rf $(BUILD_ROOT_DIR)/rootfs
#copy rootfs
	@python3 $(COPY_FILE) $(PROJECT) $(PROJECT_MAK) $(ROOTFS_BASE_DIR) $(ROOTFS_FILELIST) $(DST_ROOTFS_DIR)
#copy soc
	$(ECHO) -e $(GREEN) "make soc partiton" $(DONE);
	$(MKDIR) -p $(BUILD_ROOT_DIR)/rootfs/soc/lib
	$(MKDIR) -p $(BUILD_ROOT_DIR)/rootfs/soc/ko
	$(MKDIR) -p $(BUILD_ROOT_DIR)/rootfs/soc/spl

ifeq ($(strip $(SUPPORT_DEB_X86)),TRUE)
	$(RM)   -rf $(BUILD_ROOT_DIR)/axcl_x86_deb
	$(MKDIR) -p $(BUILD_ROOT_DIR)/axcl_x86_deb/DEBIAN
	$(MKDIR) -p $(BUILD_ROOT_DIR)/axcl_x86_deb/usr/lib/axcl
	$(MKDIR) -p $(BUILD_ROOT_DIR)/axcl_x86_deb/usr/bin/axcl
	$(MKDIR) -p $(BUILD_ROOT_DIR)/axcl_x86_deb/usr/include/axcl
	$(MKDIR) -p $(BUILD_ROOT_DIR)/axcl_x86_deb/etc/ld.so.conf.d
	$(MKDIR) -p $(BUILD_ROOT_DIR)/axcl_x86_deb/etc/profile.d
	$(MKDIR) -p $(BUILD_ROOT_DIR)/axcl_x86_deb/etc/modules-load.d
	$(MKDIR) -p $(BUILD_ROOT_DIR)/axcl_x86_deb/etc/udev/rules.d
	$(MKDIR) -p $(BUILD_ROOT_DIR)/axcl_x86_deb/lib/firmware/axcl
	$(MKDIR) -p $(BUILD_ROOT_DIR)/axcl_x86_deb/usr/src/axcl/build
	cp -rf $(HOME_PATH)/axcl/build/package/deb/control $(BUILD_ROOT_DIR)/axcl_x86_deb/DEBIAN
	@sed -i "s/^Version: .*/Version: ${DEB_VERSION}/" "$(BUILD_ROOT_DIR)/axcl_x86_deb/DEBIAN/control"
	cp -rf $(HOME_PATH)/axcl/build/package/deb/preinst $(BUILD_ROOT_DIR)/axcl_x86_deb/DEBIAN
	cp -rf $(HOME_PATH)/axcl/build/package/deb/postinst $(BUILD_ROOT_DIR)/axcl_x86_deb/DEBIAN
	cp -rf $(HOME_PATH)/axcl/build/package/deb/prerm $(BUILD_ROOT_DIR)/axcl_x86_deb/DEBIAN
	cp -rf $(HOME_PATH)/axcl/build/package/deb/postrm $(BUILD_ROOT_DIR)/axcl_x86_deb/DEBIAN
	cp -rf $(HOME_PATH)/axcl/build/package/axcl.conf $(BUILD_ROOT_DIR)/axcl_x86_deb/etc/ld.so.conf.d
	cp -rf $(HOME_PATH)/axcl/build/package/axcl.sh $(BUILD_ROOT_DIR)/axcl_x86_deb/etc/profile.d
	cp -rf $(HOME_PATH)/axcl/build/package/axcl_pcie.conf $(BUILD_ROOT_DIR)/axcl_x86_deb/etc/modules-load.d
	cp -rf $(HOME_PATH)/axcl/build/package/axcl_host.rules $(BUILD_ROOT_DIR)/axcl_x86_deb/etc/udev/rules.d
	cp -rf $(HOME_PATH)/axcl/out/axcl_linux_x86/data $(BUILD_ROOT_DIR)/axcl_x86_deb/usr/bin/axcl
	cp -rf $(HOME_PATH)/axcl/out/axcl_linux_x86/bin/* $(BUILD_ROOT_DIR)/axcl_x86_deb/usr/bin/axcl
	cp -rf $(HOME_PATH)/axcl/out/usrworker $(BUILD_ROOT_DIR)/axcl_x86_deb/usr/bin/axcl
	cp -rf $(HOME_PATH)/axcl/out/axcl_linux_x86/json/* $(BUILD_ROOT_DIR)/axcl_x86_deb/usr/bin/axcl
	cp -rf $(HOME_PATH)/axcl/out/axcl_linux_x86/lib/*.so* $(BUILD_ROOT_DIR)/axcl_x86_deb/usr/lib/axcl/
	cp -rf $(HOME_PATH)/axcl/out/axcl_linux_x86/lib/ffmpeg $(BUILD_ROOT_DIR)/axcl_x86_deb/usr/lib/axcl/
	cp -rfL $(HOME_PATH)/axcl/build/projects $(BUILD_ROOT_DIR)/axcl_x86_deb/usr/src/axcl/build
	cp -rfL $(HOME_PATH)/axcl/build/*.mak $(BUILD_ROOT_DIR)/axcl_x86_deb/usr/src/axcl/build
	cp -rfL $(HOME_PATH)/axcl/build/Makefile $(BUILD_ROOT_DIR)/axcl_x86_deb/usr/src/axcl/build
	cp -rfL $(HOME_PATH)/axcl/drv $(BUILD_ROOT_DIR)/axcl_x86_deb/usr/src/axcl
	cp -rfL $(HOME_PATH)/axcl/out/axcl_linux_x86/include/* $(BUILD_ROOT_DIR)/axcl_x86_deb/usr/include/axcl
endif

ifeq ($(strip $(SUPPORT_DEB_ARM64)),TRUE)
	$(RM)   -rf $(BUILD_ROOT_DIR)/axcl_arm64_deb
	$(MKDIR) -p $(BUILD_ROOT_DIR)/axcl_arm64_deb/DEBIAN
	$(MKDIR) -p $(BUILD_ROOT_DIR)/axcl_arm64_deb/usr/lib/axcl
	$(MKDIR) -p $(BUILD_ROOT_DIR)/axcl_arm64_deb/usr/bin/axcl
	$(MKDIR) -p $(BUILD_ROOT_DIR)/axcl_arm64_deb/usr/include/axcl
	$(MKDIR) -p $(BUILD_ROOT_DIR)/axcl_arm64_deb/etc/ld.so.conf.d
	$(MKDIR) -p $(BUILD_ROOT_DIR)/axcl_arm64_deb/etc/profile.d
	$(MKDIR) -p $(BUILD_ROOT_DIR)/axcl_arm64_deb/etc/modules-load.d
	$(MKDIR) -p $(BUILD_ROOT_DIR)/axcl_arm64_deb/etc/udev/rules.d
	$(MKDIR) -p $(BUILD_ROOT_DIR)/axcl_arm64_deb/lib/firmware/axcl
	$(MKDIR) -p $(BUILD_ROOT_DIR)/axcl_arm64_deb/usr/src/axcl/build
	cp -rf $(HOME_PATH)/axcl/build/package/deb/control $(BUILD_ROOT_DIR)/axcl_arm64_deb/DEBIAN
	@sed -i "s/^Version: .*/Version: $(DEB_VERSION)/" "$(BUILD_ROOT_DIR)/axcl_arm64_deb/DEBIAN/control"
	cp -rf $(HOME_PATH)/axcl/build/package/deb/preinst $(BUILD_ROOT_DIR)/axcl_arm64_deb/DEBIAN
	cp -rf $(HOME_PATH)/axcl/build/package/deb/postinst $(BUILD_ROOT_DIR)/axcl_arm64_deb/DEBIAN
	cp -rf $(HOME_PATH)/axcl/build/package/deb/prerm $(BUILD_ROOT_DIR)/axcl_arm64_deb/DEBIAN
	cp -rf $(HOME_PATH)/axcl/build/package/deb/postrm $(BUILD_ROOT_DIR)/axcl_arm64_deb/DEBIAN
	cp -rf $(HOME_PATH)/axcl/build/package/axcl.conf $(BUILD_ROOT_DIR)/axcl_arm64_deb/etc/ld.so.conf.d
	cp -rf $(HOME_PATH)/axcl/build/package/axcl.sh $(BUILD_ROOT_DIR)/axcl_arm64_deb/etc/profile.d
	cp -rf $(HOME_PATH)/axcl/build/package/axcl_pcie.conf $(BUILD_ROOT_DIR)/axcl_arm64_deb/etc/modules-load.d
	cp -rf $(HOME_PATH)/axcl/build/package/axcl_host.rules $(BUILD_ROOT_DIR)/axcl_arm64_deb/etc/udev/rules.d
	cp -rf $(HOME_PATH)/axcl/out/axcl_linux_arm64/data $(BUILD_ROOT_DIR)/axcl_arm64_deb/usr/bin/axcl
	cp -rf $(HOME_PATH)/axcl/out/axcl_linux_arm64/bin/* $(BUILD_ROOT_DIR)/axcl_arm64_deb/usr/bin/axcl
	cp -rf $(HOME_PATH)/axcl/out/usrworker $(BUILD_ROOT_DIR)/axcl_arm64_deb/usr/bin/axcl
	cp -rf $(HOME_PATH)/axcl/out/axcl_linux_arm64/json/* $(BUILD_ROOT_DIR)/axcl_arm64_deb/usr/bin/axcl
	cp -rf $(HOME_PATH)/axcl/out/axcl_linux_arm64/lib/*.so* $(BUILD_ROOT_DIR)/axcl_arm64_deb/usr/lib/axcl/
	cp -rf $(HOME_PATH)/axcl/out/axcl_linux_arm64/lib/ffmpeg $(BUILD_ROOT_DIR)/axcl_arm64_deb/usr/lib/axcl/
	cp -rf $(HOME_PATH)/axcl/build/projects $(BUILD_ROOT_DIR)/axcl_arm64_deb/usr/src/axcl/build
	cp -rfL $(HOME_PATH)/axcl/build/*.mak $(BUILD_ROOT_DIR)/axcl_arm64_deb/usr/src/axcl/build
	cp -rf $(HOME_PATH)/axcl/build/Makefile $(BUILD_ROOT_DIR)/axcl_arm64_deb/usr/src/axcl/build
	cp -rfL $(HOME_PATH)/axcl/drv $(BUILD_ROOT_DIR)/axcl_arm64_deb/usr/src/axcl
	cp -rfL $(HOME_PATH)/axcl/out/axcl_linux_arm64/include/* $(BUILD_ROOT_DIR)/axcl_arm64_deb/usr/include/axcl
endif

ifeq ($(strip $(SUPPORT_DEB_RISCV)),TRUE)
	$(RM)   -rf $(BUILD_ROOT_DIR)/axcl_riscv_deb
	$(MKDIR) -p $(BUILD_ROOT_DIR)/axcl_riscv_deb/DEBIAN
	$(MKDIR) -p $(BUILD_ROOT_DIR)/axcl_riscv_deb/usr/lib/axcl
	$(MKDIR) -p $(BUILD_ROOT_DIR)/axcl_riscv_deb/usr/bin/axcl
	$(MKDIR) -p $(BUILD_ROOT_DIR)/axcl_riscv_deb/usr/include/axcl
	$(MKDIR) -p $(BUILD_ROOT_DIR)/axcl_riscv_deb/etc/ld.so.conf.d
	$(MKDIR) -p $(BUILD_ROOT_DIR)/axcl_riscv_deb/etc/profile.d
	$(MKDIR) -p $(BUILD_ROOT_DIR)/axcl_riscv_deb/etc/modules-load.d
	$(MKDIR) -p $(BUILD_ROOT_DIR)/axcl_riscv_deb/etc/udev/rules.d
	$(MKDIR) -p $(BUILD_ROOT_DIR)/axcl_riscv_deb/lib/firmware/axcl
	$(MKDIR) -p $(BUILD_ROOT_DIR)/axcl_riscv_deb/usr/src/axcl/build
	cp -rf $(HOME_PATH)/axcl/build/package/deb/control $(BUILD_ROOT_DIR)/axcl_riscv_deb/DEBIAN
	@sed -i "s/^Version: .*/Version: $(DEB_VERSION)/" "$(BUILD_ROOT_DIR)/axcl_riscv_deb/DEBIAN/control"
	cp -rf $(HOME_PATH)/axcl/build/package/deb/preinst $(BUILD_ROOT_DIR)/axcl_riscv_deb/DEBIAN
	cp -rf $(HOME_PATH)/axcl/build/package/deb/postinst $(BUILD_ROOT_DIR)/axcl_riscv_deb/DEBIAN
	cp -rf $(HOME_PATH)/axcl/build/package/deb/prerm $(BUILD_ROOT_DIR)/axcl_riscv_deb/DEBIAN
	cp -rf $(HOME_PATH)/axcl/build/package/deb/postrm $(BUILD_ROOT_DIR)/axcl_riscv_deb/DEBIAN
	cp -rf $(HOME_PATH)/axcl/build/package/axcl.conf $(BUILD_ROOT_DIR)/axcl_riscv_deb/etc/ld.so.conf.d
	cp -rf $(HOME_PATH)/axcl/build/package/axcl.sh $(BUILD_ROOT_DIR)/axcl_riscv_deb/etc/profile.d
	cp -rf $(HOME_PATH)/axcl/build/package/axcl_pcie.conf $(BUILD_ROOT_DIR)/axcl_riscv_deb/etc/modules-load.d
	cp -rf $(HOME_PATH)/axcl/build/package/axcl_host.rules $(BUILD_ROOT_DIR)/axcl_riscv_deb/etc/udev/rules.d
	cp -rf $(HOME_PATH)/axcl/out/axcl_linux_riscv/data $(BUILD_ROOT_DIR)/axcl_riscv_deb/usr/bin/axcl
	cp -rf $(HOME_PATH)/axcl/out/axcl_linux_riscv/bin/* $(BUILD_ROOT_DIR)/axcl_riscv_deb/usr/bin/axcl
	cp -rf $(HOME_PATH)/axcl/out/usrworker $(BUILD_ROOT_DIR)/axcl_riscv_deb/usr/bin/axcl
	cp -rf $(HOME_PATH)/axcl/out/axcl_linux_riscv/json/* $(BUILD_ROOT_DIR)/axcl_riscv_deb/usr/bin/axcl
	cp -rf $(HOME_PATH)/axcl/out/axcl_linux_riscv/lib/*.so* $(BUILD_ROOT_DIR)/axcl_riscv_deb/usr/lib/axcl/
	cp -rf $(HOME_PATH)/axcl/out/axcl_linux_riscv/lib/ffmpeg $(BUILD_ROOT_DIR)/axcl_riscv_deb/usr/lib/axcl/
	cp -rf $(HOME_PATH)/axcl/build/projects $(BUILD_ROOT_DIR)/axcl_riscv_deb/usr/src/axcl/build
	cp -rfL $(HOME_PATH)/axcl/build/*.mak $(BUILD_ROOT_DIR)/axcl_riscv_deb/usr/src/axcl/build
	cp -rf $(HOME_PATH)/axcl/build/Makefile $(BUILD_ROOT_DIR)/axcl_riscv_deb/usr/src/axcl/build
	cp -rfL $(HOME_PATH)/axcl/drv $(BUILD_ROOT_DIR)/axcl_riscv_deb/usr/src/axcl
	cp -rfL $(HOME_PATH)/axcl/out/axcl_linux_riscv/include/* $(BUILD_ROOT_DIR)/axcl_riscv_deb/usr/include/axcl
endif

ifeq ($(strip $(SUPPORT_RPM_X86)),TRUE)
	$(RM)   -rf $(BUILD_ROOT_DIR)/axcl_x86_rpm
	$(MKDIR) -p $(BUILD_ROOT_DIR)/axcl_x86_rpm/SPECS
	$(MKDIR) -p $(BUILD_ROOT_DIR)/axcl_x86_rpm/SOURCES
	$(MKDIR) -p $(BUILD_ROOT_DIR)/axcl_x86_rpm/BUILD
	$(MKDIR) -p $(BUILD_ROOT_DIR)/axcl_x86_rpm/SOURCES/axcl/build
	$(MKDIR) -p $(BUILD_ROOT_DIR)/axcl_x86_rpm/SOURCES/axcl/image
	$(MKDIR) -p $(BUILD_ROOT_DIR)/axcl_x86_rpm/SOURCES/axcl/lib
	cp -rf $(HOME_PATH)/axcl/build/package/rpm/x86_64/axcl_host.spec $(BUILD_ROOT_DIR)/axcl_x86_rpm/SPECS
	cp -rf $(HOME_PATH)/axcl/build/package/axcl.conf $(BUILD_ROOT_DIR)/axcl_x86_rpm/SOURCES/axcl
	cp -rf $(HOME_PATH)/axcl/build/package/axcl_pcie.conf $(BUILD_ROOT_DIR)/axcl_x86_rpm/SOURCES/axcl
	cp -rf $(HOME_PATH)/axcl/build/package/axcl_host.rules $(BUILD_ROOT_DIR)/axcl_x86_rpm/SOURCES/axcl
	cp -rf $(HOME_PATH)/axcl/build/package/axcl.sh $(BUILD_ROOT_DIR)/axcl_x86_rpm/SOURCES/axcl
	cp -rf $(HOME_PATH)/axcl/build/projects $(BUILD_ROOT_DIR)/axcl_x86_rpm/SOURCES/axcl/build
	cp -rfL $(HOME_PATH)/axcl/build/*.mak $(BUILD_ROOT_DIR)/axcl_x86_rpm/SOURCES/axcl/build
	cp -rf $(HOME_PATH)/axcl/build/Makefile $(BUILD_ROOT_DIR)/axcl_x86_rpm/SOURCES/axcl/build
	cp -rfL $(HOME_PATH)/axcl/drv $(BUILD_ROOT_DIR)/axcl_x86_rpm/SOURCES/axcl
	cp -rfL $(HOME_PATH)/axcl/out/axcl_linux_x86/include $(BUILD_ROOT_DIR)/axcl_x86_rpm/SOURCES/axcl
	cp -rf $(HOME_PATH)/axcl/out/axcl_linux_x86/lib/*.so* $(BUILD_ROOT_DIR)/axcl_x86_rpm/SOURCES/axcl/lib/
	cp -rf $(HOME_PATH)/axcl/out/axcl_linux_x86/lib/ffmpeg $(BUILD_ROOT_DIR)/axcl_x86_rpm/SOURCES/axcl/lib/
	cp -rf $(HOME_PATH)/axcl/out/axcl_linux_x86/bin $(BUILD_ROOT_DIR)/axcl_x86_rpm/SOURCES/axcl
	cp -rf $(HOME_PATH)/axcl/out/usrworker $(BUILD_ROOT_DIR)/axcl_x86_rpm/SOURCES/axcl/bin/
	cp -rf $(HOME_PATH)/axcl/out/axcl_linux_x86/json $(BUILD_ROOT_DIR)/axcl_x86_rpm/SOURCES/axcl
	cp -rf $(HOME_PATH)/axcl/out/axcl_linux_x86/data $(BUILD_ROOT_DIR)/axcl_x86_rpm/SOURCES/axcl
	cp  -f $(HOME_PATH)/axcl/build/package/dkms.conf $(BUILD_ROOT_DIR)/axcl_x86_rpm/SOURCES/axcl/
endif

ifeq ($(strip $(SUPPORT_RPM_ARM64)),TRUE)
	$(RM)   -rf $(BUILD_ROOT_DIR)/axcl_arm64_rpm
	$(MKDIR) -p $(BUILD_ROOT_DIR)/axcl_arm64_rpm/SPECS
	$(MKDIR) -p $(BUILD_ROOT_DIR)/axcl_arm64_rpm/SOURCES
	$(MKDIR) -p $(BUILD_ROOT_DIR)/axcl_arm64_rpm/BUILD
	$(MKDIR) -p $(BUILD_ROOT_DIR)/axcl_arm64_rpm/SOURCES/axcl/build
	$(MKDIR) -p $(BUILD_ROOT_DIR)/axcl_arm64_rpm/SOURCES/axcl/image
	$(MKDIR) -p $(BUILD_ROOT_DIR)/axcl_arm64_rpm/SOURCES/axcl/lib
	cp -rf $(HOME_PATH)/axcl/build/package/rpm/arm64/axcl_host.spec $(BUILD_ROOT_DIR)/axcl_arm64_rpm/SPECS
	cp -rf $(HOME_PATH)/axcl/build/package/axcl.conf $(BUILD_ROOT_DIR)/axcl_arm64_rpm/SOURCES/axcl
	cp -rf $(HOME_PATH)/axcl/build/package/axcl_pcie.conf $(BUILD_ROOT_DIR)/axcl_arm64_rpm/SOURCES/axcl
	cp -rf $(HOME_PATH)/axcl/build/package/axcl_host.rules $(BUILD_ROOT_DIR)/axcl_arm64_rpm/SOURCES/axcl
	cp -rf $(HOME_PATH)/axcl/build/package/axcl.sh $(BUILD_ROOT_DIR)/axcl_arm64_rpm/SOURCES/axcl
	cp -rf $(HOME_PATH)/axcl/build/projects $(BUILD_ROOT_DIR)/axcl_arm64_rpm/SOURCES/axcl/build
	cp -rfL $(HOME_PATH)/axcl/build/*.mak $(BUILD_ROOT_DIR)/axcl_arm64_rpm/SOURCES/axcl/build
	cp -rf $(HOME_PATH)/axcl/build/Makefile $(BUILD_ROOT_DIR)/axcl_arm64_rpm/SOURCES/axcl/build
	cp -rfL $(HOME_PATH)/axcl/drv $(BUILD_ROOT_DIR)/axcl_arm64_rpm/SOURCES/axcl
	cp -rfL $(HOME_PATH)/axcl/out/axcl_linux_arm64/include $(BUILD_ROOT_DIR)/axcl_arm64_rpm/SOURCES/axcl
	cp -rf $(HOME_PATH)/axcl/out/axcl_linux_arm64/lib/*.so* $(BUILD_ROOT_DIR)/axcl_arm64_rpm/SOURCES/axcl/lib/
	cp -rf $(HOME_PATH)/axcl/out/axcl_linux_arm64/lib/ffmpeg $(BUILD_ROOT_DIR)/axcl_arm64_rpm/SOURCES/axcl/lib/
	cp -rf $(HOME_PATH)/axcl/out/axcl_linux_arm64/bin $(BUILD_ROOT_DIR)/axcl_arm64_rpm/SOURCES/axcl
	cp -rf $(HOME_PATH)/axcl/out/usrworker $(BUILD_ROOT_DIR)/axcl_arm64_rpm/SOURCES/axcl/bin/
	cp -rf $(HOME_PATH)/axcl/out/axcl_linux_arm64/json $(BUILD_ROOT_DIR)/axcl_arm64_rpm/SOURCES/axcl
	cp -rf $(HOME_PATH)/axcl/out/axcl_linux_arm64/data $(BUILD_ROOT_DIR)/axcl_arm64_rpm/SOURCES/axcl
endif

ifeq ($(strip $(SUPPORT_RPM_RISCV)),TRUE)
	$(RM)   -rf $(BUILD_ROOT_DIR)/axcl_riscv_rpm
	$(MKDIR) -p $(BUILD_ROOT_DIR)/axcl_riscv_rpm/SPECS
	$(MKDIR) -p $(BUILD_ROOT_DIR)/axcl_riscv_rpm/SOURCES
	$(MKDIR) -p $(BUILD_ROOT_DIR)/axcl_riscv_rpm/BUILD
	$(MKDIR) -p $(BUILD_ROOT_DIR)/axcl_riscv_rpm/SOURCES/axcl/build
	$(MKDIR) -p $(BUILD_ROOT_DIR)/axcl_riscv_rpm/SOURCES/axcl/image
	$(MKDIR) -p $(BUILD_ROOT_DIR)/axcl_riscv_rpm/SOURCES/axcl/lib
	cp -rf $(HOME_PATH)/axcl/build/package/rpm/riscv/axcl_host.spec $(BUILD_ROOT_DIR)/axcl_riscv_rpm/SPECS
	cp -rf $(HOME_PATH)/axcl/build/package/axcl.conf $(BUILD_ROOT_DIR)/axcl_riscv_rpm/SOURCES/axcl
	cp -rf $(HOME_PATH)/axcl/build/package/axcl_pcie.conf $(BUILD_ROOT_DIR)/axcl_riscv_rpm/SOURCES/axcl
	cp -rf $(HOME_PATH)/axcl/build/package/axcl_host.rules $(BUILD_ROOT_DIR)/axcl_riscv_rpm/SOURCES/axcl
	cp -rf $(HOME_PATH)/axcl/build/package/axcl.sh $(BUILD_ROOT_DIR)/axcl_riscv_rpm/SOURCES/axcl
	cp -rf $(HOME_PATH)/axcl/build/projects $(BUILD_ROOT_DIR)/axcl_riscv_rpm/SOURCES/axcl/build
	cp -rfL $(HOME_PATH)/axcl/build/*.mak $(BUILD_ROOT_DIR)/axcl_riscv_rpm/SOURCES/axcl/build
	cp -rf $(HOME_PATH)/axcl/build/Makefile $(BUILD_ROOT_DIR)/axcl_riscv_rpm/SOURCES/axcl/build
	cp -rfL $(HOME_PATH)/axcl/drv $(BUILD_ROOT_DIR)/axcl_riscv_rpm/SOURCES/axcl
	cp -rfL $(HOME_PATH)/axcl/out/axcl_linux_riscv/include $(BUILD_ROOT_DIR)/axcl_riscv_rpm/SOURCES/axcl
	cp -rf $(HOME_PATH)/axcl/out/axcl_linux_riscv/lib/*.so* $(BUILD_ROOT_DIR)/axcl_riscv_rpm/SOURCES/axcl/lib/
	cp -rf $(HOME_PATH)/axcl/out/axcl_linux_riscv/lib/ffmpeg $(BUILD_ROOT_DIR)/axcl_riscv_rpm/SOURCES/axcl/lib/
	cp -rf $(HOME_PATH)/axcl/out/axcl_linux_riscv/bin $(BUILD_ROOT_DIR)/axcl_riscv_rpm/SOURCES/axcl
	cp -rf $(HOME_PATH)/axcl/out/usrworker $(BUILD_ROOT_DIR)/axcl_riscv_rpm/SOURCES/axcl/bin/
	cp -rf $(HOME_PATH)/axcl/out/axcl_linux_riscv/json $(BUILD_ROOT_DIR)/axcl_riscv_rpm/SOURCES/axcl
	cp -rf $(HOME_PATH)/axcl/out/axcl_linux_riscv/data $(BUILD_ROOT_DIR)/axcl_riscv_rpm/SOURCES/axcl
endif

	@python3 $(COPY_FILE) $(PROJECT) $(PROJECT_MAK) $(SOC_BASE_DIR) $(SOC_FILELIST) $(DST_SOC_DIR)
ifeq ($(debug), yes)
	cp -rf $(HOME_PATH)/msp/out/lib/*.debug $(BUILD_ROOT_DIR)/rootfs/soc/lib
	cp -rf $(HOME_PATH)/kernel/osdrv/out/debug_ko/*.ko $(BUILD_ROOT_DIR)/rootfs/soc/ko
	for debug in `find $(BUILD_ROOT_DIR)/rootfs/soc/lib -name "*.debug"`;do mv $$debug `echo $$debug|sed 's/\.debug/\.so/'` ;done
endif
ifeq ($(asan), yes)
	cp -rf $(CROSS_LIB_PATH)/libasan*.so* $(BUILD_ROOT_DIR)/rootfs/soc/lib
endif
ifneq ($(CMM_POOL_PARAM),)
	sed -i "s/DEFAULT_CMM_POOL_CONFIG=.*/DEFAULT_CMM_POOL_CONFIG=$(CMM_POOL_PARAM)/g" $(BUILD_ROOT_DIR)/rootfs/soc/scripts/auto_load_all_drv.sh
endif
	@python3 $(COPY_FILE) $(PROJECT) $(PROJECT_MAK) $(OPT_BASE_DIR) $(OPT_FILELIST) $(DST_OPT_DIR)

# rootfs image
	$(HOME_PATH)/tools/mkext4fs/make_ext4fs -l 128M $(BUILD_PATH)/out/$(PROJECT)/images/rootfs.ext4 $(BUILD_ROOT_DIR)/rootfs
	$(HOME_PATH)/tools/mkext4fs/make_ext4fs -l 128M -s $(BUILD_PATH)/out/$(PROJECT)/images/rootfs_sparse.ext4 $(BUILD_ROOT_DIR)/rootfs

	@$(ECHO) -e $(GREEN) "make rootfs partiton" $(DONE)

install:
	@$(ECHO)

clean:
	@echo -e $(GREEN) "rootfs $@..." $(DONE)
	rm -rf $(BUILD_ROOT_DIR)/rootfs
	rm -rf $(BUILD_ROOT_DIR)/axcl_x86_deb
	rm -rf $(BUILD_ROOT_DIR)/axcl_x86_rpm
	rm -rf $(BUILD_ROOT_DIR)/axcl_arm64_deb
	rm -rf $(BUILD_ROOT_DIR)/axcl_arm64_rpm
	rm -rf $(BUILD_PATH)/out/$(PROJECT)/images/rootfs.ext4
	rm -rf $(BUILD_PATH)/out/$(PROJECT)/images/rootfs\_sparse.ext4