/*
 * Copyright (c) 2022 AXERA in AX650 project.
 *
 * SPDX-License-Identifier:     GPL-2.0+
 */
#include "cmn.h"
#include "ax_efuse_api.h"
#include "boot_mode.h"
#include "timer.h"

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
#define CHIP_BOND_BLK	9

/* To get the ID of the chip type. */
static unsigned char hanming_decode_74(unsigned char  a)
{
	unsigned char b=0,c=0;
	b = a;
	c = 0;
	if(b&0x40) {
		c = c^0x07 ;
	}
	if(b&0x20) {
		c = c^0x06 ;
	}
	if(b&0x10) {
		c = c^0x05 ;
	}
	if(b&0x08) {
		c = c^0x03 ;
	}
	if(b&0x04) {
		c = c^0x04 ;
	}
	if(b&0x02) {
		c = c^0x02 ;
	}
	if(b&0x01) {
		c = c^0x01 ;
	}
	switch(c) {
	case 0 :
		b = b >> 3 ;
		break ;
	case 1 :
		b = b >> 3 ;
		break ;
	case 2 :
		b = b >> 3 ;
		break ;
	case 3 :
		b = (b^0x08) >> 3 ;
		break ;
	case 4 :
		b = b >> 3 ;
		break ;
	case 5 :
		b = (b^0x10) >> 3 ;
		break ;
	case 6 :
		b = (b^0x20) >> 3 ;
		break ;
	case 7 :
		b = (b^0x40) >> 3 ;
		break ;
	}
	return (b & 0x0F);
}

static unsigned char get_chip_value_from_efuse(void)
{
	int val;
	int ret;
	ret = ax_efuse_init();
	if(ret != EFUSE_SUCCESS) {
		return -1;
	}
	ret = ax_efuse_read(CHIP_BOND_BLK, &val);
	if(ret != EFUSE_SUCCESS) {
		return -1;
	}
	val = val >> 24;
	if((val & 0xff) == 0) {
		//early_printf("In the early chips, there was no write value\n");
		return 0;
	}
	ax_efuse_deinit();
	return (val & 0xff);
}

unsigned char get_chip_type_id(void)
{
	char value;
	value = get_chip_value_from_efuse();
	return hanming_decode_74(value);
}


int ax_adc_calibrate_config(void)
{
	int ret = 0;
	int tref = 0;
	int data = 0;
	int offset = 0;
	int value = 0;
	int offset_blk = 0;

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
	tref = data & 0x0f;
	if(tref  == 0) {
		tref = 0x8;
	}
	value = readl(AX_ADC_TREF_TRF);
	value |= tref << 1;
	writel(value,AX_ADC_TREF_TRF);
	offset = (data >> 4) & 0x3ff;
	writel(offset, AX_ADC_OFFSET);

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

int calc_board_id(int data)
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
