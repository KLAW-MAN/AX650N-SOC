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
#include <linux/mfd/syscon.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/regmap.h>
#include <linux/io.h>
#include <linux/reset.h>
#include <linux/sort.h>

#include <drm/drm_of.h>
#include <drm/drm_crtc_helper.h>
#include <drm/drm_edid.h>
#include <uapi/linux/videodev2.h>

#include "ax_drm_drv.h"
#include "ax_drm_crtc.h"
#include "ax_dw_hdmi.h"

static int ax_hdmi_clk_prepare(struct ax_dw_hdmi *hdmi);
static void ax_hdmi_clk_unprepare(struct ax_dw_hdmi *hdmi);

static const struct drm_encoder_funcs ax_hdmi_encoder_funcs = {
	.destroy = drm_encoder_cleanup,
};

static void ax_hdmi_encoder_disable(struct drm_encoder *encoder)
{
	struct ax_dw_hdmi *hdmi = to_ax_hdmi(encoder);

	clk_disable_unprepare(hdmi->video_clk);
}

static bool ax_hdmi_encoder_mode_fixup(struct drm_encoder *encoder,
						const struct drm_display_mode *mode,
						struct drm_display_mode *adj_mode)
{
	DRM_DEBUG_DRIVER("mode: " DRM_MODE_FMT "\n", DRM_MODE_ARG(mode));
	DRM_DEBUG_DRIVER("dj_mode: " DRM_MODE_FMT "\n", DRM_MODE_ARG(adj_mode));

	return true;
}

static void ax_hdmi_encoder_mode_set(struct drm_encoder *encoder,
					struct drm_display_mode *mode,
					struct drm_display_mode *adj_mode)
{
	unsigned long clk = 0;
	struct ax_dw_hdmi *ax_hdmi = to_ax_hdmi(encoder);

	DRM_DEBUG_DRIVER("mode: " DRM_MODE_FMT "\n", DRM_MODE_ARG(mode));
	DRM_DEBUG_DRIVER("dj_mode: " DRM_MODE_FMT "\n", DRM_MODE_ARG(adj_mode));

	if (ax_hdmi->video_clk) {
		clk_set_rate(ax_hdmi->video_clk, adj_mode->clock * 1000);
		clk = clk_get_rate(ax_hdmi->video_clk);
	}

	DRM_DEBUG_DRIVER("expected clk: %d, actual clk: %ld\n", adj_mode->clock * 1000, clk);
}

static void ax_hdmi_encoder_enable(struct drm_encoder *encoder)
{
	struct ax_dw_hdmi *hdmi = to_ax_hdmi(encoder);

	WARN(clk_prepare_enable(hdmi->video_clk), "hdmi%d clk prepare failed\n", hdmi->id);
}

static int ax_hdmi_encoder_atomic_check(struct drm_encoder *encoder,
						 struct drm_crtc_state *crtc_state,
						 struct drm_connector_state *conn_state)
{
	return 0;
}

static const struct drm_encoder_helper_funcs dw_hdmi_ax_encoder_helper_funcs = {
	.mode_fixup = ax_hdmi_encoder_mode_fixup,
	.mode_set = ax_hdmi_encoder_mode_set,
	.enable = ax_hdmi_encoder_enable,
	.disable = ax_hdmi_encoder_disable,
	.atomic_check = ax_hdmi_encoder_atomic_check,
};

