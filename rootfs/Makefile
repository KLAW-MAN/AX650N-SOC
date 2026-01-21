HOME_PATH    := $(abspath $(shell pwd)/..)
BUILD_PATH   := $(HOME_PATH)/build
BUSYBOX_PATH := $(shell pwd)/busybox-1.35.0

include $(BUILD_PATH)/color.mk
include $(BUILD_PATH)/config.mak
include $(HOME_PATH)/axcl/build/config.mak

OUT_DIR       := $(BUILD_PATH)/out/$(PROJECT)/objs
IMAGE_OUTDIR  := $(BUILD_PATH)/out/$(PROJECT)/images

.PHONY: all rootfs image
.NOTPARALLEL: clean install prepare

ORIG_ROOTFS := rootfs
UBUNTU_ROOTFS := ubuntu_rootfs
ifneq (,$(findstring $(PROJECT), "AX650_emmc" "AX650_pipro_box"))
UBUNTU_ROOTFS_DESKTOP := ubuntu_rootfs_desktop
endif

#OUT_ROOTFS := $(BUILD_PATH)/out/$(PROJECT)/rootfs
OUT_ROOTFS_DIR := $(BUILD_PATH)/out/$(PROJECT)
BUILD_ROOT := $(OUT_DIR)/rootfs
MKSWU_DIR  := $(HOME_PATH)/tools/mkswu
ROOTFS_LIST := $(ORIG_ROOTFS)
SWPUDATE_PATH    := $(HOME_PATH)/third-party/swupdate
BUILD_RECOVERY_ROOT := $(OUT_DIR)/rootfs_recovery
INSTALL_AXCL := no

ifeq ($(strip $(use_ubuntu_rootfs)),yes)
ROOTFS_LIST += $(UBUNTU_ROOTFS) $(UBUNTU_ROOTFS_DESKTOP)
ifeq ($(PROJECT), AX650_emmc)
INSTALL_AXCL := yes
endif
endif

ifeq ($(PROJECT), AX650_master)
INSTALL_AXCL := yes
endif

ifeq ($(asan), yes)
CROSS_PATH     := $(abspath $(dir $(shell whereis $(CC)|awk '{print $$2}'))/..)
CROSS_NAME     := $(shell echo $(CROSS) | sed -e 's/-$$//g')
CROSS_LIB_PATH := $(CROSS_PATH)/$(CROSS_NAME)/lib64
endif

SUPPORT_VALGRIND := no
ifeq ($(valgrind), yes)
VALGRIND_PATH := $(HOME_PATH)/tools/valgrind/bin/$(CHIP_NAME)/valgrind
endif
ifneq ($(wildcard $(VALGRIND_PATH)),)
SUPPORT_VALGRIND := yes
endif
ifeq ($(busybox), yes)
BUILD_BUSYBOX := TRUE
endif
ifneq ($(SLAVE_PROJECT),)
SLAVE_IMG_PATH_SRC := $(BUILD_PATH)/out/$(SLAVE_PROJECT)/images
SLAVE_IMG_PATH_DST := $(BUILD_ROOT)/opt/slave
SLAVE_IMAGE_LIST   := $(SLAVE_IMG_PATH_SRC)/u-boot.bin \
                      $(SLAVE_IMG_PATH_SRC)/AX650_slave.dtb \
                      $(SLAVE_IMG_PATH_SRC)/atf_bl31.img \
                      $(SLAVE_IMG_PATH_SRC)/Image \
                      $(SLAVE_IMG_PATH_SRC)/rootfs.ext4 \
		      $(SLAVE_IMG_PATH_SRC)/spl_AX650_slave_signed.bin
endif

CARD_PAC_PATH     := $(BUILD_PATH)/out/AX650_card/ax650_card.pac

ROOTFS_PART_SIZE ?= 4736M
SOC_PART_SIZE ?= 384M
OPT_PART_SIZE ?= 1900M
FSTAB_NAME := fstab
AB_MOUNT_NAME := ab_mount
SWUPDATE_NAME := swupdate
HW_VERSION ?= 1.0
ifeq ($(PROJECT), AX650_ssd)
	FSTAB_NAME := $(FSTAB_NAME)_ssd
	SWUPDATE_NAME := $(SWUPDATE_NAME)_ssd
