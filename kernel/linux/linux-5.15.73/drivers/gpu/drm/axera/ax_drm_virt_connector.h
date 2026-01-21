/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#ifndef __AX_VIRT_CONNECTOR_H
#define __AX_VIRT_CONNECTOR_H

#include "ax_drm_drv.h"

struct ax_virt_connector {
	int state;
	bool bt_8bit_low;

	int encoder_type;
	struct drm_encoder encoder;

	int connector_type;
	struct drm_connector connector;

	struct drm_panel *panel;

	struct drm_crtc *crtc;
};

#endif /* __AX_VIRT_CONNECTOR_H */
