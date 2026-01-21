/*
 *Driver for Epson's RTC module at8563
 */

#include <linux/bcd.h>
#include <linux/bitops.h>
#include <linux/i2c.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/rtc.h>

/* Register definitions */
#define at8563_CTRL_STA1_REG_SEC	0x00
#define at8563_CTRL_STA2_REG_SEC	0x01
#define at8563_REG_SEC	        0x02
#define at8563_REG_MIN		0x03
#define at8563_REG_HOUR		0x04
#define at8563_REG_DAY		0x05
#define at8563_REG_WEEKDAY  		0x06
#define at8563_REG_MONTH	        0x07
#define at8563_REG_YEAR		0x08
#define at8563_REG_ALWMIN	        0x09
#define at8563_REG_ALWHOUR	        0x0a
#define at8563_REG_ALWDTAE	        0x0b
#define at8563_REG_ALWWEEKDAY	        0x0c
#define at8563_CLKOUT_FREQ	        0x0d
#define at8563_TIMER_CTRL	        0x0e


static const struct i2c_device_id at8563_id[] = {
	{ "at8563", 0 },
	{ }
};

MODULE_DEVICE_TABLE(i2c, at8563_id);

static const struct of_device_id at8563_of_match[] = {
	{ .compatible = "analogtek,at8563" },
	{ }
};

MODULE_DEVICE_TABLE(of, at8563_of_match);

struct at8563_data {
	struct i2c_client *client;
	struct rtc_device *rtc;
	u8 ctrl1;
};

#if 0
static s32 at8563_read_reg(const struct i2c_client *client, u8 number)
{
	return i2c_smbus_read_byte_data(client, number);
}

static s32 at8563_write_reg(const struct i2c_client *client, u8 number,
			    u8 value)
{
	return i2c_smbus_write_byte_data(client, number, value);
}
#endif

static int at8563_read_regs(const struct i2c_client *client,
			    u8 number, u8 length, u8 *values)
{
	int ret = i2c_smbus_read_i2c_block_data(client, number, length,
						values);
	if (ret != length)
		return ret < 0 ? ret : -EIO;

	return 0;
}


static s32 at8563_write_regs(const struct i2c_client *client,
			     u8 number, u8 length, const u8 *values)
{
	return i2c_smbus_write_i2c_block_data(client, number,
					      length, values);
}


#if 0
static irqreturn_t at8563_handle_irq(int irq, void *dev_id)
{

	return IRQ_HANDLED;
}
#endif

static int at8563_get_time(struct device *dev, struct rtc_time *dt)
{
	struct at8563_data *at8563 = dev_get_drvdata(dev);
	u8 date[7];
	int err;

        err = at8563_read_regs(at8563->client, at8563_REG_SEC, 7, date);
        if (err)
		return err;

	dev_dbg(dev,"%s: read 0x%02x 0x%02x "
		"0x%02x 0x%02x 0x%02x 0x%02x 0x%02x\n", __func__,
		date[0], date[1], date[2], date[3], date[4],
		date[5], date[6]);

	dt->tm_sec = bcd2bin(date[at8563_REG_SEC-at8563_REG_SEC] & 0x7f);
	dt->tm_min = bcd2bin(date[at8563_REG_MIN-at8563_REG_SEC] & 0x7f);
	dt->tm_hour = bcd2bin(date[at8563_REG_HOUR-at8563_REG_SEC] & 0x3f);
	dt->tm_mday = bcd2bin(date[at8563_REG_DAY-at8563_REG_SEC] & 0x3f);
	dt->tm_wday = bcd2bin(date[at8563_REG_WEEKDAY-at8563_REG_SEC] & 0x07);
	dt->tm_mon = bcd2bin(date[at8563_REG_MONTH-at8563_REG_SEC] & 0x1f)-1;
	dt->tm_year = bcd2bin(date[at8563_REG_YEAR-at8563_REG_SEC]) + 100;

	dev_dbg(dev,"%s: date %ds %dm %dh %dmd %dwd %dm %dy\n", __func__,
		dt->tm_sec, dt->tm_min, dt->tm_hour,
		dt->tm_mday, dt->tm_wday, dt->tm_mon, dt->tm_year);

	return 0;
}


