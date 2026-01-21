/*
 * Copyright (c) 2022 AXERA in AX650 project.
 *
 * SPDX-License-Identifier:     GPL-2.0+
 */

#include <common.h>
#include <asm/armv8/mmu.h>
#include <asm/io.h>
#include <malloc.h>
#include <dm.h>
#include <dm/lists.h>
#include <dm/device-internal.h>
#include <dm/uclass-internal.h>
#include <adc.h>
#include <asm/arch/ax650.h>
#include <asm/arch/boot_mode.h>
#include "ax_efuse_api.h"
#define AX_ADC_BASE_ADDR    0x4D00000
#define AX_ADC_MA_EN        0x4D00000
#define AX_ADC_MA_CTRL      0x4D00004
#define AX_ADC_TREF_TRF     0x4D00008	/*verf fill register */
#define AX_ADC_CTRL_EN      0x4D0000c	/*clk enable */
#define AX_ADC_OFFSET       0x4D00010	/*offset fill register */
#define AX_ADC_CH_MASK      0x4D000B4
#define AX_ADC_STAP         0x4D00018
#define AX_ADC_CH_EN        0x4D00020
#define AX_ADC_DATA_CHANNEL0          0x4D00028
#define AX_ADC_DATA_CHANNEL1          0x4D0002c
#define AX_ADC_DATA_CHANNEL2          0x4D00030
#define AX_ADC_CTRL_CHANNEL_MASK      0x4D000B4
#define AX_ADC_CLEAR_CHANNEL_MASK     0x4D000B8

#define AX_ADC_CTRL_CH0_ENABLE      0x1000
#define AX_ADC_CTRL_CH1_ENABLE      0x0800
#define AX_ADC_CTRL_CH2_ENABLE      0x0400

#define COMMON_DUMMY_SW13 0x4508214

extern unsigned char get_chip_type_id(void);

static const char * board_name[AX650_BOARD_MAX] = {
	[AX650A_Demo] = "AX650A_Demo",
	[AX650N_Demo] = "AX650N_Demo",
	[AX650A_EVB] = "AX650A_EVB",
	[AX650N_EVB] = "AX650N_EVB",
	[AX650A_SLT] = "AX650A_SLT",
	[AX650N_SLT] = "AX650N_SLT",
	[AX650N_PCIE] = "AX650N_PCIE",
};

static const char * adp_plate_name[ADP_RX_MAX] = {
	[ADP_RX_DPHY_2X4LANE] = "ADP_RX_DPHY_2X4LANE",
	[ADP_RX_DPHY_8X2LANE] = "ADP_RX_DPHY_8X2LANE",
	[ADP_RX_DPHY_4X4LANE] = "ADP_RX_DPHY_4X4LANE",
	[ADP_RX_DPHY_2X8_LVDS_1X16LANE] = "ADP_RX_DPHY_2X8_LVDS_1X16LANE",
	[ADP_RX_DPHY_2X4LANE_N] = "ADP_RX_DPHY_2X4LANE_N",
	[ADP_RX_CPHY_8X2TRIO] = "ADP_RX_CPHY_8X2TRIO",
	[ADP_RX_CPHY_4X3TRIO] = "ADP_RX_CPHY_4X3TRIO",
	[ADP_RX_DVP_1X10BIT_DPHY_1X2LANE_BT601_656_1120] = "ADP_RX_DVP_1X10BIT_DPHY_1X2LANE_BT601_656_1120",
	[ADP_RX_BT1120_2X10BIT] = "ADP_RX_BT1120_2X10BIT",
	[ADP_RX_BT1120_2X8BIT_DPHY_2X2LANE] = "ADP_RX_BT1120_2X8BIT_DPHY_2X2LANE",
	[ADP_RX_BT601_656_2X10BIT_DPHY_2X2LANE] = "ADP_RX_BT601_656_2X10BIT_DPHY_2X2LANE",
	[ADP_RX_BT601_656_2X8BIT_DPHY_4X2LANE] = "ADP_RX_BT601_656_2X8BIT_DPHY_4X2LANE",
	[ADP_RX_DVP_16bit_HDMI_TO_DPHY_2X4LANE] = "ADP_RX_DVP_16bit_HDMI_TO_DPHY_2X4LANE",
	[ADP_RX_BT_TO_CVBS] = "ADP_RX_BT_TO_CVBS",
};

static const char * chip_type[AX650_CHIP_MAX] = {
	[AX650A_CHIP] = "AX650A_CHIP",
	[AX650N_CHIP] = "AX650N_CHIP",
	[AX650C_CHIP] = "AX650C_CHIP",
	[AX750_CHIP] = "AX750_CHIP",
};

