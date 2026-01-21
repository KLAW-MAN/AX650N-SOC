MKFILE_PATH             := $(abspath $(lastword $(MAKEFILE_LIST)))
SRC_PATH                := $(abspath $(dir $(MKFILE_PATH)))
AXCL_HOME_PATH          := $(abspath $(dir $(MKFILE_PATH))../../../..)

include $(AXCL_HOME_PATH)/build/config.mak

AXCL_PCIE_PATH          := $(AXCL_HOME_PATH)/drv/pcie
OBJ_OUT_PATH            := $(AXCL_PRJ_OUT_PATH)/objs
SRC_RELATIVE_PATH       := $(subst $(AXCL_HOME_PATH)/,,$(SRC_PATH))
KBUILD_DIR              := $(OBJ_OUT_PATH)/$(SRC_RELATIVE_PATH)
KBUILD_MAKEFILE         := $(KBUILD_DIR)/Makefile

MODULE_NAME             := ax_pcie_host_dev

EXTRA_CFLAGS            += -I$(AXCL_PCIE_PATH)/include \
                           -I$(KERNEL_DIR)/fs/proc

ccflags-y               += -DAXCL_MODULE_NAME=\"$(MODULE_NAME)\"
ccflags-y               += -DAXCL_HOST

SRCS                    := ax_pcie_dev_host.c \
                           ax_pcie_opt.c \
                           ax_pcie_proc.c \
                           ax_pcie_msg_transfer.c \
                           version.c

$(MODULE_NAME)-objs     := $(SRCS:%.c=%.o)

obj-m                   := $(MODULE_NAME).o

clean-objs              := $(SRCS:%.c=%.o)
clean-objs              += $(join $(dir $(SRCS)), $(patsubst %.c, .%.o.cmd, $(notdir $(SRCS))))

include $(AXCL_BUILD_PATH)/krules.mak