static const struct ax_dw_hdmi_phy_cfg hdmi_phy_cfg_def[AX_HDMI_TX_PHY_CFG_MAX] = {
	{			/* 3840x2160@60 */
		594000000,
		{
			{0x0006, 0x0658},
			{0x0009, 0x83f6},
			{0x000e, 0x0180},
			{0x0010, 0x1040},
			{0x0011, 0x0014},
			{0x0019, 0x0000},
			{0xffff},
		}
	},
	{			/* 3840x2160@60 Reduceed Blanking */
		533250000,
		{
			{0x0006, 0x0658},
			{0x0009, 0x81f3},
			{0x000e, 0x0180},
			{0x0010, 0x1040},
			{0x0011, 0x0014},
			{0x0019, 0x0000},
			{0xffff},
		}
	},
	{			/* 2560x1600@60 */
		348500000,
		{
			{0x0006, 0x0659},
			{0x0009, 0x81f3},
			{0x000e, 0x0180},
			{0x0010, 0x1041},
			{0x0011, 0x0028},
			{0x0019, 0x0000},
			{0xffff},
		}
	},
	{			/* 3840x2160@30 */
		297000000,
		{
			{0x0006, 0x0019},
			{0x0009, 0x81c8},
			{0x000e, 0x0180},
			{0x0010, 0x1041},
			{0x0011, 0x0028},
			{0x0019, 0x0004},
			{0xffff},
		}
	},
	{			/* 2560x1600@60 Reduceed Blanking */
		268500000,
		{
			{0x0006, 0x0000},
			{0x0009, 0x81c8},
			{0x000e, 0x0180},
			{0x0010, 0x3040},
			{0x0011, 0x0605},
			{0x0019, 0x0004},
			{0xffff},
		}
	},
	{			/* 1600x1200@60 */
		193250000,
		{
			{0x0006, 0x0000},
			{0x0009, 0x81c8},
			{0x000e, 0x0280},
			{0x0010, 0x3040},
			{0x0011, 0x0605},
			{0x0019, 0x0004},
			{0xffff},
		}
	},
	{			/* 1600x1200@60 */
		162000000,
		{
			{0x0006, 0x0001},
			{0x0009, 0x81c8},
			{0x000e, 0x0280},
			{0x0010, 0x2080},
			{0x0011, 0x020a},
			{0x0019, 0x0004},
			{0xffff},
		}
	},
	{			/* 1920x1200@60 */
		154000000,
		{
			{0x0006, 0x0001},
			{0x0009, 0x81c8},
			{0x000e, 0x0280},
			{0x0010, 0x3041},
			{0x0011, 0x010a},
			{0x0019, 0x0004},
			{0xffff},
		}
	},
	{			/* 1080p60 */
		148500000,
		{
			{0x0006, 0x0001},
			{0x0009, 0x8088},
			{0x000e, 0x0180},
			{0x0010, 0x2080},
			{0x0011, 0x020a},
			{0x0019, 0x0007},
			{0xffff},
		}
	},
	{			/* 1680x1050@60 */
		146250000,
		{
			{0x0006, 0x0001},
			{0x0009, 0x8088},
			{0x000e, 0x0180},
			{0x0010, 0x2080},
			{0x0011, 0x020a},
			{0x0019, 0x0007},
			{0xffff},
		}
	},
	{			/* 1680x1050@60 Reduceed Blanking */
		119000000,
		{
			{0x0006, 0x0001},
			{0x0009, 0x8088},
			{0x000e, 0x0180},
			{0x0010, 0x20c0},
			{0x0011, 0x060a},
			{0x0019, 0x0007},
			{0xffff},
		}
	},
	{			/* 1280x1024@60 */
		108000000,
		{
			{0x0006, 0x0001},
			{0x0009, 0x8088},
			{0x000e, 0x0180},
			{0x0010, 0x20c0},
			{0x0011, 0x060a},
			{0x0019, 0x0007},
			{0xffff},
		}
	},
	{			/* 1440x900@60 */
		106500000,
		{
			{0x0006, 0x0001},
			{0x0009, 0x8088},
			{0x000e, 0x0180},
			{0x0010, 0x20c0},
			{0x0011, 0x060a},
			{0x0019, 0x0007},
			{0xffff},
		}
	},
	{			/* 1366x768@60 */
		85500000,
		{
			{0x0006, 0x0002},
			{0x0009, 0x8088},
			{0x000e, 0x0180},
			{0x0010, 0x1142},
			{0x0011, 0x0214},
			{0x0019, 0x0007},
			{0xffff},
		}
	},
	{			/* 1280x800@60 */
		83500000,
		{
			{0x0006, 0x0002},
			{0x0009, 0x8088},
			{0x000e, 0x0180},
			{0x0010, 0x1142},
			{0x0011, 0x0214},
			{0x0019, 0x0007},
			{0xffff},
		}
	},
	{			/* 1280x768@60 */
		79500000,
		{
			{0x0006, 0x0002},
			{0x0009, 0x8088},
			{0x000e, 0x0180},
			{0x0010, 0x1142},
			{0x0011, 0x0214},
			{0x0019, 0x0007},
			{0xffff},
		}
	},
	{			/* 720p60 */
		74250000,
		{
			{0x0006, 0x0002},
			{0x0009, 0x8088},
			{0x000e, 0x0180},
			{0x0010, 0x1142},
			{0x0011, 0x0214},
			{0x0019, 0x0007},
			{0xffff},
		}
	},
	{			/* 1280x800@60 */
		71000000,
		{
			{0x0006, 0x0002},
			{0x0009, 0x8088},
			{0x000e, 0x0180},
			{0x0010, 0x1183},
			{0x0011, 0x0614},
			{0x0019, 0x0007},
			{0xffff},
		}
	},
	{			/* 1024x768@50 */
		65000000,
		{
			{0x0006, 0x0002},
			{0x0009, 0x8088},
			{0x000e, 0x0180},
			{0x0010, 0x1183},
			{0x0011, 0x0614},
			{0x0019, 0x0007},
			{0xffff},
		}
	},
	{			/* 800x600@60 */
		40000000,
		{
			{0x0006, 0x0003},
			{0x0009, 0x8088},
			{0x000e, 0x0180},
			{0x0010, 0x0285},
			{0x0011, 0x0228},
			{0x0019, 0x0007},
			{0xffff},
		}
	},
	{			/* 720x576@50Hz 480p60 */
		27000000,
		{
			{0x0006, 0x0003},
			{0x0009, 0x8088},
			{0x000e, 0x0180},
			{0x0010, 0x0283},
			{0x0011, 0x0628},
			{0x0019, 0x0007},
			{0xffff},
		}
	},
	{			/* 640x480@60Hz */
		25175000,
		{
			{0x0006, 0x0003},
			{0x0009, 0x8088},
			{0x000e, 0x0180},
			{0x0010, 0x0283},
			{0x0011, 0x0628},
			{0x0019, 0x0007},
			{0xffff},
		}
	},
	{			/* end */
		0,
	},
};

