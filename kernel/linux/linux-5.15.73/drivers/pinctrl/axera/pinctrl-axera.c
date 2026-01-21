/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#include <linux/io.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/of_device.h>
#include <linux/pinctrl/pinctrl.h>
#include <linux/pinctrl/pinconf-generic.h>
#include <linux/pinctrl/pinmux.h>
#include <linux/platform_device.h>
#include <linux/slab.h>
#include <linux/gpio/driver.h>
#include <linux/gpio.h>
#include <linux/delay.h>

#include "../core.h"
#include "../pinctrl-utils.h"
#include "../pinmux.h"
#include "pinctrl-axera.h"
#include "pinctrl-ax650x.h"

#define DPHY0_BASE 0x13c00000
#define DPHY1_BASE 0x13c10000
#define DPHY2_BASE 0x13c20000
#define DPHY3_BASE 0x13c30000
#define DPHY4_BASE 0x13c40000
#define DPHY5_BASE 0x13c50000
#define DPHY6_BASE 0x13c60000
#define DPHY7_BASE 0x13c70000

#define DPHYRX0_REG 0x1300d000
#define DPHYRX1_REG 0x1300e000
#define DPHYRX2_REG 0x1300f000
#define DPHYRX3_REG 0x13010000

#define DPHYRX0_BASE 0x4251200
#define DPHYRX1_BASE 0x4251400
#define DPHYRX2_BASE 0x4251600
#define DPHYRX3_BASE 0x4251800

#define LANE0  0
#define LANE1  1
#define LANE2  2
/*         lane1                    lane0                         lane2         */
/*  [RX*_CKP_C0,RX*_CKN_C1],  [RX*_DP0_A0,RX*_DN0_B0] , [RX*_DP1_A1,RX*_DN1_B1] */
/*GPIO OUTPUT MODE */
#define CORE_DIG_IOCTRL_RW_AFE_LANE0_CTRL_2_7_offset  (0x1027 * 2) //204e
#define CORE_DIG_IOCTRL_RW_AFE_LANE0_CTRL_2_8_offset  (0x1028 * 2) //2050
#define CORE_DIG_IOCTRL_RW_AFE_LANE0_CTRL_2_1_offset  (0x1021 * 2) //2042

#define CORE_DIG_IOCTRL_RW_AFE_LANE1_CTRL_2_7_offset  (0x1227 * 2)
#define CORE_DIG_IOCTRL_RW_AFE_LANE1_CTRL_2_8_offset  (0x1228 * 2)
#define CORE_DIG_IOCTRL_RW_AFE_LANE1_CTRL_2_1_offset  (0x1221 * 2)

#define CORE_DIG_IOCTRL_RW_AFE_LANE2_CTRL_2_7_offset  (0x1427 * 2)
#define CORE_DIG_IOCTRL_RW_AFE_LANE2_CTRL_2_8_offset  (0x1428 * 2)
#define CORE_DIG_IOCTRL_RW_AFE_LANE2_CTRL_2_1_offset  (0x1421 * 2)


/*GPIO OUTPUT MODE */
#define CORE_DIG_IOCTRL_RW_AFE_CB_CTRL_2_8_offset (0x1c28 * 2)//3850
#define CORE_DIG_IOCTRL_RW_AFE_CB_CTRL_2_0_offset  (0x1c20 * 2)//3840
#define CORE_DIG_IOCTRL_RW_AFE_CB_CTRL_2_7_offset  (0x1c27 * 2)//384e
#define CORE_DIG_IOCTRL_RW_AFE_CB_CTRL_2_1_offset  (0x1c21 * 2)//3842
    /*phy switch gpio output mode*/
struct ax_pinctrl {
	struct pinctrl_dev *pctldev;
	struct device *dev;
	void __iomem *base;
	spinlock_t lock;
	struct axera_pinctrl_soc_info *info;
};

static int ax_dt_node_to_map(struct pinctrl_dev *pctldev,
			     struct device_node *np_config,
			     struct pinctrl_map **map, u32 *num_maps)
{
	return pinconf_generic_dt_node_to_map(pctldev, np_config, map,
					      num_maps, PIN_MAP_TYPE_INVALID);
}

static const struct pinctrl_ops ax_pinctrl_ops = {
	.dt_node_to_map = ax_dt_node_to_map,
	.dt_free_map = pinctrl_utils_free_map,
	.get_groups_count = pinctrl_generic_get_group_count,
	.get_group_name = pinctrl_generic_get_group_name,
	.get_group_pins = pinctrl_generic_get_group_pins,
};
static int phy_switch_gpio_output(unsigned long phy_base, int p_or_n, u32 CORE_DIG_IOCTRL_RW_AFE_LANE_CTRL_2_7_offset, \
				u32 CORE_DIG_IOCTRL_RW_AFE_LANE_CTRL_2_8_offset, u32 CORE_DIG_IOCTRL_RW_AFE_LANE_CTRL_2_1_offset)
{
	unsigned int val = 0;
	void __iomem *reg = ioremap(phy_base, 0x10000);
	if (reg == NULL)
		pr_err("%s is failed\n", __func__);
	/*phy switch gpio output mode*/
	val = ioread16(reg + CORE_DIG_IOCTRL_RW_AFE_CB_CTRL_2_8_offset);
	val |= 0xf;
	iowrite16(val, reg + CORE_DIG_IOCTRL_RW_AFE_CB_CTRL_2_8_offset);
	val = ioread16(reg + CORE_DIG_IOCTRL_RW_AFE_CB_CTRL_2_7_offset);
	val |= (1 << 11);
	iowrite16(val, reg + CORE_DIG_IOCTRL_RW_AFE_CB_CTRL_2_7_offset);
	val = ioread16(reg + CORE_DIG_IOCTRL_RW_AFE_CB_CTRL_2_0_offset);
	val |= (1 << 4);
	iowrite16(val, reg + CORE_DIG_IOCTRL_RW_AFE_CB_CTRL_2_0_offset);
	val = ioread16(reg + CORE_DIG_IOCTRL_RW_AFE_CB_CTRL_2_1_offset);
	val |= (1 << 4);
	iowrite16(val, reg + CORE_DIG_IOCTRL_RW_AFE_CB_CTRL_2_1_offset);

	/*******************LAN0 [RX*_DP0_A0,RX*_DN0_B0] two bits:high bit is p,low bit is n****************/
	/*******************LAN1 [RX*_CKP_C0,RX*_CKN_C1] two bits:high bit is p,low bit is n****************/
	/*******************LAN2 [RX*_DP1_A1,RX*_DN1_B1] two bits:high bit is p,low bit is n****************/
	val = ioread16(reg + CORE_DIG_IOCTRL_RW_AFE_LANE_CTRL_2_7_offset);//2bit
	if (p_or_n) {
		val |= (1 << 3);
	} else {
		val |= (1 << 2);
	}
	iowrite16(val, reg + CORE_DIG_IOCTRL_RW_AFE_LANE_CTRL_2_7_offset);
	val = ioread16(reg + CORE_DIG_IOCTRL_RW_AFE_LANE_CTRL_2_8_offset);
	val |= (1 << 7);
	iowrite16(val, reg + CORE_DIG_IOCTRL_RW_AFE_LANE_CTRL_2_8_offset);
	udelay(5);
	val = ioread16(reg + CORE_DIG_IOCTRL_RW_AFE_LANE_CTRL_2_1_offset);//2bit
	if (p_or_n) {
		val |= (1 << 11);
		val &= ~(1 << 7);
	} else {
		val |= (1 << 10);
		val &= ~(1 << 6);
	}
	iowrite16(val, reg + CORE_DIG_IOCTRL_RW_AFE_LANE_CTRL_2_1_offset);
	val = ioread16(reg + CORE_DIG_IOCTRL_RW_AFE_LANE_CTRL_2_7_offset);//2bit
	if (p_or_n) {
		val |= (1 << 1);
	} else {
		val |= (1 << 0);
	}
	iowrite16(val, reg + CORE_DIG_IOCTRL_RW_AFE_LANE_CTRL_2_7_offset);
	val = ioread16(reg + CORE_DIG_IOCTRL_RW_AFE_LANE_CTRL_2_8_offset);
	val |= (1 << 6);
	iowrite16(val, reg + CORE_DIG_IOCTRL_RW_AFE_LANE_CTRL_2_8_offset);
	iounmap(reg);

	return 0;
}

