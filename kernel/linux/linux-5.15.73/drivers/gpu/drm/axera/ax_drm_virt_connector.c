/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#include <linux/clk.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/of_graph.h>
#include <video/of_display_timing.h>
#include <video/of_videomode.h>
#include <dt-bindings/display/axera_display.h>

#include <drm/drm_print.h>
#include <drm/drm_modes.h>
#include <drm/drm_crtc_helper.h>
#include <drm/drm_atomic_helper.h>
#include <drm/drm_probe_helper.h>
#include <drm/drm_panel.h>
#include <drm/drm_of.h>

#include "ax_drm_crtc.h"
#include "ax_drm_virt_connector.h"

#define encoder_to_virt_connector(x) \
	container_of(x, struct ax_virt_connector, encoder)
#define connector_to_virt_connector(x) \
	container_of(x, struct ax_virt_connector, connector)

/* Connector */

static enum drm_connector_status ax_virt_connector_detect(struct drm_connector *connector, bool force)
{
	return connector_status_connected;
}

static const struct drm_connector_funcs ax_virt_connector_funcs = {
	.detect = ax_virt_connector_detect,
	.fill_modes = drm_helper_probe_single_connector_modes,
	.destroy = drm_connector_cleanup,
	.reset = drm_atomic_helper_connector_reset,
	.atomic_duplicate_state = drm_atomic_helper_connector_duplicate_state,
	.atomic_destroy_state = drm_atomic_helper_connector_destroy_state,
};

static int ax_virt_connector_get_modes(struct drm_connector *connector)
{
	struct ax_virt_connector *virt_connector;

	DRM_DEBUG_DRIVER("enter, [connector:%d:%s]\n", connector->base.id,
			 connector->name);

	virt_connector = connector_to_virt_connector(connector);

	return drm_panel_get_modes(virt_connector->panel, connector);
}

static const struct drm_connector_helper_funcs ax_virt_connector_helper_funcs = {
	.get_modes = ax_virt_connector_get_modes,
};

/* Encoder */

static int ax_virt_encoder_atomic_check(struct drm_encoder *encoder,
					struct drm_crtc_state *crtc_state,
					struct drm_connector_state *conn_state)
{
	struct ax_virt_connector *virt_connector =
	    encoder_to_virt_connector(encoder);
	struct drm_crtc *crtc = crtc_state->crtc;

	DRM_DEBUG_DRIVER("enter, bind crtc%d\n", drm_crtc_index(crtc));

	virt_connector->crtc = crtc_state->crtc;

	return 0;
}

static void ax_virt_encoder_disable(struct drm_encoder *encoder)
{
	int ret;
	struct ax_virt_connector *virt_connector =
	    encoder_to_virt_connector(encoder);

	DRM_DEBUG_DRIVER("enter, [encoder:%d:%s]\n", encoder->base.id,
			 encoder->name);

	if (!IS_ERR(virt_connector->panel)) {
		ret = drm_panel_disable(virt_connector->panel);
		if (ret)
			DRM_ERROR("pannel disable failed, ret = %d\n", ret);

		ret = drm_panel_unprepare(virt_connector->panel);
		if (ret)
			DRM_ERROR("pannel unprepare failed, ret = %d\n", ret);
	}
}

static void ax_virt_encoder_enable(struct drm_encoder *encoder)
{
	int ret;
	struct ax_virt_connector *virt_connector =
	    encoder_to_virt_connector(encoder);

	DRM_DEBUG_DRIVER("enter, [encoder:%d:%s]\n", encoder->base.id,
			 encoder->name);

	if (!IS_ERR(virt_connector->panel)) {
		ret = drm_panel_prepare(virt_connector->panel);
		if (ret)
			DRM_ERROR("pannel prepare failed, ret = %d\n", ret);

		ret = drm_panel_enable(virt_connector->panel);
		if (ret)
			DRM_ERROR("pannel enable failed, ret = %d\n", ret);
	}
}

static void ax_virt_encoder_mode_set(struct drm_encoder *encoder,
				     struct drm_display_mode *mode,
				     struct drm_display_mode *adjusted_mode)
{
	struct ax_virt_connector *virt_connector =
	    encoder_to_virt_connector(encoder);
	struct ax_crtc *ax_crtc = to_ax_crtc(virt_connector->crtc);
	struct ax_disp_mode *ax_mode = &ax_crtc->mode;

