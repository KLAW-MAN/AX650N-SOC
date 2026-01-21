#ifndef _AX_HWINFO_H_
#define _AX_HWINFO_H_

#include "efuse.h"


#define AX_UID_NODE_NAME    "ax_proc/uid"
#define AX_BOARD_ID_NODE_NAME    "ax_proc/board_id"
#define AX_ADP_PLATE_ID_NODE_NAME "ax_proc/apd_plate_id"
#define AX_CHIP_TYPE_NODE_NAME    "ax_proc/chip_type"

#define  HW_FATAL_LEVEL  0
#define  HW_ERR_LEVEL  1
#define  HW_WARNING_LEVEL  2
#define  HW_INFO_LEVEL  3
#define  HW_DEBUG_LEVEL  4

#define COMM_SYS_GLB		0x4200000	//aon_glb
#define COMM_SYS_DUMMY_SW1	(COMM_SYS_GLB + 0x184)/*Use COMM_SYS_DUMMY_SW1 to store board_id*/
#define COMM_SYS_DUMMY_SW2	(COMM_SYS_GLB + 0x190)/*Use COMM_SYS_DUMMY_SW2 to store chip_type*/
#define ADC_IOC_MAGIC 'A'
#define SET_ADC_CHAN  _IOW(ADC_IOC_MAGIC, 0x80, u32)
#define GET_ADC_VALUE _IOR(ADC_IOC_MAGIC, 0x81, struct adc_value)

struct adc_value {
	u32 chan0_val;
	u32 chan1_val;
	u32 chan2_val;
	u32 chan3_val;
	u32 chan4_val;
};

typedef enum chip_type {
	NONE_CHIP_TYPE = 0x0,
	AX650A_CHIP = 0x1,
	AX650N_CHIP = 0x2,
	AX650C_CHIP = 0x3,
	AX750_CHIP = 0x4,
	AX650_CHIP_MAX = 0x5,
} chip_type_t;

typedef enum board_type {
	AX650A_Demo = 0,
	AX650N_Demo = 1,
	AX650A_EVB = 3,
	AX650N_EVB = 2,
	AX650A_SLT = 4,
	AX650N_SLT = 5,
	AX650N_PCIE = 7,
	AX650_BOARD_MAX = 8,
} board_type_t;

typedef enum adp_plate_type {
	ADP_RX_DPHY_2X4LANE = 0,
	ADP_RX_DPHY_8X2LANE,
	ADP_RX_DPHY_4X4LANE,
	ADP_RX_DPHY_2X8_LVDS_1X16LANE,
	ADP_RX_DPHY_2X4LANE_N,
	ADP_RX_CPHY_8X2TRIO,
	ADP_RX_CPHY_4X3TRIO,
	ADP_RX_DVP_1X10BIT_DPHY_1X2LANE_BT601_656_1120,
	ADP_RX_BT1120_2X10BIT,
	ADP_RX_BT1120_2X8BIT_DPHY_2X2LANE,
	ADP_RX_BT601_656_2X10BIT_DPHY_2X2LANE,
	ADP_RX_BT601_656_2X8BIT_DPHY_4X2LANE,
	ADP_RX_DVP_16bit_HDMI_TO_DPHY_2X4LANE,
	ADP_RX_BT_TO_CVBS,
	ADP_RX_MAX,
} adp_plate_type_t;
#endif