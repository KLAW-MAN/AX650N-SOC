HOME_PATH  := $(abspath $(shell pwd)/../..)
BUILD_PATH := $(HOME_PATH)/build
TOOLS_PATH := $(HOME_PATH)/tools
OSDRV_OUT_PATCH := $(HOME_PATH)/kernel/osdrv/out
AES_KEY := aes-256.key

include $(BUILD_PATH)/color.mk
include $(BUILD_PATH)/config.mak

.PHONY: all prepare linux
.NOTPARALLEL: clean install prepare

KERNEL_DIR  ?=  linux-5.15.73
ifeq ($(PROJECT), AX650_card)
CONFIG ?= axera_AX650A_card_defconfig
endif
ifeq ($(PROJECT), AX650_lp)
CONFIG ?= axera_AX650A_lp_defconfig
endif
ifeq ($(PROJECT), AX650_pipro_box)
CONFIG ?= axera_AX650A_pipro_box_defconfig
endif
ifeq ($(PROJECT), AX650_ssd)
CONFIG ?= axera_AX650A_ssd_defconfig
endif
ifeq ($(PROJECT), AX650_emmc)
CONFIG ?= axera_AX650A_emmc_defconfig
ifeq ($(SUPPORT_RECOVERY), TRUE)
CONFIG_RECOVERY ?= axera_AX650A_emmc_recovery_defconfig
endif
endif
ifeq ($(PROJECT), AX650_xpilot_6v)
CONFIG ?= axera_AX650A_xpilot_6v_defconfig
endif
ifeq ($(PROJECT), AX650_nand)
CONFIG ?= axera_AX650A_nand_defconfig
endif
ifeq ($(PROJECT), AX650_slave)
CONFIG ?= axera_AX650A_slave_defconfig
endif
ifeq ($(PROJECT), AX650_master)
CONFIG ?= axera_AX650A_master_defconfig
endif
AXERA_SCRIPT_PATH    := $(KERNEL_DIR)/scripts/axera
PATCH_KCONFIG_SCRIPT := $(AXERA_SCRIPT_PATH)/patch_defconfig.py
DEF_PATCH_FILES      :=

ifeq ($(debug), yes)
DEF_PATCH_FILES      += $(AXERA_SCRIPT_PATH)/AX650_kmemleak_defconfig
endif
ifeq ($(debugkconfig), yes)
DEF_PATCH_FILES      += $(AXERA_SCRIPT_PATH)/AX650_debug_defconfig
endif
ifneq (,$(findstring $(PROJECT), "AX650_emmc" "AX650_master" "AX650_slave" "AX650_ssd" "AX650_pipro_box" "AX650_lp"))
DEF_PATCH_FILES     += $(AXERA_SCRIPT_PATH)/AX650_docker_defconfig
endif

DTS_OPTEE_FILE   := AX650_optee_define.h
DTS_OPTEE_FILE_PATH := $(KERNEL_DIR)/arch/arm64/boot/dts/axera
ifneq (,$(findstring $(PROJECT), "AX650_emmc" "AX650_master" "AX650_xpilot_6v" "AX650_ssd" "AX650_pipro_box" "AX650_lp"))
ifneq ($(strip $(SUPPORT_OPTEE)),false)
DEF_PATCH_FILES      += $(AXERA_SCRIPT_PATH)/AX650_optee_defconfig

OPTEE_RES_START := $(OPTEE_IMAGE_ADDR)
OPTEE_RES_SIZE  := $(OPTEE_RESERVED_SIZE)
PATCH_OPTEE_SCRIPT := $(AXERA_SCRIPT_PATH)/patch_optee_dts.sh
endif
endif

#UIO_DRIVER_PATH = $(HOME_PATH)/kernel/linux/$(KERNEL_DIR)/drivers/uio
KBUILD_OUTDIR   := $(BUILD_PATH)/out/$(PROJECT)/objs/kernel/linux/$(KERNEL_DIR)
IMAGE_OUTDIR    := $(BUILD_PATH)/out/$(PROJECT)/images
EXT_FLAG        := O=$(KBUILD_OUTDIR)  HOME_PATH=$(HOME_PATH)
KBUILD_RECOVERY_OUTDIR   := $(BUILD_PATH)/out/$(PROJECT)/objs/kernel/linux/recovery/$(KERNEL_DIR)
EXT_RECOVERY_FLAG        := O=$(KBUILD_RECOVERY_OUTDIR)  HOME_PATH=$(HOME_PATH)

