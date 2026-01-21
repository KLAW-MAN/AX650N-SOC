/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#include <common.h>

struct util_color_component {
	u32 length;
	u32 offset;
};

struct util_rgb_info {
	struct util_color_component red;
	struct util_color_component green;
	struct util_color_component blue;
	struct util_color_component alpha;
};

struct color_rgb24 {
	u32 value:24;
} __attribute__((__packed__));

/* This function takes 8-bit color values */
static inline u32 shiftcolor8(const struct util_color_component *comp, u32 value)
{
	value &= 0xff;
	/* Fill the low bits with the high bits. */
	value = (value << 8) | value;
	/* Shift down to remove unwanted low bits */
	value = value >> (16 - comp->length);
	/* Shift back up to where the value should be */
	return value << comp->offset;
}

#define MAKE_RGBA(rgb, r, g, b, a) \
	(shiftcolor8(&(rgb)->red, (r)) | \
	 shiftcolor8(&(rgb)->green, (g)) | \
	 shiftcolor8(&(rgb)->blue, (b)) | \
	 shiftcolor8(&(rgb)->alpha, (a)))

#define MAKE_RGB24(rgb, r, g, b) \
	{ .value = MAKE_RGBA(rgb, r, g, b, 0) }

void fill_smpte_rgb24(void *mem, u32 width, u32 height, u32 stride)
{
	struct util_rgb_info rgb24 = { {8, 16}, {8, 8}, {8, 0}, {0, 0}};
	const struct util_rgb_info *rgb = &rgb24;
	const struct color_rgb24 colors_top[] = {
		MAKE_RGB24(rgb, 192, 192, 192), /* grey */
		MAKE_RGB24(rgb, 192, 192, 0),	/* yellow */
		MAKE_RGB24(rgb, 0, 192, 192),	/* cyan */
		MAKE_RGB24(rgb, 0, 192, 0),	/* green */
		MAKE_RGB24(rgb, 192, 0, 192),	/* magenta */
		MAKE_RGB24(rgb, 192, 0, 0),	/* red */
		MAKE_RGB24(rgb, 0, 0, 192),	/* blue */
	};
	const struct color_rgb24 colors_middle[] = {
		MAKE_RGB24(rgb, 0, 0, 192),	/* blue */
		MAKE_RGB24(rgb, 19, 19, 19),	/* black */
		MAKE_RGB24(rgb, 192, 0, 192),	/* magenta */
		MAKE_RGB24(rgb, 19, 19, 19),	/* black */
		MAKE_RGB24(rgb, 0, 192, 192),	/* cyan */
		MAKE_RGB24(rgb, 19, 19, 19),	/* black */
		MAKE_RGB24(rgb, 192, 192, 192), /* grey */
	};
	const struct color_rgb24 colors_bottom[] = {
		MAKE_RGB24(rgb, 0, 33, 76),	/* in-phase */
		MAKE_RGB24(rgb, 255, 255, 255), /* super white */
		MAKE_RGB24(rgb, 50, 0, 106),	/* quadrature */
		MAKE_RGB24(rgb, 19, 19, 19),	/* black */
		MAKE_RGB24(rgb, 9, 9, 9),	/* 3.5% */
		MAKE_RGB24(rgb, 19, 19, 19),	/* 7.5% */
		MAKE_RGB24(rgb, 29, 29, 29),	/* 11.5% */
		MAKE_RGB24(rgb, 19, 19, 19),	/* black */
	};
	unsigned int x;
	unsigned int y;

	for (y = 0; y < height * 6 / 9; ++y) {
		for (x = 0; x < width; ++x)
			((struct color_rgb24 *)mem)[x] =
				colors_top[x * 7 / width];
		mem += stride;
	}

	for (; y < height * 7 / 9; ++y) {
		for (x = 0; x < width; ++x)
			((struct color_rgb24 *)mem)[x] =
				colors_middle[x * 7 / width];
		mem += stride;
	}

	for (; y < height; ++y) {
		for (x = 0; x < width * 5 / 7; ++x)
			((struct color_rgb24 *)mem)[x] =
				colors_bottom[x * 4 / (width * 5 / 7)];
		for (; x < width * 6 / 7; ++x)
			((struct color_rgb24 *)mem)[x] =
				colors_bottom[(x - width * 5 / 7) * 3
					      / (width / 7) + 4];
		for (; x < width; ++x)
			((struct color_rgb24 *)mem)[x] = colors_bottom[7];
		mem += stride;
	}
}