static int calc_id(int data)
{
	int id;
	if(data < 0 || data > 0x220) {
		id = -1;
	}
	if(data >= 0 && data <= 0x20) {
		id = 0;
	} else if(data >= 0x3E0 && data <= 0x3FF) {
		id = 16;
	} else {
		id = (((data - 0x20) / 0x40) + 1);
	}
	return id;
}

int ax_adc_calibrate_config(void)
{
	int ret = 0;
	int tref = 0;
	int data = 0;
	int offset = 0;
	int value = 0;
	int offset_blk = 0;
	int cal_sec;

	ret = ax_efuse_init();
	if (ret != EFUSE_SUCCESS) {
		return -1;
	}
	ret = ax_efuse_read(2, &offset_blk);
	if (ret != 0) {
		return -1;
	}
	offset_blk = (offset_blk >> 16) & 0xf;
	if (offset_blk == 0xa) {
		ret = ax_efuse_read(2, &data);
		if (ret != 0) {
			return -1;
		}
	} else {
		ret = ax_efuse_read(9, &data);
		if (ret != 0) {
			return -1;
		}
	}
	offset = (data >> 4) & 0x3ff;
	writel(offset, COMMON_DUMMY_SW13);
	tref = data & 0x0f;
	if(tref  == 0) {
		tref = 0x8;
	}
	value = readl(AX_ADC_TREF_TRF);
	value &= (~(0x1e));
	value |= tref << 1;
	writel(value,AX_ADC_TREF_TRF);

	value = readl(AX_ADC_OFFSET);
	value &= ~(0x3ff);
	if (offset < 0x200)
		cal_sec = (10 * offset) / 18;
	else
		cal_sec = 0x3ff - ((10 *(0x3ff - offset)) / 18);
	value |= cal_sec;

	writel(value, AX_ADC_OFFSET);

	writel(0x0A, AX_ADC_MA_CTRL);	/*Set the reference voltage 1.8V*/
	writel(0x0E, AX_ADC_MA_EN);
	writel(0x01, AX_ADC_CTRL_EN);

	return 0;
}

int adc_read_boardid(int channel, unsigned int *data)
{
	ax_adc_calibrate_config();
	switch (channel) {
	case 0:
		writel(AX_ADC_CTRL_CH0_ENABLE, AX_ADC_CH_EN);
		break;
	case 1:
		writel(AX_ADC_CTRL_CH1_ENABLE, AX_ADC_CH_EN);
		break;
	case 2:
		writel(AX_ADC_CTRL_CH2_ENABLE, AX_ADC_CH_EN);
		break;
	}

	mdelay(1);

	switch (channel) {
	case 0:
		*data = readl(AX_ADC_DATA_CHANNEL0);
		break;
	case 1:
		*data = readl(AX_ADC_DATA_CHANNEL1);
		break;
	case 2:
		*data = readl(AX_ADC_DATA_CHANNEL2);
		break;
	}

	writel(0, AX_ADC_CH_EN);

	return 0;

}
int get_board_id(void)
{
	unsigned int board_val;
	adc_read_boardid(0, &board_val);

	return calc_id(board_val);
}

const char* __attribute__((weak)) board_id_to_name(int id)
{
	if (AX650C_CHIP == get_chip_type_id()) {
		board_name[AX650N_Demo] = "AX650C_Demo";
		board_name[AX650N_PCIE] = "AX650C_PCIE";
	}

	if (id >= 0 && id < ARRAY_SIZE(board_name)) {
		return board_name[id];
	}

	return "Unknow board ID";
}

static int get_adp_plate_id(void)
{
	unsigned int plate_val;
	adc_read_boardid(1, &plate_val);

	return calc_id(plate_val);
}

/* The high 16bit value stores the board id, and the low 16bit value stores the plate id . */
void store_board_id(void)
{
	unsigned int board_val;
	unsigned int plate_val;
	board_val = get_board_id();
	plate_val = get_adp_plate_id();
	writel((board_val << 16) | plate_val, COMM_SYS_DUMMY_SW1);
}

void store_chip_type(void)
{
	unsigned char chip_type;
	chip_type = get_chip_type_id();

	writel(chip_type, COMM_SYS_DUMMY_SW2);
}

void print_chip_type(void)
{
	printf("Current chip type: %s\n",chip_type[get_chip_type_id()]);
}

void print_board_id(void)
{
	printf("Current board type: %s\n", board_id_to_name(get_board_id()));
}

void print_plate_id(void)
{
	printf("Current adapter plate: %s\n",adp_plate_name[get_adp_plate_id()]);
}