	DRM_DEBUG_DRIVER("enter, [encoder:%d:%s]\n", encoder->base.id,
			 encoder->name);
	DRM_DEBUG_DRIVER("mode: " DRM_MODE_FMT "\n", DRM_MODE_ARG(mode));
	DRM_DEBUG_DRIVER("djusted_mode: " DRM_MODE_FMT "\n",
			 DRM_MODE_ARG(adjusted_mode));

	if (adjusted_mode->flags & DRM_MODE_FLAG_INTERLACE)
		ax_mode->flags |= MODE_FLAG_INTERLACE;
	else
		ax_mode->flags &= ~(MODE_FLAG_INTERLACE);

	if (virt_connector->bt_8bit_low)
		ax_mode->flags |= MODE_FLAG_BT_8BIT_LOW;

	if (virt_connector->connector.display_info.bus_flags & DRM_BUS_FLAG_DE_LOW)
		ax_mode->de_pol = 1;
	else
		ax_mode->de_pol = 0;
}

static const struct drm_encoder_helper_funcs ax_misc_encoder_helper_funcs = {
	.atomic_check = ax_virt_encoder_atomic_check,
	.disable = ax_virt_encoder_disable,
	.enable = ax_virt_encoder_enable,
	.mode_set = ax_virt_encoder_mode_set,
	.atomic_check = ax_virt_encoder_atomic_check,
};

static void ax_virt_encoder_destroy(struct drm_encoder *encoder)
{
	drm_encoder_cleanup(encoder);
}

static const struct drm_encoder_funcs ax_virt_encoder_funcs = {
	.destroy = ax_virt_encoder_destroy,
};

static int ax_misc_panel_find(struct device *dev,
			      struct ax_virt_connector *virt_connector)
{
	int ret = 0;
	struct drm_panel *panel = NULL;
	struct device_node *port = NULL, *ep = NULL, *panel_node = NULL;

	port = of_get_next_child(dev->of_node, NULL);
	if (!port) {
		DRM_ERROR("%pOF/port not found\n", dev->of_node);
		ret = -ENODEV;
		goto exit;
	}

	ep = of_get_next_child(port, NULL);
	if (!ep) {
		DRM_ERROR("%pOF/endpoint not found\n", dev->of_node);
		ret = -ENODEV;
		goto exit;
	}

	DRM_INFO("%pOF: [%pOF, %pOF]\n", dev->of_node, port, ep);

	panel_node = of_graph_get_remote_port_parent(ep);
	if (!panel_node) {
		DRM_ERROR("%pOF: no remote endpoint\n", ep);
		ret = -ENODEV;
		goto exit;
	}

	panel = of_drm_find_panel(panel_node);
	if (IS_ERR(panel)) {
		DRM_WARN("%pOF: misc panel not found\n", dev->of_node);
		ret = 0;
		goto exit;
	}

	DRM_INFO("%pOF: misc panel found\n", dev->of_node);

	virt_connector->panel = panel;

exit:
	of_node_put(ep);
	of_node_put(port);
	of_node_put(panel_node);

	return ret;
}

