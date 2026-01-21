/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <math.h>

#include "ax_base_type.h"
#include "ax_isp_common.h"

#include "isp_sensor_types.h"
#include "isp_sensor_internal.h"

#include "sc910gs_reg.h"
#include "sc910gs_ae_ctrl.h"

#define SC910GS_MAX_VTS         (0xFFFF)
#define SC910GS_MAX_RATIO       (45.88f * 3.969f)
#define SC910GS_MIN_RATIO       (1.0f)

typedef struct _SNSSC910GS_OBJ_T_ {
    AX_U32 hts;
    AX_U32 vs_hts;
    AX_U32 vts;
    AX_F32 sclk;
    AX_F32 line_period;
} SNSSC910GS0_OBJ_T;

typedef struct _SC910GS_GAIN_TABLE_T_ {
    float gain;
    AX_U8 gain_in;
    AX_U8 gain_de;
} SC910GS_GAIN_TABLE_T;

extern SNS_STATE_OBJ *g_szsc910gsCtx[AX_VIN_MAX_PIPE_NUM];
extern AX_SNS_COMMBUS_T gsc910gsBusInfo[AX_VIN_MAX_PIPE_NUM];
#define SENSOR_GET_CTX(dev, pstCtx) (pstCtx = g_szsc910gsCtx[dev])

static SNSSC910GS0_OBJ_T sns_sc910gsparams[AX_VIN_MAX_PIPE_NUM];
static AX_F32 nAgainTable[SENSOR_MAX_GAIN_STEP];
static AX_F32 nDgainTable[SENSOR_MAX_GAIN_STEP];


static AX_SNS_DRV_DELAY_TABLE_T gsc910gsAeRegsTable[] = {
    /* regs index */          /* regs addr */                /*regs value*/        /*Delay Frame Num*/
    {0, SC910GS_LONG_EXP_LINE_H,       0,          0},
    {1, SC910GS_LONG_EXP_LINE_M,       0,          0},
    {2, SC910GS_LONG_EXP_LINE_L,       0,          0},
    {3, SC910GS_LONG_AGAIN_H,          0,          0},
    {4, SC910GS_LONG_AGAIN_L,          0,          0},
    {5, SC910GS_LONG_DGAIN_H,          0,          0},
    {6, SC910GS_LONG_DGAIN_L,          0,          0},
#if 0
    {7, SC910GS_REG_331f,              0,          0},
    {8, SC910GS_REG_3385,              0,          0},
#endif
};


