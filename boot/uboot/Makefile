HOME_PATH  := $(abspath $(shell pwd)/../..)
BUILD_PATH := $(HOME_PATH)/build
TOOL_PATH  := $(HOME_PATH)/tools/imgsign

include $(BUILD_PATH)/color.mk
include $(BUILD_PATH)/config.mak

UBOOT_ELF := u-boot
UBOOT_BIN := u-boot.bin
UBOOT_SIGN_BIN := u-boot_signed.bin
AES_KEY := aes-256.key
UBOOT_ENC_BIN := u-boot_enc.bin
UBOOT_ENC_SIGN_BIN := u-boot_enc_signed.bin
FDL2_BIN := fdl2.bin
FDL2_SIGN_BIN := fdl2_signed.bin

.PHONY: all prepare uboot
.NOTPARALLEL: clean install prepare

UBOOT_DIR ?= u-boot-2020.04

DEFAULT_CONFIG_STR := _defconfig

CONFIG ?=$(PROJECT)$(DEFAULT_CONFIG_STR)

BUILD_BOARD_PATH = $(shell echo $(PROJECT) | tr A-Z a-z)
KBUILD_OUTDIR    := $(HOME_PATH)/build/out/$(PROJECT)/objs/boot/uboot/$(UBOOT_DIR)
IMAGE_OUTDIR     :=$(BUILD_PATH)/out/$(PROJECT)/images
EXT_FLAG         := O=$(KBUILD_OUTDIR)  HOME_PATH=$(HOME_PATH)
ifneq (,$(findstring $(PROJECT), "AX650_emmc" "AX650_master" "AX650_xpilot_6v" "AX650_ssd" "AX650_pipro_box" "AX650_lp"))
ifneq ($(strip $(SUPPORT_OPTEE)),false)
USERCFLAGS       += -DOPTEE_BOOT
USERCFLAGS       += -DOPTEE_IMAGE_ADDR="$(OPTEE_IMAGE_ADDR)"
USERCFLAGS       += -DOPTEE_RESERVED_SIZE="$(OPTEE_RESERVED_SIZE)"
USERCFLAGS       += -DOPTEE_SHMEM_SIZE="$(OPTEE_SHMEM_SIZE)"
endif
ifeq ($(strip $(SECURE_ENABLE)),true)
USERCFLAGS       += -DSECUREBOOT_ENABLE
USERCFLAGS       += -DSECURE_ENABLE
endif
ifeq ($(SUPPORT_RECOVERY),TRUE)
USERCFLAGS += -DSUPPORT_RECOVERY
endif
EXT_FLAG         += KCFLAGS="$(USERCFLAGS)"
endif

ifeq ($(boot_bk_en),y)
	USE_BACKUP := true
endif
ifeq ($(PROJECT),AX650_emmc)
	USE_BACKUP := true
endif
ifeq ($(PROJECT),AX650_xpilot_6v)
	USE_BACKUP := true
endif
ifeq ($(PROJECT),AX650_haps)
	USE_BACKUP := true
endif
ifeq ($(PROJECT),AX650_slave)
	USE_BACKUP := true
endif
ifeq ($(PROJECT),AX650_master)
	USE_BACKUP := true
endif
ifeq ($(PROJECT),AX650_ssd)
	USE_BACKUP := true
endif
ifeq ($(PROJECT),AX650_pipro_box)
	USE_BACKUP := true
endif
ifeq ($(PROJECT),AX650_lp)
	USE_BACKUP := true
endif
default: uboot

all: prepare uboot
	@$(ECHO) -e $(GREEN)"\nBuild uboot success!!\n" $(DONE)

prepare:
	@$(ECHO)
	@$(ECHO) -e $(GREEN) "first build prepare" $(DONE)
	@$(MAKE) -C $(UBOOT_DIR)/ CROSS_COMPILE=$(CROSS) $(CONFIG) $(EXT_FLAG)

menuconfig: prepare
	@$(ECHO)
	@$(ECHO) -e $(GREEN) "build menuconfig" $(DONE)
	@$(MAKE) -C $(UBOOT_DIR)/ CROSS_COMPILE=$(CROSS) menuconfig $(EXT_FLAG)

savedefconfig:
	@$(ECHO)
	@$(ECHO) -e $(GREEN) "build savedefconfig" $(DONE)
	@$(MAKE) -C $(UBOOT_DIR)/ CROSS_COMPILE=$(CROSS) savedefconfig $(EXT_FLAG)

