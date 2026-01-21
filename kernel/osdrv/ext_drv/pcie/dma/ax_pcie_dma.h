#ifndef __AX_PCIE_DMA_H__
#define __AX_PCIE_DMA_H__

#define	PCIE_DMA_BASE		0x380000

/* HDMA register */
#define	CHAN_ADDR_OFFSET	0x200

#define	RD_CHAN_EN		    0x100
#define	RD_DOORBELL_EN		0x104
#define	RD_CHAN_TRAN_SIZE	0x11c
#define	RD_SAR_LOW_ADDR		0x120
#define	RD_SAR_UPPER_ADDR	0x124
#define	RD_DAR_LOW_ADDR		0x128
#define	RD_DAR_UPPER_ADDR	0x12C
#define	RD_CHAN_STATUS		0x180
#define	RD_CHAN_INT_STATUS  0x184
#define RD_CHAN_INT_SETUP   0x188
#define RD_CHAN_INT_CLEAR   0x18c

#define	WR_CHAN_EN		    0x0
#define	WR_DOORBELL_EN		0x04
#define	WR_CHAN_TRAN_SIZE	0x1c
#define	WR_SAR_LOW_ADDR		0x20
#define	WR_SAR_UPPER_ADDR	0x24
#define	WR_DAR_LOW_ADDR		0x28
#define	WR_DAR_UPPER_ADDR	0x2C
#define	WR_CHAN_STATUS		0x80
#define	WR_CHAN_INT_STATUS	0x84
#define WR_CHAN_INT_SETUP	0x88
#define WR_CHAN_INT_CLEAR	0x8c

/* HDMA Link list register */
#define RD_CHAN_LLP_LOW		0x110
#define RD_CHAN_LLP_HIGH	0x114
#define RD_CHAN_CYCLE		0x118
#define RD_CHAN_WATERMARK_EN	0x130
#define RD_CHAN_CONTROL_SET	0x134

#define WR_CHAN_LLP_LOW		0x10
#define WR_CHAN_LLP_HIGH	0x14
#define WR_CHAN_CYCLE		0x18
#define WR_CHAN_WATERMARK_EN	0x30
#define WR_CHAN_CONTROL_SET	0x34

#define HDMA_LLP_EN	0x1

/* This bit will be set after a dma transferred */
#define DMA_DONE_INTERRUPT_BIT		(1<<0)
#define DMA_WATERMARK_INTERRUPT_BIT	(1<<1)
/* This bit will be set when abort accurred in a dma transfer */
#define DMA_ABORT_INTERRUPT_BIT		(1<<2)

#define DMA_LOCAL_ABORT_INT_EN      (1<<6)
#define DMA_LOCAL_STOP_INT_EN       (1<<4)


enum dma_trans_status {
	UNKNOW_EXCEPTION = -1,
	NO_TRANSFER = 0,
	NORMAL_DONE = 1,
	TRANSFER_ABORT = 2,
	LL_WATERMARK = 3,
};

extern void ax_pcie_dma_ops_init(struct ax_pcie_operation *s_pcie_opt);

#endif
