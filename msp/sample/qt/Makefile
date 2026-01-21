CUR_PATH            := $(shell pwd)
BIN_PATH            := $(CUR_PATH)/bin
HOME_PATH           := $(CUR_PATH)/../../..
OUT_PATH            := $(HOME_PATH)/msp/out/bin
LIB_PATH            := $(HOME_PATH)/msp/out/lib
SRC_PATH            := $(CUR_PATH)/source
QT_PATH             := $(HOME_PATH)/third-party/qt-5.15.7/bin
ROOTFS_TARGET_PATH  ?= $(OUT_PATH)

include $(HOME_PATH)/build/color.mk
include $(HOME_PATH)/build/config.mak

debug=no

################################################################################
# build
################################################################################

.PHONY : all clean install default
.NOTPARALLEL: clean all

all: default
	@echo -e $(GREEN)"\nBuild $@ success!!\n"  $(DONE)

ifneq ($(wildcard Makefile.qt),)
clean:
	$(VERB) $(MAKE) -f Makefile.qt clean
	$(VERB) $(RM) -rf $(CUR_PATH)/build
	$(VERB) $(RM) -rf $(CUR_PATH)/Makefile.qt
	$(VERB) $(RM) -rf $(CUR_PATH)/bin
else
clean:
	$(VERB) $(RM) -rf $(CUR_PATH)/build
	$(VERB) $(RM) -rf $(CUR_PATH)/bin
endif

ifeq ($(debug), yes)
default:
	@echo -e $(GREEN)"Build $@ ..."  $(DONE)
	$(shell qmake -o Makefile.qt $(SRC_PATH)/qt_demo.pro CONFIG+=debug QMAKE_CFLAGS+="-O0 -ggdb3")
	$(VERB) $(MAKE) -f Makefile.qt
	$(MKDIR) -p $(BIN_PATH)
	$(VERB) $(CP) $(CUR_PATH)/build/debug/sample_qt $(BIN_PATH)
else
default:
	@echo -e $(GREEN)"Build $@ ..." $(DONE)
	$(VERB) $(QT_PATH)/qmake -o Makefile.qt $(SRC_PATH)/qt_demo.pro CONFIG+=release
	$(VERB) $(MAKE) -f Makefile.qt
	$(MKDIR) -p $(BIN_PATH)
	$(VERB) $(CP) $(CUR_PATH)/build/release/sample_qt $(BIN_PATH)
endif

export ROOTFS_TARGET_PATH
