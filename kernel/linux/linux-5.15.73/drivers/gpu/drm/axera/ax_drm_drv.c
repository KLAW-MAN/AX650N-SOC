/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#include <linux/of_graph.h>
#include <linux/of_platform.h>
#include <linux/dma-mapping.h>
#include <linux/pm_runtime.h>
#include <linux/module.h>
#include <linux/component.h>
#include <linux/console.h>
#include <linux/soc/axera/ax_boardinfo.h>

#include <drm/drm_mipi_dsi.h>
#include <drm/drm_print.h>
#include <drm/drm_of.h>
#include <drm/drm_aperture.h>
#include <drm/drm_atomic_helper.h>
#include <drm/drm_drv.h>
#include <drm/drm_fb_helper.h>
#include <drm/drm_gem_cma_helper.h>
#include <drm/drm_gem_framebuffer_helper.h>
#include <drm/drm_modeset_helper_vtables.h>
#include <drm/drm_probe_helper.h>
#include <drm/drm_vblank.h>

#include <video/display_timing.h>
#include <video/of_display_timing.h>
#include <video/videomode.h>

#include "ax_drm_ioctl.h"
#include "ax_drm_drv.h"
#include "ax_drm_crtc.h"
#include "ax_drm_fb.h"
#include "ax_drm_gem.h"

#define DRIVER_NAME "AXERA"
#define DRIVER_DESC "AX SOC DRM"
#define DRIVER_DATE "20210624"
#define DRIVER_MAJOR 1
#define DRIVER_MINOR 0
#define PROC_MSG_LEN 1000

u32 ax_get_chip_type(void);

int ax_drm_release(struct inode *inode, struct file *filp)
{
	int ret;
	struct drm_file *file_priv = filp->private_data;
	struct drm_minor *minor = file_priv->minor;
	struct drm_device *dev = minor->dev;
	struct ax_drm_mgr *drm_mgr = dev->dev_private;

	ret = drm_release(inode, filp);

	if (!atomic_read(&dev->open_count)) {
		struct ax_display_mgr *dp_mgr = drm_mgr->dp_mgr;
		struct ax_display_dev *dp_dev = &dp_mgr->display_dev[drm_mgr->id];
		struct ax_display_funcs *dp_funs = dp_dev->display_funs;

		if (dp_funs && dp_funs->dispc_fb_release)
			dp_funs->dispc_fb_release(dp_dev->data);
	}

	return ret;
}

const struct drm_ioctl_desc ax_drm_ioctls[] = {
	DRM_IOCTL_DEF_DRV(AXPCLK_ADJ_PARAM_SET, ax_pclk_adj_param_set_ioctl, DRM_AUTH),
	DRM_IOCTL_DEF_DRV(AXPCLK_ADJ_PARAM_GET, ax_pclk_adj_param_get_ioctl, DRM_AUTH),
};

static const struct file_operations ax_drm_drv_fops = {
	.owner = THIS_MODULE,
	.open = drm_open,
	.release = ax_drm_release,
	.unlocked_ioctl = drm_ioctl,
	.compat_ioctl = drm_compat_ioctl,
	.poll = drm_poll,
	.read = drm_read,
	.llseek = noop_llseek,
	.mmap = ax_gem_mmap,
};

static struct drm_driver ax_drm_driver = {
	.driver_features = DRIVER_MODESET | DRIVER_GEM | DRIVER_ATOMIC,

	.dumb_create = ax_gem_dumb_create,
	.prime_handle_to_fd = drm_gem_prime_handle_to_fd,
	.prime_fd_to_handle = drm_gem_prime_fd_to_handle,
	.gem_prime_import = drm_gem_prime_import,
	.gem_prime_mmap = ax_gem_mmap_buf,

	.fops = &ax_drm_drv_fops,
	.name = DRIVER_NAME,
	.desc = DRIVER_DESC,
	.date = DRIVER_DATE,
	.major = DRIVER_MAJOR,
	.minor = DRIVER_MINOR,

	.ioctls = ax_drm_ioctls,
	.num_ioctls = ARRAY_SIZE(ax_drm_ioctls),
};

extern struct ax_display_mgr display_mgr;

static void ax_drm_of_parse_display_init_state(struct drm_device *drm_dev)
{
	int ret;
	struct display_timing dt;
	struct videomode vm;
	struct device_node *im_np;
	struct ax_drm_mgr *drm_mgr = (struct ax_drm_mgr *)drm_dev->dev_private;

	drm_mgr->init_state.active = false;

	im_np = of_get_child_by_name(drm_dev->dev->of_node, "init-mode");
	if (!im_np) {
		DRM_INFO("init mode not set\n");
		return;
	}

	ret = of_property_read_u32(im_np, "interface-type", &drm_mgr->init_state.interface_type);

	of_node_put(im_np);

	if (!ret && !of_get_display_timing(drm_dev->dev->of_node, "init-mode", &dt)) {
		drm_mgr->init_state.active = true;
		videomode_from_timing(&dt, &vm);
		drm_display_mode_from_videomode(&vm, &drm_mgr->init_state.mode);

		DRM_INFO("init_mode: " DRM_MODE_FMT "\n", DRM_MODE_ARG(&drm_mgr->init_state.mode));
	}
}

