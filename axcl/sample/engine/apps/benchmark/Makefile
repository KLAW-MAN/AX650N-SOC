MKFILE_PATH                 := $(abspath $(lastword $(MAKEFILE_LIST)))
CUR_PATH                    := $(abspath $(dir $(MKFILE_PATH)))
SRC_PATH                    := $(CUR_PATH)
AXCL_HOME_PATH              := $(abspath $(CUR_PATH)/../../../..)

include $(AXCL_HOME_PATH)/build/config.mak

OUT_PATH					:= $(AXCL_OUT_PATH)
OBJ_OUT_PATH				:= $(AXCL_PRJ_OUT_PATH)/objs
SRC_RELATIVE_PATH			:= $(subst $(AXCL_HOME_PATH)/,,$(SRC_PATH))
TARGET_OUT_PATH				:= $(OBJ_OUT_PATH)/$(SRC_RELATIVE_PATH)

# output
MOD_NAME					:= axcl_run_model
OUTPUT						:= $(TARGET_OUT_PATH)/.obj

# source
SRCCPPS						:= $(wildcard $(SRC_PATH)/*.cc) \
                               $(wildcard $(SRC_PATH)/../../middleware/*.cc)

CINCLUDE					:= -I$(SRC_PATH) \
                               -I$(SRC_PATH)/../.. \
                               -I$(AXCL_INC_PATH) \
                               -I$(AXCL_HOME_PATH)/3rdparty/cmdline/include

OBJS						:= $(SRCS:%.c=$(OUTPUT)/%.o)
CPPOBJS						:= $(SRCCPPS:%.cc=$(OUTPUT)/%.o)

DEPS						:= $(OBJS:%.o=%.d)
CPPDEPS						:= $(CPPOBJS:%.o=%.d)

# exec
TARGET						:= $(TARGET_OUT_PATH)/$(MOD_NAME)

# build flags
CPPFLAGS					:= --std=c++17
CFLAGS						+= $(CPPFLAGS)
CFLAGS						+= -DAXCL_BUILD_VERSION=\"$(SDK_VERSION)\"
CFLAGS						+= -DENV_AXCL_RUNTIME_API_ENABLE
CFLAGS						+= -DENV_AXCL_NATIVE_API_ENABLE
CFLAGS						+= -DENV_CHIP_SERIES_MC50
CFLAGS						+= -DENV_HAS_STD_FILESYSTEM
CFLAGS						+= -DENV_HAS_POSIX_FILE_STAT

ifeq ($(debug),yes)
CFLAGS						+= -Wall -O0 -ggdb3
else
CFLAGS						+= -Wall -O2
endif

# dependency
CLIB						:= -lstdc++ -pthread
CLIB						+= -Wl,-rpath-link=$(AXCL_LIB_PATH)
CLIB						+= -L$(AXCL_LIB_PATH) -laxcl_rt -laxcl_npu

# install
INSTALL_TARGET				:= $(TARGET)
INSTALL_DIR					:= $(MOD_TARGET_PATH)/bin/

# link
LINK = $(CC)

include $(AXCL_BUILD_PATH)/rules.mak
