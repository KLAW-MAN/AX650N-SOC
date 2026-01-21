MKFILE_PATH               := $(abspath $(lastword $(MAKEFILE_LIST)))
CUR_PATH                  := $(abspath $(dir $(MKFILE_PATH)))
SRC_PATH                  := $(CUR_PATH)
AXCL_HOME_PATH            := $(abspath $(CUR_PATH)/../..)
HOME_PATH                 := $(abspath $(AXCL_HOME_PATH)/..)

include $(AXCL_HOME_PATH)/build/config.mak

FFMPEG_LIB_PATH           := $(AXCL_LIB_PATH)/ffmpeg
FFMPEG_INC_PATH           := $(AXCL_HOME_PATH)/3rdparty/ffmpeg/$(ARCH)/include
SPDLOG_LIB_PATH           := $(AXCL_HOME_PATH)/3rdparty/spdlog/$(ARCH)/lib
SPDLOG_INC_PATH           := $(AXCL_HOME_PATH)/3rdparty/spdlog/$(ARCH)/include

OUT_PATH                  := $(AXCL_OUT_PATH)
OBJ_OUT_PATH              := $(AXCL_PRJ_OUT_PATH)/objs
SRC_RELATIVE_PATH         := $(subst $(AXCL_HOME_PATH)/,,$(SRC_PATH))
TARGET_OUT_PATH           := $(OBJ_OUT_PATH)/$(SRC_RELATIVE_PATH)

MSP_LIB_PATH              := $(HOME_PATH)/msp/out/lib

# output
MOD_NAME                  := axcl_sample_vdec
OUTPUT                    := $(TARGET_OUT_PATH)/.obj

# source
SRCS                      := $(wildcard $(AXCL_HOME_PATH)/toolkit/axcl_fifo.c)
SRCCPPS                   := $(wildcard $(SRC_PATH)/*.cpp) \
                             $(wildcard $(AXCL_HOME_PATH)/sample/sys/sample_sys.cpp) \
                             $(wildcard $(AXCL_HOME_PATH)/sample/demux/ffmpeg.cpp) \
                             $(wildcard $(AXCL_HOME_PATH)/toolkit/threadx.cpp) \
                             $(wildcard $(AXCL_HOME_PATH)/toolkit/event.cpp) \
                             $(wildcard $(AXCL_HOME_PATH)/toolkit/elapser.cpp) \
                             $(wildcard $(AXCL_HOME_PATH)/toolkit/dma_buffer.cpp)

CINCLUDE                  := -I$(SRC_PATH) \
                             -I$(AXCL_INC_PATH) \
                             -I$(AXCL_HOME_PATH)/toolkit \
                             -I$(AXCL_HOME_PATH)/logger \
                             -I$(AXCL_HOME_PATH)/sample \
                             -I$(FFMPEG_INC_PATH) \
                             -I$(AXCL_HOME_PATH)/3rdparty/cmdline/include \
                             -I$(SPDLOG_INC_PATH)

OBJS                      := $(SRCS:%.c=$(OUTPUT)/%.o)
CPPOBJS                   := $(SRCCPPS:%.cpp=$(OUTPUT)/%.o)

DEPS                      := $(OBJS:%.o=%.d)
CPPDEPS                   := $(CPPOBJS:%.o=%.d)

# exec
TARGET                    := $(TARGET_OUT_PATH)/$(MOD_NAME)

# build flags
CPPFLAGS                  := --std=c++17
CFLAGS                    += -DAXCL_BUILD_VERSION=\"$(SDK_VERSION)\"

ifeq ($(debug),yes)
CFLAGS                    += -Wall -O0 -ggdb3
else
CFLAGS                    += -Wall -O2
endif

# dependency
CLIB                      := -lstdc++ -pthread -lm
ifeq ($(HOST),ax650)
CLIB                      += -Wl,-rpath-link=$(AXCL_LIB_PATH):$(FFMPEG_LIB_PATH):$(MSP_LIB_PATH)
CLIB                      += -L$(MSP_LIB_PATH) -lax_sys
else
CLIB                      += -Wl,-rpath-link=$(AXCL_LIB_PATH):$(FFMPEG_LIB_PATH)
endif
CLIB                      += -L$(FFMPEG_LIB_PATH) -lavcodec -lavutil -lavformat
CLIB                      += -L$(AXCL_LIB_PATH) -laxcl_sys -laxcl_vdec -laxcl_rt -laxcl_logger
CLIB                      += -L$(SPDLOG_LIB_PATH) -lspdlog

# install
INSTALL_TARGET            := $(TARGET)
INSTALL_DIR               := $(MOD_TARGET_PATH)/bin/

# link
LINK = $(CC)

include $(AXCL_BUILD_PATH)/rules.mak