const SC910GS_GAIN_TABLE_T sc910gs_again_table[] = {
    {1.000, 0x03, 0x40},
    {1.016, 0x03, 0x41},
    {1.031, 0x03, 0x42},
    {1.047, 0x03, 0x43},
    {1.063, 0x03, 0x44},
    {1.078, 0x03, 0x45},
    {1.094, 0x03, 0x46},
    {1.109, 0x03, 0x47},
    {1.125, 0x03, 0x48},
    {1.141, 0x03, 0x49},
    {1.156, 0x03, 0x4A},
    {1.172, 0x03, 0x4B},
    {1.188, 0x03, 0x4C},
    {1.203, 0x03, 0x4D},
    {1.219, 0x03, 0x4E},
    {1.234, 0x03, 0x4F},
    {1.250, 0x03, 0x50},
    {1.266, 0x03, 0x51},
    {1.281, 0x03, 0x52},
    {1.297, 0x03, 0x53},
    {1.313, 0x03, 0x54},
    {1.328, 0x03, 0x55},
    {1.344, 0x03, 0x56},
    {1.359, 0x03, 0x57},
    {1.375, 0x03, 0x58},
    {1.391, 0x03, 0x59},
    {1.406, 0x03, 0x5A},
    {1.422, 0x03, 0x5B},
    {1.438, 0x03, 0x5C},
    {1.453, 0x03, 0x5D},
    {1.469, 0x03, 0x5E},
    {1.484, 0x03, 0x5F},
    {1.500, 0x03, 0x60},
    {1.516, 0x03, 0x61},
    {1.531, 0x03, 0x62},
    {1.547, 0x03, 0x63},
    {1.563, 0x03, 0x64},
    {1.578, 0x03, 0x65},
    {1.594, 0x03, 0x66},
    {1.609, 0x03, 0x67},
    {1.625, 0x03, 0x68},
    {1.641, 0x03, 0x69},
    {1.656, 0x03, 0x6A},
    {1.672, 0x03, 0x6B},
    {1.688, 0x03, 0x6C},
    {1.703, 0x03, 0x6D},
    {1.719, 0x03, 0x6E},
    {1.734, 0x03, 0x6F},
    {1.750, 0x03, 0x70},
    {1.766, 0x03, 0x71},
    {1.781, 0x03, 0x72},
    {1.797, 0x03, 0x73},
    {1.813, 0x03, 0x74},
    {1.828, 0x03, 0x75},
    {1.844, 0x03, 0x76},
    {1.859, 0x03, 0x77},
    {1.875, 0x03, 0x78},
    {1.891, 0x03, 0x79},
    {1.906, 0x03, 0x7A},
    {1.922, 0x03, 0x7B},
    {1.938, 0x03, 0x7C},
    {1.953, 0x03, 0x7D},
    {1.969, 0x03, 0x7E},
    {1.984, 0x03, 0x7F},
    {2.000, 0x07, 0x40},
    {2.031, 0x07, 0x41},
    {2.063, 0x07, 0x42},
    {2.094, 0x07, 0x43},
    {2.125, 0x07, 0x44},
    {2.156, 0x07, 0x45},
    {2.188, 0x07, 0x46},
    {2.219, 0x07, 0x47},
    {2.250, 0x07, 0x48},
    {2.281, 0x07, 0x49},
    {2.313, 0x07, 0x4A},
    {2.344, 0x07, 0x4B},
    {2.375, 0x07, 0x4C},
    {2.406, 0x07, 0x4D},
    {2.438, 0x07, 0x4E},
    {2.469, 0x07, 0x4F},
    {2.500, 0x07, 0x50},
    {2.531, 0x07, 0x51},
    {2.563, 0x07, 0x52},
    {2.594, 0x07, 0x53},
    {2.625, 0x07, 0x54},
    {2.656, 0x07, 0x55},
    {2.688, 0x07, 0x56},
    {2.719, 0x07, 0x57},
    {2.750, 0x07, 0x58},
    {2.781, 0x07, 0x59},
    {2.813, 0x07, 0x5A},
    {2.844, 0x07, 0x5B},
    {2.875, 0x07, 0x5C},
    {2.890, 0x23, 0x40},
    {2.935, 0x23, 0x41},
    {2.980, 0x23, 0x42},
    {3.025, 0x23, 0x43},
    {3.071, 0x23, 0x44},
    {3.116, 0x23, 0x45},
    {3.161, 0x23, 0x46},
    {3.206, 0x23, 0x47},
    {3.251, 0x23, 0x48},
    {3.296, 0x23, 0x49},
    {3.342, 0x23, 0x4A},
    {3.387, 0x23, 0x4B},
    {3.432, 0x23, 0x4C},
    {3.477, 0x23, 0x4D},
    {3.522, 0x23, 0x4E},
    {3.567, 0x23, 0x4F},
    {3.613, 0x23, 0x50},
    {3.658, 0x23, 0x51},
    {3.703, 0x23, 0x52},
    {3.748, 0x23, 0x53},
    {3.793, 0x23, 0x54},
    {3.838, 0x23, 0x55},
    {3.883, 0x23, 0x56},
    {3.929, 0x23, 0x57},
    {3.974, 0x23, 0x58},
    {4.019, 0x23, 0x59},
    {4.064, 0x23, 0x5A},
    {4.109, 0x23, 0x5B},
    {4.154, 0x23, 0x5C},
    {4.200, 0x23, 0x5D},
    {4.245, 0x23, 0x5E},
    {4.290, 0x23, 0x5F},
    {4.335, 0x23, 0x60},
    {4.380, 0x23, 0x61},
    {4.425, 0x23, 0x62},
    {4.470, 0x23, 0x63},
    {4.516, 0x23, 0x64},
    {4.561, 0x23, 0x65},
    {4.606, 0x23, 0x66},
    {4.651, 0x23, 0x67},
    {4.696, 0x23, 0x68},
    {4.741, 0x23, 0x69},
    {4.787, 0x23, 0x6A},
    {4.832, 0x23, 0x6B},
    {4.877, 0x23, 0x6C},
    {4.922, 0x23, 0x6D},
    {4.967, 0x23, 0x6E},
    {5.012, 0x23, 0x6F},
    {5.058, 0x23, 0x70},
    {5.103, 0x23, 0x71},
    {5.148, 0x23, 0x72},
    {5.193, 0x23, 0x73},
    {5.238, 0x23, 0x74},
    {5.283, 0x23, 0x75},
    {5.328, 0x23, 0x76},
    {5.374, 0x23, 0x77},
    {5.419, 0x23, 0x78},
    {5.464, 0x23, 0x79},
    {5.509, 0x23, 0x7A},
    {5.554, 0x23, 0x7B},
    {5.599, 0x23, 0x7C},
    {5.645, 0x23, 0x7D},
    {5.690, 0x23, 0x7E},
    {5.735, 0x23, 0x7F},
    {5.780, 0x27, 0x40},
    {5.870, 0x27, 0x41},
    {5.961, 0x27, 0x42},
    {6.051, 0x27, 0x43},
    {6.141, 0x27, 0x44},
    {6.232, 0x27, 0x45},
    {6.322, 0x27, 0x46},
    {6.412, 0x27, 0x47},
    {6.503, 0x27, 0x48},
    {6.593, 0x27, 0x49},
    {6.683, 0x27, 0x4A},
    {6.773, 0x27, 0x4B},
    {6.864, 0x27, 0x4C},
    {6.954, 0x27, 0x4D},
    {7.044, 0x27, 0x4E},
    {7.135, 0x27, 0x4F},
    {7.225, 0x27, 0x50},
    {7.315, 0x27, 0x51},
    {7.406, 0x27, 0x52},
    {7.496, 0x27, 0x53},
    {7.586, 0x27, 0x54},
    {7.677, 0x27, 0x55},
    {7.767, 0x27, 0x56},
    {7.857, 0x27, 0x57},
    {7.948, 0x27, 0x58},
    {8.038, 0x27, 0x59},
    {8.128, 0x27, 0x5A},
    {8.218, 0x27, 0x5B},
    {8.309, 0x27, 0x5C},
    {8.399, 0x27, 0x5D},
    {8.489, 0x27, 0x5E},
    {8.580, 0x27, 0x5F},
    {8.670, 0x27, 0x60},
    {8.760, 0x27, 0x61},
    {8.851, 0x27, 0x62},
    {8.941, 0x27, 0x63},
    {9.031, 0x27, 0x64},
    {9.122, 0x27, 0x65},
    {9.212, 0x27, 0x66},
    {9.302, 0x27, 0x67},
    {9.393, 0x27, 0x68},
    {9.483, 0x27, 0x69},
    {9.573, 0x27, 0x6A},
    {9.663, 0x27, 0x6B},
    {9.754, 0x27, 0x6C},
    {9.844, 0x27, 0x6D},
    {9.934, 0x27, 0x6E},
    {10.025, 0x27, 0x6F},
    {10.115, 0x27, 0x70},
    {10.205, 0x27, 0x71},
    {10.296, 0x27, 0x72},
    {10.386, 0x27, 0x73},
    {10.476, 0x27, 0x74},
    {10.567, 0x27, 0x75},
    {10.657, 0x27, 0x76},
    {10.747, 0x27, 0x77},
    {10.838, 0x27, 0x78},
    {10.928, 0x27, 0x79},
    {11.018, 0x27, 0x7A},
    {11.108, 0x27, 0x7B},
    {11.199, 0x27, 0x7C},
    {11.289, 0x27, 0x7D},
    {11.379, 0x27, 0x7E},
    {11.470, 0x27, 0x7F},
    {11.560, 0x2F, 0x40},
    {11.741, 0x2F, 0x41},
    {11.921, 0x2F, 0x42},
    {12.102, 0x2F, 0x43},
    {12.283, 0x2F, 0x44},
    {12.463, 0x2F, 0x45},
    {12.644, 0x2F, 0x46},
    {12.824, 0x2F, 0x47},
    {13.005, 0x2F, 0x48},
    {13.186, 0x2F, 0x49},
    {13.366, 0x2F, 0x4A},
    {13.547, 0x2F, 0x4B},
    {13.728, 0x2F, 0x4C},
    {13.908, 0x2F, 0x4D},
    {14.089, 0x2F, 0x4E},
    {14.269, 0x2F, 0x4F},
    {14.450, 0x2F, 0x50},
    {14.631, 0x2F, 0x51},
    {14.811, 0x2F, 0x52},
    {14.992, 0x2F, 0x53},
    {15.173, 0x2F, 0x54},
    {15.353, 0x2F, 0x55},
    {15.534, 0x2F, 0x56},
    {15.714, 0x2F, 0x57},
    {15.895, 0x2F, 0x58},
    {16.076, 0x2F, 0x59},
    {16.256, 0x2F, 0x5A},
    {16.437, 0x2F, 0x5B},
    {16.618, 0x2F, 0x5C},
    {16.798, 0x2F, 0x5D},
    {16.979, 0x2F, 0x5E},
    {17.159, 0x2F, 0x5F},
    {17.340, 0x2F, 0x60},
    {17.521, 0x2F, 0x61},
    {17.701, 0x2F, 0x62},
    {17.882, 0x2F, 0x63},
    {18.063, 0x2F, 0x64},
    {18.243, 0x2F, 0x65},
    {18.424, 0x2F, 0x66},
    {18.604, 0x2F, 0x67},
    {18.785, 0x2F, 0x68},
    {18.966, 0x2F, 0x69},
    {19.146, 0x2F, 0x6A},
    {19.327, 0x2F, 0x6B},
    {19.508, 0x2F, 0x6C},
    {19.688, 0x2F, 0x6D},
    {19.869, 0x2F, 0x6E},
    {20.049, 0x2F, 0x6F},
    {20.230, 0x2F, 0x70},
    {20.411, 0x2F, 0x71},
    {20.591, 0x2F, 0x72},
    {20.772, 0x2F, 0x73},
    {20.953, 0x2F, 0x74},
    {21.133, 0x2F, 0x75},
    {21.314, 0x2F, 0x76},
    {21.494, 0x2F, 0x77},
    {21.675, 0x2F, 0x78},
    {21.856, 0x2F, 0x79},
    {22.036, 0x2F, 0x7A},
    {22.217, 0x2F, 0x7B},
    {22.398, 0x2F, 0x7C},
    {22.578, 0x2F, 0x7D},
    {22.759, 0x2F, 0x7E},
    {22.939, 0x2F, 0x7F},
    {23.120, 0x3F, 0x40},
    {23.481, 0x3F, 0x41},
    {23.843, 0x3F, 0x42},
    {24.204, 0x3F, 0x43},
    {24.565, 0x3F, 0x44},
    {24.926, 0x3F, 0x45},
    {25.288, 0x3F, 0x46},
    {25.649, 0x3F, 0x47},
    {26.010, 0x3F, 0x48},
    {26.371, 0x3F, 0x49},
    {26.733, 0x3F, 0x4A},
    {27.094, 0x3F, 0x4B},
    {27.455, 0x3F, 0x4C},
    {27.816, 0x3F, 0x4D},
    {28.178, 0x3F, 0x4E},
    {28.539, 0x3F, 0x4F},
    {28.900, 0x3F, 0x50},
    {29.261, 0x3F, 0x51},
    {29.623, 0x3F, 0x52},
    {29.984, 0x3F, 0x53},
    {30.345, 0x3F, 0x54},
    {30.706, 0x3F, 0x55},
    {31.068, 0x3F, 0x56},
    {31.429, 0x3F, 0x57},
    {31.790, 0x3F, 0x58},
    {32.151, 0x3F, 0x59},
    {32.513, 0x3F, 0x5A},
    {32.874, 0x3F, 0x5B},
    {33.235, 0x3F, 0x5C},
    {33.596, 0x3F, 0x5D},
    {33.958, 0x3F, 0x5E},
    {34.319, 0x3F, 0x5F},
    {34.680, 0x3F, 0x60},
    {35.041, 0x3F, 0x61},
    {35.403, 0x3F, 0x62},
    {35.764, 0x3F, 0x63},
    {36.125, 0x3F, 0x64},
    {36.486, 0x3F, 0x65},
    {36.848, 0x3F, 0x66},
    {37.209, 0x3F, 0x67},
    {37.570, 0x3F, 0x68},
    {37.931, 0x3F, 0x69},
    {38.293, 0x3F, 0x6A},
    {38.654, 0x3F, 0x6B},
    {39.015, 0x3F, 0x6C},
    {39.376, 0x3F, 0x6D},
    {39.738, 0x3F, 0x6E},
    {40.099, 0x3F, 0x6F},
    {40.460, 0x3F, 0x70},
    {40.821, 0x3F, 0x71},
    {41.183, 0x3F, 0x72},
    {41.544, 0x3F, 0x73},
    {41.905, 0x3F, 0x74},
    {42.266, 0x3F, 0x75},
    {42.628, 0x3F, 0x76},
    {42.989, 0x3F, 0x77},
    {43.350, 0x3F, 0x78},
    {43.711, 0x3F, 0x79},
    {44.073, 0x3F, 0x7A},
    {44.434, 0x3F, 0x7B},
    {44.795, 0x3F, 0x7C},
    {45.156, 0x3F, 0x7D},
    {45.518, 0x3F, 0x7E},
    {45.879, 0x3F, 0x7F},

};