endif
ifneq ($(strip $(SUPPORT_OPTEE)),false)
	FSTAB_NAME := $(FSTAB_NAME)_optee
	AB_MOUNT_NAME := $(AB_MOUNT_NAME)_optee
	SWUPDATE_NAME := $(SWUPDATE_NAME)_optee
endif
ifeq ($(strip $(SUPPORT_AB_PART)),TRUE)
	FSTAB_NAME := $(FSTAB_NAME)_AB
	SWUPDATE_NAME := $(SWUPDATE_NAME)_AB
endif

default: rootfs
all: prepare rootfs
	@$(ECHO) -e $(GREEN)"\nBuild Rootfs success!!\n" $(DONE)

prepare:
	@$(ECHO) -e $(GREEN) "Making $@..." $(DONE)
	@$(MKDIR) -p $(OUT_DIR)
	@$(MKDIR) -p $(BUILD_ROOT)
	@$(CP) -avf rootfs/* $(BUILD_ROOT)
ifeq ($(BUILD_BUSYBOX),TRUE)
rootfs: busybox

else

rootfs:
endif
	@$(ECHO) -e $(GREEN) "Making $@..." $(DONE)
	@$(MKDIR) -p $(BUILD_ROOT)/proc
	@$(MKDIR) -p $(BUILD_ROOT)/sys
	@$(MKDIR) -p $(BUILD_ROOT)/mnt
	@$(MKDIR) -p $(BUILD_ROOT)/sys
	@$(MKDIR) -p $(BUILD_ROOT)/tmp
	@$(MKDIR) -p $(BUILD_ROOT)/run
	@$(MKDIR) -p $(BUILD_ROOT)/root
	@$(MKDIR) -p $(BUILD_ROOT)/media
	@$(MKDIR) -p $(BUILD_ROOT)/param
	@$(MKDIR) -p $(BUILD_ROOT)/soc
	@$(MKDIR) $(BUILD_ROOT)/var/www

	@$(MKDIR) $(BUILD_ROOT)/opt/bin
	@$(MKDIR) $(BUILD_ROOT)/opt/bin/FRTDemo
	@$(MKDIR) $(BUILD_ROOT)/opt/bin/BoxDemo
	@$(MKDIR) $(BUILD_ROOT)/opt/data
	@$(MKDIR) $(BUILD_ROOT)/opt/etc
	@$(MKDIR) $(BUILD_ROOT)/opt/scripts
	@$(MKDIR) $(IMAGE_OUTDIR)
	cd $(BUILD_ROOT) && find . | cpio -o -H newc > $(IMAGE_OUTDIR)/rootfs.cpio
ifeq ($(SUPPORT_RECOVERY), TRUE)
	@$(MKDIR) -p $(BUILD_RECOVERY_ROOT)
	@$(CP) -avf $(HOME_PATH)/rootfs/rootfs/* $(BUILD_RECOVERY_ROOT)
	@$(MKDIR) -p $(BUILD_RECOVERY_ROOT)/opt/swupdate/bin
	$(CP) -r $(SWPUDATE_PATH)/bin/swupdate  $(BUILD_RECOVERY_ROOT)/opt/swupdate/bin
	$(CP) -r $(SWPUDATE_PATH)/etc  $(BUILD_RECOVERY_ROOT)/opt/swupdate
	$(CP) -r $(SWPUDATE_PATH)/lib  $(BUILD_RECOVERY_ROOT)/opt/swupdate
	$(CP) -r $(SWPUDATE_PATH)/v2  $(BUILD_RECOVERY_ROOT)/opt/swupdate
	$(CP) -r $(BUILD_PATH)/projects/$(PROJECT)/rootfs/usr/bin/bootable-set.sh  $(BUILD_RECOVERY_ROOT)/usr/bin
	$(CP) -r $(BUILD_PATH)/projects/$(PROJECT)/recovery/rootfs/etc/fstab  $(BUILD_RECOVERY_ROOT)/etc
	$(CP) -r $(BUILD_PATH)/projects/$(PROJECT)/recovery/rootfs/etc/init.d/rcS  $(BUILD_RECOVERY_ROOT)/etc/init.d
	$(CP) -r $(BUILD_PATH)/projects/$(PROJECT)/recovery/mkswu/swupdate.sh  $(BUILD_RECOVERY_ROOT)/opt/swupdate/bin
	@openssl rsa -in $(BUILD_PATH)/projects/$(PROJECT)/recovery/mkswu/priv.pem -pubout > $(BUILD_RECOVERY_ROOT)/opt/swupdate/public.pem
	@cd $(BUILD_RECOVERY_ROOT) && find . | cpio -o -H newc | lzma > $(IMAGE_OUTDIR)/rootfs_recovery.cpio.lzma
endif

install:
	@$(ECHO) -e $(GREEN) "rootfs $@..." $(DONE)
ifeq ($(BUILD_BUSYBOX),TRUE)
	@$(CP) -avf $(BUSYBOX_PATH)/_install/* ./rootfs/
endif

image:
	@$(ECHO) -e $(GREEN) "rootfs $@..." $(DONE)
	@$(MKDIR) $(BUILD_PATH)/out/$(PROJECT)/
	cp -rfp rootfs $(OUT_ROOTFS_DIR)/
ifneq (,$(UBUNTU_ROOTFS_DESKTOP))
	tar -zxpf ubuntu_rootfs_desktop.tar.gz  -C $(OUT_ROOTFS_DIR)/
endif
	tar -zxpf ubuntu_rootfs.tar.gz -C $(OUT_ROOTFS_DIR)/
	@for ROOTFS_TARGET in $(ROOTFS_LIST); do \
	cp -avf rootfs/usr/local/lib/* $(OUT_ROOTFS_DIR)/$$ROOTFS_TARGET/usr/local/lib; \
	cp -avf rootfs/usr/local/bin/* $(OUT_ROOTFS_DIR)/$$ROOTFS_TARGET/usr/local/bin; \
	rm -rf $(OUT_ROOTFS_DIR)/$$ROOTFS_TARGET/opt/*; \
	rm -rf $(OUT_ROOTFS_DIR)/$$ROOTFS_TARGET/soc/*; \
	rm -rf $(OUT_ROOTFS_DIR)/$$ROOTFS_TARGET/param/*; \
	echo "$(PROJECT) $(HW_VERSION)" > $(OUT_ROOTFS_DIR)/$$ROOTFS_TARGET/etc/hwrevision; \
	done;
ifeq ($(docker), yes)
ifeq ($(PROJECT), AX650_emmc)
	@bash docker/docker_pack.sh $(shell pwd)/docker $(OUT_ROOTFS_DIR)/$(ORIG_ROOTFS)
endif
endif
ifeq ($(SUPPORT_VALGRIND), yes)
	@for ROOTFS_TARGET in $(ROOTFS_LIST); do \
	sed -i '/VALGRIND_LIB/{d}' $(OUT_ROOTFS_DIR)/$$ROOTFS_TARGET/etc/profile; \
	sed -i '$$a\export VALGRIND_LIB=/opt/libexec/valgrind' $(OUT_ROOTFS_DIR)/$$ROOTFS_TARGET/etc/profile; \
	done;
endif
ifneq ($(strip $(SUPPORT_OPTEE)),false)
	@for ROOTFS_TARGET in $(ROOTFS_LIST); do \
	$(MKDIR) -p $(OUT_ROOTFS_DIR)/$$ROOTFS_TARGET/lib/optee_armtz; \
	cp -rf $(HOME_PATH)/msp/out/ta/* $(OUT_ROOTFS_DIR)/$$ROOTFS_TARGET/lib/optee_armtz; \
	rm -f $(OUT_ROOTFS_DIR)/$$ROOTFS_TARGET/etc/fstab; \
	cp -rf $(OUT_ROOTFS_DIR)/$$ROOTFS_TARGET/etc/fstab_optee $(OUT_ROOTFS_DIR)/$$ROOTFS_TARGET/etc/fstab; \
	done;
else
	@for ROOTFS_TARGET in $(ROOTFS_LIST); do \
	rm -f $(OUT_ROOTFS_DIR)/$$ROOTFS_TARGET/etc/fstab_optee; \
	done;
endif
ifeq ($(PROJECT), AX650_ssd)
ifneq ($(strip $(SUPPORT_OPTEE)),false)
	@for ROOTFS_TARGET in $(ROOTFS_LIST); do \
	rm -f $(OUT_ROOTFS_DIR)/$$ROOTFS_TARGET/etc/fw_env.config; \
	cp -rf $(OUT_ROOTFS_DIR)/$$ROOTFS_TARGET/etc/fw_env_ssd.config $(OUT_ROOTFS_DIR)/$$ROOTFS_TARGET/etc/fw_env.config; \
	rm -f $(OUT_ROOTFS_DIR)/$$ROOTFS_TARGET/etc/fstab; \
	cp -rf $(OUT_ROOTFS_DIR)/$$ROOTFS_TARGET/etc/fstab_ssd_optee $(OUT_ROOTFS_DIR)/$$ROOTFS_TARGET/etc/fstab; \
	done;
else
	@for ROOTFS_TARGET in $(ROOTFS_LIST); do \
	rm -f $(OUT_ROOTFS_DIR)/$$ROOTFS_TARGET/etc/fw_env.config; \
	cp -rf $(OUT_ROOTFS_DIR)/$$ROOTFS_TARGET/etc/fw_env_ssd.config $(OUT_ROOTFS_DIR)/$$ROOTFS_TARGET/etc/fw_env.config; \
	rm -f $(OUT_ROOTFS_DIR)/$$ROOTFS_TARGET/etc/fstab; \
	cp -rf $(OUT_ROOTFS_DIR)/$$ROOTFS_TARGET/etc/fstab_ssd $(OUT_ROOTFS_DIR)/$$ROOTFS_TARGET/etc/fstab; \
	done;
endif
endif

ifeq ($(strip $(SUPPORT_AB_PART)),TRUE)
	@cp -rf automotive/etc/* $(OUT_ROOTFS_DIR)/rootfs/etc
	@cp -rf $(OUT_ROOTFS_DIR)/rootfs/etc/$(FSTAB_NAME) $(OUT_ROOTFS_DIR)/rootfs/etc/fstab
	@cp -rf $(OUT_ROOTFS_DIR)/rootfs/etc/init.d/$(AB_MOUNT_NAME) $(OUT_ROOTFS_DIR)/rootfs/etc/init.d/ab_mount
	@rm $(OUT_ROOTFS_DIR)/rootfs/etc/init.d/ab_mount_*
endif
ifeq ($(SUPPORT_RECOVERY), TRUE)
	$(CP) -r $(BUILD_PATH)/projects/$(PROJECT)/rootfs/usr/bin/bootable-set.sh  $(OUT_ROOTFS_DIR)/rootfs/usr/bin
	@for ROOTFS_TARGET in $(ROOTFS_LIST); do \
		$(CP) -r $(BUILD_PATH)/projects/$(PROJECT)/rootfs/etc/fstab  $(OUT_ROOTFS_DIR)/$$ROOTFS_TARGET/etc; \
	done;
	$(CP) -r $(BUILD_PATH)/projects/$(PROJECT)/rootfs/etc/init.d/rcS  $(OUT_ROOTFS_DIR)/rootfs/etc/init.d
	$(CP) -r $(BUILD_PATH)/projects/$(PROJECT)/rootfs/etc/init.d/S90checkrecovery  $(OUT_ROOTFS_DIR)/rootfs/etc/init.d
endif
ifeq ($(strip $(ENABLE_SWUPDATE)),TRUE)
	@cp -rf automotive/etc/init.d/S99checkota $(OUT_ROOTFS_DIR)/rootfs/etc/init.d
	@cp -rf rootfs/opt/swupdate $(BUILD_ROOT)/opt
	@cp -rf $(MKSWU_DIR)/$(SWUPDATE_NAME).sh $(BUILD_ROOT)/opt/swupdate/swupdate.sh
	@cp -rf $(MKSWU_DIR)/$(SWUPDATE_NAME)_ota.sh $(IMAGE_OUTDIR)/ota.sh
	@cp -rf $(MKSWU_DIR)/$(SWUPDATE_NAME)_description $(IMAGE_OUTDIR)/sw-description-source
	@cp -rf $(MKSWU_DIR)/$(SWUPDATE_NAME)_priv.pem $(IMAGE_OUTDIR)/priv.pem
	@openssl rsa -in $(MKSWU_DIR)/$(SWUPDATE_NAME)_priv.pem -pubout > $(BUILD_ROOT)/opt/swupdate/public.pem
endif
	@for ROOTFS_TARGET in $(ROOTFS_LIST); do \
	rm $(OUT_ROOTFS_DIR)/$$ROOTFS_TARGET/etc/fstab_*; \
	done;
ifneq ($(findstring yes, $(debugkconfig) $(debug)),)
	@for ROOTFS_TARGET in $(ROOTFS_LIST); do \
		if [ "$$ROOTFS_TARGET" == "rootfs" ]; then \
			echo "chmod 755 /soc/scripts/kmemleak_scan.sh" >> $(OUT_ROOTFS_DIR)/rootfs/etc/init.d/rcS; \
			echo "/soc/scripts/kmemleak_scan.sh &" >> $(OUT_ROOTFS_DIR)/rootfs/etc/init.d/rcS; \
		elif [[ "$$ROOTFS_TARGET" == "ubuntu"* ]]; then \
			echo "chmod 755 /soc/scripts/kmemleak_scan.sh" >> $(OUT_ROOTFS_DIR)/$$ROOTFS_TARGET/etc/rc.local; \
			echo "/soc/scripts/kmemleak_scan.sh &" >> $(OUT_ROOTFS_DIR)/$$ROOTFS_TARGET/etc/rc.local; \
		fi \
	done;
endif

ifeq ($(PROJECT), AX650_master)
ifneq ($(wildcard $(CARD_PAC_PATH)),)
	@for ROOTFS_TARGET in $(ROOTFS_LIST); do \
	mkdir -p $(OUT_ROOTFS_DIR)/$$ROOTFS_TARGET/lib/firmware/axcl; \
	cp -f $(CARD_PAC_PATH) $(OUT_ROOTFS_DIR)/$$ROOTFS_TARGET/lib/firmware/axcl/; \
	done;
endif
endif

ifeq ($(PROJECT), AX650_xpilot_6v)
	@for ROOTFS_TARGET in $(ROOTFS_LIST); do \
	echo "chmod 755 /soc/scripts/xpilot_6v_pmic_set.sh" >> $(OUT_ROOTFS_DIR)/$$ROOTFS_TARGET/etc/init.d/rcS; \
	echo "/soc/scripts/xpilot_6v_pmic_set.sh &" >> $(OUT_ROOTFS_DIR)/$$ROOTFS_TARGET/etc/init.d/rcS; \
	done;
	$(HOME_PATH)/tools/mkext4fs/make_ext4fs -l $(ROOTFS_PART_SIZE) $(BUILD_PATH)/out/$(PROJECT)/images/rootfs.ext4 $(OUT_ROOTFS_DIR)/rootfs
	$(HOME_PATH)/tools/mkext4fs/make_ext4fs -l $(ROOTFS_PART_SIZE) -s $(BUILD_PATH)/out/$(PROJECT)/images/rootfs_sparse.ext4 $(OUT_ROOTFS_DIR)/rootfs
else
	@for ROOTFS_TARGET in $(ROOTFS_LIST); do \
	$(HOME_PATH)/tools/mkext4fs/make_ext4fs -l $(ROOTFS_PART_SIZE) $(BUILD_PATH)/out/$(PROJECT)/images/$$ROOTFS_TARGET.ext4 $(OUT_ROOTFS_DIR)/$$ROOTFS_TARGET; \
	$(HOME_PATH)/tools/mkext4fs/make_ext4fs -l $(ROOTFS_PART_SIZE) -s $(BUILD_PATH)/out/$(PROJECT)/images/$$ROOTFS_TARGET\_sparse.ext4 $(OUT_ROOTFS_DIR)/$$ROOTFS_TARGET; \
	done;
endif
	@cp -rf ubi/fstab $(BUILD_PATH)/out/$(PROJECT)/rootfs/etc/
	@cp -rf ubi/fw_env.config $(BUILD_PATH)/out/$(PROJECT)/rootfs/etc/
ifeq ($(SUPPORT_UBIFS), TRUE)
	@rm -rf $(BUILD_PATH)/out/$(PROJECT)/rootfs/usr/local/lib/libfdk-aac.so
	@rm -rf $(BUILD_PATH)/out/$(PROJECT)/rootfs/usr/local/lib/libfdk-aac.so.2
	@rm -rf $(BUILD_PATH)/out/$(PROJECT)/rootfs/usr/local/lib/libfdk-aac.so.2.0.1
	@$(HOME_PATH)/tools/mkubifs/mkfs.ubifs -F -q -r $(BUILD_PATH)/out/$(PROJECT)/rootfs -m 2048 -e 126976 -c 2047 -o rootfs.ubifs
endif

	@for ROOTFS_TARGET in $(ROOTFS_LIST); do \
	rm -rf $(OUT_ROOTFS_DIR)/$$ROOTFS_TARGET; \
	done;

	@$(ECHO) -e $(GREEN) "make param partiton" $(DONE)
	@cp -rf $(HOME_PATH)/msp/out/bin/FRTDemo/res/avs $(BUILD_ROOT)/param
	@$(HOME_PATH)/tools/mkext4fs/make_ext4fs -l 20M $(BUILD_PATH)/out/$(PROJECT)/images/param.ext4 $(BUILD_ROOT)/param
	@$(HOME_PATH)/tools/mkext4fs/make_ext4fs -l 20M -s $(BUILD_PATH)/out/$(PROJECT)/images/param_sparse.ext4 $(BUILD_ROOT)/param
	@$(ECHO) -e $(GREEN) "make soc partiton" $(DONE)
	@$(MKDIR) -p $(BUILD_ROOT)/soc/lib
	@$(MKDIR) -p $(BUILD_ROOT)/soc/ko
	@$(MKDIR) -p $(BUILD_ROOT)/opt/bin
	@$(MKDIR) -p $(BUILD_ROOT)/opt/data
	@$(MKDIR) -p $(BUILD_ROOT)/opt/etc
	@$(MKDIR) -p $(BUILD_ROOT)/opt/lib
ifeq ($(debug), yes)
	@cp -rf $(HOME_PATH)/msp/out/lib/*.debug $(BUILD_ROOT)/soc/lib
	@for debug in `find $(BUILD_ROOT)/soc/lib -name "*.debug"`;do mv $$debug `echo $$debug|sed 's/\.debug/\.so/'` ;done
	@cp -rf $(HOME_PATH)/kernel/osdrv/out/debug_ko/*.ko $(BUILD_ROOT)/soc/ko
else
	@cp -rf $(HOME_PATH)/msp/out/lib/*.so* $(BUILD_ROOT)/soc/lib
	@cp -rf $(HOME_PATH)/kernel/osdrv/out/ko/*.ko $(BUILD_ROOT)/soc/ko
endif
ifeq ($(asan), yes)
	@cp -rf $(CROSS_LIB_PATH)/libasan*.so* $(BUILD_ROOT)/soc/lib
endif
	@cp -rf $(HOME_PATH)/msp/out/bin/* $(BUILD_ROOT)/opt/bin
	@rm -rf $(BUILD_ROOT)/opt/bin/FRTDemo/res/avs
	@cp -rf $(HOME_PATH)/msp/out/data/* $(BUILD_ROOT)/opt/data
	@cp -rf $(HOME_PATH)/msp/out/etc/* $(BUILD_ROOT)/opt/etc

ifeq ($(INSTALL_AXCL), yes)

# cp axcl so and bin
ifeq ($(debug), yes)
	@cp -rf $(AXCL_LIB_PATH)/libaxcl_*.debug $(BUILD_ROOT)/opt/lib
	@for debug in `find $(BUILD_ROOT)/opt/lib -name "*.debug"`;do mv $$debug `echo $$debug|sed 's/\.debug/\.so/'` ;done
else
	@cp -rf $(AXCL_LIB_PATH)/libaxcl_*.so* $(BUILD_ROOT)/opt/lib/
endif
	@$(MKDIR) -p $(BUILD_ROOT)/opt/bin/axcl
	@$(MKDIR) -p $(BUILD_ROOT)/opt/etc/axcl
	@cp -rf $(AXCL_BIN_PATH)/* $(BUILD_ROOT)/opt/bin/axcl/
	@cp -rf $(AXCL_ETC_PATH)/* $(BUILD_ROOT)/opt/etc/axcl/
	@cp -rf $(AXCL_JSON_PATH)/* $(BUILD_ROOT)/opt/bin/axcl/

	@cp -rf $(AXCL_HOME_PATH)/out/usrworker $(BUILD_ROOT)/opt/bin/axcl/

	@cp -rfd $(AXCL_LIB_PATH)/ffmpeg $(BUILD_ROOT)/opt/lib/
	@cp -rfd $(AXCL_BIN_PATH)/ffmpeg $(BUILD_ROOT)/opt/bin/axcl/

	@rm -rf $(BUILD_ROOT)/opt/bin/axclSlave/
	@rm -rf $(BUILD_ROOT)/soc/lib/libax_pkg.so
	@rm -rf $(BUILD_ROOT)/soc/lib/libax_comm.so
endif
	@cp -rf $(HOME_PATH)/msp/out/lib/qt-5.15.7 $(BUILD_ROOT)/opt/lib/
	@cp -rf rootfs/opt/scripts $(BUILD_ROOT)/soc
ifeq ($(PROJECT), AX650_lp)
	@rm -rf $(BUILD_ROOT)/soc/scripts/auto_load_all_drv.sh
	@mv $(BUILD_ROOT)/soc/scripts/auto_load_all_drv_lp.sh $(BUILD_ROOT)/soc/scripts/auto_load_all_drv.sh
endif
ifneq ($(CMM_POOL_PARAM),)
	@sed -i "s/cmmpool*=.*/cmmpool=$(CMM_POOL_PARAM)/g" $(BUILD_ROOT)/soc/scripts/auto_load_all_drv.sh
