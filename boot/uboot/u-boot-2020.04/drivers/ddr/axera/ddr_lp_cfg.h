/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * Copyright (c) 2022 AXERA.
 *
 */
static void ddr_axiclk_off_enable(ulong ddr_glb_base_addr)
{
	u32 rdata = 0;
	rdata = core_read_word(ddr_glb_base_addr + DDR_SYS_GLB_AXICLK_OFF_ADDR);
	rdata |= (1 << 1);
	core_write_word(ddr_glb_base_addr + DDR_SYS_GLB_AXICLK_OFF_ADDR, rdata);
}

static void ddr_axiclk_off_disable(ulong ddr_glb_base_addr)
{
	u32 rdata = 0;
	rdata = core_read_word(ddr_glb_base_addr + DDR_SYS_GLB_AXICLK_OFF_ADDR);
	rdata &= ~(1 << 1);
	core_write_word(ddr_glb_base_addr + DDR_SYS_GLB_AXICLK_OFF_ADDR, rdata);
}

static void ddr_coreclk_off_enable(ulong ddr_glb_base_addr)
{
	u32 rdata = 0;
	rdata = core_read_word(ddr_glb_base_addr + DDR_SYS_GLB_CORECLK_OFF_ADDR);
	rdata |= (1 << 1);
	core_write_word(ddr_glb_base_addr + DDR_SYS_GLB_CORECLK_OFF_ADDR, rdata);
}

static void ddr_coreclk_off_disable(ulong ddr_glb_base_addr)
{
	u32 rdata = 0;
	rdata = core_read_word(ddr_glb_base_addr + DDR_SYS_GLB_CORECLK_OFF_ADDR);
	rdata &= ~(1 << 1);
	core_write_word(ddr_glb_base_addr + DDR_SYS_GLB_CORECLK_OFF_ADDR, rdata);
}

// sw slp
static void enter_ddr_axi_lpw_req(ulong ddr_glb_base_addr, int axi_port_idx)
{
	u32 rdata = 0;
	rdata = core_read_word(ddr_glb_base_addr + DDR_SYS_GLB_DDR_AXI_ADDR);
	rdata |= (1 << axi_port_idx);
	core_write_word(ddr_glb_base_addr + DDR_SYS_GLB_DDR_AXI_ADDR, rdata);
}

static void enter_ddrc_slp_req(ulong ddr_glb_base_addr)
{
	core_write_word(ddr_glb_base_addr + DDR_SYS_GLB_DDRC_SLEEP_ADDR, 1);
}

static void exit_ddrc_slp_req(ulong ddr_glb_base_addr)
{
	core_write_word(ddr_glb_base_addr + DDR_SYS_GLB_DDRC_SLEEP_ADDR, 0);
}

static void exit_ddr_axi_lpw_req(ulong ddr_glb_base_addr, int axi_port_idx)
{
	u32 rdata = 0;
	rdata = core_read_word(ddr_glb_base_addr + DDR_SYS_GLB_DDR_AXI_ADDR);
	rdata &= ~(1 << axi_port_idx);
	core_write_word(ddr_glb_base_addr + DDR_SYS_GLB_DDR_AXI_ADDR, rdata);
}

static void set_ddrc_wakeup(ulong ddr_glb_base_addr)
{
	core_write_word(ddr_glb_base_addr + DDR_SYS_GLB_DDRC_WAKEUP_ADDR, 1);
}

static void ddr_data_bus_idle_enable(ulong ddr_glb_base_addr)
{
	u32 rdata = 0;
	rdata = core_read_word(ddr_glb_base_addr + DDR_SYS_GLB_LPC_DATA_ADDR);
	rdata |= 1;
	core_write_word(ddr_glb_base_addr + DDR_SYS_GLB_LPC_DATA_ADDR, rdata);
}

static void ddr_dfs_ctrl_tar_freq_set(ulong ddr_glb_base_addr)
{
	u32 rdata = 0;
	rdata = core_read_word(ddr_glb_base_addr + DDR_SYS_GLB_DFS_CTRL_ADDR);
	rdata &= 0x3 << 3;
	rdata |= 0x3 << 3;
	core_write_word(ddr_glb_base_addr + DDR_SYS_GLB_DFS_CTRL_ADDR, rdata);
}

//  static config
static void enable_ddrc_refresh(ulong ddrc_base_addr)
{
	u32 rdata = 0;
	rdata = core_read_word(ddrc_base_addr + 0x30);	//PWRCTL
	rdata |= 1;
	core_write_word(ddrc_base_addr + 0x30, rdata);
	rdata = core_read_word(ddrc_base_addr + 0x1b30);	//DCH1_PWRCTL
	rdata |= 1;
	core_write_word(ddrc_base_addr + 0x1b30, rdata);
}

static void enable_ddrc_hw_lp_en(ulong ddrc_base_addr)
{
	core_write_word(ddrc_base_addr + 0x38, 0x30003);	//HWLPCTL
	core_write_word(ddrc_base_addr + 0x1b38, 0x30003);	//DCH1_HWLPCTL
}

static void enable_ddrc_wakeup(ulong ddrc_base_addr)
{				//DFILPCFG0
	u32 rdata = 0;
	rdata = core_read_word(ddrc_base_addr + 0x198);
	rdata |= 1 << 8;
	rdata &= ~(0xf << 12);
	rdata |= 0x9;
	core_write_word(ddrc_base_addr + 0x198, rdata);
}

static void ddrc_lp_cfg(ulong ddrc_base_addr)
{
	enable_ddrc_refresh(ddrc_base_addr);
	enable_ddrc_hw_lp_en(ddrc_base_addr);
	enable_ddrc_wakeup(ddrc_base_addr);
}

//ddr glb reg deep sleep cfg
//step (2)
static void ddr_glb_reg_deep_lp_cfg(ulong ddr_glb_base_addr)
{
	ddr_data_bus_idle_enable(ddr_glb_base_addr);
	ddr_axiclk_off_enable(ddr_glb_base_addr);
	ddr_coreclk_off_enable(ddr_glb_base_addr);
	ddr_dfs_ctrl_tar_freq_set(ddr_glb_base_addr);
}

//step(3)
void ddrc_deep_lp_cfg(ulong ddrc_base_addr)
{
	u32 rdata;
	core_write_word(ddrc_base_addr + 0x320, 0);
	rdata = core_read_word(ddrc_base_addr + 0x1b0);	//DFIMISC.dfi_frequency
	rdata |= (0x1f << 12);
	core_write_word(ddrc_base_addr + 0x1b0, rdata);
	rdata = core_read_word(ddrc_base_addr + 0x3c);	//HWFFCCTL.hwffc_en
	rdata |= 0x3;
	core_write_word(ddrc_base_addr + 0x3c, rdata);
	core_write_word(ddrc_base_addr + 0x320, 1);
}