const SC910GS_GAIN_TABLE_T sc910gs_dgain_table[] = {
    {1.000, 0x00, 0x80},
    {1.016, 0x00, 0x82},
    {1.031, 0x00, 0x84},
    {1.047, 0x00, 0x86},
    {1.063, 0x00, 0x88},
    {1.078, 0x00, 0x8a},
    {1.094, 0x00, 0x8c},
    {1.109, 0x00, 0x8e},
    {1.125, 0x00, 0x90},
    {1.141, 0x00, 0x92},
    {1.156, 0x00, 0x94},
    {1.172, 0x00, 0x96},
    {1.188, 0x00, 0x98},
    {1.203, 0x00, 0x9a},
    {1.219, 0x00, 0x9c},
    {1.234, 0x00, 0x9e},
    {1.250, 0x00, 0xa0},
    {1.266, 0x00, 0xa2},
    {1.281, 0x00, 0xa4},
    {1.297, 0x00, 0xa6},
    {1.313, 0x00, 0xa8},
    {1.328, 0x00, 0xaa},
    {1.344, 0x00, 0xac},
    {1.359, 0x00, 0xae},
    {1.375, 0x00, 0xb0},
    {1.391, 0x00, 0xb2},
    {1.406, 0x00, 0xb4},
    {1.422, 0x00, 0xb6},
    {1.438, 0x00, 0xb8},
    {1.453, 0x00, 0xba},
    {1.469, 0x00, 0xbc},
    {1.484, 0x00, 0xbe},
    {1.500, 0x00, 0xc0},
    {1.516, 0x00, 0xc2},
    {1.531, 0x00, 0xc4},
    {1.547, 0x00, 0xc6},
    {1.563, 0x00, 0xc8},
    {1.578, 0x00, 0xca},
    {1.594, 0x00, 0xcc},
    {1.609, 0x00, 0xce},
    {1.625, 0x00, 0xd0},
    {1.641, 0x00, 0xd2},
    {1.656, 0x00, 0xd4},
    {1.672, 0x00, 0xd6},
    {1.688, 0x00, 0xd8},
    {1.703, 0x00, 0xda},
    {1.719, 0x00, 0xdc},
    {1.734, 0x00, 0xde},
    {1.750, 0x00, 0xe0},
    {1.766, 0x00, 0xe2},
    {1.781, 0x00, 0xe4},
    {1.797, 0x00, 0xe6},
    {1.813, 0x00, 0xe8},
    {1.828, 0x00, 0xea},
    {1.844, 0x00, 0xec},
    {1.859, 0x00, 0xee},
    {1.875, 0x00, 0xf0},
    {1.891, 0x00, 0xf2},
    {1.906, 0x00, 0xf4},
    {1.922, 0x00, 0xf6},
    {1.938, 0x00, 0xf8},
    {1.953, 0x00, 0xfa},
    {1.969, 0x00, 0xfc},
    {1.984, 0x00, 0xfe},
    {2.000, 0x01, 0x80},
    {2.031, 0x01, 0x82},
    {2.063, 0x01, 0x84},
    {2.094, 0x01, 0x86},
    {2.125, 0x01, 0x88},
    {2.156, 0x01, 0x8a},
    {2.188, 0x01, 0x8c},
    {2.219, 0x01, 0x8e},
    {2.250, 0x01, 0x90},
    {2.281, 0x01, 0x92},
    {2.313, 0x01, 0x94},
    {2.344, 0x01, 0x96},
    {2.375, 0x01, 0x98},
    {2.406, 0x01, 0x9a},
    {2.438, 0x01, 0x9c},
    {2.469, 0x01, 0x9e},
    {2.500, 0x01, 0xa0},
    {2.531, 0x01, 0xa2},
    {2.563, 0x01, 0xa4},
    {2.594, 0x01, 0xa6},
    {2.625, 0x01, 0xa8},
    {2.656, 0x01, 0xaa},
    {2.688, 0x01, 0xac},
    {2.719, 0x01, 0xae},
    {2.750, 0x01, 0xb0},
    {2.781, 0x01, 0xb2},
    {2.813, 0x01, 0xb4},
    {2.844, 0x01, 0xb6},
    {2.875, 0x01, 0xb8},
    {2.906, 0x01, 0xba},
    {2.938, 0x01, 0xbc},
    {2.969, 0x01, 0xbe},
    {3.000, 0x01, 0xc0},
    {3.031, 0x01, 0xc2},
    {3.063, 0x01, 0xc4},
    {3.094, 0x01, 0xc6},
    {3.125, 0x01, 0xc8},
    {3.156, 0x01, 0xca},
    {3.188, 0x01, 0xcc},
    {3.219, 0x01, 0xce},
    {3.250, 0x01, 0xd0},
    {3.281, 0x01, 0xd2},
    {3.313, 0x01, 0xd4},
    {3.344, 0x01, 0xd6},
    {3.375, 0x01, 0xd8},
    {3.406, 0x01, 0xda},
    {3.438, 0x01, 0xdc},
    {3.469, 0x01, 0xde},
    {3.500, 0x01, 0xe0},
    {3.531, 0x01, 0xe2},
    {3.563, 0x01, 0xe4},
    {3.594, 0x01, 0xe6},
    {3.625, 0x01, 0xe8},
    {3.656, 0x01, 0xea},
    {3.688, 0x01, 0xec},
    {3.719, 0x01, 0xee},
    {3.750, 0x01, 0xf0},
    {3.781, 0x01, 0xf2},
    {3.813, 0x01, 0xf4},
    {3.844, 0x01, 0xf6},
    {3.875, 0x01, 0xf8},
    {3.906, 0x01, 0xfa},
    {3.938, 0x01, 0xfc},
    {3.969, 0x01, 0xfe},
};