endif
ifneq ($(DSP_PARAM),)
	@sed -i "s/dsp0_ddr*=.*/$(DSP_PARAM)/g" $(BUILD_ROOT)/soc/scripts/auto_load_all_drv.sh
endif
ifeq ($(docker), yes)
ifeq ($(PROJECT), AX650_emmc)
	@cp -rf docker $(BUILD_ROOT)/opt
	@rm -rf $(BUILD_ROOT)/opt/docker/etc
	@rm -rf $(BUILD_ROOT)/opt/docker/docker_pack.sh
endif
endif
	cd $(BUILD_ROOT) && find . | cpio -o -H newc > $(IMAGE_OUTDIR)/rootfs.cpio
	@rm -rf $(BUILD_ROOT)/opt/scripts
	@$(HOME_PATH)/tools/mkext4fs/make_ext4fs -l $(SOC_PART_SIZE) $(BUILD_PATH)/out/$(PROJECT)/images/soc.ext4 $(BUILD_ROOT)/soc
	@$(HOME_PATH)/tools/mkext4fs/make_ext4fs -l $(SOC_PART_SIZE) -s $(BUILD_PATH)/out/$(PROJECT)/images/soc_sparse.ext4 $(BUILD_ROOT)/soc
ifeq ($(SUPPORT_UBIFS), TRUE)
	@$(HOME_PATH)/tools/mkubifs/mkfs.ubifs –F -q -r $(BUILD_ROOT)/soc -m 2048 -e 126976 -c 2047 -o soc.ubifs
