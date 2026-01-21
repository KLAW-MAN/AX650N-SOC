/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#ifndef __AX650_VO_CSC_H
#define __AX650_VO_CSC_H

/* yuv to rgb matrix */
#define BT601_TO_RGB_PC_MATRIX(_en) \
	.yuv2rgb_cfg = { \
		.en = _en, \
		.uv_seq = 0, \
		.matrix = { \
			{0x100, 0x0, 0x167}, \
			{0x100, 0x7a8, 0x749}, \
			{0x100, 0x1c6, 0x0}, \
		}, \
		.offset = {0x0, 0x0, 0x0}, \
	},

#define BT709_TO_RGB_PC_MATRIX(_en) \
	.yuv2rgb_cfg = { \
		.en = _en, \
		.uv_seq = 0, \
		.matrix = { \
			{0x100, 0x0, 0x193}, \
			{0x100, 0x7d0, 0x788}, \
			{0x100, 0x1db, 0x0}, \
		}, \
		.offset = {0x0, 0x0, 0x0}, \
	},

#define BT2020_TO_RGB_PC_MATRIX(_en) \
	.yuv2rgb_cfg = { \
		.en = _en, \
		.uv_seq = 0, \
		.matrix = { \
			{0x100, 0x0, 0x179}, \
			{0x100, 0x7d6, 0x76e}, \
			{0x100, 0x1e2, 0x0}, \
		}, \
		.offset = {0x0, 0x0, 0x0}, \
	},

#define BT601_TO_RGB_TV_MATRIX(_en) \
	.yuv2rgb_cfg = { \
		.en = _en, \
		.uv_seq = 0, \
		.matrix = { \
			{0x12a, 0x0, 0x199}, \
			{0x12a, 0x79c, 0x730}, \
			{0x12a, 0x204, 0x0}, \
		}, \
		.offset = {0x40, 0x0, 0x0}, \
	},

#define BT709_TO_RGB_TV_MATRIX(_en) \
	.yuv2rgb_cfg = { \
		.en = _en, \
		.uv_seq = 0, \
		.matrix = { \
			{0x12a, 0x0, 0x1cb}, \
			{0x12a, 0x7c9, 0x778}, \
			{0x12a, 0x21d, 0x0}, \
		}, \
		.offset = {0x40, 0x0, 0x0}, \
	},

/* rgb to yuv matrix */
#define RGB_TO_BT601_PC_MATRIX(_en) \
	.csc = { \
		.csc0 = { \
			.en = _en, \
			.matrix = { \
				{0x4d, 0x96, 0x1d}, \
				{0x7d5, 0x7ab, 0x80}, \
				{0x80, 0x795, 0x7eb}, \
			}, \
			.offset = { \
				{0x0, 0x0, 0x0}, \
				{0x0, 0x0, 0x0} \
			}, \
			.decimation_h = {0x0, 0x0, 0x0, 0x10, 0x10, 0x0, 0x0}, \
			.c_offbin_en = 0, \
			.uv_seq_sel = 0, \
		}, \
		.csc1 = { \
			.en = _en, \
			.decimation = (1 << 2) | 1, \
			.c_offbin_en = 0, \
		}, \
	},

#define RGB_TO_BT709_PC_MATRIX(_en) \
	.csc = { \
		.csc0 = { \
			.en = _en, \
			.matrix = { \
				{0x36, 0xb7, 0x12}, \
				{0x7e3, 0x79d, 0x80}, \
				{0x80, 0x78c, 0x7f4}, \
			}, \
			.offset = { \
				{0x0, 0x0, 0x0}, \
				{0x0, 0x0, 0x0} \
			}, \
			.decimation_h = {0x0, 0x0, 0x0, 0x10, 0x10, 0x0, 0x0}, \
			.c_offbin_en = 0, \
			.uv_seq_sel = 0, \
		}, \
		.csc1 = { \
			.en = _en, \
			.decimation = (1 << 2) | 1, \
			.c_offbin_en = 0, \
		}, \
	},

#define RGB_TO_BT2020_PC_MATRIX(_en) \
	.csc = { \
		.csc0 = { \
			.en = _en, \
			.matrix = { \
				{0x43, 0xae, 0xf}, \
				{0x7dc, 0x7a4, 0x80}, \
				{0x80, 0x78a, 0x7f6}, \
			}, \
			.offset = { \
				{0x0, 0x0, 0x0}, \
				{0x0, 0x0, 0x0} \
			}, \
			.decimation_h = {0x0, 0x0, 0x0, 0x10, 0x10, 0x0, 0x0}, \
			.c_offbin_en = 0, \
			.uv_seq_sel = 0, \
		}, \
		.csc1 = { \
			.en = _en, \
			.decimation = (1 << 2) | 1, \
			.c_offbin_en = 0, \
		}, \
	},

