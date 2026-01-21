#include <common.h>
#include <adc.h>
#include <asm/io.h>
#include <linux/delay.h>
#include <dm/device_compat.h>
#include <linux/iopoll.h>
#include <clk.h>
#include <dm.h>
#include <errno.h>
#include <linux/err.h>
#include <sdhci.h>
#include <malloc.h>
#include <asm/arch/boot_mode.h>
#include <asm/arch-axera/dma.h>
#include <ax_efuse_api.h>

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
#define AX_ADC_DATA_CHANNEL3          0x4D00034
#define AX_ADC_DATA_CHANNEL4          0x4D00038
#define AX_ADC_CTRL_CHANNEL_MASK      0x4D000B4
#define AX_ADC_CLEAR_CHANNEL_MASK     0x4D000B8

#define AX_ADC_CTRL_CH0_ENABLE      0x1000
#define AX_ADC_CTRL_CH1_ENABLE      0x0800
#define AX_ADC_CTRL_CH2_ENABLE      0x0400
#define AX_ADC_CTRL_CH3_ENABLE      0x0200
#define AX_ADC_CTRL_CH4_ENABLE      0x0100

struct ax_adc_regs {
	unsigned int data;
	unsigned int ctrl;
};

struct ax_adc_data {
	int num_bits;
	int num_channels;
};

struct ax_adc_priv {
	struct ax_adc_regs *regs;
	int active_channel;
	const struct ax_adc_data *data;
	struct udevice *dev;
	int max_id;
};

int ax_adc_channel_data(struct udevice *dev, int channel, unsigned int *data)
{

	struct ax_adc_priv *priv = dev_get_priv(dev);
	if ((channel != priv->active_channel) || (priv == NULL)) {
		pr_err("Requested channel is not active!");
		return -EINVAL;
	}

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
	case 3:
		*data = readl(AX_ADC_DATA_CHANNEL3);
		break;
	case 4:
		*data = readl(AX_ADC_DATA_CHANNEL4);
		break;
	}

	return 0;

}

int ax_adc_start_channel(struct udevice *dev, int channel)
{
	struct ax_adc_priv *priv = dev_get_priv(dev);
	if ((channel < 0) || (channel > 4) || (priv == NULL)) {
		printf("The channel is invalid\n");
		return 0;
	}
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
	case 3:
		writel(AX_ADC_CTRL_CH3_ENABLE, AX_ADC_CH_EN);
		break;
	case 4:
		writel(AX_ADC_CTRL_CH4_ENABLE, AX_ADC_CH_EN);
		break;
	}
	mdelay(1);
	priv->active_channel = channel;

	return 0;
}

int ax_adc_stop(struct udevice *dev)
{
	struct ax_adc_priv *priv = dev_get_priv(dev);
	if (priv == NULL) {
		printf("Dev Stop Failed\n");
		return 0;
	}
	/* Power down adc */
	writel(0, AX_ADC_CH_EN);
	priv->active_channel = -1;

	return 0;
}

int ax_adc_probe(struct udevice *dev)
{
	struct ax_adc_priv *priv = dev_get_priv(dev);
	if (priv == NULL) {
		printf("Probe Failed\n");
		return 0;
	}
	printf("%s\n", __func__);
	priv->active_channel = -1;

	return 0;
}

int ax_adc_ofdata_to_platdata(struct udevice *dev)
{
	struct adc_uclass_platdata *uc_pdata = dev_get_uclass_platdata(dev);
	struct ax_adc_priv *priv = dev_get_priv(dev);
	struct ax_adc_data *data;

	data = (struct ax_adc_data *)dev_get_driver_data(dev);
	priv->regs = (struct ax_adc_regs *)dev_read_addr(dev);
	if (priv->regs == (struct ax_adc_regs *)FDT_ADDR_T_NONE) {
		pr_err("Dev: %s - can't get address!", dev->name);
		return -ENODATA;
	}
	priv->data = data;
	uc_pdata->data_format = ADC_DATA_FORMAT_BIN;
	uc_pdata->data_mask = AX_ADC_CH_MASK;
	uc_pdata->channel_mask = GENMASK(4, 0);

	return 0;
}

static const struct adc_ops ax_adc_ops = {
	.start_channel = ax_adc_start_channel,
	.channel_data = ax_adc_channel_data,
	.stop = ax_adc_stop,
};

struct ax_adc_data adc_data = {
	.num_bits = 10,
	.num_channels = 5,
};

static const struct udevice_id ax_adc_ids[] = {
	{.compatible = "axera,ax650-adc",},
	{},
};

U_BOOT_DRIVER(ax_adc) = {
	.name = "ax_adc",
	.id = UCLASS_ADC,
	.of_match = ax_adc_ids,
	.ops =&ax_adc_ops,
	.probe = ax_adc_probe,
	.ofdata_to_platdata =ax_adc_ofdata_to_platdata,
	.priv_auto_alloc_size =sizeof(struct ax_adc_priv),
};