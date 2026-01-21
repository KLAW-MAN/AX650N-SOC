/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#include "ax-misc.h"
#include <linux/reset.h>

int ax_config_mapping(struct dw_i2s_dev *dev)
{
	int ret;
	void __iomem *pin_sd_base;
	u32 i2s_m0_sel;
	u32 i2s_m1_sel;
	u32 i2s_s0_sel;
	u32 i2s_s1_sel;
	u32 tdm_m0_sel;
	u32 tdm_m1_sel;
	u32 tdm_s0_sel;
	u32 tdm_s1_sel;
	u32 pin_sd_misc_value;
	void __iomem *pin_g3_base;
	u32 iis0_func_sel;
	u32 iis1_func_sel;
	u32 iis2_func_sel;
	u32 iis3_func_sel;
	u32 pin_g3_misc_value;
	struct device_node *np = dev->dev->of_node;

	/* config pin_sd misc register */
	pin_sd_base = devm_ioremap(dev->dev, dev->pin_sd_res->start, resource_size(dev->pin_sd_res));
	if (IS_ERR(pin_sd_base)) {
		pr_err("remap pin_sd_base failed\n");
		return PTR_ERR(pin_sd_base);
	}
	ret = of_property_read_u32(np, "i2s-m0-sel", &i2s_m0_sel);
	if (ret) {
		pr_err("get i2s_m0_sel failed\n");
		return ret;
	}
	ret = of_property_read_u32(np, "i2s-m1-sel", &i2s_m1_sel);
	if (ret) {
		pr_err("get i2s_m1_sel failed\n");
		return ret;
	}
	ret = of_property_read_u32(np, "i2s-s0-sel", &i2s_s0_sel);
	if (ret) {
		pr_err("get i2s_s0_sel failed\n");
		return ret;
	}
	ret = of_property_read_u32(np, "i2s-s1-sel", &i2s_s1_sel);
	if (ret) {
		pr_err("get i2s_s1_sel failed\n");
		return ret;
	}
	ret = of_property_read_u32(np, "tdm-m0-sel", &tdm_m0_sel);
	if (ret) {
		pr_err("get tdm_m0_sel failed\n");
		return ret;
	}
	ret = of_property_read_u32(np, "tdm-m1-sel", &tdm_m1_sel);
	if (ret) {
		pr_err("get tdm_m1_sel failed\n");
		return ret;
	}
	ret = of_property_read_u32(np, "tdm-s0-sel", &tdm_s0_sel);
	if (ret) {
		pr_err("get tdm_s0_sel failed\n");
		return ret;
	}
	ret = of_property_read_u32(np, "tdm-s1-sel", &tdm_s1_sel);
	if (ret) {
		pr_err("get tdm_s1_sel failed\n");
		return ret;
	}

	pin_sd_misc_value = i2s_m0_sel |
			    (i2s_m1_sel << 2) |
			    (i2s_s0_sel << 4) |
			    (i2s_s1_sel << 6) |
			    (tdm_m0_sel << 8) |
			    (tdm_m1_sel << 10) |
			    (tdm_s0_sel << 12) |
			    (tdm_s1_sel << 14);

	writel(pin_sd_misc_value, pin_sd_base + PIN_SD_MISC_REG_OFFSET);
	devm_iounmap(dev->dev, pin_sd_base);
	pr_info("config pin_sd_misc to 0x%x\n", pin_sd_misc_value);

	/* config pin_g3 misc register */
	pin_g3_base = devm_ioremap(dev->dev, dev->pin_g3_res->start, resource_size(dev->pin_g3_res));
	if (IS_ERR(pin_g3_base)) {
		pr_err("remap pin_g3_base failed\n");
		return PTR_ERR(pin_g3_base);
	}
	ret = of_property_read_u32(np, "iis0-func-sel", &iis0_func_sel);
	if (ret) {
		pr_err("get iis0_func_sel failed\n");
		return ret;
	}
	ret = of_property_read_u32(np, "iis1-func-sel", &iis1_func_sel);
	if (ret) {
		pr_err("get iis1_func_sel failed\n");
		return ret;
	}
	ret = of_property_read_u32(np, "iis2-func-sel", &iis2_func_sel);
	if (ret) {
		pr_err("get iis2_func_sel failed\n");
		return ret;
	}
	ret = of_property_read_u32(np, "iis3-func-sel", &iis3_func_sel);
	if (ret) {
		pr_err("get iis3_func_sel failed\n");
		return ret;
	}

	pin_g3_misc_value = iis0_func_sel |
			    (iis1_func_sel << 4) |
			    (iis2_func_sel << 8) |
			    (iis3_func_sel << 12);

	writel(pin_g3_misc_value, pin_g3_base + PIN_G3_MISC_REG_OFFSET);
	devm_iounmap(dev->dev, pin_g3_base);
	pr_info("config pin_g3_misc to 0x%x\n", pin_g3_misc_value);

	return 0;
}

int ax_audio_reset_hw(struct platform_device *pdev, const char *rst_name)
{
	struct reset_control *rst = NULL;

	if (NULL == pdev || NULL == rst_name)
		return -1;

	rst = devm_reset_control_get_optional(&pdev->dev, rst_name);
	if (IS_ERR(rst))
		return -1;

	//reset_control_assert(rst);
	reset_control_deassert(rst);

	return 0;
}

