MKFILE_PATH               := $(abspath $(lastword $(MAKEFILE_LIST)))
CUR_PATH                  := $(abspath $(dir $(MKFILE_PATH)))
SRC_PATH                  := $(CUR_PATH)
AXCL_HOME_PATH            := $(abspath $(CUR_PATH)/../../..)

include $(AXCL_HOME_PATH)/build/config.mak

OUT_PATH                  := $(AXCL_OUT_PATH)
OBJ_OUT_PATH              := $(AXCL_PRJ_OUT_PATH)/objs
SRC_RELATIVE_PATH         := $(subst $(AXCL_HOME_PATH)/,,$(SRC_PATH))
TARGET_OUT_PATH           := $(OBJ_OUT_PATH)/$(SRC_RELATIVE_PATH)

AXCL_LITE_PATH            := $(AXCL_HOME_PATH)/sample/axclite
AXCL_PPL_PATH             := $(AXCL_HOME_PATH)/sample/ppl

AXCL_HEADER_INTERNAL_PATH := $(AXCL_HOME_PATH)/include/internal

# output
MOD_NAME                  := axcl_ppl
OUTPUT                    := $(TARGET_OUT_PATH)/.obj

# source
SRCS                      :=
SRCCPPS                   := $(wildcard $(SRC_PATH)/*.cpp) \
                             $(wildcard $(SRC_PATH)/*/*.cpp) \
                             $(wildcard $(AXCL_HOME_PATH)/threadx.cpp) \
                             $(wildcard $(AXCL_HOME_PATH)/event.cpp)

CINCLUDE                  := -I$(SRC_PATH) \
                             -I$(SRC_PATH)/transcode \
                             -I$(SRC_PATH)/hvcp \
                             -I$(AXCL_PPL_PATH)/include \
                             -I$(AXCL_LITE_PATH)/include \
                             -I$(AXCL_LITE_PATH)/msys \
                             -I$(AXCL_LITE_PATH)/ivps \
                             -I$(AXCL_LITE_PATH)/vdec \
                             -I$(AXCL_LITE_PATH)/venc \
                             -I$(AXCL_INC_PATH) \
                             -I$(AXCL_HEADER_INTERNAL_PATH) \
                             -I$(AXCL_HOME_PATH)/toolkit \
                             -I$(AXCL_HOME_PATH)/logger \
                             -I$(AXCL_HOME_PATH)/3rdparty/spdlog/$(ARCH)/include

OBJS                      := $(SRCCPPS:%.cpp=$(OUTPUT)/%.o)
OBJS                      += $(SRCS:%.c=$(OUTPUT)/%.o)
DEPS                      := $(OBJS:%.o=%.d)

# dynamic lib version (must)
SONAME                    := lib$(MOD_NAME).so
STRIPPED_TARGET           := $(SONAME)
DEBUG_TARGET              := lib$(MOD_NAME).debug

# build flags
CPPFLAGS                  := --std=c++17
CFLAGS                    := $(DYNAMIC_FLAG)
CFLAGS                    += -Werror -Wunused-function
CFLAGS                    += -fvisibility=hidden
CFLAGS                    += -DAXCL_BUILD_VERSION=\"$(SDK_VERSION)\"
CFLAGS                    += -DAXCL_MODULE_NAME=\"$(SONAME)\"

CFLAGS                    += -Wall -O2 -ggdb3

# dependency
CLIB                      := -lstdc++ -pthread
CLIB                      += -L$(AXCL_LIB_PATH) -laxcl_lite -laxcl_rt -laxcl_logger

# install
INSTALL_LIB               := $(STRIPPED_TARGET) $(DEBUG_TARGET)
MV_TARGET                 := $(INSTALL_LIB)

# link
LINK = $(CC)

include $(AXCL_BUILD_PATH)/rules.mak