default: linux

all: prepare linux
	@$(ECHO) -e $(GREEN)"\nBuild Linux success!!\n" $(DONE)

prepare:
	@$(ECHO)
	@$(ECHO) -e $(GREEN) "first build prepare" $(DONE)
	@$(MKDIR) -p $(KBUILD_OUTDIR)
ifneq ($(DEF_PATCH_FILES),)
	@$(CP) $(KERNEL_DIR)/arch/$(ARCH)/configs/$(CONFIG) $(AXERA_SCRIPT_PATH)/$(CONFIG)_bak
	python3 $(PATCH_KCONFIG_SCRIPT) $(DEF_PATCH_FILES) $(KERNEL_DIR)/arch/$(ARCH)/configs/$(CONFIG)
endif
	@$(MAKE) -C $(KERNEL_DIR)/ ARCH=$(ARCH) CROSS_COMPILE=$(CROSS) $(CONFIG) $(EXT_FLAG)
ifneq ($(DEF_PATCH_FILES),)
	@$(CP) $(AXERA_SCRIPT_PATH)/$(CONFIG)_bak $(KERNEL_DIR)/arch/$(ARCH)/configs/$(CONFIG)
	@$(RM) $(AXERA_SCRIPT_PATH)/$(CONFIG)_bak
endif
ifeq ($(SUPPORT_RECOVERY), TRUE)
	@$(ECHO) -e $(GREEN) "first build recovery prepare" $(DONE)
	@$(MKDIR) -p $(KBUILD_RECOVERY_OUTDIR)
ifneq ($(DEF_PATCH_FILES),)
	@$(CP) $(KERNEL_DIR)/arch/$(ARCH)/configs/$(CONFIG_RECOVERY) $(AXERA_SCRIPT_PATH)/$(CONFIG_RECOVERY)_bak
	python3 $(PATCH_KCONFIG_SCRIPT) $(DEF_PATCH_FILES) $(KERNEL_DIR)/arch/$(ARCH)/configs/$(CONFIG_RECOVERY)
endif
	@$(MAKE) -C $(KERNEL_DIR)/ ARCH=$(ARCH) CROSS_COMPILE=$(CROSS) $(CONFIG_RECOVERY) $(EXT_RECOVERY_FLAG)
ifneq ($(DEF_PATCH_FILES),)
	@$(CP) $(AXERA_SCRIPT_PATH)/$(CONFIG_RECOVERY)_bak $(KERNEL_DIR)/arch/$(ARCH)/configs/$(CONFIG_RECOVERY)
	@$(RM) $(AXERA_SCRIPT_PATH)/$(CONFIG_RECOVERY)_bak
endif
endif
ifneq ("$(wildcard $(DTS_OPTEE_FILE_PATH)/$(DTS_OPTEE_FILE)_bak)","")
	@$(CP) $(DTS_OPTEE_FILE_PATH)/$(DTS_OPTEE_FILE)_bak $(DTS_OPTEE_FILE_PATH)/$(DTS_OPTEE_FILE)
	@$(RM) $(DTS_OPTEE_FILE_PATH)/$(DTS_OPTEE_FILE)_bak
endif

linux:
	@$(ECHO)
	@$(ECHO) -e $(GREEN) "Making $@..." $(DONE)
	@$(MAKE) -C $(KERNEL_DIR)/ ARCH=$(ARCH) CROSS_COMPILE=$(CROSS) Image $(EXT_FLAG)
ifeq ($(SUPPORT_RECOVERY), TRUE)
	@$(ECHO) -e $(GREEN) "Making Recovery..." $(DONE)
	@$(MAKE) -C $(KERNEL_DIR)/ ARCH=$(ARCH) CROSS_COMPILE=$(CROSS) Image $(EXT_RECOVERY_FLAG)
