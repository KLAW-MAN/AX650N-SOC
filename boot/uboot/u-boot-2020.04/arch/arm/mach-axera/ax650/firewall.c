#include <common.h>
#include <asm/io.h>
#include "stdarg.h"
#include "early_printf.h"
#include <linux/serial_reg.h>

#define DDR_BASE (0x100000000UL)
#define SEC_GLB_BASE (0x4000000UL)
#define SEC_COMMON_CE_AR (SEC_GLB_BASE + 0xB8)
#define SEC_COMMON_CE_AW (SEC_GLB_BASE + 0xBC)
#define SEC_CK_RST_BASE		(0x400F000UL)
#define CLK_EB_0	(SEC_CK_RST_BASE + 0x0)
#define SW_RST_0	(SEC_CK_RST_BASE + 0x4)
#define CLK_EB_0_SET	(SEC_CK_RST_BASE + 0x8)
#define CLK_EB_0_CLR	(SEC_CK_RST_BASE + 0xC)
#define SW_RST_0_SET	(SEC_CK_RST_BASE + 0x10)
#define SW_RST_0_CLR	(SEC_CK_RST_BASE + 0x14)

#define DDR_FW_PORT0_BASE	(0x4018000UL)
#define DDR_FW_PORT1_BASE	(0x401A000UL)
#define DDR_FW_PORT3_BASE	(0x401C000UL)
#define DDR_FW_PORT4_BASE	(0x401E000UL)

#define FW_PORT0	DDR_FW_PORT0_BASE
#define FW_PORT1	DDR_FW_PORT1_BASE
#define FW_PORT3	DDR_FW_PORT3_BASE
#define FW_PORT4	DDR_FW_PORT4_BASE

#define REGION_SETTING_REG(_FW_PORT_)		((_FW_PORT_) + 0x0)
#define MASTER_SETTING_REG(_FW_PORT_)		((_FW_PORT_) + 0x1000)
#define CFG_REG(_FW_PORT_)					((_FW_PORT_) + 0x1400)
#define PORT_ERROR_STATUS_INT_REG(_FW_PORT_)	((_FW_PORT_) + 0x1800)

#define CHOSEN_MASTER_ID(_FW_PORT_)		(MASTER_SETTING_REG(_FW_PORT_) + 0x0)
#define IGNORE_MASTER_PROT(_FW_PORT_)	(MASTER_SETTING_REG(_FW_PORT_) + 0x4)

#define REGION_START(_FW_PORT_)					(REGION_SETTING_REG(_FW_PORT_) + 0x0)
#define REGION_END(_FW_PORT_)					(REGION_SETTING_REG(_FW_PORT_) + 0x4)
#define REGION_EN(_FW_PORT_)					(REGION_SETTING_REG(_FW_PORT_) + 0x8)
#define REGION_PERMITTED_MASTER0(_FW_PORT_)		(REGION_SETTING_REG(_FW_PORT_) + 0xC)
#define REGION_PERMITTED_MASTER1(_FW_PORT_)		(REGION_SETTING_REG(_FW_PORT_) + 0x18)
#define REGION_UPD_PRMTD_MASTER(_FW_PORT_)		(REGION_SETTING_REG(_FW_PORT_) + 0x24)

#define INT_RAW_STATUS(_FW_PORT_)		(PORT_ERROR_STATUS_INT_REG(_FW_PORT_) + 0x0)
#define INT_CLEAR(_FW_PORT_)			(PORT_ERROR_STATUS_INT_REG(_FW_PORT_) + 0x14)
#define INT_CLEAR_SET(_FW_PORT_)		(PORT_ERROR_STATUS_INT_REG(_FW_PORT_) + 0x18)
#define ILLEGAL_WR_LO_ADDR(_FW_PORT_)		(PORT_ERROR_STATUS_INT_REG(_FW_PORT_) + 0x1C)
#define ILLEGAL_WR_HI_ADDR(_FW_PORT_)		(PORT_ERROR_STATUS_INT_REG(_FW_PORT_) + 0x20)
#define ILLEGAL_WR_ATTR0(_FW_PORT_)		(PORT_ERROR_STATUS_INT_REG(_FW_PORT_) + 0x24)
#define ILLEGAL_WR_ATTR1(_FW_PORT_)		(PORT_ERROR_STATUS_INT_REG(_FW_PORT_) + 0x28)
#define ILLEGAL_RD_LO_ADDR(_FW_PORT_)		(PORT_ERROR_STATUS_INT_REG(_FW_PORT_) + 0x2C)
#define ILLEGAL_RD_HI_ADDR(_FW_PORT_)		(PORT_ERROR_STATUS_INT_REG(_FW_PORT_) + 0x30)
#define ILLEGAL_RD_ATTR0(_FW_PORT_)		(PORT_ERROR_STATUS_INT_REG(_FW_PORT_) + 0x34)
#define ILLEGAL_RD_ATTR1(_FW_PORT_)		(PORT_ERROR_STATUS_INT_REG(_FW_PORT_) + 0x38)
#define ILLEGAL_WR_CNT(_FW_PORT_)		(PORT_ERROR_STATUS_INT_REG(_FW_PORT_) + 0x3C)
#define ILLEGAL_RD_CNT(_FW_PORT_)		(PORT_ERROR_STATUS_INT_REG(_FW_PORT_) + 0x40)

