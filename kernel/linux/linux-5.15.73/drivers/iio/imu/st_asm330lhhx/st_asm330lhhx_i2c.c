// SPDX-License-Identifier: GPL-2.0-only
/*
 * STMicroelectronics st_asm330lhhx i2c driver
 *
 * MEMS Software Solutions Team
 *
 * Copyright 2022 STMicroelectronics Inc.
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/i2c.h>
#include <linux/slab.h>
#include <linux/of.h>
#include <linux/of_gpio.h>

#include "st_asm330lhhx.h"

static const struct regmap_config st_asm330lhhx_i2c_regmap_config = {
	.reg_bits = 8,
	.val_bits = 8,
};

static int st_asm330lhhx_i2c_probe(struct i2c_client *client,
			       const struct i2c_device_id *id)
{
	int hw_id = id->driver_data;
	struct regmap *regmap;
    struct device_node *node;
    int irq_gpio = 0;
    int ret = -1;

    printk("vision_d:%s start \n",__func__);
    node = client->dev.of_node;
    irq_gpio = of_get_named_gpio(node, "int-gpio", 0);
    if(!gpio_is_valid(irq_gpio)) {
        dev_err(&client->dev, "cannot get dts gpio node: int-gpio, ret=%d", irq_gpio);
        return irq_gpio;
    }
    ret = devm_gpio_request(&client->dev, irq_gpio, "asm330-irq");
    if(ret < 0) {
        dev_err(&client->dev, "cannot request gpio: %d, ret=%d", irq_gpio, ret);
        return ret;
    }
    ret = gpio_direction_input(irq_gpio);
    if(ret < 0) {
        dev_err(&client->dev, "cannot set gpio input mode: %d, ret=%d", irq_gpio, ret);
        return ret;
    }
    ret = gpio_to_irq(irq_gpio);
    if(ret < 0) {
        dev_err(&client->dev, "cannot request IRQ for gpio: %d, ret=%d", irq_gpio, ret);
        return ret;
    }
    client->irq = ret;

	regmap = devm_regmap_init_i2c(client,
				      &st_asm330lhhx_i2c_regmap_config);
	if (IS_ERR(regmap)) {
		dev_err(&client->dev,
			"Failed to register i2c regmap %d\n",
			(int)PTR_ERR(regmap));
		return PTR_ERR(regmap);
	}

	return st_asm330lhhx_probe(&client->dev, client->irq,
			       hw_id, regmap);
}

static const struct of_device_id st_asm330lhhx_i2c_of_match[] = {
	{
		.compatible = "st,asm330lhhx",
		.data = (void *)ST_ASM330LHHX_ID,
	},
	{
		.compatible = "st,asm330lhh",
		.data = (void *)ST_ASM330LHH_ID,
	},
	{},
};
MODULE_DEVICE_TABLE(of, st_asm330lhhx_i2c_of_match);

static const struct i2c_device_id st_asm330lhhx_i2c_id_table[] = {
	{ ST_ASM330LHHX_DEV_NAME, ST_ASM330LHHX_ID },
	{ ST_ASM330LHH_DEV_NAME , ST_ASM330LHH_ID },
	{},
};
MODULE_DEVICE_TABLE(i2c, st_asm330lhhx_i2c_id_table);

static struct i2c_driver st_asm330lhhx_driver = {
	.driver = {
		.name = "st_asm330lhhx_i2c",
		.pm = &st_asm330lhhx_pm_ops,
		.of_match_table = st_asm330lhhx_i2c_of_match,
	},
	.probe = st_asm330lhhx_i2c_probe,
	.id_table = st_asm330lhhx_i2c_id_table,
};
module_i2c_driver(st_asm330lhhx_driver);

MODULE_AUTHOR("MEMS Software Solutions Team");
MODULE_DESCRIPTION("STMicroelectronics st_asm330lhhx i2c driver");
MODULE_LICENSE("GPL v2");
