#ifndef __AXERA_AXI_DMAXOR_PLATFORM_H__
#define __AXERA_AXI_DMAXOR_PLATFORM_H__

#include <linux/bitops.h>
#include <linux/delay.h>
#include <linux/device.h>
#include <linux/dmaengine.h>
#include <linux/dmapool.h>
#include <linux/dma-mapping.h>
#include <linux/err.h>
#include <linux/interrupt.h>
#include <linux/io.h>
#include <linux/iopoll.h>
#include <linux/io-64-nonatomic-lo-hi.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/of_dma.h>
#include <linux/platform_device.h>
#include <linux/pm_runtime.h>
#include <linux/property.h>
#include <linux/slab.h>
#include <linux/types.h>
#include <linux/of_device.h>
#include <linux/async_tx.h>
#include <linux/clk.h>
#include <linux/reset.h>
#include <asm/barrier.h>

#include "../dmaengine.h"
#include "../virt-dma.h"
#include "../../soc/axera/ax_kmem_alloc/axera-kmemalloc.h"

#define DMAC_MAX_CHANNELS	(32)
#define DMAC_MAX_BLK_SIZE	(1 << 22)
#define AX_DMAXOR_ALIGN		(3)
#define AX_DMAXOR_MAX_XOR_CNT	(15)
#define DMAXOR_TR_1B_MAXSIZE	(0x3fffE0)
#define DMAXOR_TR_2B_MAXSIZE	(0x7fffE0)
#define DMAXOR_TR_4B_MAXSIZE	(0x800000)
#define DMAXOR_ALLOC_SIZE_1M	(0x100000)

struct ax_dmaxor_hcfg {
	u32 nr_channels;
	u32 m_data_width;
	u32 block_size[DMAC_MAX_CHANNELS];
	u32 axi_rw_burst_len;
};

struct ax_dmaxor_hw_desc {
	u32 len;
	dma_addr_t llp;
	struct ax_dmaxor_lli *lli;
};

struct ax_dmaxor_desc {
	u32 length;
	u32 *xor_check_result;
	enum dma_status stat;
	struct ax_dmaxor_hw_desc *hw_desc;
	struct virt_dma_desc vd;
	struct ax_dmaxor_chan *chan;
	atomic_t hw_descs_allocated;
	struct list_head desc_node;
};

struct ax_dmaxor_chan {
	u32 id;
	u32 irq_status;
	void __iomem *chan_regs;
	struct ax_dmaxor_chip *chip;
	struct virt_dma_chan vc;
	enum dma_status stat;
	struct ax_dmaxor_desc *desc;
	struct list_head alloc_list;
};

struct ax_dmaxor {
	struct dma_device dma;
	struct ax_dmaxor_chan *chan;
	struct ax_dmaxor_hcfg *hdata;
	struct device_dma_parameters dma_parms;
};

struct ax_dmaxor_chip {
	u32 cur_id;
	int irq;
	struct device *dev;
	volatile u32 start;
	void __iomem *regs;
	struct clk *core_clk;
	struct reset_control *rst;
	spinlock_t lock;
	struct dma_pool *desc_pool;
	struct ax_kmem_info *desc_kmem;
	struct ax_dmaxor *axdmaxor;
	struct ax_dmaxor_desc *desc_starting;
	struct list_head pending_list;
};

/* LLI == Linked List Item */
struct __packed ax_dmaxor_lli {
	__le64 src;
	__le64 dst;
	__le64 xorch_and_size;
	__le64 ctrl;
	__le64 llp;
	__le64 Reserved1;
	__le64 Reserved2;
	__le64 Reserved3;
	__le64 Reserved4;
	__le64 Reserved5;
	__le64 sar_buf[15];
};

static inline struct device *dchan2dev(struct dma_chan *dchan)
{
	return &dchan->dev->device;
}

static inline struct device *chan2dev(struct ax_dmaxor_chan *chan)
{
	return &chan->vc.chan.dev->device;
}

static inline struct ax_dmaxor_desc *vd_to_ax_desc(struct virt_dma_desc *vd)
{
	return container_of(vd, struct ax_dmaxor_desc, vd);
}

static inline
    struct ax_dmaxor_chan *vc_to_ax_dmaxor_chan(struct virt_dma_chan *vc)
{
	return container_of(vc, struct ax_dmaxor_chan, vc);
}

static inline
    struct ax_dmaxor_chan *dchan_to_ax_dmaxor_chan(struct dma_chan *dchan)
{
	return vc_to_ax_dmaxor_chan(to_virt_chan(dchan));
}

