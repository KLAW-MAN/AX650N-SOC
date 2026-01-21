/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * Copyright (c) 2022 AXERA.
 *
 */
 /* ddr wait time is 100ms*/
#define DDR_WAIT_TIMEOUT 1000000
static int dp_slp_en __attribute__((section(".data"))) = 0;
static int curr_rank_num __attribute__((section(".data"))) = 2;	//default dual rank
static int use_lpddr4 __attribute__((section(".data"))) = 0;	//default lpddr4x
extern int early_printf(const char *fmt, ...);
static const struct dram_cfg_param ddrc_cfg[] = {
#if DDR_CFG_4266
#include "ddrc_train_4266.h"
#endif
#if DDR_CFG_3733
#include "ddrc_train_3733.h"
#endif
#if DDR_CFG_3200
#include "ddrc_train_3200.h"
#endif
};

static const struct dram_cfg_param ddrc_common_cfg[] = {
#include "ddrc_common_cfg.h"
};
#define ax_readw(addr) (*(volatile u16 *) (addr))
#define ax_readl(addr) (*(volatile u32 *) (addr))
#define ax_writew(b,addr) ((*(volatile u16 *) (addr)) = (b))
#define ax_writel(b,addr) ((*(volatile u32 *) (addr)) = (b))
static void core_write_word(ulong reg, u32 val)
{
	ax_writel(val, reg);
}

static u32 core_read_word(ulong reg)
{
	return ax_readl(reg);
}
static void core_write_halfword(ulong addr, u32 val)
{
	ax_writew(val, addr);
}
static u16 core_read_halfword(ulong addr)
{
	return ax_readw(addr);
}
static void ddr_init_ctr_reg(ulong base_addr, int dp_slp_en)
{
	int i;
	core_write_word(base_addr + 0x00000304, 0x00000001);	//reg_cfg.DBG1
	//core_write_word(base_addr + 0x00003020,  reg_cfg.FREQ2_DERATEEN);  //FREQ2.DERATEEN          FREQ2.DERATEEN=0x00000100
	core_write_word(base_addr + 0x00003024, 0x2ced27be);	//FREQ2.DERATEINT
	//core_write_word(base_addr + 0x00003034, 0x00403804 );              //FREQ2.PWRTMG
	core_write_word(base_addr + 0x00003050, 0x68210002);	//FREQ2.RFSHCTL0
	//core_write_word(base_addr + 0x000030e8, 0x0000004d );              //FREQ2.INIT6
	core_write_word(base_addr + 0x000030ec, 0x0000004d);	//FREQ2.INIT7
	core_write_word(base_addr + 0x000030f4, 0x0000032f);	//FREQ2.RANKCTL
	core_write_word(base_addr + 0x00003120, 0x00000101);	//FREQ2.DRAMTMG8
	core_write_word(base_addr + 0x00003130, 0x00020000);	//FREQ2.DRAMTMG12
	//core_write_word(base_addr + 0x00003194, 0x000a0404 );                 //FREQ2.DFITMG1
	if (dp_slp_en == 1)
		core_write_word(base_addr + 0x00000030, 0x000000a0);
	else
		core_write_word(base_addr + 0x00000030, 0x00000020);	//PWRCTL
	if (dp_slp_en == 1)
		core_write_word(base_addr + 0x00001b30, 0x000000a0);
	else
		core_write_word(base_addr + 0x00001b30, 0x00000020);	//DCH1.PWRCTL
	for (i = 0; i < ARRAY_SIZE(ddrc_cfg); i++) {
		core_write_word(ddrc_cfg[i].reg + base_addr, ddrc_cfg[i].val);
	}
	for (i = 0; i < ARRAY_SIZE(ddrc_common_cfg); i++) {
		core_write_word(ddrc_common_cfg[i].reg + base_addr, ddrc_common_cfg[i].val);
	}
}

static void ddrc_additional_cfg(ulong base_addr)
{
	core_write_word(base_addr + 0x000000f4, 0x0000f42f);	//reg_cfg.RANKCTL, 0x0);
	core_write_word(base_addr + 0x000020f8, 0x00000004);	//reg_cfg.FREQ1_RANKCTL1, 0x0);
	core_write_word(base_addr + 0x000020f4, 0x0000f42f);	//reg_cfg.FREQ1_RANKCTL, 0x0);
	core_write_word(base_addr + 0x000020f4, 0x00000006);	//reg_cfg.FREQ1_RANKCTL, 0x0);
	core_write_word(base_addr + 0x000030f4, 0x0000f42f);	//reg_cfg.FREQ1_RANKCTL, 0x0);
	core_write_word(base_addr + 0x000030f4, 0x00000006);	//reg_cfg.FREQ1_RANKCTL, 0x0);
	core_write_word(base_addr + 0x00000050, 0xa8210006);	//reg_cfg.RFSHCTL0, 0x0);
	core_write_word(base_addr + 0x00000060, 0x00000000);	//reg_cfg.RFSHCTL3, 0x0);
#if DDR_CFG_DFS
	core_write_word(base_addr + 0x0000003c, 0x00010073);    // for dfs HWFFCCTL
	core_write_word(base_addr + 0x00000038, 0x00030000);    // for dfs HWLPCTL
	core_write_word(base_addr + 0x00001b38, 0x00030000);    // for dfs DCH1_HWLPCTL
#endif
	/* default use dual rank, if single rank used, need to re-config */
	if(curr_rank_num == 1) {
		core_write_word(base_addr + DDRC_ADDRMAP0, 0x00031f1f);
		core_write_word(base_addr + DDRC_ADDRMAP1, 0x00090906);
		core_write_word(base_addr + DDRC_ADDRMAP2, 0x01000000);
		core_write_word(base_addr + DDRC_ADDRMAP3, 0x02020201);
		core_write_word(base_addr + DDRC_ADDRMAP4, 0x00001f1f);
		core_write_word(base_addr + DDRC_ADDRMAP5, 0x08080808);
		core_write_word(base_addr + DDRC_ADDRMAP6, 0x08080808);
		core_write_word(base_addr + DDRC_ADDRMAP7, 0x00000f0f);
	}
}