#define REGION_SETTING_REG_SIZE		64
#define MASTER_SETTING_REG_SIZE		8
#define PORT_ERROR_STATUS_INT_REG_SIZE		128

#define ILLEGAL_RW_CNT_OF_BIT	31

#define REGION_NUM		64
#define MASTER_ID_CA55		0
#define MASTER_ID_SDIO		1
#define MASTER_ID_USB2		2
#define MASTER_ID_EMMC		3
#define MASTER_ID_DMA0		4
#define MASTER_ID_DMA1		5
#define MASTER_ID_DMA_XOR		6
#define MASTER_ID_VDSP_M1		7
#define MASTER_ID_VDSP_M2		8
#define MASTER_ID_VDSP_M3		9
#define MASTER_ID_VDSP_M4		10
#define MASTER_ID_ISP		11
#define MASTER_ID_NPU		12
#define MASTER_ID_VDEC		14
#define MASTER_ID_VENC0		16
#define MASTER_ID_VENC1		17
#define MASTER_ID_MM		18
#define MASTER_ID_JDEC		19
#define MASTER_ID_SEC_CE	21
#define MASTER_ID_PUB_CE	22
#define MASTER_ID_DMA2		23
#define MASTER_ID_RISCV		24
#define MASTER_ID_SATA0     25
#define MASTER_ID_PCIE0     26
#define MASTER_ID_PCIE1     27
#define MASTER_ID_USB0      28
#define MASTER_ID_USB1      29
#define MASTER_ID_EMAC1     30
#define MASTER_ID_SPI_AHB   31
#define MASTER_ID_JTAG		32
#define MASTER_ID_EMAC0     33
#define MASTER_ID_SDIO1     34
#define MASTER_ID_DW_DMA    35

/*only cpu & sec_ce permit setting 1, other master setting to 0*/
#define REGION_PERMIT_DEFAULT_SETTING0		(0x9ULL)
#define MASTER_ID_ALL_DISEN		(0x0ULL)

#define MASTER_NUM	30
typedef struct
{
	u32 fw_port;
	u8 mst_id;
	u8 port_ind;
}FW_MASTER_PARAM_S;

static const u32 port_base[4] = {
	FW_PORT0,
	FW_PORT1,
	FW_PORT3,
	FW_PORT4
};
static u64 permitted_setting[4] = {REGION_PERMIT_DEFAULT_SETTING0, 0, 0, 0};
static const FW_MASTER_PARAM_S fw_mst[] = {
	{
		.mst_id = MASTER_ID_CA55,
		.fw_port = FW_PORT0,
		.port_ind = 0
	},
	{
		.mst_id = MASTER_ID_SDIO,
		.fw_port = FW_PORT3,
		.port_ind = 0
	},
	{
		.mst_id = MASTER_ID_USB2,
		.fw_port = FW_PORT3,
		.port_ind = 1
	},
	{
		.mst_id = MASTER_ID_EMMC,
		.fw_port = FW_PORT3,
		.port_ind = 2
	},
	{
		.mst_id = MASTER_ID_DMA0,
		.fw_port = FW_PORT4,
		.port_ind = 0
	},
	{
		.mst_id = MASTER_ID_DMA_XOR,
		.fw_port = FW_PORT4,
		.port_ind = 1
	},
	{
		.mst_id = MASTER_ID_VDSP_M1,
		.fw_port = FW_PORT0,
		.port_ind = 1
	},
	{
		.mst_id = MASTER_ID_VDSP_M2,
		.fw_port = FW_PORT4,
		.port_ind = 2
	},
	{
		.mst_id = MASTER_ID_VDSP_M3,
		.fw_port = FW_PORT0,
		.port_ind = 2
	},
	{
		.mst_id = MASTER_ID_VDSP_M4,
		.fw_port = FW_PORT4,
		.port_ind = 3
	},
	{
		.mst_id = MASTER_ID_ISP,
		.fw_port = FW_PORT1,
		.port_ind = 0
	},
	{
		.mst_id = MASTER_ID_VDEC,
		.fw_port = FW_PORT1,
		.port_ind = 1
	},
	{
		.mst_id = MASTER_ID_VENC0,
		.fw_port = FW_PORT3,
		.port_ind = 3
	},
	{
		.mst_id = MASTER_ID_VENC1,
		.fw_port = FW_PORT3,
		.port_ind = 4
	},
	{
		.mst_id = MASTER_ID_MM,
		.fw_port = FW_PORT4,
		.port_ind = 4
	},
	{
		.mst_id = MASTER_ID_JDEC,
		.fw_port = FW_PORT4,
		.port_ind = 5
	},
	{
		.mst_id = MASTER_ID_SEC_CE,
		.fw_port = FW_PORT0,
		.port_ind = 3
	},
	{
		.mst_id = MASTER_ID_PUB_CE,
		.fw_port = FW_PORT0,
		.port_ind = 4
	},
	{
		.mst_id = MASTER_ID_RISCV,
		.fw_port = FW_PORT0,
		.port_ind = 5
	},
	{
		.mst_id = MASTER_ID_SATA0,
		.fw_port = FW_PORT4,
		.port_ind = 6
	},
	{
		.mst_id = MASTER_ID_PCIE0,
		.fw_port = FW_PORT4,
		.port_ind = 7
	},
	{
		.mst_id = MASTER_ID_PCIE1,
		.fw_port = FW_PORT4,
		.port_ind = 8
	},
	{
		.mst_id = MASTER_ID_USB0,
		.fw_port = FW_PORT4,
		.port_ind = 9
	},
	{
		.mst_id = MASTER_ID_USB1,
		.fw_port = FW_PORT4,
		.port_ind = 10
	},
	{
		.mst_id = MASTER_ID_EMAC1,
		.fw_port = FW_PORT4,
		.port_ind = 11
	},
	{
		.mst_id = MASTER_ID_SPI_AHB,
		.fw_port = FW_PORT0,
		.port_ind = 6
	},
	{
		.mst_id = MASTER_ID_JTAG,
		.fw_port = FW_PORT0,
		.port_ind = 7
	},
	{
		.mst_id = MASTER_ID_EMAC0,
		.fw_port = FW_PORT4,
		.port_ind = 12
	},
	{
		.mst_id = MASTER_ID_SDIO1,
		.fw_port = FW_PORT4,
		.port_ind = 13
	},
	{
		.mst_id = MASTER_ID_DW_DMA,
		.fw_port = FW_PORT3,
		.port_ind = 5
	}
};

