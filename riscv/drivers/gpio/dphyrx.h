#ifndef __DPHYRC_H__
#define __DPHYRC_H__

#include "rtdef.h"

typedef enum dphyrx_pin_func_type{
	AX_FUNC_NONE = 0,
	AX_FUNC_SPI,
	AX_FUNC_UART,
	AX_FUNC_GPIO,
	AX_FUNC_COM_GPIO,
	AX_FUNC_MAX,
} dphyrx_pin_func_type_t;

typedef enum dphyrx_pin_lane{
	AX_DPHY_LANE_0 = 0,
	AX_DPHY_LANE_1,
	AX_DPHY_LANE_2,
	AX_DPHY_LANE_MAX,
} dphyrx_pin_lane_t;

typedef enum dphyrx_pin_sig{
	AX_DPHY_N = 0,
	AX_DPHY_P,
} dphyrx_pin_sig_t;

typedef enum dphyrx_pin_dir{
	AX_DPHY_OUTPUT = 0,
	AX_DPHY_INPUT,
} dphyrx_pin_dir_t;

struct pin_dphyrx_info {
	char *	pad_name;
	dphyrx_pin_lane_t lane;
	dphyrx_pin_sig_t signal;
	dphyrx_pin_dir_t fun4_6_def_dir;
	unsigned long pinmux_reg_base;
	unsigned long pin_dphyrx_reg_base;
	unsigned long rxcdphy_reg_base;
	dphyrx_pin_func_type_t func4_type;
	char *	func4_name;
	dphyrx_pin_func_type_t func6_type;
	unsigned int gpio_num;
};

#ifndef NULL
#define NULL ((void *)0)
#endif

#define PINMUX_DPHYRX0  0x4251200
#define PINMUX_DPHYRX1  0x4251400
#define PINMUX_DPHYRX2  0x4251600
#define PINMUX_DPHYRX3  0x4251800
#define DPHYRX0_PIN_REG 0x1300D000
#define DPHYRX1_PIN_REG 0x1300E000
#define DPHYRX2_PIN_REG 0x1300F000
#define DPHYRX3_PIN_REG 0x13010000

#define ISP_RXCDPHY_NUM 8
#define ISP_RXCDPHY_0   0x13C00000
#define ISP_RXCDPHY_1   0x13C10000
#define ISP_RXCDPHY_2   0x13C20000
#define ISP_RXCDPHY_3   0x13C30000
#define ISP_RXCDPHY_4   0x13C40000
#define ISP_RXCDPHY_5   0x13C50000
#define ISP_RXCDPHY_6   0x13C60000
#define ISP_RXCDPHY_7   0x13C70000

#define GPIO2_A5	(69)
#define GPIO2_A6	(70)
#define GPIO2_A7	(71)
#define GPIO2_A8	(72)
#define GPIO2_A9	(73)
#define GPIO2_A10	(74)
#define GPIO2_A11	(75)
#define GPIO2_A12	(76)
#define GPIO2_A13	(77)
#define GPIO2_A14	(78)
#define GPIO2_A15	(79)
#define GPIO2_A16	(80)
#define GPIO2_A17	(81)
#define GPIO2_A18	(82)
#define GPIO2_A19	(83)
#define GPIO2_A20	(84)