static enum drm_mode_status ax_hdmi_mode_valid(struct dw_hdmi *hdmi, void *priv_data,
					       const struct drm_display_info *info,
					       const struct drm_display_mode *mode)
{
	int ret = MODE_OK;

	if (mode->flags & DRM_MODE_FLAG_INTERLACE) {
		ret = MODE_NO_INTERLACE;
		goto exit;
	}

	if (mode->clock < 25000) {
		ret = MODE_CLOCK_LOW;
		goto exit;
	}

	if (mode->clock > 594000) {
		ret = MODE_CLOCK_HIGH;
		goto exit;
	}

exit:
	DRM_DEBUG_DRIVER("%s mode: " DRM_MODE_FMT "\n", (ret == MODE_OK) ? "valid" : "invalid", DRM_MODE_ARG(mode));

	return ret;
}

static int ax_hdmi_configure_phy(struct dw_hdmi *hdmi, void *priv_data, unsigned long mpixelclock)
{
	int i;
	unsigned long diff;
	struct ax_dw_hdmi_phy_cfg *phy_data = priv_data;
	struct ax_dw_hdmi_phy_cfg *best_phy_data = NULL;

#ifdef AX_VO_DRM_HAPS_TEST
	dw_hdmi_phy_i2c_set_addr(hdmi, 0x55);
#endif

	for (diff = ~0; phy_data->pixelclock > 0; phy_data++) {
		if (mpixelclock == phy_data->pixelclock) {
			best_phy_data = phy_data;
			break;

		} else if (mpixelclock < phy_data->pixelclock) {
			best_phy_data = phy_data;
			diff = phy_data->pixelclock - mpixelclock;
		} else {
			if ((mpixelclock - phy_data->pixelclock) < diff) {
				best_phy_data = phy_data;
				diff = mpixelclock - phy_data->pixelclock;
			}

			break;
		}
	}

	if (best_phy_data) {
		for (i = 0; i < AX_HDMI_TX_PHY_RES_MAX; i++) {
			if (best_phy_data->phy_regs[i].offs == 0xffff)
				break;

			dw_hdmi_phy_i2c_write(hdmi, best_phy_data->phy_regs[i].data,
					      best_phy_data->phy_regs[i].offs);
		}

		DRM_DEBUG_DRIVER("configure phy done, pixelclock:%ld, actual pixelclock:%ld\n", mpixelclock, best_phy_data->pixelclock);

		return 0;
	}

	DRM_ERROR("configure phy failed, mpixelclock = %ld\n", mpixelclock);

	return -1;
}

