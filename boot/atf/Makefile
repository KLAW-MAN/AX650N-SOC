HOME_PATH  := $(abspath $(shell pwd)/../..)
BUILD_PATH := $(HOME_PATH)/build
TOOLS_PATH := $(HOME_PATH)/tools

include $(BUILD_PATH)/color.mk
include $(BUILD_PATH)/config.mak

ATF           := atf
ATF_DIR       := arm-trusted-firmware

ATF_BL31_ELF  := bl31.elf
ATF_BL31_BIN  := bl31.bin
ATF_BL31_SIGNED_BIN  := atf_bl31_signed.bin
ATF_BL31_IMG  := atf_bl31.img
ATF_ARCH := aarch64
ATF_PLAT := ax650
ATF_TARGET := bl31
AES_KEY := aes-256.key

.PHONY: all prepare atf_bl31 clean
.NOTPARALLEL: prepare clean install

#ATF build param
BUILD_BASE   = $(HOME_PATH)/build/out/$(PROJECT)/objs/boot/$(ATF)/$(ATF_DIR)
export BUILD_BASE

IMAGE_OUTDIR := $(BUILD_PATH)/out/$(PROJECT)/images
DEBUG_EN := 0
ifeq ($(DEBUG_EN),1)
ATF_RELEASE_PATH := debug
else
ATF_RELEASE_PATH := release
endif

default: atf_bl31

all: prepare atf_bl31
	@$(ECHO) -e $(GREEN)"\nBuild atf_bl31 success!!\n" $(DONE)

prepare:
	@$(ECHO)
	@$(ECHO) -e $(GREEN) "first build prepare" $(DONE)

atf_bl31:
	@$(ECHO)
	@$(ECHO) -e $(GREEN) "Making $@..." $(DONE)
ifneq ($(strip $(SUPPORT_OPTEE)),false)
	@$(MAKE) -C $(ATF_DIR)/ ARCH=$(ATF_ARCH) CROSS_COMPILE=$(CROSS) PLAT=$(ATF_PLAT) SPD=opteed $(ATF_TARGET) DEBUG=$(DEBUG_EN) -j4
else
	@$(MAKE) -C $(ATF_DIR)/ ARCH=$(ATF_ARCH) CROSS_COMPILE=$(CROSS) PLAT=$(ATF_PLAT) $(ATF_TARGET) DEBUG=$(DEBUG_EN) -j4
endif

install:
	@$(ECHO) -e $(GREEN) "atf_bl31 $@..." $(DONE)
	@$(MKDIR) $(IMAGE_OUTDIR)
	@$(MKDIR) $(IMAGE_OUTDIR)/debug
	$(CP) $(BUILD_BASE)/$(ATF_PLAT)/$(ATF_RELEASE_PATH)/$(ATF_BL31_BIN) $(IMAGE_OUTDIR)/atf_bl31.bin
	$(CP) $(BUILD_BASE)/$(ATF_PLAT)/$(ATF_RELEASE_PATH)/$(ATF_TARGET)/$(ATF_BL31_ELF) $(IMAGE_OUTDIR)/debug/$(ATF)_$(ATF_BL31_ELF)

ifeq ($(strip $(IMAGE_ENC)),true)
	@openssl aes-256-ecb -e -in $(BUILD_BASE)/$(ATF_PLAT)/$(ATF_RELEASE_PATH)/$(ATF_BL31_BIN) -out $(BUILD_BASE)/$(ATF_PLAT)/$(ATF_RELEASE_PATH)/$(ATF_TARGET)_enc.bin -K $(shell cat $(TOOLS_PATH)/imgsign/$(AES_KEY)) -nosalt -p
endif

ifneq (,$(findstring $(PROJECT), "AX650_emmc" "AX650_lp" "AX650_pipro_box" "AX650_ssd" "AX650_master" "AX650_xpilot_6v"))
ifeq ($(strip $(SIGN_USE_RSA3072)),true)
	@python3 $(TOOLS_PATH)/imgsign/sec_boot_AX650_sign_v2.py -i $(BUILD_BASE)/$(ATF_PLAT)/$(ATF_RELEASE_PATH)/$(ATF_BL31_BIN) -pub $(TOOLS_PATH)/imgsign/key_3072/pubkey.pem -prv $(TOOLS_PATH)/imgsign/key_3072/private.pem  -o $(IMAGE_OUTDIR)/$(ATF_BL31_SIGNED_BIN) -cap 0x6fefe -key_bit 3072
ifeq ($(strip $(IMAGE_ENC)),true)
	@python3 $(TOOLS_PATH)/imgsign/sec_boot_AX650_sign_v2.py -i $(BUILD_BASE)/$(ATF_PLAT)/$(ATF_RELEASE_PATH)/$(ATF_TARGET)_enc.bin -pub $(TOOLS_PATH)/imgsign/key_3072/pubkey.pem -prv $(TOOLS_PATH)/imgsign/key_3072/private.pem  -o $(IMAGE_OUTDIR)/$(ATF_BL31_SIGNED_BIN) -cap 0x6fffe -key_bit 3072
endif
else
	@python3 $(TOOLS_PATH)/imgsign/sec_boot_AX650_sign_v2.py -i $(BUILD_BASE)/$(ATF_PLAT)/$(ATF_RELEASE_PATH)/$(ATF_BL31_BIN) -pub $(TOOLS_PATH)/imgsign/public.pem -prv $(TOOLS_PATH)/imgsign/private.pem  -o $(IMAGE_OUTDIR)/$(ATF_BL31_SIGNED_BIN) -cap 0x6fafe -key_bit 2048
ifeq ($(strip $(IMAGE_ENC)),true)
	@python3 $(TOOLS_PATH)/imgsign/sec_boot_AX650_sign_v2.py -i $(BUILD_BASE)/$(ATF_PLAT)/$(ATF_RELEASE_PATH)/$(ATF_TARGET)_enc.bin -pub $(TOOLS_PATH)/imgsign/public.pem -prv $(TOOLS_PATH)/imgsign/private.pem  -o $(IMAGE_OUTDIR)/$(ATF_BL31_SIGNED_BIN) -cap 0x6fbfe -key_bit 2048
endif
endif
else
	@python3 $(TOOLS_PATH)/imgsign/boot_AX650_sign.py -i $(BUILD_BASE)/$(ATF_PLAT)/$(ATF_RELEASE_PATH)/$(ATF_BL31_BIN) -o $(IMAGE_OUTDIR)/$(ATF_BL31_IMG)
endif

clean:
	@$(ECHO) -e $(GREEN) "atf_bl31 $@..." $(DONE)
	@$(MAKE) -C $(ATF_DIR)/ realclean
	@rm -rf $(IMAGE_OUTDIR)/debug/$(ATF)_$(ATF_BL31_ELF)
	@rm -rf $(IMAGE_OUTDIR)/$(ATF_BL31_IMG)
	@rm -rf $(IMAGE_OUTDIR)/$(ATF_BL31_SIGNED_BIN)
