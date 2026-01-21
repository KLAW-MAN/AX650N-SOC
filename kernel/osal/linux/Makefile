CURDIR = linux
SUBDIRS = kernel
#vcodec

################################################################################
#	prepare param
################################################################################
MKFILE_PATH        := $(abspath $(lastword $(MAKEFILE_LIST)))
HOME_PATH          := $(abspath $(dir $(MKFILE_PATH))../../..)
BUILD_PATH         := $(HOME_PATH)/build

include $(HOME_PATH)/build/color.mk
include $(HOME_PATH)/build/config.mak

ROOTFS_TARGET_PATH ?= $(HOME_PATH)/kernel/osdrv/out

KERNEL_DIR  ?=  linux-5.15.73
################################################################################
#	set task
################################################################################
SUBDIRS_CLEAN   = $(addsuffix .clean, $(SUBDIRS))
SUBDIRS_INSTALL = $(addsuffix .install, $(SUBDIRS))

.PHONY: $(SUBDIRS) $(SUBDIRS_INSTALL) $(SUBDIRS_CLEAN)

all: $(SUBDIRS)
	@echo -e $(GREEN)"\nBuild All $(CURDIR) Modules success!!\n"  $(DONE)

install: $(SUBDIRS_INSTALL)
	@echo -e $(GREEN)"\nInstall $(CURDIR) success!!\n"  $(DONE)

clean:	$(SUBDIRS_CLEAN)
	@echo -e $(GREEN)"\nClean $(CURDIR) success!!\n"  $(DONE)

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
