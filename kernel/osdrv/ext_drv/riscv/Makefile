MKFILE_PATH        := $(abspath $(lastword $(MAKEFILE_LIST)))
HOME_PATH          := $(abspath $(dir $(MKFILE_PATH))../../../..)
EXT_DRV_PATH       := $(HOME_PATH)/kernel/osdrv/ext_drv
OSAL_PATH          := $(HOME_PATH)/kernel/osal
BUILD_PATH         := $(HOME_PATH)/build
ROOTFS_PATH        := $(HOME_PATH)/rootfs
EXTERNAL_PATH      := $(HOME_PATH)/header/external
GENERATED_PATH     := $(KBUILD_DIR)/generated
MSP_OUT_PATH       := $(HOME_PATH)/msp/out

include $(BUILD_PATH)/config.mak

OBJ_OUT_DIR        := $(BUILD_PATH)/out/$(PROJECT)/objs
KSRC               := $(abspath $(dir $(MKFILE_PATH)))
SRC_RELATIVE_PATH  := $(subst $(HOME_PATH)/,,$(KSRC))
KBUILD_DIR         := $(OBJ_OUT_DIR)/$(SRC_RELATIVE_PATH)
KBUILD_MAKEFILE    := $(KBUILD_DIR)/Makefile
ROOTFS_TARGET_PATH ?= $(HOME_PATH)/kernel/osdrv/out
KDIR	           ?= $(HOME_PATH)/kernel/linux/$(KERNEL_DIR)

KBUILD_EXTRA_SYMBOLS += $(OBJ_OUT_DIR)/kernel/osdrv/private_drv/mailbox/Module.symvers
export KBUILD_EXTRA_SYMBOLS

KDIR = $(HOME_PATH)/kernel/linux/$(KERNEL_DIR)

MODULE_NAME := ax_riscv

EXTRA_CFLAGS +=	-I$(EXT_DRV_PATH)/riscv \
		-I$(EXTERNAL_PATH) \
		-I$(GENERATED_PATH) \
		-I$(MSP_OUT_PATH)/include \
		-I$(OSAL_PATH)/include

GENERATED_PATH     := $(KBUILD_DIR)/generated
koversion = yes
EXTRA_CFLAGS += -I$(GENERATED_PATH) -Werror

# for debug
KCFLAGS	+=-DDEBUG

SRCS := ax_riscv_drv.c ax_riscv_debug.c ax_riscv_utils.c
SRCS += sync_signal/ax_sync_signal.c

$(MODULE_NAME)-objs := $(SRCS:%.c=%.o)

obj-m := $(MODULE_NAME).o

clean-objs := $(SRCS:%.c=%.o)
clean-objs += $(join $(dir $(SRCS)), $(patsubst %.c, .%.o.cmd, $(notdir $(SRCS))))

include $(BUILD_PATH)/krules.mak
