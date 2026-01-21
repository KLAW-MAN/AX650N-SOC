/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * Copyright (c) 2022 AXERA.
 *
 */
#include <stdio.h>
#include <common.h>
#include "../../drivers/ddr/axera/ax650x_lpddr4x.h"

#define DDR0_GLB_REG_BASE               0x8000000
#define DDR1_GLB_REG_BASE               0xC000000
#define DDR_SYS_GLB_AXICLK_OFF_ADDR     0x0060
#define DDR_SYS_GLB_CORECLK_OFF_ADDR    0x0064
#define DDR_SYS_GLB_DDR_AXI_ADDR        0x000C
#define DDR_SYS_GLB_DDRC_SLEEP_ADDR     0x0010
#define DDR_SYS_GLB_DDRC_WAKEUP_ADDR    0x0014
#define DDR_SYS_GLB_LPC_DATA_ADDR       0x0054
#define DDR_SYS_GLB_AUTO_REFRESH_ADDR   0x0068
#define DDR_SYS_GLB_DFS_CTRL_ADDR       0x006C
#define DDR_SYS_GLB_DFS_CLKSRC_ADDR     0x0070
#define DDR_SYS_GLB_DFS_INT_ADDR        0x0080
#define DDR_DFS_STATUS                  0X0084
#define DDR_SYS_GLB_DFS_CTRL_SET_FIELD  0x106C
#define DDR0_CLK_RST_BASE               0x8010000
#define DDR1_CLK_RST_BASE               0xC010000
#define CLK_MUX_0                       0x00
#define CLK_DIV_0                       0x0C

#define ax_readl(addr) (*(volatile u32 *) (addr))
#define ax_writel(b,addr) ((*(volatile u32 *) (addr)) = (b))
static void core_write_word(ulong reg, u32 val)
{
	ax_writel(val, reg);
}

static u32 core_read_word(ulong reg)
{
	return ax_readl(reg);
}
static void set_dfs_clksrc(ulong base_addr)
{
	u32 rdata;
	rdata = core_read_word(base_addr + DDR_SYS_GLB_DFS_CLKSRC_ADDR);
	rdata &= ~0x3fe;
#if DDR_CFG_4266
	rdata |= (0x04 << 7) | (0x03 << 4) | (0x04 << 1);
#endif
#if DDR_CFG_3733
	rdata |= (0x04 << 7) | (0x04 << 4);
#endif
#if DDR_CFG_3200
	rdata |= (0x03 << 7) | (0x04 << 4);
#endif
	core_write_word(base_addr + DDR_SYS_GLB_DFS_CLKSRC_ADDR, rdata);
	rdata = 0;
	rdata = core_read_word(base_addr + DDR_SYS_GLB_DFS_CLKSRC_ADDR);
}

static void set_dfs_target_freq(ulong base_addr, u32 target_freq)
{
	u32 rdata;
	rdata = core_read_word(base_addr + DDR_SYS_GLB_DFS_CTRL_ADDR);
	rdata &= ~0xc;
	switch (target_freq)
	{
#if DDR_CFG_4266
		case 4266:
			rdata |= (0 << 2);
			break;
		case 3200:
			rdata |= (1 << 2);
			break;
		case 2133:
			rdata |= (2 << 2);
			break;
#endif
#if DDR_CFG_733
		case 3733:
			rdata |= (0 << 2);
			break;
		case 2133:
			rdata |= (1 << 2);
			break;
#endif
#if DDR_CFG_3200
		case 3200:
			rdata |= (0 << 2);
			break;
		case 2133:
			rdata |= (1 << 2);
			break;
#endif
		default:
			break;
	}
	core_write_word(base_addr + DDR_SYS_GLB_DFS_CTRL_ADDR, rdata);
}

static void set_cmd_mode(ulong base_addr, u32 dfs_cmd_mode)
{
	u32 rdata;
	rdata = core_read_word(base_addr + DDR_SYS_GLB_DFS_CTRL_ADDR);
	rdata &= ~(0x1 << 1);
	rdata |= (dfs_cmd_mode << 1);
	core_write_word(base_addr + DDR_SYS_GLB_DFS_CTRL_ADDR, rdata);
}

