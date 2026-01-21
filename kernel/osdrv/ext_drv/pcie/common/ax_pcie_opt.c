/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#include <linux/interrupt.h>
#include <linux/pci.h>
#include <linux/irq.h>
#include <linux/pci_regs.h>
#include <linux/delay.h>
#include "../include/ax_pcie_dev.h"

#define PCIE0_RD_CTL_6		0x224
#define PCIE1_RD_CTL_6		0x270

#define	PCIE0_ID	0
#define	PCIE1_ID	1
#define	AXERA_PCIE0_CPU_TO_BUS_ADDR		0x3FFFFFFF
#define	AXERA_PCIE1_CPU_TO_BUS_ADDR		0x1FFFFFFF

#ifdef IS_AX_SLAVE
extern int ax_mailbox_register_notify(int mid,
				      void (*func
					    (int fromid, int regno,
					     int info_data)));
extern int ax_mailbox_read(int regno, int *data, int size);

static spinlock_t irq_lock;
#endif
static spinlock_t s_dbi_lock;

static struct ax_pcie_operation *ax_pcie_opt;

unsigned int get_pcie_controller(int bar)
{
	return 0;
}

EXPORT_SYMBOL(get_pcie_controller);


void ax_pcie_data_lock(unsigned long *flags)
{
#ifndef	IS_THIRD_PARTY_PLATFORM
#ifdef SHMEM_FROM_MASTER
	ax_pcie_dbi_spin_lock(flags);
#else
	ax_pcie_spin_lock(flags);
#endif
#else
	ax_pcie_dbi_spin_lock(flags);
#endif
}
EXPORT_SYMBOL(ax_pcie_data_lock);

void ax_pcie_data_unlock(unsigned long *flags)
{
#ifndef	IS_THIRD_PARTY_PLATFORM
#ifdef SHMEM_FROM_MASTER
	ax_pcie_dbi_spin_unlock(flags);
#else
	ax_pcie_spin_unlock(flags);
#endif
#else
	ax_pcie_dbi_spin_unlock(flags);
#endif
}
EXPORT_SYMBOL(ax_pcie_data_unlock);

void ax_pcie_dbi_spin_lock(unsigned long *flags)
{
	spin_lock_irqsave(&s_dbi_lock, *flags);
}
EXPORT_SYMBOL(ax_pcie_dbi_spin_lock);

void ax_pcie_dbi_spin_unlock(unsigned long *flags)
{
	spin_unlock_irqrestore(&s_dbi_lock, *flags);
}
EXPORT_SYMBOL(ax_pcie_dbi_spin_unlock);

#ifdef IS_AX_SLAVE
static int axera_pcie_get_awmisc_status(void)
{
	u32 ctl_id = ax_pcie_opt->local_ctl_id;
	void *glb_base;

	glb_base = g_axera_pcie_opt->local_controller[ctl_id]->glb_base;
	if (ctl_id == PCIE0_ID) {
		return readl(glb_base + PCIE0_SIDEBAND_AWMISC_OFFSET);
	} else {
		return readl(glb_base + PCIE1_SIDEBAND_AWMISC_OFFSET);
	}
}

static int axera_pcie_get_armisc_status(void)
{
	u32 ctl_id = ax_pcie_opt->local_ctl_id;
	void *glb_base;

	glb_base = g_axera_pcie_opt->local_controller[ctl_id]->glb_base;
	if (ctl_id == PCIE0_ID) {
		return readl(glb_base + PCIE0_SIDEBAND_ARMISC_OFFSET);
	} else {
		return readl(glb_base + PCIE1_SIDEBAND_ARMISC_OFFSET);
	}
}

void axera_pcie_awmisc_enable(int enable)
{
	u32 ctl_id = ax_pcie_opt->local_ctl_id;
	void *glb_base;

	glb_base = g_axera_pcie_opt->local_controller[ctl_id]->glb_base;

	if (ctl_id == PCIE0_ID) {
		if (enable) {
			writel(PCIE_SIDEBAND_MISC_EN, glb_base + PCIE0_SIDEBAND_AWMISC_OFFSET);
		} else {
			writel(0, glb_base + PCIE0_SIDEBAND_AWMISC_OFFSET);
		}
	} else {
		if (enable) {
			writel(PCIE_SIDEBAND_MISC_EN, glb_base + PCIE1_SIDEBAND_AWMISC_OFFSET);
		} else {
			writel(0, glb_base + PCIE1_SIDEBAND_AWMISC_OFFSET);
		}
	}
}
EXPORT_SYMBOL(axera_pcie_awmisc_enable);