enum {
	AXIDMAXOR_ARWLEN_1 = 0,
	AXIDMAXOR_ARWLEN_2 = 1,
	AXIDMAXOR_ARWLEN_4 = 3,
	AXIDMAXOR_ARWLEN_8 = 7,
	AXIDMAXOR_ARWLEN_16 = 15,
	AXIDMAXOR_ARWLEN_32 = 31,
	AXIDMAXOR_ARWLEN_MIN = AXIDMAXOR_ARWLEN_1,
	AXIDMAXOR_ARWLEN_MAX = AXIDMAXOR_ARWLEN_32
};

enum dmaxor_int_sta {
	AX_DMAXOR_XFER_IDLE = 0,
	AX_DMAXOR_XFER_SUCCESS = BIT(0),
	AX_DMAXOR_BLOCK_TS = BIT(1),
	AX_DMAXOR_XOR_CHECK_ERR = BIT(2),
	AX_DMAXOR_LLI_ERR = BIT(3),
	AX_DMAXOR_AXI_RD_ERR = BIT(4),
	AX_DMAXOR_AXI_WE_ERR = BIT(5),
	AX_DMAXOR_ALL_ERR = GENMASK(5, 3),
	AX_DMAXOR_IRQ_ALL = GENMASK(5, 0),
	AX_DMAXOR_CFG_SUCCESS = BIT(6),
};

enum {
	AXIDMAXOR_TRANS_WIDTH_8 = 0,
	AXIDMAXOR_TRANS_WIDTH_16,
	AXIDMAXOR_TRANS_WIDTH_32,
	AXIDMAXOR_TRANS_WIDTH_64,
	AXIDMAXOR_TRANS_WIDTH_MAX = AXIDMAXOR_TRANS_WIDTH_64
};

#define AX_DMAXOR_REG_CH			(0x0)
#define AX_DMAXOR_REG_CTRL			(0x4)
#define AX_DMAXOR_REG_CTRL_TYPE_SHIFT		(1)
#define AX_DMAXOR_REG_CTRL_TYPE_MASK		(3)
#define AX_DMAXOR_REG_TRIG			(0x8)
#define AX_DMAXOR_REG_START			(0xC)
#define AX_DMAXOR_REG_STA			(0x10)
#define AX_DMAXOR_REG_CS_FAIL_ADDR_H		(0x14)
#define AX_DMAXOR_REG_CS_FAIL_ADDR_L		(0x18)
#define AX_DMAXOR_REG_CS_FAIL_IDX		(0x1C)
#define AX_DMAXOR_REG_LLI_BASE_H		(0x20)
#define AX_DMAXOR_REG_LLI_BASE_L		(0x24)
#define AX_DMAXOR_REG_CLEAR			(0x28)
#define AX_DMAXOR_REG_CUR_SAR_H			(0x2C)
#define AX_DMAXOR_REG_CUR_SAR_L			(0x30)
#define AX_DMAXOR_REG_CUR_DAR_H			(0x34)
#define AX_DMAXOR_REG_CUR_DAR_L			(0x38)
#define AX_DMAXOR_REG_CUR_BLK_TS		(0x3C)
#define AX_DMAXOR_REG_CUR_CTRL_H		(0x40)
#define AX_DMAXOR_REG_CUR_CTRL_L		(0x44)
#define AX_DMAXOR_REG_CUR_LLP_H			(0x48)
#define AX_DMAXOR_REG_CUR_LLP_L			(0x4C)
#define AX_DMAXOR_REG_XOR_CALI_CLEAR		(0x74)
#define AX_DMAXOR_REG_XOR_CALI_DATA(x)		(0x78 + (ax_u32)x * 0x4)
#define AX_DMAXOR_REG_INT_GLB_MASK		(0x118)
#define AX_DMAXOR_REG_INT_GLB_RAW		(0x11C)
#define AX_DMAXOR_REG_INT_GLB_STA		(0x120)
#define AX_DMAXOR_REG_INT_MASK(x)		(0x124 + (ax_u32)x * 0x10)
#define AX_DMAXOR_REG_INT_CLR(x)		(0x128 + (ax_u32)x * 0x10)
#define AX_DMAXOR_REG_INT_RAW(x)		(0x12C + (ax_u32)x * 0x10)
#define AX_DMAXOR_REG_INT_STA(x)		(0x130 + (ax_u32)x * 0x10)
#define AX_DMAXOR_REG_CH_AUTO_GATE		(0x224)

#define AX_DMAXOR_INT_GLB_MASK			(0xFFFF)
#define AX_DMAXOR_CHAN_IRQ_MASK			(0x39)
#define AX_DMAXOR_CHAN_INT_EN			(0x00)
#define AX_DMAXOR_CHAN_INT_CLR			(0x04)
#define AX_DMAXOR_CHAN_INT_RAW			(0x08)
#define AX_DMAXOR_CHAN_INT_STA			(0x0C)
#define AX_DMAXOR_CHAN_REG_BASE			(0x124)
#define AX_DMAXOR_CHAN_REG_LEN			(0x10)
#define AX_DMAXOR_LLI_SIZE			(0x100)	//AX_DMAXOR_REG_CTRL bit[2:1] == 0x2

