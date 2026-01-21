MKFILE_PATH             := $(abspath $(lastword $(MAKEFILE_LIST)))
SRC_PATH                := $(abspath $(dir $(MKFILE_PATH)))
AXCL_HOME_PATH          := $(abspath $(dir $(MKFILE_PATH))../../../..)

include $(AXCL_HOME_PATH)/build/config.mak

AXCL_PCIE_PATH          := $(AXCL_HOME_PATH)/drv/pcie
OBJ_OUT_PATH            := $(AXCL_PRJ_OUT_PATH)/objs
SRC_RELATIVE_PATH       := $(subst $(AXCL_HOME_PATH)/,,$(SRC_PATH))
KBUILD_DIR              := $(OBJ_OUT_PATH)/$(SRC_RELATIVE_PATH)
KBUILD_MAKEFILE         := $(KBUILD_DIR)/Makefile

KBUILD_EXTRA_SYMBOLS    := $(OBJ_OUT_PATH)/drv/pcie/driver/host_dev/Module.symvers
export KBUILD_EXTRA_SYMBOLS

MODULE_NAME             := ax_pcie_p2p_rc

EXTRA_CFLAGS            += -I$(AXCL_PCIE_PATH)/include

ccflags-y               += -DAXCL_MODULE_NAME=\"$(MODULE_NAME)\"

SRCS                    := ax_pcie_p2p.c version.c

$(MODULE_NAME)-objs     := $(SRCS:%.c=%.o)

obj-m                   := $(MODULE_NAME).o

clean-objs              := $(SRCS:%.c=%.o)
clean-objs              += $(join $(dir $(SRCS)), $(patsubst %.c, .%.o.cmd, $(notdir $(SRCS))))

include $(AXCL_BUILD_PATH)/krules.mak
