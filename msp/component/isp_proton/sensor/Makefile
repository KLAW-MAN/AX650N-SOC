# SUBDIRS = include dummysensor dummy_bittrue ov_os08a20 ov_os08b10 ov_os04a10 ov_og09a10 sony_imx678 makebin smartsens_sc910gs smartsens_sc410gs ov_ov50a40 smartsens_sc450ai smartsens_sc850sl smartsens_sc500ai ov_ov48c40 sony_imx586 ov_ov48c sony_imx664
SUBDIRS = include ov_os08a20 ov_os08b10 ov_os04a10 ov_ov48c40 smartsens_sc910gs smartsens_sc410gs smartsens_sc850sl smartsens_sc500ai sony_imx678

################################################################################
#	prepare param
################################################################################
CUR_PATH		:= $(shell pwd)
HOME_PATH		:= $(abspath $(CUR_PATH)/../../../..)

include $(HOME_PATH)/build/color.mk
include $(HOME_PATH)/build/config.mak

OUT_DIR			:= $(HOME_PATH)/build/out/$(PROJECT)/objs
OUT_PATH		:= $(HOME_PATH)/msp/out
BASE_PATH		:= $(HOME_PATH)/msp/component
ROOTFS_TARGET_PATH	:= $(OUT_PATH)

################################################################################
#	set task
################################################################################
SUBDIRS_CLEAN   = $(addsuffix .clean, $(SUBDIRS))
SUBDIRS_INSTALL = $(addsuffix .install, $(SUBDIRS))

.PHONY: $(SUBDIRS) $(SUBDIRS_INSTALL) $(SUBDIRS_CLEAN)
.NOTPARALLEL: clean all

all: $(SUBDIRS)
	@$(ECHO) -e $(GREEN)"\nBuild All $(CURDIR) Modules success!!\n"  $(DONE)

install: $(SUBDIRS_INSTALL)
	@$(ECHO) -e $(GREEN)"\nInstall $(CURDIR) success!!\n"  $(DONE)

clean:	$(SUBDIRS_CLEAN)
	@$(ECHO) -e $(GREEN)"\nClean $(CURDIR) success!!\n"  $(DONE)

$(SUBDIRS):
	@$(ECHO)
	@$(ECHO) -e $(CYAN)"In subdir $@ ..." $(DONE)
	@$(MAKE) -C $(basename $@ )

$(SUBDIRS_INSTALL):
	@$(ECHO)
	@$(ECHO) -e $(CYAN)"In subdir $(basename $@ )..." $(DONE)
	@$(MAKE) -C $(basename $@ ) install

$(SUBDIRS_CLEAN):
	@$(ECHO) -e $(CYAN)"In subdir $(basename $@ )..." $(DONE)
	@$(MAKE) -C $(basename $@ ) clean


# Export val
export ROOTFS_TARGET_PATH
