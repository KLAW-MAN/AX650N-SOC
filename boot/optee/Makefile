HOME_PATH  := $(abspath $(shell pwd)/../..)
BUILD_PATH := $(HOME_PATH)/build
TOOLS_PATH := $(HOME_PATH)/tools

include $(BUILD_PATH)/color.mk
include $(BUILD_PATH)/config.mak

OPTEE           := optee
OPTEE_DIR       := optee_os-3.21.0

OPTEE_ELF  := tee.elf
OPTEE_BIN  := tee-pager_v2.bin
OPTEE_IMG  := optee.img
OPTEE_SIGNED_BIN  := optee_signed.bin
OPTEE_PLAT := ax650
OPTEE_TARGET := optee
AES_KEY := aes-256.key

.PHONY: all prepare optee
.NOTPARALLEL: clean install prepare

#OPTEE build param
BUILD_BASE   = $(HOME_PATH)/build/out/$(PROJECT)/objs/boot/$(OPTEE)/$(OPTEE_DIR)
export BUILD_BASE

IMAGE_OUTDIR := $(BUILD_PATH)/out/$(PROJECT)/images
DEBUG_LOG_LEVEL := 1
USERCFLAGS   += PLAT_OPTEE_ADDR_START="$(OPTEE_IMAGE_ADDR)"
USERCFLAGS   += PLAT_OPTEE_SHMEM_SIZE="$(OPTEE_SHMEM_SIZE)"
USERCFLAGS   += PLAT_OPTEE_RESERVED_SIZE="$(OPTEE_RESERVED_SIZE)"

EXT_FLAGS    += $(USERCFLAGS)

default: optee

all: prepare optee
	@$(ECHO) -e $(GREEN)"\nBuild optee success!!\n" $(DONE)

prepare:
	@$(ECHO)
	@$(ECHO) -e $(GREEN) "first build prepare" $(DONE)

optee:
	@$(ECHO)
	@$(ECHO) -e $(GREEN) "Making $@..." $(DONE)
	@$(MAKE) -C $(OPTEE_DIR)/ CROSS_COMPILE=$(CROSS) CROSS_COMPILE64=$(CROSS) CFG_TEE_CORE_LOG_LEVEL=$(DEBUG_LOG_LEVEL) PLATFORM=axera-$(OPTEE_PLAT) O=$(BUILD_BASE) CFG_ARM64_core=y $(EXT_FLAGS) -j4

install:
	@$(ECHO) -e $(GREEN) "optee $@..." $(DONE)
	@$(MKDIR) $(IMAGE_OUTDIR)
	@$(MKDIR) $(IMAGE_OUTDIR)/debug
	$(CP) $(BUILD_BASE)/core/$(OPTEE_ELF) $(IMAGE_OUTDIR)/debug/$(OPTEE_ELF)
ifeq ($(strip $(IMAGE_ENC)),true)
	@openssl aes-256-ecb -e -in $(BUILD_BASE)/core/$(OPTEE_BIN) -out $(BUILD_BASE)/core/optee_enc.bin -K $(shell cat $(TOOLS_PATH)/imgsign/$(AES_KEY)) -nosalt -p
endif
ifneq (,$(findstring $(PROJECT), "AX650_emmc" "AX650_lp" "AX650_pipro_box" "AX650_ssd" "AX650_master" "AX650_xpilot_6v"))
ifeq ($(strip $(SIGN_USE_RSA3072)),true)
	@python3 $(TOOLS_PATH)/imgsign/sec_boot_AX650_sign_v2.py -i $(BUILD_BASE)/core/$(OPTEE_BIN) -pub $(TOOLS_PATH)/imgsign/key_3072/pubkey.pem -prv $(TOOLS_PATH)/imgsign/key_3072/private.pem  -o $(IMAGE_OUTDIR)/$(OPTEE_SIGNED_BIN) -cap 0x6fefe -key_bit 3072
ifeq ($(strip $(IMAGE_ENC)),true)
	@python3 $(TOOLS_PATH)/imgsign/sec_boot_AX650_sign_v2.py -i $(BUILD_BASE)/core/optee_enc.bin -pub $(TOOLS_PATH)/imgsign/key_3072/pubkey.pem -prv $(TOOLS_PATH)/imgsign/key_3072/private.pem  -o $(IMAGE_OUTDIR)/$(OPTEE_SIGNED_BIN) -cap 0x6fffe -key_bit 3072
endif
else
	@python3 $(TOOLS_PATH)/imgsign/sec_boot_AX650_sign_v2.py -i $(BUILD_BASE)/core/$(OPTEE_BIN) -pub $(TOOLS_PATH)/imgsign/public.pem -prv $(TOOLS_PATH)/imgsign/private.pem  -o $(IMAGE_OUTDIR)/$(OPTEE_SIGNED_BIN) -cap 0x6fafe -key_bit 2048
ifeq ($(strip $(IMAGE_ENC)),true)
	@python3 $(TOOLS_PATH)/imgsign/sec_boot_AX650_sign_v2.py -i $(BUILD_BASE)/core/optee_enc.bin -pub $(TOOLS_PATH)/imgsign/public.pem -prv $(TOOLS_PATH)/imgsign/private.pem  -o $(IMAGE_OUTDIR)/$(OPTEE_SIGNED_BIN) -cap 0x6fbfe -key_bit 2048
endif
endif
else
	python3 $(TOOLS_PATH)/imgsign/boot_AX650_sign.py -i $(BUILD_BASE)/core/$(OPTEE_BIN) -o $(IMAGE_OUTDIR)/$(OPTEE_IMG)
endif

clean:
	@$(ECHO) -e $(GREEN) "optee $@..." $(DONE)
	@$(MAKE) -C $(OPTEE_DIR)/ PLATFORM=axera-$(OPTEE_PLAT) O=$(BUILD_BASE) CFG_ARM64_core=y clean
	@rm -rf $(BUILD_BASE)
	@rm -f $(IMAGE_OUTDIR)/$(OPTEE_IMG)
	@rm -f $(IMAGE_OUTDIR)/$(OPTEE_SIGNED_BIN)
	@rm -f $(IMAGE_OUTDIR)/debug/$(OPTEE_ELF)
