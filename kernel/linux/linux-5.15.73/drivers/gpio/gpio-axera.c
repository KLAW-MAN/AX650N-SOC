// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (C) 2022 Axrea Corporation
 */
#include <linux/acpi.h>
#include <linux/clk.h>
#include <linux/err.h>
#include <linux/gpio/driver.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/io.h>
#include <linux/ioport.h>
#include <linux/irq.h>
#include <linux/mod_devicetable.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/platform_device.h>
#include <linux/property.h>
#include <linux/reset.h>
#include <linux/slab.h>
#include <linux/spinlock.h>

#include "gpiolib.h"
#include "gpiolib-acpi.h"
#include "../hwspinlock/hwspinlock_internal.h"

#define GPIO_SWPORTA_DR		0x00
#define GPIO_SWPORTA_DDR	0x04
#define GPIO_SWPORTB_DR		0x0c
#define GPIO_SWPORTB_DDR	0x10
#define GPIO_SWPORTC_DR		0x18
#define GPIO_SWPORTC_DDR	0x1c
#define GPIO_SWPORTD_DR		0x24
#define GPIO_SWPORTD_DDR	0x28
#define GPIO_INTEN		0x30
#define GPIO_INTMASK		0x34
#define GPIO_INTTYPE_LEVEL	0x38
#define GPIO_INT_POLARITY	0x3c
#define GPIO_INTSTATUS		0x40
#define GPIO_PORTA_DEBOUNCE	0x48
#define GPIO_PORTA_EOI		0x4c
#define GPIO_EXT_PORTA		0x50
#define GPIO_EXT_PORTB		0x54
#define GPIO_EXT_PORTC		0x58
#define GPIO_EXT_PORTD		0x5c

#define AX_DRIVER_NAME	"gpio-ax"
#define AX_MAX_PORTS		4
#define AX_MAX_GPIOS		32

#define GPIO_EXT_PORT_STRIDE	0x04	/* register stride 32 bits */
#define GPIO_SWPORT_DR_STRIDE	0x0c	/* register stride 3*32 bits */
#define GPIO_SWPORT_DDR_STRIDE	0x0c	/* register stride 3*32 bits */

#define AX_NR_CLOCKS		2
struct hwspinlock *ax_gpio_hwlock;
struct ax_gpio;
#define DPHY0_BASE 0x13c00000
#define DPHY1_BASE 0x13c10000
#define DPHY2_BASE 0x13c20000
#define DPHY3_BASE 0x13c30000
#define DPHY4_BASE 0x13c40000
#define DPHY5_BASE 0x13c50000
#define DPHY6_BASE 0x13c60000
#define DPHY7_BASE 0x13c70000

#define DPHY0_NUMBER 0
#define DPHY1_NUMBER 1
#define DPHY2_NUMBER 2
#define DPHY3_NUMBER 3
#define DPHY4_NUMBER 4
#define DPHY5_NUMBER 5
#define DPHY6_NUMBER 6
#define DPHY7_NUMBER 7

enum ax_dphy_pin {
	PHY_GPIO2_A9 = 1,
	PHY_GPIO2_A10,
	PHY_GPIO2_A11,
	PHY_GPIO2_A12,
	PHY_GPIO2_A13,
	PHY_GPIO2_A14,
	PHY_GPIO2_A15,
	PHY_GPIO2_A16,
	PHY_GPIO2_A17,
	PHY_GPIO2_A18,
	PHY_GPIO2_A19,
	PHY_GPIO2_A20,
	PHY_COM_GPIO_A24,
	PHY_COM_GPIO_A25,
	PHY_COM_GPIO_A26,
	PHY_COM_GPIO_A27,
	PHY_COM_GPIO_A28,
	PHY_COM_GPIO_A29,
	PHY_COM_GPIO_A30,
	PHY_COM_GPIO_A31,
	PHY_GPIO2_A5,
	PHY_GPIO2_A6,
	PHY_GPIO2_A7,
	PHY_GPIO2_A8,
	PHY_COM_GPIO_A12,
	PHY_COM_GPIO_A13,
	PHY_COM_GPIO_A14,
	PHY_COM_GPIO_A15,
	PHY_COM_GPIO_A16,
	PHY_COM_GPIO_A17,
	PHY_COM_GPIO_A18,
	PHY_COM_GPIO_A19,
	PHY_COM_GPIO_A20,
	PHY_COM_GPIO_A21,
	PHY_COM_GPIO_A22,
	PHY_COM_GPIO_A23,
	PHY_COM_GPIO_A0,
	PHY_COM_GPIO_A1,
	PHY_COM_GPIO_A2,
	PHY_COM_GPIO_A3,
	PHY_COM_GPIO_A4,
	PHY_COM_GPIO_A5,
	PHY_COM_GPIO_A6,
	PHY_COM_GPIO_A7,
	PHY_COM_GPIO_A8,
	PHY_COM_GPIO_A9,
	PHY_COM_GPIO_A10,
	PHY_COM_GPIO_A11,
};

/*         lane1                    lane0                         lane2         */
/*  [RX*_CKP_C0,RX*_CKN_C1],  [RX*_DP0_A0,RX*_DN0_B0] , [RX*_DP1_A1,RX*_DN1_B1] */
/*GPIO OUTPUT MODE */
#define CORE_DIG_IOCTRL_RW_AFE_LANE0_CTRL_2_7_offset  (0x1027 * 2) /*204e*/
#define CORE_DIG_IOCTRL_RW_AFE_LANE0_CTRL_2_8_offset  (0x1028 * 2) /*2050*/
#define CORE_DIG_IOCTRL_RW_AFE_LANE0_CTRL_2_1_offset  (0x1021 * 2) /*2042*/

#define CORE_DIG_IOCTRL_RW_AFE_LANE1_CTRL_2_7_offset  (0x1227 * 2)
#define CORE_DIG_IOCTRL_RW_AFE_LANE1_CTRL_2_8_offset  (0x1228 * 2)
#define CORE_DIG_IOCTRL_RW_AFE_LANE1_CTRL_2_1_offset  (0x1221 * 2)

#define CORE_DIG_IOCTRL_RW_AFE_LANE2_CTRL_2_7_offset  (0x1427 * 2)
#define CORE_DIG_IOCTRL_RW_AFE_LANE2_CTRL_2_8_offset  (0x1428 * 2)
#define CORE_DIG_IOCTRL_RW_AFE_LANE2_CTRL_2_1_offset  (0x1421 * 2)


/*GPIO OUTPUT MODE */
#define CORE_DIG_IOCTRL_RW_AFE_CB_CTRL_2_8_offset (0x1c28 * 2)/*3850*/
#define CORE_DIG_IOCTRL_RW_AFE_CB_CTRL_2_0_offset  (0x1c20 * 2)/*3840*/
#define CORE_DIG_IOCTRL_RW_AFE_CB_CTRL_2_7_offset  (0x1c27 * 2)/*384e*/
#define CORE_DIG_IOCTRL_RW_AFE_CB_CTRL_2_1_offset  (0x1c21 * 2)/*3842*/
static u32 phy_lane0_reg_value[8] = {0};
static u32 phy_lane1_reg_value[8] = {0};
static u32 phy_lane2_reg_value[8] = {0};
static u32 phy_lane0_reg_value2_7[8] = {0};
static u32 phy_lane1_reg_value2_7[8] = {0};
static u32 phy_lane2_reg_value2_7[8] = {0};
static void __iomem *phy_reg[8] = {NULL};
static spinlock_t phy_lock;
static int phy_reset_flag = 0;

