MKFILE_PATH        := $(abspath $(lastword $(MAKEFILE_LIST)))
HOME_PATH          := $(abspath $(dir $(MKFILE_PATH))../../../..)
BUILD_PATH         := $(HOME_PATH)/build

include $(BUILD_PATH)/config.mak

PRV_DRV_PATH       := $(HOME_PATH)/kernel/osdrv/private_drv
OSAL_PATH          := $(HOME_PATH)/kernel/osal
OBJ_OUT_DIR        := $(BUILD_PATH)/out/$(PROJECT)/objs
KSRC               := $(abspath $(dir $(MKFILE_PATH)))
SRC_RELATIVE_PATH  := $(subst $(HOME_PATH)/,,$(KSRC))
KBUILD_DIR         := $(OBJ_OUT_DIR)/$(SRC_RELATIVE_PATH)
KBUILD_MAKEFILE    := $(KBUILD_DIR)/Makefile
ROOTFS_TARGET_PATH ?= $(HOME_PATH)/kernel/osdrv/out
KDIR	           ?= $(HOME_PATH)/kernel/linux/$(KERNEL_DIR)
GENERATED_PATH     := $(KBUILD_DIR)/generated
EXTERNAL_PATH      := $(HOME_PATH)/header/external
MSP_OUT_PATH      := $(HOME_PATH)/msp/out/include

KBUILD_EXTRA_SYMBOLS += $(OBJ_OUT_DIR)/kernel/osdrv/private_drv/sys/Module.symvers
KBUILD_EXTRA_SYMBOLS += $(OBJ_OUT_DIR)/kernel/osdrv/private_drv/isp/mod_ext/Module.symvers

export KBUILD_EXTRA_SYMBOLS

koversion = yes

MODULE_NAME := icm42670

EXTRA_CFLAGS += -I$(src)/ \
				-I$(PRV_DRV_PATH)/sys/include \
				-I$(PRV_DRV_PATH)/include \
				-I$(PRV_DRV_PATH)/mem/include \
				-I$(HOME_PATH)/header/external \
				-I$(HOME_PATH)/header/internal \
				-I$(OSAL_PATH)/include \
				-I$(GENERATED_PATH) \
				-I$(EXTERNAL_PATH) \
				-I$(MSP_OUT_PATH)

#SRCS := $(wildcard *.c)
SRCS := icm42670_driver.c \
        inv_time.c \
        imu/inv_imu_driver.c \
        imu/inv_imu_transport.c \
        imu/inv_imu_apex.c

#target:
#	@echo $(SRCS)

$(MODULE_NAME)-objs := $(SRCS:%.c=%.o)

obj-m := $(MODULE_NAME).o

clean-objs := $(SRCS:%.c=%.o)
clean-objs += $(join $(dir $(SRCS)), $(patsubst %.c, .%.o.cmd, $(notdir $(SRCS))))

include $(BUILD_PATH)/krules.mak