void axera_pcie_armisc_enable(int enable)
{
	u32 ctl_id = ax_pcie_opt->local_ctl_id;
	void *glb_base;

	glb_base = g_axera_pcie_opt->local_controller[ctl_id]->glb_base;

	if (ctl_id == PCIE0_ID) {
		if (enable) {
			writel(PCIE_SIDEBAND_MISC_EN, glb_base + PCIE0_SIDEBAND_ARMISC_OFFSET);
		} else {
			writel(0, glb_base + PCIE0_SIDEBAND_ARMISC_OFFSET);
		}
	} else {
		if (enable) {
			writel(PCIE_SIDEBAND_MISC_EN, glb_base + PCIE1_SIDEBAND_ARMISC_OFFSET);
		} else {
			writel(0, glb_base + PCIE1_SIDEBAND_ARMISC_OFFSET);
		}
	}
}
EXPORT_SYMBOL(axera_pcie_armisc_enable);

static inline u32 axera_pcie_readl(void __iomem *base, u32 offset)
{
	return readl(base + offset);
}

static inline void axera_pcie_writel(void __iomem *base, u32 offset,
				      u32 value)
{
	writel(value, base + offset);
}

static inline void ax_pcie_write(void __iomem *addr, int size, u32 val)
{
	if (size == 4)
		writel(val, addr);
	else if (size == 2)
		writew(val, addr);
	else if (size == 1)
		writeb(val, addr);
}

static inline void ax_pcie_read(void __iomem *addr, int size, u32 *val)
{
	if (size == 4)
		*val = readl(addr);
	else if (size == 2)
		*val = readw(addr);
	else if (size == 1)
		*val = readb(addr);
}

void ax_pcie_dbi_write(u32 reg, u32 val)
{
	void *dbi_base;
	unsigned long flags = 0;
	u32 ctl_id = ax_pcie_opt->local_ctl_id;

	dbi_base = ax_pcie_opt->local_controller[ctl_id]->config_virt;
	ax_pcie_dbi_spin_lock(&flags);
	if (axera_pcie_get_awmisc_status() & BIT(21)) {
		ax_pcie_write(dbi_base + reg, 4, val);
	} else {
		axera_pcie_awmisc_enable(true);
		ax_pcie_write(dbi_base + reg, 4, val);
		axera_pcie_awmisc_enable(false);
	}
	ax_pcie_dbi_spin_unlock(&flags);
}
EXPORT_SYMBOL(ax_pcie_dbi_write);

void ax_pcie_dbi_read(u32 reg, u32 *val)
{
	void *dbi_base;
	unsigned long flags = 0;
	u32 ctl_id = ax_pcie_opt->local_ctl_id;

	dbi_base = ax_pcie_opt->local_controller[ctl_id]->config_virt;
	ax_pcie_dbi_spin_lock(&flags);
	if (axera_pcie_get_armisc_status() & BIT(21)) {
		ax_pcie_read(dbi_base + reg, 4, val);
	} else {
		axera_pcie_armisc_enable(true);
		ax_pcie_read(dbi_base + reg, 4, val);
		axera_pcie_armisc_enable(false);
	}
	ax_pcie_dbi_spin_unlock(&flags);
}
EXPORT_SYMBOL(ax_pcie_dbi_read);

static void ax_pcie_writel_atu(u32 reg, u32 val)
{
	ax_pcie_dbi_write(reg, val);
}

static void ax_pcie_readl_atu(u32 reg, u32 *val)
{
	ax_pcie_dbi_read(reg, val);
}

static void ax_pcie_writel_ib_unroll(u32 index, u32 reg, u32 val)
{
	u32 offset = PCIE_GET_ATU_INB_UNR_REG_OFFSET(index);

	ax_pcie_writel_atu(offset + reg, val);
}

static void ax_pcie_readl_ob_unroll(u32 index, u32 reg, u32 *val)
{
	u32 offset = PCIE_GET_ATU_OUTB_UNR_REG_OFFSET(index);

	ax_pcie_readl_atu(offset + reg, val);
}

static void ax_pcie_writel_ob_unroll(u32 index, u32 reg, u32 val)
{
	u32 offset = PCIE_GET_ATU_OUTB_UNR_REG_OFFSET(index);

	ax_pcie_writel_atu(offset + reg, val);
}