endif
	@$(MAKE) -C $(KERNEL_DIR)/ ARCH=$(ARCH) CROSS_COMPILE=$(CROSS) M=$(UIO_DRIVER_PATH) modules  $(EXT_FLAG)
ifneq (,$(findstring $(PROJECT), "AX650_emmc" "AX650_master" "AX650_xpilot_6v" "AX650_ssd" "AX650_pipro_box" "AX650_lp"))
ifneq ($(strip $(SUPPORT_OPTEE)),false)
	@$(CP) $(DTS_OPTEE_FILE_PATH)/$(DTS_OPTEE_FILE)	$(DTS_OPTEE_FILE_PATH)/$(DTS_OPTEE_FILE)_bak
	@bash $(PATCH_OPTEE_SCRIPT) $(DTS_OPTEE_FILE_PATH)/$(DTS_OPTEE_FILE) $(OPTEE_RES_START) $(OPTEE_RES_SIZE)
endif
endif
	@$(MAKE) -C $(KERNEL_DIR)/ ARCH=$(ARCH) CROSS_COMPILE=$(CROSS) dtbs  $(EXT_FLAG)
ifeq ($(PROJECT), AX650_xpilot_6v)
	@dd if=$(KBUILD_OUTDIR)/arch/arm64/boot/dts/axera/$(PROJECT)_alpha.dtb of=$(KBUILD_OUTDIR)/arch/arm64/boot/dts/axera/$(PROJECT).dtb
	@dd if=$(KBUILD_OUTDIR)/arch/arm64/boot/dts/axera/$(PROJECT)_beta.dtb of=$(KBUILD_OUTDIR)/arch/arm64/boot/dts/axera/$(PROJECT).dtb bs=128k seek=2
endif
ifneq (,$(findstring $(PROJECT), "AX650_emmc" "AX650_master" "AX650_xpilot_6v" "AX650_ssd" "AX650_pipro_box" "AX650_lp"))
ifneq ($(strip $(SUPPORT_OPTEE)),false)
	@$(CP) $(DTS_OPTEE_FILE_PATH)/$(DTS_OPTEE_FILE)_bak $(DTS_OPTEE_FILE_PATH)/$(DTS_OPTEE_FILE)
	@$(RM) $(DTS_OPTEE_FILE_PATH)/$(DTS_OPTEE_FILE)_bak
endif
endif

menuconfig: prepare
	@$(ECHO)
	@$(ECHO) -e $(GREEN) "Making $@..." $(DONE)
	@$(MAKE) -C $(KERNEL_DIR)/ ARCH=$(ARCH) CROSS_COMPILE=$(CROSS) menuconfig $(EXT_FLAG)

savedefconfig:
	@$(ECHO)
	@$(ECHO) -e $(GREEN) "Making $@..." $(DONE)
	@$(MAKE) -C $(KERNEL_DIR)/ ARCH=$(ARCH) CROSS_COMPILE=$(CROSS) savedefconfig $(EXT_FLAG)

olddefconfig:
	@$(ECHO)
	@$(ECHO) -e $(GREEN) "Making $@..." $(DONE)
	@$(MAKE) -C $(KERNEL_DIR)/ ARCH=$(ARCH) CROSS_COMPILE=$(CROSS) olddefconfig $(EXT_FLAG)

dtbs:
	@$(ECHO)
	@$(ECHO) -e $(GREEN) "Making $@..." $(DONE)
	@$(MAKE) -C $(KERNEL_DIR)/ ARCH=$(ARCH) CROSS_COMPILE=$(CROSS) dtbs $(EXT_FLAG)
ifeq ($(PROJECT), AX650_xpilot_6v)
	@dd if=$(KBUILD_OUTDIR)/arch/arm64/boot/dts/axera/$(PROJECT)_alpha.dtb of=$(KBUILD_OUTDIR)/arch/arm64/boot/dts/axera/$(PROJECT).dtb
	@dd if=$(KBUILD_OUTDIR)/arch/arm64/boot/dts/axera/$(PROJECT)_beta.dtb of=$(KBUILD_OUTDIR)/arch/arm64/boot/dts/axera/$(PROJECT).dtb bs=128k seek=2
endif

