#ifndef __AX650_PCI_H__
#define __AX650_PCI_H__

#include <common.h>

#define PCI_BASE_ADDRESS_0	0x10	/* 32 bits */
#define PCI_BASE_ADDRESS_1	0x14	/* 32 bits [htype 0,1 only] */
#define PCI_BASE_ADDRESS_2	0x18	/* 32 bits [htype 0 only] */
#define PCI_BASE_ADDRESS_3	0x1c	/* 32 bits */
#define PCI_BASE_ADDRESS_4	0x20	/* 32 bits */
#define PCI_BASE_ADDRESS_5	0x24	/* 32 bits */

#define	PCI_BASE_ADDRESS_MEM_TYPE_32	0x00	/* 32 bit address */
#define	PCI_BASE_ADDRESS_MEM_TYPE_64	0x04	/* 64 bit address */
#define	PCI_BASE_ADDRESS_MEM_PREFETCH	0x08	/* prefetchable? */

#define DBI_CS2			0x100000
#define PCI_DBI_BASE		0x40000000
#define PCI_DBI2_BASE		(PCI_DBI_BASE + DBI_CS2)
#define PCIE0_PERST_GPIO	115 //GPIO3A19
#define PCIE1_PERST_GPIO	149 //GPIO4A21
#define PCIE0_CLKREQ_GPIO	128 //GPIO4A0
#define PCIE1_CLKREQ_GPIO	23  //GPIO0A23

#define PCIE_LINK_ASPM_CAP	0x7c

#define PCI_REBAR_CAP		4	/* capability register */
#define PCI_EXT_CAP_ID_REBAR	0x15	/* Resizable BAR */
#define PCI_CFG_SPACE_SIZE	256
#define PCI_EXT_CAP_ID(header)		(header & 0x0000ffff)
#define PCI_EXT_CAP_NEXT(header)	((header >> 20) & 0xffc)
#define PCI_REBAR_CTRL		8	/* control register */
#define PCI_REBAR_CTRL_BAR_IDX		0x00000007  /* BAR index */
#define PCI_REBAR_CTRL_NBAR_MASK	0x000000E0  /* # of resizable BARs */
#define PCI_REBAR_CTRL_NBAR_SHIFT	5  	    /* shift for # of BARs */
#define PCI_REBAR_CTRL_BAR_SIZE	0x00001F00  /* BAR size */

#define PCIE_PORT_LINK_CONTROL		0x710
#define PORT_LINK_MODE_MASK		(0x3f << 16)
#define PORT_LINK_MODE_X_LANES		(0x3 << 16)	 /* x1:(0x1 << 16); x2:(0x3 << 16)*/
#define PCIE_LINK_WIDTH_SPEED_CONTROL	0x80C
#define PORT_LOGIC_SPEED_CHANGE     	(0x1 << 17)
#define PORT_LOGIC_LINK_WIDTH_MASK	(0x1f << 8)
#define PORT_LOGIC_LINK_WIDTH_X_LANES	(0x2 << 8) /* x1:(0x1 << 16); x2:(0x3 << 16) */
#define	PCIE_PIPE_PHY_MODEL_SEL_MASK	0x3

#define PCIE_MISC_CONTROL_1_OFF		0x8BC
#define PCIE_DBI_RO_WR_EN		(0x1 << 0)

#define PCIE_LINK_CTL2_REG		0xa0
#define PCIE_LINK_SPEED_MASK	0xf
#define PCIE_LINK_SPEED_GENX	(0x1 << 1)   /* GEN1:(0x1 << 0); GEN2:(0x1 << 1) */

/*
 * Configuration Space header
 */
#define	PCI_VENDOR_ID		0x00	/* 16 bit	Vendor ID */
#define	PCI_DEVICE_ID		0x02	/* 16 bit	Device ID */
#define	PCI_COMMAND		0x04	/* 16 bit	Command */
#define	PCI_STATUS		0x06	/* 16 bit	Status */

#define PCI_CLASS_REVISION	0x08	/* High 24 bits are class, low 8 revision */
#define PCI_REVISION_ID		0x08	/* Revision ID */
#define PCI_CLASS_PROG		0x09	/* Reg. Level Programming Interface */
#define PCI_CLASS_DEVICE	0x0a	/* Device class */
#define PCI_CACHE_LINE_SIZE	0x0c	/* 8 bits */
#define PCI_SUBSYSTEM_VENDOR_ID	0x2c
#define PCI_SUBSYSTEM_ID	0x2e
#define PCI_INTERRUPT_PIN	0x3d	/* 8 bits */

/* device info */
#define AXERA_PCIE_VENDORID		0x1F4B
#define AXERA_PCIE_DEVICEID		0x0650
#define AXERA_PCIE_BASECLASSCODE	0x04
#define AXERA_PCIE_SUBCLASSCODE	0x00
#define	INTERRUPT_PIN	0x01
#define	SUBSYS_ID		0x0000
#define	SUBSYS_VENDOR_ID	0x0000
#define	CACHE_LINE_SIZE		0x0000
#define	PROGIF_CODE		0x00
#define	REVID			0x00

