CUR_PATH           := $(shell pwd)
AXCL_HOME_PATH     := $(CUR_PATH)/../..

include $(AXCL_HOME_PATH)/build/config.mak

SRC_PATH           := $(CUR_PATH)/$(ARCH)
INSTALL_PATH       := $(AXCL_LIB_PATH)/zmq

all:
	@$(ECHO) -e $(GREEN)"\nBuild zmq success!!\n"  $(DONE)

install:
	@$(MKDIR) $(INSTALL_PATH)
	@$(ECHO) -e $(GREEN)"install: $(SRC_PATH)"  $(DONE)
	@$(CP) -d $(SRC_PATH)/libzmq.so* $(INSTALL_PATH)
	@$(ECHO) -e $(GREEN)"\nInstall zmq success!!\n"  $(DONE)

clean:
	@$(RM) $(INSTALL_PATH)
	@$(ECHO) -e $(GREEN)"\nClean zmq success!!\n"  $(DONE)