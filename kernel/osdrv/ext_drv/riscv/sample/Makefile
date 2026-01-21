MKFILE_PATH        := $(abspath $(lastword $(MAKEFILE_LIST)))
HOME_PATH          := $(abspath $(dir $(MKFILE_PATH))../../../../../)
EXT_DRV_PATH       := $(HOME_PATH)/kernel/osdrv/ext_drv
OSAL_PATH          := $(HOME_PATH)/kernel/osal
BUILD_PATH         := $(HOME_PATH)/build

include $(BUILD_PATH)/config.mak

OBJ_OUT_DIR        := $(BUILD_PATH)/out/$(PROJECT)/objs
KSRC               := $(abspath $(dir $(MKFILE_PATH)))
SRC_RELATIVE_PATH  := $(subst $(HOME_PATH)/,,$(KSRC))
KBUILD_DIR         := $(OBJ_OUT_DIR)/$(SRC_RELATIVE_PATH)
KBUILD_MAKEFILE    := $(KBUILD_DIR)/Makefile
ROOTFS_TARGET_PATH ?= $(HOME_PATH)/kernel/osdrv/out
KDIR	           ?= $(HOME_PATH)/kernel/linux/$(KERNEL_DIR)
EXTERNAL_PATH       := $(HOME_PATH)/header/external
INTERNAL_PATH       := $(HOME_PATH)/header/internal
GENERATED_PATH      := $(KBUILD_DIR)/generated

KBUILD_EXTRA_SYMBOLS += $(OBJ_OUT_DIR)/kernel/osdrv/ext_drv/riscv/Module.symvers
export KBUILD_EXTRA_SYMBOLS

KDIR = $(HOME_PATH)/kernel/linux/$(KERNEL_DIR)

MODULE_NAME := riscv_sample

EXTRA_CFLAGS += -I$(EXT_DRV_PATH)/include \
		-I$(EXT_DRV_PATH)/riscv \
		-I$(EXT_DRV_PATH)/sys/include \
		-I$(EXTERNAL_PATH) \
		-I$(INTERNAL_PATH) \
		-I$(GENERATED_PATH) \
		-I$(OSAL_PATH)/include \
		-I$(HOME_PATH)/msp/out/include

GENERATED_PATH     := $(KBUILD_DIR)/generated
koversion = yes
EXTRA_CFLAGS += -I$(GENERATED_PATH) -Werror

# for debug
KCFLAGS	+=-DDEBUG

SRCS := ax_riscv_sample.c


$(MODULE_NAME)-objs := $(SRCS:%.c=%.o)

obj-m := $(MODULE_NAME).o

clean-objs := $(SRCS:%.c=%.o)
clean-objs += $(join $(dir $(SRCS)), $(patsubst %.c, .%.o.cmd, $(notdir $(SRCS))))

include $(BUILD_PATH)/krules.mak
