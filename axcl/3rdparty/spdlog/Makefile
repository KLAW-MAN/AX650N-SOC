MKFILE_PATH        := $(abspath $(lastword $(MAKEFILE_LIST)))
CUR_PATH           := $(abspath $(dir $(MKFILE_PATH)))
AXCL_HOME_PATH     := $(abspath $(CUR_PATH)/../..)

include $(AXCL_HOME_PATH)/build/config.mak

SRC_LIB_PATH       := $(AXCL_HOME_PATH)/3rdparty/spdlog/$(ARCH)/lib
INSTALL_PATH       := $(AXCL_LIB_PATH)

all:
	@$(MKDIR) $(INSTALL_PATH)
	@$(CP) -d $(SRC_LIB_PATH)/libspdlog.so* $(INSTALL_PATH)
	@$(ECHO) -e $(GREEN)"\nBuild spdlog success!!\n"  $(DONE)

install:
ifeq ($(HOST),ax650)
	@$(CP) -d $(SRC_LIB_PATH)/libspdlog.so* $(HOME_PATH)/rootfs/rootfs/usr/local/lib
endif
	@$(ECHO) -e $(GREEN)"\nInstall spdlog success!!\n"  $(DONE)

clean:
	@$(RM) $(INSTALL_PATH)/libspdlog.so*
ifeq ($(HOST),ax650)
	@$(RM) $(HOME_PATH)/rootfs/rootfs/usr/local/lib/libspdlog.so*
endif
	@$(ECHO) -e $(GREEN)"\nClean spdlog success!!\n"  $(DONE)