static void ax_pcie_prog_outbound_atu_unroll(int index, u64 cpu_addr,
						 u64 pci_addr, u64 size)
{
	u64 limit_addr = cpu_addr + size - 1;
	u32 retries, val;
	u32 type = PCIE_ATU_TYPE_MEM;

	ax_pcie_writel_ob_unroll(index, PCIE_ATU_UNR_LOWER_BASE,
				 lower_32_bits(cpu_addr));
	ax_pcie_writel_ob_unroll(index, PCIE_ATU_UNR_UPPER_BASE,
				 upper_32_bits(cpu_addr));
	ax_pcie_writel_ob_unroll(index, PCIE_ATU_UNR_LOWER_LIMIT,
				 lower_32_bits(limit_addr));
	ax_pcie_writel_ob_unroll(index, PCIE_ATU_UNR_UPPER_LIMIT,
				 upper_32_bits(limit_addr));
	ax_pcie_writel_ob_unroll(index, PCIE_ATU_UNR_LOWER_TARGET,
				 lower_32_bits(pci_addr));
	ax_pcie_writel_ob_unroll(index, PCIE_ATU_UNR_UPPER_TARGET,
				 upper_32_bits(pci_addr));

	if (upper_32_bits(limit_addr) > upper_32_bits(cpu_addr))
		type |= PCIE_ATU_INCREASE_REGION_SIZE;

	ax_pcie_writel_ob_unroll(index, PCIE_ATU_UNR_REGION_CTRL1, type);
	ax_pcie_writel_ob_unroll(index, PCIE_ATU_UNR_REGION_CTRL2,
				 PCIE_ATU_ENABLE | PCIE_DMA_BYPASS);

	/*
	 * Make sure ATU enable takes effect before any subsequent config
	 * and I/O accesses.
	 */
	for (retries = 0; retries < LINK_WAIT_MAX_IATU_RETRIES; retries++) {
		ax_pcie_readl_ob_unroll(index, PCIE_ATU_UNR_REGION_CTRL2, &val);
		if (val & PCIE_ATU_ENABLE)
			return;
		mdelay(LINK_WAIT_IATU);
	}
	printk("Outbound iATU is not being enabled\n");
}

static u64 axera_pcie_cpu_addr_fixup(u32 pcie_id, u64 cpu_addr)
{

	if (pcie_id == PCIE0_ID)
		return cpu_addr & AXERA_PCIE0_CPU_TO_BUS_ADDR;
	else
		return cpu_addr & AXERA_PCIE1_CPU_TO_BUS_ADDR;
}

static void ax_pcie_prog_outbound_atu(u64 cpu_addr, u64 pci_addr, int index, u64 size)
{
	u32 ctl_id = ax_pcie_opt->local_ctl_id;

	cpu_addr = axera_pcie_cpu_addr_fixup(ctl_id, cpu_addr);
	ax_pcie_prog_outbound_atu_unroll(index, cpu_addr, pci_addr, size);
	return;
}

static void ax_pcie_prog_inbound_atu_unroll(u64 cpu_addr, int index, int bar)
{
    ax_pcie_writel_ib_unroll(index, PCIE_ATU_UNR_LOWER_TARGET,
				lower_32_bits(cpu_addr));
    ax_pcie_writel_ib_unroll(index, PCIE_ATU_UNR_UPPER_TARGET,
				upper_32_bits(cpu_addr));

    ax_pcie_writel_ib_unroll(index, PCIE_ATU_UNR_REGION_CTRL1, PCIE_ATU_TYPE_MEM);
    ax_pcie_writel_ib_unroll(index, PCIE_ATU_UNR_REGION_CTRL2,
            PCIE_ATU_ENABLE | PCIE_ATU_BAR_MODE_ENABLE | (bar << 8));
}
#endif

static unsigned int get_local_slot_number(void)
{
	struct axera_dev *ax_dev;
	unsigned int slot = 0;
	unsigned int slot_offset;
	int val;

	if (!ax_pcie_opt->is_host()) {
		ax_dev = ax_pcie_opt->slot_to_axdev(0);
		if (g_axera_pcie_opt->local_ctl_id == PCIE0_ID)
			slot_offset = PCIE0_RD_CTL_6;
		else
			slot_offset = PCIE1_RD_CTL_6;

		val =
		    readl(g_axera_pcie_opt->
			  local_controller[g_axera_pcie_opt->local_ctl_id]->glb_base +
			  slot_offset);
		slot = ((val & 0x1fe) >> 1);

		if (0 == slot) {
			axera_trace(AXERA_ERR,
				    "invalid local slot number[0x%x]!", slot);
			return -1;
		}
	}

	ax_pcie_opt->local_slotid = slot;
	return slot;
}

