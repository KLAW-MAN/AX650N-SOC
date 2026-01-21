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
KDIR                ?= $(HOME_PATH)/kernel/linux/$(KERNEL_DIR)
ROOTFS_TARGET_PATH ?= $(HOME_PATH)/kernel/osdrv/out

PRV_DRV_PATH       := $(HOME_PATH)/kernel/osdrv/private_drv
OSAL_PATH          := $(HOME_PATH)/kernel/osal
EXTERNAL_PATH       := $(HOME_PATH)/header/external
FTC_DRV_PATH      := $(HOME_PATH)/kernel/osdrv/ext_drv/ftc_demo
GENERATED_PATH      := $(KBUILD_DIR)/generated
RISCV_PATH          := $(HOME_PATH)/kernel/osdrv/ext_drv/riscv/sync_signal

#KBUILD_EXTRA_SYMBOLS += $(OBJ_OUT_DIR)/kernel/osal/linux/kernel/Module.symvers
KBUILD_EXTRA_SYMBOLS += $(OBJ_OUT_DIR)/kernel/osdrv/private_drv/pool/Module.symvers
KBUILD_EXTRA_SYMBOLS += $(OBJ_OUT_DIR)/kernel/osdrv/ext_drv/riscv/Module.symvers
KBUILD_EXTRA_SYMBOLS += $(OBJ_OUT_DIR)/kernel/osdrv/private_drv/sys/Module.symvers
KBUILD_EXTRA_SYMBOLS += $(OBJ_OUT_DIR)/kernel/osdrv/private_drv/mipi_rx/Module.symvers
KBUILD_EXTRA_SYMBOLS += $(OBJ_OUT_DIR)/kernel/osdrv/private_drv/isp/Module.symvers
export KBUILD_EXTRA_SYMBOLS

koversion=yes
MODULE_NAME     := ax_ftc

EXTRA_CFLAGS += -I$(EXTERNAL_PATH) \
		-I$(OSAL_PATH)/include \
		-I$(PRV_DRV_PATH)/include \
		-I$(PRV_DRV_PATH)/sys/include \
		-I$(FTC_DRV_PATH)/include_priv \
		-I$(COMMON_PATH)/include \
                -I$(GENERATED_PATH) \
                -I$(RISCV_PATH) \
	        -I$(MSP_OUT_PATH)/include \

EXTRA_CFLAGS += -Werror

# for debug
KCFLAGS	+=-DDEBUG

SRCS := src/ax_ftc_driver.c \
		src/ax_ftc_process.c \
		src/ax_ftc_extern.c \
		src/ax_ftc_event.c \
		src/ax_sif_ftc.c \




$(MODULE_NAME)-objs := $(SRCS:%.c=%.o)

obj-m := $(MODULE_NAME).o

clean-objs := $(SRCS:%.c=%.o)
clean-objs += $(join $(dir $(SRCS)), $(patsubst %.c, .%.o.cmd, $(notdir $(SRCS))))

include $(BUILD_PATH)/krules.mak

