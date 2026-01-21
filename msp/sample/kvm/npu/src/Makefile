CUR_PATH            := $(shell pwd)
HOME_PATH           := $(abspath $(CUR_PATH)/../../../../..)

include $(HOME_PATH)/build/config.mak

SRC_PATH            := $(CUR_PATH)
PRJ_OUT_HOME        := $(HOME_PATH)/build/out/$(PROJECT)
OBJ_OUT_PATH     	:= $(PRJ_OUT_HOME)/objs
SRC_RELATIVE_PATH   := $(subst $(HOME_PATH)/,,$(SRC_PATH))
TARGET_OUT_PATH     := $(OBJ_OUT_PATH)/$(SRC_RELATIVE_PATH)
MSP_OUT_PATH        := $(HOME_PATH)/msp/out
MSP_LIB_PATH        := $(MSP_OUT_PATH)/lib
INC_PATH            := $(MSP_OUT_PATH)/include
ROOTFS_TARGET_PATH 	:= $(MSP_OUT_PATH)


MOD_NAME             = sample_kvm_npu
OUTPUT 				:= $(TARGET_OUT_PATH)/obj
# source
SRCS                :=
SRCCPPS             := $(wildcard $(SRC_PATH)/*.cpp) \
					   $(wildcard $(SRC_PATH)/runner/ax_model_runner/*.cpp) \
					   $(wildcard $(SRC_PATH)/runner/utils/*.cpp) \
					   $(wildcard $(SRC_PATH)/runner/vdsp/*.cpp) \


CINCLUDE            := -I$(SRC_PATH) \
                       -I$(INC_PATH) \
					   -I$(SRC_PATH)/runner \
					   -I$(SRC_PATH)/runner/ax_model_runner \
					   -I$(SRC_PATH)/runner/utils \
					   -I$(SRC_PATH)/runner/vdsp

 OBJS               := $(SRCS:%.c=$(OUTPUT)/%.o)
 CPPOBJS            := $(SRCCPPS:%.cpp=$(OUTPUT)/%.o)

 DEPS               := $(OBJS:%.o=%.d)
 CPPDEPS            := $(CPPOBJS:%.o=%.d)

# exec
TARGET				:= $(TARGET_OUT_PATH)/$(MOD_NAME)

# build flags
CPPFLAGS            := --std=c++17
CFLAGS				:= $(CPPFLAGS) -Wall -Wl,-rpath-link=$(MSP_LIB_PATH)

ifeq ($(debug),yes)
CFLAGS				+= -O0 -ggdb3
else
CFLAGS				+= -O2
endif

# dependency
CLIB				:= -lstdc++
CLIB				+= -L$(MSP_LIB_PATH) -lax_sys -lax_engine -lax_dsp -lax_dsp_cv -lax_kvm_dsp_cv

# install
INSTALL_TARGET		:= $(TARGET)
INSTALL_DIR			:= $(ROOTFS_TARGET_PATH)/bin

# link
LINK = $(CC)

include $(HOME_PATH)/build/rules.mak
