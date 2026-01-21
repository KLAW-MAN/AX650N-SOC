#include <linux/cpufreq.h>
#include <linux/delay.h>
#include <linux/interrupt.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/io.h>
#include <linux/of_device.h>
#include <linux/reset.h>
#include "thermal_core.h"
#define THERMAL_MANUAL_EN           (0x00)
#define THERMAL_MANUAL_CTRL         (0x04)
#define THERMAL_CTRL     (0x08)
#define THERMAL_CLK      (0x0c)
#define THERMAL_OFFSET       (0x10)
#define THERMAL_MSR_ONE        (0x20)
#define THERMAL_MSR_TEMP_INT (0x48)
#define THERMAL_MSR_TEMP_EXT0 (0x4c)
#define THERMAL_MSR_TEMP_EXT1 (0x50)
#define THERMAL_MSR_TEMP_EXT2 (0x54)
#define THERMAL_MSR_TEMP_EXT3 (0x58)
#define THERMAL_CHANNEL 		(0x5C)
#define THERMAL_MNR_EN 			(0x60)
#define THERMAL_TEMP_HIGH		(0x68)
#define THERMAL_TEMP_MEIAN		(0x6c)
#define THERMAL_TEMP_LOW		(0x70)
#define THERMAL_INT_MASK 		(0xB4)
#define THERMAL_INT_CLR 		(0xB8)
#define THERMAL_INT_RAW 		(0xBC)
#define THERMAL_INT_STA 		(0xC0)
#define ADC_DATA_CHAN0			(0x28) /* chan0 */
#define ADC_DATA_CHAN1			(0x2C) /* chan1 */
#define ADC_DATA_CHAN2			(0x30) /* chan2 */
#define ADC_DATA_CHAN3			(0x34) /* chan3 */
#define ADC_DATA_CHAN4			(0x38) /* chan4 */
#define AX650_DEFAULT_SENSOR 0

#define COMMON_DUMMY_SW13 0x4508214

static u32 calib_tvef, calib_offset_1_8v, calib_offset_1_0v;
struct axera_thermal_sensor {
	struct thermal_zone_device *tzd;
	u32 id;
	u32 thres[3];
};

struct adc_value {
	u32 chan0_val;
	u32 chan1_val;
	u32 chan2_val;
	u32 chan3_val;
	u32 chan4_val;
};

static struct axera_thermal_data *axera_thermal_data_g;

struct axera_thermal_data {
	int (*get_temp) (struct axera_thermal_data * data);
	int (*enable_sensor) (struct axera_thermal_data * data);
	int (*enable_adc_sensor)(struct axera_thermal_data *data);
	int (*disable_sensor) (struct axera_thermal_data * data);
	struct platform_device *pdev;
	struct mutex		lock;
	struct clk *clk;
	struct clk *pclk;
	struct reset_control *prst;
	struct reset_control *rst;
	struct axera_thermal_sensor sensor;
	void __iomem *regs;
	int irq;
	int sensor_en;
};

static void thm_reset_set(struct axera_thermal_data *data)
{

	reset_control_assert(data->prst);
	reset_control_assert(data->rst);
	usleep_range(50, 60);
	reset_control_deassert(data->prst);
	reset_control_deassert(data->rst);

	return;
}
int ax650_adc_switch_thm(struct axera_thermal_data *data)
{
	int ret;
	thm_reset_set(data);
	ret = data->enable_sensor(data);
	if (ret) {
		printk("Failed to setup the thm sensor: %d\n", ret);
		return ret;
	}
	return 0;
}

EXPORT_SYMBOL(ax650_adc_switch_thm);

static int ax650_adc_enable_sensor(struct axera_thermal_data *data)
{
	void __iomem *regs;
	regs = data->regs;
	if (IS_ERR(regs))
		return PTR_ERR(regs);
	writel(calib_tvef, regs + THERMAL_CTRL);
	writel(calib_offset_1_8v, regs + THERMAL_OFFSET);
	writel(0xa, regs + THERMAL_MANUAL_CTRL);
	writel(0xe, regs + THERMAL_MANUAL_EN);
	udelay(2);
	writel(0x1, regs + THERMAL_CLK);
	usleep_range(1000, 2000);
	return 0;
}