static int phy_data_parse(const char *buf, size_t count, struct ax_dw_hdmi_phy_cfg *phy_cfg)
{
	int i, ret = 0, num_len;
	unsigned long val;
	char *p, *next, *end, tmp[128];

	memset(phy_cfg, 0, sizeof(*phy_cfg));

	p = (char *)buf;
	end = (char *)buf + count;
	next = strstr(p, ",");
	if (!next) {
		DRM_ERROR("format no support\n");
		ret = -1;
		goto exit;
	}
	num_len = (int)(next - p) > (sizeof(tmp) - 1) ? (sizeof(tmp) - 1) : (int)(next - p);
	strncpy(tmp, p, num_len);
	tmp[num_len] = 0;
	ret = kstrtoul(tmp, 10, &val);
	if (ret) {
		DRM_ERROR("parse pixelclock failed, ret = %d\n", ret);
		goto exit;
	}
	phy_cfg->pixelclock = val;

	for (i = 0, p = next + 1; i < (AX_HDMI_TX_PHY_RES_MAX - 1) && p < end; i++, p = next + 1) {
		next = strstr(p, ",");
		if (!next || next >= end) {
			DRM_ERROR("find offs[%d] pos failed\n", i);
			ret = -1;
			goto exit;
		}
		num_len = (int)(next - p) > (sizeof(tmp) - 1) ? (sizeof(tmp) - 1) : (int)(next - p);
		strncpy(tmp, p, num_len);
		tmp[num_len] = 0;
		ret = kstrtoul(tmp, 16, &val);
		if (ret) {
			DRM_ERROR("parse offs[%d] failed, ret = %d\n", i, ret);
			goto exit;
		}
		phy_cfg->phy_regs[i].offs = val;

		p = next + 1;
		next = strstr(p, ",");
		if (!next && p >= end) {
			DRM_ERROR("find data[%d] pos failed\n", i);
			ret = -1;
			goto exit;
		}
		if (!next)
			next = end;
		num_len = (int)(next - p) > (sizeof(tmp) - 1) ? (sizeof(tmp) - 1) : (int)(next - p);
		strncpy(tmp, p, num_len);
		tmp[num_len] = 0;
		ret = kstrtoul(tmp, 16, &val);
		if (ret) {
			DRM_ERROR("parse data[%d] failed, str = %s, ret = %d\n", i, tmp, ret);
			goto exit;
		}
		phy_cfg->phy_regs[i].data = val;
		DRM_INFO("pexelclock: %ld, offs: 0x%x, data: 0x%x\n",
				 phy_cfg->pixelclock,
				 phy_cfg->phy_regs[i].offs,
				 phy_cfg->phy_regs[i].data);
	}

exit:
	if (!ret)
		phy_cfg->phy_regs[i].offs = 0xffff;

	return ret;
}

static void phy_reg_insert(struct phy_reg *reg, struct ax_dw_hdmi_phy_cfg *phy_cfg)
{
	int i;

	for (i = 0; i < AX_HDMI_TX_PHY_RES_MAX - 1; i++) {
		if (phy_cfg->phy_regs[i].offs == 0xffff ||
			phy_cfg->phy_regs[i].offs == reg->offs) {
			if (phy_cfg->phy_regs[i].offs == 0xffff)
				phy_cfg->phy_regs[i + 1].offs = 0xffff;
			phy_cfg->phy_regs[i].offs = reg->offs;
			phy_cfg->phy_regs[i].data = reg->data;
			break;
		}
	}
}

static int compare_pixel_clock(const void *a, const void *b)
{
	const struct ax_dw_hdmi_phy_cfg *cfg1 = a;
	const struct ax_dw_hdmi_phy_cfg *cfg2 = b;

	return (cfg2->pixelclock > cfg1->pixelclock) ? 1 : -1;
}

