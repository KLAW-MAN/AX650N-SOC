/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#include <linux/types.h>
#include <linux/module.h>
#include <linux/of_gpio.h>
#include <linux/platform_device.h>
#include <linux/fs.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/delay.h>
#include <linux/uaccess.h>
#include <linux/mutex.h>
#include <linux/clk.h>
#include <linux/clk-provider.h>
#include <linux/clk/clk-conf.h>
#include <linux/notifier.h>
#include <linux/clkdev.h>
#include "../../../../fs/proc/internal.h"
#include <linux/soc/axera/ax_bw_limiter.h>

#define DRIVER_NAME         "ax_bw_limiter"
#define NO_EXIST            -1
#define BIT_WIDTH_32        32
#define BIT_WIDTH_64        64
#define BIT_WIDTH_128       128
#define BIT_WIDTH_256       256

#define DEFAULT_CLK_800M (800)
#define DEFAULT_CLK_700M (700)
#define DEFAULT_CLK_624M (624)
#define DEFAULT_CLK_500M (500)

#define PROC_NODE_ROOT_NAME         "ax_proc/bw_limit"
#define PROC_BW_LIMITER_VAL_RD      "limiter_val_rd"
#define PROC_BW_LIMITER_VAL_WR      "limiter_val_wr"
#define CHECK_SYS_ENABLE
#ifdef BW_DEBUG_LOG_EN
#define BW_LOG_DBG(fmt, arg...) printk(KERN_DEBUG "[%s : %d] " fmt "\n", __func__, __LINE__, ##arg)
#else
#define BW_LOG_DBG(fmt, arg...)
#endif
#define BW_LOG_ERR(fmt, arg...) printk(KERN_ERR "[%s : %d] " fmt "\n", __func__, __LINE__, ##arg)

static struct proc_dir_entry *bw_limiter_root;
static DEFINE_MUTEX(ax_bw_limiter_mutex);

typedef enum {
	BW_PORT_RD,
	BW_PORT_WR,
	BW_PORT_END
} BW_OPS_E;

typedef struct {
	u32 limiter_enable:    1;
	u32 auto_limiter_wr:   1;
	u32 auto_limiter_rd:   1;
	u32 interl_value:      5;
} bw_limiter_ctrl_t;

typedef struct {
	u32 limiter_value_rd: 14;
	u32 limiter_value_wr: 14;
} bw_limiter_value_t;

/*  define base type to control bandwidth   */
typedef struct {
	u32 limiter_base_paddr;
	u32 limiter_update_paddr_offset;
	u32 limiter_ctrl_paddr_offset;
	u32 limiter_value_paddr_offset;
	u32 bus_bitwidth;
} bw_limit_t;

typedef struct {
	u32 index;
	char proc_name[32];
	bw_limit_t *bw_limiter;
	struct proc_dir_entry *root_proc_file;
	struct proc_dir_entry *proc_rfile;
	struct proc_dir_entry *proc_wfile;
	u32 bwLimiter_value_rd;
	u32 bwLimiter_value_wr;
	u32 clk;
	bool sub_sys_enabled;
	struct clk *pclk;
	struct notifier_block nb;
} proc_node_info_t;

bw_limit_t bl_ddr_port0 = {
	.limiter_base_paddr             = 0x0450F000,
	.limiter_update_paddr_offset    = 0xEC,
	.limiter_ctrl_paddr_offset      = 0xEC,
	.limiter_value_paddr_offset     = 0xF0,
	.bus_bitwidth                   = BIT_WIDTH_128
};

