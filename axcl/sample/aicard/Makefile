MKFILE_PATH               := $(abspath $(lastword $(MAKEFILE_LIST)))
CUR_PATH                  := $(abspath $(dir $(MKFILE_PATH)))
SRC_PATH                  := $(CUR_PATH)
HOME_PATH                 := $(abspath $(CUR_PATH)/../../..)

include $(HOME_PATH)/axcl/build/config.mak

FFMPEG_LIB_PATH           := $(AXCL_LIB_PATH)/ffmpeg
FFMPEG_INC_PATH           := $(AXCL_HOME_PATH)/3rdparty/ffmpeg/$(ARCH)/include

THIRD_PARTY_PATH          := $(HOME_PATH)/third-party

MSP_INC_PATH              := $(HOME_PATH)/msp/out/include
MSP_LIB_PATH              := $(HOME_PATH)/msp/out/lib

OUT_PATH                  := $(AXCL_OUT_PATH)
OBJ_OUT_PATH              := $(AXCL_PRJ_OUT_PATH)/objs
SRC_RELATIVE_PATH         := $(subst $(AXCL_HOME_PATH)/,,$(SRC_PATH))
TARGET_OUT_PATH           := $(OBJ_OUT_PATH)/$(SRC_RELATIVE_PATH)
DEMO_BIN_PATH             := $(AXCL_HOME_PATH)/sample/aicard/bin

# output
MOD_NAME                  := axcl_demo
OUTPUT                    := $(TARGET_OUT_PATH)/.obj

# source
SRCS                      :=
SRCCPPS                   := $(wildcard $(SRC_PATH)/*.cpp) \
                             $(wildcard $(AXCL_HOME_PATH)/sample/aicard/component/capture/*.cpp) \
                             $(wildcard $(AXCL_HOME_PATH)/sample/aicard/component/config/ini/*.cpp) \
                             $(wildcard $(AXCL_HOME_PATH)/sample/aicard/component/detect/*.cpp) \
                             $(wildcard $(AXCL_HOME_PATH)/sample/aicard/component/dispatch/*.cpp) \
                             $(wildcard $(AXCL_HOME_PATH)/sample/aicard/component/ivps/*.cpp) \
                             $(wildcard $(AXCL_HOME_PATH)/sample/aicard/component/log/*.cpp) \
                             $(wildcard $(AXCL_HOME_PATH)/sample/aicard/component/pool/AXPoolManager.cpp) \
                             $(wildcard $(AXCL_HOME_PATH)/sample/aicard/component/transfer/*.cpp) \
                             $(wildcard $(AXCL_HOME_PATH)/sample/aicard/component/utils/ElapsedTimer.cpp) \
                             $(wildcard $(AXCL_HOME_PATH)/sample/aicard/component/vdec/*.cpp) \
                             $(wildcard $(AXCL_HOME_PATH)/sample/aicard/component/vdec/stream/FileStreamer.cpp) \
                             $(wildcard $(AXCL_HOME_PATH)/sample/aicard/component/vo/*.cpp) \
                             $(wildcard $(AXCL_HOME_PATH)/sample/aicard/component/yuv/*.cpp)

SRCCPPS                   += $(AXCL_HOME_PATH)/toolkit/elapser.cpp

SRCS                      += $(AXCL_HOME_PATH)/toolkit/axcl_fifo.c

CINCLUDE                  := -I$(SRC_PATH) \
                             -I$(MSP_INC_PATH) \
                             -I$(AXCL_INC_PATH) \
                             -I$(AXCL_HOME_PATH)/sample \
                             -I$(AXCL_HOME_PATH)/toolkit \
                             -I$(FFMPEG_INC_PATH) \
                             -I$(AXCL_HOME_PATH)/3rdparty/cmdline/include \
                             -I$(AXCL_HOME_PATH)/sample/aicard/component/capture \
                             -I$(AXCL_HOME_PATH)/sample/aicard/component/config/ini \
                             -I$(AXCL_HOME_PATH)/sample/aicard/component/detect \
                             -I$(AXCL_HOME_PATH)/sample/aicard/component/dispatch \
                             -I$(AXCL_HOME_PATH)/sample/aicard/component/header \
                             -I$(AXCL_HOME_PATH)/sample/aicard/component/ivps \
                             -I$(AXCL_HOME_PATH)/sample/aicard/component/log \
                             -I$(AXCL_HOME_PATH)/sample/aicard/component/pool \
                             -I$(AXCL_HOME_PATH)/sample/aicard/component/transfer \
                             -I$(AXCL_HOME_PATH)/sample/aicard/component/utils \
                             -I$(AXCL_HOME_PATH)/sample/aicard/component/vdec \
                             -I$(AXCL_HOME_PATH)/sample/aicard/component/vdec/stream \
                             -I$(AXCL_HOME_PATH)/sample/aicard/component/vo \
                             -I$(AXCL_HOME_PATH)/sample/aicard/component/yuv

OBJS                      := $(SRCS:%.c=$(OUTPUT)/%.o)
CPPOBJS                   := $(SRCCPPS:%.cpp=$(OUTPUT)/%.o)

DEPS                      := $(OBJS:%.o=%.d)
CPPDEPS                   := $(CPPOBJS:%.o=%.d)

# exec
TARGET                    := $(TARGET_OUT_PATH)/$(MOD_NAME)

# build flags
CPPFLAGS                  := --std=c++17
CFLAGS                    += -DAXCL_BUILD_VERSION=\"$(SDK_VERSION)\"
CFLAGS                    += -D__RTSP_UNSUPPORT__

# Independant debug
# CFLAGS                    += -D__HOST_DEBUG__

ifeq ($(debug),yes)
CFLAGS                    += -Wall -O0 -ggdb3
else
CFLAGS                    += -Wall -O2
endif


# dependency
CLIB                      := -lstdc++ -pthread -lm
CLIB                      += -Wl,-rpath-link=$(AXCL_LIB_PATH):$(MSP_LIB_PATH):$(FFMPEG_LIB_PATH)
CLIB                      += -L$(FFMPEG_LIB_PATH) -lavcodec -lavutil -lavformat
CLIB                      += -L$(MSP_LIB_PATH) -lax_sys -lax_vdec -lax_ivps -lax_vo
CLIB                      += -L$(AXCL_LIB_PATH) -laxcl_rt -laxcl_pkg -laxcl_sys -laxcl_vdec -laxcl_venc -laxcl_ivps -laxcl_npu -laxcl_skel
CLIB                      += -L$(THIRD_PARTY_PATH)/drm/lib -ldrm

# install
INSTALL_TARGET            := $(TARGET)
INSTALL_TARGET            += $(DEMO_BIN_PATH)/*
INSTALL_DIR               := $(MOD_TARGET_PATH)/bin/axcl_demo

# link
LINK = $(CC)

include $(AXCL_BUILD_PATH)/rules.mak