static int phy_cfg_update(struct ax_dw_hdmi_phy_cfg *cur, void *phy_data)
{
	int i, j;
	struct ax_dw_hdmi_phy_cfg *phy_cfgs = phy_data, *phy_cfg;

	for (i = 0; i < AX_HDMI_TX_PHY_CFG_MAX; i += 1) {
		phy_cfg = &phy_cfgs[i];
		if (phy_cfg->pixelclock && (phy_cfg->pixelclock == cur->pixelclock)) {
			for (j = 0; cur->phy_regs[j].offs != 0xffff; j++)
				phy_reg_insert(&cur->phy_regs[j], phy_cfg);
			return 0;
		}
	}

	for (i = 0; i < AX_HDMI_TX_PHY_CFG_MAX; i += 1) {
		phy_cfg = &phy_cfgs[i];
		if (!phy_cfg->pixelclock) {
			memcpy(phy_cfg, cur, sizeof(*phy_cfg));
			sort(phy_cfgs, i + 1, sizeof(struct ax_dw_hdmi_phy_cfg), compare_pixel_clock, NULL);
			return 0;
		}
	}

	return -EINVAL;
}

ssize_t ax_hdmi_phy_reg_store(void *priv_data, const char *buf, size_t count)
{
	int ret;
	struct ax_dw_hdmi_phy_cfg *phy_data = priv_data, cur_phy_cfg;

	ret = phy_data_parse(buf, count, &cur_phy_cfg);
	if (ret) {
		DRM_ERROR("usage: echo pixelclock,offs[0],data[0]...offs[n],data[n] > phy_reg\n");
		return ret;
	}

	ret = phy_cfg_update(&cur_phy_cfg, phy_data);

	return ret ? ret : count;
}

ssize_t ax_hdmi_phy_reg_show(void *priv_data, char *buf)
{
	int i, index = 0, len = PAGE_SIZE;
	struct ax_dw_hdmi_phy_cfg *phy_cfgs = priv_data, *phy_cfg;

	for (phy_cfg = &phy_cfgs[0]; phy_cfg->pixelclock > 0; phy_cfg++) {
		index += snprintf(buf + index, len - index, "\npixel clock: %ld\n", phy_cfg->pixelclock);
		for (i = 0; i < AX_HDMI_TX_PHY_RES_MAX; i++) {
			if (phy_cfg->phy_regs[i].offs == 0xffff)
				break;

			index += snprintf(buf + index, len - index,
					 "0x%x, 0x%x\n",
					 phy_cfg->phy_regs[i].offs,
					 phy_cfg->phy_regs[i].data);
		}
	}

	return index;
}

static const struct of_device_id hdmi_dt_ids[] = {
	{.compatible = "axera,hdmi",},
	{},
};

MODULE_DEVICE_TABLE(of, hdmi_dt_ids);

static void ax_hdmi_plat_data_init(struct dw_hdmi_plat_data *plat_data, void *priv_data)
{
	plat_data->input_bus_encoding = V4L2_YCBCR_ENC_709;
	plat_data->priv_data = priv_data;
	plat_data->mode_valid = ax_hdmi_mode_valid;
	plat_data->configure_phy = ax_hdmi_configure_phy;
	plat_data->ycbcr_420_allowed = true;
	plat_data->disable_cec = true;
}