struct radix_tree_root dphy_gpio_tree;
struct ax_phy_data dphy_real_pin_data[50] = {
	{73, 1},	/*AX_GPIO2_A9*/
	{74, 2},	/*AX_GPIO2_A10*/
	{75, 3},	/*AX_GPIO2_A11*/
	{76, 4},	/*AX_GPIO2_A12*/
	{77, 5},	/*AX_GPIO2_A13*/
	{78, 6},	/*AX_GPIO2_A14*/
	{79, 7},	/*AX_GPIO2_A15*/
	{80, 8},	/*AX_GPIO2_A16*/
	{81, 9},	/*AX_GPIO2_A17*/
	{82, 10},	/*AX_GPIO2_A18*/
	{83, 11},	/*AX_GPIO2_A19*/
	{84, 12},	/*AX_GPIO2_A20*/
	{184, 13},	/*AX_COM_GPIO_A24*/
	{185, 14},	/*AX_COM_GPIO_A25*/
	{186, 15},	/*AX_COM_GPIO_A26*/
	{187, 16},	/*AX_COM_GPIO_A27*/
	{188, 17},	/*AX_COM_GPIO_A28*/
	{189, 18},	/*AX_COM_GPIO_A29*/
	{190, 19},	/*AX_COM_GPIO_A30*/
	{191, 20},	/*AX_COM_GPIO_A31*/
	{69, 21},	/*AX_GPIO2_A5*/
	{70, 22},	/*AX_GPIO2_A6*/
	{71, 23},	/*AX_GPIO2_A7*/
	{72, 24},	/*AX_GPIO2_A8*/
	{172, 25},	/*AX_COM_GPIO_A12*/
	{173, 26},	/*AX_COM_GPIO_A13*/
	{174, 27},	/*AX_COM_GPIO_A14*/
	{175, 28},	/*AX_COM_GPIO_A15*/
	{176, 29},	/*AX_COM_GPIO_A16*/
	{177, 30},	/*AX_COM_GPIO_A17*/
	{178, 31},	/*AX_COM_GPIO_A18*/
	{179, 32},	/*AX_COM_GPIO_A19*/
	{180, 33},	/*AX_COM_GPIO_A20*/
	{181, 34},	/*AX_COM_GPIO_A21*/
	{182, 35},	/*AX_COM_GPIO_A22*/
	{183, 36},	/*AX_COM_GPIO_A23*/
	{160, 37},	/*AX_COM_GPIO_A0*/
	{161, 38},	/*AX_COM_GPIO_A1*/
	{162, 39},	/*AX_COM_GPIO_A2*/
	{163, 40},	/*AX_COM_GPIO_A3*/
	{164, 41},	/*AX_COM_GPIO_A4*/
	{165, 42},	/*AX_COM_GPIO_A5*/
	{166, 43},	/*AX_COM_GPIO_A6*/
	{167, 44},	/*AX_COM_GPIO_A7*/
	{168, 45},	/*AX_COM_GPIO_A8*/
	{169, 46},	/*AX_COM_GPIO_A9*/
	{170, 47},	/*AX_COM_GPIO_A10*/
	{171, 48}	/*AX_COM_GPIO_A11*/
};
struct ax_port_property {
	struct fwnode_handle *fwnode;
	unsigned int idx;
	unsigned int ngpio;
	unsigned int gpio_base;
	int irq[AX_MAX_GPIOS];
};

struct ax_platform_data {
	struct ax_port_property *properties;
	unsigned int nports;
};

#ifdef CONFIG_PM_SLEEP
/* Store GPIO context across system-wide suspend/resume transitions */
struct ax_context {
	u32 data;
	u32 dir;
	u32 ext;
	u32 int_en;
	u32 int_mask;
	u32 int_type;
	u32 int_pol;
	u32 int_deb;
	u32 wake_en;
};
#endif

struct ax_gpio_port_irqchip {
	struct irq_chip irqchip;
	unsigned int nr_irqs;
	unsigned int irq[AX_MAX_GPIOS];
};

struct ax_gpio_port {
	struct gpio_chip gc;
	struct ax_gpio_port_irqchip *pirq;
	struct ax_gpio *gpio;
#ifdef CONFIG_PM_SLEEP
	struct ax_context *ctx;
#endif
	unsigned int idx;
};
#define to_ax_gpio(_gc) \
	(container_of(_gc, struct ax_gpio_port, gc)->gpio)

struct ax_gpio {
	struct device *dev;
	void __iomem *regs;
	struct ax_gpio_port *ports;
	unsigned int nr_ports;
	unsigned int flags;
	struct reset_control *prst;
	struct reset_control *rst;
	struct clk_bulk_data clks[AX_NR_CLOCKS];
};

static inline u32 ax_read(struct ax_gpio *gpio, unsigned int offset)
{

	void __iomem *reg_base = gpio->regs;

	return readl(reg_base + offset);
}

static inline void ax_write(struct ax_gpio *gpio, unsigned int offset, u32 val)
{

	void __iomem *reg_base = gpio->regs;

	writel(val, reg_base + offset);

}

int ax_gpio_try_lock(void)
{
	int i = 0;

	if ((ax_gpio_hwlock->bank->ops->trylock(ax_gpio_hwlock))) {
		while (1) {
			i++;
			if (i > LOCK_COUNT)
				return 1;
			if (!(ax_gpio_hwlock->bank->ops->trylock(ax_gpio_hwlock)))
				return 0;
		}
	}

	return 0;
}
int ax_gpio_hwunlock(void)
{
	ax_gpio_hwlock->bank->ops->unlock(ax_gpio_hwlock);
	return 0;

}

static struct ax_gpio_port *ax_offs_to_port(struct ax_gpio *gpio,
					    unsigned int offs)
{
	struct ax_gpio_port *port;
	int i;

	for (i = 0; i < gpio->nr_ports; i++) {
		port = &gpio->ports[i];
		if (port->idx == offs / AX_MAX_GPIOS)
			return port;
	}

	return NULL;
}

static void ax_toggle_trigger(struct ax_gpio *gpio, unsigned int offs)
{
	struct ax_gpio_port *port = ax_offs_to_port(gpio, offs);
	struct gpio_chip *gc;
	u32 pol;
	int val;

	if (!port)
		return;
	gc = &port->gc;

	pol = ax_read(gpio, GPIO_INT_POLARITY);
	/* Just read the current value right out of the data register */
	val = gc->get(gc, offs % AX_MAX_GPIOS);
	if (val)
		pol &= ~BIT(offs);
	else
		pol |= BIT(offs);

	ax_write(gpio, GPIO_INT_POLARITY, pol);
}

