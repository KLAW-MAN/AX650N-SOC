/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#include <linux/module.h>
#include <linux/of_platform.h>
#include <linux/io.h>
#include <linux/printk.h>
#include <linux/ioport.h>
#include <linux/slab.h>

#define AX_FBCDC_DRIVER_NAME	"ax_fbcdc"
#define MAX_FBCDC_REGS_NUM	100
typedef struct __AX_FBCDC_CFG {
	u32 offset;
	u32 val;
} AX_FBCDC_CFG;

static int ax_fbcdc_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct device_node *node = NULL;
	u32 regs_num = 0;
	u32 regs_param = 0;
	u32 i = 0;
	AX_FBCDC_CFG *pFbcdcCfg;
	void __iomem *regs = NULL;
	struct resource *mem = NULL;
	struct property *prop = NULL;
	int len = 0;

	mem = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if(!mem){
		pr_err(" %s: platform_get_resource failed\n", pdev->name);
		return -EINVAL;
	}

	regs = devm_ioremap_resource(dev, mem);
	if (IS_ERR(regs)){
		pr_err(" %s devm_ioremap_resource failed\n", pdev->name);
		return PTR_ERR(regs);
	}

	prop = of_find_property(pdev->dev.of_node, "regs-param", &len);
	if(!prop){
		pr_err(" %s of_find_property failed\n", pdev->name);
		return -EINVAL;
	}

	regs_param = of_read_number((__be32 *)prop->value, len / sizeof(__be32));
	node = of_find_node_by_phandle(regs_param);
	if(!node){
		pr_err(" %s of_find_node_by_phandle failed\n", pdev->name);
		return -EINVAL;
	}

	of_property_read_u32_array(node, "regs-num", &regs_num, 0x1);
	pr_info(" %s regs_num %d \n", pdev->name, regs_num);
	if(regs_num > MAX_FBCDC_REGS_NUM){
		pr_err(" %s regs_num of range\n", pdev->name);
		return -EINVAL;
	}

	pFbcdcCfg = (AX_FBCDC_CFG *)kzalloc(sizeof(AX_FBCDC_CFG) * regs_num, 0);
	if(NULL == pFbcdcCfg){
		pr_err(" %s kzalloc failed\n", pdev->name);
		return -EFAULT;
	}

	of_property_read_u32_array(node, "regs-val", (u32 *)pFbcdcCfg, 2 * regs_num);
	for (i = 0; i < regs_num; i++) {
		writel(pFbcdcCfg[i].val, (void __iomem *)regs + pFbcdcCfg[i].offset);
	}

	pr_info("%s module success!\n", pdev->name);

	kfree(pFbcdcCfg);
	return 0;
}

static const struct of_device_id fbcdc_of_match[] = {
	{.compatible = "axera, fbcdc-dev"},
	{},
};

static struct platform_driver fbcdc_driver = {
	.probe = ax_fbcdc_probe,
	.driver = {
		   .name = AX_FBCDC_DRIVER_NAME,
		   .owner = THIS_MODULE,
		   .of_match_table = of_match_ptr(fbcdc_of_match),
		   },
};

static int __init ax_fbcdc_drv_init(void)
{
	if (platform_driver_register(&fbcdc_driver)) {
		pr_err("failed to register ax_fbcdc driver!\n");
		return -ENODEV;
	}

	return 0;
}

subsys_initcall(ax_fbcdc_drv_init);