static int at8563_set_time(struct device *dev, struct rtc_time *dt)
{
	struct at8563_data *at8563 = dev_get_drvdata(dev);
	u8 date[7];
	int ret;

	if ((dt->tm_year < 100) || (dt->tm_year > 199))
		return -EINVAL;

        date[at8563_REG_SEC-at8563_REG_SEC] = bin2bcd(dt->tm_sec);
	date[at8563_REG_MIN-at8563_REG_SEC] = bin2bcd(dt->tm_min);
	date[at8563_REG_HOUR-at8563_REG_SEC] = bin2bcd(dt->tm_hour);
        date[at8563_REG_WEEKDAY-at8563_REG_SEC] = bin2bcd(dt->tm_wday);
	date[at8563_REG_DAY-at8563_REG_SEC] = bin2bcd(dt->tm_mday);
	date[at8563_REG_MONTH-at8563_REG_SEC] = bin2bcd(dt->tm_mon+1) ;
	date[at8563_REG_YEAR-at8563_REG_SEC] = bin2bcd(dt->tm_year - 100);

	dev_dbg(dev,"%s: write 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x\n",
		__func__,
		date[0], date[1], date[2], date[3], date[4], date[5], date[6]);

	ret = at8563_write_regs(at8563->client, at8563_REG_SEC, 7, date);
	if (ret < 0)
		return ret;

	return 0;
}

#if 0
static int init_at8563(struct i2c_client *client)
{
        struct at8563_data *at8563 = i2c_get_clientdata(client);
	int err,val;

	err = at8563_read_reg(at8563->client, at8563_REG_CTRL1, 2, ctrl);
	if (err)
        return  err;


	err = at8563_write_reg(client, at8563_REG_SQW,0x00);
	if (err)
        return  err;

	val = at8563_read_reg(at8563->client, at8563_REG_SQW);


	return 0;
}


/* Alarm support */
static int at8563_read_alarm(struct device *dev, struct rtc_wkalrm *t)
{
	return 0;
}

static int at8563_set_alarm(struct device *dev, struct rtc_wkalrm *t)
{
	return 0;
}

static int at8563_alarm_irq_enable(struct device *dev, unsigned int enabled)
{
	return 0;
}
#endif

static const struct rtc_class_ops at8563_rtc_ops = {
	.read_time = at8563_get_time,
	.set_time = at8563_set_time,
#if 0
	.read_alarm = at8563_read_alarm,
	.set_alarm = at8563_set_alarm,
	.alarm_irq_enable = at8563_alarm_irq_enable,
#endif
        };


static int at8563_probe(struct i2c_client *client,
			const struct i2c_device_id *id)
{
	struct i2c_adapter *adapter = to_i2c_adapter(client->dev.parent);
	struct at8563_data *at8563;

	if (!i2c_check_functionality(adapter, I2C_FUNC_SMBUS_BYTE_DATA
				     | I2C_FUNC_SMBUS_I2C_BLOCK)) {
		dev_err(&adapter->dev,
			"doesn't support required functionality\n");
		return -EIO;
	}

	at8563 = devm_kzalloc(&client->dev, sizeof(*at8563), GFP_KERNEL);
	if (!at8563)
		return -ENOMEM;

	at8563->client = client;
	i2c_set_clientdata(client, at8563);


	at8563->rtc = devm_rtc_device_register(&client->dev, client->name,
					  &at8563_rtc_ops, THIS_MODULE);
	if (IS_ERR(at8563->rtc)) {
		dev_err(&client->dev, "unable to register the class device\n");
		return PTR_ERR(at8563->rtc);
	}

#if 0
	if (client->irq > 0) {
		dev_info(&client->dev, "IRQ %d supplied\n", client->irq);
		err = devm_request_threaded_irq(&client->dev, client->irq, NULL,
						at8563_handle_irq,
						IRQF_ONESHOT,
						"at8563", client);
		if (err) {
			dev_err(&client->dev, "unable to request IRQ, alarms disabled\n");
 			client->irq = 0;
		}
	}
#endif


	return 0;
}

static int at8563_remove(struct i2c_client *client)
{
	return 0;
}

static struct i2c_driver at8563_driver = {
	.driver = {
		.name = "rtc-at8563",
		.of_match_table = of_match_ptr(at8563_of_match),
	},
	.probe		= at8563_probe,
	.remove		= at8563_remove,
	.id_table	= at8563_id,
};

module_i2c_driver(at8563_driver);

MODULE_DESCRIPTION("at8563  RTC driver");
MODULE_LICENSE("GPL");
