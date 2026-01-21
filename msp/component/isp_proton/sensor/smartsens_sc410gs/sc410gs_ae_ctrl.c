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

#include "sc410gs_reg.h"
#include "sc410gs_ae_ctrl.h"


extern SNS_STATE_OBJ *g_szsc410gsCtx[AX_VIN_MAX_PIPE_NUM];
extern AX_SNS_COMMBUS_T gsc410gsBusInfo[AX_VIN_MAX_PIPE_NUM];
#define SENSOR_GET_CTX(dev, pstCtx) (pstCtx = g_szsc410gsCtx[dev])

static SNSSC410GS0_OBJ_T gSc410gsParams[AX_VIN_MAX_PIPE_NUM];
static AX_F32 gAgainTable[SENSOR_MAX_GAIN_STEP];
static AX_F32 gDgainTable[SENSOR_MAX_GAIN_STEP];

/*user config*/
static AX_F32 gFpsGear[] = {1.00, 2.00, 3.00, 4.00, 5.00, 6.00, 7.00, 8.00, 9.00, 10.00,
                            11.00, 12.00, 13.00, 14.00, 15.00, 16.00, 17.00, 18.00, 19.00, 20.00,
                            21.00, 22.00, 23.00, 24.00, 25.00, 26.00, 27.00, 28.00, 29.00, 30.00,
                            31.00, 32.00, 33.00, 34.00, 35.00, 36.00, 37.00, 38.00, 39.00, 40.00,
                            41.00, 42.00, 43.00, 44.00, 45.00, 46.00, 47.00, 48.00, 49.00, 50.00,
                            51.00, 52.00, 53.00, 54.00, 55.00, 56.00, 57.00, 58.00, 59.00, 60.00
                           };


static AX_SNS_DRV_DELAY_TABLE_T gSc410gsAeRegsTable[] = {
        /* regs index */          /* regs addr */    /*regs value*/ /* Delay Num */
    {SC410GS_LONG_EXPLINE_H_IDX, SC410GS_LONG_EXPLINE_H,    0,          0},
    {SC410GS_LONG_EXPLINE_M_IDX, SC410GS_LONG_EXPLINE_M,    0,          0},
    {SC410GS_LONG_EXPLINE_L_IDX, SC410GS_LONG_EXPLINE_L,    0,          0},
    {SC410GS_LONG_AGAIN_H_IDX,   SC410GS_LONG_AGAIN_H,      0,          0},
    {SC410GS_LONG_AGAIN_L_IDX,   SC410GS_LONG_AGAIN_L,      0,          0},
    {SC410GS_LONG_DGAIN_H_IDX,   SC410GS_LONG_DGAIN_H,      0,          0},
    {SC410GS_LONG_DGAIN_L_IDX,   SC410GS_LONG_DGAIN_L,      0,          0},
    {SC410GS_VTS_H_IDX,          SC410GS_VTS_H,             0,          0},
    {SC410GS_VTS_L_IDX,          SC410GS_VTS_L,             0,          0},
};