bw_limit_t bl_sysvdec       = {0x19000000, NO_EXIST, 0x148, 0x30, BIT_WIDTH_128};
bw_limit_t bl_sysvenc       = {0x1A000000, 0x1F8, 0x2B8, 0x204, BIT_WIDTH_128};
bw_limit_t bl_sata0         = {0x30000000, 0x78, 0x1F0, 0x7c, BIT_WIDTH_64};
bw_limit_t bl_dma_xor       = {0x30000000, 0x60, 0x1E4, 0x64, BIT_WIDTH_64};
bw_limit_t bl_dma0          = {0x30000000, 0x58, 0x1E0, 0x5c, BIT_WIDTH_64};
bw_limit_t bl_dma1          = {NO_EXIST, NO_EXIST, NO_EXIST, NO_EXIST, NO_EXIST};
bw_limit_t bl_pcie0         = {0x30000000, 0x68, 0x1E8, 0x6c, BIT_WIDTH_64};
bw_limit_t bl_pcie1         = {0x30000000, 0x70, 0x1EC, 0x74, BIT_WIDTH_64};
bw_limit_t bl_mm_dpu_0      = {0x10000000, 0x20C, 0x218, 0x254, BIT_WIDTH_128};
bw_limit_t bl_mm_dpu_1      = {0x10000000, 0x224, 0x230, 0x260, BIT_WIDTH_128};
bw_limit_t bl_mm_axi2csitx  = {0x10000000, 0x23C, 0x248, 0x26c, BIT_WIDTH_128};
bw_limit_t bl_mm_cdma       = {0x10101000, 0x174, 0x178, 0x17c, BIT_WIDTH_128};
bw_limit_t bl_mm_vgp        = {0x10101000, 0x174, 0x178, 0x180, BIT_WIDTH_128};
bw_limit_t bl_mm_ive        = {0x10101000, 0x174, 0x178, 0x184, BIT_WIDTH_128};
bw_limit_t bl_mm_pyra       = {0x10101000, 0x174, 0x178, 0x188, BIT_WIDTH_128};
bw_limit_t bl_mm_gdc        = {0x10101000, 0x174, 0x178, 0x18c, BIT_WIDTH_128};
bw_limit_t bl_mm_yuvwdma    = {0x10101000, 0x174, 0x178, 0x190, BIT_WIDTH_128};
bw_limit_t bl_vdsp0dma      = {0x18000000, 0x68, 0x120, 0x6c, BIT_WIDTH_128};
bw_limit_t bl_vdsp1dma      = {0x18000000, 0xdc, 0x124, 0xE0, BIT_WIDTH_128};
bw_limit_t bl_npu_vec_0     = {0x164E0000, NO_EXIST, 0x84, 0x88, BIT_WIDTH_256};
bw_limit_t bl_npu_vec_1     = {0x164E0000, NO_EXIST, 0x8c, 0x90, BIT_WIDTH_256};
bw_limit_t bl_npu_vec_2     = {0x164E0000, NO_EXIST, 0x94, 0x98, BIT_WIDTH_256};
bw_limit_t bl_npu_interl    = {0x164E0000, NO_EXIST, 0x70, 0x74, BIT_WIDTH_256};
bw_limit_t bl_isp_ife_m1    = {0x12000000, NO_EXIST, 0x340, 0x344, BIT_WIDTH_128};
bw_limit_t bl_isp_ife_wdma_m1 = {0x12000000, NO_EXIST, 0x348, 0x34C, BIT_WIDTH_128};
bw_limit_t bl_isp_itp_tdp0  = {0x12080000, NO_EXIST, 0x150, 0x154, BIT_WIDTH_128};
bw_limit_t bl_isp_itp_tdp1  = {0x12080000, NO_EXIST, 0x158, 0x15C, BIT_WIDTH_128};
bw_limit_t bl_isp_itp_m1    = {0x12080000, NO_EXIST, 0x148, 0x14C, BIT_WIDTH_128};
bw_limit_t bl_isp_pipe_vpp  = {0x120C0000, NO_EXIST, 0x198, 0x19C, BIT_WIDTH_128};
bw_limit_t bl_isp_yuv_m1    = {0x120C0000, NO_EXIST, 0x190, 0x194, BIT_WIDTH_128};