static int ax650_thm_switch_adc(struct axera_thermal_data *data)
{
	int ret;
	void __iomem *regs;
	regs = data->regs;
	if (IS_ERR(regs))
		return PTR_ERR(regs);
	/* close channel_en and set the state machine to idle. */
	writel(0x0, regs + THERMAL_CHANNEL);
	usleep_range(1000, 2000);
	thm_reset_set(data);
	ret = data->enable_adc_sensor(data);
	if (ret) {
		printk("Failed to setup the adc sensor: %d\n", ret);
		return ret;
	}
	return 0;
}

static int set_reg_to_get_value(u32 enable, struct adc_value *adc_value)
{
	void __iomem *regs;
	int timeout = 500; //5ms timeout
	int chan_num = 5;
	int i;
	u32 adc_code;
	u32 *ptr = (u32 *) adc_value;

	regs = axera_thermal_data_g->regs;
	memset(adc_value, 0, sizeof(struct adc_value));
	for (i = 0; i < chan_num; i++) {
		if ((enable & BIT(i)) == 0)
			continue;
		adc_code = 0;

		/* clr int */
		writel(0xfffff, regs + THERMAL_INT_CLR);
		writel(BIT(12 - i), regs + THERMAL_MSR_ONE);
		/* Judge single measurement completed */
		timeout = 500;
		while (1) {
			if (readl(regs + THERMAL_INT_RAW) & BIT(11)) {
				adc_code = readl(regs + ADC_DATA_CHAN0 + (i * 4));
				break;
			}
			if (timeout-- <= 0)
				return -1;

			udelay(10);
		}
		ptr[i] = adc_code;
	}
	return 0;
}

/* set enable to enable chanx, bit0: chan0, bit1: chan1, bit2: chan2, bit3: chan3, bit4: chan4 */
int read_adc_value(u32 enable, struct adc_value *adc_value)
{
	int ret;
	if (IS_ERR(axera_thermal_data_g))
		return PTR_ERR(axera_thermal_data_g);
	mutex_lock(&axera_thermal_data_g->lock);
	/* switch to adc mode */
	ret = ax650_thm_switch_adc(axera_thermal_data_g);
	if (ret) {
		printk("read_adc_value switch adc error\n");
		mutex_unlock(&axera_thermal_data_g->lock);
		return -1;
	}
	ret = set_reg_to_get_value(enable, adc_value);
	if (ret) {
		printk("read_adc_value get adc value error\n");
		mutex_unlock(&axera_thermal_data_g->lock);
		return -1;
	}
	/* switch to thm mode */
	ret = ax650_adc_switch_thm(axera_thermal_data_g);
	if (ret) {
		printk("read_adc_value switch thm error\n");
		mutex_unlock(&axera_thermal_data_g->lock);
		return -1;
	}
	mutex_unlock(&axera_thermal_data_g->lock);
	return 0;
}
EXPORT_SYMBOL(read_adc_value);

static int step2temp(int step)
{
	int temp;

	if (step < 450) {
		temp = 60000 + (450 - step) * 100000 / 340;
	} else {
		temp = -40000 + (778 - step) * 100000 / 326;
	}
	return temp;
}

static int temp2step(int temp)
{
	int step;

	if (temp < 65000) {
		step = temp - (-40000);
		step = step * 3260 / 1000 / 1000;
		step = 778 - step;
	} else {
		step = temp - 65000;
		step = step * 3400 / 1000 / 1000;
		step = 433 - step;
	}
	return step;
}

static int s_temp;
static int ax650_thermal_get_temp(struct axera_thermal_data *data)
{
	int val;
	int temp;

	void __iomem *regs;
	regs = data->regs;
	if (data->sensor_en) {
		val = (readl(regs + THERMAL_MSR_TEMP_EXT3)) & 0x3ff;
		temp = step2temp(val);
		s_temp = temp;
	} else {
		temp = s_temp;
	}
	return temp;
}

static int axera_thermal_get_temp(void *__data, int *temp)
{
	struct axera_thermal_data *data = __data;
	mutex_lock(&axera_thermal_data_g->lock);
	/*struct axera_thermal_sensor *sensor = &data->sensor; */
	*temp = data->get_temp(data);
	mutex_unlock(&axera_thermal_data_g->lock);
	return 0;
}