static int phy_switch_gpio_input(unsigned long phy_base, int p_or_n, u32 CORE_DIG_IOCTRL_RW_AFE_LANE_CTRL_2_7_offset, \
				u32 CORE_DIG_IOCTRL_RW_AFE_LANE_CTRL_2_8_offset, u32 CORE_DIG_IOCTRL_RW_AFE_LANE_CTRL_2_1_offset)
{
	unsigned int val = 0;
	void __iomem *reg = ioremap(phy_base, 0x10000);
	if (reg == NULL)
		pr_err("%s is failed\n", __func__);
	/*******************LAN0 [RX*_DP0_A0,RX*_DN0_B0] two bits:high bit is p,low bit is n****************/
	/*******************LAN1 [RX*_CKP_C0,RX*_CKN_C1] two bits:high bit is p,low bit is n****************/
	/*******************LAN2 [RX*_DP1_A1,RX*_DN1_B1] two bits:high bit is p,low bit is n****************/
	val = ioread16(reg + CORE_DIG_IOCTRL_RW_AFE_LANE_CTRL_2_7_offset);//2bit
	if (p_or_n) {
		val &= ~(1 << 1);
	} else {
		val &= ~0x1;
	}
	iowrite16(val, reg + CORE_DIG_IOCTRL_RW_AFE_LANE_CTRL_2_7_offset);
	val = ioread16(reg + CORE_DIG_IOCTRL_RW_AFE_LANE_CTRL_2_8_offset);
	val |= (1 << 6);
	iowrite16(val, reg + CORE_DIG_IOCTRL_RW_AFE_LANE_CTRL_2_8_offset);
	val = ioread16(reg + CORE_DIG_IOCTRL_RW_AFE_LANE_CTRL_2_1_offset);//2bit
	if (p_or_n) {
		val &= ~(1 << 11);
	} else {
		val &= ~(1 << 10);
	}
	iowrite16(val, reg + CORE_DIG_IOCTRL_RW_AFE_LANE_CTRL_2_1_offset);
	val = ioread16(reg + CORE_DIG_IOCTRL_RW_AFE_LANE_CTRL_2_1_offset);//2bit
	val &= ~(0x3 << 8);
	iowrite16(val, reg + CORE_DIG_IOCTRL_RW_AFE_LANE_CTRL_2_1_offset);
	val = ioread16(reg + CORE_DIG_IOCTRL_RW_AFE_LANE_CTRL_2_1_offset);//2bit
	if (p_or_n) {
		val |= (1 << 7);
	} else {
		val |= (1 << 6);
	}
	iowrite16(val, reg + CORE_DIG_IOCTRL_RW_AFE_LANE_CTRL_2_1_offset);
	iounmap(reg);

	return 0;
}