static u32 ax_do_irq(struct ax_gpio *gpio)
{
	struct gpio_chip *gc = &gpio->ports[0].gc;
	unsigned long irq_status;
	irq_hw_number_t hwirq;

	irq_status = ax_read(gpio, GPIO_INTSTATUS);
	for_each_set_bit(hwirq, &irq_status, AX_MAX_GPIOS) {
		int gpio_irq = irq_find_mapping(gc->irq.domain, hwirq);
		u32 irq_type = irq_get_trigger_type(gpio_irq);

		generic_handle_irq(gpio_irq);

		if ((irq_type & IRQ_TYPE_SENSE_MASK) == IRQ_TYPE_EDGE_BOTH)
			ax_toggle_trigger(gpio, hwirq);
	}

	return irq_status;
}

static void ax_irq_handler(struct irq_desc *desc)
{
	struct ax_gpio *gpio = irq_desc_get_handler_data(desc);
	struct irq_chip *chip = irq_desc_get_chip(desc);

	chained_irq_enter(chip, desc);
	ax_do_irq(gpio);
	chained_irq_exit(chip, desc);
}

static irqreturn_t ax_irq_handler_mfd(int irq, void *dev_id)
{
	return IRQ_RETVAL(ax_do_irq(dev_id));
}

static void ax_irq_ack(struct irq_data *d)
{
	struct gpio_chip *gc = irq_data_get_irq_chip_data(d);
	struct ax_gpio *gpio = to_ax_gpio(gc);
	u32 val = BIT(irqd_to_hwirq(d));
	unsigned long flags;

	spin_lock_irqsave(&gc->bgpio_lock, flags);
	ax_write(gpio, GPIO_PORTA_EOI, val);
	spin_unlock_irqrestore(&gc->bgpio_lock, flags);
}

static void ax_irq_mask(struct irq_data *d)
{
	struct gpio_chip *gc = irq_data_get_irq_chip_data(d);
	struct ax_gpio *gpio = to_ax_gpio(gc);
	unsigned long flags;
	u32 val;
	spin_lock_irqsave(&gc->bgpio_lock, flags);
	val = ax_read(gpio, GPIO_INTMASK) | BIT(irqd_to_hwirq(d));
	ax_write(gpio, GPIO_INTMASK, val);
	spin_unlock_irqrestore(&gc->bgpio_lock, flags);
}

static void ax_irq_unmask(struct irq_data *d)
{
	struct gpio_chip *gc = irq_data_get_irq_chip_data(d);
	struct ax_gpio *gpio = to_ax_gpio(gc);
	unsigned long flags;
	u32 val;
	spin_lock_irqsave(&gc->bgpio_lock, flags);
	val = ax_read(gpio, GPIO_INTMASK) & ~BIT(irqd_to_hwirq(d));
	ax_write(gpio, GPIO_INTMASK, val);
	spin_unlock_irqrestore(&gc->bgpio_lock, flags);
}

static void ax_irq_enable(struct irq_data *d)
{
	struct gpio_chip *gc = irq_data_get_irq_chip_data(d);
	struct ax_gpio *gpio = to_ax_gpio(gc);
	unsigned long flags;
	u32 val;

	spin_lock_irqsave(&gc->bgpio_lock, flags);
	val = ax_read(gpio, GPIO_INTEN);
	val |= BIT(irqd_to_hwirq(d));
	ax_write(gpio, GPIO_INTEN, val);
	spin_unlock_irqrestore(&gc->bgpio_lock, flags);
}

static void ax_irq_disable(struct irq_data *d)
{
	struct gpio_chip *gc = irq_data_get_irq_chip_data(d);
	struct ax_gpio *gpio = to_ax_gpio(gc);
	unsigned long flags;
	u32 val;
	spin_lock_irqsave(&gc->bgpio_lock, flags);
	val = ax_read(gpio, GPIO_INTEN);
	val &= ~BIT(irqd_to_hwirq(d));
	ax_write(gpio, GPIO_INTEN, val);
	spin_unlock_irqrestore(&gc->bgpio_lock, flags);
}

static int ax_irq_set_type(struct irq_data *d, u32 type)
{
	struct gpio_chip *gc = irq_data_get_irq_chip_data(d);
	struct ax_gpio *gpio = to_ax_gpio(gc);
	irq_hw_number_t bit = irqd_to_hwirq(d);
	unsigned long level, polarity, flags;
	spin_lock_irqsave(&gc->bgpio_lock, flags);
	level = ax_read(gpio, GPIO_INTTYPE_LEVEL);
	polarity = ax_read(gpio, GPIO_INT_POLARITY);

	switch (type) {
	case IRQ_TYPE_EDGE_BOTH:
		level |= BIT(bit);
		ax_toggle_trigger(gpio, bit);
		break;
	case IRQ_TYPE_EDGE_RISING:
		level |= BIT(bit);
		polarity |= BIT(bit);
		break;
	case IRQ_TYPE_EDGE_FALLING:
		level |= BIT(bit);
		polarity &= ~BIT(bit);
		break;
	case IRQ_TYPE_LEVEL_HIGH:
		level &= ~BIT(bit);
		polarity |= BIT(bit);
		break;
	case IRQ_TYPE_LEVEL_LOW:
		level &= ~BIT(bit);
		polarity &= ~BIT(bit);
		break;
	}

	if (type & IRQ_TYPE_LEVEL_MASK)
		irq_set_handler_locked(d, handle_level_irq);
	else if (type & IRQ_TYPE_EDGE_BOTH)
		irq_set_handler_locked(d, handle_edge_irq);

	ax_write(gpio, GPIO_INTTYPE_LEVEL, level);
	if (type != IRQ_TYPE_EDGE_BOTH)
		ax_write(gpio, GPIO_INT_POLARITY, polarity);
	spin_unlock_irqrestore(&gc->bgpio_lock, flags);
	return 0;
}

static int ax_gpio_set_debounce(struct gpio_chip *gc,
				unsigned offset, unsigned debounce)
{
	struct ax_gpio_port *port = gpiochip_get_data(gc);
	struct ax_gpio *gpio = port->gpio;
	unsigned long flags, val_deb;
	unsigned long mask = BIT(offset);

	spin_lock_irqsave(&gc->bgpio_lock, flags);

	val_deb = ax_read(gpio, GPIO_PORTA_DEBOUNCE);
	if (debounce)
		val_deb |= mask;
	else
		val_deb &= ~mask;
	ax_write(gpio, GPIO_PORTA_DEBOUNCE, val_deb);

	spin_unlock_irqrestore(&gc->bgpio_lock, flags);

	return 0;
}

static int ax_gpio_set_config(struct gpio_chip *gc, unsigned offset,
			      unsigned long config)
{
	u32 debounce;


	if (pinconf_to_config_param(config) != PIN_CONFIG_INPUT_DEBOUNCE)
		return -ENOTSUPP;

	debounce = pinconf_to_config_argument(config);
	return ax_gpio_set_debounce(gc, offset, debounce);
}

static int ax_convert_irqs(struct ax_gpio_port_irqchip *pirq,
			   struct ax_port_property *pp)
{
	int i;