static int ax_hdmi_parse_dt(struct device *dev, struct ax_dw_hdmi *hdmi)
{
	int i, ret = 0;
	struct ax_dw_hdmi_phy_cfg *phy_data, cur_phy_cfg;

	ret = of_property_read_u32(dev->of_node, "id", &hdmi->id);
	if (ret) {
		DRM_ERROR("get hdmi id failed, of_node: %pOF\n", dev->of_node);
		goto exit;
	}

	phy_data = hdmi->phy_cfgs;
	memcpy(phy_data, hdmi_phy_cfg_def, sizeof(hdmi->phy_cfgs));

	i = of_property_count_strings(dev->of_node, "phy-configs");
	for (; i > 0; i -= 1) {
		const char *phy_configs;

		ret = of_property_read_string_index(dev->of_node, "phy-configs", i - 1, &phy_configs);
		if (of_property_read_string_index(dev->of_node, "phy-configs", i - 1, &phy_configs) == 0) {
			if (phy_data_parse(phy_configs, strlen(phy_configs), &cur_phy_cfg) == 0)
				phy_cfg_update(&cur_phy_cfg, phy_data);
		}
	}

	hdmi->prst_ctrl = devm_reset_control_get_optional(dev, "hdmi_p_rst");
	if (IS_ERR(hdmi->prst_ctrl)) {
		DRM_ERROR("get hdmi prst_ctrl control failed, of_node: %pOF\n", dev->of_node);
		ret = PTR_ERR(hdmi->prst_ctrl);
		goto exit;
	}

	hdmi->rst_ctrl = devm_reset_control_get_optional(dev, "hdmi_rst");
	if (IS_ERR(hdmi->rst_ctrl)) {
		DRM_ERROR("get hdmi rst_ctrl control failed, of_node: %pOF\n", dev->of_node);
		ret = PTR_ERR(hdmi->rst_ctrl);
		goto exit;
	}

#ifndef AX_VO_DRM_HAPS_TEST
	hdmi->video_clk = devm_clk_get(dev, "ivideo");
	if (IS_ERR(hdmi->video_clk)) {
		DRM_ERROR("get ivideo clk failed, of_node: %pOF\n", dev->of_node);
		ret = PTR_ERR(hdmi->video_clk);
		hdmi->video_clk = NULL;
		goto exit;
	}

	hdmi->i32k_clk = devm_clk_get(dev, "i32k");
	if (IS_ERR(hdmi->i32k_clk)) {
		DRM_ERROR("get i32k clk failed, of_node: %pOF\n", dev->of_node);
		ret = PTR_ERR(hdmi->i32k_clk);
		hdmi->i32k_clk = NULL;
		goto exit;
	}

	hdmi->apb_clk = devm_clk_get(dev, "iapb");
	if (IS_ERR(hdmi->apb_clk)) {
		DRM_ERROR("get iapb clk failed, of_node: %pOF\n", dev->of_node);
		ret = PTR_ERR(hdmi->apb_clk);
		hdmi->apb_clk = NULL;
		goto exit;
	}

	hdmi->mm_isfr_clk = devm_clk_get(dev, "mm-isfr");
	if (IS_ERR(hdmi->mm_isfr_clk)) {
		DRM_ERROR("get mm_isfr_clk clk failed, of_node: %pOF\n", dev->of_node);
		ret = PTR_ERR(hdmi->mm_isfr_clk);
		hdmi->mm_isfr_clk = NULL;
		goto exit;
	}
#endif

exit:
	return ret;

}

static void ax_hdmi_reset(struct ax_dw_hdmi *hdmi)
{
	if (hdmi->prst_ctrl) {
		reset_control_assert(hdmi->prst_ctrl);
		usleep_range(10, 20);
		reset_control_deassert(hdmi->prst_ctrl);
	}

	if (hdmi->rst_ctrl) {
		reset_control_assert(hdmi->rst_ctrl);
		usleep_range(10, 20);
		reset_control_deassert(hdmi->rst_ctrl);
	}
}

static void ax_hdmi_clk_unprepare(struct ax_dw_hdmi *hdmi)
{
	clk_disable_unprepare(hdmi->i32k_clk);
	clk_disable_unprepare(hdmi->apb_clk);
	clk_disable_unprepare(hdmi->mm_isfr_clk);
}

static int ax_hdmi_clk_prepare(struct ax_dw_hdmi *hdmi)
{
	int ret = 0;

	ret = clk_prepare_enable(hdmi->i32k_clk);
	if (ret) {
		DRM_ERROR("enable hdmi%d i32k clk failed\n", hdmi->id);
		goto exit;
	}

	ret = clk_prepare_enable(hdmi->apb_clk);
	if (ret) {
		DRM_ERROR("enable hdmi%d apb clk failed\n", hdmi->id);
		goto exit;
	}

	ret = clk_prepare_enable(hdmi->mm_isfr_clk);
	if (ret) {
		DRM_ERROR("enable hdmi%d mm isfr clk failed\n", hdmi->id);
		goto exit;
	}

exit:
	if (ret)
		ax_hdmi_clk_unprepare(hdmi);

	return ret;
}