#define RGB_TO_BT601_TV_MATRIX(_en) \
	.csc = { \
		.csc0 = { \
			.en = _en, \
			.matrix = { \
				{0x42, 0x84, 0x1a}, \
				{0x7db, 0x7b6, 0x70}, \
				{0x6e, 0x7a2, 0x7ee}, \
			}, \
			.offset = { \
				{0x0, 0x0, 0x0}, \
				{0x40, 0x0, 0x0}, \
			}, \
			.decimation_h = {0x0, 0x0, 0x0, 0x10, 0x10, 0x0, 0x0}, \
			.c_offbin_en = 0, \
			.uv_seq_sel = 0, \
		}, \
		.csc1 = { \
			.en = _en, \
			.decimation = (1 << 2) | 1, \
			.c_offbin_en = 0, \
		}, \
	},

#define RGB_TO_BT709_TV_MATRIX(_en) \
	.csc = { \
		.csc0 = { \
			.en = _en, \
			.matrix = { \
				{0x2f, 0xa1, 0x10}, \
				{0x7e7, 0x7a9, 0x70}, \
				{0x6e, 0x79a, 0x7f6}, \
			}, \
			.offset = { \
				{0x0, 0x0, 0x0}, \
				{0x40, 0x0, 0x0} \
			}, \
			.decimation_h = {0x0, 0x0, 0x0, 0x10, 0x10, 0x0, 0x0}, \
			.c_offbin_en = 0, \
			.uv_seq_sel = 0, \
		}, \
		.csc1 = { \
			.en = _en, \
			.decimation = (1 << 2) | 1, \
			.c_offbin_en = 0, \
		}, \
	},


/* color space repository. */
#define AX650_VO_CSC_DATA \
	[CSC_MATRIX_IDENTITY] = { \
		.yuv2rgb_cfg = { \
			.en = 1, \
			.uv_seq = 0, \
			.matrix = { \
				{0x1, 0x0, 0x1}, \
				{0x0, 0x1, 0x0}, \
				{0x1, 0x0, 0x1}, \
			}, \
			.offset = {0, 0, 0}, \
		}, \
 		\
		.csc = { \
			.csc0 = { \
				.en = 1, \
				.matrix = { \
					{0x1, 0x0, 0x1}, \
					{0x0, 0x1, 0x0}, \
					{0x1, 0x0, 0x1}, \
				}, \
				.offset = { \
					{0x0, 0x0, 0x0}, \
					{0x0, 0x0, 0x0} \
				}, \
				.decimation_h = {0x0, 0x0, 0x0, 0x10, 0x10, 0x0, 0x0}, \
				.c_offbin_en = 0, \
				.uv_seq_sel = 0, \
			}, \
			.csc1 = { \
				.en = 1, \
				.decimation = (1 << 2) | 1, \
				.c_offbin_en = 0, \
			}, \
		}, \
	}, \
	[CSC_MATRIX_BT601_TO_BT601] = { \
		BT601_TO_RGB_TV_MATRIX(1) \
		RGB_TO_BT601_TV_MATRIX(1) \
	}, \
	[CSC_MATRIX_BT601_TO_BT709] = { \
		BT601_TO_RGB_TV_MATRIX(1) \
		RGB_TO_BT709_TV_MATRIX(1) \
	}, \
	[CSC_MATRIX_BT709_TO_BT709] = { \
		BT709_TO_RGB_TV_MATRIX(1) \
		RGB_TO_BT709_TV_MATRIX(1) \
	}, \
	[CSC_MATRIX_BT709_TO_BT601] = { \
		BT709_TO_RGB_TV_MATRIX(1) \
		RGB_TO_BT601_TV_MATRIX(1) \
	}, \
	[CSC_MATRIX_BT601_TO_RGB_PC] = { \
		BT601_TO_RGB_PC_MATRIX(1) \
		RGB_TO_BT601_PC_MATRIX(0) \
	}, \
	[CSC_MATRIX_BT709_TO_RGB_PC] = { \
		BT709_TO_RGB_PC_MATRIX(1) \
		RGB_TO_BT709_PC_MATRIX(0) \
	}, \
	[CSC_MATRIX_RGB_TO_BT601_PC] = { \
		BT709_TO_RGB_PC_MATRIX(0) \
		RGB_TO_BT601_PC_MATRIX(1) \
	}, \
	[CSC_MATRIX_RGB_TO_BT709_PC] = { \
		BT601_TO_RGB_TV_MATRIX(0) \
		RGB_TO_BT709_PC_MATRIX(1) \
	}, \
	[CSC_MATRIX_RGB_TO_BT2020_PC] = { \
		BT709_TO_RGB_PC_MATRIX(0) \
		RGB_TO_BT2020_PC_MATRIX(1) \
	}, \
	[CSC_MATRIX_BT2020_TO_RGB_PC] = { \
		BT2020_TO_RGB_PC_MATRIX(1) \
		RGB_TO_BT2020_PC_MATRIX(0) \
	}, \
	[CSC_MATRIX_RGB_TO_BT601_TV] = { \
		BT601_TO_RGB_TV_MATRIX(0) \
		RGB_TO_BT601_TV_MATRIX(1) \
	}, \
	[CSC_MATRIX_RGB_TO_BT709_TV] = { \
		BT709_TO_RGB_TV_MATRIX(0) \
		RGB_TO_BT709_TV_MATRIX(1) \
	},

#endif /* end of __AX650_VO_CSC_H */
