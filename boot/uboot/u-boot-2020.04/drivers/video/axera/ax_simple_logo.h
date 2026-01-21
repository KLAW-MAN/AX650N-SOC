/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#ifndef __AXERA_SIMPLE_LOGO_H__
#define __AXERA_SIMPLE_LOGO_H__

#define AX_MAX_VO_JPG_WIDTH 2048
#define AX_MAX_VO_JPG_HEIGHT 2048
#define AX_VO_JPG_CHANNEL 3
#define AX_MIN_RESIZE_WIDTH 1024
#define AX_MIN_RESIZE_HEIGHT 768
#define AX_RESIZE_CHANNEL 3
#define UPDATE_LOGO_X 0
#define UPDATE_LOGO_Y 0
#define AX_MAX_JPEG_VO_LOGO_SIZE             (2 * 1024 * 1024)
#define AX_MAX_BMP_VO_LOGO_SIZE              (6 * 1024 * 1024)
#define AX_MAX_VO_LOGO_SIZE                  AX_MAX_BMP_VO_LOGO_SIZE
#define AX_VO_LOGO_ALIGN_SIZE                (2 * 1024 * 1024)

#define AX_VO_JPEG_ALIGN(x,a)   ( ((x) + ((a) - 1) ) & ( ~((a) - 1) ) )


typedef enum {
	AX_VO_LOGO_FMT_NONE,
	AX_VO_LOGO_FMT_BMP = 1,
	AX_VO_LOGO_FMT_JPEG,
	AX_VO_LOGO_FMT_GZ,
	AX_VO_LOGO_FMT_BUTT
} AX_VO_LOGO_FMT_E;
#endif