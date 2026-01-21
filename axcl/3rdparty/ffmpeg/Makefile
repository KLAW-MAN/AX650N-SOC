MKFILE_PATH         := $(abspath $(lastword $(MAKEFILE_LIST)))
CUR_PATH            := $(abspath $(dir $(MKFILE_PATH)))
AXCL_HOME_PATH      := $(abspath $(CUR_PATH)/../..)
AXCL_BUILD_PATH     := $(AXCL_HOME_PATH)/build

include $(AXCL_BUILD_PATH)/config.mak

BUILD_SCRIPT = ./build.sh

ifeq ($(HOST),x86)
FFMPEG_OUT_PATH=$(CUR_PATH)/out/x64
FFMPEG_INC_PATH=$(CUR_PATH)/x64/
else ifeq ($(HOST),riscv)
FFMPEG_OUT_PATH=$(CUR_PATH)/out/riscv
FFMPEG_INC_PATH=$(CUR_PATH)/riscv/
else
FFMPEG_OUT_PATH=$(CUR_PATH)/out/arm64
FFMPEG_INC_PATH=$(CUR_PATH)/arm64/
endif

ifeq ($(debug),yes)
DBG=yes
else
DBG=
endif

FFMPEG_SRC_PATH=$(CUR_PATH)/FFmpeg-n7.1

.PHONY: install clean all

all: clean
	@$(MKDIR) $(FFMPEG_OUT_PATH)
	@$(BUILD_SCRIPT) dbg=$(DBG) cross=$(CROSS) axcl_inc=$(AXCL_INC_PATH) out=$(FFMPEG_OUT_PATH) host=$(HOST) src=$(FFMPEG_SRC_PATH)
	@$(ECHO) -e $(GREEN)"\nBuild ffmpeg success!!\n"  $(DONE)
	@$(CP) -rf $(FFMPEG_OUT_PATH)/include $(FFMPEG_INC_PATH)

	@$(MKDIR) $(AXCL_LIB_PATH)/ffmpeg
	@$(CP) -d $(FFMPEG_OUT_PATH)/lib/lib* $(AXCL_LIB_PATH)/ffmpeg/

install:
	@$(MKDIR) $(AXCL_BIN_PATH)/ffmpeg
	@$(CP) -f $(FFMPEG_OUT_PATH)/bin/ffmpeg $(AXCL_BIN_PATH)/ffmpeg/
	@$(CP) -f $(FFMPEG_OUT_PATH)/bin/ffprobe $(AXCL_BIN_PATH)/ffmpeg/
	@$(ECHO) -e $(GREEN)"\nInstall ffmpeg success!!\n"  $(DONE)

clean:
	@$(BUILD_SCRIPT) clean src=$(FFMPEG_SRC_PATH)
	@$(RM) $(AXCL_LIB_PATH)/ffmpeg
	@$(RM) $(AXCL_BIN_PATH)/ffmpeg
	@$(RM) $(FFMPEG_OUT_PATH)
	@$(ECHO) -e $(GREEN)"\nClean ffmpeg success!!\n"  $(DONE)
