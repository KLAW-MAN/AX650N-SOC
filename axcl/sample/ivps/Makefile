################################################################################
#	prepare param
################################################################################
MKFILE_PATH             := $(abspath $(lastword $(MAKEFILE_LIST)))
CUR_PATH                := $(abspath $(dir $(MKFILE_PATH)))
SRC_PATH                := $(CUR_PATH)
AXCL_HOME_PATH          := $(abspath $(CUR_PATH)/../..)

include $(AXCL_HOME_PATH)/build/config.mak

OUT_PATH                := $(AXCL_OUT_PATH)
OBJ_OUT_PATH            := $(AXCL_PRJ_OUT_PATH)/objs
SRC_RELATIVE_PATH       := $(subst $(AXCL_HOME_PATH)/,,$(SRC_PATH))
TARGET_OUT_PATH         := $(OBJ_OUT_PATH)/$(SRC_RELATIVE_PATH)
UTILS_PATH              := $(AXCL_HOME_PATH)/sample/utils

################################################################################
#	set task
################################################################################
MOD_NAME                := axcl_sample_ivps
OUTPUT                  := $(TARGET_OUT_PATH)/.obj

# source
SRCS                    := $(wildcard $(SRC_PATH)/*.c) \
                           $(wildcard $(SRC_PATH)/tools/*.c) \
                           $(wildcard $(SRC_PATH)/tools/ini_parser/*.c) \
                           $(wildcard $(SRC_PATH)/sync_api/*.c)


CINCLUDE                := -I$(SRC_PATH) \
                           -I$(AXCL_INC_PATH) \
                           -I$(AXCL_HOME_PATH)/sample \
                           -I$(AXCL_HOME_PATH)/utils \
                           -I$(AXCL_HOME_PATH)/toolkit \
                           -I$(SRC_PATH)/tools \
                           -I$(SRC_PATH)/tools/ini_parser \
                           -I$(SRC_PATH)/sync_api \
                           -I$(SRC_PATH)/data


OBJS                    := $(SRCS:%.c=$(OUTPUT)/%.o)

DEPS                    := $(OBJS:%.o=%.d)
# exec
TARGET                  := $(TARGET_OUT_PATH)/$(MOD_NAME)

# build flags
CFLAGS                  := -Werror -Wall -Wl,-rpath-link=$(LIB_PATH)
CFLAGS                  += -DAXCL_BUILD_VERSION=\"$(SDK_VERSION)\"

# build flags
ifeq ($(debug),yes)
CFLAGS                  += -Wall -O0 -ggdb3 -Wno-unused
else
CFLAGS                  += -Wall -O2 -Wno-unused
endif

# dependency
CLIB                    := -lstdc++ -pthread -lm
CLIB                    += -Wl,-rpath-link=$(AXCL_LIB_PATH)
CLIB                    += -L$(AXCL_LIB_PATH) -laxcl_sys -laxcl_ivps -laxcl_vdec -laxcl_rt

# install
INSTALL_TARGET          := $(TARGET)
INSTALL_DIR             := $(MOD_TARGET_PATH)/bin/

INSTALL_DATA            := $(CUR_PATH)/data/ivps
INSTALL_DATA            += $(CUR_PATH)/config
# link
LINK = $(CC)

include $(AXCL_BUILD_PATH)/rules.mak