	/* Group all available IRQs into an array of parental IRQs. */
	for (i = 0; i < pp->ngpio; ++i) {
		if (!pp->irq[i])
			continue;

		pirq->irq[pirq->nr_irqs++] = pp->irq[i];
	}

	return pirq->nr_irqs ? 0 : -ENOENT;
}

static void ax_configure_irqs(struct ax_gpio *gpio,
			      struct ax_gpio_port *port,
			      struct ax_port_property *pp)
{
	struct ax_gpio_port_irqchip *pirq;
	struct gpio_chip *gc = &port->gc;
	struct gpio_irq_chip *girq;
	int err;

	pirq = devm_kzalloc(gpio->dev, sizeof(*pirq), GFP_KERNEL);
	if (!pirq)
		return;

	if (ax_convert_irqs(pirq, pp)) {
		dev_warn(gpio->dev, "no IRQ for port%d\n", pp->idx);
		goto err_kfree_pirq;
	}

	girq = &gc->irq;
	girq->handler = handle_bad_irq;
	girq->default_type = IRQ_TYPE_NONE;

	port->pirq = pirq;
	pirq->irqchip.name = AX_DRIVER_NAME;
	pirq->irqchip.irq_ack = ax_irq_ack;
	pirq->irqchip.irq_mask = ax_irq_mask;
	pirq->irqchip.irq_unmask = ax_irq_unmask;
	pirq->irqchip.irq_set_type = ax_irq_set_type;
	pirq->irqchip.irq_enable = ax_irq_enable;
	pirq->irqchip.irq_disable = ax_irq_disable;

	/*
	 * Intel ACPI-based platforms mostly have the DesignWare APB GPIO
	 * IRQ lane shared between several devices. In that case the parental
	 * IRQ has to be handled in the shared way so to be properly delivered
	 * to all the connected devices.
	 */
	if (has_acpi_companion(gpio->dev)) {
		girq->num_parents = 0;
		girq->parents = NULL;
		girq->parent_handler = NULL;

		err = devm_request_irq(gpio->dev, pp->irq[0],
				       ax_irq_handler_mfd,
				       IRQF_SHARED, AX_DRIVER_NAME, gpio);
		if (err) {
			dev_err(gpio->dev, "error requesting IRQ\n");
			goto err_kfree_pirq;
		}
	} else {
		girq->num_parents = pirq->nr_irqs;
		girq->parents = pirq->irq;
		girq->parent_handler_data = gpio;
		girq->parent_handler = ax_irq_handler;
	}

	girq->chip = &pirq->irqchip;

	return;

err_kfree_pirq:
	devm_kfree(gpio->dev, pirq);
}

static int ax_gpio_add_port(struct ax_gpio *gpio,
			    struct ax_port_property *pp, unsigned int offs)
{
	struct ax_gpio_port *port;
	void __iomem *dat, *set, *dirout;
	int err;
	unsigned long flags = 0;
#ifdef CONFIG_ARCH_AXERA
	static int gpio_base;
#endif
	port = &gpio->ports[offs];
	port->gpio = gpio;
	port->idx = pp->idx;

#ifdef CONFIG_PM_SLEEP
	port->ctx = devm_kzalloc(gpio->dev, sizeof(*port->ctx), GFP_KERNEL);
	if (!port->ctx)
		return -ENOMEM;
#endif

	dat = gpio->regs + GPIO_EXT_PORTA + pp->idx * GPIO_EXT_PORT_STRIDE;
	set = gpio->regs + GPIO_SWPORTA_DR + pp->idx * GPIO_SWPORT_DR_STRIDE;
	dirout =
	    gpio->regs + GPIO_SWPORTA_DDR + pp->idx * GPIO_SWPORT_DDR_STRIDE;
	flags |= BGPIOF_READ_OUTPUT_REG_SET;

	/* This registers 32 GPIO lines per port */
	err = bgpio_init(&port->gc, gpio->dev, 4, dat, set, NULL, dirout,
			 NULL, flags);
	if (err) {
		dev_err(gpio->dev, "failed to init gpio chip for port%d\n",
			port->idx);
		return err;
	}
#ifdef CONFIG_OF_GPIO
	port->gc.of_node = to_of_node(pp->fwnode);
#endif
	port->gc.ngpio = pp->ngpio;
	port->gc.base = gpio_base;
	gpio_base += pp->ngpio;

	/* Only port A support debounce */
	if (pp->idx == 0)
		port->gc.set_config = ax_gpio_set_config;

	/* Only port A can provide interrupts in all configurations of the IP */
	if (pp->idx == 0)
		ax_configure_irqs(gpio, port, pp);

	err = devm_gpiochip_add_data(gpio->dev, &port->gc, port);
	if (err) {
		dev_err(gpio->dev, "failed to register gpiochip for port%d\n",
			port->idx);
		return err;
	}

	return 0;
}

static void ax_get_irq(struct device *dev, struct fwnode_handle *fwnode,
		       struct ax_port_property *pp)
{
	int irq, j;

	for (j = 0; j < pp->ngpio; j++) {
		if (has_acpi_companion(dev)) {
			irq =
			    platform_get_irq_optional(to_platform_device(dev),
						      j);
		} else
			irq = fwnode_irq_get(fwnode, j);
		if (irq > 0)
			pp->irq[j] = irq;
	}
}

static struct ax_platform_data *ax_gpio_get_pdata(struct device *dev)
{
	struct fwnode_handle *fwnode;
	struct ax_platform_data *pdata;
	struct ax_port_property *pp;
	int nports;
	int i;

	nports = device_get_child_node_count(dev);
	if (nports == 0)
		return ERR_PTR(-ENODEV);

	pdata = devm_kzalloc(dev, sizeof(*pdata), GFP_KERNEL);
	if (!pdata)
		return ERR_PTR(-ENOMEM);

	pdata->properties = devm_kcalloc(dev, nports, sizeof(*pp), GFP_KERNEL);
	if (!pdata->properties)
		return ERR_PTR(-ENOMEM);

	pdata->nports = nports;

	i = 0;
	device_for_each_child_node(dev, fwnode) {
		pp = &pdata->properties[i++];
		pp->fwnode = fwnode;

		if (fwnode_property_read_u32(fwnode, "reg", &pp->idx) ||
		    pp->idx >= AX_MAX_PORTS) {
			dev_err(dev,
				"missing/invalid port index for port%d\n", i);
			fwnode_handle_put(fwnode);
			return ERR_PTR(-EINVAL);
		}

		if (fwnode_property_read_u32(fwnode, "ngpios", &pp->ngpio) &&
		    fwnode_property_read_u32(fwnode, "snps,nr-gpios",
					     &pp->ngpio)) {
			dev_info(dev,
				 "failed to get number of gpios for port%d\n",
				 i);
			pp->ngpio = AX_MAX_GPIOS;
		}

		pp->gpio_base = -1;

		/* For internal use only, new platforms mustn't exercise this */
		if (is_software_node(fwnode))
			fwnode_property_read_u32(fwnode, "gpio-base",
						 &pp->gpio_base);

		/*
		 * Only port A can provide interrupts in all configurations of
		 * the IP.
		 */
		if (pp->idx == 0)
			ax_get_irq(dev, fwnode, pp);
	}

