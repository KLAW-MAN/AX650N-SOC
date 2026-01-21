/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#ifndef __PINCTRL_AXERA_H
#define __PINCTRL_AXERA_H

/**
 * struct axera_mux_desc - hardware mux descriptor
 * @name: mux function name
 * @muxval: mux register bit value
 */
struct axera_mux_desc {
	const char *name;
	u8 muxval;
};

/**
 * struct axera_pin_data - hardware per-pin data
 * @offset: register offset pinmux controller
 */
struct axera_pin_data {
	u16 offset;
	u16 num;
	struct axera_mux_desc *muxes;
};

struct axera_pinctrl_soc_info {
	const struct pinctrl_pin_desc *pins;
	unsigned int npins;
};


#define AX_PINCTRL_PIN(pin, off, nums, ...) {		\
	.number = pin,						\
	.name = #pin,						\
	.drv_data = &(struct axera_pin_data) {			\
		.offset = off,					\
		.num = nums,						\
		.muxes = (struct axera_mux_desc[]) {		\
			 __VA_ARGS__, { } },			\
	},							\
}

#define AX_PINCTRL_MUX(_val, _name) {					\
	.name = _name,						\
	.muxval = _val,				\
}


int axera_pinctrl_init(struct platform_device *pdev,
		    struct axera_pinctrl_soc_info *info);

#endif /* __PINCTRL_AXERA_H */
