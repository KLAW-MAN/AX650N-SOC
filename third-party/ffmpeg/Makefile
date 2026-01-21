MKFILE_PATH         := $(abspath $(lastword $(MAKEFILE_LIST)))
CUR_PATH            := $(abspath $(dir $(MKFILE_PATH)))
HOME_PATH           := $(abspath $(CUR_PATH)/../..)
ROOTFS_TARGET_PATH ?= $(HOME_PATH)/rootfs/rootfs/usr/local
MSP_OUT_PATH		:= $(HOME_PATH)/msp/out

HOST                := ax650

CROSS	     := aarch64-none-linux-gnu-
CC            = $(CROSS)gcc
CPP           = $(CROSS)g++
LD            = $(CROSS)ld
AR            = $(CROSS)ar -rcs
OBJCPOY       = $(CROSS)objcopy
STRIP         = $(CROSS)strip

VERB          = @
RM            = rm -rf
MKDIR         = mkdir -p
ECHO          = echo
MV            = mv
LN            = ln -sf
CP            = cp -f
TAR           = tar
TOUCH         = touch
ARCH          = arm64
STATIC_FLAG  := -fPIC
DYNAMIC_FLAG := -shared -fPIC


INC_PATH       := $(MSP_OUT_PATH)/include


BUILD_SCRIPT = ./build.sh


FFMPEG_OUT_PATH=$(CUR_PATH)/out


ifeq ($(debug),yes)
DBG=yes
else
DBG=
endif

FFMPEG_SRC_PATH=$(CUR_PATH)/FFmpeg-n7.1

.PHONY: install clean all

all: clean
	@$(MKDIR) $(FFMPEG_OUT_PATH)
	@$(BUILD_SCRIPT) dbg=$(DBG) cross=$(CROSS) axcl_inc=$(INC_PATH) out=$(FFMPEG_OUT_PATH) host=$(HOST) src=$(FFMPEG_SRC_PATH)
	@$(ECHO) -e $(GREEN)"\nBuild ffmpeg success!!\n"  $(DONE)

	@$(CP) -rf $(FFMPEG_OUT_PATH)/include $(CUR_PATH)/
	@$(CP) -d $(FFMPEG_OUT_PATH)/lib/lib* $(CUR_PATH)/lib

install:
	@$(MKDIR) $(ROOTFS_TARGET_PATH)/lib
	@$(CP) -d $(CUR_PATH)/lib/libavcodec.so* $(ROOTFS_TARGET_PATH)/lib
	@$(CP) -d $(CUR_PATH)/lib/libavfilter.so* $(ROOTFS_TARGET_PATH)/lib
	@$(CP) -d $(CUR_PATH)/lib/libavformat.so* $(ROOTFS_TARGET_PATH)/lib
	@$(CP) -d $(CUR_PATH)/lib/libavutil.so* $(ROOTFS_TARGET_PATH)/lib
	@$(CP) -d $(CUR_PATH)/lib/libswresample.so* $(ROOTFS_TARGET_PATH)/lib
	@$(CP) -d $(CUR_PATH)/lib/libswscale.so* $(ROOTFS_TARGET_PATH)/lib
	@$(CP) -d $(CUR_PATH)/lib/libpostproc.so* $(ROOTFS_TARGET_PATH)/lib
	@$(CP) -d $(FFMPEG_OUT_PATH)/bin/ffmpeg $(ROOTFS_TARGET_PATH)/bin/
	@$(CP) -d $(FFMPEG_OUT_PATH)/bin/ffprobe $(ROOTFS_TARGET_PATH)/bin/
	@$(ECHO) -e $(GREEN)"\nInstall ffmpeg success!!\n"  $(DONE)

clean:
	@$(RM)  $(ROOTFS_TARGET_PATH)/lib/libavcodec.so*
	@$(RM)  $(ROOTFS_TARGET_PATH)/lib/libavfilter.so*
	@$(RM)  $(ROOTFS_TARGET_PATH)/lib/libavformat.so*
	@$(RM)  $(ROOTFS_TARGET_PATH)/lib/libavutil.so*
	@$(RM)  $(ROOTFS_TARGET_PATH)/lib/libswresample.so*
	@$(RM)  $(ROOTFS_TARGET_PATH)/lib/libswscale.so*
	@$(RM)  $(ROOTFS_TARGET_PATH)/lib/libpostproc.so*
	@$(RM)  $(ROOTFS_TARGET_PATH)/bin/ffmpeg
	@$(RM)  $(ROOTFS_TARGET_PATH)/bin/ffprobe

	@$(BUILD_SCRIPT) clean src=$(FFMPEG_SRC_PATH)
	@$(RM) $(FFMPEG_OUT_PATH)
	@$(ECHO) -e $(GREEN)"\nClean ffmpeg success!!\n"  $(DONE)