static int ax_set_mux(struct pinctrl_dev *pctldev, unsigned int func_selector,
		      unsigned int group_selector)
{
	struct ax_pinctrl *axpctl = pinctrl_dev_get_drvdata(pctldev);
	struct axera_pinctrl_soc_info *info = axpctl->info;
	const struct pinctrl_pin_desc *pindesc = info->pins + group_selector;
	struct axera_pin_data *data = pindesc->drv_data;
	struct axera_mux_desc *mux;
	u32 offset, pin_reg_offset;
	struct function_desc *func;
	unsigned long flags;
	u32 val;
	/* Skip reserved pin */
	if (!data)
		return -EINVAL;

	mux = data->muxes;
	offset = data->offset;

	func = pinmux_generic_get_function(pctldev, func_selector);
	if (!func)
		return -EINVAL;

	while (mux->name) {
		if (strcmp(mux->name, func->name) == 0)
			break;
		mux++;
	}
	if (!(mux->name))
		pr_err("%s : pinctrl function not found\n", __func__);

	/************gpio fun_sel writel in DPHYRX_REG0 synchronously**********/
	if ((pindesc->number >= 91) && (pindesc->number <= 102)) {
		void __iomem *reg = ioremap(DPHYRX0_REG, 0x3ff);
		if (reg == NULL)
			pr_err("%s is failed\n", __func__);
		pin_reg_offset = ((pindesc->number) - 91 + 1) * 0xc;
		pr_debug("[axera] %s pin_reg_offset:%d, pindesc->number:%d\n", __func__, pin_reg_offset, pindesc->number);
		writel(((mux->muxval) << FUNCTION_SELECT), (reg + pin_reg_offset));
		iounmap(reg);
	}
	/************gpio fun_sel writel in DPHYRX_REG1 synchronously**********/
	if ((pindesc->number >= 103) && (pindesc->number <= 114)) {
		void __iomem *reg = ioremap(DPHYRX1_REG, 0x3ff);
		if (reg == NULL)
			pr_err("%s is failed\n", __func__);
		pin_reg_offset = ((pindesc->number) - 91 + 1) * 0xc;
		writel(((mux->muxval) << FUNCTION_SELECT), (reg + pin_reg_offset));
		iounmap(reg);
	}
	/************gpio fun_sel writel in DPHYRX_REG2 synchronously**********/
	if ((pindesc->number >= 115) && (pindesc->number <= 126)) {
		void __iomem *reg = ioremap(DPHYRX2_REG, 0x3ff);
		if (reg == NULL)
			pr_err("%s is failed\n", __func__);
		pin_reg_offset = ((pindesc->number) - 91 + 1) * 0xc;
		writel(((mux->muxval) << FUNCTION_SELECT), (reg + pin_reg_offset));
		iounmap(reg);
	}
	/************gpio fun_sel writel in DPHYRX_REG3 synchronously**********/
	if ((pindesc->number >= 127) && (pindesc->number <= 138)) {
		void __iomem *reg = ioremap(DPHYRX3_REG, 0x3ff);
		if (reg == NULL)
			pr_err("%s is failed\n", __func__);
		pin_reg_offset = ((pindesc->number) - 91 + 1) * 0xc;
		writel(((mux->muxval) << FUNCTION_SELECT), (reg + pin_reg_offset));
		iounmap(reg);
	}

	/* Found mux value to be written */
	spin_lock_irqsave(&axpctl->lock, flags);
	val = readl((axpctl->base) + offset);
	val &= ~FUNCTION_SELECT_BIT_CLEAR;
	val |= (mux->muxval) << FUNCTION_SELECT;
	writel(val, (axpctl->base) + offset);
	spin_unlock_irqrestore(&axpctl->lock, flags);
	pr_debug("%s val:%x is mux->name:%s group_selector:%d  func_selector:%d \
	                          data->offset:%x\n", __func__, val, mux->name, group_selector, func_selector, data->offset);
	return 0;
}

/****************request gpio******************/
int axera_request_gpio(struct pinctrl_dev *pctldev, struct pinctrl_gpio_range *range, unsigned offset)
{
	pr_debug("%s, %d, range->base:%d range->pin_base:%d offset:%d range->gc->offset:%d\n", __func__, __LINE__, range->base, \
	range->pin_base, range->base, range->gc->offset);
	if (range == NULL) {
		pr_err("%s: pinctrl request gpio failed\n", __func__);
		return -1;
	}
	gpio_request(range->base, NULL);
	return 0;
}

/****************free gpio******************/
void axera_free_gpio(struct pinctrl_dev *pctldev, struct pinctrl_gpio_range *range, unsigned offset)
{
	if (range == NULL) {
		pr_err("%s: pinctrl free gpio failed\n", __func__);
		return;
	}
	gpio_free(range->base);
	pr_debug("%s, %d, range->base:%d offset:%d range->gc->offset:%d\n", __func__, __LINE__, range->base, offset, range->gc->offset);
	return;
}