#define COM_GPIO_A0	(160)
#define COM_GPIO_A1	(161)
#define COM_GPIO_A2	(162)
#define COM_GPIO_A3	(163)
#define COM_GPIO_A4	(164)
#define COM_GPIO_A5	(165)
#define COM_GPIO_A6	(166)
#define COM_GPIO_A7	(167)
#define COM_GPIO_A8	(168)
#define COM_GPIO_A9	(169)
#define COM_GPIO_A10	(170)
#define COM_GPIO_A11	(171)
#define COM_GPIO_A12	(172)
#define COM_GPIO_A13	(173)
#define COM_GPIO_A14	(174)
#define COM_GPIO_A15	(175)
#define COM_GPIO_A16	(176)
#define COM_GPIO_A17	(177)
#define COM_GPIO_A18	(178)
#define COM_GPIO_A19	(179)
#define COM_GPIO_A20	(180)
#define COM_GPIO_A21	(181)
#define COM_GPIO_A22	(182)
#define COM_GPIO_A23	(183)
#define COM_GPIO_A24	(184)
#define COM_GPIO_A25	(185)
#define COM_GPIO_A26	(186)
#define COM_GPIO_A27	(187)
#define COM_GPIO_A28	(188)
#define COM_GPIO_A29	(189)
#define COM_GPIO_A30	(190)
#define COM_GPIO_A31	(191)


#define CORE_DIG_IOCTRL_RW_AFE_LANE_CTRL_2_1 (0x1021 * 2)
#define CORE_DIG_IOCTRL_RW_AFE_LANE_CTRL_2_7 (0x1027 * 2)
#define CORE_DIG_IOCTRL_RW_AFE_LANE_CTRL_2_8 (0x1028 * 2)
#define CORE_DIG_IOCTRL_RW_AFE_LANE_CTRL_2_1_OA_LANE_GPI_EN_BIT           (6)
#define CORE_DIG_IOCTRL_RW_AFE_LANE_CTRL_2_1_OA_LANE_GPI_HYST_EN_BIT      (8)
#define CORE_DIG_IOCTRL_RW_AFE_LANE_CTRL_2_1_OA_LANE_GPO_EN_BIT           (10)
#define CORE_DIG_IOCTRL_RW_AFE_LANE_CTRL_2_7_OA_LANE_LPTX_EN_OVR_VAL_BIT  (0)
#define CORE_DIG_IOCTRL_RW_AFE_LANE_CTRL_2_7_OA_LANE_LPTX_PON_OVR_VAL_BIT (2)
#define CORE_DIG_IOCTRL_RW_AFE_LANE_CTRL_2_8_OA_LANE_LPTX_EN_OVR_EN_BIT   (6)
#define CORE_DIG_IOCTRL_RW_AFE_LANE_CTRL_2_8_OA_LANE_LPTX_PON_OVR_EN_BIT  (7)

#define CORE_DIG_IOCTRL_RW_AFE_CB_CTRL_2_0   (0x1C20 * 2)
#define CORE_DIG_IOCTRL_RW_AFE_CB_CTRL_2_1   (0x1C21 * 2)
#define CORE_DIG_IOCTRL_RW_AFE_CB_CTRL_2_7   (0x1C27 * 2)
#define CORE_DIG_IOCTRL_RW_AFE_CB_CTRL_2_8   (0x1C28 * 2)
#define CORE_DIG_IOCTRL_RW_AFE_CB_CTRL_2_0_OA_CB_PON_OVR_VAL_BIT          (4)
#define CORE_DIG_IOCTRL_RW_AFE_CB_CTRL_2_1_OA_CB_PON_OVR_EN_BIT           (4)
#define CORE_DIG_IOCTRL_RW_AFE_CB_CTRL_2_7_OA_SETR_OVR_EN_BIT             (11)
#define CORE_DIG_IOCTRL_RW_AFE_CB_CTRL_2_8_OA_SETR_OVR_VAL_BIT            (0)
#define CORE_DIG_IOCTRL_RW_AFE_CB_CTRL_2_8_OA_SETR_OVR_VAL_MASK           (0xf << CORE_DIG_IOCTRL_RW_AFE_CB_CTRL_2_8_OA_SETR_OVR_VAL_BIT)

#define LANE_OUTPUT_OVERRIDE 0xf
#define PINMUX_GPIO_FUNC    (0x6 << 16)

void dphyrx_gpio_set_config(unsigned int gpio_num, unsigned int dir);

#endif /* __DPHYRC_H__ */