static proc_node_info_t sys_proc_node_array[BL_NUM_MAX] = {
	{BL_DDR_PORT0, "ddr_port0", &bl_ddr_port0, NULL, NULL, NULL, 0, 0, DEFAULT_CLK_800M, false},
	{BL_SYSVDEC, "sysvdec", &bl_sysvdec, NULL, NULL, NULL, 0, 0, DEFAULT_CLK_624M, false},
	{BL_STSVENC, "sysvenc", &bl_sysvenc, NULL, NULL, NULL, 0, 0,  DEFAULT_CLK_800M, false},
	{BL_SATA0, "sata0", &bl_sata0, NULL, NULL, NULL, 0, 0, DEFAULT_CLK_500M, false},
	{BL_DMA_XOR, "dma_xor", &bl_dma_xor, NULL, NULL, NULL, 0, 0, DEFAULT_CLK_500M, false},
	{BL_DMA0, "dma0", &bl_dma0, NULL, NULL, NULL, 0, 0, DEFAULT_CLK_500M, false},
	{BL_DMA1, "dma1", &bl_dma1, NULL, NULL, NULL, 0, 0, DEFAULT_CLK_500M,  false},
	{BL_PCIE0, "pcie0", &bl_pcie0, NULL, NULL, NULL, 0, 0, DEFAULT_CLK_500M,  false},
	{BL_PCIE1, "pcie1", &bl_pcie1, NULL, NULL, NULL, 0, 0, DEFAULT_CLK_500M,  false},
	{BL_MM_DPU_0, "mm_dpu_0", &bl_mm_dpu_0, NULL, NULL, NULL, 0, 0, DEFAULT_CLK_624M,  false},
	{BL_MM_DPU_1, "mm_dpu_1", &bl_mm_dpu_1, NULL, NULL, NULL, 0, 0, DEFAULT_CLK_624M,  false},
	{BL_MM_AXI2CSITX, "mm_axi2csitx", &bl_mm_axi2csitx, NULL, NULL, NULL, 0, 0,  false},
	{BL_MM_CDMA, "mm_cdma", &bl_mm_cdma, NULL, NULL, NULL, 0, 0, DEFAULT_CLK_624M,  false},
	{BL_MM_VGP, "mm_vgp", &bl_mm_vgp, NULL, NULL, NULL, 0, 0, DEFAULT_CLK_624M,  false},
	{BL_MM_IVE, "mm_ive", &bl_mm_ive, NULL, NULL, NULL, 0, 0, DEFAULT_CLK_624M,  false},
	{BL_MM_PYRA, "mm_pyra", &bl_mm_pyra, NULL, NULL, NULL, 0, 0, DEFAULT_CLK_624M,  false},
	{BL_MM_GDC, "mm_gdc", &bl_mm_gdc, NULL, NULL, NULL, 0, 0, DEFAULT_CLK_624M,  false},
	{BL_MM_YUVWDMA, "mm_yuvwdma", &bl_mm_yuvwdma, NULL, NULL, NULL, 0, 0, DEFAULT_CLK_624M,  false},
	{BL_VDSP0DMA, "vdsp0dma", &bl_vdsp0dma, NULL, NULL, NULL, 0, 0, DEFAULT_CLK_800M,  false},
	{BL_VDSP1DMA, "vdsp1dma", &bl_vdsp1dma, NULL, NULL, NULL, 0, 0, DEFAULT_CLK_800M,  false},
	{BL_NPU_VEC_0, "npu_vec_0", &bl_npu_vec_0, NULL, NULL, NULL, 0, 0, DEFAULT_CLK_800M,  false},
	{BL_NPU_VEC_1, "npu_vec_1", &bl_npu_vec_1, NULL, NULL, NULL, 0, 0, DEFAULT_CLK_800M,  false},
	{BL_NPU_VEC_2, "npu_vec_2", &bl_npu_vec_2, NULL, NULL, NULL, 0, 0, DEFAULT_CLK_800M,  false},
	{BL_NPU_INTERLEAVE, "npu_interl", &bl_npu_interl, NULL, NULL, NULL, 0, 0, DEFAULT_CLK_800M,   false},
	{BL_ISP_IFE_M1, "isp_ife_m1", &bl_isp_ife_m1, NULL, NULL, NULL, 0, 0, DEFAULT_CLK_624M,  false},
	{BL_ISP_IFE_WDAM_M1, "isp_ife_wdma_m1", &bl_isp_ife_wdma_m1, NULL, NULL, NULL, 0, 0, DEFAULT_CLK_624M,  false},
	{BL_ISP_ITP_TDP0, "isp_itp_tdp0", &bl_isp_itp_tdp0, NULL, NULL, NULL, 0, 0, DEFAULT_CLK_700M,  false},
	{BL_ISP_ITP_TDP1, "isp_itp_tdp1", &bl_isp_itp_tdp1, NULL, NULL, NULL, 0, 0, DEFAULT_CLK_700M,  false},
	{BL_ISP_ITP_M1, "isp_itp_m1", &bl_isp_itp_m1, NULL, NULL, NULL, 0, 0, DEFAULT_CLK_700M,  false},
	{BL_ISP_PIPE_VPP, "isp_pipe_vpp", &bl_isp_pipe_vpp, NULL, NULL, NULL, 0, 0, DEFAULT_CLK_700M,  false},
	{BL_ISP_YUV_M1, "isp_yuv_m1", &bl_isp_yuv_m1, NULL, NULL, NULL, 0, 0, DEFAULT_CLK_700M,  false}
};

static atomic_t multisys_registered_cnt = ATOMIC_INIT(0);

static bool bw_is_DDR_PORT0(bw_limit_t *limiter)
{
	if (limiter && limiter->limiter_base_paddr == 0x0450F000) {
		return true;
	}
	return false;
}

static bool bw_is_SYS_VENC(bw_limit_t *limiter)
{
	if (limiter && limiter->limiter_base_paddr == 0x1A000000) {
		return true;
	}
	return false;
}

static bool bw_is_ISP_SYS(bw_limit_t *limiter)
{
	if (limiter && (limiter->limiter_base_paddr == 0x12000000
		|| limiter->limiter_base_paddr == 0x12080000
		|| limiter->limiter_base_paddr == 0x120C0000)) {
		return true;
	}
	return false;
}

static u32 bw_swap_wr_rd(u32 value, u32 pos)
{
	u32 temp = 0;
	u32 mask_bits = (1 << 14) - 1;
	if (!value) {
		return 0;
	}

	temp = value & mask_bits;
	temp = temp << pos;
	temp |= (value >> pos) & mask_bits;
	return temp;
}