/****************set gpio direction******************/
int axera_gpio_direction(struct pinctrl_dev *pctldev, struct pinctrl_gpio_range *range, unsigned offset, bool input)
{
	int p_or_n;
	pr_debug("%s, %d, range->base:%d,offset:%d input:%d\n", __func__, __LINE__, range->base, offset, input);
	if (range == NULL) {
		pr_err("%s: pinctrl change gpio_direction failed\n", __func__);
		return -1;
	}
	p_or_n = (range->pin_base) % 2;
	if (input == true) {
		/****************dphyrx0***************/
		if ((range->pin_base == 91) || (range->pin_base == 92)) {
			return phy_switch_gpio_input(DPHY1_BASE, p_or_n, CORE_DIG_IOCTRL_RW_AFE_LANE1_CTRL_2_7_offset, \
				CORE_DIG_IOCTRL_RW_AFE_LANE1_CTRL_2_8_offset, CORE_DIG_IOCTRL_RW_AFE_LANE1_CTRL_2_1_offset);/*dphy switch gpio input mode*/
		}
		if ((range->pin_base == 93) || (range->pin_base == 94)) {
			return phy_switch_gpio_input(DPHY1_BASE, p_or_n, CORE_DIG_IOCTRL_RW_AFE_LANE0_CTRL_2_7_offset, \
				CORE_DIG_IOCTRL_RW_AFE_LANE0_CTRL_2_8_offset, CORE_DIG_IOCTRL_RW_AFE_LANE0_CTRL_2_1_offset);/*dphy switch gpio input mode*/
		}
		if ((range->pin_base == 95) || (range->pin_base == 96)) {
			return phy_switch_gpio_input(DPHY1_BASE, p_or_n, CORE_DIG_IOCTRL_RW_AFE_LANE2_CTRL_2_7_offset, \
				CORE_DIG_IOCTRL_RW_AFE_LANE2_CTRL_2_8_offset, CORE_DIG_IOCTRL_RW_AFE_LANE2_CTRL_2_1_offset);/*dphy switch gpio input mode*/
		}
		if ((range->pin_base == 97) || (range->pin_base == 98)) {
			return phy_switch_gpio_input(DPHY0_BASE, p_or_n, CORE_DIG_IOCTRL_RW_AFE_LANE1_CTRL_2_7_offset, \
				CORE_DIG_IOCTRL_RW_AFE_LANE1_CTRL_2_8_offset, CORE_DIG_IOCTRL_RW_AFE_LANE1_CTRL_2_1_offset);/*dphy switch gpio input mode*/
		}
		if ((range->pin_base == 99) || (range->pin_base == 100)) {
			return phy_switch_gpio_input(DPHY0_BASE, p_or_n, CORE_DIG_IOCTRL_RW_AFE_LANE0_CTRL_2_7_offset, \
				CORE_DIG_IOCTRL_RW_AFE_LANE0_CTRL_2_8_offset, CORE_DIG_IOCTRL_RW_AFE_LANE0_CTRL_2_1_offset);/*dphy switch gpio input mode*/
		}
		if ((range->pin_base == 101) || (range->pin_base == 102)) {
			return phy_switch_gpio_input(DPHY0_BASE, p_or_n, CORE_DIG_IOCTRL_RW_AFE_LANE2_CTRL_2_7_offset, \
				CORE_DIG_IOCTRL_RW_AFE_LANE2_CTRL_2_8_offset, CORE_DIG_IOCTRL_RW_AFE_LANE2_CTRL_2_1_offset);/*dphy switch gpio input mode*/
		}

		/****************dphyrx1***************/
		if ((range->pin_base == 103) || (range->pin_base == 104)) {
			return phy_switch_gpio_input(DPHY3_BASE, p_or_n, CORE_DIG_IOCTRL_RW_AFE_LANE1_CTRL_2_7_offset, \
				CORE_DIG_IOCTRL_RW_AFE_LANE1_CTRL_2_8_offset, CORE_DIG_IOCTRL_RW_AFE_LANE1_CTRL_2_1_offset);/*dphy switch gpio input mode*/
		}
		if ((range->pin_base == 105) || (range->pin_base == 106)) {
			return phy_switch_gpio_input(DPHY3_BASE, p_or_n, CORE_DIG_IOCTRL_RW_AFE_LANE0_CTRL_2_7_offset, \
				CORE_DIG_IOCTRL_RW_AFE_LANE0_CTRL_2_8_offset, CORE_DIG_IOCTRL_RW_AFE_LANE0_CTRL_2_1_offset);/*dphy switch gpio input mode*/
		}
		if ((range->pin_base == 107) || (range->pin_base == 108)) {
			return phy_switch_gpio_input(DPHY3_BASE, p_or_n, CORE_DIG_IOCTRL_RW_AFE_LANE2_CTRL_2_7_offset, \
				CORE_DIG_IOCTRL_RW_AFE_LANE2_CTRL_2_8_offset, CORE_DIG_IOCTRL_RW_AFE_LANE2_CTRL_2_1_offset);/*dphy switch gpio input mode*/
		}
		if ((range->pin_base == 109) || (range->pin_base == 110)) {
			return phy_switch_gpio_input(DPHY2_BASE, p_or_n, CORE_DIG_IOCTRL_RW_AFE_LANE1_CTRL_2_7_offset, \
				CORE_DIG_IOCTRL_RW_AFE_LANE1_CTRL_2_8_offset, CORE_DIG_IOCTRL_RW_AFE_LANE1_CTRL_2_1_offset);/*dphy switch gpio input mode*/
		}
		if ((range->pin_base == 111) || (range->pin_base == 112)) {
			return phy_switch_gpio_input(DPHY2_BASE, p_or_n, CORE_DIG_IOCTRL_RW_AFE_LANE0_CTRL_2_7_offset, \
				CORE_DIG_IOCTRL_RW_AFE_LANE0_CTRL_2_8_offset, CORE_DIG_IOCTRL_RW_AFE_LANE0_CTRL_2_1_offset);/*dphy switch gpio input mode*/
		}
		if ((range->pin_base == 113) || (range->pin_base == 114)) {
			return phy_switch_gpio_input(DPHY2_BASE, p_or_n, CORE_DIG_IOCTRL_RW_AFE_LANE2_CTRL_2_7_offset, \
				CORE_DIG_IOCTRL_RW_AFE_LANE2_CTRL_2_8_offset, CORE_DIG_IOCTRL_RW_AFE_LANE2_CTRL_2_1_offset);/*dphy switch gpio input mode*/
		}

		/****************dphyrx2***************/
		if ((range->pin_base == 115) || (range->pin_base == 116)) {
			return phy_switch_gpio_input(DPHY5_BASE, p_or_n, CORE_DIG_IOCTRL_RW_AFE_LANE1_CTRL_2_7_offset, \
				CORE_DIG_IOCTRL_RW_AFE_LANE1_CTRL_2_8_offset, CORE_DIG_IOCTRL_RW_AFE_LANE1_CTRL_2_1_offset);/*dphy switch gpio input mode*/
		}
		if ((range->pin_base == 117) || (range->pin_base == 118)) {
			return phy_switch_gpio_input(DPHY5_BASE, p_or_n, CORE_DIG_IOCTRL_RW_AFE_LANE0_CTRL_2_7_offset, \
				CORE_DIG_IOCTRL_RW_AFE_LANE0_CTRL_2_8_offset, CORE_DIG_IOCTRL_RW_AFE_LANE0_CTRL_2_1_offset);/*dphy switch gpio input mode*/
		}
		if ((range->pin_base == 119) || (range->pin_base == 120)) {
			return phy_switch_gpio_input(DPHY5_BASE, p_or_n, CORE_DIG_IOCTRL_RW_AFE_LANE2_CTRL_2_7_offset, \
				CORE_DIG_IOCTRL_RW_AFE_LANE2_CTRL_2_8_offset, CORE_DIG_IOCTRL_RW_AFE_LANE2_CTRL_2_1_offset);/*dphy switch gpio input mode*/
		}
		if ((range->pin_base == 121) || (range->pin_base == 122)) {
			return phy_switch_gpio_input(DPHY4_BASE, p_or_n, CORE_DIG_IOCTRL_RW_AFE_LANE1_CTRL_2_7_offset, \
				CORE_DIG_IOCTRL_RW_AFE_LANE1_CTRL_2_8_offset, CORE_DIG_IOCTRL_RW_AFE_LANE1_CTRL_2_1_offset);/*dphy switch gpio input mode*/
		}
		if ((range->pin_base == 123) || (range->pin_base == 124)) {
			return phy_switch_gpio_input(DPHY4_BASE, p_or_n, CORE_DIG_IOCTRL_RW_AFE_LANE0_CTRL_2_7_offset, \
				CORE_DIG_IOCTRL_RW_AFE_LANE0_CTRL_2_8_offset, CORE_DIG_IOCTRL_RW_AFE_LANE0_CTRL_2_1_offset);/*dphy switch gpio input mode*/
		}
		if ((range->pin_base == 125) || (range->pin_base == 126)) {
			return phy_switch_gpio_input(DPHY4_BASE, p_or_n, CORE_DIG_IOCTRL_RW_AFE_LANE2_CTRL_2_7_offset, \
				CORE_DIG_IOCTRL_RW_AFE_LANE2_CTRL_2_8_offset, CORE_DIG_IOCTRL_RW_AFE_LANE2_CTRL_2_1_offset);/*dphy switch gpio input mode*/
		}

		/****************dphyrx3***************/
		if ((range->pin_base == 127) || (range->pin_base == 128)) {
			return phy_switch_gpio_input(DPHY7_BASE, p_or_n, CORE_DIG_IOCTRL_RW_AFE_LANE1_CTRL_2_7_offset, \
				CORE_DIG_IOCTRL_RW_AFE_LANE1_CTRL_2_8_offset, CORE_DIG_IOCTRL_RW_AFE_LANE1_CTRL_2_1_offset);/*dphy switch gpio input mode*/
		}
		if ((range->pin_base == 129) || (range->pin_base == 130)) {
			return phy_switch_gpio_input(DPHY7_BASE, p_or_n, CORE_DIG_IOCTRL_RW_AFE_LANE0_CTRL_2_7_offset, \
				CORE_DIG_IOCTRL_RW_AFE_LANE0_CTRL_2_8_offset, CORE_DIG_IOCTRL_RW_AFE_LANE0_CTRL_2_1_offset);/*dphy switch gpio input mode*/
		}
		if ((range->pin_base == 131) || (range->pin_base == 132)) {
			return phy_switch_gpio_input(DPHY7_BASE, p_or_n, CORE_DIG_IOCTRL_RW_AFE_LANE2_CTRL_2_7_offset, \
				CORE_DIG_IOCTRL_RW_AFE_LANE2_CTRL_2_8_offset, CORE_DIG_IOCTRL_RW_AFE_LANE2_CTRL_2_1_offset);/*dphy switch gpio input mode*/
		}
		if ((range->pin_base == 133) || (range->pin_base == 134)) {
			return phy_switch_gpio_input(DPHY6_BASE, p_or_n, CORE_DIG_IOCTRL_RW_AFE_LANE1_CTRL_2_7_offset, \
				CORE_DIG_IOCTRL_RW_AFE_LANE1_CTRL_2_8_offset, CORE_DIG_IOCTRL_RW_AFE_LANE1_CTRL_2_1_offset);/*dphy switch gpio input mode*/
		}
		if ((range->pin_base == 135) || (range->pin_base == 136)) {
			return phy_switch_gpio_input(DPHY6_BASE, p_or_n, CORE_DIG_IOCTRL_RW_AFE_LANE0_CTRL_2_7_offset, \
				CORE_DIG_IOCTRL_RW_AFE_LANE0_CTRL_2_8_offset, CORE_DIG_IOCTRL_RW_AFE_LANE0_CTRL_2_1_offset);/*dphy switch gpio input mode*/
		}
		if ((range->pin_base == 137) || (range->pin_base == 138)) {
			return phy_switch_gpio_input(DPHY6_BASE, p_or_n, CORE_DIG_IOCTRL_RW_AFE_LANE2_CTRL_2_7_offset, \
				CORE_DIG_IOCTRL_RW_AFE_LANE2_CTRL_2_8_offset, CORE_DIG_IOCTRL_RW_AFE_LANE2_CTRL_2_1_offset);/*dphy switch gpio input mode*/
		}
		return  gpio_direction_input(range->base);
	}
	if (input == false) {
		if ((range->pin_base == 91) || (range->pin_base == 92)) {
			return phy_switch_gpio_output(DPHY1_BASE, p_or_n, CORE_DIG_IOCTRL_RW_AFE_LANE1_CTRL_2_7_offset, \
				CORE_DIG_IOCTRL_RW_AFE_LANE1_CTRL_2_8_offset, CORE_DIG_IOCTRL_RW_AFE_LANE1_CTRL_2_1_offset);/*dphy switch gpio output mode*/
		}
		if ((range->pin_base == 93) || (range->pin_base == 94)) {
			return phy_switch_gpio_output(DPHY1_BASE, p_or_n, CORE_DIG_IOCTRL_RW_AFE_LANE0_CTRL_2_7_offset, \
				CORE_DIG_IOCTRL_RW_AFE_LANE0_CTRL_2_8_offset, CORE_DIG_IOCTRL_RW_AFE_LANE0_CTRL_2_1_offset);/*dphy switch gpio output mode*/
		}
		if ((range->pin_base == 95) || (range->pin_base == 96)) {
			return phy_switch_gpio_output(DPHY1_BASE, p_or_n, CORE_DIG_IOCTRL_RW_AFE_LANE2_CTRL_2_7_offset, \
				CORE_DIG_IOCTRL_RW_AFE_LANE2_CTRL_2_8_offset, CORE_DIG_IOCTRL_RW_AFE_LANE2_CTRL_2_1_offset);/*dphy switch gpio output mode*/
		}
		if ((range->pin_base == 97) || (range->pin_base == 98)) {
			return phy_switch_gpio_output(DPHY0_BASE, p_or_n, CORE_DIG_IOCTRL_RW_AFE_LANE1_CTRL_2_7_offset, \
				CORE_DIG_IOCTRL_RW_AFE_LANE1_CTRL_2_8_offset, CORE_DIG_IOCTRL_RW_AFE_LANE1_CTRL_2_1_offset);/*dphy switch gpio output mode*/
		}
		if ((range->pin_base == 99) || (range->pin_base == 100)) {
			return phy_switch_gpio_output(DPHY0_BASE, p_or_n, CORE_DIG_IOCTRL_RW_AFE_LANE0_CTRL_2_7_offset, \
				CORE_DIG_IOCTRL_RW_AFE_LANE0_CTRL_2_8_offset, CORE_DIG_IOCTRL_RW_AFE_LANE0_CTRL_2_1_offset);/*dphy switch gpio output mode*/
		}
		if ((range->pin_base == 101) || (range->pin_base == 102)) {
			return phy_switch_gpio_output(DPHY0_BASE, p_or_n, CORE_DIG_IOCTRL_RW_AFE_LANE2_CTRL_2_7_offset, \
				CORE_DIG_IOCTRL_RW_AFE_LANE2_CTRL_2_8_offset, CORE_DIG_IOCTRL_RW_AFE_LANE2_CTRL_2_1_offset);/*dphy switch gpio output mode*/
		}

		/****************dphyrx1***************/
		if ((range->pin_base == 103) || (range->pin_base == 104)) {
			return phy_switch_gpio_output(DPHY3_BASE, p_or_n, CORE_DIG_IOCTRL_RW_AFE_LANE1_CTRL_2_7_offset, \
				CORE_DIG_IOCTRL_RW_AFE_LANE1_CTRL_2_8_offset, CORE_DIG_IOCTRL_RW_AFE_LANE1_CTRL_2_1_offset);/*dphy switch gpio output mode*/
		}
		if ((range->pin_base == 105) || (range->pin_base == 106)) {
			return phy_switch_gpio_output(DPHY3_BASE, p_or_n, CORE_DIG_IOCTRL_RW_AFE_LANE0_CTRL_2_7_offset, \
				CORE_DIG_IOCTRL_RW_AFE_LANE0_CTRL_2_8_offset, CORE_DIG_IOCTRL_RW_AFE_LANE0_CTRL_2_1_offset);/*dphy switch gpio output mode*/
		}
		if ((range->pin_base == 107) || (range->pin_base == 108)) {
			return phy_switch_gpio_output(DPHY3_BASE, p_or_n, CORE_DIG_IOCTRL_RW_AFE_LANE2_CTRL_2_7_offset, \
				CORE_DIG_IOCTRL_RW_AFE_LANE2_CTRL_2_8_offset, CORE_DIG_IOCTRL_RW_AFE_LANE2_CTRL_2_1_offset);/*dphy switch gpio output mode*/
		}
		if ((range->pin_base == 109) || (range->pin_base == 110)) {
			return phy_switch_gpio_output(DPHY2_BASE, p_or_n, CORE_DIG_IOCTRL_RW_AFE_LANE1_CTRL_2_7_offset, \
				CORE_DIG_IOCTRL_RW_AFE_LANE1_CTRL_2_8_offset, CORE_DIG_IOCTRL_RW_AFE_LANE1_CTRL_2_1_offset);/*dphy switch gpio output mode*/
		}
		if ((range->pin_base == 111) || (range->pin_base == 112)) {
			return phy_switch_gpio_output(DPHY2_BASE, p_or_n, CORE_DIG_IOCTRL_RW_AFE_LANE0_CTRL_2_7_offset, \
				CORE_DIG_IOCTRL_RW_AFE_LANE0_CTRL_2_8_offset, CORE_DIG_IOCTRL_RW_AFE_LANE0_CTRL_2_1_offset);/*dphy switch gpio output mode*/
		}
		if ((range->pin_base == 113) || (range->pin_base == 114)) {
			return phy_switch_gpio_output(DPHY2_BASE, p_or_n, CORE_DIG_IOCTRL_RW_AFE_LANE2_CTRL_2_7_offset, \
				CORE_DIG_IOCTRL_RW_AFE_LANE2_CTRL_2_8_offset, CORE_DIG_IOCTRL_RW_AFE_LANE2_CTRL_2_1_offset);/*dphy switch gpio output mode*/
		}

		/****************dphyrx2***************/
		if ((range->pin_base == 115) || (range->pin_base == 116)) {
			return phy_switch_gpio_output(DPHY5_BASE, p_or_n, CORE_DIG_IOCTRL_RW_AFE_LANE1_CTRL_2_7_offset, \
				CORE_DIG_IOCTRL_RW_AFE_LANE1_CTRL_2_8_offset, CORE_DIG_IOCTRL_RW_AFE_LANE1_CTRL_2_1_offset);/*dphy switch gpio output mode*/
		}
		if ((range->pin_base == 117) || (range->pin_base == 118)) {
			return phy_switch_gpio_output(DPHY5_BASE, p_or_n, CORE_DIG_IOCTRL_RW_AFE_LANE0_CTRL_2_7_offset, \
				CORE_DIG_IOCTRL_RW_AFE_LANE0_CTRL_2_8_offset, CORE_DIG_IOCTRL_RW_AFE_LANE0_CTRL_2_1_offset);/*dphy switch gpio output mode*/
		}
		if ((range->pin_base == 119) || (range->pin_base == 120)) {
			return phy_switch_gpio_output(DPHY5_BASE, p_or_n, CORE_DIG_IOCTRL_RW_AFE_LANE2_CTRL_2_7_offset, \
				CORE_DIG_IOCTRL_RW_AFE_LANE2_CTRL_2_8_offset, CORE_DIG_IOCTRL_RW_AFE_LANE2_CTRL_2_1_offset);/*dphy switch gpio output mode*/
		}
		if ((range->pin_base == 121) || (range->pin_base == 122)) {
			return phy_switch_gpio_output(DPHY4_BASE, p_or_n, CORE_DIG_IOCTRL_RW_AFE_LANE1_CTRL_2_7_offset, \
				CORE_DIG_IOCTRL_RW_AFE_LANE1_CTRL_2_8_offset, CORE_DIG_IOCTRL_RW_AFE_LANE1_CTRL_2_1_offset);/*dphy switch gpio output mode*/
		}
		if ((range->pin_base == 123) || (range->pin_base == 124)) {
			return phy_switch_gpio_output(DPHY4_BASE, p_or_n, CORE_DIG_IOCTRL_RW_AFE_LANE0_CTRL_2_7_offset, \
				CORE_DIG_IOCTRL_RW_AFE_LANE0_CTRL_2_8_offset, CORE_DIG_IOCTRL_RW_AFE_LANE0_CTRL_2_1_offset);/*dphy switch gpio output mode*/
		}
		if ((range->pin_base == 125) || (range->pin_base == 126)) {
			return phy_switch_gpio_output(DPHY4_BASE, p_or_n, CORE_DIG_IOCTRL_RW_AFE_LANE2_CTRL_2_7_offset, \
				CORE_DIG_IOCTRL_RW_AFE_LANE2_CTRL_2_8_offset, CORE_DIG_IOCTRL_RW_AFE_LANE2_CTRL_2_1_offset);/*dphy switch gpio output mode*/
		}

		/****************dphyrx3***************/
		if ((range->pin_base == 127) || (range->pin_base == 128)) {
			return phy_switch_gpio_output(DPHY7_BASE, p_or_n, CORE_DIG_IOCTRL_RW_AFE_LANE1_CTRL_2_7_offset, \
				CORE_DIG_IOCTRL_RW_AFE_LANE1_CTRL_2_8_offset, CORE_DIG_IOCTRL_RW_AFE_LANE1_CTRL_2_1_offset);/*dphy switch gpio output mode*/
		}
		if ((range->pin_base == 129) || (range->pin_base == 130)) {
			return phy_switch_gpio_output(DPHY7_BASE, p_or_n, CORE_DIG_IOCTRL_RW_AFE_LANE0_CTRL_2_7_offset, \
				CORE_DIG_IOCTRL_RW_AFE_LANE0_CTRL_2_8_offset, CORE_DIG_IOCTRL_RW_AFE_LANE0_CTRL_2_1_offset);/*dphy switch gpio output mode*/
		}
		if ((range->pin_base == 131) || (range->pin_base == 132)) {
			return phy_switch_gpio_output(DPHY7_BASE, p_or_n, CORE_DIG_IOCTRL_RW_AFE_LANE2_CTRL_2_7_offset, \
				CORE_DIG_IOCTRL_RW_AFE_LANE2_CTRL_2_8_offset, CORE_DIG_IOCTRL_RW_AFE_LANE2_CTRL_2_1_offset);/*dphy switch gpio output mode*/
		}
		if ((range->pin_base == 133) || (range->pin_base == 134)) {
			return phy_switch_gpio_output(DPHY6_BASE, p_or_n, CORE_DIG_IOCTRL_RW_AFE_LANE1_CTRL_2_7_offset, \
				CORE_DIG_IOCTRL_RW_AFE_LANE1_CTRL_2_8_offset, CORE_DIG_IOCTRL_RW_AFE_LANE1_CTRL_2_1_offset);/*dphy switch gpio output mode*/
		}
		if ((range->pin_base == 135) || (range->pin_base == 136)) {
			return phy_switch_gpio_output(DPHY6_BASE, p_or_n, CORE_DIG_IOCTRL_RW_AFE_LANE0_CTRL_2_7_offset, \
				CORE_DIG_IOCTRL_RW_AFE_LANE0_CTRL_2_8_offset, CORE_DIG_IOCTRL_RW_AFE_LANE0_CTRL_2_1_offset);/*dphy switch gpio output mode*/
		}
		if ((range->pin_base == 137) || (range->pin_base == 138)) {
			return phy_switch_gpio_output(DPHY6_BASE, p_or_n, CORE_DIG_IOCTRL_RW_AFE_LANE2_CTRL_2_7_offset, \
				CORE_DIG_IOCTRL_RW_AFE_LANE2_CTRL_2_8_offset, CORE_DIG_IOCTRL_RW_AFE_LANE2_CTRL_2_1_offset);/*dphy switch gpio output mode*/
		}
		return gpio_direction_output(range->base, !input);
	}
	pr_err("%s is failed\n", __func__);

	return 0;
}
static const struct pinmux_ops ax_pinmux_ops = {
	.get_functions_count = pinmux_generic_get_function_count,
	.get_function_name = pinmux_generic_get_function_name,
	.get_function_groups = pinmux_generic_get_function_groups,
	.set_mux = ax_set_mux,
	.gpio_request_enable = axera_request_gpio,
	.gpio_disable_free = axera_free_gpio,
	.gpio_set_direction = axera_gpio_direction,
	.strict = true,
};