static int ax650_thermal_enable_sensor(struct axera_thermal_data *data)
{
	struct axera_thermal_sensor *sensor = &data->sensor;
	int ret;
	void __iomem *regs;
	int val;

	/* enable clock for tsensor */
	ret = clk_prepare_enable(data->clk);
	if (ret)
		return ret;
	ret = clk_prepare_enable(data->pclk);
	if (ret)
		return ret;
	regs = data->regs;
	writel(0x0e, regs + THERMAL_MANUAL_CTRL);
	writel(0x0e, regs + THERMAL_MANUAL_EN);
	writel(0x700, regs + THERMAL_INT_MASK);
	writel(0x700, regs + THERMAL_INT_CLR);
	writel(0x1, regs + THERMAL_CHANNEL);
	val = temp2step(sensor->thres[0]);
	writel(val, regs + THERMAL_TEMP_LOW);
	val = temp2step(sensor->thres[1]);
	writel(val, regs + THERMAL_TEMP_MEIAN);
	val = temp2step(sensor->thres[2]);
	writel(val, regs + THERMAL_TEMP_HIGH);
	/* write calibration parameters to restore calibration values in wake up. */
	writel(calib_tvef, data->regs + THERMAL_CTRL);
	writel(calib_offset_1_0v, data->regs + THERMAL_OFFSET);
	writel(0x1, regs + THERMAL_MNR_EN);
	udelay(2);
	writel(0x1, regs + THERMAL_CLK);
	usleep_range(1000, 2000);
	data->sensor_en = 1;
	return 0;
}

static int ax650_thermal_disable_sensor(struct axera_thermal_data *data)
{
	void __iomem *regs;
	regs = data->regs;
	mutex_lock(&axera_thermal_data_g->lock);
	writel(0x0, regs + THERMAL_CHANNEL);
	writel(0x700, regs + THERMAL_INT_MASK);
	writel(0x700, regs + THERMAL_INT_CLR);
	writel(0x0, regs + THERMAL_MNR_EN);
	data->sensor_en = 0;
	mutex_unlock(&axera_thermal_data_g->lock);
	clk_disable_unprepare(data->clk);
	clk_disable_unprepare(data->pclk);


	return 0;
}

static int ax650_thermal_probe(struct axera_thermal_data *data)
{
	struct platform_device *pdev = data->pdev;
	struct device *dev = &pdev->dev;
	struct resource *res;
	int ret;
	data->get_temp = ax650_thermal_get_temp;
	data->enable_sensor = ax650_thermal_enable_sensor;
	data->enable_adc_sensor = ax650_adc_enable_sensor;
	data->disable_sensor = ax650_thermal_disable_sensor;

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	data->regs = devm_ioremap_resource(dev, res);
	if (IS_ERR(data->regs)) {
		dev_err(dev, "failed to get io address\n");
		return PTR_ERR(data->regs);
	}

	data->clk = devm_clk_get(dev, "thermal_clk");
	if (IS_ERR(data->clk)) {
		ret = PTR_ERR(data->clk);
		if (ret != -EPROBE_DEFER)
			dev_err(dev, "failed to get thermal clk: %d\n", ret);
		return ret;
	}
	data->pclk = devm_clk_get(dev, "thermal_pclk");
	if (IS_ERR(data->pclk)) {
		ret = PTR_ERR(data->pclk);
		if (ret != -EPROBE_DEFER)
			dev_err(dev, "failed to get thermal pclk: %d\n", ret);
		return ret;
	}

	data->irq = platform_get_irq(pdev, 0);
	if (data->irq < 0) {
		dev_err(dev, "ax650_thermal_probe error\n");
		return data->irq;
	}
	data->prst = devm_reset_control_get(dev, "thm_prst");
	if (IS_ERR(data->prst))
		return dev_err_probe(dev, PTR_ERR(data->prst),
				     "Cannot get reset descriptor\n");
	data->rst = devm_reset_control_get(dev, "thm_rst");
	if (IS_ERR(data->rst))
		return dev_err_probe(dev, PTR_ERR(data->rst),
				     "Cannot get reset descriptor\n");
	axera_thermal_data_g = data;
	mutex_init(&data->lock);
	data->sensor.id = AX650_DEFAULT_SENSOR;

	return 0;
}

static const struct thermal_zone_of_device_ops axera_of_thermal_ops = {
	.get_temp = axera_thermal_get_temp,
};

static int axera_thermal_register_sensor(struct platform_device *pdev,
					 struct axera_thermal_data *data,
					 struct axera_thermal_sensor *sensor)
{
	int ret, i;
	const struct thermal_trip *trip;
	sensor->tzd = devm_thermal_zone_of_sensor_register(&pdev->dev,
							   sensor->id, data,
							   &axera_of_thermal_ops);
	if (IS_ERR(sensor->tzd)) {
		ret = PTR_ERR(sensor->tzd);
		sensor->tzd = NULL;
		dev_err(&pdev->dev, "failed to register sensor id %d: %d\n",
			sensor->id, ret);
		return ret;
	}
	trip = of_thermal_get_trip_points(sensor->tzd);