	return pdata;
}

static void ax_assert_reset(void *data)
{
	struct ax_gpio *gpio = data;

	reset_control_assert(gpio->prst);
	reset_control_assert(gpio->rst);
}

static void ax_deassert_reset(void *data)
{
	struct ax_gpio *gpio = data;

	reset_control_deassert(gpio->prst);
	reset_control_deassert(gpio->rst);
}

static int ax_get_reset(struct ax_gpio *gpio, int flag)
{
	gpio->prst = devm_reset_control_get_optional_shared(gpio->dev, "gpio_prst");
	if (IS_ERR(gpio->prst))
		return dev_err_probe(gpio->dev, PTR_ERR(gpio->prst),
				     "Cannot get reset descriptor\n");
	gpio->rst = devm_reset_control_get_optional_shared(gpio->dev, "gpio_rst");
	if (IS_ERR(gpio->rst))
		return dev_err_probe(gpio->dev, PTR_ERR(gpio->rst),
				     "Cannot get reset descriptor\n");
	if (flag == 0) {
		ax_deassert_reset(gpio);
	}
	if (flag == 1) {
		ax_assert_reset(gpio);
	}

	return 0;
}

static int ax_get_clks(struct ax_gpio *gpio)
{
	int err;
	int ret_clk;
	int ret_pclk;

	/* Optional bus and debounce clocks */
	gpio->clks[0].id = "clk_gpio";
	gpio->clks[1].id = "pclk_gpio";
	err = devm_clk_bulk_get_optional(gpio->dev, AX_NR_CLOCKS, gpio->clks);
	if (err) {
		dev_err(gpio->dev, "Cannot get APB/Debounce clocks\n");
		return err;
	}

	ret_clk = clk_set_rate(gpio->clks[0].clk, 24000000);
	if (ret_clk != 0) {
		dev_err(gpio->dev, "Cannot set the rate of gpio_clk\n");
		return ret_clk;
	}
	ret_pclk = clk_set_rate(gpio->clks[1].clk, 200000000);
	if (ret_pclk != 0) {
		dev_err(gpio->dev, "Cannot set the rate of gpio_pclk\n");
		return ret_pclk;
	}

	err = clk_bulk_prepare_enable(AX_NR_CLOCKS, gpio->clks);
	if (err) {
		dev_err(gpio->dev, "Cannot enable CLK/PCLK clocks\n");
		return err;
	}

	return 0;
}

static const struct of_device_id ax_of_match[] = {
	{.compatible = "snps,ax-apb-gpio",.data = (void *)0},
	{ /* Sentinel */ }
};

MODULE_DEVICE_TABLE(of, ax_of_match);

static int ax_phy_switch_gpio_output(int p_or_n, u32 CORE_DIG_IOCTRL_RW_AFE_LANE_CTRL_2_1_offset, int phy_number)
{
	void __iomem *reg;
	u32 val, val2_7, offset_2_7;
	unsigned long flags;

	spin_lock_irqsave(&phy_lock, flags);
	reg = phy_reg[phy_number];
	/*LAN0 [RX*_DP0_A0,RX*_DN0_B0] two bits:high bit is p,low bit is n*/
	if(CORE_DIG_IOCTRL_RW_AFE_LANE_CTRL_2_1_offset == CORE_DIG_IOCTRL_RW_AFE_LANE0_CTRL_2_1_offset) {
		offset_2_7 = CORE_DIG_IOCTRL_RW_AFE_LANE0_CTRL_2_7_offset;
		if (p_or_n) {
			val2_7 = phy_lane0_reg_value2_7[phy_number] = phy_lane0_reg_value2_7[phy_number] | BIT(3);
			val = phy_lane0_reg_value[phy_number] = (phy_lane0_reg_value[phy_number] | BIT(11)) & (~BIT(7));
			val2_7 = phy_lane0_reg_value2_7[phy_number] = phy_lane0_reg_value2_7[phy_number] | BIT(1);
		} else {
			val2_7 = phy_lane0_reg_value2_7[phy_number] = phy_lane0_reg_value2_7[phy_number] | BIT(2);
			val = phy_lane0_reg_value[phy_number] = (phy_lane0_reg_value[phy_number] | BIT(10)) & (~BIT(6));
			val2_7 = phy_lane0_reg_value2_7[phy_number] = phy_lane0_reg_value2_7[phy_number] | BIT(0);
		}
	}
	/*LAN1 [RX*_CKP_C0,RX*_CKN_C1] two bits:high bit is p,low bit is n*/
	if(CORE_DIG_IOCTRL_RW_AFE_LANE_CTRL_2_1_offset == CORE_DIG_IOCTRL_RW_AFE_LANE1_CTRL_2_1_offset) {
		offset_2_7 = CORE_DIG_IOCTRL_RW_AFE_LANE1_CTRL_2_7_offset;
		if (p_or_n) {
			val2_7 = phy_lane1_reg_value2_7[phy_number] = phy_lane1_reg_value2_7[phy_number] | BIT(3);
			val = phy_lane1_reg_value[phy_number] = (phy_lane1_reg_value[phy_number] | BIT(11)) & (~BIT(7));
			val2_7 = phy_lane1_reg_value2_7[phy_number] = phy_lane1_reg_value2_7[phy_number] | BIT(1);
		} else {
			val2_7 = phy_lane1_reg_value2_7[phy_number] = phy_lane1_reg_value2_7[phy_number] | BIT(2);
			val = phy_lane1_reg_value[phy_number] = (phy_lane1_reg_value[phy_number] | BIT(10)) & (~BIT(6));
			val2_7 = phy_lane1_reg_value2_7[phy_number] = phy_lane1_reg_value2_7[phy_number] | BIT(0);
		}
	}
	/*LAN2 [RX*_DP1_A1,RX*_DN1_B1] two bits:high bit is p,low bit is n*/
	if(CORE_DIG_IOCTRL_RW_AFE_LANE_CTRL_2_1_offset == CORE_DIG_IOCTRL_RW_AFE_LANE2_CTRL_2_1_offset) {
		offset_2_7 = CORE_DIG_IOCTRL_RW_AFE_LANE2_CTRL_2_7_offset;
		if (p_or_n) {
			val2_7 = phy_lane2_reg_value2_7[phy_number] = phy_lane2_reg_value2_7[phy_number] | BIT(3);
			val = phy_lane2_reg_value[phy_number] = (phy_lane2_reg_value[phy_number] | BIT(11)) & (~BIT(7));
			val2_7 = phy_lane2_reg_value2_7[phy_number] = phy_lane2_reg_value2_7[phy_number] | BIT(1);
		} else {
			val2_7 = phy_lane2_reg_value2_7[phy_number] = phy_lane2_reg_value2_7[phy_number] | BIT(2);
			val = phy_lane2_reg_value[phy_number] = (phy_lane2_reg_value[phy_number] | BIT(10)) & (~BIT(6));
			val2_7 = phy_lane2_reg_value2_7[phy_number] = phy_lane2_reg_value2_7[phy_number] | BIT(0);
		}
	}
	iowrite16(val, reg + CORE_DIG_IOCTRL_RW_AFE_LANE_CTRL_2_1_offset);
	iowrite16(val2_7, reg + offset_2_7);
	spin_unlock_irqrestore(&phy_lock, flags);
	return 0;
}