static int ddr_bw_limiter_ctrl(bw_limit_t *limiter, bw_limiter_ctrl_t *ctrl, bw_limiter_value_t *value, BW_OPS_E bw_rw)
{
	u32 ctrl_val;
	u32 limiter_val;
	u32 limiter_update;
	void *limiter_base_vaddr, *temp_vaddr;
	u32 shift_pos = 0;
	BW_LOG_DBG("start");

	if (ctrl == NULL || limiter == NULL) {
		BW_LOG_ERR("one of parameters is NULL !!!");
		return -1;
	}

	if (limiter->limiter_base_paddr == NO_EXIST) {
		BW_LOG_ERR("this BW Limiter doesn't exist");
		return 0;
	}

	limiter_base_vaddr = ioremap(limiter->limiter_base_paddr, 0x1000);
	if (!limiter_base_vaddr) {
		BW_LOG_ERR("ioremap failed !!!");
		return -1;
	}

	limiter_val = readl(limiter_base_vaddr + limiter->limiter_value_paddr_offset);
	if (bw_is_SYS_VENC(limiter)) {
		limiter_val = bw_swap_wr_rd(limiter_val, 14);
	}

	if (bw_is_DDR_PORT0(limiter)) {
		limiter_val = bw_swap_wr_rd(limiter_val, 16);
	}

	BW_LOG_DBG("limiter_value_wr = 0x%x, value_rd = 0x%x ", value->limiter_value_wr, value->limiter_value_rd);
	if (ctrl->limiter_enable) {
		if (bw_is_DDR_PORT0(limiter)) {
			ctrl_val = (ctrl->limiter_enable & 0x1) << 1 | (ctrl->auto_limiter_wr & 0x1) << 2
						| (ctrl->auto_limiter_rd & 0x1) << 3 | (ctrl->interl_value & 0x3F) << 4;
			shift_pos = 16;
		} else {
			ctrl_val = (ctrl->limiter_enable & 0x1) << 8 | (ctrl->auto_limiter_wr & 0x1) << 7
						| (ctrl->auto_limiter_rd & 0x1) << 6 | (ctrl->interl_value & 0x3F);
			if (bw_is_ISP_SYS(limiter)) {
				shift_pos = 16;
			} else {
				shift_pos = 14;
			}
		}

		if (bw_rw == BW_PORT_RD) {
			limiter_val &= ~0x3FFF;
			limiter_val |= (value->limiter_value_rd & 0x3FFF);
		} else if (bw_rw == BW_PORT_WR) {
			limiter_val &= ~(0x3FFF << shift_pos);
			limiter_val |= (value->limiter_value_wr & 0x3FFF) << shift_pos;
		} else {
			BW_LOG_ERR("don't support this Mode : %d", bw_rw);
		}
	} else {
		ctrl_val = 0x0;
		limiter_val = 0x0;
	}
	BW_LOG_DBG("bw limiter ctrl_val = 0x%x, new limiter_val = 0x%x", ctrl_val, limiter_val);
#ifdef CHECK_SYS_ENABLE
	writel(ctrl_val, limiter_base_vaddr + limiter->limiter_ctrl_paddr_offset);

	temp_vaddr = limiter_base_vaddr + limiter->limiter_value_paddr_offset;
	if (bw_is_SYS_VENC(limiter)) {
		writel(bw_swap_wr_rd(limiter_val, 14), temp_vaddr);
	} else if (bw_is_DDR_PORT0(limiter)) {
		writel(bw_swap_wr_rd(limiter_val, 16), temp_vaddr);
	} else {
		writel(limiter_val, temp_vaddr);
	}
#endif

	if (limiter->limiter_update_paddr_offset != NO_EXIST) {
		temp_vaddr = limiter_base_vaddr + limiter->limiter_update_paddr_offset;
		limiter_update = readl(temp_vaddr);
		if (bw_is_SYS_VENC(limiter)) {
			writel(limiter_update | (1 << 1), temp_vaddr);
		} else if (bw_is_DDR_PORT0(limiter)) {
			writel((ctrl_val | (1 << 0)) | limiter_update, temp_vaddr);
		} else {
			writel(limiter_update | (1 << 0), temp_vaddr);
		}
		mdelay(1);
		writel(limiter_update & ~(1 << 0), temp_vaddr);
	}

	if (limiter_base_vaddr) {
		iounmap(limiter_base_vaddr);
	}
	BW_LOG_DBG("end");
	return 0;
}

int bw_limiter_set(bw_limit_t *limiter, u32 bus_clk, u32 bw_val, BW_OPS_E bw_rw)
{
	int ret = 0;
	u32 value;
	bw_limiter_ctrl_t limiter_ctrl;
	bw_limiter_value_t limiter_value;
	#define ONE_MB 1000000				/*	1MB			*/
	#define UINT_1S 1000000				/*	1000000us	*/
	#define WINDOW_INTERVAL 10			/*	1.0us		*/

	BW_LOG_DBG("this limiter 's bw_val = %d MB, bw_ops = %s", bw_val, bw_rw == BW_PORT_WR ? "wr" : "rd");
	if (bw_val == 0) {
		limiter_ctrl.limiter_enable = 0;
	} else {
		limiter_ctrl.limiter_enable = 1;
	}
	limiter_ctrl.auto_limiter_wr = 1;
	limiter_ctrl.auto_limiter_rd = 1;

	/* 1. get bus clk then to computer limiter_value */
	limiter_ctrl.interl_value = (bus_clk * WINDOW_INTERVAL / 10) >> 8;

	/* 2. computer value on base of bw ,like 1000 = 1GB */
	/*    the unit of bw_val is MB */
	value = (bw_val * WINDOW_INTERVAL / 10) / (limiter->bus_bitwidth >> 3);
	BW_LOG_DBG("value 1= %d, 2= %d", (bw_val * WINDOW_INTERVAL / 10), (limiter->bus_bitwidth >> 3));

	if (bw_rw == BW_PORT_WR) {
		limiter_value.limiter_value_wr = value;
	} else if (bw_rw == BW_PORT_RD) {
		limiter_value.limiter_value_rd = value;
	}

	ret = ddr_bw_limiter_ctrl(limiter, &limiter_ctrl, &limiter_value, bw_rw);
	if (ret) {
		BW_LOG_ERR("ddr limiter set failed ");
		return ret;
	}

	return ret;
}