static int ax_pin_config_get(struct pinctrl_dev *pctldev, unsigned int pin,
			     unsigned long *config)
{
	struct ax_pinctrl *axpctl = pinctrl_dev_get_drvdata(pctldev);
	struct axera_pinctrl_soc_info *info = axpctl->info;
	const struct pinctrl_pin_desc *pindesc = info->pins + pin;
	struct axera_pin_data *data = pindesc->drv_data;
	enum pin_config_param param = pinconf_to_config_param(*config);
	u32 val;
	u32 arg = 0;
	/* Skip reserved pin */
	if (!data)
		return -EINVAL;
	val = readl(axpctl->base + data->offset);
	switch (param) {
	case PIN_CONFIG_BIAS_PULL_DOWN:
			val &= AX_PULL_DOWN;
			if (val != 0x40)
				return -EINVAL;
			arg = 1;
		break;
	case PIN_CONFIG_BIAS_PULL_UP:
		if ((((data->offset) >= 0xc) && ((data->offset) <= 0x90)) ||
								(((data->offset) >= 0x60c) && ((data->offset) <= 0x6b4))) {
			val &= (0x3 << AX_PULL_DOWN_BIT);
			if (val != 0xc0)
				return -EINVAL;
			arg = 1;
		} else {
			val &= AX_PULL_UP;
			if (val == 0)
				return -EINVAL;
			arg = 1;
		}
		break;
	case PIN_CONFIG_BIAS_DISABLE:
		val &= (0x3 << AX_PULL_DOWN_BIT);
		if (val != 0)
			return -EINVAL;
		arg = 0;
		break;
	case PIN_CONFIG_DRIVE_STRENGTH:
		val &= AX_DRIVE_STRENGTH;
		arg = val;
		break;
	case PIN_CONFIG_INPUT_SCHMITT_ENABLE:
		val &= (1 << AX_SCHMITT_ENABLE);
		if (val != 0)
			arg = 1;
		break;
	default:
		return -ENOTSUPP;
	}

	*config = pinconf_to_config_packed(param, arg);
	return 0;
}