static int ax_phy_switch_gpio_input(int p_or_n, u32 CORE_DIG_IOCTRL_RW_AFE_LANE_CTRL_2_1_offset, int phy_number)
{
	void __iomem *reg;
	unsigned long flags;
	u32 val, val2_7, offset_2_7;

	spin_lock_irqsave(&phy_lock, flags);
	reg = phy_reg[phy_number];
	/*LAN0 [RX*_DP0_A0,RX*_DN0_B0] two bits:high bit is p,low bit is n*/

	if(CORE_DIG_IOCTRL_RW_AFE_LANE_CTRL_2_1_offset == CORE_DIG_IOCTRL_RW_AFE_LANE0_CTRL_2_1_offset) {
		offset_2_7 = CORE_DIG_IOCTRL_RW_AFE_LANE0_CTRL_2_7_offset;
		if (p_or_n) {
			// out: 1027*2 bit 2/3 set1; msleep 5; 1027*2 bit[0:1]  set1
			// in : 1027*2 bit 2/3 set0; msleep 5; 1027*2 bit[0:1]  set0
			val2_7 = phy_lane0_reg_value2_7[phy_number] = phy_lane0_reg_value2_7[phy_number] & (~BIT(3));
			val = phy_lane0_reg_value[phy_number] = phy_lane0_reg_value[phy_number] & (~BIT(11)) | BIT(7);
			val2_7 = phy_lane0_reg_value2_7[phy_number] = phy_lane0_reg_value2_7[phy_number] & (~BIT(1));
		} else {
			val2_7 = phy_lane0_reg_value2_7[phy_number] = phy_lane0_reg_value2_7[phy_number] & (~BIT(2));
			val = phy_lane0_reg_value[phy_number] = phy_lane0_reg_value[phy_number] & (~BIT(10)) | BIT(6);
			val2_7 = phy_lane0_reg_value2_7[phy_number] = phy_lane0_reg_value2_7[phy_number] & (~BIT(0));
		}
	}
	/*LAN1 [RX*_CKP_C0,RX*_CKN_C1] two bits:high bit is p,low bit is n*/
	if(CORE_DIG_IOCTRL_RW_AFE_LANE_CTRL_2_1_offset == CORE_DIG_IOCTRL_RW_AFE_LANE1_CTRL_2_1_offset) {
		offset_2_7 = CORE_DIG_IOCTRL_RW_AFE_LANE1_CTRL_2_7_offset;
		if (p_or_n) {
			val2_7 = phy_lane1_reg_value2_7[phy_number] = phy_lane1_reg_value2_7[phy_number] & (~BIT(3));
			val = phy_lane1_reg_value[phy_number] = phy_lane1_reg_value[phy_number] & (~BIT(11)) | BIT(7);
			val2_7 = phy_lane1_reg_value2_7[phy_number] = phy_lane1_reg_value2_7[phy_number] & (~BIT(1));
		} else {
			val2_7 = phy_lane1_reg_value2_7[phy_number] = phy_lane1_reg_value2_7[phy_number] & (~BIT(2));
			val = phy_lane1_reg_value[phy_number] = phy_lane1_reg_value[phy_number] & (~BIT(10)) | BIT(6);
			val2_7 = phy_lane1_reg_value2_7[phy_number] = phy_lane1_reg_value2_7[phy_number] & (~BIT(0));
		}
	}
	/*LAN2 [RX*_DP1_A1,RX*_DN1_B1] two bits:high bit is p,low bit is n*/
	if(CORE_DIG_IOCTRL_RW_AFE_LANE_CTRL_2_1_offset == CORE_DIG_IOCTRL_RW_AFE_LANE2_CTRL_2_1_offset) {
		offset_2_7 = CORE_DIG_IOCTRL_RW_AFE_LANE2_CTRL_2_7_offset;
		if (p_or_n) {
			val2_7 = phy_lane2_reg_value2_7[phy_number] = phy_lane2_reg_value2_7[phy_number] & (~BIT(3));
			val = phy_lane2_reg_value[phy_number] = phy_lane2_reg_value[phy_number] & (~BIT(11)) | BIT(7);
			val2_7 = phy_lane2_reg_value2_7[phy_number] = phy_lane2_reg_value2_7[phy_number] & (~BIT(1));
		} else {
			val2_7 = phy_lane2_reg_value2_7[phy_number] = phy_lane2_reg_value2_7[phy_number] & (~BIT(2));
			val = phy_lane2_reg_value[phy_number] = phy_lane2_reg_value[phy_number] & (~BIT(10)) | BIT(6);
			val2_7 = phy_lane2_reg_value2_7[phy_number] = phy_lane2_reg_value2_7[phy_number] & (~BIT(0));
		}
	}
	iowrite16(val, reg + CORE_DIG_IOCTRL_RW_AFE_LANE_CTRL_2_1_offset);
	iowrite16(val2_7, reg + offset_2_7);
	spin_unlock_irqrestore(&phy_lock, flags);
	return 0;
}
void ax_phy_reg_unmap(int phy_number)
{
	iounmap(phy_reg[phy_number]);
}