#define PCIE_ATU_TYPE_MEM	(0x0 << 0)
#define PCIE_MSG_BASE		0x04800000
#define PCIE_SYS_RESET_BASE	0x4000000
#define PCIE_SYS_RESET_BASE_1M	0x4200000
#define PCIE_HWID_BASE_ADDR	0x160c00000
#define INBOUND_INDEX0		0
#define INBOUND_INDEX1		1
#define INBOUND_INDEX2		2

#define BAR0_SIZE	0x800000
#define BAR1_SIZE	0x10000
#define BAR4_SIZE	0x100000

#define LINK_WAIT_IATU	10000
#define	LINK_WAIT_MAX_IATU_RETRIES	5

/* msi register */
#define PCI_MSI_CAP		0x50
#define PCI_MSI_FLAGS		2	/* Message Control */
#define PCI_MSI_INT_NUM		0x2	/* MSI interrupts number 0x0;0x1;0x2;03;0x4;0x5*/
#define	PCI_MSI_ADDR_LO		4	/* MSI low addr */
#define	PCI_MSI_ADDR_HI		8	/* MSI low addr */

/* HDMA register */
#define	CHAN_ADDR_BASE		0x200
#define	RD_CHAN_EN		0x100
#define	RD_CHAN_TRAN_SIZE	0x11c
#define	RD_SAR_LOW_ADDR		0x120
#define	RD_SAR_UPPER_ADDR	0x124
#define	RD_DAR_LOW_ADDR		0x128
#define	RD_DAR_UPPER_ADDR	0x12C
#define	RD_DOORBELL_EN		0x104
#define	RD_CHAN_STATUS		0x180

#define	WR_CHAN_EN		0x000
#define	WR_CHAN_TRAN_SIZE	0x01c
#define	WR_SAR_LOW_ADDR		0x020
#define	WR_SAR_UPPER_ADDR	0x024
#define	WR_DAR_LOW_ADDR		0x028
#define	WR_DAR_UPPER_ADDR	0x02C
#define	WR_DOORBELL_EN		0x004
#define	WR_CHAN_STATUS		0x080

/*
 * iATU Unroll-specific register definitions
 * From 4.80 core version the address translation will be made by unroll
 */
#define PCIE_ATU_UNR_REGION_CTRL1	0x00
#define PCIE_ATU_UNR_REGION_CTRL2	0x04
#define PCIE_ATU_UNR_LOWER_BASE		0x08
#define PCIE_ATU_UNR_UPPER_BASE		0x0C
#define PCIE_ATU_UNR_LIMIT		0x10
#define PCIE_ATU_UNR_LOWER_TARGET	0x14
#define PCIE_ATU_UNR_UPPER_TARGET	0x18

#define PCIE_ATU_ENABLE			(0x1 << 31)
#define PCIE_ATU_BAR_MODE_ENABLE	(0x1 << 30)
#define PCIE_DMA_BYPASS			(0x1 << 27)
#define	PCIE_DMA_BASE			0x380000

#define IRQ_TYPE_MSIX			2
#define COMMAND_READ			BIT(3)
#define COMMAND_WRITE			BIT(4)

#define STATUS_READ_SUCCESS		BIT(0)
#define STATUS_READ_FAIL		BIT(1)
#define STATUS_WRITE_SUCCESS		BIT(13)
#define STATUS_WRITE_FAIL		BIT(14)

#define	HDMA_RD_ORG_HDR_FL		BIT(2)
#define	HDMA_RD_BAK_HDR_FL		BIT(3)
#define	HDR_CHKSUM_FL			BIT(4)
#define	HDMA_RD_FW_FL			BIT(5)
#define	FW_CHKSUM_FL			BIT(6)
#define	EIP_INIT_FL				BIT(7)
#define	PUB_KEY_CHK_FL			BIT(8)
#define	HDMA_FW_IMG_FL			BIT(9)
#define	IMG_CHKSUM_FL			BIT(10)
#define	CE_SHA_FL				BIT(11)
#define	CE_AES_FL				BIT(12)

#define	BOOT_START_DEVICES		BIT(0)

#define	PCIE_DATA_FAIL		-1
#define	CE_FW_FAIL		-2
#define	PCIE_HEADER_CHECHSUM_FAIL	-3
#define	PCIE_HDMA_READ_ABORT		-10

/* Register address builder */
#define PCIE_GET_ATU_OUTB_UNR_REG_OFFSET(region)	\
			((0x3 << 20) | ((region) << 9))

#define PCIE_GET_ATU_INB_UNR_REG_OFFSET(region)				\
			((0x3 << 20) | ((region) << 9) | (0x1 << 8))

#define upper_32_bits(n) ((u32)(((n) >> 16) >> 16))
#define lower_32_bits(n) ((u32)(n))

enum pci_barno {
	NO_BAR = -1,
	BAR_0,
	BAR_1,
	BAR_2,
	BAR_3,
	BAR_4,
	BAR_5,
};

struct pci_msg_reg {
	u32	magic;
	u32	command;
	u32	status;
	u32	low_src_addr;
	u32	upper_src_addr;
	u32	low_dst_addr;
	u32	upper_dst_addr;
	u32	size;
	u32	checksum;
	u32	boot_reason;
	u32	finish;
} __packed;

void ax_pcie_ep_ctl_init(void);
int polling_remote_command(struct pci_msg_reg *reg);
#endif