	for (i = 0; i < of_thermal_get_ntrips(sensor->tzd); i++) {
		sensor->thres[i] = trip[i].temperature;
	}

	return 0;
}

static void axera_thermal_toggle_sensor(struct axera_thermal_sensor *sensor,
					bool on)
{
	struct thermal_zone_device *tzd = sensor->tzd;

	if (on == true)
		thermal_zone_device_enable(tzd);
	else
		thermal_zone_device_disable(tzd);
}

static irqreturn_t axera_thermal_alarm_irq(int irq, void *dev)
{
	struct axera_thermal_data *data = dev;
	int temp;

	void __iomem *regs = data->regs;
	temp = data->get_temp(data);
	pr_debug("%s enter,temp:%d\n", __func__, temp);
	writel(0x700, regs + THERMAL_INT_CLR);

	return IRQ_HANDLED;
}

static int axera_thermal_probe(struct platform_device *pdev)
{
	struct axera_thermal_data *data;
	void __iomem *regs;
	int (*platform_probe) (struct axera_thermal_data *);
	struct device *dev = &pdev->dev;
	int ret;

	data = devm_kzalloc(dev, sizeof(*data), GFP_KERNEL);
	if (!data)
		return -ENOMEM;
	data->pdev = pdev;
	platform_set_drvdata(pdev, data);

	platform_probe = of_device_get_match_data(dev);
	if (!platform_probe) {
		dev_err(dev, "failed to get probe func\n");
		return -EINVAL;
	}
	ret = platform_probe(data);
	if (ret)
		return ret;
	ret = axera_thermal_register_sensor(pdev, data, &data->sensor);
	if (ret) {
		dev_err(dev, "failed to register thermal sensor: %d\n", ret);
		return ret;
	}

	regs = ioremap(COMMON_DUMMY_SW13, 0x4);
	calib_offset_1_0v = readl(regs);
	iounmap(regs);

	/* Save calibration parameters to restore calibration values. */
	calib_tvef = readl(data->regs + THERMAL_CTRL);
	calib_offset_1_8v = readl(data->regs + THERMAL_OFFSET);
	ret = data->enable_sensor(data);
	if (ret) {
		dev_err(dev, "Failed to setup the sensor: %d\n", ret);
		return ret;
	}
	if (data->irq) {
		ret = devm_request_irq(dev, data->irq, axera_thermal_alarm_irq,
				IRQF_ONESHOT, "axera_thermal", data);
		if (ret < 0) {
			dev_err(dev, "failed to request alarm irq: %d\n", ret);
			return ret;
		}
	}
	axera_thermal_toggle_sensor(&data->sensor, true);
	return 0;
}

static int axera_thermal_remove(struct platform_device *pdev)
{
	struct axera_thermal_data *data = platform_get_drvdata(pdev);
	struct axera_thermal_sensor *sensor = &data->sensor;
	axera_thermal_toggle_sensor(sensor, false);
	data->disable_sensor(data);

	return 0;
}

#ifdef CONFIG_PM_SLEEP
static int axera_thermal_suspend(struct device *dev)
{
	struct axera_thermal_data *data = dev_get_drvdata(dev);
	data->disable_sensor(data);
	return 0;
}

static int axera_thermal_resume(struct device *dev)
{
	struct axera_thermal_data *data = dev_get_drvdata(dev);
	data->enable_sensor(data);
	return 0;
}
#endif

static SIMPLE_DEV_PM_OPS(axera_thermal_pm_ops,
			 axera_thermal_suspend, axera_thermal_resume);

static const struct of_device_id of_axera_thermal_match[] = {
	{
		.compatible = "axera,ax650-tsensor",
		.data = ax650_thermal_probe
	},
	{ /* end */ }
};

static struct platform_driver axera_thermal_driver = {
	.driver = {
		.name = "ax_thermal",
		.pm = &axera_thermal_pm_ops,
		.of_match_table = of_axera_thermal_match,
	},
	.probe = axera_thermal_probe,
	.remove = axera_thermal_remove,
};

module_platform_driver(axera_thermal_driver);

MODULE_AUTHOR("Axera");
MODULE_DESCRIPTION("Axera thermal driver");
MODULE_LICENSE("GPL v2");