const SC410GS_GAIN_TABLE_T sc410gs_again_table[] = {
    {1.000,  0x03, 0x40},
    {1.016,  0x03, 0x41},
    {1.031,  0x03, 0x42},
    {1.047,  0x03, 0x43},
    {1.063,  0x03, 0x44},
    {1.078,  0x03, 0x45},
    {1.094,  0x03, 0x46},
    {1.109,  0x03, 0x47},
    {1.125,  0x03, 0x48},
    {1.141,  0x03, 0x49},
    {1.156,  0x03, 0x4A},
    {1.172,  0x03, 0x4B},
    {1.188,  0x03, 0x4C},
    {1.203,  0x03, 0x4D},
    {1.219,  0x03, 0x4E},
    {1.234,  0x03, 0x4F},
    {1.250,  0x03, 0x50},
    {1.266,  0x03, 0x51},
    {1.281,  0x03, 0x52},
    {1.297,  0x03, 0x53},
    {1.313,  0x03, 0x54},
    {1.328,  0x03, 0x55},
    {1.344,  0x03, 0x56},
    {1.359,  0x03, 0x57},
    {1.375,  0x03, 0x58},
    {1.391,  0x03, 0x59},
    {1.406,  0x03, 0x5A},
    {1.422,  0x03, 0x5B},
    {1.438,  0x03, 0x5C},
    {1.453,  0x03, 0x5D},
    {1.469,  0x03, 0x5E},
    {1.484,  0x03, 0x5F},
    {1.500,  0x03, 0x60},
    {1.516,  0x03, 0x61},
    {1.531,  0x03, 0x62},
    {1.547,  0x03, 0x63},
    {1.563,  0x03, 0x64},
    {1.578,  0x03, 0x65},
    {1.594,  0x03, 0x66},
    {1.609,  0x03, 0x67},
    {1.625,  0x03, 0x68},
    {1.641,  0x03, 0x69},
    {1.656,  0x03, 0x6A},
    {1.672,  0x03, 0x6B},
    {1.688,  0x03, 0x6C},
    {1.700,  0x23, 0x40},
    {1.727,  0x23, 0x41},
    {1.753,  0x23, 0x42},
    {1.780,  0x23, 0x43},
    {1.806,  0x23, 0x44},
    {1.833,  0x23, 0x45},
    {1.859,  0x23, 0x46},
    {1.886,  0x23, 0x47},
    {1.913,  0x23, 0x48},
    {1.939,  0x23, 0x49},
    {1.966,  0x23, 0x4A},
    {1.992,  0x23, 0x4B},
    {2.019,  0x23, 0x4C},
    {2.045,  0x23, 0x4D},
    {2.072,  0x23, 0x4E},
    {2.098,  0x23, 0x4F},
    {2.125,  0x23, 0x50},
    {2.152,  0x23, 0x51},
    {2.178,  0x23, 0x52},
    {2.205,  0x23, 0x53},
    {2.231,  0x23, 0x54},
    {2.258,  0x23, 0x55},
    {2.284,  0x23, 0x56},
    {2.311,  0x23, 0x57},
    {2.338,  0x23, 0x58},
    {2.364,  0x23, 0x59},
    {2.391,  0x23, 0x5A},
    {2.417,  0x23, 0x5B},
    {2.444,  0x23, 0x5C},
    {2.470,  0x23, 0x5D},
    {2.497,  0x23, 0x5E},
    {2.523,  0x23, 0x5F},
    {2.550,  0x23, 0x60},
    {2.577,  0x23, 0x61},
    {2.603,  0x23, 0x62},
    {2.630,  0x23, 0x63},
    {2.656,  0x23, 0x64},
    {2.683,  0x23, 0x65},
    {2.709,  0x23, 0x66},
    {2.736,  0x23, 0x67},
    {2.763,  0x23, 0x68},
    {2.789,  0x23, 0x69},
    {2.816,  0x23, 0x6A},
    {2.842,  0x23, 0x6B},
    {2.869,  0x23, 0x6C},
    {2.895,  0x23, 0x6D},
    {2.922,  0x23, 0x6E},
    {2.948,  0x23, 0x6F},
    {2.975,  0x23, 0x70},
    {3.002,  0x23, 0x71},
    {3.028,  0x23, 0x72},
    {3.055,  0x23, 0x73},
    {3.081,  0x23, 0x74},
    {3.108,  0x23, 0x75},
    {3.134,  0x23, 0x76},
    {3.161,  0x23, 0x77},
    {3.188,  0x23, 0x78},
    {3.214,  0x23, 0x79},
    {3.241,  0x23, 0x7A},
    {3.267,  0x23, 0x7B},
    {3.294,  0x23, 0x7C},
    {3.320,  0x23, 0x7D},
    {3.347,  0x23, 0x7E},
    {3.373,  0x23, 0x7F},
    {3.400,  0x27, 0x40},
    {3.453,  0x27, 0x41},
    {3.506,  0x27, 0x42},
    {3.559,  0x27, 0x43},
    {3.613,  0x27, 0x44},
    {3.666,  0x27, 0x45},
    {3.719,  0x27, 0x46},
    {3.772,  0x27, 0x47},
    {3.825,  0x27, 0x48},
    {3.878,  0x27, 0x49},
    {3.931,  0x27, 0x4A},
    {3.984,  0x27, 0x4B},
    {4.038,  0x27, 0x4C},
    {4.091,  0x27, 0x4D},
    {4.144,  0x27, 0x4E},
    {4.197,  0x27, 0x4F},
    {4.250,  0x27, 0x50},
    {4.303,  0x27, 0x51},
    {4.356,  0x27, 0x52},
    {4.409,  0x27, 0x53},
    {4.463,  0x27, 0x54},
    {4.516,  0x27, 0x55},
    {4.569,  0x27, 0x56},
    {4.622,  0x27, 0x57},
    {4.675,  0x27, 0x58},
    {4.728,  0x27, 0x59},
    {4.781,  0x27, 0x5A},
    {4.834,  0x27, 0x5B},
    {4.888,  0x27, 0x5C},
    {4.941,  0x27, 0x5D},
    {4.994,  0x27, 0x5E},
    {5.047,  0x27, 0x5F},
    {5.100,  0x27, 0x60},
    {5.153,  0x27, 0x61},
    {5.206,  0x27, 0x62},
    {5.259,  0x27, 0x63},
    {5.313,  0x27, 0x64},
    {5.366,  0x27, 0x65},
    {5.419,  0x27, 0x66},
    {5.472,  0x27, 0x67},
    {5.525,  0x27, 0x68},
    {5.578,  0x27, 0x69},
    {5.631,  0x27, 0x6A},
    {5.684,  0x27, 0x6B},
    {5.738,  0x27, 0x6C},
    {5.791,  0x27, 0x6D},
    {5.844,  0x27, 0x6E},
    {5.897,  0x27, 0x6F},
    {5.950,  0x27, 0x70},
    {6.003,  0x27, 0x71},
    {6.056,  0x27, 0x72},
    {6.109,  0x27, 0x73},
    {6.163,  0x27, 0x74},
    {6.216,  0x27, 0x75},
    {6.269,  0x27, 0x76},
    {6.322,  0x27, 0x77},
    {6.375,  0x27, 0x78},
    {6.428,  0x27, 0x79},
    {6.481,  0x27, 0x7A},
    {6.534,  0x27, 0x7B},
    {6.588,  0x27, 0x7C},
    {6.641,  0x27, 0x7D},
    {6.694,  0x27, 0x7E},
    {6.747,  0x27, 0x7F},
    {6.800,  0x2F, 0x40},
    {6.906,  0x2F, 0x41},
    {7.013,  0x2F, 0x42},
    {7.119,  0x2F, 0x43},
    {7.225,  0x2F, 0x44},
    {7.331,  0x2F, 0x45},
    {7.438,  0x2F, 0x46},
    {7.544,  0x2F, 0x47},
    {7.650,  0x2F, 0x48},
    {7.756,  0x2F, 0x49},
    {7.863,  0x2F, 0x4A},
    {7.969,  0x2F, 0x4B},
    {8.075,  0x2F, 0x4C},
    {8.181,  0x2F, 0x4D},
    {8.288,  0x2F, 0x4E},
    {8.394,  0x2F, 0x4F},
    {8.500,  0x2F, 0x50},
    {8.606,  0x2F, 0x51},
    {8.713,  0x2F, 0x52},
    {8.819,  0x2F, 0x53},
    {8.925,  0x2F, 0x54},
    {9.031,  0x2F, 0x55},
    {9.138,  0x2F, 0x56},
    {9.244,  0x2F, 0x57},
    {9.350,  0x2F, 0x58},
    {9.456,  0x2F, 0x59},
    {9.563,  0x2F, 0x5A},
    {9.669,  0x2F, 0x5B},
    {9.775,  0x2F, 0x5C},
    {9.881,  0x2F, 0x5D},
    {9.988,  0x2F, 0x5E},
    {10.094, 0x2F, 0x5F},
    {10.200, 0x2F, 0x60},
    {10.306, 0x2F, 0x61},
    {10.413, 0x2F, 0x62},
    {10.519, 0x2F, 0x63},
    {10.625, 0x2F, 0x64},
    {10.731, 0x2F, 0x65},
    {10.838, 0x2F, 0x66},
    {10.944, 0x2F, 0x67},
    {11.050, 0x2F, 0x68},
    {11.156, 0x2F, 0x69},
    {11.263, 0x2F, 0x6A},
    {11.369, 0x2F, 0x6B},
    {11.475, 0x2F, 0x6C},
    {11.581, 0x2F, 0x6D},
    {11.688, 0x2F, 0x6E},
    {11.794, 0x2F, 0x6F},
    {11.900, 0x2F, 0x70},
    {12.006, 0x2F, 0x71},
    {12.113, 0x2F, 0x72},
    {12.219, 0x2F, 0x73},
    {12.325, 0x2F, 0x74},
    {12.431, 0x2F, 0x75},
    {12.538, 0x2F, 0x76},
    {12.644, 0x2F, 0x77},
    {12.750, 0x2F, 0x78},
    {12.856, 0x2F, 0x79},
    {12.963, 0x2F, 0x7A},
    {13.069, 0x2F, 0x7B},
    {13.175, 0x2F, 0x7C},
    {13.281, 0x2F, 0x7D},
    {13.388, 0x2F, 0x7E},
    {13.494, 0x2F, 0x7F},
    {13.600, 0x3F, 0x40},
    {13.813, 0x3F, 0x41},
    {14.025, 0x3F, 0x42},
    {14.238, 0x3F, 0x43},
    {14.450, 0x3F, 0x44},
    {14.663, 0x3F, 0x45},
    {14.875, 0x3F, 0x46},
    {15.088, 0x3F, 0x47},
    {15.300, 0x3F, 0x48},
    {15.513, 0x3F, 0x49},
    {15.725, 0x3F, 0x4A},
    {15.938, 0x3F, 0x4B},
    {16.150, 0x3F, 0x4C},
    {16.363, 0x3F, 0x4D},
    {16.575, 0x3F, 0x4E},
    {16.788, 0x3F, 0x4F},
    {17.000, 0x3F, 0x50},
    {17.213, 0x3F, 0x51},
    {17.425, 0x3F, 0x52},
    {17.638, 0x3F, 0x53},
    {17.850, 0x3F, 0x54},
    {18.063, 0x3F, 0x55},
    {18.275, 0x3F, 0x56},
    {18.488, 0x3F, 0x57},
    {18.700, 0x3F, 0x58},
    {18.913, 0x3F, 0x59},
    {19.125, 0x3F, 0x5A},
    {19.338, 0x3F, 0x5B},
    {19.550, 0x3F, 0x5C},
    {19.763, 0x3F, 0x5D},
    {19.975, 0x3F, 0x5E},
    {20.188, 0x3F, 0x5F},
    {20.400, 0x3F, 0x60},
    {20.613, 0x3F, 0x61},
    {20.825, 0x3F, 0x62},
    {21.038, 0x3F, 0x63},
    {21.250, 0x3F, 0x64},
    {21.463, 0x3F, 0x65},
    {21.675, 0x3F, 0x66},
    {21.888, 0x3F, 0x67},
    {22.100, 0x3F, 0x68},
    {22.313, 0x3F, 0x69},
    {22.525, 0x3F, 0x6A},
    {22.738, 0x3F, 0x6B},
    {22.950, 0x3F, 0x6C},
    {23.163, 0x3F, 0x6D},
    {23.375, 0x3F, 0x6E},
    {23.588, 0x3F, 0x6F},
    {23.800, 0x3F, 0x70},
    {24.013, 0x3F, 0x71},
    {24.225, 0x3F, 0x72},
    {24.438, 0x3F, 0x73},
    {24.650, 0x3F, 0x74},
    {24.863, 0x3F, 0x75},
    {25.075, 0x3F, 0x76},
    {25.288, 0x3F, 0x77},
    {25.500, 0x3F, 0x78},
    {25.713, 0x3F, 0x79},
    {25.925, 0x3F, 0x7A},
    {26.138, 0x3F, 0x7B},
    {26.350, 0x3F, 0x7C},
    {26.563, 0x3F, 0x7D},
    {26.775, 0x3F, 0x7E},
    {26.988, 0x3F, 0x7F},
};

