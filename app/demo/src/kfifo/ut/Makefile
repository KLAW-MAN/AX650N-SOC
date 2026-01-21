CUR_PATH		    := $(shell pwd)
HOME_PATH		    := $(abspath $(CUR_PATH)/../../../../..)

include $(HOME_PATH)/build/color.mk
include $(HOME_PATH)/build/config.mak

PRJ_OUT_HOME        := $(HOME_PATH)/build/out/$(PROJECT)
OBJ_OUT_PATH        := $(PRJ_OUT_HOME)/objs
MSP_OUT_PATH        := $(HOME_PATH)/msp/out
SRC_PATH            := $(CUR_PATH)
LIB_PATH            := $(MSP_OUT_PATH)/lib
INC_PATH            := $(MSP_OUT_PATH)/include
SRC_RELATIVE_PATH   := $(subst $(HOME_PATH)/,,$(SRC_PATH))
TARGET_OUT_PATH     := $(OBJ_OUT_PATH)/$(SRC_RELATIVE_PATH)
ROOTFS_TARGET_PATH  := $(MSP_OUT_PATH)
DEMO_SRC_PATH       := $(HOME_PATH)/app/demo/src

# output
MOD_NAME            := kfifo_ut
OUTPUT              := $(TARGET_OUT_PATH)/obj

SRCS                := $(wildcard $(SRC_PATH)/../*.c)
SRCCPPS             := $(wildcard $(SRC_PATH)/*.cpp)

CINCLUDE            := -I$(SRC_PATH) \
                       -I$(SRC_PATH)/.. \

OBJS                := $(SRCS:%.c=$(OUTPUT)/%.o)
CPPOBJS             := $(SRCCPPS:%.cpp=$(OUTPUT)/%.o)

DEPS                := $(OBJS:%.o=%.d)
CPPDEPS             := $(CPPOBJS:%.o=%.d)

# exec
TARGET              := $(TARGET_OUT_PATH)/$(MOD_NAME)

# build flags
CPPFLAGS            := --std=c++11
CFLAGS              := $(CPPFLAGS)
CFLAGS              += -DAPP_BUILD_VERSION=\"$(SDK_VERSION)\"

ifeq ($(debug),yes)
CFLAGS              += -O0 -ggdb3
else
CFLAGS              += -O2
endif

# dependency
CLIB                := -lstdc++ -pthread
CLIB                += -L$(LIB_PATH)

# install
INSTALL_TARGET      := $(TARGET)
INSTALL_DIR         := $(ROOTFS_TARGET_PATH)/bin/

# link
LINK = $(CC)

include $(HOME_PATH)/build/rules.mak
