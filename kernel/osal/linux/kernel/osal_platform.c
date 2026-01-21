#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/printk.h>
#include <linux/wait.h>
#include <linux/sched.h>
#include <linux/sched/signal.h>
#include <linux/slab.h>
#include <linux/device.h>
#include <linux/kdev_t.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/printk.h>
#include <linux/fs.h>
#include <linux/poll.h>
#include <linux/slab.h>
#include <linux/vmalloc.h>
#include <linux/mm_types.h>
#include <linux/mm.h>
#include <linux/kmod.h>
#include <linux/fs.h>

#include <linux/wait.h>

#include "linux/platform_device.h"
#include "linux/device.h"
#include "osal_ax.h"
#include "osal_dev_ax.h"
#include "axdev.h"
#include "axdev_log.h"
#include "osal_lib_ax.h"

static AX_S32 axera_osal_common_probe(struct platform_device *pdev)
{
	struct platform_driver *axera_osal_platform_drv = NULL;
	struct AX_PLATFORM_DRIVER *axera_pdrv = NULL;
	struct device_driver *drv = pdev->dev.driver;
	if (IS_ERR_OR_NULL(drv))
		return -ENODEV;
	axera_osal_platform_drv = to_platform_driver(drv);
	if (IS_ERR_OR_NULL(axera_osal_platform_drv))
		return -EINVAL;
	axera_pdrv = (struct AX_PLATFORM_DRIVER *)axera_osal_platform_drv->axera_driver_ptr;
	if (IS_ERR_OR_NULL(axera_pdrv))
		return -EINVAL;
	if (IS_ERR_OR_NULL(axera_pdrv->probe))
		return -EINVAL;
	axera_pdrv->probe(pdev);
	return 0;
}

static AX_S32 axera_osal_common_remove(struct platform_device *pdev)
{
	struct platform_driver *axera_osal_platform_drv = NULL;
	struct AX_PLATFORM_DRIVER *axera_pdrv = NULL;
	struct device_driver *drv = pdev->dev.driver;
	if (IS_ERR_OR_NULL(drv))
		return -EINVAL;
	axera_osal_platform_drv = to_platform_driver(drv);
	if (IS_ERR_OR_NULL(axera_osal_platform_drv))
		return -EINVAL;
	axera_pdrv = (struct AX_PLATFORM_DRIVER *)axera_osal_platform_drv->axera_driver_ptr;
	if (IS_ERR_OR_NULL(axera_pdrv))
		return -EINVAL;
	if (IS_ERR_OR_NULL(axera_pdrv->remove))
		return -EINVAL;
	axera_pdrv->remove(pdev);
	return 0;
}

static AX_S32 axera_osal_common_pm_suspend(struct device *dev)
{
	struct platform_driver *axera_osal_platform_drv = NULL;
	struct AX_PLATFORM_DRIVER *axera_pdrv = NULL;
	struct platform_device *pdev = to_platform_device(dev);
	struct device_driver *drv = pdev->dev.driver;

	if (IS_ERR_OR_NULL(drv))
		return -ENODEV;
	axera_osal_platform_drv = to_platform_driver(drv);
	if (IS_ERR_OR_NULL(axera_osal_platform_drv))
		return -EINVAL;
	axera_pdrv = (struct AX_PLATFORM_DRIVER *)axera_osal_platform_drv->axera_driver_ptr;
	if (IS_ERR_OR_NULL(axera_pdrv))
		return -EINVAL;
	if (IS_ERR_OR_NULL(axera_pdrv->probe))
		return -EINVAL;
	axera_pdrv->suspend(pdev);
	return 0;
}

static AX_S32 axera_osal_common_pm_resume(struct device *dev)
{
	struct platform_driver *axera_osal_platform_drv = NULL;
	struct AX_PLATFORM_DRIVER *axera_pdrv = NULL;
	struct platform_device *pdev = to_platform_device(dev);
	struct device_driver *drv = pdev->dev.driver;

	if (IS_ERR_OR_NULL(drv))
		return -ENODEV;
	axera_osal_platform_drv = to_platform_driver(drv);
	if (IS_ERR_OR_NULL(axera_osal_platform_drv))
		return -EINVAL;
	axera_pdrv = (struct AX_PLATFORM_DRIVER *)axera_osal_platform_drv->axera_driver_ptr;
	if (IS_ERR_OR_NULL(axera_pdrv))
		return -EINVAL;
	if (IS_ERR_OR_NULL(axera_pdrv->probe))
		return -EINVAL;
	axera_pdrv->resume(pdev);
	return 0;
}

static const struct dev_pm_ops axera_osal_common_pm_ops = {
	.suspend = axera_osal_common_pm_suspend,
	.resume = axera_osal_common_pm_resume,
};

AX_S32 AX_OSAL_DEV_platform_driver_register(AX_VOID *drv)
{
	struct platform_driver *axera_osal_platform_drv = NULL;
	struct AX_PLATFORM_DRIVER *axera_pdrv = (struct AX_PLATFORM_DRIVER *)drv;
	if (IS_ERR_OR_NULL(axera_pdrv))
		return -EINVAL;
	axera_osal_platform_drv = kzalloc(sizeof(struct platform_driver), GFP_KERNEL);
	if (IS_ERR_OR_NULL(axera_osal_platform_drv))
		return -ENOMEM;
	axera_pdrv->axera_ptr = axera_osal_platform_drv;
	axera_osal_platform_drv->probe = axera_osal_common_probe;
	axera_osal_platform_drv->remove = axera_osal_common_remove;
	axera_osal_platform_drv->driver.name = axera_pdrv->driver.name;
	axera_osal_platform_drv->driver.of_match_table = axera_pdrv->driver.of_match_table;
#ifdef CONFIG_PM
	axera_osal_platform_drv->driver.pm = &axera_osal_common_pm_ops,
#endif
	axera_osal_platform_drv->axera_driver_ptr = drv;
	return __platform_driver_register(axera_osal_platform_drv, THIS_MODULE);
}

