MKFILE_PATH        := $(abspath $(lastword $(MAKEFILE_LIST)))
HOME_PATH          := $(abspath $(dir $(MKFILE_PATH))../../../../../)
BUILD_PATH         := $(HOME_PATH)/build

include $(BUILD_PATH)/config.mak

OBJ_OUT_DIR        := $(BUILD_PATH)/out/$(PROJECT)/objs
KSRC               := $(abspath $(dir $(MKFILE_PATH)))
SRC_RELATIVE_PATH  := $(subst $(HOME_PATH)/,,$(KSRC))
KBUILD_DIR         := $(OBJ_OUT_DIR)/$(SRC_RELATIVE_PATH)
KBUILD_MAKEFILE    := $(KBUILD_DIR)/Makefile
ROOTFS_TARGET_PATH ?= $(OBJ_OUT_DIR)/kernel/osdrv/out
KDIR	           ?= $(HOME_PATH)/kernel/linux/$(KERNEL_DIR)
PRV_DRV_PATH       := $(HOME_PATH)/kernel/osdrv/private_drv
OSAL_PATH          := $(HOME_PATH)/kernel/osal
MIPI_DRV_PATH      := $(HOME_PATH)/kernel/osdrv/private_drv/mipi
ISP_DRV_PATH       := $(HOME_PATH)/kernel/osdrv/private_drv/isp
PCIE_DRV_PATH      := $(HOME_PATH)/kernel/osdrv/ext_drv/pcie
COMMON_PATH    	   := $(HOME_PATH)/msp/component/common

KBUILD_EXTRA_SYMBOLS += $(OBJ_OUT_DIR)/kernel/osdrv/ext_drv/pcie/slave_dev/Module.symvers
KBUILD_EXTRA_SYMBOLS += $(OBJ_OUT_DIR)/kernel/osdrv/ext_drv/pcie/host_dev/Module.symvers
export KBUILD_EXTRA_SYMBOLS

MODULE_NAME     := ax_pcie_msg

EXTRA_CFLAGS += -I$(HOME_PATH)/msp/component/sys/include \
		-I$(MIPI_DRV_PATH)/include \
		-I$(PRV_DRV_PATH)/include \
		-I$(ISP_DRV_PATH)/include \
		-I$(PRV_DRV_PATH)/sys/include \
		-I$(OSAL_PATH)/include \
		-I$(COMMON_PATH)/include \
		-I$(PCIE_DRV_PATH)/include

# for debug
KCFLAGS	+=-DDEBUG

SRCS := ax_pcie_msg_usrdev.c

$(MODULE_NAME)-objs := $(SRCS:%.c=%.o)

obj-m := $(MODULE_NAME).o

clean-objs := $(SRCS:%.c=%.o)
clean-objs += $(join $(dir $(SRCS)), $(patsubst %.c, .%.o.cmd, $(notdir $(SRCS))))

include $(BUILD_PATH)/krules.mak