static int ax_hdmi_bind(struct device *dev, struct device *master, void *data)
{
	int ret = 0;
	struct platform_device *pdev = to_platform_device(dev);
	struct drm_device *drm_dev = data;
	struct ax_drm_mgr *drm_mgr = drm_dev->dev_private;
	struct drm_encoder *encoder;
	struct ax_dw_hdmi *ax_hdmi;
	struct ax_display_init_state *init_state = &drm_mgr->init_state;

	ax_hdmi = devm_kzalloc(&pdev->dev, sizeof(*ax_hdmi), GFP_KERNEL);
	if (!ax_hdmi)
		return -ENOMEM;

	ret = ax_hdmi_parse_dt(dev, ax_hdmi);
	if (ret)
		goto exit;

	if (!init_state->active || init_state->interface_type != AX_DISP_OUT_MODE_HDMI)
		ax_hdmi_reset(ax_hdmi);

	ret = ax_hdmi_clk_prepare(ax_hdmi);
	if (ret)
		goto exit;

	encoder = &ax_hdmi->encoder;
	encoder->possible_crtcs = drm_of_find_possible_crtcs(drm_dev, dev->of_node);
	/*
	 * If we failed to find the CRTC(s) which this encoder is
	 * supposed to be connected to, it's because the CRTC has
	 * not been registered yet.  Defer probing, and hope that
	 * the required CRTC is added later.
	 */
	if (encoder->possible_crtcs == 0) {
		DRM_ERROR("%pOF: can not find crtcs for encoder\n", dev->of_node);
		ret = -EPROBE_DEFER;
		goto exit;
	}

	drm_encoder_helper_add(encoder, &dw_hdmi_ax_encoder_helper_funcs);
	drm_encoder_init(drm_dev, encoder, &ax_hdmi_encoder_funcs, DRM_MODE_ENCODER_TMDS, NULL);

	ax_hdmi_plat_data_init(&ax_hdmi->plat_data, ax_hdmi->phy_cfgs);
	ax_hdmi->hdmi = dw_hdmi_bind(pdev, encoder, &ax_hdmi->plat_data);
	/*
	 * If dw_hdmi_bind() fails we'll never call dw_hdmi_unbind(),
	 * which would have called the encoder cleanup.  Do it manually.
	 */
	if (IS_ERR(ax_hdmi->hdmi)) {
		ret = PTR_ERR(ax_hdmi->hdmi);
		drm_encoder_cleanup(encoder);
		goto exit;
	}

	platform_set_drvdata(pdev, ax_hdmi);

exit:
	if (ret)
		ax_hdmi_clk_unprepare(ax_hdmi);

	DRM_INFO("%pOF bind %s\n", dev->of_node, ret ? "failure" : "success");

	return ret;
}

static void ax_hdmi_unbind(struct device *dev, struct device *master, void *data)
{
	struct ax_dw_hdmi *ax_hdmi = dev_get_drvdata(dev);

	dw_hdmi_unbind(ax_hdmi->hdmi);
	ax_hdmi_clk_unprepare(ax_hdmi);
	drm_encoder_cleanup(&ax_hdmi->encoder);

	DRM_INFO("%pOF unbind done\n", dev->of_node);
}

static const struct component_ops ax_hdmi_ops = {
	.bind = ax_hdmi_bind,
	.unbind = ax_hdmi_unbind,
};

static int ax_hdmi_probe(struct platform_device *pdev)
{
	if (!pdev->dev.of_node)
		return -ENODEV;

	return component_add(&pdev->dev, &ax_hdmi_ops);
}

static int ax_hdmi_remove(struct platform_device *pdev)
{
	component_del(&pdev->dev, &ax_hdmi_ops);

	return 0;
}

struct platform_driver ax_hdmi_platform_driver = {
	.probe = ax_hdmi_probe,
	.remove = ax_hdmi_remove,
	.driver = {
		.name = "ax-dw-hdmi",
		.of_match_table = hdmi_dt_ids,
	},
};

MODULE_AUTHOR("liuxiong@axera-tech.com");
MODULE_DESCRIPTION("Axera dw hdmi driver");
MODULE_LICENSE("GPL v2");