int ax_phy_dir(int p_or_n, u32 CORE_DIG_IOCTRL_RW_AFE_LANE_CTRL_2_1_offset, int phy_number, bool ax_direction)
{
	if (ax_direction)
		ax_phy_switch_gpio_input(p_or_n, CORE_DIG_IOCTRL_RW_AFE_LANE_CTRL_2_1_offset, phy_number);
	else
		ax_phy_switch_gpio_output(p_or_n, CORE_DIG_IOCTRL_RW_AFE_LANE_CTRL_2_1_offset, phy_number);
	return 0;
}
/*input_or_output is 1 to input, input_or_output is 0 to output*/
int axera_phygpio_direction(int gpio_number, bool ax_direction)
{
	int p_or_n;
	p_or_n = (gpio_number) % 2;
	u32 CORE_DIG_IOCTRL_RW_AFE_LANE_CTRL_2_1_offset;
	int phy_num;
	switch (gpio_number) {
	/*dphyrx0*/
	case 1 ... 2:
		CORE_DIG_IOCTRL_RW_AFE_LANE_CTRL_2_1_offset = CORE_DIG_IOCTRL_RW_AFE_LANE1_CTRL_2_1_offset;
		phy_num =  DPHY1_NUMBER;
		break;
	case 3 ... 4:
		CORE_DIG_IOCTRL_RW_AFE_LANE_CTRL_2_1_offset = CORE_DIG_IOCTRL_RW_AFE_LANE0_CTRL_2_1_offset;
		phy_num =  DPHY1_NUMBER;
		break;
	case 5 ... 6:
		CORE_DIG_IOCTRL_RW_AFE_LANE_CTRL_2_1_offset = CORE_DIG_IOCTRL_RW_AFE_LANE2_CTRL_2_1_offset;
		phy_num =  DPHY1_NUMBER;
		break;
	case 7 ... 8:
		CORE_DIG_IOCTRL_RW_AFE_LANE_CTRL_2_1_offset = CORE_DIG_IOCTRL_RW_AFE_LANE1_CTRL_2_1_offset;
		phy_num =  DPHY0_NUMBER;
		break;
	case 9 ... 10:
		CORE_DIG_IOCTRL_RW_AFE_LANE_CTRL_2_1_offset = CORE_DIG_IOCTRL_RW_AFE_LANE0_CTRL_2_1_offset;
		phy_num =  DPHY0_NUMBER;
		break;
	case 11 ... 12:
		CORE_DIG_IOCTRL_RW_AFE_LANE_CTRL_2_1_offset = CORE_DIG_IOCTRL_RW_AFE_LANE2_CTRL_2_1_offset;
		phy_num =  DPHY0_NUMBER;
		break;
	/*dphyrx1*/
	case 13 ... 14:
		CORE_DIG_IOCTRL_RW_AFE_LANE_CTRL_2_1_offset = CORE_DIG_IOCTRL_RW_AFE_LANE1_CTRL_2_1_offset;
		phy_num =  DPHY3_NUMBER;
		break;
	case 15 ... 16:
		CORE_DIG_IOCTRL_RW_AFE_LANE_CTRL_2_1_offset = CORE_DIG_IOCTRL_RW_AFE_LANE0_CTRL_2_1_offset;
		phy_num =  DPHY3_NUMBER;
		break;
	case 17 ... 18:
		CORE_DIG_IOCTRL_RW_AFE_LANE_CTRL_2_1_offset = CORE_DIG_IOCTRL_RW_AFE_LANE2_CTRL_2_1_offset;
		phy_num =  DPHY3_NUMBER;
		break;
	case 19 ... 20:
		CORE_DIG_IOCTRL_RW_AFE_LANE_CTRL_2_1_offset = CORE_DIG_IOCTRL_RW_AFE_LANE1_CTRL_2_1_offset;
		phy_num =  DPHY2_NUMBER;
		break;
	case 21 ... 22:
		CORE_DIG_IOCTRL_RW_AFE_LANE_CTRL_2_1_offset = CORE_DIG_IOCTRL_RW_AFE_LANE0_CTRL_2_1_offset;
		phy_num =  DPHY2_NUMBER;
		break;
	case 23 ... 24:
		CORE_DIG_IOCTRL_RW_AFE_LANE_CTRL_2_1_offset = CORE_DIG_IOCTRL_RW_AFE_LANE2_CTRL_2_1_offset;
		phy_num =  DPHY2_NUMBER;
		break;
	/*dphyrx2*/
	case 25 ... 26:
		CORE_DIG_IOCTRL_RW_AFE_LANE_CTRL_2_1_offset = CORE_DIG_IOCTRL_RW_AFE_LANE1_CTRL_2_1_offset;
		phy_num =  DPHY5_NUMBER;
		break;
	case 27 ... 28:
		CORE_DIG_IOCTRL_RW_AFE_LANE_CTRL_2_1_offset = CORE_DIG_IOCTRL_RW_AFE_LANE0_CTRL_2_1_offset;
		phy_num =  DPHY5_NUMBER;
		break;
	case 29 ... 30:
		CORE_DIG_IOCTRL_RW_AFE_LANE_CTRL_2_1_offset = CORE_DIG_IOCTRL_RW_AFE_LANE2_CTRL_2_1_offset;
		phy_num =  DPHY5_NUMBER;
		break;
	case 31 ... 32:
		CORE_DIG_IOCTRL_RW_AFE_LANE_CTRL_2_1_offset = CORE_DIG_IOCTRL_RW_AFE_LANE1_CTRL_2_1_offset;
		phy_num =  DPHY4_NUMBER;
		break;
	case 33 ... 34:
		CORE_DIG_IOCTRL_RW_AFE_LANE_CTRL_2_1_offset = CORE_DIG_IOCTRL_RW_AFE_LANE0_CTRL_2_1_offset;
		phy_num =  DPHY4_NUMBER;
		break;
	case 35 ... 36:
		CORE_DIG_IOCTRL_RW_AFE_LANE_CTRL_2_1_offset = CORE_DIG_IOCTRL_RW_AFE_LANE2_CTRL_2_1_offset;
		phy_num =  DPHY4_NUMBER;
		break;
	/*dphyrx3*/
	case 37 ... 38:
		CORE_DIG_IOCTRL_RW_AFE_LANE_CTRL_2_1_offset = CORE_DIG_IOCTRL_RW_AFE_LANE1_CTRL_2_1_offset;
		phy_num =  DPHY7_NUMBER;
		break;
	case 39 ... 40:
		CORE_DIG_IOCTRL_RW_AFE_LANE_CTRL_2_1_offset = CORE_DIG_IOCTRL_RW_AFE_LANE0_CTRL_2_1_offset;
		phy_num =  DPHY7_NUMBER;
		break;
	case 41 ... 42:
		CORE_DIG_IOCTRL_RW_AFE_LANE_CTRL_2_1_offset = CORE_DIG_IOCTRL_RW_AFE_LANE2_CTRL_2_1_offset;
		phy_num =  DPHY7_NUMBER;
		break;
	case 43 ... 44:
		CORE_DIG_IOCTRL_RW_AFE_LANE_CTRL_2_1_offset = CORE_DIG_IOCTRL_RW_AFE_LANE1_CTRL_2_1_offset;
		phy_num =  DPHY6_NUMBER;
		break;
	case 45 ... 46:
		CORE_DIG_IOCTRL_RW_AFE_LANE_CTRL_2_1_offset = CORE_DIG_IOCTRL_RW_AFE_LANE0_CTRL_2_1_offset;
		phy_num =  DPHY6_NUMBER;
		break;
	case 47 ... 48:
		CORE_DIG_IOCTRL_RW_AFE_LANE_CTRL_2_1_offset = CORE_DIG_IOCTRL_RW_AFE_LANE2_CTRL_2_1_offset;
		phy_num =  DPHY6_NUMBER;
		break;
	default:
		pr_err("ax_gpio: phy_num is wrong\n");
		return -EINVAL;
	}
	ax_phy_dir(p_or_n, CORE_DIG_IOCTRL_RW_AFE_LANE_CTRL_2_1_offset, phy_num, ax_direction);
	return 0;
}

