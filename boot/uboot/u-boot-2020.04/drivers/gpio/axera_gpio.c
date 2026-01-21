// SPDX-License-Identifier: GPL-2.0+
/*
 * (C) Copyright 2015 Marek Vasut <marex@denx.de>
 *
 * DesignWare APB GPIO driver
 */

#include <common.h>
#include <malloc.h>
#include <asm/arch/gpio.h>
#include <asm/gpio.h>
#include <asm/io.h>
#include <linux/io.h>
#include <dm.h>
#include <dm/device-internal.h>
#include <dm/device_compat.h>
#include <dm/devres.h>
#include <dm/lists.h>
#include <dm/root.h>
#include <errno.h>
#include <reset.h>

#define GPIO_SWPORT_DR(p)	(0x00 + (p) * 0xc)
#define GPIO_SWPORT_DDR(p)	(0x04 + (p) * 0xc)
#define GPIO_INTEN		0x30
#define GPIO_INTMASK		0x34
#define GPIO_INTTYPE_LEVEL	0x38
#define GPIO_INT_POLARITY	0x3c
#define GPIO_INTSTATUS		0x40
#define GPIO_PORTA_DEBOUNCE	0x48
#define GPIO_PORTA_EOI		0x4c
#define GPIO_EXT_PORT(p)	(0x50 + (p) * 4)

#define reg_num 1
#define GPIO_REG_EB_RST_SZ (sizeof(struct gpio_reg_eb_rst) / sizeof(uint))
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
#define AX_INPUT 1
#define AX_OUTPUT 0
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

static u32 phy_lane0_reg_value[8] = {0};
static u32 phy_lane1_reg_value[8] = {0};
static u32 phy_lane2_reg_value[8] = {0};
static u32 phy_lane0_reg_value2_7[8] = {0};
static u32 phy_lane1_reg_value2_7[8] = {0};
static u32 phy_lane2_reg_value2_7[8] = {0};
static int phy_reset_flag = 0;
static spinlock_t phy_lock __attribute__((unused));
struct ax_phy_data {
	int number;
	int id;
};
struct lnode {
	struct ax_phy_data data;
	struct lnode *next;
};
static struct lnode *root_head;
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
struct gpio_reg_eb_rst {
	uint off;
	uint function_value;
};
struct gpio_ax_priv {
	struct reset_ctl_bulk resets;
};
static int ax_gpio_base = 0;
struct gpio_ax_platdata {
	const char *name;
	int bank;
	int pins;
	fdt_addr_t base;
	int gpio_base;
	fdt_addr_t clk_rst_base;
	struct gpio_reg_eb_rst clk_eb_set;
	struct gpio_reg_eb_rst pclk_eb_set;
	struct gpio_reg_eb_rst sw_rst_set;
};

static int ax_phy_switch_gpio_output(int p_or_n, u32 CORE_DIG_IOCTRL_RW_AFE_LANE_CTRL_2_1_offset, int phy_number)
{
	fdt_addr_t reg;
	u32 val = 0, val2_7 = 0, offset_2_7 = 0;

	spin_lock(&phy_lock);
	reg = DPHY0_BASE + 0x10000 * phy_number;
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
	iowrite16(val, (void __iomem *)(reg + CORE_DIG_IOCTRL_RW_AFE_LANE_CTRL_2_1_offset));
	iowrite16(val2_7, (void __iomem *)(reg + offset_2_7));
	spin_unlock(&phy_lock);
	return 0;
}