static int bw_find_id_by_proc_name(const char *name)
{
	u32 i;
	if (!name) {
		return -1;
	}

	for (i = 0; i < BL_NUM_MAX; i++) {
		if (!strcmp(sys_proc_node_array[i].proc_name, name)) {
			break;
		}
	}
	return i;
}

/*
	convert inputed string to integer
*/
static int bw_limiter_str2int(const char __user *buffer, u32 count, u32 *data)
{
	char kbuf[8];
	u32 i, temp;
	u32 input_flag = 0;
	u32 value = 0;

	if (count > 8) {
		return -1;
	}

	if (copy_from_user(kbuf, buffer, count)) {
		return -EFAULT;
	}

	for (i = 0; i < count; i++) {
		if ((kbuf[i] >= '0') && (kbuf[i] <= '9')) {
			input_flag = 1;
			temp = kbuf[i] - '0';
			value = value * 10 + temp;
		} else {
			if (input_flag) {
				break;
			} else {
				return -1;
			}
		}
	}

	if (data != NULL) {
		*data = value;
	} else {
		BW_LOG_ERR(" data is NULL ");
		return -1;
	}

	return 0;
}

static int bw_find_id_by_clk(struct clk *pclk)
{
	u32 i;
	for (i = 0; i < BL_NUM_MAX; i++) {
		if ((sys_proc_node_array[i].pclk == pclk)) {
			break;
		}
	}
	return i;
}

static int bw_find_id_by_proc_entry(struct proc_dir_entry *entry, BW_OPS_E ops_rw)
{
	u32 i;
	if (!entry) {
		return -1;
	}
	BW_LOG_DBG("bw_find_id_by_proc_entry entry= %px", entry);
	for (i = 0; i < BL_NUM_MAX; i++) {
		if ((ops_rw == BW_PORT_RD && sys_proc_node_array[i].proc_rfile == entry)
			|| (ops_rw == BW_PORT_WR && (sys_proc_node_array[i].proc_wfile == entry))) {
			BW_LOG_DBG("ops_rw = %d, [%d].proc_rfile = %px, proc_wfile = %px", ops_rw, i,
			sys_proc_node_array[i].proc_rfile,
			sys_proc_node_array[i].proc_wfile);
			break;
		}
	}
	return i;
}

static ssize_t ax_bw_limiter_rd_proc_write(struct file *file, const char __user *buffer, size_t count, loff_t *ppos)
{
	u32 id;
	u32 *bw_value;
	struct proc_dir_entry *entry;

	entry = PDE_DATA(file_inode(file));
	mutex_lock(&ax_bw_limiter_mutex);

	id = bw_find_id_by_proc_entry(entry, BW_PORT_RD);
	if (id >= BL_NUM_MAX) {
		mutex_unlock(&ax_bw_limiter_mutex);
		BW_LOG_ERR(" fail to find the right id of sub sys\n");
		return -1;
	}

	bw_value = &sys_proc_node_array[id].bwLimiter_value_rd;
	if (bw_limiter_str2int(buffer, count, bw_value)) {
		mutex_unlock(&ax_bw_limiter_mutex);
		BW_LOG_ERR(" fail to convert integer value \n");
		return -1;
	}

#ifdef CHECK_SYS_ENABLE
	if (sys_proc_node_array[id].sub_sys_enabled == false) {
		mutex_unlock(&ax_bw_limiter_mutex);
		BW_LOG_ERR("sub sys[%s] is not ready, please first power it!\n", sys_proc_node_array[id].proc_name);
		return -1;
	}
#endif
	bw_limiter_set(sys_proc_node_array[id].bw_limiter, sys_proc_node_array[id].clk, *bw_value, BW_PORT_RD);
	mutex_unlock(&ax_bw_limiter_mutex);

	return count;
}

static int ax_bw_limiter_rd_proc_show(struct seq_file *m, void *v)
{
	u32 id;
	struct proc_dir_entry *local_entry;
	mutex_lock(&ax_bw_limiter_mutex);

	local_entry = PDE_DATA(file_inode(m->file));

	BW_LOG_DBG("ax_bw_limiter_rd_proc_read = %px", local_entry);
	id = bw_find_id_by_proc_entry(local_entry, BW_PORT_RD);
	if (id >= BL_NUM_MAX) {
		mutex_unlock(&ax_bw_limiter_mutex);
		BW_LOG_ERR(" fail to find the right id[%d] of sub sys\n", id);
		return -1;
	}

	seq_printf(m, "%d MB\n", sys_proc_node_array[id].bwLimiter_value_rd);
	mutex_unlock(&ax_bw_limiter_mutex);
	return 0;
}

static int ax_bw_limiter_rd_proc_open(struct inode *inode, struct file *file)
{
	return single_open(file, ax_bw_limiter_rd_proc_show, NULL);
}

static struct proc_ops ax_bw_limiter_rd_fops = {
	.proc_open   = ax_bw_limiter_rd_proc_open,
	.proc_read   = seq_read,
	.proc_write = ax_bw_limiter_rd_proc_write,
	.proc_release = single_release,
};

