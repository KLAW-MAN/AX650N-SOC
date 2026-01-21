#include <linux/clk.h>
#include <linux/err.h>
#include <linux/io.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/platform_device.h>
#include <linux/devfreq.h>

typedef struct axera_dfs_resource {
	struct clk *clk;
	u32 *freq_arr;
	u32 freq_arr_num;
	void *reg_addr;
	u32 offset;
	u32 mask;
} dfs_resource_s;

static struct axera_dfs_resource *axera_get_resource(struct device *dev)
{
	struct platform_device *pdev;
	struct axera_dfs_resource *resource;

	pdev = to_platform_device(dev);
	resource = platform_get_drvdata(pdev);
	if (IS_ERR(resource)) {
		return NULL;
	}

	return resource;
}

static int axera_freq_target(struct device *dev, unsigned long *freq, u32 flags)
{
	struct dev_pm_opp *opp;
	unsigned long rate;
	struct axera_dfs_resource *resource;
	int index = 0, num = 0;
	u32 value = 0;

	resource = axera_get_resource(dev);

	if (!resource) {
		return -ENODATA;
	}

	opp = devfreq_recommended_opp(dev, freq, flags);
	if (IS_ERR(opp)) {
		return PTR_ERR(opp);
	}

	rate = dev_pm_opp_get_freq(opp);

	if (resource->clk) {
		clk_set_rate(resource->clk, rate);
	} else if (resource->reg_addr) {
		num = resource->freq_arr_num;
		for (index = 0; index < num; index++) {
			if (resource->freq_arr[index] == rate) {
				break;
			}
		}
		if (index == num) {
			pr_err("frequence not in freq_arr\n");
			return -ENODATA;
		}
		value = readl(resource->reg_addr);
		value &= ~(resource->mask << resource->offset);
		value |= (index << resource->offset);
		writel(value, resource->reg_addr);
	}

	dev_pm_opp_put(opp);

	return 0;
}

static int axera_get_cur_freq(struct device *dev, unsigned long *freq)
{
	unsigned long rate;
	struct axera_dfs_resource *resource;

	resource = axera_get_resource(dev);

	if (!resource) {
		return -ENODATA;
	}

	if (resource->clk) {
		rate = clk_get_rate(resource->clk);
	} else if (resource->reg_addr) {
		rate = readl(resource->reg_addr);
		rate >>= resource->offset;
		rate &= resource->mask;
		rate = resource->freq_arr[rate];
	}

	*freq = rate;
	return 0;
}

static int axera_dfs_probe(struct platform_device *pdev)
{
	int ret = 0;
	u32 offset = 0, mask = 0, index = 0, num = 0;
	struct clk *clk = NULL;
	struct devfreq *fp;
	struct resource *res;
	void *reg_addr;
	u32 *freq_arr;
	struct axera_dfs_resource *resource;
	struct devfreq_dev_profile *axera_devfreq_profile;

	axera_devfreq_profile = devm_kzalloc(&pdev->dev,
		sizeof(struct devfreq_dev_profile), GFP_KERNEL);

	if (IS_ERR(axera_devfreq_profile)) {
		pr_err("get devfreq profile failed.\n");
		return PTR_ERR(axera_devfreq_profile);
	}

	axera_devfreq_profile->target = axera_freq_target;
	axera_devfreq_profile->get_cur_freq = axera_get_cur_freq;

	resource = devm_kzalloc(&pdev->dev,
		sizeof(struct axera_dfs_resource), GFP_KERNEL);

	if (IS_ERR(resource)) {
		pr_err("get resource failed.\n");
		return PTR_ERR(resource);
	}

	clk = devm_clk_get(&pdev->dev, "bus_clk");
	if (IS_ERR(clk)) {
		/* some bus don't be supported in clk system now, use ioremap instead */
		res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
		if (IS_ERR(res)) {
			return PTR_ERR(res);
		}

		reg_addr = devm_ioremap(&pdev->dev, res->start, resource_size(res));
		if (IS_ERR(reg_addr)) {
			pr_err("get reg_addr failed.\n");
			return PTR_ERR(reg_addr);
		}

		resource->reg_addr = reg_addr;
		ret = of_property_read_u32(pdev->dev.of_node, "offset", &offset);
		if (ret) {
			pr_err("get register offset failed.\n");
			return ret;
		}

		resource->offset = offset;

		ret = of_property_read_u32(pdev->dev.of_node, "mask", &mask);

		if (ret) {
			pr_err("get register mask failed.\n");
			return ret;
		}
		resource->mask = mask;
	}

	if (!IS_ERR(clk))
		resource->clk = clk;

	platform_set_drvdata(pdev, resource);

	ret = dev_pm_opp_of_add_table(&pdev->dev);
	if (ret) {
		pr_err("cannot add opp table!\n");
		return ret;
	}

	if (!IS_ERR(clk)) {
		axera_devfreq_profile->initial_freq = clk_get_rate(clk);
	} else {
		index = (readl(reg_addr) >> offset) & mask;

		num = of_property_count_u32_elems(pdev->dev.of_node, "translate");
		freq_arr = devm_kzalloc(&pdev->dev, num * sizeof(u32), GFP_KERNEL);

		if (IS_ERR(freq_arr)) {
			pr_err("get freq_arr failed.\n");
			return PTR_ERR(freq_arr);
		}

		ret = of_property_read_u32_array(pdev->dev.of_node, "translate", freq_arr, num);
		if (ret) {
			pr_err("get translate value failed.\n");
			return ret;
		}

		resource->freq_arr = freq_arr;
		resource->freq_arr_num = num;
		axera_devfreq_profile->initial_freq = freq_arr[index];
	}

	fp = devm_devfreq_add_device(&pdev->dev, axera_devfreq_profile, "userspace", NULL);
	if (IS_ERR(fp)) {
		pr_err("cannot add device to devfreq!\n");
		return PTR_ERR(fp);
	}

	return 0;
}

static int axera_dfs_remove(struct platform_device *pdev)
{
	return 0;
}

static const struct of_device_id axera_dfs_matches[] = {
	{.compatible = "axera,aclk_top"},
	{.compatible = "axera,aclk_top1"},
	{.compatible = "axera,aclk_top2"},
	{.compatible = "axera,aclk_top3"},
	{.compatible = "axera,aclk_top_nn"},
	{},
};

static struct platform_driver axera_dfs_driver = {
	.driver = {
		.name = "axera-dfs",
		.of_match_table = axera_dfs_matches,
	},
	.probe = axera_dfs_probe,
	.remove = axera_dfs_remove,
};

module_platform_driver(axera_dfs_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Axera");
MODULE_ALIAS("platform:axera-dfs");