static int ax_pin_config_set(struct pinctrl_dev *pctldev, unsigned int pin,
			     unsigned long *configs, unsigned int num_configs)
{
	struct ax_pinctrl *axpctl = pinctrl_dev_get_drvdata(pctldev);
	struct axera_pinctrl_soc_info *info = axpctl->info;
	const struct pinctrl_pin_desc *pindesc = info->pins + pin;
	struct axera_pin_data *data = pindesc->drv_data;
	enum pin_config_param param;
	u32 val, arg;
	int i;
	/* Skip reserved pin */
	if (!data)
		return -EINVAL;

	val = readl((axpctl->base) + (data->offset));

	for (i = 0; i < num_configs; i++) {
		param = pinconf_to_config_param(configs[i]);
		arg = pinconf_to_config_argument(configs[i]);
		switch (param) {
		case PIN_CONFIG_BIAS_PULL_DOWN:
			val &= ~AX_PULL_UP;
			val |= AX_PULL_DOWN;
			break;
		case PIN_CONFIG_BIAS_PULL_UP:
			if ((((data->offset) >= 0xc) && ((data->offset) <= 0x90)) ||
						(((data->offset) >= 0x60c) && ((data->offset) <= 0x6b4))) {
				val |= AX_PULL_UP;
				val |= AX_PULL_DOWN;
			} else {
				val &= ~AX_PULL_DOWN;
				val |= AX_PULL_UP;
			}
			break;
		case PIN_CONFIG_BIAS_DISABLE:
			val &= ~AX_PULL_DOWN;
			val &= ~AX_PULL_UP;
			break;
		case PIN_CONFIG_DRIVE_STRENGTH:
			val &= ~AX_DRIVE_STRENGTH;
			val |= arg;
			break;
		case PIN_CONFIG_INPUT_SCHMITT_ENABLE:
			if (arg == 0)
				val &= ~(1 << AX_SCHMITT_ENABLE);
			else
				val |= arg << AX_SCHMITT_ENABLE;
			break;
		default:
			return -ENOTSUPP;
		}

	}
	pr_debug("%s is param:%d val:%x data->offset:%x\n", __func__, param, val, data->offset);
	writel(val, (axpctl->base) + (data->offset));
	return 0;
}