static int ax_gpio_probe(struct platform_device *pdev)
{
	unsigned int i;
	struct ax_gpio *gpio;
	int err, hwlock_id;
	struct ax_platform_data *pdata;
	struct device *dev = &pdev->dev;


	if(phy_reset_flag == 0) {
		for(i=0;i<8;i++){
			phy_reg[i] = ioremap(DPHY0_BASE + 0x10000 * i, 0x10000);
			phy_lane0_reg_value[i] = ioread16((phy_reg[i]) + CORE_DIG_IOCTRL_RW_AFE_LANE0_CTRL_2_1_offset);
			phy_lane1_reg_value[i] = ioread16((phy_reg[i]) + CORE_DIG_IOCTRL_RW_AFE_LANE1_CTRL_2_1_offset);
			phy_lane2_reg_value[i] = ioread16((phy_reg[i]) + CORE_DIG_IOCTRL_RW_AFE_LANE2_CTRL_2_1_offset);
			phy_lane0_reg_value2_7[i] = ioread16((phy_reg[i]) + CORE_DIG_IOCTRL_RW_AFE_LANE0_CTRL_2_7_offset);
			phy_lane1_reg_value2_7[i] = ioread16((phy_reg[i]) + CORE_DIG_IOCTRL_RW_AFE_LANE1_CTRL_2_7_offset);
			phy_lane2_reg_value2_7[i] = ioread16((phy_reg[i]) + CORE_DIG_IOCTRL_RW_AFE_LANE2_CTRL_2_7_offset);
		}

		for(i=0; i<48; i++){
			radix_tree_preload(GFP_KERNEL);
			radix_tree_insert(&dphy_gpio_tree, dphy_real_pin_data[i].number, &dphy_real_pin_data[i]);
			radix_tree_preload_end();
		}
	}
	phy_reset_flag = 1;
	spin_lock_init(&phy_lock);

	pdata = ax_gpio_get_pdata(dev);
	if (IS_ERR(pdata))
		return PTR_ERR(pdata);

	gpio = devm_kzalloc(&pdev->dev, sizeof(*gpio), GFP_KERNEL);
	if (!gpio)
		return -ENOMEM;

	gpio->dev = &pdev->dev;
	gpio->nr_ports = pdata->nports;

	err = ax_get_reset(gpio, 0);
	if (err)
		return err;

	gpio->ports = devm_kcalloc(&pdev->dev, gpio->nr_ports,
				   sizeof(*gpio->ports), GFP_KERNEL);
	if (!gpio->ports)
		return -ENOMEM;

	gpio->regs = devm_platform_ioremap_resource(pdev, 0);
	if (IS_ERR(gpio->regs))
		return PTR_ERR(gpio->regs);

	err = ax_get_clks(gpio);
	if (err)
		return err;

	gpio->flags = (uintptr_t) device_get_match_data(dev);

	for (i = 0; i < gpio->nr_ports; i++) {
		err = ax_gpio_add_port(gpio, &pdata->properties[i], i);
		if (err)
			return err;
	}

	err = device_property_read_u32(dev, "hwlock_id", &hwlock_id);
	if (err == 0)
		ax_gpio_hwlock = hwspin_lock_request_specific(hwlock_id);

	platform_set_drvdata(pdev, gpio);

	return 0;
}

#ifdef CONFIG_PM_SLEEP
static int ax_gpio_suspend(struct device *dev)
{
	struct ax_gpio *gpio = dev_get_drvdata(dev);
	struct gpio_chip *gc = &gpio->ports[0].gc;
	unsigned long flags;
	int i;
	int err;
	spin_lock_irqsave(&gc->bgpio_lock, flags);
	for (i = 0; i < gpio->nr_ports; i++) {
		unsigned int offset;
		unsigned int idx = gpio->ports[i].idx;
		struct ax_context *ctx = gpio->ports[i].ctx;

		offset = GPIO_SWPORTA_DDR + idx * GPIO_SWPORT_DDR_STRIDE;
		ctx->dir = ax_read(gpio, offset);

		offset = GPIO_SWPORTA_DR + idx * GPIO_SWPORT_DR_STRIDE;
		ctx->data = ax_read(gpio, offset);

		offset = GPIO_EXT_PORTA + idx * GPIO_EXT_PORT_STRIDE;
		ctx->ext = ax_read(gpio, offset);

		/* Only port A can provide interrupts */
		if (idx == 0) {
			ctx->int_mask = ax_read(gpio, GPIO_INTMASK);
			ctx->int_en = ax_read(gpio, GPIO_INTEN);
			ctx->int_pol = ax_read(gpio, GPIO_INT_POLARITY);
			ctx->int_type = ax_read(gpio, GPIO_INTTYPE_LEVEL);
			ctx->int_deb = ax_read(gpio, GPIO_PORTA_DEBOUNCE);

			/* Mask out interrupts */
			ax_write(gpio, GPIO_INTMASK, ~ctx->wake_en);
		}
	}
	spin_unlock_irqrestore(&gc->bgpio_lock, flags);

	clk_bulk_disable_unprepare(AX_NR_CLOCKS, gpio->clks);
	err = ax_get_reset(gpio, 1);
	if (err)
		return err;

	return 0;
}

static int ax_gpio_resume(struct device *dev)
{
	struct ax_gpio *gpio = dev_get_drvdata(dev);
	struct gpio_chip *gc = &gpio->ports[0].gc;
	unsigned long flags;
	int i, err, err1;

	err = clk_bulk_prepare_enable(AX_NR_CLOCKS, gpio->clks);
	if (err) {
		dev_err(gpio->dev, "Cannot reenable CLK/PCLK clocks\n");
		return err;
	}

	err1 = ax_get_reset(gpio, 0);
	if (err1)
		return err1;

	spin_lock_irqsave(&gc->bgpio_lock, flags);
	for (i = 0; i < gpio->nr_ports; i++) {
		unsigned int offset;
		unsigned int idx = gpio->ports[i].idx;
		struct ax_context *ctx = gpio->ports[i].ctx;

		offset = GPIO_SWPORTA_DR + idx * GPIO_SWPORT_DR_STRIDE;
		ax_write(gpio, offset, ctx->data);

		offset = GPIO_SWPORTA_DDR + idx * GPIO_SWPORT_DDR_STRIDE;
		ax_write(gpio, offset, ctx->dir);

		offset = GPIO_EXT_PORTA + idx * GPIO_EXT_PORT_STRIDE;
		ax_write(gpio, offset, ctx->ext);

		/* Only port A can provide interrupts */
		if (idx == 0) {
			ax_write(gpio, GPIO_INTTYPE_LEVEL, ctx->int_type);
			ax_write(gpio, GPIO_INT_POLARITY, ctx->int_pol);
			ax_write(gpio, GPIO_PORTA_DEBOUNCE, ctx->int_deb);
			ax_write(gpio, GPIO_INTEN, ctx->int_en);
			ax_write(gpio, GPIO_INTMASK, ctx->int_mask);

			/* Clear out spurious interrupts */
			ax_write(gpio, GPIO_PORTA_EOI, 0xffffffff);
		}
	}
	spin_unlock_irqrestore(&gc->bgpio_lock, flags);

	return 0;
}
#endif

static SIMPLE_DEV_PM_OPS(ax_gpio_pm_ops, ax_gpio_suspend, ax_gpio_resume);

static struct platform_driver ax_gpio_driver = {
	.driver = {
		    .name = AX_DRIVER_NAME,
		    .pm = &ax_gpio_pm_ops,
		    .of_match_table = ax_of_match,
		    },
	.probe = ax_gpio_probe,
};

module_platform_driver(ax_gpio_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("AXERA");
MODULE_DESCRIPTION("Synopsys DesignWare AXERA GPIO driver");
MODULE_ALIAS("platform:" AX_DRIVER_NAME);
