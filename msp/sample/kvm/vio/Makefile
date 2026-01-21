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
SRC_PATH			:= $(CUR_PATH)
KVM_PATH			:= $(CUR_PATH)/..
SRC_RELATIVE_PATH	:= $(subst $(HOME_PATH)/,,$(SRC_PATH))
TARGET_OUT_PATH		:= $(OBJ_OUT_PATH)/$(SRC_RELATIVE_PATH)
ROOTFS_TARGET_PATH 	?= $(MSP_OUT_PATH)

LIB_PATH			:= $(OUT_PATH)/lib

################################################################################
#	set task
################################################################################
# output
MOD_NAME 		:= sample_kvm_vio
OUTPUT       	:= $(TARGET_OUT_PATH)/obj

# source
SRCS 			:= $(wildcard $(SRC_PATH)/*.c) \
				   $(wildcard $(SRC_PATH)/detect_resolution/*.c) \
				   $(wildcard $(KVM_PATH)/uxe/*.c)

SRCS := $(filter-out $(KVM_PATH)/uxe/main.c, $(SRCS))

vpath %.c $(addsuffix :,$(dir $(patsubst %.c,%.o,$(SRCS))))

CINCLUDE		+= -I$(SRC_PATH) \
                   -I$(SRC_PATH)/detect_resolution \
				   -I$(KVM_PATH)/uxe \
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

CFLAGS			+= -ldl

CFLAGS              += -D__KVM_LOG__

# dependency
CLIB			+= -Wl,-rpath-link=$(OUT_PATH)/lib
CLIB			+= -L$(OUT_PATH)/lib
CLIB			+= -lax_sys
CLIB			+= -lax_proton
CLIB			+= -lax_engine
CLIB			+= -lax_mipi
CLIB			+= -lm -lpthread
CLIB            += -lax_ivps
CLIB            += -lax_vo
CLIB            += -L$(HOME_PATH)/third-party/drm/lib
CLIB            += -ldrm

# install
INSTALL_TARGET	:= $(TARGET)
INSTALL_DIR		:= $(ROOTFS_TARGET_PATH)/bin/

# link
LINK = $(CC)

include $(HOME_PATH)/build/rules.mak

export ROOTFS_TARGET_PATH