install:
	@$(ECHO) -e $(GREEN) "Linux $@..." $(DONE)
	@$(MKDIR) $(IMAGE_OUTDIR)
	@$(MKDIR) $(IMAGE_OUTDIR)/debug
	$(CP) $(KBUILD_OUTDIR)/vmlinux  $(IMAGE_OUTDIR)/debug/
	$(CP) $(KBUILD_OUTDIR)/arch/arm64/boot/Image $(IMAGE_OUTDIR)
	$(CP) $(KBUILD_OUTDIR)/arch/arm64/boot/dts/axera/$(PROJECT).dtb $(IMAGE_OUTDIR)
	$(CP) $(KBUILD_OUTDIR)/arch/arm64/boot/dts/axera/AX650_emmc_dual_pcie.dtb $(IMAGE_OUTDIR)
	$(CP) $(KBUILD_OUTDIR)/arch/arm64/boot/dts/axera/AX650_zebu.dtb $(IMAGE_OUTDIR)
	@$(MKDIR) $(OSDRV_OUT_PATCH)/ko -p
	$(CP) $(KBUILD_OUTDIR)/drivers/pci/controller/dwc/pcie-ax-ep.ko  $(OSDRV_OUT_PATCH)/ko
ifeq ($(strip $(IMAGE_ENC)),true)
	@openssl aes-256-ecb -e -in $(KBUILD_OUTDIR)/arch/arm64/boot/dts/axera/$(PROJECT).dtb -out $(KBUILD_OUTDIR)/arch/arm64/boot/dts/axera/$(PROJECT)_enc.dtb -K $(shell cat $(TOOLS_PATH)/imgsign/$(AES_KEY)) -nosalt -p
	@openssl aes-256-ecb -e -in $(KBUILD_OUTDIR)/arch/arm64/boot/Image -out $(KBUILD_OUTDIR)/arch/arm64/boot/Image_enc -K $(shell cat $(TOOLS_PATH)/imgsign/$(AES_KEY)) -nosalt -p
endif
	@$(ECHO) -e $(GREEN) "Make boot.img file" $(DONE)
ifneq (,$(findstring $(PROJECT), "AX650_emmc" "AX650_master" "AX650_xpilot_6v" "AX650_ssd" "AX650_pipro_box" "AX650_lp"))
ifeq ($(strip $(SIGN_USE_RSA3072)),true)
	@python3 $(TOOLS_PATH)/imgsign/sec_boot_AX650_sign_v2.py -i $(KBUILD_OUTDIR)/arch/arm64/boot/Image -pub $(TOOLS_PATH)/imgsign/key_3072/pubkey.pem -prv $(TOOLS_PATH)/imgsign/key_3072/private.pem  -o $(BUILD_PATH)/out/$(PROJECT)/images/boot_signed.bin -cap 0x6fefe -key_bit 3072
	@python3 $(TOOLS_PATH)/imgsign/sec_boot_AX650_sign_v2.py -i $(KBUILD_OUTDIR)/arch/arm64/boot/dts/axera/$(PROJECT).dtb -pub $(TOOLS_PATH)/imgsign/key_3072/pubkey.pem -prv $(TOOLS_PATH)/imgsign/key_3072/private.pem  -o $(BUILD_PATH)/out/$(PROJECT)/images/$(PROJECT)_signed.dtb -cap 0x6fefe -key_bit 3072
	@python3 $(TOOLS_PATH)/imgsign/sec_boot_AX650_sign_v2.py -i $(KBUILD_OUTDIR)/arch/arm64/boot/dts/axera/AX650_emmc_dual_pcie.dtb -pub $(TOOLS_PATH)/imgsign/key_3072/pubkey.pem -prv $(TOOLS_PATH)/imgsign/key_3072/private.pem  -o $(BUILD_PATH)/out/$(PROJECT)/images/AX650_emmc_dual_pcie_signed.dtb -cap 0x6fefe -key_bit 3072
	$(CP) $(BUILD_PATH)/out/$(PROJECT)/images/AX650_emmc_dual_pcie_signed.dtb $(IMAGE_OUTDIR)/debug/