static const struct pinconf_ops ax_pinconf_ops = {
	.pin_config_set = ax_pin_config_set,
	.pin_config_get = ax_pin_config_get,
	.is_generic = true,
};

static int ax_pinctrl_build_state(struct platform_device *pdev)
{
	struct ax_pinctrl *axpctl = platform_get_drvdata(pdev);
	struct axera_pinctrl_soc_info *info = axpctl->info;
	struct pinctrl_dev *pctldev = axpctl->pctldev;
	struct function_desc *functions;
	int nfunctions;
	struct group_desc *groups;
	int ngroups;
	int i, j;

	/* Every single pin composes a group */
	ngroups = info->npins;
	groups = devm_kcalloc(&pdev->dev, ngroups, sizeof(*groups),
			      GFP_KERNEL);
	if (!groups)
		return -ENOMEM;
	for (i = 0; i < ngroups; i++) {
		const struct pinctrl_pin_desc *pindesc = info->pins + i;
		struct group_desc *group = groups + i;

		group->name = pindesc->name;
		group->pins = (int *) &pindesc->number;
		group->num_pins = 1;
		radix_tree_insert(&pctldev->pin_group_tree, i, group);
	}

	pctldev->num_groups = ngroups;

	/* Build function list from pin mux functions */
	functions = kcalloc(FUNCTION_MAX, sizeof(*functions), GFP_KERNEL);
	if (!functions)
		return -ENOMEM;
	nfunctions = 0;

	for (i = 0; i < info->npins; i++) {
		const struct pinctrl_pin_desc *pindesc = info->pins + i;
		struct axera_pin_data *data = pindesc->drv_data;
		struct axera_mux_desc *mux;
		u32 nums;
		u32 count;

		/* Reserved pins do not have a drv_data at all */
		if (!data)
			continue;

		/* Loop over all muxes for the pin */
		mux = data->muxes;
		count = data->num;
		nums += data->num;

		while (mux->name) {
			struct function_desc *func = functions;
			j = 0;
			/* Search function list for given mux */
			while (func->name && (j < nums)) {
				if (strcmp(mux->name, func->name) == 0) {
					/* Function exists */
					func->num_group_names++;
					break;
				}
				j++;
				func++;
			}

			if (!func->name) {
				/* New function */
				func->name = mux->name;
				func->num_group_names = 1;
				radix_tree_insert(&pctldev->pin_function_tree,
						  nfunctions++, func);
			}
			count--;
			if (count <= 0) {
				break;
			}
			mux++;
		}
	}

	pctldev->num_functions = nfunctions;
	pr_debug("%s  nfunctions:%d\n", __func__, nfunctions);
	functions = krealloc(functions, nfunctions * sizeof(*functions),
			     GFP_KERNEL);

	/* Find pin groups for every single function */
	for (i = 0; i < info->npins; i++) {
		const struct pinctrl_pin_desc *pindesc = info->pins + i;
		struct axera_pin_data *data = pindesc->drv_data;
		struct axera_mux_desc *mux;
		u32 count;
		if (!data)
			continue;

		mux = data->muxes;
		count = data->num;
		while (mux->name) {
			struct function_desc *func;
			const char **group;
			int j;

			/* Find function for given mux */
			for (j = 0; j < nfunctions; j++)
				if (strcmp(functions[j].name, mux->name) == 0)
					break;

			func = functions + j;
			if (!func->group_names) {
				func->group_names = devm_kcalloc(&pdev->dev,
						func->num_group_names,
						sizeof(*func->group_names),
						GFP_KERNEL);
				if (!func->group_names) {
					kfree(functions);
					return -ENOMEM;
				}
			}

			group = func->group_names;
			while (*group)
				group++;
			*group = pindesc->name;
			count--;
			if (count <= 0) {
				break;
			}
			mux++;
		}
	}

	return 0;
}