static unsigned int pcie_vendor_device_id(struct axera_dev *ax_dev)
{
	u32 vendor_device_id = 0x0;

	ax_pcie_opt->pci_config_read(ax_dev, CFG_VENDORID_REG,
				     &vendor_device_id);
	return vendor_device_id;
}

#ifdef IS_AX_SLAVE
static int clear_message_irq(int regno, int count, void *data)
{
	unsigned int ret = 0;

	ret = ax_mailbox_read(regno, (int *)data, count);
	axera_trace(AXERA_DEBUG, "regno = %d count = %d", regno, count);
	if (ret != count) {
		axera_trace(AXERA_ERR, "callback return %d", ret);
		return 0;
	}

	return 1;
}
#endif

#ifdef IS_AX_SLAVE
static int
slave_request_message_irq(int mailboxid, void (*handler) (int fromid, int regno, int count))
{
	int ret = 0;
	ret = ax_mailbox_register_notify(mailboxid, (void *)handler);
	if (ret < 0)
		axera_trace(AXERA_ERR, "ax mailbox register notify fail!");

	return ret;
}
#else
static irqreturn_t host_irq_handler(int irq, void *id)
{
	return IRQ_WAKE_THREAD;
}

static int host_request_message_irq(irqreturn_t(*handler) (int irq, void *id), struct axera_dev *ax_dev)
{
	int ret = 0;

	ret = devm_request_threaded_irq(&ax_dev->pdev->dev, pci_irq_vector(ax_dev->pdev, 0), host_irq_handler, handler,
					IRQF_SHARED, dev_name(&ax_dev->pdev->dev), ax_dev);
	if (ret) {
		axera_trace(AXERA_ERR,
				"Failed to request IRQ %d for MSI %d",
				pci_irq_vector(ax_dev->pdev, 0), 0);
		return false;
	}
	return 0;
}
#endif

static void release_message_irq(struct axera_dev *ax_dev)
{
#ifdef IS_AX_SLAVE
	ax_mailbox_register_notify(PCIE_MASTERID, 0);
#else
	devm_free_irq(&ax_dev->pdev->dev,
			      pci_irq_vector(ax_dev->pdev, 0), ax_dev);
	ax_dev->irq_num = 0;
#endif
}

#ifdef IS_AX_SLAVE
void ax_pcie_slave_reset_map(void)
{
	u32 ctl_id = ax_pcie_opt->local_ctl_id;
	void *dbi_base;
	u32 l;
	u32 sz;

	dbi_base = ax_pcie_opt->local_controller[ctl_id]->config_virt;
	ax_pcie_dbi_read(CFG_BAR4_REG, &l);
	ax_pcie_dbi_write(CFG_BAR4_REG, (l|(~0)));
	ax_pcie_dbi_read(CFG_BAR4_REG, &sz);
	ax_pcie_dbi_write(CFG_BAR4_REG, l);
	sz = ~(sz & (~0xf));

	if (sz == 0xfffff) {
		ax_pcie_prog_inbound_atu_unroll(SYSTEM_RESET_BASE_1M, INBOUND_INDEX2, BAR_4);
	} else {
		ax_pcie_prog_inbound_atu_unroll(SYSTEM_RESET_BASE_16M, INBOUND_INDEX2, BAR_4);
	}
}