EXPORT_SYMBOL(AX_OSAL_DEV_platform_driver_register);

AX_VOID AX_OSAL_DEV_platform_driver_unregister(AX_VOID *drv)
{
	struct AX_PLATFORM_DRIVER *axera_pdrv = (struct AX_PLATFORM_DRIVER *)drv;
	struct platform_driver *axera_osal_platform_drv = axera_pdrv->axera_ptr;
	if (IS_ERR_OR_NULL(axera_osal_platform_drv)) {
		printk("platform_driver axera_osal_platform_drv is invalid\n");
		return;
	}

	platform_driver_unregister(axera_osal_platform_drv);
	kfree(axera_osal_platform_drv);
}

EXPORT_SYMBOL(AX_OSAL_DEV_platform_driver_unregister);

AX_S32 AX_OSAL_DEV_platform_get_resource_byname(AX_VOID *dev, AX_U32 type, const AX_S8 *name, struct AXERA_RESOURCE *res)
{
	struct resource *res_tmp = NULL;
	res_tmp = platform_get_resource_byname((struct platform_device *)dev, type, name);
	if (!res_tmp) {
		return -EINVAL;
	} else {
		AX_OSAL_LIB_memset(res, 0, sizeof(struct AXERA_RESOURCE));
		res->start = res_tmp->start;
		res->end = res_tmp->end;
		AX_OSAL_LIB_memcpy(res->name, res_tmp->name, AX_OSAL_LIB_strlen(res_tmp->name));
		res->flags = res_tmp->flags;
		res->desc = res_tmp->desc;
	}
	return 0;
}

EXPORT_SYMBOL(AX_OSAL_DEV_platform_get_resource_byname);

AX_S32 AX_OSAL_DEV_platform_get_resource(AX_VOID *dev, AX_U32 type, AX_U32 num, struct AXERA_RESOURCE *res)
{
	struct resource *res_tmp = NULL;
	res_tmp = platform_get_resource((struct platform_device *)dev, type, num);
	if (!res_tmp) {
		return -EINVAL;
	} else {
		AX_OSAL_LIB_memset(res, 0, sizeof(struct AXERA_RESOURCE));
		res->start = res_tmp->start;
		res->end = res_tmp->end;
		AX_OSAL_LIB_memcpy(res->name, res_tmp->name, AX_OSAL_LIB_strlen(res_tmp->name));
		res->flags = res_tmp->flags;
		res->desc = res_tmp->desc;
	}
	return 0;
}

EXPORT_SYMBOL(AX_OSAL_DEV_platform_get_resource);

AX_S32 AX_OSAL_DEV_platform_get_irq(AX_VOID *dev, AX_U32 num)
{
	return platform_get_irq((struct platform_device *)dev, num);
}

EXPORT_SYMBOL(AX_OSAL_DEV_platform_get_irq);

AX_S32 AX_OSAL_DEV_platform_get_irq_byname(AX_VOID *dev, const AX_S8 *name)
{
	return platform_get_irq_byname((struct platform_device *)dev, name);
}

EXPORT_SYMBOL(AX_OSAL_DEV_platform_get_irq_byname);

AX_ULONG AX_OSAL_DEV_resource_size(const struct AXERA_RESOURCE *res)
{
	return res->end - res->start + 1;
}

EXPORT_SYMBOL(AX_OSAL_DEV_resource_size);

AX_VOID *AX_OSAL_DEV_platform_get_drvdata(AX_VOID *pdev)
{
	struct platform_device *pvdev = (struct platform_device *)pdev;
	return platform_get_drvdata(pvdev);
}

EXPORT_SYMBOL(AX_OSAL_DEV_platform_get_drvdata);

AX_VOID AX_OSAL_DEV_platform_set_drvdata(AX_VOID *pdev, AX_VOID *data)
{
	struct platform_device *pvdev = (struct platform_device *)pdev;
	platform_set_drvdata(pvdev, data);
}

EXPORT_SYMBOL(AX_OSAL_DEV_platform_set_drvdata);

AX_S32 AX_OSAL_DEV_platform_irq_count(AX_VOID *pdev)
{
	struct platform_device *pvdev = (struct platform_device *)pdev;
	return platform_irq_count(pvdev);
}

EXPORT_SYMBOL(AX_OSAL_DEV_platform_irq_count);

AX_VOID __attribute__ ((__noinline__)) * AX_OSAL_DEV_to_platform_device(AX_VOID *dev)
{
	AX_VOID *pdev = (AX_VOID *) to_platform_device((struct device *)dev);
	return pdev;
}

EXPORT_SYMBOL(AX_OSAL_DEV_to_platform_device);

AX_VOID __attribute__ ((__noinline__)) *AX_OSAL_DEV_to_platform_driver(AX_VOID *drv)
{
	AX_VOID *pdrv = (AX_VOID *) to_platform_driver((struct device_driver *)drv);
	return pdrv;
}

EXPORT_SYMBOL(AX_OSAL_DEV_to_platform_driver);