olddefconfig:
	@$(ECHO)
	@$(ECHO) -e $(GREEN) "build olddefconfig" $(DONE)
	@$(MAKE) -C $(UBOOT_DIR)/ CROSS_COMPILE=$(CROSS) olddefconfig $(EXT_FLAG)

uboot:
	@$(ECHO)
	@$(ECHO) -e $(GREEN) "Making $@..." $(DONE)
	@$(MAKE) -C $(UBOOT_DIR)/ CROSS_COMPILE=$(CROSS) -j4 $(EXT_FLAG)

install:
	@$(ECHO) -e $(GREEN) "uboot $@..." $(DONE)
	@$(MKDIR) $(IMAGE_OUTDIR)
	@$(MKDIR) $(IMAGE_OUTDIR)/debug
	$(CP) $(KBUILD_OUTDIR)/$(UBOOT_ELF) $(IMAGE_OUTDIR)/debug/
	$(CP) $(KBUILD_OUTDIR)/$(UBOOT_BIN) $(IMAGE_OUTDIR)/$(FDL2_BIN)
	@openssl aes-256-ecb -e -in $(KBUILD_OUTDIR)/$(UBOOT_BIN) -out $(KBUILD_OUTDIR)/$(UBOOT_ENC_BIN) -K $(shell cat $(UBOOT_DIR)/$(AES_KEY)) -nosalt -p
	$(CP) $(KBUILD_OUTDIR)/$(UBOOT_BIN) $(IMAGE_OUTDIR)/$(UBOOT_BIN)
	$(CP) $(KBUILD_OUTDIR)/$(UBOOT_ENC_BIN) $(IMAGE_OUTDIR)/$(UBOOT_ENC_BIN)
	$(CP) $(UBOOT_DIR)/$(AES_KEY) $(IMAGE_OUTDIR)/$(AES_KEY)
	$(CP) $(UBOOT_DIR)/tools/logos/axera_logo* $(IMAGE_OUTDIR)



ifeq ($(strip $(USE_BACKUP)),true)
ifeq ($(strip $(SIGN_USE_RSA3072)),true)
	$(warning SIGN_USE_RSA3072)
	@python3 $(TOOL_PATH)/spl_AX650_sign_3072_bk.py -i $(KBUILD_OUTDIR)/$(UBOOT_BIN) -o $(IMAGE_OUTDIR)/$(UBOOT_SIGN_BIN) -ob $(IMAGE_OUTDIR)/uboot_bk.bin -pub $(TOOL_PATH)/key_3072/pubkey.pem -prv $(TOOL_PATH)/key_3072/private.pem -fw $(HOME_PATH)/tools/imgsign/eip.bin -cap 0x4fefe -partsize 0x180000
	@python3 $(TOOL_PATH)/spl_AX650_sign_3072_bk.py -i $(KBUILD_OUTDIR)/$(UBOOT_ENC_BIN) -o $(IMAGE_OUTDIR)/$(UBOOT_ENC_SIGN_BIN) -ob $(IMAGE_OUTDIR)/uboot_enc_bk.bin -pub $(TOOL_PATH)/key_3072/pubkey.pem -prv $(TOOL_PATH)/key_3072/private.pem -fw $(HOME_PATH)/tools/imgsign/eip.bin -cap 0x4fffe -partsize 0x180000
	@python3 $(TOOL_PATH)/fdl_AX650_sign_3072.py -i $(KBUILD_OUTDIR)/$(UBOOT_BIN) -o $(IMAGE_OUTDIR)/$(FDL2_SIGN_BIN) -pub $(TOOL_PATH)/key_3072/pubkey.pem -prv $(TOOL_PATH)/key_3072/private.pem -fw $(HOME_PATH)/tools/imgsign/eip.bin -cap 0x4fefe -partsize 0x180000