static ssize_t ax_bw_limiter_wr_proc_write(struct file *file, const char __user *buffer, size_t count, loff_t *ppos)
{
	u32 id;
	u32 *bw_value;
	struct proc_dir_entry *entry;

	mutex_lock(&ax_bw_limiter_mutex);

	entry = PDE_DATA(file_inode(file));
	id = bw_find_id_by_proc_entry(entry, BW_PORT_WR);
	if (id >= BL_NUM_MAX) {
		mutex_unlock(&ax_bw_limiter_mutex);
		BW_LOG_ERR(" fail to find the right id of sub sys\n");
		return -1;
	}
	bw_value = &sys_proc_node_array[id].bwLimiter_value_wr;
	if (bw_limiter_str2int(buffer, count, bw_value)) {
		mutex_unlock(&ax_bw_limiter_mutex);
		BW_LOG_ERR(" fail to convert integer value \n");
		return -1;
	}

#ifdef CHECK_SYS_ENABLE
	if (sys_proc_node_array[id].sub_sys_enabled == false) {
		mutex_unlock(&ax_bw_limiter_mutex);
		BW_LOG_ERR("sub sys[%s] is not ready, please first power it!\n", sys_proc_node_array[id].proc_name);
		return -1;
	}
#endif
	bw_limiter_set(sys_proc_node_array[id].bw_limiter, sys_proc_node_array[id].clk, *bw_value, BW_PORT_WR);
	mutex_unlock(&ax_bw_limiter_mutex);

	return count;
}

static int ax_bw_limiter_wr_proc_show(struct seq_file *m, void *v)
{
	u32 id;
	struct proc_dir_entry *local_entry;
	mutex_lock(&ax_bw_limiter_mutex);

	local_entry = PDE_DATA(file_inode(m->file));

	BW_LOG_DBG("ax_bw_limiter_wr_proc_read = %px", local_entry);
	id = bw_find_id_by_proc_entry(local_entry, BW_PORT_WR);
	if (id >= BL_NUM_MAX) {
		mutex_unlock(&ax_bw_limiter_mutex);
		BW_LOG_ERR(" fail to find the right id[%d] of sub sys\n", id);
		return -1;
	}

	seq_printf(m, "%d MB\n", sys_proc_node_array[id].bwLimiter_value_wr);
	mutex_unlock(&ax_bw_limiter_mutex);
	return 0;
}

static int ax_bw_limiter_wr_proc_open(struct inode *inode, struct file *file)
{
	return single_open(file, ax_bw_limiter_wr_proc_show, NULL);
}

static struct proc_ops ax_bw_limiter_wr_fsops = {
	.proc_open   = ax_bw_limiter_wr_proc_open,
	.proc_read   = seq_read,
	.proc_write = ax_bw_limiter_wr_proc_write,
	.proc_release = single_release,
};

static int ax_bw_limiter_proc_create_file(const char *proc_limiter)
{
	u32 id;
	struct proc_dir_entry *bw_sys_limiter_root;
	struct proc_dir_entry *bw_limiter_val_rfile;
	struct proc_dir_entry *bw_limiter_val_wfile;

	if (!proc_limiter) {
		return -1;
	}

	BW_LOG_DBG("generated proc name : %s", proc_limiter);
	bw_sys_limiter_root = proc_mkdir(proc_limiter, bw_limiter_root);
	bw_limiter_val_rfile = proc_create_data(PROC_BW_LIMITER_VAL_RD, 0644, bw_sys_limiter_root, &ax_bw_limiter_rd_fops, NULL);
	if (!bw_limiter_val_rfile) {
		BW_LOG_ERR("create proc bw_limiter_rd failen\n");
		return -1;
	}
	bw_limiter_val_rfile->data = bw_limiter_val_rfile;

	bw_limiter_val_wfile = proc_create_data(PROC_BW_LIMITER_VAL_WR, 0644, bw_sys_limiter_root, &ax_bw_limiter_wr_fsops, NULL);
	if (!bw_limiter_val_wfile) {
		BW_LOG_ERR("create proc bw_limiter_wr failen\n");
		return -1;
	}
	bw_limiter_val_wfile->data = bw_limiter_val_wfile;

	id = bw_find_id_by_proc_name(proc_limiter);
	if (id >= BL_NUM_MAX) {
		BW_LOG_ERR("cann't find the right id to index sub sys\n");
		return -1;
	}

	sys_proc_node_array[id].root_proc_file = bw_sys_limiter_root;
	sys_proc_node_array[id].proc_rfile = bw_limiter_val_rfile;
	sys_proc_node_array[id].proc_wfile = bw_limiter_val_wfile;
	return 0;
}

static int ax_bw_limiter_probe(struct platform_device *pdev)
{
	bw_limiter_root = proc_mkdir(PROC_NODE_ROOT_NAME, NULL);
	if (bw_limiter_root == NULL) {
		BW_LOG_ERR("create bw limiter root proc node failed!\n");
		return -1;
	}
	ax_bw_limiter_register_with_val(BL_DDR_PORT0, DEFAULT_CLK_800M);
	ax_bw_limiter_register_with_val(BL_PCIE0, DEFAULT_CLK_500M);
	ax_bw_limiter_register_with_val(BL_PCIE1, DEFAULT_CLK_500M);
	ax_bw_limiter_register_with_val(BL_SATA0, DEFAULT_CLK_500M);
	return 0;
}

