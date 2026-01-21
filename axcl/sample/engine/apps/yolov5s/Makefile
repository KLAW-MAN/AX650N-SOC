MKFILE_PATH                 := $(abspath $(lastword $(MAKEFILE_LIST)))
CUR_PATH                    := $(abspath $(dir $(MKFILE_PATH)))
SRC_PATH                    := $(CUR_PATH)
AXCL_HOME_PATH              := $(abspath $(CUR_PATH)/../../../..)
HOME_PATH                   := $(abspath $(AXCL_HOME_PATH)/..)

include $(AXCL_HOME_PATH)/build/config.mak

OUT_PATH					:= $(AXCL_OUT_PATH)
OBJ_OUT_PATH				:= $(AXCL_PRJ_OUT_PATH)/objs
SRC_RELATIVE_PATH			:= $(subst $(AXCL_HOME_PATH)/,,$(SRC_PATH))
TARGET_OUT_PATH				:= $(OBJ_OUT_PATH)/$(SRC_RELATIVE_PATH)
THIRD_PATH					:= $(HOME_PATH)/third-party

# output
MOD_NAME					:= axcl_sample_yolov5s
OUTPUT						:= $(TARGET_OUT_PATH)/.obj

# source
SRCCPPS						:= $(wildcard $(SRC_PATH)/*.cc)

CINCLUDE					:= -I$(SRC_PATH) \
                               -I$(SRC_PATH)/../.. \
                               -I$(AXCL_INC_PATH) \
                               -I$(AXCL_HOME_PATH)/3rdparty/cmdline/include \
                               -I$(THIRD_PATH)/opencv-4.5.5/include/opencv4

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
CLIB						:= -lstdc++ -pthread -lm
CLIB						+= -Wl,-rpath-link=$(AXCL_LIB_PATH)
CLIB						+= -L$(AXCL_LIB_PATH)
CLIB						+= -L$(THIRD_PATH)/opencv-4.5.5/lib
CLIB						+= -L$(THIRD_PATH)/opencv-4.5.5/lib/opencv4/3rdparty/
CLIB						+= -Wl,--start-group -lopencv_core -lopencv_imgproc -lopencv_imgcodecs -ltegra_hal \
                               -llibjpeg-turbo -llibwebp -llibpng -llibtiff -llibopenjp2 \
                               -littnotify -lpthread -lzlib  -ldl -lm -lrt -Wl,--end-group
CLIB						+= -laxcl_rt

# install
INSTALL_TARGET				:= $(TARGET)
INSTALL_DIR					:= $(MOD_TARGET_PATH)/bin/

# link
LINK = $(CC)

include $(AXCL_BUILD_PATH)/rules.mak
