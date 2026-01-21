include ../Makefile.inc
HOME_PATH               := ../../../../../..
INSTALL_BIN_PATH ?= $(HOME_PATH)/rootfs/rootfs/opt/data/dsp
BINDIR  = $(ROOTDIR)/out/dsp_algo
all : $(BINDIR)/dsp

$(BINDIR)/dsp : dsp_algo.c sample_add.c sample_tile_manager.c ax_dsp_trace.c sample_idma.c
	@mkdir -p $(BINDIR)
	$(CC) $(CFLAGS) -Iinclude -DIDMA_USE_XTOS $(LDFLAGS) -mlsp=$(LSP) $^ -o $@ -L$(LIBDIR)  -lax_tile_manager -lax650x -lax_cvlib -lm -lxcvFrameLib -lTileManagerFIK -lTileManager  -lxi -lidma-xtos -lidma
	$(DUMPELF) --base=0x20000000 --width=32 --little-endian --default=0x00000000 --full $(BINDIR)/dsp --size=0x80000 >  $(BINDIR)/sram.txt
	$(DUMPELF) --base=0x00000000 --width=32 --little-endian --default=0x00000000 --full $(BINDIR)/dsp --size=0x8000 >  $(BINDIR)/itcm.txt
	$(DUMPELF) --base=0x18500000 --width=32 --little-endian --default=0x00000000  $(BINDIR)/dsp --size=0x40000 >  $(BINDIR)/dtcm.txt
	$(DUMPELF) --base=0x18540000 --width=32 --little-endian --default=0x00000000  $(BINDIR)/dsp --size=0x40000 >  $(BINDIR)/dtcm2.txt
	sed -i '1d' $(BINDIR)/dtcm.txt
	sed -i '1d' $(BINDIR)/dtcm2.txt
	python2 ../hex2bin.py $(BINDIR)/itcm.txt $(BINDIR)/itcm.bin
	python2 ../hex2bin.py $(BINDIR)/sram.txt $(BINDIR)/sram.bin
	python2 ../hex2bin.py $(BINDIR)/dtcm.txt $(BINDIR)/dtcm.bin
	python2 ../hex2bin.py $(BINDIR)/dtcm2.txt $(BINDIR)/dtcm2.bin

$(BINDIR)/%.o: %.S
	$(CC) $(ASFLAGS) $(FLAGS_$*) -o $@ -c $<

$(BINDIR)/%.o: %.c
	$(CC) $(CFLAGS)  $(FLAGS_$*) -o $@ -c $<
install: all
	@mkdir -p $(INSTALL_BIN_PATH)
	@cp -f $(BINDIR)/itcm.bin  $(INSTALL_BIN_PATH)/
	@cp -f $(BINDIR)/sram.bin  $(INSTALL_BIN_PATH)/

INSTALL_DATA            := $(BINDIR)

clean:
	-$(RM) -rf $(BINDIR)/*

.PHONY : all install clean

