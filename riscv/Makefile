
################################################################################
#	prepare param
################################################################################
HOME_PATH  := $(abspath $(shell pwd)/..)
BUILD_PATH := $(HOME_PATH)/build

include $(BUILD_PATH)/color.mk
include $(BUILD_PATH)/config.mak

MSP_OUT_PATH        := $(HOME_PATH)/msp/out
ROOTFS_TARGET_PATH  := $(MSP_OUT_PATH)

CURDIR	:= $(abspath $(shell pwd))
CONFIG	:= $(CURDIR)/config/
OUTDIR	:= $(CURDIR)/out

KBUILD_OUTDIR    := $(CURDIR)/config
IMAGE_OUTDIR     := $(BUILD_PATH)/out/$(PROJECT)/images

RISCV_BIN := rtthread.bin

MAKE    := scons

.PHONY: clean all install
.NOTPARALLEL: clean install

default: all

all:
	@$(MAKE) -C $(CONFIG) -j16
	@echo -e $(GREEN)"\nBuild All $(CURDIR) Modules success!!\n"  $(DONE)

install:
	@mkdir -p $(ROOTFS_TARGET_PATH)/bin/riscv
	$(CP) $(KBUILD_OUTDIR)/$(RISCV_BIN) $(ROOTFS_TARGET_PATH)/bin/riscv
	@echo -e $(GREEN)"\nInstall riscv bin success!!\n"  $(DONE)

clean:
	@rm -rf $(CONFIG)/build
	@rm -rf $(OUTDIR)
	@echo -e $(GREEN)"\nClean $(CURDIR) success!!\n"  $(DONE)