const SC410GS_GAIN_TABLE_T sc410gs_dgain_table[] = {
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

static AX_F32 sc410gs_again2value(float gain, AX_U8 *again_in, AX_U8 *again_de)
{
    AX_U32 i;
    AX_U32 count;
    AX_U32 ret_value = 0;

    if (!again_in || !again_de)
        return -1;

    count = sizeof(sc410gs_again_table) / sizeof(sc410gs_again_table[0]);

    for (i = 0; i < count; i++) {
        if (AXSNS_CAMPARE_FLOAT(gain, sc410gs_again_table[i].gain)) {
            continue;
        } else {
            *again_in = sc410gs_again_table[i].gain_in;
            *again_de = sc410gs_again_table[i].gain_de;

            SNS_DBG("again=%f, again_in=0x%x, again_de=0x%x\n", gain, *again_in, *again_de);

            return sc410gs_again_table[i].gain;
        }
    }

    return -1;
}

static AX_F32 sc410gs_dgain2value(float gain, AX_U8 *dgain_in, AX_U8 *dgain_de)
{
    AX_U32 i;
    AX_U32 count;
    AX_U32 ret_value = 0;

    if (!dgain_in || !dgain_de)
        return -1;

    count = sizeof(sc410gs_dgain_table) / sizeof(sc410gs_dgain_table[0]);

    for (i = 0; i < count; i++) {
        if (AXSNS_CAMPARE_FLOAT(gain, sc410gs_dgain_table[i].gain)) {
            continue;
        } else {
            *dgain_in = sc410gs_dgain_table[i].gain_in;
            *dgain_de = sc410gs_dgain_table[i].gain_de;

            SNS_DBG("dgain=%f, dgain_in=0x%x, dgain_de=0x%x\n", gain, *dgain_in, *dgain_de);

            return sc410gs_dgain_table[i].gain;
        }
    }

    return -1;
}

AX_U32 sc410gs_set_vts(ISP_PIPE_ID nPipeId, AX_U32 vts)
{
    AX_U8 vts_h = 0;
    AX_U8 vts_l = 0;
    AX_S32 result = 0;

    SNS_CHECK_VALUE_RANGE_VALID(nPipeId, 0, AX_VIN_MAX_PIPE_NUM - 1);

    vts_h = (vts >> 8) & 0xff;
    vts_l = vts & 0xff;

    result |= sc410gs_update_regidx_table(nPipeId, SC410GS_VTS_H_IDX, vts_h);
    result |= sc410gs_update_regidx_table(nPipeId, SC410GS_VTS_L_IDX, vts_l);

    return result;
}

AX_U32 sc410gs_get_vts(ISP_PIPE_ID nPipeId)
{
    AX_U8 vts_h = 0;
    AX_U8 vts_l = 0;
    AX_U32 vts = 0;

    vts_h = sc410gs_reg_read(nPipeId, SC410GS_VTS_H);
    vts_l = sc410gs_reg_read(nPipeId, SC410GS_VTS_L);

    vts = vts_h << 8 | vts_l;

    return vts;
}

/****************************************************************************
 * exposure control external function
 ****************************************************************************/
 AX_S32 sc410gs_get_params_from_setting(ISP_PIPE_ID nPipeId)
{
    AX_S32 ret = 0;
    AX_U32 reg_cnt = 0;
    camera_i2c_reg_array *setting = AX_NULL;
    AX_U32 vts = 0;

    ret = sc410gs_select_setting(nPipeId, &setting, &reg_cnt);
    if(ret) {
        return ret;
    }

    ret = sc410gs_get_vts_from_setting(nPipeId, setting, reg_cnt, &vts);
    if(ret) {
        return ret;
    }

    gSc410gsParams[nPipeId].vts = vts;

    SNS_DBG("pipe:%d, get setting params vts:0x%x\n", nPipeId, gSc410gsParams[nPipeId].vts);

    return SNS_SUCCESS;
}

AX_S32 sc410gs_cfg_aec_param(ISP_PIPE_ID nPipeId)
{
    SNS_STATE_OBJ *sns_obj = AX_NULL;

    SNS_CHECK_VALUE_RANGE_VALID(nPipeId, 0, AX_VIN_MAX_PIPE_NUM - 1);
    SENSOR_GET_CTX(nPipeId, sns_obj);
    SNS_CHECK_PTR_VALID(sns_obj);

    sc410gs_get_params_from_setting(nPipeId);
    sns_obj->sns_mode_obj.nVts = gSc410gsParams[nPipeId].vts;
    gSc410gsParams[nPipeId].setting_vts = sns_obj->sns_mode_obj.nVts;
    gSc410gsParams[nPipeId].setting_fps = sns_obj->sns_mode_obj.fFrameRate;

    sns_obj->ae_ctrl_param.fTimePerLine =
        SNS_1_SECOND_UNIT_US / (sns_obj->sns_mode_obj.nVts * sns_obj->sns_mode_obj.fFrameRate);

    SNS_INFO("pipe:%d, setting fps:%f, vts:0x%x, fTimePerLine:%f\n", nPipeId,
        sns_obj->sns_mode_obj.fFrameRate, sns_obj->sns_mode_obj.nVts, sns_obj->ae_ctrl_param.fTimePerLine);

    sns_obj->ae_ctrl_param.sns_ae_limit.fMinRatio = SC410GS_MIN_RATIO;
    sns_obj->ae_ctrl_param.sns_ae_limit.fMaxRatio = SC410GS_MAX_RATIO;

    /* sensor exp limit */
    sns_obj->ae_ctrl_param.sns_ae_limit.tIntTimeRange.fMinIntegrationTime[HDR_LONG_FRAME_IDX] = 0;
    sns_obj->ae_ctrl_param.sns_ae_limit.tIntTimeRange.fMaxIntegrationTime[HDR_LONG_FRAME_IDX] = gSc410gsParams[nPipeId].vts - 8;
    sns_obj->ae_ctrl_param.sns_ae_param.fIntegrationTimeIncrement[HDR_LONG_FRAME_IDX] = 1;

    /* sensor again  limit*/
    sns_obj->ae_ctrl_param.sns_ae_limit.fMinAgain[HDR_LONG_FRAME_IDX] = SC410GS_MIN_AGAIN;
    sns_obj->ae_ctrl_param.sns_ae_limit.fMaxAgain[HDR_LONG_FRAME_IDX] = SC410GS_MAX_AGAIN;
    sns_obj->ae_ctrl_param.sns_ae_param.fAGainIncrement[HDR_LONG_FRAME_IDX] = (AX_F32)1/64;

    /* sensor dgain  limit*/
    sns_obj->ae_ctrl_param.sns_ae_limit.fMinDgain[HDR_LONG_FRAME_IDX] = SC410GS_MIN_DGAIN;
    sns_obj->ae_ctrl_param.sns_ae_limit.fMaxDgain[HDR_LONG_FRAME_IDX] = SC410GS_MAX_DGAIN;
    sns_obj->ae_ctrl_param.sns_ae_param.fDGainIncrement[HDR_LONG_FRAME_IDX] = (AX_F32)1/64;

    /* sensor hcglcg */
    sns_obj->ae_ctrl_param.fSnsHcgLcgRatio = 1.0f;
    sns_obj->ae_ctrl_param.eSnsHcgLcgMode = AX_LCG_NOTSUPPORT_MODE;

    /* cur fps */
    sns_obj->ae_ctrl_param.sns_ae_param.fCurFps = sns_obj->sns_mode_obj.fFrameRate;

    return SNS_SUCCESS;
}


AX_S32 sc410gs_get_gain_table(ISP_PIPE_ID nPipeId, AX_SNS_AE_GAIN_TABLE_T *ptParams)
{
    AX_U32 i = 0;

    SNS_CHECK_PTR_VALID(ptParams);
    SNS_CHECK_VALUE_RANGE_VALID(nPipeId, 0, AX_VIN_MAX_PIPE_NUM - 1);

    ptParams->nAgainTableSize = sizeof(sc410gs_again_table) / sizeof(sc410gs_again_table[0]);
    ptParams->nDgainTableSize = sizeof(sc410gs_dgain_table) / sizeof(sc410gs_dgain_table[0]);

    for (i = 0; i < ptParams->nAgainTableSize; i++) {
        gAgainTable[i] = sc410gs_again_table[i].gain;
    }

    for (i = 0; i < ptParams->nDgainTableSize; i++) {
        gDgainTable[i] = sc410gs_dgain_table[i].gain;
    }

    ptParams->pAgainTable = gAgainTable;
    ptParams->pDgainTable = gDgainTable;

    return SNS_SUCCESS;
}


AX_S32 sc410gs_set_again(ISP_PIPE_ID nPipeId, AX_SNS_AE_GAIN_CFG_T *ptAGain)
{
    AX_U8 nGainIn;
    AX_U8 nGainDe;
    AX_F32 fGainValue = 0;
    AX_F32 fGainFromUser = 0;

    SNS_STATE_OBJ *sns_obj = AX_NULL;

    SNS_CHECK_VALUE_RANGE_VALID(nPipeId, 0, AX_VIN_MAX_PIPE_NUM - 1);
    SENSOR_GET_CTX(nPipeId, sns_obj);
    SNS_CHECK_PTR_VALID(sns_obj);
    SNS_CHECK_PTR_VALID(ptAGain);

    /* long gain seting */
    fGainFromUser = ptAGain->fGain[HDR_LONG_FRAME_IDX];
    fGainFromUser = AXSNS_CLIP3(fGainFromUser, sns_obj->ae_ctrl_param.sns_ae_limit.fMinAgain[HDR_LONG_FRAME_IDX],
                                sns_obj->ae_ctrl_param.sns_ae_limit.fMaxAgain[HDR_LONG_FRAME_IDX]);

    fGainValue = sc410gs_again2value(fGainFromUser, &nGainIn, &nGainDe);
    if (fGainValue == -1) {
        SNS_ERR("userAgain:%f match failed\n", fGainFromUser);
        return -1;
    }

    sc410gs_update_regidx_table(nPipeId, SC410GS_LONG_AGAIN_H_IDX, nGainIn);
    sc410gs_update_regidx_table(nPipeId, SC410GS_LONG_AGAIN_L_IDX, nGainDe);

    sns_obj->ae_ctrl_param.sns_ae_param.fCurAGain[HDR_LONG_FRAME_IDX] = fGainValue;
    /* Do not delete, for its project */
    sns_obj->sztRegsInfo[0].tSnsExpInfo.szCurAGain[HDR_LONG_FRAME_IDX] = fGainValue;

    SNS_INFO("pipe:%d, userAgain:%f, curAgain:%f\n",
            nPipeId, ptAGain->fGain[HDR_LONG_FRAME_IDX],
            sns_obj->ae_ctrl_param.sns_ae_param.fCurAGain[HDR_LONG_FRAME_IDX]);

    return SNS_SUCCESS;
}

AX_S32 sc410gs_set_dgain(ISP_PIPE_ID nPipeId, AX_SNS_AE_GAIN_CFG_T *ptDGain)
{
    AX_U8 nGainIn;
    AX_U8 nGainDe;
    AX_F32 fGainValue = 0;
    AX_F32 fGainFromUser = 0;
    SNS_STATE_OBJ *sns_obj = AX_NULL;

    SNS_CHECK_VALUE_RANGE_VALID(nPipeId, 0, AX_VIN_MAX_PIPE_NUM - 1);
    SENSOR_GET_CTX(nPipeId, sns_obj);
    SNS_CHECK_PTR_VALID(sns_obj);
    SNS_CHECK_PTR_VALID(ptDGain);

    /* long frame digital gain seting */
    fGainFromUser = ptDGain->fGain[HDR_LONG_FRAME_IDX];
    fGainFromUser = AXSNS_CLIP3(fGainFromUser, sns_obj->ae_ctrl_param.sns_ae_limit.fMinDgain[HDR_LONG_FRAME_IDX],
                                sns_obj->ae_ctrl_param.sns_ae_limit.fMaxDgain[HDR_LONG_FRAME_IDX]);

    fGainValue = sc410gs_dgain2value(fGainFromUser, &nGainIn, &nGainDe);
    if (fGainValue == -1) {
        SNS_ERR("userDgain:%f match failed\n", fGainFromUser);
        return -1;
    }

    sc410gs_update_regidx_table(nPipeId, SC410GS_LONG_DGAIN_H_IDX, nGainIn);
    sc410gs_update_regidx_table(nPipeId, SC410GS_LONG_DGAIN_L_IDX, nGainDe);

    sns_obj->ae_ctrl_param.sns_ae_param.fCurDGain[HDR_LONG_FRAME_IDX] = fGainValue;
    /* Do not delete, for its project */
    sns_obj->sztRegsInfo[0].tSnsExpInfo.szCurDGain[HDR_LONG_FRAME_IDX] = fGainValue;

    SNS_INFO("pipe:%d, userDgain:%f, curDgain:%f\n",
            nPipeId, ptDGain->fGain[HDR_LONG_FRAME_IDX],
            sns_obj->ae_ctrl_param.sns_ae_param.fCurDGain[HDR_LONG_FRAME_IDX]);

    return SNS_SUCCESS;
}


/* Calculate the max int time according to the exposure ratio */
AX_S32 sc410gs_get_integration_time_range(ISP_PIPE_ID nPipeId, AX_F32 fHdrRatio,
        AX_SNS_AE_INT_TIME_RANGE_T *ptIntTimeRange)
{
    SNS_STATE_OBJ *sns_obj = AX_NULL;

    SNS_CHECK_VALUE_RANGE_VALID(nPipeId, 0, AX_VIN_MAX_PIPE_NUM - 1);
    SENSOR_GET_CTX(nPipeId, sns_obj);
    SNS_CHECK_PTR_VALID(sns_obj);
    SNS_CHECK_PTR_VALID(ptIntTimeRange);

    sns_obj->ae_ctrl_param.sns_ae_limit.tIntTimeRange.fMaxIntegrationTime[HDR_LONG_FRAME_IDX] =
        gSc410gsParams[nPipeId].vts - 8;

    if (AX_SNS_LINEAR_MODE == sns_obj->sns_mode_obj.eHDRMode) {
        ptIntTimeRange->fMaxIntegrationTime[HDR_LONG_FRAME_IDX] =
            sns_obj->ae_ctrl_param.sns_ae_limit.tIntTimeRange.fMaxIntegrationTime[HDR_LONG_FRAME_IDX];
        ptIntTimeRange->fMinIntegrationTime[HDR_LONG_FRAME_IDX] =
            sns_obj->ae_ctrl_param.sns_ae_limit.tIntTimeRange.fMinIntegrationTime[HDR_LONG_FRAME_IDX];
    } else {
        // do nothing
    }

    SNS_DBG("pipe:%d, expline_limit:%.2f-%.2f(%.2f-%.2f)\n", nPipeId,
            sns_obj->ae_ctrl_param.sns_ae_limit.tIntTimeRange.fMaxIntegrationTime[HDR_LONG_FRAME_IDX],
            sns_obj->ae_ctrl_param.sns_ae_limit.tIntTimeRange.fMinIntegrationTime[HDR_LONG_FRAME_IDX],
            sns_obj->ae_ctrl_param.sns_ae_limit.tIntTimeRange.fMaxIntegrationTime[HDR_LONG_FRAME_IDX] * sns_obj->ae_ctrl_param.fTimePerLine,
            sns_obj->ae_ctrl_param.sns_ae_limit.tIntTimeRange.fMinIntegrationTime[HDR_LONG_FRAME_IDX] * sns_obj->ae_ctrl_param.fTimePerLine);

    return SNS_SUCCESS;
}

AX_S32 sc410gs_set_integration_time(ISP_PIPE_ID nPipeId, AX_SNS_AE_SHUTTER_CFG_T *ptIntTime)
{
    AX_U32 nExpReg = 0;
    AX_F32 fExpLineFromUser = 0;
    SNS_STATE_OBJ *sns_obj = AX_NULL;

    SNS_CHECK_VALUE_RANGE_VALID(nPipeId, 0, AX_VIN_MAX_PIPE_NUM - 1);
    SENSOR_GET_CTX(nPipeId, sns_obj);
    SNS_CHECK_PTR_VALID(sns_obj);
    SNS_CHECK_PTR_VALID(ptIntTime);

    fExpLineFromUser = ptIntTime->fIntTime[HDR_LONG_FRAME_IDX];
    fExpLineFromUser = AXSNS_CLIP3(fExpLineFromUser,
                                   sns_obj->ae_ctrl_param.sns_ae_limit.tIntTimeRange.fMinIntegrationTime[HDR_LONG_FRAME_IDX],
                                   sns_obj->ae_ctrl_param.sns_ae_limit.tIntTimeRange.fMaxIntegrationTime[HDR_LONG_FRAME_IDX]);

    nExpReg = round(fExpLineFromUser);

    sc410gs_update_regidx_table(nPipeId, SC410GS_LONG_EXPLINE_H_IDX, (nExpReg & 0xF000) >> 12);
    sc410gs_update_regidx_table(nPipeId, SC410GS_LONG_EXPLINE_M_IDX, (nExpReg & 0xFF0) >> 4);
    sc410gs_update_regidx_table(nPipeId, SC410GS_LONG_EXPLINE_L_IDX, (nExpReg & 0xF) << 4);

    sns_obj->ae_ctrl_param.sns_ae_param.fCurIntegrationTime[HDR_LONG_FRAME_IDX] = nExpReg;
    /* Do not delete, for its project */
    sns_obj->sztRegsInfo[0].tSnsExpInfo.szExpTime[HDR_LONG_FRAME_IDX] = nExpReg * sns_obj->ae_ctrl_param.fTimePerLine;

    SNS_INFO("pipe:%d, userExpline:%f, curExpline:%.2f(%.2f)\n",
            nPipeId, ptIntTime->fIntTime[0], sns_obj->ae_ctrl_param.sns_ae_param.fCurIntegrationTime[HDR_LONG_FRAME_IDX],
            sns_obj->ae_ctrl_param.sns_ae_param.fCurIntegrationTime[HDR_LONG_FRAME_IDX] * sns_obj->ae_ctrl_param.fTimePerLine);

    return SNS_SUCCESS;
}

AX_S32 sc410gs_get_hw_exposure_params(ISP_PIPE_ID nPipeId, AX_SNS_EXP_CTRL_PARAM_T *ptAeCtrlParam)
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

AX_S32 sc410gs_update_regidx_table(ISP_PIPE_ID nPipeId, AX_U8 nRegIdx, AX_U8 nRegValue)
{
    SNS_STATE_OBJ *sns_obj = AX_NULL;

    SNS_CHECK_VALUE_RANGE_VALID(nPipeId, 0, AX_VIN_MAX_PIPE_NUM - 1);
    SENSOR_GET_CTX(nPipeId, sns_obj);
    SNS_CHECK_PTR_VALID(sns_obj);

    if (gsc410gsBusInfo[nPipeId].busType == ISP_SNS_CONNECT_I2C_TYPE) {
        sns_obj->sztRegsInfo[0].sztData.sztI2cData[nRegIdx].nData = nRegValue;

        SNS_DBG("pipe:%d, [%2d] addr 0x%x, data 0x%x\n", nPipeId, nRegIdx,
            sns_obj->sztRegsInfo[0].sztData.sztI2cData[nRegIdx].nRegAddr,
            sns_obj->sztRegsInfo[0].sztData.sztI2cData[nRegIdx].nData);

    } else {
        sns_obj->sztRegsInfo[0].sztData.sztSpiData[nRegIdx].nData = reg_endian_swap(nRegValue, SC410GS_DATA_BYTE);

        SNS_DBG("pipe:%d, [%2d] addr 0x%x, data 0x%x (0x%x-0x%x)\n", nPipeId, nRegIdx,
                gSc410gsAeRegsTable[nRegIdx].nRegAddr, nRegValue,
                sns_obj->sztRegsInfo[0].sztData.sztSpiData[nRegIdx].nRegAddr,
                sns_obj->sztRegsInfo[0].sztData.sztSpiData[nRegIdx].nData);
    }

    return SNS_SUCCESS;
}

AX_U32 sc410gs_refresh_all_regs_from_tbl(ISP_PIPE_ID nPipeId)
{
    AX_S32 i = 0;
    AX_U32 nNum = 0;
    AX_U32 nRegValue = 0;
    SNS_STATE_OBJ *sns_obj = AX_NULL;

    SNS_CHECK_VALUE_RANGE_VALID(nPipeId, 0, AX_VIN_MAX_PIPE_NUM - 1);
    SENSOR_GET_CTX(nPipeId, sns_obj);

    nNum = sizeof(gSc410gsAeRegsTable) / sizeof(gSc410gsAeRegsTable[0]);

    for (i = 0; i < nNum; i++) {
        nRegValue = sc410gs_reg_read(nPipeId, gSc410gsAeRegsTable[i].nRegAddr);
        if (gsc410gsBusInfo[nPipeId].busType == ISP_SNS_CONNECT_I2C_TYPE) {
            sns_obj->sztRegsInfo[0].sztData.sztI2cData[i].nRegAddr = gSc410gsAeRegsTable[i].nRegAddr;
            sns_obj->sztRegsInfo[0].sztData.sztI2cData[i].nData = nRegValue;

            SNS_DBG("pipe:%d, [%2d] addr 0x%x, data 0x%x\n", nPipeId, i,
                    sns_obj->sztRegsInfo[0].sztData.sztI2cData[i].nRegAddr,
                    sns_obj->sztRegsInfo[0].sztData.sztI2cData[i].nData);

        } else {
            sns_obj->sztRegsInfo[0].sztData.sztSpiData[i].nRegAddr =
                reg_endian_swap(gSc410gsAeRegsTable[i].nRegAddr, SC410GS_ADDR_BYTE);
            sns_obj->sztRegsInfo[0].sztData.sztSpiData[i].nData =
                reg_endian_swap(nRegValue, SC410GS_DATA_BYTE);

            SNS_DBG("pipe:%d, [%2d] addr 0x%x, data 0x%x (0x%x-0x%x)\n", nPipeId, i,
                    gSc410gsAeRegsTable[i].nRegAddr, nRegValue,
                    sns_obj->sztRegsInfo[0].sztData.sztSpiData[i].nRegAddr,
                    sns_obj->sztRegsInfo[0].sztData.sztSpiData[i].nData);
        }
    }

    return SNS_SUCCESS;
}

AX_S32 sc410gs_update_init_exposure_reg(ISP_PIPE_ID nPipeId)
{
    int i = 0;
    AX_U32 nRegAddr = 0;
    AX_U32 nRegValue = 0;
    SNS_STATE_OBJ *sns_obj = AX_NULL;

    SNS_CHECK_VALUE_RANGE_VALID(nPipeId, 0, AX_VIN_MAX_PIPE_NUM - 1);
    SENSOR_GET_CTX(nPipeId, sns_obj);
    SNS_CHECK_PTR_VALID(sns_obj);

    for (i = 0; i < sns_obj->sztRegsInfo[0].nRegNum; i++) {
        if (gsc410gsBusInfo[nPipeId].busType == ISP_SNS_CONNECT_I2C_TYPE) {
            sc410gs_write_register(nPipeId, sns_obj->sztRegsInfo[0].sztData.sztI2cData[i].nRegAddr,
                                            sns_obj->sztRegsInfo[0].sztData.sztI2cData[i].nData);
            SNS_DBG("pipe:%d, [%2d] addr 0x%x, data 0x%x\n", nPipeId, i, nRegAddr, nRegValue);

        } else {
            nRegAddr = reg_endian_swap(sns_obj->sztRegsInfo[0].sztData.sztSpiData[i].nRegAddr, SC410GS_ADDR_BYTE);
            nRegValue = reg_endian_swap(sns_obj->sztRegsInfo[0].sztData.sztSpiData[i].nData, SC410GS_DATA_BYTE);
            sc410gs_write_register(nPipeId, nRegAddr, nRegValue);

            SNS_DBG("pipe:%d, [%2d] addr 0x%x, data 0x%x (0x%x-0x%x)\n", nPipeId, i, nRegAddr, nRegValue,
                    sns_obj->sztRegsInfo[0].sztData.sztSpiData[i].nRegAddr, sns_obj->sztRegsInfo[0].sztData.sztSpiData[i].nData);
        }
    }

    return SNS_SUCCESS;
}

AX_S32 sc410gs_ae_get_reg_info_i2c(ISP_PIPE_ID nPipeId, AX_SNS_REGS_CFG_TABLE_T *ptSnsRegsInfo)
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
        sns_obj->sztRegsInfo[0].tComBus.I2cDev = sc410gs_get_bus_num(nPipeId);
        sns_obj->sztRegsInfo[0].nRegNum = sizeof(gSc410gsAeRegsTable) / sizeof(gSc410gsAeRegsTable[0]);
        sns_obj->sztRegsInfo[0].nCfg2ValidDelayMax = 1; /* setting set=1, for trigger mode */

        for (i = 0; i < sns_obj->sztRegsInfo[0].nRegNum; i++) {
            sns_obj->sztRegsInfo[0].sztData.sztI2cData[i].bUpdate = AX_TRUE;
            sns_obj->sztRegsInfo[0].sztData.sztI2cData[i].nDevAddr = SC410GS_SLAVE_ADDR;
            sns_obj->sztRegsInfo[0].sztData.sztI2cData[i].nAddrByteNum = SC410GS_ADDR_BYTE;
            sns_obj->sztRegsInfo[0].sztData.sztI2cData[i].nDataByteNum = SC410GS_DATA_BYTE;
            sns_obj->sztRegsInfo[0].sztData.sztI2cData[i].nDelayFrmNum = gSc410gsAeRegsTable[i].nDelayFrmNum;
        }

        bUpdateReg = AX_TRUE;
        sns_obj->bSyncInit = AX_TRUE;
        sc410gs_update_init_exposure_reg(nPipeId);

    } else {
        for (i = 0; i < sns_obj->sztRegsInfo[0].nRegNum; i++) {
            if (sns_obj->sztRegsInfo[0].sztData.sztI2cData[i].nData == sns_obj->sztRegsInfo[1].sztData.sztI2cData[i].nData) {
                sns_obj->sztRegsInfo[0].sztData.sztI2cData[i].bUpdate = AX_FALSE;
            } else {
                sns_obj->sztRegsInfo[0].sztData.sztI2cData[i].bUpdate = AX_TRUE;
                bUpdateReg = AX_TRUE;

                SNS_INFO("pipe:%d, [%2d] addr 0x%x, data 0x%x\n", nPipeId, i,
                        sns_obj->sztRegsInfo[0].sztData.sztI2cData[i].nRegAddr,
                        sns_obj->sztRegsInfo[0].sztData.sztSpiData[i].nData);
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

AX_S32 sc410gs_ae_get_reg_info_spi(ISP_PIPE_ID nPipeId, AX_SNS_REGS_CFG_TABLE_T *ptSnsRegsInfo)
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
        sns_obj->sztRegsInfo[0].nRegNum = sizeof(gSc410gsAeRegsTable) / sizeof(gSc410gsAeRegsTable[0]);
        sns_obj->sztRegsInfo[0].nCfg2ValidDelayMax = 1; /* setting set=1, for trigger mode */

        for (i = 0; i < sns_obj->sztRegsInfo[0].nRegNum; i++) {
            sns_obj->sztRegsInfo[0].sztData.sztSpiData[i].bUpdate = AX_TRUE;
            sns_obj->sztRegsInfo[0].sztData.sztSpiData[i].nDevAddr =
                reg_endian_swap((SC410GS_DEVICEID << 1) | SC410GS_SPI_WIRTE, SC410GS_DEVADDR_BYTE);
            sns_obj->sztRegsInfo[0].sztData.sztSpiData[i].nDevAddrByteNum = SC410GS_DEVADDR_BYTE;
            sns_obj->sztRegsInfo[0].sztData.sztSpiData[i].nRegAddrByteNum = SC410GS_ADDR_BYTE;
            sns_obj->sztRegsInfo[0].sztData.sztSpiData[i].nDataByteNum = SC410GS_DATA_BYTE;
            sns_obj->sztRegsInfo[0].sztData.sztSpiData[i].nDelayFrmNum = gSc410gsAeRegsTable[i].nDelayFrmNum;
        }

        bUpdateReg = AX_TRUE;
        sns_obj->bSyncInit = AX_TRUE;
        sc410gs_update_init_exposure_reg(nPipeId);

    } else {
        for (i = 0; i < sns_obj->sztRegsInfo[0].nRegNum; i++) {
            if (sns_obj->sztRegsInfo[0].sztData.sztSpiData[i].nData == sns_obj->sztRegsInfo[1].sztData.sztSpiData[i].nData) {
                sns_obj->sztRegsInfo[0].sztData.sztSpiData[i].bUpdate = AX_FALSE;
            } else {
                sns_obj->sztRegsInfo[0].sztData.sztSpiData[i].bUpdate = AX_TRUE;
                bUpdateReg = AX_TRUE;

                SNS_INFO("pipe:%d, [%2d] addr 0x%x, data 0x%x (0x%x-0x%x)\n", nPipeId, i,
                        reg_endian_swap(sns_obj->sztRegsInfo[0].sztData.sztSpiData[i].nRegAddr, SC410GS_ADDR_BYTE),
                        reg_endian_swap(sns_obj->sztRegsInfo[0].sztData.sztSpiData[i].nData, SC410GS_DATA_BYTE),
                        sns_obj->sztRegsInfo[0].sztData.sztSpiData[i].nRegAddr,
                        sns_obj->sztRegsInfo[0].sztData.sztSpiData[i].nData);

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

AX_S32 sc410gs_ae_get_reg_info(ISP_PIPE_ID nPipeId, AX_SNS_REGS_CFG_TABLE_T *ptSnsRegsInfo)
{
    AX_S32 nRet = 0;
    SNS_STATE_OBJ *sns_obj = AX_NULL;

    SNS_CHECK_VALUE_RANGE_VALID(nPipeId, 0, AX_VIN_MAX_PIPE_NUM - 1);
    SENSOR_GET_CTX(nPipeId, sns_obj);
    SNS_CHECK_PTR_VALID(sns_obj);

    if (gsc410gsBusInfo[nPipeId].busType == ISP_SNS_CONNECT_I2C_TYPE) {
        nRet = sc410gs_ae_get_reg_info_i2c(nPipeId, ptSnsRegsInfo);
    } else {
        nRet = sc410gs_ae_get_reg_info_spi(nPipeId, ptSnsRegsInfo);
    }

    return nRet;
}

AX_S32 sc410gs_set_fps(ISP_PIPE_ID nPipeId, AX_F32 fFps)
{
    AX_U32 nVts = 0;
    SNS_STATE_OBJ *sns_obj = AX_NULL;

    SNS_CHECK_VALUE_RANGE_VALID(nPipeId, 0, AX_VIN_MAX_PIPE_NUM - 1);
    SENSOR_GET_CTX(nPipeId, sns_obj);
    SNS_CHECK_PTR_VALID(sns_obj);

    if (IS_FLOAT_ZERO(fFps)) {
        SNS_ERR("userFps is zero !\n");
        return SNS_ERR_CODE_ILLEGAL_PARAMS;
    }

    if (IS_SNS_FPS_EQUAL(fFps, gSc410gsParams[nPipeId].setting_fps)) {
        nVts = gSc410gsParams[nPipeId].setting_vts;
    } else {
        nVts = 1 * SNS_1_SECOND_UNIT_US / (sns_obj->ae_ctrl_param.fTimePerLine * fFps);
        if (nVts > SC410GS_MAX_VTS){
            nVts = SC410GS_MAX_VTS;
            SNS_WRN("Beyond max vts:0x%x\n", nVts);
        }
    }

    sc410gs_set_vts(nPipeId, nVts);
    gSc410gsParams[nPipeId].vts = nVts;
    if (IS_SNS_FPS_EQUAL(fFps, sns_obj->sns_attr_param.fFrameRate)) {
        sns_obj->sns_mode_obj.nVts = nVts;
        sns_obj->sns_mode_obj.fFrameRate = sns_obj->sns_attr_param.fFrameRate;
    }

    sns_obj->ae_ctrl_param.sns_ae_limit.tIntTimeRange.fMaxIntegrationTime[HDR_LONG_FRAME_IDX] = gSc410gsParams[nPipeId].vts - 8;
    sns_obj->ae_ctrl_param.sns_ae_param.fCurFps = 1 * SNS_1_SECOND_UNIT_US / (sns_obj->ae_ctrl_param.fTimePerLine * nVts);

    SNS_INFO("pipe:%d, userFps:%f, curFps:%f, vts:0x%x\n",
        nPipeId, fFps, sns_obj->ae_ctrl_param.sns_ae_param.fCurFps, nVts);

    return SNS_SUCCESS;
};

AX_S32 sc410gs_get_slow_shutter_param(ISP_PIPE_ID nPipeId, AX_SNS_AE_SLOW_SHUTTER_PARAM_T *ptSlowShutterParam)
{
    AX_U32 i = 0;
    AX_U32 nVts = 0;
    SNS_STATE_OBJ *sns_obj = AX_NULL;
    AX_U32 nFrameRate = 0;

    SNS_CHECK_PTR_VALID(ptSlowShutterParam);
    SNS_CHECK_VALUE_RANGE_VALID(nPipeId, 0, AX_VIN_MAX_PIPE_NUM - 1);

    SENSOR_GET_CTX(nPipeId, sns_obj);
    SNS_CHECK_PTR_VALID(sns_obj);

    if (AXSNS_CAMPARE_FLOAT(sns_obj->sns_mode_obj.fFrameRate, SNS_MAX_FRAME_RATE)) {
        SNS_ERR("framerate:%f out of range:%d\n", sns_obj->sns_mode_obj.fFrameRate, SNS_MAX_FRAME_RATE);
        return SNS_ERR_CODE_ILLEGAL_PARAMS;
    }

    if(AXSNS_CAMPARE_FLOAT(sns_obj->sns_mode_obj.fFrameRate, (AX_U32)sns_obj->sns_mode_obj.fFrameRate)) {
        nFrameRate = (AX_U32)ceil(sns_obj->sns_mode_obj.fFrameRate);
    } else {
        nFrameRate = (AX_U32)sns_obj->sns_mode_obj.fFrameRate;
    }

    ptSlowShutterParam->nGroupNum = AXSNS_MIN((sizeof(gFpsGear) / sizeof(AX_F32)), nFrameRate);
    //ax_sns_quick_sort_float(gFpsGear, ptSlowShutterParam->nGroupNum);
    ptSlowShutterParam->fMinFps = AXSNS_MAX(gFpsGear[0],
                                            (1 * SNS_1_SECOND_UNIT_US / (sns_obj->ae_ctrl_param.fTimePerLine * SC410GS_MAX_VTS)));

    for (i = 0; i < ptSlowShutterParam->nGroupNum; i++) {
        if (AXSNS_CAMPARE_FLOAT(gFpsGear[i], sns_obj->sns_mode_obj.fFrameRate) ||
            IS_SNS_FPS_EQUAL(gFpsGear[i], sns_obj->sns_mode_obj.fFrameRate)) {
            nVts = sns_obj->sns_mode_obj.nVts;
        } else {
            nVts = 1 * SNS_1_SECOND_UNIT_US / (sns_obj->ae_ctrl_param.fTimePerLine * gFpsGear[i]);
            if (nVts > SC410GS_MAX_VTS) {
                nVts = SC410GS_MAX_VTS;
                SNS_DBG("Beyond min fps:%f\n", ptSlowShutterParam->fMinFps);
            }
        }

        ptSlowShutterParam->tSlowShutterTbl[i].fRealFps = 1 * SNS_1_SECOND_UNIT_US / (sns_obj->ae_ctrl_param.fTimePerLine * nVts);
        ptSlowShutterParam->fMaxFps = ptSlowShutterParam->tSlowShutterTbl[i].fRealFps;

        if (IS_LINEAR_MODE(sns_obj->sns_mode_obj.eHDRMode)) {
            ptSlowShutterParam->tSlowShutterTbl[i].fMaxIntTime = nVts - 8;
        } else if (IS_2DOL_HDR_MODE(sns_obj->sns_mode_obj.eHDRMode)) {
            /**/
        }

        SNS_DBG("pipe=%d, fps=%f, max_expline=%.2f(%.2f), vts=0x%x\n",
                nPipeId, ptSlowShutterParam->tSlowShutterTbl[i].fRealFps, ptSlowShutterParam->tSlowShutterTbl[i].fMaxIntTime,
                ptSlowShutterParam->tSlowShutterTbl[i].fMaxIntTime * sns_obj->ae_ctrl_param.fTimePerLine, nVts);
    }

    return SNS_SUCCESS;
}