else
	@python3 $(TOOL_PATH)/spl_AX650_sign_bk.py -i $(KBUILD_OUTDIR)/$(UBOOT_BIN) -o $(IMAGE_OUTDIR)/$(UBOOT_SIGN_BIN) -ob $(IMAGE_OUTDIR)/uboot_bk.bin -pub $(TOOL_PATH)/public.pem -prv $(TOOL_PATH)/private.pem -fw $(HOME_PATH)/tools/imgsign/eip.bin -cap 0x4fafe -partsize 0x180000
	@python3 $(TOOL_PATH)/spl_AX650_sign_bk.py -i $(KBUILD_OUTDIR)/$(UBOOT_ENC_BIN) -o $(IMAGE_OUTDIR)/$(UBOOT_ENC_SIGN_BIN) -ob $(IMAGE_OUTDIR)/uboot_enc_bk.bin -pub $(TOOL_PATH)/public.pem -prv $(TOOL_PATH)/private.pem -fw $(HOME_PATH)/tools/imgsign/eip.bin -cap 0x4fbfe -partsize 0x180000
	@python3 $(TOOL_PATH)/fdl_AX650_sign.py -i $(KBUILD_OUTDIR)/$(UBOOT_BIN) -o $(IMAGE_OUTDIR)/$(FDL2_SIGN_BIN) -pub $(TOOL_PATH)/public.pem -prv $(TOOL_PATH)/private.pem -fw $(HOME_PATH)/tools/imgsign/eip.bin -cap 0x4fafe -partsize 0x180000
endif
else
ifeq ($(strip $(SIGN_USE_RSA3072)),true)
	$(warning SIGN_USE_RSA3072)
	@python3 $(TOOL_PATH)/spl_AX650_sign_3072.py -i $(KBUILD_OUTDIR)/$(UBOOT_BIN) -o $(IMAGE_OUTDIR)/$(UBOOT_SIGN_BIN) -pub $(TOOL_PATH)/key_3072/pubkey.pem -prv $(TOOL_PATH)/key_3072/private.pem -fw $(HOME_PATH)/tools/imgsign/eip.bin -packsize 0x140000 -cap 0x54fefe -partsize 0x180000
	@python3 $(TOOL_PATH)/spl_AX650_sign_3072.py -i $(KBUILD_OUTDIR)/$(UBOOT_ENC_BIN) -o $(IMAGE_OUTDIR)/$(UBOOT_ENC_SIGN_BIN) -pub $(TOOL_PATH)/key_3072/pubkey.pem -prv $(TOOL_PATH)/key_3072/private.pem -fw $(HOME_PATH)/tools/imgsign/eip.bin -packsize 0x140000 -cap 0x54fffe -partsize 0x180000
	@python3 $(TOOL_PATH)/fdl_AX650_sign_3072.py -i $(KBUILD_OUTDIR)/$(UBOOT_BIN) -o $(IMAGE_OUTDIR)/$(FDL2_SIGN_BIN) -pub $(TOOL_PATH)/key_3072/pubkey.pem -prv $(TOOL_PATH)/key_3072/private.pem -fw $(HOME_PATH)/tools/imgsign/eip.bin -cap 0x54fefe -partsize 0x180000
else
	@python3 $(TOOL_PATH)/spl_AX650_sign.py -i $(KBUILD_OUTDIR)/$(UBOOT_BIN) -o $(IMAGE_OUTDIR)/$(UBOOT_SIGN_BIN) -pub $(TOOL_PATH)/public.pem -prv $(TOOL_PATH)/private.pem -fw $(HOME_PATH)/tools/imgsign/eip.bin -packsize 0x140000 -cap 0x54fafe -partsize 0x180000
	@python3 $(TOOL_PATH)/spl_AX650_sign.py -i $(KBUILD_OUTDIR)/$(UBOOT_ENC_BIN) -o $(IMAGE_OUTDIR)/$(UBOOT_ENC_SIGN_BIN) -pub $(TOOL_PATH)/public.pem -prv $(TOOL_PATH)/private.pem -fw $(HOME_PATH)/tools/imgsign/eip.bin -packsize 0x140000 -cap 0x54fbfe -partsize 0x180000
	@python3 $(TOOL_PATH)/fdl_AX650_sign.py -i $(KBUILD_OUTDIR)/$(UBOOT_BIN) -o $(IMAGE_OUTDIR)/$(FDL2_SIGN_BIN) -pub $(TOOL_PATH)/public.pem -prv $(TOOL_PATH)/private.pem -fw $(HOME_PATH)/tools/imgsign/eip.bin -cap 0x54fafe -partsize 0x180000
endif
endif
clean:
	@$(ECHO) -e $(GREEN) "uboot $@..." $(DONE)
	@$(MAKE) -C $(UBOOT_DIR)/ clean  $(EXT_FLAG)
	@$(MAKE) -C $(UBOOT_DIR)/ mrproper
	@rm -rf $(KBUILD_OUTDIR)