static AX_U32 reg_endian_swap(AX_U32 reg, AX_U32 reg_bytes)
{
    int i = 0;
    AX_U8 temp =0;
    AX_U8 temp1 = 0;
    AX_U8 temp2 = 0;

    if (reg_bytes == 1) {
        for (i = 7; i >= 0; i--) {
            temp = (reg >> i) & 1;
            temp1 += temp << (7 - i);
        }

        return temp1;
    } else {
        for (i = 7; i >= 0; i--) {
            temp = (reg >> i) & 1;
            temp1 += temp << (7 - i);
        }
        for (i = 7; i >= 0; i--) {
            temp = (reg >> (i + 8)) & 1;
            temp2 += temp << (7 - i);
        }
        return temp2 << 8 | temp1;
    }
}

static AX_F32 sc910gs_again2value(float gain, AX_U8 *again_in, AX_U8 *again_de)
{
    AX_U32 i;
    AX_U32 count;
    AX_U32 ret_value = 0;

    if (!again_in || !again_de)
        return -1;

    count = sizeof(sc910gs_again_table) / sizeof(sc910gs_again_table[0]);

    for (i = 0; i < count; i++) {
        if (gain > sc910gs_again_table[i].gain) {
            continue;
        } else {
            if (again_in)
                *again_in = sc910gs_again_table[i].gain_in;
            if (again_de)
                *again_de = sc910gs_again_table[i].gain_de;
            SNS_DBG("again=%f, again_in=0x%x, again_de=0x%x\n", gain, *again_in, *again_de);
            return sc910gs_again_table[i].gain;
        }
    }

    return -1;
}

static AX_F32 sc910gs_dgain2value(float gain, AX_U8 *dgain_in, AX_U8 *dgain_de, AX_U8 *dgain_de2)
{
    AX_U32 i;
    AX_U32 count;
    AX_U32 ret_value = 0;

    if (!dgain_in || !dgain_de)
        return -1;

    count = sizeof(sc910gs_dgain_table) / sizeof(sc910gs_dgain_table[0]);

    for (i = 0; i < count; i++) {
        if (gain > sc910gs_dgain_table[i].gain) {
            continue;
        } else {
            *dgain_in = sc910gs_dgain_table[i].gain_in;
            *dgain_de = sc910gs_dgain_table[i].gain_de;

            SNS_DBG("dgain=%f, dgain_in=0x%x, dgain_de=0x%x\n", gain, *dgain_in, *dgain_de);

            return sc910gs_dgain_table[i].gain;
        }
    }

    return -1;
}


static AX_S32 sc910gs_get_gain_table(AX_SNS_AE_GAIN_TABLE_T *params)
{
    AX_U32 i;
    AX_S32 ret = 0;
    if (!params)
        return -1;

    params->nAgainTableSize = sizeof(sc910gs_again_table) / sizeof(sc910gs_again_table[0]);
    params->nDgainTableSize = sizeof(sc910gs_dgain_table) / sizeof(sc910gs_dgain_table[0]);

    for (i = 0; i < params->nAgainTableSize ; i++) {
        nAgainTable[i] = sc910gs_again_table[i].gain;
        params->pAgainTable = nAgainTable;
    }

    for (i = 0; i < params->nDgainTableSize ; i++) {
        nDgainTable[i] = sc910gs_dgain_table[i].gain;
        params->pDgainTable = nDgainTable;
    }

    return ret;
}


/****************************************************************************
 * exposure control external function
 ****************************************************************************/
AX_S32 sc910gs_get_params_from_setting(ISP_PIPE_ID nPipeId)
{
    AX_S32 ret = 0;
    AX_U32 reg_cnt = 0;
    camera_i2c_reg_array *setting = AX_NULL;
    AX_U32 vts = 0;

    ret = sc910gs_select_setting(nPipeId, &setting, &reg_cnt);
    if(ret) {
        return ret;
    }

    ret = sc910gs_get_vts_from_setting(nPipeId, setting, reg_cnt, &vts);
    if(ret) {
        return ret;
    }

    sns_sc910gsparams[nPipeId].vts = vts;

    SNS_DBG("pipe:%d, get setting params vts:0x%x\n", nPipeId, sns_sc910gsparams[nPipeId].vts);

    return SNS_SUCCESS;
}