ifeq ($(strip $(IMAGE_ENC)),true)
	@python3 $(TOOLS_PATH)/imgsign/sec_boot_AX650_sign_v2.py -i $(KBUILD_OUTDIR)/arch/arm64/boot/Image_enc -pub $(TOOLS_PATH)/imgsign/key_3072/pubkey.pem -prv $(TOOLS_PATH)/imgsign/key_3072/private.pem  -o $(BUILD_PATH)/out/$(PROJECT)/images/boot_signed.bin -cap 0x6fffe -key_bit 3072
	@python3 $(TOOLS_PATH)/imgsign/sec_boot_AX650_sign_v2.py -i $(KBUILD_OUTDIR)/arch/arm64/boot/dts/axera/$(PROJECT)_enc.dtb -pub $(TOOLS_PATH)/imgsign/key_3072/pubkey.pem -prv $(TOOLS_PATH)/imgsign/key_3072/private.pem  -o $(BUILD_PATH)/out/$(PROJECT)/images/$(PROJECT)_signed.dtb -cap 0x6fffe -key_bit 3072
endif
else
	@python3 $(TOOLS_PATH)/imgsign/sec_boot_AX650_sign_v2.py -i $(KBUILD_OUTDIR)/arch/arm64/boot/Image -pub $(TOOLS_PATH)/imgsign/public.pem -prv $(TOOLS_PATH)/imgsign/private.pem  -o $(BUILD_PATH)/out/$(PROJECT)/images/boot_signed.bin -cap 0x6fafe -key_bit 2048
	@python3 $(TOOLS_PATH)/imgsign/sec_boot_AX650_sign_v2.py -i $(KBUILD_OUTDIR)/arch/arm64/boot/dts/axera/$(PROJECT).dtb -pub $(TOOLS_PATH)/imgsign/public.pem -prv $(TOOLS_PATH)/imgsign/private.pem  -o $(BUILD_PATH)/out/$(PROJECT)/images/$(PROJECT)_signed.dtb -cap 0x6fafe -key_bit 2048
	@python3 $(TOOLS_PATH)/imgsign/sec_boot_AX650_sign_v2.py -i $(KBUILD_OUTDIR)/arch/arm64/boot/dts/axera/AX650_emmc_dual_pcie.dtb -pub $(TOOLS_PATH)/imgsign/public.pem -prv $(TOOLS_PATH)/imgsign/private.pem  -o $(BUILD_PATH)/out/$(PROJECT)/images/AX650_emmc_dual_pcie_signed.dtb -cap 0x6fafe -key_bit 2048
	$(CP) $(BUILD_PATH)/out/$(PROJECT)/images/AX650_emmc_dual_pcie_signed.dtb $(IMAGE_OUTDIR)/debug/
ifeq ($(strip $(IMAGE_ENC)),true)
	@python3 $(TOOLS_PATH)/imgsign/sec_boot_AX650_sign_v2.py -i $(KBUILD_OUTDIR)/arch/arm64/boot/Image_enc -pub $(TOOLS_PATH)/imgsign/public.pem -prv $(TOOLS_PATH)/imgsign/private.pem  -o $(BUILD_PATH)/out/$(PROJECT)/images/boot_signed.bin -cap 0x6fbfe -key_bit 2048
	@python3 $(TOOLS_PATH)/imgsign/sec_boot_AX650_sign_v2.py -i $(KBUILD_OUTDIR)/arch/arm64/boot/dts/axera/$(PROJECT)_enc.dtb -pub $(TOOLS_PATH)/imgsign/public.pem -prv $(TOOLS_PATH)/imgsign/private.pem  -o $(BUILD_PATH)/out/$(PROJECT)/images/$(PROJECT)_signed.dtb -cap 0x6fbfe -key_bit 2048
endif
endif
else
	@python3 $(TOOLS_PATH)/imgsign/boot_AX650_sign.py -i $(KBUILD_OUTDIR)/arch/arm64/boot/Image -o $(BUILD_PATH)/out/$(PROJECT)/images/boot.img
endif

