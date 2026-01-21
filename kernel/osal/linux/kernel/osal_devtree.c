#include <linux/module.h>
#include <linux/device.h>
#include <linux/kdev_t.h>
#include <linux/kernel.h>
#include <linux/printk.h>
#include <linux/kmod.h>
#include <linux/fs.h>
#include <linux/of.h>
#include <linux/of_address.h>

#include "linux/platform_device.h"
#include "linux/device.h"

#include "osal_ax.h"

#include "osal_dev_ax.h"
#include "axdev.h"
#include "axdev_log.h"

#define osal_printk printk
#define DRVAL_DEBUG 0

AX_S32
    __attribute__ ((__noinline__)) AX_OSAL_DEV_of_property_read_string(AX_VOID * pdev, const char *propname,
								       const char **out_string)
{
	struct platform_device *pvdev = (struct platform_device *)pdev;
	struct device *dev = &pvdev->dev;
	return of_property_read_string(dev->of_node, propname, out_string);
}

EXPORT_SYMBOL(AX_OSAL_DEV_of_property_read_string);

#if 0
const __attribute__ ((__noinline__))
AX_VOID *AX_OSAL_DEV_of_get_property(AX_VOID * pdev, const AX_S8 * name, AX_S32 * lenp)
{
	struct platform_device *pvdev = (struct platform_device *)pdev;
	struct device *dev = &pvdev->dev;
	return of_get_property(dev->of_node, name, lenp);
}

EXPORT_SYMBOL(AX_OSAL_DEV_of_get_property);

__attribute__ ((__noinline__))
AX_S32 AX_OSAL_DEV_of_property_read_string_array(AX_VOID * pdev, const AX_S8 * propname, const char **out_strs,
						 AX_U64 sz)
{
	struct platform_device *pvdev = (struct platform_device *)pdev;
	struct device *dev = &pvdev->dev;
	return of_property_read_string_array(dev->of_node, propname, out_strs, sz);
}

__attribute__ ((__noinline__))
AX_S32 AX_OSAL_DEV_of_property_count_strings(AX_VOID * pdev, const AX_S8 * propname)
{
	struct platform_device *pvdev = (struct platform_device *)pdev;
	struct device *dev = &pvdev->dev;
	return of_property_count_strings(dev->of_node, propname);
}

__attribute__ ((__noinline__))
AX_S32 AX_OSAL_DEV_of_property_read_string_index(AX_VOID * pdev, const AX_S8 * propname, AX_S32 index,
						 const char **output)
{
	struct platform_device *pvdev = (struct platform_device *)pdev;
	struct device *dev = &pvdev->dev;
	return of_property_read_string_index(dev->of_node, propname, index, output);
}
#endif

__attribute__ ((__noinline__))
bool AX_OSAL_DEV_of_property_read_bool(AX_VOID * pdev, const AX_S8 * propname)
{
	struct platform_device *pvdev = (struct platform_device *)pdev;
	struct device *dev = &pvdev->dev;
	return of_property_read_bool(dev->of_node, propname);
}

EXPORT_SYMBOL(AX_OSAL_DEV_of_property_read_bool);

#if 0
__attribute__ ((__noinline__))
AX_S32 AX_OSAL_DEV_of_property_read_u8(AX_VOID * pdev, const AX_S8 * propname, AX_S8 * out_value)
{
	struct platform_device *pvdev = (struct platform_device *)pdev;
	struct device *dev = &pvdev->dev;
	return of_property_read_u8(dev->of_node, propname, out_value);
}

EXPORT_SYMBOL(AX_OSAL_DEV_of_property_read_u8);
__attribute__ ((__noinline__))
AX_S32 AX_OSAL_DEV_of_property_read_u16(AX_VOID * pdev, const AX_S8 * propname, AX_U16 * out_value)
{
	struct platform_device *pvdev = (struct platform_device *)pdev;
	struct device *dev = &pvdev->dev;
	return of_property_read_u16(dev->of_node, propname, out_value);
}

EXPORT_SYMBOL(AX_OSAL_DEV_of_property_read_u16);
#endif

__attribute__ ((__noinline__))
AX_S32 AX_OSAL_DEV_of_property_read_u32(AX_VOID * pdev, const char *propname, AX_U32 * out_value)
{
	struct platform_device *pvdev = (struct platform_device *)pdev;
	struct device *dev = &pvdev->dev;
	return of_property_read_u32(dev->of_node, propname, out_value);
}

EXPORT_SYMBOL(AX_OSAL_DEV_of_property_read_u32);

__attribute__ ((__noinline__))
AX_S32 AX_OSAL_DEV_of_property_read_s32(AX_VOID * pdev, const AX_S8 * propname, AX_S32 * out_value)
{
	struct platform_device *pvdev = (struct platform_device *)pdev;
	struct device *dev = &pvdev->dev;
	return of_property_read_s32(dev->of_node, propname, out_value);
}

EXPORT_SYMBOL(AX_OSAL_DEV_of_property_read_s32);