static int ax_bw_limiter_remove(struct platform_device *pdev)
{
	ax_bw_limiter_unregister(BL_SATA0, NULL);
	ax_bw_limiter_unregister(BL_PCIE1, NULL);
	ax_bw_limiter_unregister(BL_PCIE0, NULL);
	ax_bw_limiter_unregister(BL_DDR_PORT0, NULL);
	remove_proc_entry(PROC_NODE_ROOT_NAME, NULL);
	return 0;
}

static int ax_bw_clk_change_cb_clkevt(struct notifier_block *nb, unsigned long event, void *data)
{
	u32 id;
	struct clk_notifier_data *ndata = data;
	unsigned long cur_rate = ndata->new_rate;
	struct clk *cur_clk = ndata->clk;
	BW_LOG_DBG(" enter , clk =0x%px", cur_clk);

	mutex_lock(&ax_bw_limiter_mutex);
	if (event == POST_RATE_CHANGE) {
		id = bw_find_id_by_clk(cur_clk);
		if (id >= BL_NUM_MAX) {
			mutex_unlock(&ax_bw_limiter_mutex);
			BW_LOG_ERR("fail to get the right sys id [%d]", id);
			return -1;
		}
		sys_proc_node_array[id].clk = cur_rate / 1000000 ;
		bw_limiter_set(sys_proc_node_array[id].bw_limiter, sys_proc_node_array[id].clk,
						sys_proc_node_array[id].bwLimiter_value_rd, BW_PORT_RD);
		bw_limiter_set(sys_proc_node_array[id].bw_limiter, sys_proc_node_array[id].clk,
						sys_proc_node_array[id].bwLimiter_value_wr, BW_PORT_WR);
	}
	mutex_unlock(&ax_bw_limiter_mutex);
	return 0;
}

int ax_bw_limiter_clk_notifier_register(SUB_SYS_BW_LIMITERS sub_sys_bw, struct clk *pclk)
{
	struct notifier_block *pnb = &sys_proc_node_array[sub_sys_bw].nb;
	if (sub_sys_bw < 0 || sub_sys_bw >= BL_NUM_MAX) {
		BW_LOG_ERR("invalid sub_sys_bw value\n");
		return -1;
	}

	pnb->notifier_call = ax_bw_clk_change_cb_clkevt;
	pnb->next = NULL;

	if (clk_notifier_register(pclk, pnb)) {
		BW_LOG_ERR("fail to register clk cb");
		return -1;
	}

	BW_LOG_DBG("clk[0x%px] notifier register done", pclk);
	return 0;
}

int ax_bw_limiter_refresh_limiter(SUB_SYS_BW_LIMITERS sub_sys_bw)
{
	u32 work_clk, bw_val;
	bw_limit_t *syslimiter;
	if (sub_sys_bw < 0 || sub_sys_bw >= BL_NUM_MAX) {
		BW_LOG_ERR("invalid sub_sys_bw value\n");
		return -1;
	}

	mutex_lock(&ax_bw_limiter_mutex);
	if (sys_proc_node_array[sub_sys_bw].sub_sys_enabled == false) {
		mutex_unlock(&ax_bw_limiter_mutex);
		BW_LOG_ERR("not register before");
		return -1;
	}

	work_clk = sys_proc_node_array[sub_sys_bw].clk;
	syslimiter = sys_proc_node_array[sub_sys_bw].bw_limiter;
	bw_val = sys_proc_node_array[sub_sys_bw].bwLimiter_value_rd;
	bw_limiter_set(syslimiter, work_clk, bw_val, BW_PORT_RD);
	bw_val = sys_proc_node_array[sub_sys_bw].bwLimiter_value_wr;
	bw_limiter_set(syslimiter, work_clk, bw_val, BW_PORT_WR);

	mutex_unlock(&ax_bw_limiter_mutex);
	return 0;
}
EXPORT_SYMBOL(ax_bw_limiter_refresh_limiter);

int ax_bw_limiter_register_with_clk(SUB_SYS_BW_LIMITERS sub_sys_bw, void *clk)
{
	int ret;
	unsigned long rate;
	if (sub_sys_bw < 0 || sub_sys_bw >= BL_NUM_MAX || !clk) {
		BW_LOG_ERR("invalid sub_sys_bw value\n");
		return -1;
	}

	mutex_lock(&ax_bw_limiter_mutex);
	if (sys_proc_node_array[sub_sys_bw].sub_sys_enabled == true) {
		if (sub_sys_bw == BL_ISP_IFE_WDAM_M1) {
			atomic_inc(&multisys_registered_cnt);
			mutex_unlock(&ax_bw_limiter_mutex);
			return 0;
		} else {
			mutex_unlock(&ax_bw_limiter_mutex);
			BW_LOG_ERR(" don't attempt to register the same bwlimiter once more");
			return -1;
		}
	}


	sys_proc_node_array[sub_sys_bw].sub_sys_enabled = true;
	sys_proc_node_array[sub_sys_bw].pclk = (struct clk *)clk;
	mutex_unlock(&ax_bw_limiter_mutex);

	rate = clk_get_rate(sys_proc_node_array[sub_sys_bw].pclk);

	mutex_lock(&ax_bw_limiter_mutex);
	sys_proc_node_array[sub_sys_bw].clk = rate / 1000000;   //unit: MHz
	ret = ax_bw_limiter_proc_create_file(sys_proc_node_array[sub_sys_bw].proc_name);
	if (ret) {
		mutex_unlock(&ax_bw_limiter_mutex);
		BW_LOG_ERR("create proc node[%s] failed!\n", sys_proc_node_array[sub_sys_bw].proc_name);
		return -1;
	}

	mutex_unlock(&ax_bw_limiter_mutex);

	if (ax_bw_limiter_clk_notifier_register(sub_sys_bw, sys_proc_node_array[sub_sys_bw].pclk)) {
		return -1;
	}

	if (sub_sys_bw == BL_ISP_IFE_WDAM_M1) {
		atomic_inc(&multisys_registered_cnt);
	}

	return 0;
}
EXPORT_SYMBOL(ax_bw_limiter_register_with_clk);