static int ax_drm_bind(struct device *dev)
{
	struct drm_device *drm_dev;
	struct ax_drm_mgr *drm_mgr;
	int ret;

	drm_dev = drm_dev_alloc(&ax_drm_driver, dev);
	if (IS_ERR(drm_dev))
		return PTR_ERR(drm_dev);

	dev_set_drvdata(dev, drm_dev);

	drm_mgr = devm_kzalloc(drm_dev->dev, sizeof(*drm_mgr), GFP_KERNEL);
	if (!drm_mgr) {
		ret = -ENOMEM;
		goto err_free;
	}

	drm_mgr->drm_dev = drm_dev;
	drm_mgr->dp_mgr = &display_mgr;

	drm_mgr->sleep_state = AX_DISP_RUNNING;
	mutex_init(&drm_mgr->sleep_lock);

	drm_dev->dev_private = drm_mgr;

	ret = dma_set_coherent_mask(drm_dev->dev, DMA_BIT_MASK(36));
	if (ret)
		goto err_free;

	ax_drm_of_parse_display_init_state(drm_dev);

	ret = drmm_mode_config_init(drm_dev);
	if (ret)
		goto err_free;

	ax_drm_mode_config_init(drm_dev);

	/* Try to bind all sub drivers. */
	ret = component_bind_all(dev, drm_dev);
	if (ret)
		goto err_free;

	ret = drm_vblank_init(drm_dev, drm_dev->mode_config.num_crtc);
	if (ret)
		goto err_free;

	drm_mode_config_reset(drm_dev);

	/* init kms poll for handling hpd */
	drm_kms_helper_poll_init(drm_dev);

	ret = drm_dev_register(drm_dev, 0);
	if (ret)
		goto err_kms;

	DRM_INFO("%pOF bind success\n", dev->of_node);

	return 0;

err_kms:
	drm_kms_helper_poll_fini(drm_dev);
err_free:
	drm_dev_put(drm_dev);

	DRM_ERROR("%pOF bind failure(%d)\n", dev->of_node, ret);

	return ret;
}

static void ax_drm_unbind(struct device *dev)
{
	struct drm_device *drm_dev = dev_get_drvdata(dev);

	drm_dev_unregister(drm_dev);
	drm_kms_helper_poll_fini(drm_dev);
	component_unbind_all(dev, drm_dev);
	drm_dev_put(drm_dev);

	DRM_INFO("%pOF unbind done\n", dev->of_node);
}

static int compare_of(struct device *dev, void *data)
{
	/* DRM_INFO("Comparing of node %pOF with %pOF\n", dev->of_node, data); */

	return dev->of_node == data;
}

static const struct component_master_ops ax_drm_ops = {
	.bind = ax_drm_bind,
	.unbind = ax_drm_unbind,
};

static int ax_drm_of_component_probe(struct device *dev,
				     int (*compare_of) (struct device *, void *),
				     const struct component_master_ops *m_ops)
{
	struct device_node *ep, *port, *remote;
	struct component_match *match = NULL;
	int i;

	/*
	 * Bind the crtc's ports first, so that drm_of_find_possible_crtcs()
	 * called from encoder's .bind callbacks works as expected
	 */
	for (i = 0;; i++) {
		port = of_parse_phandle(dev->of_node, "ports", i);
		if (!port)
			break;

		if (of_device_is_available(port->parent))
			drm_of_component_match_add(dev, &match, compare_of,
						   port->parent);

		of_node_put(port);
	}

	if (i == 0) {
		dev_err(dev, "missing 'ports' property\n");
		return -ENODEV;
	}

	if (!match) {
		dev_err(dev, "no available port\n");
		return -ENODEV;
	}

	/*
	 * For bound crtcs, bind the encoders attached to their remote endpoint
	 */
	for (i = 0;; i++) {
		port = of_parse_phandle(dev->of_node, "ports", i);
		if (!port)
			break;

		if (!of_device_is_available(port->parent)) {
			of_node_put(port);
			continue;
		}

		for_each_child_of_node(port, ep) {
			remote = of_graph_get_remote_port_parent(ep);
			if (!remote || !of_device_is_available(remote)) {
				of_node_put(remote);
				continue;
			} else if (!of_device_is_available(remote->parent)) {
				dev_warn(dev, "parent device of %pOF is not available\n", remote);
				of_node_put(remote);
				continue;
			}

			if (strcmp(remote->name, "panel"))
				drm_of_component_match_add(dev, &match, compare_of, remote);

			of_node_put(remote);
		}
		of_node_put(port);
	}

	return component_master_add_with_match(dev, m_ops, match);
}

static inline int drm_valid_check(struct device_node *of_node)
{
	if (!of_node)
		return -EINVAL;

	if ((ax_get_chip_type() == AX650C_CHIP) && !strcmp(of_node->full_name, "drm@2"))
		return -ENODEV;

	return 0;
}