static int ax_phy_switch_gpio_input(int p_or_n, u32 CORE_DIG_IOCTRL_RW_AFE_LANE_CTRL_2_1_offset, int phy_number)
{
	fdt_addr_t reg;
	u32 val = 0, val2_7 = 0, offset_2_7 = 0;

	spin_lock(&phy_lock);
	reg = DPHY0_BASE + 0x10000 * phy_number;
	/*LAN0 [RX*_DP0_A0,RX*_DN0_B0] two bits:high bit is p,low bit is n*/

	if(CORE_DIG_IOCTRL_RW_AFE_LANE_CTRL_2_1_offset == CORE_DIG_IOCTRL_RW_AFE_LANE0_CTRL_2_1_offset) {
		offset_2_7 = CORE_DIG_IOCTRL_RW_AFE_LANE0_CTRL_2_7_offset;
		if (p_or_n) {
			// out: 1027*2 bit 2/3 set1; msleep 5; 1027*2 bit[0:1]  set1
			// in : 1027*2 bit 2/3 set0; msleep 5; 1027*2 bit[0:1]  set0
			val2_7 = phy_lane0_reg_value2_7[phy_number] = phy_lane0_reg_value2_7[phy_number] & (~BIT(3));
			val = phy_lane0_reg_value[phy_number] = (phy_lane0_reg_value[phy_number] & (~BIT(11))) | BIT(7);
			val2_7 = phy_lane0_reg_value2_7[phy_number] = phy_lane0_reg_value2_7[phy_number] & (~BIT(1));
		} else {
			val2_7 = phy_lane0_reg_value2_7[phy_number] = phy_lane0_reg_value2_7[phy_number] & (~BIT(2));
			val = phy_lane0_reg_value[phy_number] = (phy_lane0_reg_value[phy_number] & (~BIT(10))) | BIT(6);
			val2_7 = phy_lane0_reg_value2_7[phy_number] = phy_lane0_reg_value2_7[phy_number] & (~BIT(0));
		}
	}
	/*LAN1 [RX*_CKP_C0,RX*_CKN_C1] two bits:high bit is p,low bit is n*/
	if(CORE_DIG_IOCTRL_RW_AFE_LANE_CTRL_2_1_offset == CORE_DIG_IOCTRL_RW_AFE_LANE1_CTRL_2_1_offset) {
		offset_2_7 = CORE_DIG_IOCTRL_RW_AFE_LANE1_CTRL_2_7_offset;
		if (p_or_n) {
			val2_7 = phy_lane1_reg_value2_7[phy_number] = phy_lane1_reg_value2_7[phy_number] & (~BIT(3));
			val = phy_lane1_reg_value[phy_number] = (phy_lane1_reg_value[phy_number] & (~BIT(11))) | BIT(7);
			val2_7 = phy_lane1_reg_value2_7[phy_number] = phy_lane1_reg_value2_7[phy_number] & (~BIT(1));
		} else {
			val2_7 = phy_lane1_reg_value2_7[phy_number] = phy_lane1_reg_value2_7[phy_number] & (~BIT(2));
			val = phy_lane1_reg_value[phy_number] = (phy_lane1_reg_value[phy_number] & (~BIT(10))) | BIT(6);
			val2_7 = phy_lane1_reg_value2_7[phy_number] = phy_lane1_reg_value2_7[phy_number] & (~BIT(0));
		}
	}
	/*LAN2 [RX*_DP1_A1,RX*_DN1_B1] two bits:high bit is p,low bit is n*/
	if(CORE_DIG_IOCTRL_RW_AFE_LANE_CTRL_2_1_offset == CORE_DIG_IOCTRL_RW_AFE_LANE2_CTRL_2_1_offset) {
		offset_2_7 = CORE_DIG_IOCTRL_RW_AFE_LANE2_CTRL_2_7_offset;
		if (p_or_n) {
			val2_7 = phy_lane2_reg_value2_7[phy_number] = phy_lane2_reg_value2_7[phy_number] & (~BIT(3));
			val = phy_lane2_reg_value[phy_number] = (phy_lane2_reg_value[phy_number] & (~BIT(11))) | BIT(7);
			val2_7 = phy_lane2_reg_value2_7[phy_number] = phy_lane2_reg_value2_7[phy_number] & (~BIT(1));
		} else {
			val2_7 = phy_lane2_reg_value2_7[phy_number] = phy_lane2_reg_value2_7[phy_number] & (~BIT(2));
			val = phy_lane2_reg_value[phy_number] = (phy_lane2_reg_value[phy_number] & (~BIT(10))) | BIT(6);
			val2_7 = phy_lane2_reg_value2_7[phy_number] = phy_lane2_reg_value2_7[phy_number] & (~BIT(0));
		}
	}

	iowrite16(val, (void __iomem *)(reg + CORE_DIG_IOCTRL_RW_AFE_LANE_CTRL_2_1_offset));
	iowrite16(val2_7, (void __iomem *)(reg + offset_2_7));

	spin_unlock(&phy_lock);
	return 0;
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
struct lnode *search_gpio_node(struct lnode *head, int value) {
	struct lnode *node = head;
	while (node != NULL && node->data.number != value) {
		node = node->next;
	}
	return node;
}

static int ax_gpio_direction_input(struct udevice *dev, unsigned pin)
{
	struct gpio_ax_platdata *plat = dev_get_platdata(dev);
	int phy_gpio_number;
	struct lnode *node = root_head;
	phy_gpio_number = plat->gpio_base + pin;
	if(((phy_gpio_number <= 84) && (phy_gpio_number >=69)) || ((phy_gpio_number <= 191) && (phy_gpio_number >=160))) {
		node = search_gpio_node(root_head, phy_gpio_number);
		if (node == NULL) {
			pr_err("GPIO%d not found\n", phy_gpio_number);
			return -1;
		}
		if ((node->data.id >= 1) && (node->data.id <= 48))
			axera_phygpio_direction(node->data.id, AX_INPUT);
	}
	clrbits_le32(plat->base + GPIO_SWPORT_DDR(plat->bank), 1 << pin);
	return 0;
}

static int ax_gpio_direction_output(struct udevice *dev, unsigned pin, int val)
{
	struct gpio_ax_platdata *plat = dev_get_platdata(dev);
	int phy_gpio_number;
	struct lnode *node = root_head;
	phy_gpio_number = plat->gpio_base + pin;
	if(((phy_gpio_number <= 84) && (phy_gpio_number >=69)) || ((phy_gpio_number <= 191) && (phy_gpio_number >=160))) {
		node = search_gpio_node(root_head, phy_gpio_number);
		if (node == NULL) {
			pr_err("GPIO%d not found\n", phy_gpio_number);
			return -1;
		}
		if ((node->data.id >= 1) && (node->data.id <= 48))
			axera_phygpio_direction(node->data.id, AX_OUTPUT);
	}
	setbits_le32(plat->base + GPIO_SWPORT_DDR(plat->bank), 1 << pin);

	if (val)
		setbits_le32(plat->base + GPIO_SWPORT_DR(plat->bank), 1 << pin);
	else
		clrbits_le32(plat->base + GPIO_SWPORT_DR(plat->bank), 1 << pin);

	return 0;
}

static int ax_gpio_get_value(struct udevice *dev, unsigned pin)
{
	struct gpio_ax_platdata *plat = dev_get_platdata(dev);
	return ! !(readl(plat->base + GPIO_EXT_PORT(plat->bank)) & (1 << pin));
}

static int ax_gpio_set_value(struct udevice *dev, unsigned pin, int val)
{
	struct gpio_ax_platdata *plat = dev_get_platdata(dev);

	if (val)
		setbits_le32(plat->base + GPIO_SWPORT_DR(plat->bank), 1 << pin);
	else
		clrbits_le32(plat->base + GPIO_SWPORT_DR(plat->bank), 1 << pin);

	return 0;
}

static int ax_gpio_get_function(struct udevice *dev, unsigned offset)
{
	struct gpio_ax_platdata *plat = dev_get_platdata(dev);
	u32 gpio;

	gpio = readl(plat->base + GPIO_SWPORT_DDR(plat->bank));

	if (gpio & BIT(offset))
		return GPIOF_OUTPUT;
	else
		return GPIOF_INPUT;
}

static const struct dm_gpio_ops gpio_ax_ops = {
	.direction_input = ax_gpio_direction_input,
	.direction_output = ax_gpio_direction_output,
	.get_value = ax_gpio_get_value,
	.set_value = ax_gpio_set_value,
	.get_function = ax_gpio_get_function,
};

static int gpio_ax_probe(struct udevice *dev)
{
	u32 i;
	struct lnode *head;
	struct gpio_dev_priv *priv = dev_get_uclass_priv(dev);
	struct gpio_ax_platdata *plat = dev->platdata;
	if (!plat) {
		/* Reset on parent device only */
		return 0;
	}
	if(phy_reset_flag == 0) {
		for(i=0;i<8;i++){
			phy_lane0_reg_value[i] = ioread16((void __iomem *)(ulong)(DPHY0_BASE + 0x10000 * i + CORE_DIG_IOCTRL_RW_AFE_LANE0_CTRL_2_1_offset));
			phy_lane1_reg_value[i] = ioread16((void __iomem *)(ulong)(DPHY0_BASE + 0x10000 * i + CORE_DIG_IOCTRL_RW_AFE_LANE1_CTRL_2_1_offset));
			phy_lane2_reg_value[i] = ioread16((void __iomem *)(ulong)(DPHY0_BASE + 0x10000 * i + CORE_DIG_IOCTRL_RW_AFE_LANE2_CTRL_2_1_offset));
		}
		root_head = head = malloc(sizeof(struct lnode));
		memcpy(&head->data, &dphy_real_pin_data[0], sizeof(struct ax_phy_data));
		head->next = NULL;
		for(i=1; i<48; i++){
			head->next = malloc(sizeof(struct lnode));
			memcpy(&head->next->data, &dphy_real_pin_data[i], sizeof(struct ax_phy_data));
			if(i == 47){
				head->next->next = NULL;
				break;
			}
			head = head->next;
		}
	}

	phy_reset_flag = 1;
	spin_lock_init(&phy_lock);
	writel((plat->clk_eb_set.function_value),((plat->clk_rst_base) + (plat->clk_eb_set.off)));
	writel((plat->pclk_eb_set.function_value),((plat->clk_rst_base) + (plat->pclk_eb_set.off)));
	writel((plat->sw_rst_set.function_value),((plat->clk_rst_base) + (plat->sw_rst_set.off)));
	priv->gpio_count = plat->pins;
	priv->bank_name = plat->name;

	return 0;
}

static int gpio_ax_bind(struct udevice *dev)
{
	struct gpio_ax_platdata *plat = dev_get_platdata(dev);
	struct udevice *subdev;
	fdt_addr_t base;
	fdt_addr_t clk_rst_base;
	u32 clk_eb_set[GPIO_REG_EB_RST_SZ];
	u32 pclk_eb_set[GPIO_REG_EB_RST_SZ];
	u32 sw_rst_set[GPIO_REG_EB_RST_SZ];
	int ret, bank = 0;
	ofnode node;

	/* If this is a child device, there is nothing to do here */
	if (plat)
		return 0;

	base = dev_read_addr(dev);
	if (base == FDT_ADDR_T_NONE) {
		debug("Can't get the GPIO register base address\n");
		return -ENXIO;
	}

	clk_rst_base = devfdt_get_addr_index(dev,1);
	if (clk_rst_base == FDT_ADDR_T_NONE) {
		pr_err("EB add RST get reg fail\n");
		return -1;
	}
	memset(clk_eb_set,0,sizeof(clk_eb_set));
	memset(pclk_eb_set,0,sizeof(pclk_eb_set));
	memset(sw_rst_set,0,sizeof(sw_rst_set));

	ret = fdtdec_get_int_array(gd->fdt_blob, dev_of_offset(dev), "clk-eb-set",
					clk_eb_set, (GPIO_REG_EB_RST_SZ * reg_num));
	if (ret) {
		pr_err("%s get clk_eb_set fail\n", __func__);
		return -1;
	}

	ret = fdtdec_get_int_array(gd->fdt_blob, dev_of_offset(dev), "pclk-eb-set",
					pclk_eb_set, (GPIO_REG_EB_RST_SZ * reg_num));
	if (ret) {
		pr_err("%s get pclk_eb_set fail\n", __func__);
		return -1;
	}

	ret = fdtdec_get_int_array(gd->fdt_blob, dev_of_offset(dev), "sw-rst-set",
					sw_rst_set, (GPIO_REG_EB_RST_SZ * reg_num));
	if (ret) {
		pr_err("%s get sw_rst_set fail\n", __func__);
		return -1;
	}

	for (node = dev_read_first_subnode(dev); ofnode_valid(node);
	     node = dev_read_next_subnode(node)) {
		if (!ofnode_read_bool(node, "gpio-controller"))
			continue;

		plat = devm_kcalloc(dev, 1, sizeof(*plat), GFP_KERNEL);
		if (!plat)
			return -ENOMEM;

		plat->base = base;
		plat->gpio_base = ax_gpio_base;
		ax_gpio_base += 32;
		plat->bank = bank;
		plat->clk_rst_base = clk_rst_base;
		plat->clk_eb_set.off = clk_eb_set[0];
		plat->clk_eb_set.function_value = clk_eb_set[1];
		plat->pclk_eb_set.off = pclk_eb_set[0];
		plat->pclk_eb_set.function_value = pclk_eb_set[1];
		plat->sw_rst_set.off = sw_rst_set[0];
		plat->sw_rst_set.function_value = sw_rst_set[1];
		plat->pins = ofnode_read_u32_default(node, "snps,nr-gpios", 0);

		if (ofnode_read_string_index(node, "bank-name", 0, &plat->name)) {
			/*
			 * Fall back to node name. This means accessing pins
			 * via bank name won't work.
			 */
			plat->name = ofnode_get_name(node);
		}

		ret = device_bind_ofnode(dev, dev->driver, plat->name,
					 plat, node, &subdev);
		if (ret)
			return ret;

		bank++;
	}

	return 0;
}

static const struct udevice_id gpio_ax_ids[] = {
	{.compatible = "snps,ax-apb-gpio"},
	{}
};

U_BOOT_DRIVER(gpio_ax) = {
	.name = "gpio-ax",
	.id = UCLASS_GPIO,
	.of_match = gpio_ax_ids,
	.ops = &gpio_ax_ops,
	.bind = gpio_ax_bind,
	.probe = gpio_ax_probe,
	.priv_auto_alloc_size = sizeof(struct gpio_ax_priv),
};