endif

	@rm -rf $(BUILD_PATH)/out/$(PROJECT)/soc

ifeq ($(CHECK_SLAVE), TRUE)
ifneq ($(SLAVE_PROJECT),)
	mkdir -p $(SLAVE_IMG_PATH_DST)
	@for file in $(SLAVE_IMAGE_LIST); do cp $$file $(SLAVE_IMG_PATH_DST); done
	@mv $(BUILD_ROOT)/soc/scripts/axdl $(BUILD_ROOT)/opt/bin
endif
else
	@rm -rf $(BUILD_ROOT)/soc/scripts/axdl
endif

ifeq ($(SUPPORT_VALGRIND), yes)
	@cp -rf $(VALGRIND_PATH)/bin/* $(BUILD_ROOT)/opt/bin
	@cp -rf $(VALGRIND_PATH)/libexec $(BUILD_ROOT)/opt/
endif
	@$(ECHO) -e $(GREEN) "make opt ext4 partiton" $(DONE)
	@$(HOME_PATH)/tools/mkext4fs/make_ext4fs -l $(OPT_PART_SIZE) $(BUILD_PATH)/out/$(PROJECT)/images/opt.ext4 $(BUILD_ROOT)/opt
	@$(HOME_PATH)/tools/mkext4fs/make_ext4fs -l $(OPT_PART_SIZE) -s $(BUILD_PATH)/out/$(PROJECT)/images/opt_sparse.ext4 $(BUILD_ROOT)/opt
ifneq ($(SLAVE_PROJECT),)
	@for ROOTFS_TARGET in $(ROOTFS_LIST); do \
		if [ "$$ROOTFS_TARGET" != "rootfs" ]; then \
			cp $(SLAVE_IMG_PATH_SRC)/$$ROOTFS_TARGET.ext4 $(SLAVE_IMG_PATH_DST); \
			rm $(SLAVE_IMG_PATH_DST)/rootfs.ext4; \
			mv $(SLAVE_IMG_PATH_DST)/$$ROOTFS_TARGET.ext4 $(SLAVE_IMG_PATH_DST)/rootfs.ext4; \
			$(HOME_PATH)/tools/mkext4fs/make_ext4fs -l $(OPT_PART_SIZE) $(BUILD_PATH)/out/$(PROJECT)/images/opt_$$ROOTFS_TARGET.ext4 $(BUILD_ROOT)/opt; \
			$(HOME_PATH)/tools/mkext4fs/make_ext4fs -l $(OPT_PART_SIZE) -s $(BUILD_PATH)/out/$(PROJECT)/images/opt_$$ROOTFS_TARGET\_sparse.ext4 $(BUILD_ROOT)/opt; \
		fi \
	done;
endif

ifeq ($(SUPPORT_UBIFS), TRUE)
	@$(ECHO) -e $(GREEN) "cut opt ubifs partiton" $(DONE)
	@rm -rf $(BUILD_ROOT)/opt/etc/*
	@rm -rf $(BUILD_ROOT)/opt/lib/qt-5.15.7
	@rm -rf $(BUILD_ROOT)/opt/bin
	@rm -rf $(BUILD_ROOT)/opt/data/dsp
	@rm -rf $(BUILD_ROOT)/opt/data/npu
	@rm -rf $(BUILD_ROOT)/opt/data/ivps
	@find $(BUILD_ROOT)/opt/data/ive/ -type f ! -name "*.json" -exec rm -rf {} \;
	@$(HOME_PATH)/tools/mkubifs/mkfs.ubifs –F -q -r $(BUILD_ROOT)/opt -m 2048 -e 126976 -c 2047 -o opt.ubifs
	@$(HOME_PATH)/tools/mkubifs/ubinize -o $(BUILD_PATH)/out/$(PROJECT)/images/rootfs_soc_opt.ubi -m 2048 -p 128KiB ubi/ubinize.cfg
endif

busybox:
	@$(CP) -rf $(BUSYBOX_PATH)/configs/AX650_defconfig $(BUSYBOX_PATH)/.config
	@$(MAKE) -C $(BUSYBOX_PATH) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS)
	@$(MAKE) -C $(BUSYBOX_PATH) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS) install

bbxconfig:
	@$(CP) -rf $(BUSYBOX_PATH)/configs/AX650_defconfig $(BUSYBOX_PATH)/.config
	@$(MAKE) -C $(BUSYBOX_PATH) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS) menuconfig

clean:
	@$(ECHO) -e $(GREEN) "rootfs $@..." $(DONE)
	@rm -rf $(BUILD_ROOT)/opt/*
ifeq ($(BUILD_BUSYBOX),TRUE)
	@$(MAKE) -C $(BUSYBOX_PATH) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS) clean
endif