int axera_pinctrl_init(struct platform_device *pdev,
		    struct axera_pinctrl_soc_info *info)
{
	struct pinctrl_desc *pctldesc;
	struct ax_pinctrl *axpctl;
	struct resource *res;
	int ret;

	axpctl = devm_kzalloc(&pdev->dev, sizeof(*axpctl), GFP_KERNEL);
	if (!axpctl)
		return -ENOMEM;

	spin_lock_init(&axpctl->lock);

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	axpctl->base = devm_ioremap_resource(&pdev->dev, res);
	if (IS_ERR(axpctl->base))
		return PTR_ERR(axpctl->base);
	axpctl->dev = &pdev->dev;
	axpctl->info = info;

	pctldesc = devm_kzalloc(&pdev->dev, sizeof(*pctldesc), GFP_KERNEL);
	if (!pctldesc)
		return -ENOMEM;

	pctldesc->name = dev_name(&pdev->dev);
	pctldesc->owner = THIS_MODULE;
	pctldesc->pins = info->pins;
	pctldesc->npins = info->npins;
	pctldesc->pctlops = &ax_pinctrl_ops;
	pctldesc->pmxops = &ax_pinmux_ops;
	pctldesc->confops = &ax_pinconf_ops;

	axpctl->pctldev = devm_pinctrl_register(&pdev->dev, pctldesc, axpctl);
	if (IS_ERR(axpctl->pctldev)) {
		ret = PTR_ERR(axpctl->pctldev);
		dev_err(&pdev->dev, "failed to register pinctrl: %d\n", ret);
		return ret;
	}

	platform_set_drvdata(pdev, axpctl);

	ret = ax_pinctrl_build_state(pdev);
	if (ret) {
		dev_err(&pdev->dev, "failed to build state: %d\n", ret);
		return ret;
	}
	dev_info(&pdev->dev, "initialized pinctrl driver\n");
	return 0;
}