static int ax_pcie_slave_irq_to_host(struct axera_dev *ax_dev, unsigned int irq_num)
{
	int ctl_id = g_axera_pcie_opt->local_ctl_id;
	void *glb_base;
	u32 val;
	int retrys = 10;
	unsigned long flags;

	axera_trace(AXERA_DEBUG, "Msi interrupt_num %d", irq_num);

	spin_lock_irqsave(&irq_lock, flags);
	glb_base = g_axera_pcie_opt->local_controller[ctl_id]->glb_base;

	if (ctl_id == PCIE0_ID) {
		/* clr send req and vector */
		val = axera_pcie_readl(glb_base, PIPE_PCIE0_VEN_MSI_REG);
		val &= ~(0x81f);
		axera_pcie_writel(glb_base, PIPE_PCIE0_VEN_MSI_REG, val);

		/* set msi vector */
		val = axera_pcie_readl(glb_base, PIPE_PCIE0_VEN_MSI_REG);
		val |= irq_num;
		axera_pcie_writel(glb_base, PIPE_PCIE0_VEN_MSI_REG, val);

		/* update msi vector */
		val = axera_pcie_readl(glb_base, PIPE_PCIE0_REG_UPDATE);
		val |= 0x1;
		axera_pcie_writel(glb_base, PIPE_PCIE0_REG_UPDATE, val);

		/* clr reg update */
		val = axera_pcie_readl(glb_base, PIPE_PCIE0_REG_UPDATE);
		val &= ~(0x3);
		axera_pcie_writel(glb_base, PIPE_PCIE0_REG_UPDATE, val);

		/* send msi irq */
		val = axera_pcie_readl(glb_base, PIPE_PCIE0_VEN_MSI_REG);
		val |= (0x1 << 11);
		axera_pcie_writel(glb_base, PIPE_PCIE0_VEN_MSI_REG, val);

		while (!(axera_pcie_readl(glb_base, PIPE_PCIE0_VEN_MSI_REG) & BIT(12))) {
			if (retrys == 0) {
				spin_unlock_irqrestore(&irq_lock, flags);
				return -3;
			}
			retrys--;
			udelay(10);
		}
	} else {
		/* clr send req and vector */
		val = axera_pcie_readl(glb_base, PIPE_PCIE1_VEN_MSI_REG);
		val &= ~(0x81f);
		axera_pcie_writel(glb_base, PIPE_PCIE1_VEN_MSI_REG, val);

		/* set msi vector */
		val = axera_pcie_readl(glb_base, PIPE_PCIE1_VEN_MSI_REG);
		val |= irq_num;
		axera_pcie_writel(glb_base, PIPE_PCIE1_VEN_MSI_REG, val);

		/* update msi vector */
		val = axera_pcie_readl(glb_base, PIPE_PCIE1_REG_UPDATE);
		val |= 0x1;
		axera_pcie_writel(glb_base, PIPE_PCIE1_REG_UPDATE, val);

		/* clr reg update */
		val = axera_pcie_readl(glb_base, PIPE_PCIE1_REG_UPDATE);
		val &= ~(0x3);
		axera_pcie_writel(glb_base, PIPE_PCIE1_REG_UPDATE, val);

		/* send msi irq */
		val = axera_pcie_readl(glb_base, PIPE_PCIE1_VEN_MSI_REG);
		val |= (0x1 << 11);
		axera_pcie_writel(glb_base, PIPE_PCIE1_VEN_MSI_REG, val);

		while (!(axera_pcie_readl(glb_base, PIPE_PCIE1_VEN_MSI_REG) & BIT(12))) {
			if (retrys == 0) {
				spin_unlock_irqrestore(&irq_lock, flags);
				return -3;
			}
			retrys--;
			udelay(10);
		}
	}
	spin_unlock_irqrestore(&irq_lock, flags);

	return 0;
}

#else
static void host_reset_slave_device(struct axera_dev *ax_dev)
{
	int sz;
#ifndef IS_THIRD_PARTY_PLATFORM
	unsigned long flags;
#endif

	if (!ax_dev) {
		axera_trace(AXERA_ERR,
			    "host reset to slave fail, pcie dev does not exist!");
		return;
	}

	printk("start reset slave\n");

	sz = ax_dev->bar_info[4].size;

#ifndef IS_THIRD_PARTY_PLATFORM
	ax_pcie_spin_lock(&flags);
#endif
	if (sz == 0x100000) {
		REG32(ax_dev->bar_virt[4] + CHIP_RST_SW_SET) = 0x1;
	} else {
		REG32(ax_dev->bar_virt[4] + SYSTEM_RESET_BASE_OFFSET + CHIP_RST_SW_SET) = 0x1;
	}
#ifndef IS_THIRD_PARTY_PLATFORM
	ax_pcie_spin_unlock(&flags);
#endif

	mdelay(1000);
	g_axera_pcie_opt->pci_config_restore(ax_dev);
	ax_dev->msg_checked_success = 0;
	return;
}

static int mailbox_requst_count_check(struct axera_dev *ax_dev)
{
	volatile unsigned long long host_mailbox_reqcnt;
	volatile unsigned long long slave_mailbox_clrcnt;
	unsigned int count;

	host_mailbox_reqcnt = *(volatile unsigned long long *)(ax_dev->shm_base_virt + 0x20);
	slave_mailbox_clrcnt = *(volatile unsigned long long *)(ax_dev->shm_base_virt + 0x28);
	smp_mb();
	count = (host_mailbox_reqcnt - slave_mailbox_clrcnt);
	if (count > MAILBOX_REQ_LIMIT_COUNT)
		return -1;
	return 0;
}

