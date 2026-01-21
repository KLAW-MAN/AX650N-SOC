CUR_PATH		   := $(shell pwd)
HOME_PATH		   := $(CUR_PATH)/../..
OUT_PATH           := $(HOME_PATH)/msp/out

include $(HOME_PATH)/build/color.mk
include $(HOME_PATH)/build/config.mak

.PHONY : all clean install

all:
	@echo -e $(GREEN)"\nBuild qt success!!\n"  $(DONE)

install:
	@$(MKDIR) $(OUT_PATH)/lib/qt-5.15.7
	@$(CP) -r $(CUR_PATH)/lib $(OUT_PATH)/lib/qt-5.15.7/
	@$(CP) -r $(CUR_PATH)/fonts $(OUT_PATH)/lib/qt-5.15.7/
	@$(CP) -r $(CUR_PATH)/plugins $(OUT_PATH)/lib/qt-5.15.7/
	@$(CP) -r $(CUR_PATH)/qml $(OUT_PATH)/lib/qt-5.15.7/
	@$(ECHO) -e $(GREEN)"\nInstall qt success!!\n"  $(DONE)

clean:
	@$(RM) -r $(OUT_PATH)/lib/qt-5.15.7
	@$(ECHO) -e $(GREEN)"\nClean qt success!!\n"  $(DONE)