AX_S32 sc910gs_cfg_aec_param(ISP_PIPE_ID nPipeId)
{
    AX_F32 vs_line_period = 0;
    SNS_STATE_OBJ *sns_obj = AX_NULL;
    SNS_CHECK_VALUE_RANGE_VALID(nPipeId, 0, AX_VIN_MAX_PIPE_NUM - 1);

    SENSOR_GET_CTX(nPipeId, sns_obj);
    SNS_CHECK_PTR_VALID(sns_obj);

    sc910gs_get_params_from_setting(nPipeId);
    sns_obj->sns_mode_obj.nVts = sns_sc910gsparams[nPipeId].vts;

    if (IS_LINEAR_MODE(sns_obj->sns_mode_obj.eHDRMode)) {
        sns_sc910gsparams[nPipeId].line_period = 2 * SNS_1_SECOND_UNIT_US / (sns_obj->sns_mode_obj.nVts * sns_obj->sns_mode_obj.fFrameRate);
    } else {
        // wrong hdr mode
    }

    sns_obj->ae_ctrl_param.fTimePerLine = sns_sc910gsparams[nPipeId].line_period;

    SNS_INFO("pipe:%d, setting fps:%f, vts:0x%x, fTimePerLine:%f\n", nPipeId,
        sns_obj->sns_mode_obj.fFrameRate, sns_obj->sns_mode_obj.nVts, sns_obj->ae_ctrl_param.fTimePerLine);

    /* sensor again  limit*/
    sns_obj->ae_ctrl_param.sns_ae_limit.fMinAgain[HDR_LONG_FRAME_IDX] = 1.0f;
    sns_obj->ae_ctrl_param.sns_ae_limit.fMaxAgain[HDR_LONG_FRAME_IDX] = 45.879f;
    sns_obj->ae_ctrl_param.sns_ae_param.fAGainIncrement[HDR_LONG_FRAME_IDX] = (AX_F32)0.12859;

    /* sensor dgain  limit*/
    sns_obj->ae_ctrl_param.sns_ae_limit.fMinDgain[HDR_LONG_FRAME_IDX] = 1.0f;
    sns_obj->ae_ctrl_param.sns_ae_limit.fMaxDgain[HDR_LONG_FRAME_IDX] = 3.969f;
    sns_obj->ae_ctrl_param.sns_ae_param.fDGainIncrement[HDR_LONG_FRAME_IDX] = (AX_F32)0.02319;

    sns_obj->ae_ctrl_param.sns_ae_limit.fMinRatio = SC910GS_MIN_RATIO;
    sns_obj->ae_ctrl_param.sns_ae_limit.fMaxRatio = (AX_F32)45.879f * 3.969f;

    if (IS_LINEAR_MODE(sns_obj->sns_mode_obj.eHDRMode)) {
        sns_obj->ae_ctrl_param.sns_ae_limit.tIntTimeRange.fMinIntegrationTime[HDR_LONG_FRAME_IDX] = 1;
        sns_obj->ae_ctrl_param.sns_ae_limit.tIntTimeRange.fMaxIntegrationTime[HDR_LONG_FRAME_IDX] = 8000 / sns_sc910gsparams[nPipeId].line_period;
        sns_obj->ae_ctrl_param.sns_ae_param.fIntegrationTimeIncrement[HDR_LONG_FRAME_IDX] = 1;
    } else {
        // wrong hdr mode
    }

    sns_obj->ae_ctrl_param.fSnsHcgLcgRatio = 1.0f;
    sns_obj->ae_ctrl_param.eSnsHcgLcgMode = AX_LCG_NOTSUPPORT_MODE;

    sns_obj->ae_ctrl_param.sns_ae_param.fCurFps = sns_obj->sns_mode_obj.fFrameRate;

    SNS_DBG("inttime min = %.2f, max = %.2f, line_period=%f, vts = %d \n",
            sns_obj->ae_ctrl_param.sns_ae_limit.tIntTimeRange.fMinIntegrationTime[HDR_LONG_FRAME_IDX],
            sns_obj->ae_ctrl_param.sns_ae_limit.tIntTimeRange.fMaxIntegrationTime[HDR_LONG_FRAME_IDX],
            sns_sc910gsparams[nPipeId].line_period, sns_sc910gsparams[nPipeId].vts);

    return SNS_SUCCESS;
}


AX_S32 sc910gs_get_sensor_gain_table(ISP_PIPE_ID nPipeId, AX_SNS_AE_GAIN_TABLE_T *params)
{
    AX_S32 result = 0;
    SNS_CHECK_PTR_VALID(params);
    SNS_CHECK_VALUE_RANGE_VALID(nPipeId, 0, AX_VIN_MAX_PIPE_NUM - 1);

    result = sc910gs_get_gain_table(params);
    return result;

    return SNS_SUCCESS;
}


AX_S32 sc910gs_set_again(ISP_PIPE_ID nPipeId, AX_SNS_AE_GAIN_CFG_T *ptAGain)
{
    AX_U8 Gain_in;
    AX_U8 Gain_de;
    AX_S32 result = 0;
    AX_F32 gain_value = 0;
    AX_F32 nGainFromUser = 0;

    SNS_STATE_OBJ *sns_obj = AX_NULL;

    SNS_CHECK_VALUE_RANGE_VALID(nPipeId, 0, AX_VIN_MAX_PIPE_NUM - 1);
    SENSOR_GET_CTX(nPipeId, sns_obj);
    SNS_CHECK_PTR_VALID(sns_obj);
    SNS_CHECK_PTR_VALID(ptAGain);

    /* long gain seting */
    nGainFromUser = ptAGain->fGain[HDR_LONG_FRAME_IDX];
    nGainFromUser = AXSNS_CLIP3(nGainFromUser, sns_obj->ae_ctrl_param.sns_ae_limit.fMinAgain[HDR_LONG_FRAME_IDX],
                                sns_obj->ae_ctrl_param.sns_ae_limit.fMaxAgain[HDR_LONG_FRAME_IDX]);

    gain_value = sc910gs_again2value(nGainFromUser, &Gain_in, &Gain_de);
    if (gain_value == -1) {
        SNS_ERR("new gain match failed \n");
    } else {
        sns_obj->ae_ctrl_param.sns_ae_param.fCurAGain[HDR_LONG_FRAME_IDX] = gain_value;
        result = sc910gs_sns_update_regs_table(nPipeId, SC910GS_LONG_AGAIN_H, (Gain_in & 0xFF));
        result |= sc910gs_sns_update_regs_table(nPipeId, SC910GS_LONG_AGAIN_L, (Gain_de & 0xFF));
        if (result != 0) {
            SNS_ERR("write hw failed %d \n", result);
            return result;
        }
    }

    /* Do not delete, for its project */
    sns_obj->sztRegsInfo[0].tSnsExpInfo.szCurAGain[HDR_LONG_FRAME_IDX] =
        sns_obj->ae_ctrl_param.sns_ae_param.fCurAGain[HDR_LONG_FRAME_IDX];

    SNS_DBG("again: long frame %f\n",
            sns_obj->ae_ctrl_param.sns_ae_param.fCurAGain[HDR_LONG_FRAME_IDX]);

    return SNS_SUCCESS;
}

AX_S32 sc910gs_set_dgain(ISP_PIPE_ID nPipeId, AX_SNS_AE_GAIN_CFG_T *ptDGain)
{
    AX_U8 Gain_in;
    AX_U8 Gain_de;
    AX_U8 Gain_de2;
    AX_S32 result = 0;
    AX_F32 gain_val = 0;
    AX_F32 nGainFromUser = 0;

    SNS_STATE_OBJ *sns_obj = AX_NULL;

    SNS_CHECK_VALUE_RANGE_VALID(nPipeId, 0, AX_VIN_MAX_PIPE_NUM - 1);
    SENSOR_GET_CTX(nPipeId, sns_obj);
    SNS_CHECK_PTR_VALID(sns_obj);
    SNS_CHECK_PTR_VALID(ptDGain);

    /* long frame digital gain seting */
    nGainFromUser = ptDGain->fGain[HDR_LONG_FRAME_IDX];
    nGainFromUser = AXSNS_CLIP3(nGainFromUser, sns_obj->ae_ctrl_param.sns_ae_limit.fMinDgain[HDR_LONG_FRAME_IDX],
                                sns_obj->ae_ctrl_param.sns_ae_limit.fMaxDgain[HDR_LONG_FRAME_IDX]);

    gain_val = sc910gs_dgain2value(nGainFromUser, &Gain_in, &Gain_de, &Gain_de2);
    if (gain_val == -1) {
        SNS_ERR("new gain match failed \n");
    } else {
        sns_obj->ae_ctrl_param.sns_ae_param.fCurDGain[HDR_LONG_FRAME_IDX] = gain_val;
        result = sc910gs_sns_update_regs_table(nPipeId, SC910GS_LONG_DGAIN_H, Gain_in);
        result = sc910gs_sns_update_regs_table(nPipeId, SC910GS_LONG_DGAIN_L, Gain_de);
        if (result != 0) {
            SNS_ERR("write hw failed %d \n", result);
            return result;
        }
    }

    /* Do not delete, for its project */
    sns_obj->sztRegsInfo[0].tSnsExpInfo.szCurDGain[HDR_LONG_FRAME_IDX] =
        sns_obj->ae_ctrl_param.sns_ae_param.fCurDGain[HDR_LONG_FRAME_IDX];

    SNS_DBG("dgain: long frame %f\n",
            sns_obj->ae_ctrl_param.sns_ae_param.fCurDGain[HDR_LONG_FRAME_IDX]);

    return SNS_SUCCESS;
}