static int ax_drm_platform_probe(struct platform_device *pdev)
{
	int ret;

	ret = drm_valid_check(pdev->dev.of_node);
	if (ret)
		return (ret == -ENODEV) ? 0 : ret;

	return ax_drm_of_component_probe(&pdev->dev, compare_of, &ax_drm_ops);
}

static int ax_drm_platform_remove(struct platform_device *pdev)
{
	int ret;

	ret = drm_valid_check(pdev->dev.of_node);
	if (ret)
		return 0;

	component_master_del(&pdev->dev, &ax_drm_ops);

	return 0;
}

#ifdef CONFIG_PM_SLEEP
static int ax_drm_sys_suspend(struct device *dev)
{
	struct drm_device *drm_dev = dev_get_drvdata(dev);
	struct ax_drm_mgr *drm_mgr;
	struct drm_crtc *crtc;
	struct ax_crtc *ax_crtc;
	struct ax_display_dev *dp_dev;
	struct ax_display_funcs *dp_funs;

	DRM_DEBUG_DRIVER("enter\n");

	if (!drm_dev)
		return 0;

	drm_kms_helper_poll_disable(drm_dev);

	drm_mgr = drm_dev->dev_private;
	drm_mgr->state = drm_atomic_helper_suspend(drm_dev);
	if (IS_ERR(drm_mgr->state)) {
		drm_kms_helper_poll_enable(drm_dev);
		return PTR_ERR(drm_mgr->state);
	}

	mutex_lock(&drm_mgr->sleep_lock);
	drm_for_each_crtc(crtc, drm_dev) {
		/* dpu suspend to sleep. */
		if (drm_mgr->sleep_state == AX_DISP_SLEEP) {
			ax_crtc = to_ax_crtc(crtc);
			dp_dev = ax_crtc->display_dev;
			dp_funs = dp_dev->display_funs;

			if (dp_funs->dpu_suspend)
				dp_funs->dpu_suspend(dp_dev->data);
		}
	}
	mutex_unlock(&drm_mgr->sleep_lock);

	return 0;
}

static int ax_drm_sys_resume(struct device *dev)
{
	struct drm_device *drm_dev = dev_get_drvdata(dev);
	struct ax_drm_mgr *drm_mgr;
	struct drm_crtc *crtc;
	struct ax_crtc *ax_crtc;
	struct ax_display_dev *dp_dev;
	struct ax_display_funcs *dp_funs;

	DRM_DEBUG_DRIVER("enter\n");

	if (!drm_dev)
		return 0;

	drm_mgr = drm_dev->dev_private;
	drm_atomic_helper_resume(drm_dev, drm_mgr->state);
	drm_kms_helper_poll_enable(drm_dev);

	mutex_lock(&drm_mgr->sleep_lock);
	drm_for_each_crtc(crtc, drm_dev) {
		/* dpu resume from sleep mode. */
		if (drm_mgr->sleep_state == AX_DISP_SLEEP) {
			ax_crtc = to_ax_crtc(crtc);
			dp_dev = ax_crtc->display_dev;
			dp_funs = dp_dev->display_funs;

			if (dp_funs->dpu_resume)
				dp_funs->dpu_resume(dp_dev->data);
		}
	}
	mutex_unlock(&drm_mgr->sleep_lock);

	return 0;
}
#endif

static const struct dev_pm_ops ax_drm_pm_ops = {
	SET_SYSTEM_SLEEP_PM_OPS(ax_drm_sys_suspend, ax_drm_sys_resume)
};

static const struct of_device_id ax_drm_dt_ids[] = {
	{.compatible = "axera,display-subsystem",},
	{ /* sentinel */ },
};

MODULE_DEVICE_TABLE(of, ax_drm_dt_ids);

static struct platform_driver ax_drm_platform_driver = {
	.probe = ax_drm_platform_probe,
	.remove = ax_drm_platform_remove,
	.driver = {
		   .name = "ax drm drv",
		   .of_match_table = ax_drm_dt_ids,
		   .pm = &ax_drm_pm_ops,
	},
};

static struct platform_driver *const drivers[] = {
	&ax_mipi_dsi_driver,
	&ax_hdmi_platform_driver,
	&ax_drm_platform_driver,
	&bt_dpi_platform_driver,
	&crtc_platform_driver,
};

static int __init ax_drm_init(void)
{
	DRM_INFO("drm init enter\n");

	return platform_register_drivers(drivers, ARRAY_SIZE(drivers));
}

static void __exit ax_drm_exit(void)
{
	DRM_INFO("drm exit enter\n");

	platform_unregister_drivers(drivers, ARRAY_SIZE(drivers));
}

late_initcall_sync(ax_drm_init);
module_exit(ax_drm_exit);

MODULE_AUTHOR("wanhu.zheng@axera-tech.com");
MODULE_DESCRIPTION("Axera DRM driver");
MODULE_LICENSE("GPL v2");