int ax_bw_limiter_register_with_val(SUB_SYS_BW_LIMITERS sub_sys_bw, u32 work_clk)
{
	int ret;
	if (sub_sys_bw < 0 || sub_sys_bw >= BL_NUM_MAX) {
		BW_LOG_ERR("invalid sub_sys_bw value\n");
		return -1;
	}

	if (sys_proc_node_array[sub_sys_bw].sub_sys_enabled == true) {
		sys_proc_node_array[sub_sys_bw].clk = work_clk;
		return -1;
	} else {
		sys_proc_node_array[sub_sys_bw].sub_sys_enabled = true;
		sys_proc_node_array[sub_sys_bw].clk = work_clk;
	}

	mutex_lock(&ax_bw_limiter_mutex);
	ret = ax_bw_limiter_proc_create_file(sys_proc_node_array[sub_sys_bw].proc_name);
	if (ret) {
		mutex_unlock(&ax_bw_limiter_mutex);
		BW_LOG_ERR("create proc node[%s] failed!\n", sys_proc_node_array[sub_sys_bw].proc_name);
		return -1;
	}
	mutex_unlock(&ax_bw_limiter_mutex);

	return 0;
}
EXPORT_SYMBOL(ax_bw_limiter_register_with_val);

int ax_bw_limiter_unregister(SUB_SYS_BW_LIMITERS sub_sys_bw, void *clk)
{
	bool can_unregistered = true;
	struct clk *pclk = (struct clk *)clk;
	if (sub_sys_bw < 0 || sub_sys_bw >= BL_NUM_MAX) {
		BW_LOG_ERR("invalid sub_sys_bw value\n");
		return -1;
	}

	if (sys_proc_node_array[sub_sys_bw].sub_sys_enabled == false) {
		BW_LOG_ERR("sub sys[%s] wasn't enabled before!!!\n", sys_proc_node_array[sub_sys_bw].proc_name);
	}

	mutex_lock(&ax_bw_limiter_mutex);
	if (sub_sys_bw == BL_ISP_IFE_WDAM_M1 && atomic_read(&multisys_registered_cnt) > 0) {
		atomic_dec(&multisys_registered_cnt);
		if (atomic_read(&multisys_registered_cnt) == 0) {
			can_unregistered = true;
		} else {
			can_unregistered = false;
		}
	}

	if (can_unregistered == true) {
		if (sys_proc_node_array[sub_sys_bw].pclk == pclk) {
			clk_notifier_unregister(pclk, &sys_proc_node_array[sub_sys_bw].nb);
		}

		remove_proc_entry(PROC_BW_LIMITER_VAL_RD, sys_proc_node_array[sub_sys_bw].root_proc_file);
		remove_proc_entry(PROC_BW_LIMITER_VAL_WR, sys_proc_node_array[sub_sys_bw].root_proc_file);
		remove_proc_entry(sys_proc_node_array[sub_sys_bw].proc_name, bw_limiter_root);
		sys_proc_node_array[sub_sys_bw].sub_sys_enabled = false;
		sys_proc_node_array[sub_sys_bw].proc_rfile = NULL;
		sys_proc_node_array[sub_sys_bw].proc_wfile = NULL;
	}
	mutex_unlock(&ax_bw_limiter_mutex);

	return 0;
}
EXPORT_SYMBOL(ax_bw_limiter_unregister);

static const struct of_device_id ax_bw_limiter_of_match[] = {
	{.compatible = "axera,bw_limiter"},
	{},
};

static struct platform_driver ax_bw_limiter_driver = {
	.driver = {
		.name = DRIVER_NAME,
		.of_match_table = ax_bw_limiter_of_match,
	},
	.probe = ax_bw_limiter_probe,
	.remove = ax_bw_limiter_remove,
};

static int __init ax_bw_limiter_init(void)
{
	platform_driver_register(&ax_bw_limiter_driver);
	return 0;
}

static void __exit ax_bw_limiter_exit(void)
{
	return platform_driver_unregister(&ax_bw_limiter_driver);
}

module_init(ax_bw_limiter_init);
module_exit(ax_bw_limiter_exit);

MODULE_AUTHOR("axera");
MODULE_LICENSE("GPL");
MODULE_VERSION("2.0");
MODULE_INFO(intree, "Y");