/* Calculate the max int time according to the exposure ratio */
AX_S32 sc910gs_get_integration_time_range(ISP_PIPE_ID nPipeId, AX_F32 fHdrRatio,
        AX_SNS_AE_INT_TIME_RANGE_T *ptIntTimeRange)
{
    SNS_STATE_OBJ *sns_obj = AX_NULL;

    SNS_CHECK_VALUE_RANGE_VALID(nPipeId, 0, AX_VIN_MAX_PIPE_NUM - 1);
    SENSOR_GET_CTX(nPipeId, sns_obj);
    SNS_CHECK_PTR_VALID(sns_obj);
    SNS_CHECK_PTR_VALID(ptIntTimeRange);

    if (AX_SNS_LINEAR_MODE == sns_obj->sns_mode_obj.eHDRMode) {
        ptIntTimeRange->fMaxIntegrationTime[HDR_LONG_FRAME_IDX] =
            sns_obj->ae_ctrl_param.sns_ae_limit.tIntTimeRange.fMaxIntegrationTime[HDR_LONG_FRAME_IDX];
        ptIntTimeRange->fMinIntegrationTime[HDR_LONG_FRAME_IDX] = 1;

    } else {
        // do nothing
    }

    return SNS_SUCCESS;
}

AX_S32 sc910gs_set_integration_time(ISP_PIPE_ID nPipeId, AX_SNS_AE_SHUTTER_CFG_T *ptIntTime)
{
    AX_U8 ex_h;
    AX_U8 ex_l;
    AX_U32 ex_ival = 0;
    AX_S32 result = 0;
    float ratio = 0.0f;
    AX_U32 nExpLineFromUser = 0;

    SNS_STATE_OBJ *sns_obj = AX_NULL;

    SNS_CHECK_VALUE_RANGE_VALID(nPipeId, 0, AX_VIN_MAX_PIPE_NUM - 1);
    SENSOR_GET_CTX(nPipeId, sns_obj);
    SNS_CHECK_PTR_VALID(sns_obj);
    SNS_CHECK_PTR_VALID(ptIntTime);

    SNS_DBG("Exptime:%d-%d-%d-%d, Hdrratio:%f-%f-%f-%f\n",
            ptIntTime->fIntTime[0], ptIntTime->fIntTime[1], ptIntTime->fIntTime[2], ptIntTime->fIntTime[3],
            ptIntTime->fHdrRatio[0], ptIntTime->fHdrRatio[1], ptIntTime->fHdrRatio[2], ptIntTime->fHdrRatio[3]);

    nExpLineFromUser = ptIntTime->fIntTime[HDR_LONG_FRAME_IDX];

    nExpLineFromUser = AXSNS_CLIP3(nExpLineFromUser,
                                   sns_obj->ae_ctrl_param.sns_ae_limit.tIntTimeRange.fMinIntegrationTime[HDR_LONG_FRAME_IDX],
                                   sns_obj->ae_ctrl_param.sns_ae_limit.tIntTimeRange.fMaxIntegrationTime[HDR_LONG_FRAME_IDX]);

    ex_ival = nExpLineFromUser;
    ex_l = REG_LOW_8BITS(ex_ival);
    ex_h = REG_HIGH_8BITS(ex_ival);
#if 0
    if ((((ex_h & 0xf) << 8) | ex_l) >= 0x02a){
        sc910gs_sns_update_regs_table(nPipeId, SC910GS_REG_331f, 0x02);
        sc910gs_sns_update_regs_table(nPipeId, SC910GS_REG_3385, 0x25);
    } else if ((((ex_h & 0xf) << 8) | ex_l) <= 0x020) {
        sc910gs_sns_update_regs_table(nPipeId, SC910GS_REG_331f, 0x12);
        sc910gs_sns_update_regs_table(nPipeId, SC910GS_REG_3385, 0x1d);
    }
#endif
    sc910gs_sns_update_regs_table(nPipeId, SC910GS_LONG_EXP_LINE_H, (ex_h & 0xf0) >> 4);
    sc910gs_sns_update_regs_table(nPipeId, SC910GS_LONG_EXP_LINE_M, ((ex_h & 0xf) << 4) | ((ex_l & 0xf0) >> 4));
    sc910gs_sns_update_regs_table(nPipeId, SC910GS_LONG_EXP_LINE_L, ((ex_l & 0xf) << 4));

    sns_obj->ae_ctrl_param.sns_ae_param.fCurIntegrationTime[HDR_LONG_FRAME_IDX] = ex_ival;
    /* Do not delete, for its project */
    sns_obj->sztRegsInfo[0].tSnsExpInfo.szExpTime[HDR_LONG_FRAME_IDX] = nExpLineFromUser * sns_obj->ae_ctrl_param.fTimePerLine;

    return SNS_SUCCESS;
}

AX_S32 sc910gs_get_hw_exposure_params(ISP_PIPE_ID nPipeId, AX_SNS_EXP_CTRL_PARAM_T *ptAeCtrlParam)
{
    SNS_STATE_OBJ *sns_obj = AX_NULL;

    SNS_CHECK_PTR_VALID(ptAeCtrlParam);
    SNS_CHECK_VALUE_RANGE_VALID(nPipeId, 0, AX_VIN_MAX_PIPE_NUM - 1);

    SENSOR_GET_CTX(nPipeId, sns_obj);
    SNS_CHECK_PTR_VALID(sns_obj);

    memcpy(ptAeCtrlParam, &sns_obj->ae_ctrl_param, sizeof(AX_SNS_EXP_CTRL_PARAM_T));
    memcpy(&ptAeCtrlParam->sns_dev_attr, &sns_obj->sns_attr_param, sizeof(AX_SNS_ATTR_T));

    return SNS_SUCCESS;
}

AX_S32 sc910gs_set_fps(ISP_PIPE_ID nPipeId, AX_F32 fFps)
{
    AX_S32 result = 0;
    AX_S32 framerate = 30;
    AX_U32 vts = 0;

    SNS_STATE_OBJ *sns_obj = AX_NULL;

    SNS_CHECK_VALUE_RANGE_VALID(nPipeId, 0, AX_VIN_MAX_PIPE_NUM - 1);
    SENSOR_GET_CTX(nPipeId, sns_obj);
    SNS_CHECK_PTR_VALID(sns_obj);

    framerate =  sns_obj->sns_mode_obj.fFrameRate;
    vts = sns_obj->sns_mode_obj.nVts;
    if (fFps >= framerate) {
        sns_sc910gsparams[nPipeId].vts = vts;
    } else {
        sns_sc910gsparams[nPipeId].vts = 1 * SNS_1_SECOND_UNIT_US / (sns_sc910gsparams[nPipeId].line_period * fFps);
    }

    if (sns_sc910gsparams[nPipeId].vts > SC910GS_MAX_VTS){
        sns_sc910gsparams[nPipeId].vts = SC910GS_MAX_VTS;
        fFps = 1 * SNS_1_SECOND_UNIT_US / (sns_sc910gsparams[nPipeId].line_period *sns_sc910gsparams[nPipeId].vts);
        SNS_ERR("Beyond minmum fps  %f\n",fFps);
    }
    result = sc910gs_set_vts(nPipeId, sns_sc910gsparams[nPipeId].vts);
    if (result != 0) {
        SNS_ERR("%s: write vts failed %d \n", __func__, result);
        return result;
    }

    sns_obj->ae_ctrl_param.sns_ae_limit.tIntTimeRange.fMaxIntegrationTime[HDR_LONG_FRAME_IDX] = sns_sc910gsparams[nPipeId].vts / 2 - 4;
    sns_obj->ae_ctrl_param.sns_ae_param.fCurFps = 1 * SNS_1_SECOND_UNIT_US / (sns_sc910gsparams[nPipeId].line_period *sns_sc910gsparams[nPipeId].vts);


    SNS_DBG("nPipeId = %d, fps(from alg) = %f, current vts = 0x%x\n", nPipeId, fFps, sns_sc910gsparams[nPipeId].vts);

    return SNS_SUCCESS;
}


