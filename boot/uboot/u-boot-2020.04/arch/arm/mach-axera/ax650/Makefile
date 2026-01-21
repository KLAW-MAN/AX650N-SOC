#
# (C) Copyright 2016 Rockchip Electronics Co., Ltd
#
# SPDX-License-Identifier:     GPL-2.0+
#
ifneq ($(cpu_clk),)
KBUILD_CPPFLAGS += -DCFG_CPU_CLK_CMDLINE
KBUILD_CPPFLAGS += -DCPU_CLK=$(cpu_clk)
endif

obj-y += ax650.o
obj-y += timer.o
obj-y += efuse_drv.o
obj-y += board.o
obj-y += chip_config.o
obj-y += pll_config.o
obj-y += early_printf.o
obj-y += uart.o
obj-y += emmc_sd_phy.o
obj-y += dphyrx.o
obj-$(CONFIG_PWM_AXERA) += common/
obj-$(CONFIG_AXERA_MEMORY_FIREWALL) += firewall.o