static void set_clk_div(ulong base_addr, u32 target_freq)
{
	u32 rdata;
	if(target_freq == 2133){
		rdata = core_read_word(base_addr + DDR_SYS_GLB_DFS_CLKSRC_ADDR);
		rdata |= 0x1;
		core_write_word(base_addr + DDR_SYS_GLB_DFS_CLKSRC_ADDR, rdata);
	} else {
		rdata = core_read_word(base_addr + DDR_SYS_GLB_DFS_CLKSRC_ADDR);
		rdata &= ~0x1;
		core_write_word(base_addr + DDR_SYS_GLB_DFS_CLKSRC_ADDR, rdata);
	}
}

static void disable_int_mask(ulong base_addr)
{
	u32 rdata;
	rdata = core_read_word(base_addr + DDR_SYS_GLB_DFS_INT_ADDR);
	rdata &= ~(1 << 3);
	core_write_word(base_addr + DDR_SYS_GLB_DFS_INT_ADDR, rdata);
}

static void enable_int_mask(ulong base_addr)
{
	u32 rdata;
	rdata = core_read_word(base_addr + DDR_SYS_GLB_DFS_INT_ADDR);
	rdata |= (1 << 3);
	core_write_word(base_addr + DDR_SYS_GLB_DFS_INT_ADDR, rdata);
}

static void enable_hw_clk_gate(ulong base_addr)
{
	u32 rdata;
	rdata = core_read_word(base_addr + DDR_SYS_GLB_AXICLK_OFF_ADDR);
	rdata |= (1 << 1);
	core_write_word(base_addr + DDR_SYS_GLB_AXICLK_OFF_ADDR, rdata);

}

static void enable_hw_dfs(ulong base_addr)
{
	u32 rdata;
	rdata = core_read_word(base_addr + DDR_SYS_GLB_DFS_CTRL_ADDR);
	rdata |= (1 << 0);
	core_write_word(base_addr + DDR_SYS_GLB_DFS_CTRL_ADDR, rdata);
}

static void wait_dfs_done(ulong base_addr)
{
	u32 rdata;
	while(1) {
		rdata = core_read_word(base_addr + DDR_SYS_GLB_DFS_INT_ADDR);
		if(rdata & (1 << 1)) {
			break;
		}
	}
}

static void clr_dfs_done_int(ulong base_addr)
{
	u32 rdata;
	rdata = core_read_word(base_addr + DDR_SYS_GLB_DFS_INT_ADDR);
	rdata |= (1 << 2);
	core_write_word(base_addr + DDR_SYS_GLB_DFS_INT_ADDR, rdata);
}

static void dfs_one_cycle(ulong base_addr, u32 target_freq)
{
	set_dfs_clksrc(base_addr);
	set_dfs_target_freq(base_addr, target_freq);
	set_cmd_mode(base_addr,0);
	set_clk_div(base_addr, target_freq);
	disable_int_mask(base_addr);
	enable_hw_clk_gate(base_addr);
	enable_hw_dfs(base_addr);
	wait_dfs_done(base_addr);
	clr_dfs_done_int(base_addr);
	enable_int_mask(base_addr);
}

static int do_ddr_dfs(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	int ret = 0;
	u16 target_freq;
	if (argc < 2) {
		printf("Usage: please input freqs\n");
		return -1;
	}
	target_freq = simple_strtoul(argv[1], NULL, 10);
	if ((target_freq != 4266) && (target_freq != 3200) && (target_freq != 2133) && (target_freq != 3733)) {
		printf("Usage: target freqs must be 4266M or 3200M or 2133M\n");
		return -1;
	}
#if NON_INTERLEAVE_MODE
	dfs_one_cycle(DDR0_GLB_REG_BASE,target_freq);
#else
	dfs_one_cycle(DDR0_GLB_REG_BASE,target_freq);
	dfs_one_cycle(DDR1_GLB_REG_BASE,target_freq);
#endif
	printf("ddr dfs set frequency:%d successful!\n",target_freq);
	return ret;
}

U_BOOT_CMD(ddr_dfs, 2, 0, do_ddr_dfs,
	   "ddr dfs", "ddr use dfs function\n" "by input freq to change dfs freqs\n");