AX_U32 sc910gs_sns_update_regs_table(ISP_PIPE_ID nPipeId, AX_U32 nRegsAddr, AX_U8 nRegsValue)
{
    AX_S32 i = 0;
    AX_U32 nNum = 0;
    SNS_STATE_OBJ *sns_obj = AX_NULL;
    SNS_CHECK_VALUE_RANGE_VALID(nPipeId, 0, AX_VIN_MAX_PIPE_NUM - 1);
    SENSOR_GET_CTX(nPipeId, sns_obj);
    SNS_CHECK_PTR_VALID(sns_obj);

    nNum = sizeof(gsc910gsAeRegsTable) / sizeof(gsc910gsAeRegsTable[0]);
    for (i = 0; i < nNum; i++) {
        if (gsc910gsBusInfo[nPipeId].busType == ISP_SNS_CONNECT_I2C_TYPE) {
            if (nRegsAddr == sns_obj->sztRegsInfo[0].sztData.sztI2cData[i].nRegAddr) {
                sns_obj->sztRegsInfo[0].sztData.sztI2cData[i].nData = nRegsValue;
                gsc910gsAeRegsTable[i].nRegValue = nRegsValue;
                break;
            }
        } else {
            if (nRegsAddr == reg_endian_swap(sns_obj->sztRegsInfo[0].sztData.sztSpiData[i].nRegAddr, SC910GS_ADDR_BYTE)) {
                sns_obj->sztRegsInfo[0].sztData.sztSpiData[i].nData = reg_endian_swap(nRegsValue, SC910GS_DATA_BYTE);
                gsc910gsAeRegsTable[i].nRegValue = nRegsValue;
                break;
            }
        }

    }

    if (nNum <= i) {
        SNS_ERR(" reg addr 0x%x not find.\n", nRegsAddr);
        return SNS_ERR_CODE_INVALID_ADDRESS;
    }

    return SNS_SUCCESS;
}

AX_U32 sc910gs_sns_refresh_all_regs_from_tbl(ISP_PIPE_ID nPipeId)
{
    AX_S32 i = 0;
    AX_U32 nNum = 0;
    SNS_STATE_OBJ *sns_obj = AX_NULL;
    SNS_CHECK_VALUE_RANGE_VALID(nPipeId, 0, AX_VIN_MAX_PIPE_NUM - 1);
    SENSOR_GET_CTX(nPipeId, sns_obj);

    nNum = sizeof(gsc910gsAeRegsTable) / sizeof(gsc910gsAeRegsTable[0]);

    for (i = 0; i < nNum; i++) {
        gsc910gsAeRegsTable[i].nRegValue = sc910gs_reg_read(nPipeId, gsc910gsAeRegsTable[i].nRegAddr);
        if (gsc910gsBusInfo[nPipeId].busType == ISP_SNS_CONNECT_I2C_TYPE) {
            sns_obj->sztRegsInfo[0].sztData.sztI2cData[i].nRegAddr = gsc910gsAeRegsTable[i].nRegAddr;
            sns_obj->sztRegsInfo[0].sztData.sztI2cData[i].nData = gsc910gsAeRegsTable[i].nRegValue;
        } else {
            sns_obj->sztRegsInfo[0].sztData.sztSpiData[i].nRegAddr = reg_endian_swap(gsc910gsAeRegsTable[i].nRegAddr, SC910GS_ADDR_BYTE);
            sns_obj->sztRegsInfo[0].sztData.sztSpiData[i].nData = reg_endian_swap(gsc910gsAeRegsTable[i].nRegValue, SC910GS_DATA_BYTE);
        }
        SNS_DBG(" nRegAddr 0x%x, nRegValue 0x%x\n", gsc910gsAeRegsTable[i].nRegAddr, gsc910gsAeRegsTable[i].nRegValue);
    }

    return SNS_SUCCESS;
}

AX_S32 sc910gs_ae_get_sensor_reg_info_i2c(ISP_PIPE_ID nPipeId, AX_SNS_REGS_CFG_TABLE_T *ptSnsRegsInfo)
{
    AX_S32 i = 0;
    AX_S32 nRet = 0;
    SNS_STATE_OBJ *sns_obj = AX_NULL;
    AX_BOOL bUpdateReg = AX_FALSE;

    SNS_CHECK_PTR_VALID(ptSnsRegsInfo);
    SNS_CHECK_VALUE_RANGE_VALID(nPipeId, 0, AX_VIN_MAX_PIPE_NUM - 1);

    SENSOR_GET_CTX(nPipeId, sns_obj);
    SNS_CHECK_PTR_VALID(sns_obj);

    if ((AX_FALSE == sns_obj->bSyncInit) || (AX_FALSE == ptSnsRegsInfo->bConfig)) {
        /* sync config */
        SNS_DBG(" bSyncInit %d, bConfig %d\n", sns_obj->bSyncInit, ptSnsRegsInfo->bConfig);
        sns_obj->sztRegsInfo[0].eSnsType = ISP_SNS_CONNECT_I2C_TYPE;
        sns_obj->sztRegsInfo[0].tComBus.I2cDev = sc910gs_get_bus_num(nPipeId);
        sns_obj->sztRegsInfo[0].nRegNum = sizeof(gsc910gsAeRegsTable) / sizeof(gsc910gsAeRegsTable[0]);
        sns_obj->sztRegsInfo[0].nCfg2ValidDelayMax = 1;

        for (i = 0; i < sns_obj->sztRegsInfo[0].nRegNum; i++) {
            sns_obj->sztRegsInfo[0].sztData.sztI2cData[i].bUpdate = AX_TRUE;
            sns_obj->sztRegsInfo[0].sztData.sztI2cData[i].nDevAddr = SC910GS_SLAVE_ADDR;
            sns_obj->sztRegsInfo[0].sztData.sztI2cData[i].nRegAddr = gsc910gsAeRegsTable[i].nRegAddr;
            sns_obj->sztRegsInfo[0].sztData.sztI2cData[i].nAddrByteNum = SC910GS_ADDR_BYTE;
            sns_obj->sztRegsInfo[0].sztData.sztI2cData[i].nData = gsc910gsAeRegsTable[i].nRegValue;
            sns_obj->sztRegsInfo[0].sztData.sztI2cData[i].nDataByteNum = SC910GS_DATA_BYTE;
            sns_obj->sztRegsInfo[0].sztData.sztI2cData[i].nDelayFrmNum = gsc910gsAeRegsTable[i].nDelayFrmNum;
            SNS_DBG("pipe %d, [%2d] nRegAddr 0x%x, nRegValue 0x%x\n", nPipeId, i,
                    gsc910gsAeRegsTable[i].nRegAddr, gsc910gsAeRegsTable[i].nRegValue);
        }

        bUpdateReg = AX_TRUE;
        sns_obj->bSyncInit = AX_TRUE;

    } else {
        for (i = 0; i < sns_obj->sztRegsInfo[0].nRegNum; i++) {
            if (sns_obj->sztRegsInfo[0].sztData.sztI2cData[i].nData == sns_obj->sztRegsInfo[1].sztData.sztI2cData[i].nData) {
                sns_obj->sztRegsInfo[0].sztData.sztI2cData[i].bUpdate = AX_FALSE;
            } else {
                sns_obj->sztRegsInfo[0].sztData.sztI2cData[i].bUpdate = AX_TRUE;
                bUpdateReg = AX_TRUE;
                SNS_DBG("pipe %d, [%2d] nRegAddr 0x%x, nRegValue 0x%x\n", nPipeId, i,
                        sns_obj->sztRegsInfo[0].sztData.sztI2cData[i].nRegAddr, sns_obj->sztRegsInfo[0].sztData.sztI2cData[i].nData);
            }
        }
    }

    if (AX_TRUE == bUpdateReg) {
        sns_obj->sztRegsInfo[0].bConfig = AX_FALSE;
    } else {
        sns_obj->sztRegsInfo[0].bConfig = AX_TRUE;
    }

    memcpy(ptSnsRegsInfo, &sns_obj->sztRegsInfo[0], sizeof(AX_SNS_REGS_CFG_TABLE_T));
    /* Save the current register table */
    memcpy(&sns_obj->sztRegsInfo[1], &sns_obj->sztRegsInfo[0], sizeof(AX_SNS_REGS_CFG_TABLE_T));

    return nRet;
}