static int ax_pcie_host_irq_to_slave(struct axera_dev *ax_dev)
{
	int i;
	int size = 8;
	int timeout = 10;
	unsigned int infor_data, data_reg, regno;
	static volatile int mailbox_fromid = CPU0_MASTERID;
#ifndef IS_THIRD_PARTY_PLATFORM
	unsigned long flags;
#endif

	if (!ax_dev) {
		axera_trace(AXERA_ERR,
			    "host irq to slave fail, pcie dev does not exist!");
		return -4;
	}

	if (mailbox_requst_count_check(ax_dev))
		return -3;

#ifndef IS_THIRD_PARTY_PLATFORM
#ifdef SHMEM_FROM_MASTER
	ax_pcie_spin_lock(&flags);
#endif
#endif

	regno = REG32(ax_dev->bar_virt[1] + PCIE_SLOT_REQ);	//reg req
	axera_trace(AXERA_DEBUG, "regno = %x", regno);
	while (regno == 0xffffffff && timeout > 0) {
		regno = REG32(ax_dev->bar_virt[1] + PCIE_SLOT_REQ);
		timeout--;
		udelay(10);
	}

	if (regno == 0xffffffff) {
#ifndef IS_THIRD_PARTY_PLATFORM
#ifdef SHMEM_FROM_MASTER
		ax_pcie_spin_unlock(&flags);
#endif
#endif
		return -3;
	}

	data_reg = MAILBOX_REG_BASE + PCIE_DATA + (regno << 2);	//data_reg addr
	axera_trace(AXERA_DEBUG, "data_reg = %x", data_reg);
	for (i = 0; i < (size / 4); i++) {
		REG32(ax_dev->bar_virt[1] + data_reg) = 0x1;
	}
	infor_data = (mailbox_fromid << 28) | PCIE_MASTERID << 24 | size;	//use 16-19 saving masterid
	axera_trace(AXERA_DEBUG, "infor_data = %x", infor_data);
	REG32(ax_dev->bar_virt[1] + MAILBOX_REG_BASE + PCIE_INFO +
	      (regno << 2)) = infor_data;

#ifndef IS_THIRD_PARTY_PLATFORM
#ifdef SHMEM_FROM_MASTER
	ax_pcie_spin_unlock(&flags);
#endif
#endif
	smp_mb();
	return 0;
}
#endif

static int trigger_message_irq(struct axera_dev *ax_dev, unsigned int irq_num)
{
	axera_trace(AXERA_DEBUG, "trigger message irq");
#ifdef IS_AX_SLAVE
	return ax_pcie_slave_irq_to_host(ax_dev, irq_num);
#else
	return ax_pcie_host_irq_to_slave(ax_dev);
#endif
}

int ax_pcie_arch_init(struct ax_pcie_operation *handler)
{
	int ret = 0;

	ax_pcie_opt = handler;

	ax_pcie_opt->pci_vendor_id = pcie_vendor_device_id;
	ax_pcie_opt->pcie_controller = get_pcie_controller;
	ax_pcie_opt->local_slot_number = get_local_slot_number;

	ax_pcie_opt->trigger_msg_irq = trigger_message_irq;
	spin_lock_init(&s_dbi_lock);
#ifdef IS_AX_SLAVE
	spin_lock_init(&irq_lock);
	ax_pcie_opt->slave_request_msg_irq = slave_request_message_irq;
	ax_pcie_opt->clear_msg_irq = clear_message_irq;
	ax_pcie_opt->start_ib_map = ax_pcie_prog_inbound_atu_unroll;
	ax_pcie_opt->start_ob_map = ax_pcie_prog_outbound_atu;
	ax_pcie_opt->slave_reset_map = ax_pcie_slave_reset_map;
#else
	ax_pcie_opt->host_request_msg_irq = host_request_message_irq;
	ax_pcie_opt->reset_device = host_reset_slave_device;
#endif
	ax_pcie_opt->release_msg_irq = release_message_irq;

	return ret;
}

EXPORT_SYMBOL(ax_pcie_arch_init);

void ax_pcie_arch_exit(void)
{

}

EXPORT_SYMBOL(ax_pcie_arch_exit);