static int ax_virt_connector_create(struct device *dev, void *data)
{
	int ret = 0;
	int connector_type, encoder_type;
	struct platform_device *pdev = to_platform_device(dev);
	struct drm_device *drm_dev = (struct drm_device *)data;
	struct ax_virt_connector *virt_connector;
	struct drm_connector *connector = NULL;
	struct drm_encoder *encoder = NULL;

	virt_connector = devm_kzalloc(&pdev->dev, sizeof(*virt_connector), GFP_KERNEL);
	if (!virt_connector)
		return -ENOMEM;

	platform_set_drvdata(pdev, virt_connector);

	ret = ax_misc_panel_find(dev, virt_connector);
	if (ret || !virt_connector->panel)
		goto err0;

	virt_connector->bt_8bit_low = of_property_read_bool(dev->of_node, "bt-8bit-low");
	DRM_INFO("%pOF: select bt %s 8bit transmission\n", dev->of_node,
		 virt_connector->bt_8bit_low ? "lower" : "high");

	connector_type = DRM_MODE_CONNECTOR_VIRTUAL;
	encoder_type = DRM_MODE_ENCODER_VIRTUAL;

	virt_connector->connector_type = connector_type;
	virt_connector->encoder_type = encoder_type;
	connector = &virt_connector->connector;
	encoder = &virt_connector->encoder;

	/* Encoder */
	drm_encoder_helper_add(encoder, &ax_misc_encoder_helper_funcs);
	ret = drm_encoder_init(drm_dev, encoder, &ax_virt_encoder_funcs, encoder_type, "AX_VIRT");
	if (ret) {
		DRM_ERROR("%pOF: failed to init encoder\n", dev->of_node);
		goto err0;
	}

	encoder->possible_crtcs = drm_of_find_possible_crtcs(drm_dev, dev->of_node);
	if (encoder->possible_crtcs == 0) {
		DRM_ERROR("%pOF: can not find crtcs for encoder\n", dev->of_node);
		ret = -EPROBE_DEFER;
		goto err1;
	}

	/* Connector */
	connector->interlace_allowed = true;
	drm_connector_helper_add(connector, &ax_virt_connector_helper_funcs);
	ret = drm_connector_init(drm_dev, connector, &ax_virt_connector_funcs, connector_type);
	if (ret) {
		DRM_ERROR("%pOF: failed to init connector\n", dev->of_node);
		goto err1;
	}

	ret = drm_connector_attach_encoder(connector, encoder);
	if (ret) {
		DRM_ERROR("%pOF: failed to attach connector and encoder\n", dev->of_node);
		goto err2;
	}

	DRM_INFO("%pOF: [connector:%d:%s], [encoder:%d:%s]\n", dev->of_node,
		 encoder->base.id, encoder->name,
		 connector->base.id, connector->name);

	virt_connector->state = 1;

	return 0;

err2:
	drm_connector_unregister(connector);
	drm_connector_cleanup(connector);
err1:
	drm_encoder_cleanup(encoder);
err0:
	return ret;
}

static void ax_virt_connector_destroy(struct ax_virt_connector *virt_connector)
{
	if (virt_connector->state) {
		drm_connector_unregister(&virt_connector->connector);
		drm_connector_cleanup(&virt_connector->connector);
		drm_encoder_cleanup(&virt_connector->encoder);
	}
}

static int ax_bt_dpi_bind(struct device *dev, struct device *master, void *data)
{
	int ret;

	ret = ax_virt_connector_create(dev, data);

	DRM_INFO("%pOF bind %s\n", dev->of_node, ret ? "failure" : "success");

	return ret;
}

static void ax_bt_dpi_unbind(struct device *dev, struct device *master,
			     void *data)
{
	struct ax_virt_connector *virt_connector;
	struct platform_device *pdev = to_platform_device(dev);

	virt_connector = (struct ax_virt_connector *)platform_get_drvdata(pdev);

	ax_virt_connector_destroy(virt_connector);

	DRM_INFO("%pOF unbind done\n", dev->of_node);
}

const struct component_ops bt_dpi_component_ops = {
	.bind = ax_bt_dpi_bind,
	.unbind = ax_bt_dpi_unbind,
};

static int ax_bt_dpi_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;

	if (!dev->of_node)
		return -ENODEV;

	return component_add(dev, &bt_dpi_component_ops);
}

static int ax_bt_dpi_remove(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;

	component_del(dev, &bt_dpi_component_ops);

	return 0;
}

static const struct of_device_id bt_dpi_drm_dt_ids[] = {
	{
	 .compatible = "axera,bt-dpi",
	 },
	{},
};

MODULE_DEVICE_TABLE(of, bt_dpi_drm_dt_ids);

struct platform_driver bt_dpi_platform_driver = {
	.probe = ax_bt_dpi_probe,
	.remove = ax_bt_dpi_remove,
	.driver = {
		   .name = "bt-dpi-drv",
		   .of_match_table = of_match_ptr(bt_dpi_drm_dt_ids),
		   },
};

MODULE_DESCRIPTION("axera dpu driver");
MODULE_LICENSE("GPL v2");