AX_S32 sc910gs_ae_get_sensor_reg_info_spi(ISP_PIPE_ID nPipeId, AX_SNS_REGS_CFG_TABLE_T *ptSnsRegsInfo)
{
    AX_S32 i = 0;
    AX_S32 nRet = 0;
    SNS_STATE_OBJ *sns_obj = AX_NULL;
    AX_BOOL bUpdateReg = AX_FALSE;

    SNS_CHECK_PTR_VALID(ptSnsRegsInfo);
    SNS_CHECK_VALUE_RANGE_VALID(nPipeId, 0, AX_VIN_MAX_PIPE_NUM - 1);

    SENSOR_GET_CTX(nPipeId, sns_obj);
    SNS_CHECK_PTR_VALID(sns_obj);

    if ((AX_FALSE == sns_obj->bSyncInit) || (AX_FALSE == ptSnsRegsInfo->bConfig)) {
        /* sync config */
        SNS_DBG(" bSyncInit %d, bConfig %d\n", sns_obj->bSyncInit, ptSnsRegsInfo->bConfig);
        sns_obj->sztRegsInfo[0].eSnsType = ISP_SNS_CONNECT_SPI_TYPE;
        sns_obj->sztRegsInfo[0].tComBus.SpiDev.bit4SpiDev = sns_obj->sns_spi_obj.sns_spi_bnum;
        sns_obj->sztRegsInfo[0].tComBus.SpiDev.bit4SpiCs = sns_obj->sns_spi_obj.sns_spi_cs;
        sns_obj->sztRegsInfo[0].nRegNum = sizeof(gsc910gsAeRegsTable) / sizeof(gsc910gsAeRegsTable[0]);
        sns_obj->sztRegsInfo[0].nCfg2ValidDelayMax = 1;

        for (i = 0; i < sns_obj->sztRegsInfo[0].nRegNum; i++) {
            sns_obj->sztRegsInfo[0].sztData.sztSpiData[i].bUpdate = AX_TRUE;
            sns_obj->sztRegsInfo[0].sztData.sztSpiData[i].nDevAddr = reg_endian_swap((SC910GS_DEVICEID << 1) | SC910GS_SPI_WIRTE, SC910GS_DEVADDR_BYTE);
            sns_obj->sztRegsInfo[0].sztData.sztSpiData[i].nDevAddrByteNum = SC910GS_DEVADDR_BYTE;
            sns_obj->sztRegsInfo[0].sztData.sztSpiData[i].nRegAddr = reg_endian_swap(gsc910gsAeRegsTable[i].nRegAddr, SC910GS_ADDR_BYTE);
            sns_obj->sztRegsInfo[0].sztData.sztSpiData[i].nRegAddrByteNum = SC910GS_ADDR_BYTE;
            sns_obj->sztRegsInfo[0].sztData.sztSpiData[i].nData = reg_endian_swap(gsc910gsAeRegsTable[i].nRegValue, SC910GS_DATA_BYTE);
            sns_obj->sztRegsInfo[0].sztData.sztSpiData[i].nDataByteNum = SC910GS_DATA_BYTE;
            sns_obj->sztRegsInfo[0].sztData.sztSpiData[i].nDelayFrmNum = gsc910gsAeRegsTable[i].nDelayFrmNum;
        }

        bUpdateReg = AX_TRUE;
        sns_obj->bSyncInit = AX_TRUE;

    } else {
        for (i = 0; i < sns_obj->sztRegsInfo[0].nRegNum; i++) {
            if (sns_obj->sztRegsInfo[0].sztData.sztSpiData[i].nData == sns_obj->sztRegsInfo[1].sztData.sztSpiData[i].nData) {
                sns_obj->sztRegsInfo[0].sztData.sztSpiData[i].bUpdate = AX_FALSE;
            } else {
                sns_obj->sztRegsInfo[0].sztData.sztSpiData[i].bUpdate = AX_TRUE;
                bUpdateReg = AX_TRUE;
                SNS_DBG("pipe %d, [%2d] nRegAddr 0x%x, nRegValue 0x%x\n", nPipeId, i,
                        sns_obj->sztRegsInfo[0].sztData.sztSpiData[i].nRegAddr, sns_obj->sztRegsInfo[0].sztData.sztSpiData[i].nData);
            }
        }
    }

    if (AX_TRUE == bUpdateReg) {
        sns_obj->sztRegsInfo[0].bConfig = AX_FALSE;
    } else {
        sns_obj->sztRegsInfo[0].bConfig = AX_TRUE;
    }

    memcpy(ptSnsRegsInfo, &sns_obj->sztRegsInfo[0], sizeof(AX_SNS_REGS_CFG_TABLE_T));
    /* Save the current register table */
    memcpy(&sns_obj->sztRegsInfo[1], &sns_obj->sztRegsInfo[0], sizeof(AX_SNS_REGS_CFG_TABLE_T));

    return nRet;
}

AX_S32 sc910gs_ae_get_sensor_reg_info(ISP_PIPE_ID nPipeId, AX_SNS_REGS_CFG_TABLE_T *ptSnsRegsInfo)
{
    AX_S32 nRet = 0;
    SNS_STATE_OBJ *sns_obj = AX_NULL;
    AX_BOOL bUpdateReg = AX_FALSE;

    SNS_CHECK_VALUE_RANGE_VALID(nPipeId, 0, AX_VIN_MAX_PIPE_NUM - 1);

    SENSOR_GET_CTX(nPipeId, sns_obj);
    SNS_CHECK_PTR_VALID(sns_obj);

    if (gsc910gsBusInfo[nPipeId].busType == ISP_SNS_CONNECT_I2C_TYPE) {
        nRet = sc910gs_ae_get_sensor_reg_info_i2c(nPipeId, ptSnsRegsInfo);
    } else {
        nRet = sc910gs_ae_get_sensor_reg_info_spi(nPipeId, ptSnsRegsInfo);
    }

    return nRet;
}