ifeq ($(SUPPORT_RECOVERY), TRUE)
	@$(ECHO) -e $(GREEN) "Recovery $@..." $(DONE)
	$(CP) $(BUILD_PATH)/out/$(PROJECT)/images/$(PROJECT)_signed.dtb $(BUILD_PATH)/out/$(PROJECT)/images/recovery_signed.dtb
	@$(MKDIR) $(IMAGE_OUTDIR)/recovery/debug
	$(CP) $(KBUILD_RECOVERY_OUTDIR)/vmlinux  $(IMAGE_OUTDIR)/recovery/debug/
	$(CP) $(KBUILD_RECOVERY_OUTDIR)/arch/arm64/boot/Image $(IMAGE_OUTDIR)/recovery
ifeq ($(strip $(IMAGE_ENC)),true)
	@openssl aes-256-ecb -e -in $(KBUILD_RECOVERY_OUTDIR)/arch/arm64/boot/Image -out $(KBUILD_RECOVERY_OUTDIR)/arch/arm64/boot/Image_enc -K $(shell cat $(TOOLS_PATH)/imgsign/$(AES_KEY)) -nosalt -p
endif
	@$(ECHO) -e $(GREEN) "Make recovery.img file" $(DONE)
ifneq (,$(findstring $(PROJECT), "AX650_emmc" "AX650_master" "AX650_xpilot_6v" "AX650_ssd" "AX650_pipro_box" "AX650_lp"))
ifeq ($(strip $(SIGN_USE_RSA3072)),true)
	@python3 $(TOOLS_PATH)/imgsign/sec_boot_AX650_sign_v2.py -i $(KBUILD_RECOVERY_OUTDIR)/arch/arm64/boot/Image -pub $(TOOLS_PATH)/imgsign/key_3072/pubkey.pem -prv $(TOOLS_PATH)/imgsign/key_3072/private.pem  -o $(BUILD_PATH)/out/$(PROJECT)/images/recovery_signed.bin -cap 0x6fefe -key_bit 3072
ifeq ($(strip $(IMAGE_ENC)),true)
	@python3 $(TOOLS_PATH)/imgsign/sec_boot_AX650_sign_v2.py -i $(KBUILD_RECOVERY_OUTDIR)/arch/arm64/boot/Image_enc -pub $(TOOLS_PATH)/imgsign/key_3072/pubkey.pem -prv $(TOOLS_PATH)/imgsign/key_3072/private.pem  -o $(BUILD_PATH)/out/$(PROJECT)/images/recovery_signed.bin -cap 0x6fffe -key_bit 3072
endif
else
	@python3 $(TOOLS_PATH)/imgsign/sec_boot_AX650_sign_v2.py -i $(KBUILD_RECOVERY_OUTDIR)/arch/arm64/boot/Image -pub $(TOOLS_PATH)/imgsign/public.pem -prv $(TOOLS_PATH)/imgsign/private.pem  -o $(BUILD_PATH)/out/$(PROJECT)/images/recovery_signed.bin -cap 0x6fafe -key_bit 2048
ifeq ($(strip $(IMAGE_ENC)),true)
	@python3 $(TOOLS_PATH)/imgsign/sec_boot_AX650_sign_v2.py -i $(KBUILD_RECOVERY_OUTDIR)/arch/arm64/boot/Image_enc -pub $(TOOLS_PATH)/imgsign/public.pem -prv $(TOOLS_PATH)/imgsign/private.pem  -o $(BUILD_PATH)/out/$(PROJECT)/images/recovery_signed.bin -cap 0x6fbfe -key_bit 2048
endif
endif
else
	@python3 $(TOOLS_PATH)/imgsign/boot_AX650_sign.py -i $(KBUILD_RECOVERY_OUTDIR)/arch/arm64/boot/Image -o $(BUILD_PATH)/out/$(PROJECT)/images/recovery.img
endif
endif


clean:
	@$(ECHO) -e $(GREEN) "Linux $@..." $(DONE)
	@$(MAKE) -C $(KERNEL_DIR)/ clean  $(EXT_FLAG)
	@$(MAKE) -C $(KERNEL_DIR)/ mrproper  $(EXT_FLAG)
	@rm -rf $(KBUILD_OUTDIR)
	@rm -rf $(OSDRV_OUT_PATCH)/ko/pcie-ax-ep.ko
ifeq ($(SUPPORT_RECOVERY), TRUE)
	@rm -rf $(KBUILD_RECOVERY_OUTDIR)
endif