static void enable_firewall_region(u8 region_id)
{
	int i = 0;
	if(region_id >= REGION_NUM){
		return;
	}
	for(i = 0;i<ARRAY_SIZE(port_base);i++){
		writel(1, (ulong)(REGION_EN(port_base[i]) + region_id * REGION_SETTING_REG_SIZE));
	}
}
static int set_firewall_region(u8 region_id, u64 start, u64 end, u64 *permitted)
{
	u32 prmtd = 0;
	int i = 0;
	if(region_id >= REGION_NUM){
		return -1;
	}
	start = start >> 12;
	end = end >> 12;
	if(end < start) {
		return -1;
	}
	for(i = 0; i < ARRAY_SIZE(port_base); i++){
		/*Set region start address and end address.*/
		writel(start, (ulong)(REGION_START(port_base[i]) + region_id*REGION_SETTING_REG_SIZE));
		writel(end, (ulong)(REGION_END(port_base[i]) + region_id*REGION_SETTING_REG_SIZE));

		/*Set permitted masters.*/
		prmtd = (u32)(permitted[i] & 0xFFFFFFFFU);
		writel(prmtd, (ulong)(REGION_PERMITTED_MASTER0(port_base[i]) + region_id * REGION_SETTING_REG_SIZE));
		prmtd = (u32)((permitted[i] >> 32) & 0xFFFFFFFFU);
		writel(prmtd, (ulong)(REGION_PERMITTED_MASTER1(port_base[i]) + region_id * REGION_SETTING_REG_SIZE));

		/*Update*/
		writel(1, (ulong)(REGION_UPD_PRMTD_MASTER(port_base[i]) + region_id * REGION_SETTING_REG_SIZE));
	}
	return 0;
}
static void init_firewall_masters(void)
{
	int i = 0;
	for(i = 0;i < ARRAY_SIZE(fw_mst);i++)
	{
		writel(fw_mst[i].mst_id, (ulong)(CHOSEN_MASTER_ID(fw_mst[i].fw_port) + fw_mst[i].port_ind * 8));
		writel(0, (ulong)(IGNORE_MASTER_PROT(fw_mst[i].fw_port) + fw_mst[i].port_ind * 8));
	}
}
static void set_sec_ce_sec_mode(void)
{
	writel(0, (ulong)(SEC_COMMON_CE_AR));
	writel(0, (ulong)(SEC_COMMON_CE_AW));
}
int firewall_init(ulong optee_mem_start, ulong optee_mem_end)
{
	u64 ddr_start_offset;
	u64 ddr_end_offset;
	ddr_start_offset = optee_mem_start - DDR_BASE;
	ddr_end_offset = optee_mem_end - DDR_BASE;
	/*release_firewall_rst*/
	writel(0x1, SW_RST_0_CLR);
	/*enable_firewall_clk*/
	writel((0x1 << 7), CLK_EB_0_SET);
	init_firewall_masters();
	set_firewall_region(0, ddr_start_offset, ddr_end_offset, permitted_setting);
	enable_firewall_region(0);
	set_sec_ce_sec_mode();
	return 0;
}
