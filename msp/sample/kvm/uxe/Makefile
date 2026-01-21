################################################################################
#	prepare param
################################################################################
CUR_PATH		:= $(shell pwd)
HOME_PATH		:= $(abspath $(CUR_PATH)/../../../..)

include $(HOME_PATH)/build/color.mk
include $(HOME_PATH)/build/config.mak

PRJ_OUT_HOME		:= $(HOME_PATH)/build/out/$(PROJECT)
OBJ_OUT_PATH		:= $(PRJ_OUT_HOME)/objs
MSP_OUT_PATH		:= $(HOME_PATH)/msp/out
OUT_PATH			:= $(MSP_OUT_PATH)
SRC_PATH			:= $(CUR_PATH)/
SAMPLE_PATH			:= $(CUR_PATH)/..
SRC_RELATIVE_PATH	:= $(subst $(HOME_PATH)/,,$(SRC_PATH))
TARGET_OUT_PATH		:= $(OBJ_OUT_PATH)/$(SRC_RELATIVE_PATH)
ROOTFS_TARGET_PATH 	?= $(MSP_OUT_PATH)

LIB_PATH			:= $(OUT_PATH)/lib

################################################################################
#	set task
################################################################################
# output
MOD_NAME 		:= uxe
OUTPUT       	:= $(TARGET_OUT_PATH)/obj

# source
SRCS 			:= $(wildcard $(SRC_PATH)/*.c)

vpath %.c $(addsuffix :,$(dir $(patsubst %.c,%.o,$(SRCS))))

CINCLUDE		+= -I$(SRC_PATH) \
			       -I$(OUT_PATH)/include

OBJS 			:= $(addprefix $(OUTPUT)/,$(notdir $(patsubst %.c,%.o,$(SRCS))))
DEPS			:= $(OBJS:%.o=%.d)
# exec
TARGET			:= $(TARGET_OUT_PATH)/$(MOD_NAME)

# build flags
ifeq ($(debug),yes)
CFLAGS			+= -Werror -Wall -O0 -ggdb3
else
CFLAGS			+= -Werror -Wall -O2
endif

CFLAGS          += -DAPP_BUILD_VERSION=\"$(SDK_VERSION)\"

# dependency
CLIB			+= -lpthread

# install
INSTALL_TARGET	:= $(TARGET)
INSTALL_DIR		:= $(ROOTFS_TARGET_PATH)/bin/

# link
LINK = $(CC)

include $(HOME_PATH)/build/rules.mak

export ROOTFS_TARGET_PATH