extern void udelay(u32 us);
static void ddr_wait_dfi_init_complete(ulong base_addr)
{
	u32 dfistat = 0;
	u32 dch1_dfistat = 0;
	int rd_cnt = 0;
	core_write_word(base_addr + 0x000001b0, 0x000010b0);
	while (1) {
		dfistat = core_read_word(base_addr + 0x000001bc);
		dch1_dfistat = core_read_word(base_addr + 0x00001cbc);
		rd_cnt++;
		if ((dfistat & 1) && (dch1_dfistat & 1)) {
			break;
		}
		udelay(1);
		if(rd_cnt >= DDR_WAIT_TIMEOUT) {
			//early_printf("ddr_wait_dfi_init_complete timeout\r\n");
			while(1);
		}
	}
}

static void ddr_wait_normal_stat(ulong base_addr, int dp_slp_en)
{
	int rdata = 0;
	int stat = 0, dch1_stat = 0;
	int rd_cnt = 0;
	core_write_word(base_addr + 0x000001b0, 0x00000010);
	core_write_word(base_addr + 0x000001b0, 0x00000011);
	if (dp_slp_en == 1) {
	} else {
		core_write_word(base_addr + 0x00000030, 0x00000000);
		core_write_word(base_addr + 0x00001b30, 0x00000000);
	}
	core_write_word(base_addr + 0x00000320, 0x00000001);
	//Wait swctl done ack
	while (1) {
		rdata = core_read_word(base_addr + 0x00000324);
		rd_cnt++;
		if ((rdata & 1) == 1) {
			break;
		}
		udelay(1);
		if(rd_cnt >= DDR_WAIT_TIMEOUT) {
			//early_printf("ddr_wait_normal_stat0 timeout\n");
			while(1);
		}
	}
	rd_cnt = 0;
	//wait normal stat
	while (1) {
		stat = core_read_word(base_addr + 0x00000004);
		dch1_stat = core_read_word(base_addr + 0x00001b04);
		rd_cnt++;
		if ((stat & 1) && (dch1_stat & 1)) {
			break;
		}
		udelay(1);
		if(rd_cnt >= DDR_WAIT_TIMEOUT) {
			//early_printf("ddr_wait_normal_stat1 timeout\n");
			while(1);
		}
	}
}

void ddr_post_init(ulong base_addr, int reg_cfg, int dp_slp_en)
{
	int rdata = 0;
	int rd_cnt = 0;
	core_write_word(base_addr + 0x000001c4, 1);
	core_write_word(base_addr + 0x00000320, 0x00000000);
	core_write_word(base_addr + 0x000000d0, 0xc0020002);
	//core_write_word(base_addr + 0x000000d0, 0x0004004e);
	core_write_word(base_addr + 0x00000320, 0x00000001);
	//Wait swctl done ack
	while (1) {
		rdata = core_read_word(base_addr + 0x00000324);
		rd_cnt++;
		if (rdata & 1) {
			break;
		}
		udelay(1);
		if(rd_cnt >= DDR_WAIT_TIMEOUT) {
			//early_printf("ddr_post_init timeout\r\n");
			while(1);
		}
	}
	rd_cnt = 0;
	core_write_word(base_addr + 0x00000304, 0x00000000);
	core_write_word(base_addr + 0x00001e04, 0x00000000);
	if (dp_slp_en == 1) {
		core_write_word(base_addr + 0x00000030, 0x00000080);
	} else {
		core_write_word(base_addr + 0x00000030, 0x00000000);	//PWRCTL
	}
	if (dp_slp_en == 1) {
		core_write_word(base_addr + 0x00001b30, 0x00000080);
	} else {
		core_write_word(base_addr + 0x00001b30, 0x00000000);	//DCH1.PWRCTL
	}
	core_write_word(base_addr + 0x00000490, 0x00000001);	//PCTRL_0
	core_write_word(base_addr + 0x00000540, 0x00000001);	//PCTRL_1
	core_write_word(base_addr + 0x000005f0, 0x00000001);	//PCTRL_2
	core_write_word(base_addr + 0x000006a0, 0x00000001);	//PCTRL_3
	core_write_word(base_addr + 0x00000750, 0x00000001);	//PCTRL_4
	rdata = core_read_word(base_addr + 0x00000304);	//DBG1
	rdata = core_read_word(base_addr + 0x00000304);	//DBG1
	rdata = core_read_word(base_addr + 0x00000304);	//DBG1
	rdata = core_read_word(base_addr + 0x00001e04);	//DCH1.DBG1
	core_write_word(base_addr + 0x00000304, 0x00000000);	//DBG1
	core_write_word(base_addr + 0x00000304, 0x00000000);	//DBG1
	core_write_word(base_addr + 0x00000304, 0x00000000);	//DBG1
	core_write_word(base_addr + 0x00001e04, 0x00000000);	//DCH1.DBG1
}