#define AX_DMAXOR_RESET_SET			(0x38)
#define AX_DMAXOR_RESET_CLR			(0x3C)

// LLI_DATA2
#define LLI_CHAN_EN_MASK			(0xFFFF)
#define LLI_CHAN_EN_SHIFT			(42)
#define SRC_MSIZE_MASK				(0x3FF)
#define SRC_MSIZE_SHIFT				(32)
#define DAR_MSIZE_MASK				(0x3FF)
#define DAR_MSIZE_SHIFT				(22)
#define BLOCK_TS_MASK				(0x3FFFFF)
#define BLOCK_TS_SHIFT				(0)

// LLI_DATA3
#define AX_DMAXOR_IOC_MASK			(0x1)
#define AX_DMAXOR_IOC_SHIFT			(50)
#define AX_DMAXOR_TYPE_MASK			(0x7)
#define AX_DMAXOR_TYPE_SHIFT			(47)
#define AX_DMAXOR_XOR_NUM_MASK			(0xF)
#define AX_DMAXOR_XOR_NUM_SHIFT			(43)
#define AX_DMAXOR_CHKSUM_MASK			(0x1)
#define AX_DMAXOR_CHKSUM_SHIFT			(42)
#define AX_DMAXOR_ENDIAN_MASK			(0x1)
#define AX_DMAXOR_ENDIAN_SHIFT			(41)
#define AX_DMAXOR_WB_MASK			(0x1)
#define AX_DMAXOR_WB_SHIFT			(40)
#define AX_DMAXOR_LAST_MASK			(0x1)
#define AX_DMAXOR_LAST_SHIFT			(39)
#define AX_DMAXOR_LLI_PER_MASK			(0x1)
#define AX_DMAXOR_LLI_PER_SHIFT			(38)
#define AX_DMAXOR_AWLEN_MASK			(0x3)
#define AX_DMAXOR_AWLEN_SHIFT			(36)
#define AX_DMAXOR_ARLEN_MASK			(0x3)
#define AX_DMAXOR_ARLEN_SHIFT			(34)
#define AX_DMAXOR_AR_PROT_MASK			(0x7)
#define AX_DMAXOR_AR_PROT_SHIFT			(31)
#define AX_DMAXOR_AW_PROT_MASK			(0x7)
#define AX_DMAXOR_AW_PROT_SHIFT			(28)
#define AX_DMAXOR_AR_CACHE_MASK			(0xF)
#define AX_DMAXOR_AR_CACHE_SHIFT		(24)
#define AX_DMAXOR_AW_CACHE_MASK			(0xF)
#define AX_DMAXOR_AW_CACHE_SHIFT		(20)
#define AX_DMAXOR_ROSD_MASK			(0x3F)
#define AX_DMAXOR_ROSD_SHIFT			(14)
#define AX_DMAXOR_WOSD_MASK			(0x3F)
#define AX_DMAXOR_WOSD_SHIFT			(8)
#define AX_DMAXOR_SRC_TR_WIDTH_MASK		(0x7)
#define AX_DMAXOR_SRC_TR_WIDTH_SHIFT		(5)
#define AX_DMAXOR_DST_TR_WIDTH_MASK		(0x7)
#define AX_DMAXOR_DST_TR_WIDTH_SHIFT		(2)
#define AX_DMAXOR_LLI_DINC_MASK			(0x1)
#define AX_DMAXOR_LLI_DINC_SHIFT		(1)
#define AX_DMAXOR_LLI_SINC_MASK			(0x1)
#define AX_DMAXOR_LLI_SINC_SHIFT		(0)

#define AX_DMAXOR_MAX_LLI			(256)
#define AX_DMAXOR_MAX_ID			(128)
#define AX_DMAXOR_AXILEN_32BIT			(2)
#define AX_DMAXOR_DEFAULT_CTRL			(0x40001AAAFFF03)

typedef enum {
	AX_DMAXOR_MODE_OFFSET = 4,
	AX_DMAXOR_MEMORY_INIT = AX_DMAXOR_MODE_OFFSET,
	AX_DMAXOR_XOR_WB,
	AX_DMAXOR_XOR_CALI,
} ax_dmaxor_xfer_mode_t;

enum dmaxor_tr_width {
	AX_DMAXOR_TR_1B = 0,
	AX_DMAXOR_TR_2B,
	AX_DMAXOR_TR_4B,
	AX_DMAXOR_TR_8B
};

enum dmaxor_ax_len {
	AX_DMAXOR_AXI_1B = 0,
	AX_DMAXOR_AXI_2B,
	AX_DMAXOR_AXI_4B,
};

#endif /* __AXERA_AXI_DMAXOR_PLATFORM_H__ */
