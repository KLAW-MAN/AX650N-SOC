#ifndef __IMX678_SDR_H__
#define __IMX678_SDR_H__

/********************************************************************
* Store the default parameters of the sdr mode
* warn: user need to add 'static' when defining global variables.
        Limit the scope of the variable to this sensor
*********************************************************************/

#include "ax_isp_iq_api.h"
#include "ax_isp_api.h"

static AX_ISP_VERSION_T ax_isp_version_param_sdr = {
    /* nIspMajor */
    3,
    /* nIspMinor1 */
    9,
    /* nIspMinor2 */
    8,
    /* szBuildTime[32] */
    "Apr 19 2023 17:35:24",
    /* szIspVersion[64] */
    "AX650_ISP_V3.9.8",
};

static AX_ISP_IQ_SCENE_PARAM_T scene_param_sdr = {
    /* nAutoMode */
    1,
    /* tManualParam */
    {
        /* nTnrWorkMode */
        0,
        /* nAiWorkMode */
        0,
    },
    /* tAutoParam */
    {
        /* nSceneNum */
        2,
        /* nDelta */
        10,
        /* nRefValStart[4] */
        {0, 65536, /*0 - 1*/},
        /* nRefValEnd[4] */
        {65536, 4193280, /*0 - 1*/},
        /* nTnrWorkMode[4] */
        {0, 2, /*0 - 1*/},
        /* nAiWorkMode[4] */
        {0, 1, /*0 - 1*/},
    },
};

static AX_ISP_IQ_AE_PARAM_T ae_param_sdr = {
    /* nEnable */
    1,
    /* tExpManual */
    {
        /* nIspGain */
        10410,
        /* nAGain */
        49011,
        /* nDgain */
        1024,
        /* nHcgLcg */
        0,
        /* nSnsTotalAGain */
        132341,
        /* nSysTotalGain */
        1345483,
        /* nShutter */
        39955,
        /* nIrisPwmDuty */
        0,
        /* nPos */
        512,
        /* nHdrRealRatioLtoS */
        1024,
        /* nHdrRealRatioStoVS */
        1024,
        /* nSetPoint */
        8926,
        /* nShortAgain */
        0,
        /* nShortDgain */
        0,
        /* nShortShutter */
        0,
        /* nVsAgain */
        0,
        /* nVsDgain */
        0,
        /* nVsShutter */
        0,
        /* nHdrRatio */
        1024,
        /* nHdrMaxShutterHwLimit */
        39955,
        /* nRealMaxShutter */
        39955,
    },
    /* tAeAlgAuto */
    {
        /* nSetPoint */
        30607,
        /* nTolerance */
        10485760,
        /* nAgainLcg2HcgTh */
        4526,
        /* nAgainHcg2LcgTh */
        1096,
        /* nAgainLcg2HcgRatio */
        2765,
        /* nAgainHcg2LcgRatio */
        2765,
        /* nLuxk */
        93391,
        /* nCompensationMode */
        2,
        /* nPreChargeSize */
        0,
        /* nPreCharge[15] */
        {0,/*0 - 0*/},
        /* nMaxIspGain */
        8192,
        /* nMinIspGain */
        1024,
        /* nMaxUserDgain */
        1024,
        /* nMinUserDgain */
        1024,
        /* nMaxUserTotalAgain */
        133120,
        /* nMinUserTotalAgain */
        1024,
        /* nMaxUserSysGain */
        2048000,
        /* nMinUserSysGain */
        1024,
        /* nMaxShutter */
        39955,
        /* nMinShutter */
        120,
        /* nPositionWeightMode */
        1,
        /* nRoiStartX */
        0,
        /* nRoiStartY */
        0,
        /* nRoiWidth */
        0,
        /* nRoiHeight */
        0,
        /* nWeightRoi */
        1024,
        /* nWeightBackgnd */
        1024,
        /* nGridWeightRow */
        15,
        /* nGridWeightColumn */
        17,
        /* nGridWeightTable[54][72] */
        {
            {139, 175, 215, 255, 293, 327, 354, 371, 377, 371, 354, 327, 293, 255, 215, 175, 139, /*0 - 16*/},
            {181, 228, 280, 332, 383, 427, 461, 484, 491, 484, 461, 427, 383, 332, 280, 228, 181, /*0 - 16*/},
            {226, 286, 350, 416, 479, 534, 578, 605, 615, 605, 578, 534, 479, 416, 350, 286, 226, /*0 - 16*/},
            {272, 344, 421, 500, 575, 642, 694, 727, 739, 727, 694, 642, 575, 500, 421, 344, 272, /*0 - 16*/},
            {313, 396, 486, 577, 664, 740, 801, 839, 852, 839, 801, 740, 664, 577, 486, 396, 313, /*0 - 16*/},
            {347, 439, 538, 639, 735, 820, 887, 929, 944, 929, 887, 820, 735, 639, 538, 439, 347, /*0 - 16*/},
            {369, 467, 572, 679, 781, 872, 943, 988, 1003, 988, 943, 872, 781, 679, 572, 467, 369, /*0 - 16*/},
            {377, 476, 583, 693, 797, 890, 962, 1008, 1024, 1008, 962, 890, 797, 693, 583, 476, 377, /*0 - 16*/},
            {369, 467, 572, 679, 781, 872, 943, 988, 1003, 988, 943, 872, 781, 679, 572, 467, 369, /*0 - 16*/},
            {347, 439, 538, 639, 735, 820, 887, 929, 944, 929, 887, 820, 735, 639, 538, 439, 347, /*0 - 16*/},
            {313, 396, 486, 577, 664, 740, 801, 839, 852, 839, 801, 740, 664, 577, 486, 396, 313, /*0 - 16*/},
            {272, 344, 421, 500, 575, 642, 694, 727, 739, 727, 694, 642, 575, 500, 421, 344, 272, /*0 - 16*/},
            {226, 286, 350, 416, 479, 534, 578, 605, 615, 605, 578, 534, 479, 416, 350, 286, 226, /*0 - 16*/},
            {181, 228, 280, 332, 383, 427, 461, 484, 491, 484, 461, 427, 383, 332, 280, 228, 181, /*0 - 16*/},
            {139, 175, 215, 255, 293, 327, 354, 371, 377, 371, 354, 327, 293, 255, 215, 175, 139, /*0 - 16*/},
        },
        /* tAntiFlickerParam */
        {
            /* nAntiFlickerMode */
            1,
            /* nFlickerPeriod */
            0,
            /* tAntiFlickerToleranceCurve */
            {
                /* nSize */
                2,
                /* nRefList[12] */
                {409600, 512000, /*0 - 1*/},
                /* nAntiFlickerToleranceList[12] */
                {50, 50, /*0 - 1*/},
            },
            /* nOverExpMode */
            1,
            /* nUnderExpMode */
            1,
        },
        /* tAutoFlickerDetectParam */
        {
            /* nFlickerValidNum */
            5,
            /* tFlickerParamCurve */
            {
                /* nSize */
                2,
                /* nRefList[12] */
                {409600, 1024000, /*0 - 1*/},
                /* nFlickerValidThList[12] */
                {3072, 3072, /*0 - 1*/},
                /* nFlickerTrigTimeList[12] */
                {50, 50, /*0 - 1*/},
            },
            /* nSkipTh */
            2,
            /* nUpSlopeTh */
            6,
            /* nDownSlopeTh */
            -6,
        },
        /* nSetPointMode */
        1,
        /* nStrategyMode */
        2,
        /* nAeRouteMode */
        0,
        /* tAeRouteParam */
        {
            /* nTableNum */
            1,
            /* nUsedTableId */
            0,
            /* tRouteTable[8] */
            {
                /* 0 */
                {
                    /* sTableName[32] */
                    "DefaultAeRoute",
                    /* nRouteCurveNum */
                    2,
                    /* tRouteCurveList[16] */
                    {
                        /* 0 */
                        {
                            /* nIntergrationTime */
                            120,
                            /* nGain */
                            1024,
                            /* nAperture */
                            0,
                            /* nIncrementPriority */
                            0,
                        },
                        /* 1 */
                        {
                            /* nIntergrationTime */
                            39955,
                            /* nGain */
                            1024000,
                            /* nAperture */
                            0,
                            /* nIncrementPriority */
                            0,
                        },
                    },
                },
            },
        },
        /* tAeRouteAdvanceTable */
        {
            /* sAdvanceTableName[32] */
            "AeRouteAdvance",
            /* nRouteAdvanceCurveNum */
            5,
            /* tRouteAdvanceCurveList[16] */
            {
                /* 0 */
                {
                    /* nIntergrationTime */
                    40,
                    /* nTotalAGain */
                    1024,
                    /* nDGain */
                    1024,
                    /* nIspGain */
                    1024,
                },
                /* 1 */
                {
                    /* nIntergrationTime */
                    39500,
                    /* nTotalAGain */
                    1024,
                    /* nDGain */
                    1024,
                    /* nIspGain */
                    1024,
                },
                /* 2 */
                {
                    /* nIntergrationTime */
                    39500,
                    /* nTotalAGain */
                    1024,
                    /* nDGain */
                    1024,
                    /* nIspGain */
                    2048,
                },
                /* 3 */
                {
                    /* nIntergrationTime */
                    39500,
                    /* nTotalAGain */
                    69632,
                    /* nDGain */
                    1024,
                    /* nIspGain */
                    2048,
                },
                /* 4 */
                {
                    /* nIntergrationTime */
                    39500,
                    /* nTotalAGain */
                    69632,
                    /* nDGain */
                    1024,
                    /* nIspGain */
                    524288,
                },
            },
        },
        /* tAeSetPointCurve */
        {
            /* nSize */
            4,
            /* nRefList[10] */
            {1024, 16384, 131072, 524288, /*0 - 3*/},
            /* nSetPointList[10] */
            {30720, 24576, 18432, 15360, /*0 - 3*/},
        },
        /* tAeHdrRatio */
        {
            /* nHdrMode */
            0,
            /* tRatioStrategyParam */
            {
                /* nShortNonSatAreaPercent */
                104438168,
                /* nTolerance */
                8388608,
                /* nConvergeCntFrameNum */
                3,
                /* nDampRatio */
                922,
                /* tHdrRatioParamCurve */
                {
                    /* nListSize */
                    6,
                    /* nRefList[10] */
                    {102, 10240, 30720, 102400, 204800, 1024000, /*0 - 5*/},
                    /* nSatLumaList[10] */
                    {153600, 122880, 61440, 40960, 20480, 10240, /*0 - 5*/},
                    /* nMinRatioList[10] */
                    {1024, 1024, 1024, 1024, 1024, 1024, /*0 - 5*/},
                    /* nMaxRatioList[10] */
                    {32768, 32768, 32768, 32768, 32768, 32768, /*0 - 5*/},
                },
            },
            /* nFixedHdrRatio */
            1024,
        },
        /* nMultiCamSyncMode */
        0,
        /* nMultiCamSyncRatio */
        1048576,
        /* tSlowShutterParam */
        {
            /* nFrameRateMode */
            0,
            /* nFpsIncreaseDelayFrame */
            0,
        },
        /* tIrisParam */
        {
            /* nIrisType */
            0,
            /* tDcIrisParam */
            {
                /* nBigStepFactor */
                104858,
                /* nSmallStepFactor */
                10486,
                /* nLumaDiffOverThresh */
                35840,
                /* nLumaDiffUnderThresh */
                35840,
                /* nLumaSpeedThresh */
                205,
                /* nSpeedDownFactor */
                209715,
                /* nMinUserPwmDuty */
                30720,
                /* nMaxUserPwmDuty */
                66560,
                /* nOpenPwmDuty */
                61440,
                /* nConvergeLumaDiffTolerance */
                52429,
                /* nConvergeFrameCntThresh */
                10,
            },
        },
        /* tLumaWeightParam */
        {
            /* nEnable */
            0,
            /* nLumaWeightNum */
            16,
            /* nLumaSplitList[64] */
            {0, 16384, 32768, 49152, 65535, 81920, 98304, 114688, 131072, 147456, 163840, 180224, 196608, 212992, 229376, 245760, /*0 - 15*/},
            /* nWeightList[64] */
            {1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, /*0 - 15*/},
        },
        /* tTimeSmoothParam */
        {
            /* tStateMachineParam */
            {
                /* nToFastLumaThOver */
                1536,
                /* nToFastLumaThUnder */
                819,
                /* nToSlowFrameTh */
                3,
                /* nToConvergedFrameTh */
                2,
            },
            /* tConvergeSpeedParam */
            {
                /* nFastOverKneeCnt */
                14,
                /* nFastOverLumaDiffList[16] */
                {5120, 10240, 15360, 20480, 25600, 30720, 40960, 51200, 71680, 92160, 112640, 153600, 209920, 262144, /*0 - 13*/},
                /* nFastOverStepFactorList[16] */
                {102, 154, 184, 184, 205, 205, 205, 205, 256, 307, 358, 410, 512, 614, /*0 - 13*/},
                /* nFastOverSpeedDownFactorList[16] */
                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 102, 102, 307, /*0 - 13*/},
                /* nFastOverSkipList[16] */
                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 13*/},
                /* nFastUnderKneeCnt */
                11,
                /* nFastUnderLumaDiffList[16] */
                {5120, 10240, 15360, 20480, 25600, 30720, 35840, 40960, 51200, 153600, 262144, /*0 - 10*/},
                /* nFastUnderStepFactorList[16] */
                {102, 154, 154, 154, 154, 154, 154, 154, 154, 154, 154, /*0 - 10*/},
                /* nFastUnderSpeedDownFactorList[16] */
                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 10*/},
                /* nFastUnderSkipList[16] */
                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 10*/},
                /* nSlowOverKneeCnt */
                14,
                /* nSlowOverLumaDiffList[16] */
                {5120, 10240, 15360, 20480, 25600, 30720, 40960, 51200, 81920, 92160, 112640, 153600, 209920, 262144, /*0 - 13*/},
                /* nSlowOverStepFactorList[16] */
                {102, 123, 123, 154, 154, 174, 184, 184, 184, 184, 184, 184, 184, 184, /*0 - 13*/},
                /* nSlowOverSpeedDownFactorList[16] */
                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 13*/},
                /* nSlowOverSkipList[16] */
                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 13*/},
                /* nSlowUnderKneeCnt */
                11,
                /* nSlowUnderLumaDiffList[16] */
                {5120, 10240, 15360, 20480, 25600, 30720, 35840, 40960, 51200, 153600, 262144, /*0 - 10*/},
                /* nSlowUnderStepFactorList[16] */
                {102, 123, 154, 154, 154, 154, 154, 154, 154, 154, 154, /*0 - 10*/},
                /* nSlowUnderSpeedDownFactorList[16] */
                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 10*/},
                /* nSlowUnderSkipList[16] */
                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 10*/},
            },
        },
        /* tSleepWakeUpParam */
        {
            /* nNoiseLevel */
            41,
            /* nLinearLumaTh */
            51,
            /* nAeStatsDelayFrame */
            4,
            /* tSleepSetting */
            {
                /* nAGain */
                1024,
                /* nDGain */
                1024,
                /* nIspGain */
                1024,
                /* nHcgLcg */
                1,
                /* nShutter */
                1000,
            },
            /* nOverExpCompLumaTh */
            102400,
            /* tOverExpCompLut */
            {
                /* nLutNum */
                5,
                /* nLumaSplitList[8] */
                {179200, 220160, 225280, 240640, 244736, 0, 0, 0, /*0 - 7*/},
                /* nCompFactorList[8] */
                {1024, 1208, 1352, 2324, 2560, 0, 0, 0, /*0 - 7*/},
            },
        },
        /* tHistPointCtrlParam */
        {
            /* nEnable */
            0,
            /* nHistPointLutNum */
            1,
            /* tHistPointCtrlLut[10] */
            {
                /* 0 */
                {
                    /* nLuxStart */
                    102400,
                    /* nLuxEnd */
                    204800,
                    /* tHistPointTh */
                    {
                        /* nLumaThList[2] */
                        {0, 0, /*0 - 1*/},
                        /* nPercentThList[2] */
                        {102400, 102400, /*0 - 1*/},
                    },
                },
            },
        },
    },
    /* nLogLevel */
    4,
    /* nLogTarget */
    2,
};

static AX_ISP_IQ_AWB_PARAM_T awb_param_sdr = {
    /* nEnable */
    1,
    /* tManualParam */
    {
        /* nManualMode */
        0,
        /* tGain */
        {
            /* nGainR */
            391,
            /* nGainGr */
            256,
            /* nGainGb */
            256,
            /* nGainB */
            551,
        },
        /* tManualLightSource */
        {
            /* nLightSourceIndex */
            0,
            /* tLightSource[15] */
            {
                /* 0 */
                {
                    /* szName[32] */
                    "Shade",
                    /* nColorTemperature */
                    7500,
                    /* nGreenShift */
                    0,
                },
                /* 1 */
                {
                    /* szName[32] */
                    "Day",
                    /* nColorTemperature */
                    6500,
                    /* nGreenShift */
                    0,
                },
                /* 2 */
                {
                    /* szName[32] */
                    "Cloudy",
                    /* nColorTemperature */
                    5000,
                    /* nGreenShift */
                    0,
                },
                /* 3 */
                {
                    /* szName[32] */
                    "Flourescent",
                    /* nColorTemperature */
                    5000,
                    /* nGreenShift */
                    0,
                },
                /* 4 */
                {
                    /* szName[32] */
                    "Sunset",
                    /* nColorTemperature */
                    3500,
                    /* nGreenShift */
                    0,
                },
                /* 5 */
                {
                    /* szName[32] */
                    "Incandescent",
                    /* nColorTemperature */
                    2800,
                    /* nGreenShift */
                    0,
                },
                /* 6 */
                {
                    /* szName[32] */
                    "Candle",
                    /* nColorTemperature */
                    2000,
                    /* nGreenShift */
                    0,
                },
                /* 7 */
                {
                    /* szName[32] */
                    "Flash",
                    /* nColorTemperature */
                    3500,
                    /* nGreenShift */
                    0,
                },
                /* 8 */
                {
                    /* szName[32] */
                    "UserDefined-1",
                    /* nColorTemperature */
                    2800,
                    /* nGreenShift */
                    0,
                },
                /* 9 */
                {
                    /* szName[32] */
                    "UserDefined-2",
                    /* nColorTemperature */
                    2000,
                    /* nGreenShift */
                    0,
                },
                /* 10 */
                {
                    /* szName[32] */
                    "UserDefined-3",
                    /* nColorTemperature */
                    5000,
                    /* nGreenShift */
                    0,
                },
                /* 11 */
                {
                    /* szName[32] */
                    "UserDefined-4",
                    /* nColorTemperature */
                    5000,
                    /* nGreenShift */
                    0,
                },
                /* 12 */
                {
                    /* szName[32] */
                    "UserDefined-5",
                    /* nColorTemperature */
                    5000,
                    /* nGreenShift */
                    0,
                },
                /* 13 */
                {
                    /* szName[32] */
                    "UserDefined-6",
                    /* nColorTemperature */
                    5000,
                    /* nGreenShift */
                    0,
                },
                /* 14 */
                {
                    /* szName[32] */
                    "UserDefined-7",
                    /* nColorTemperature */
                    5000,
                    /* nGreenShift */
                    0,
                },
            },
        },
    },
    /* tAutoParam */
    {
        /* tCenterPnt */
        {
            /* nRg */
            1397365,
            /* nBg */
            1414557,
        },
        /* nCenterPntRadius */
        1212963,
        /* nLowCut */
        0,
        /* nHighCut */
        56,
        /* nCctMax */
        12000,
        /* nCctMin */
        1300,
        /* nPartCtrlPntNum */
        8,
        /* nCtrlPntNum */
        57,
        /* nCtrlSegKbNum */
        56,
        /* nCctList[512] */
        {
            1300, 1425, 1550, 1675, 1800, 1925, 2050, 2175, 2300, 2369, 2439, 2508, 2578, 2647, 2717, 2786, 2856, 2974, 3092, 3210, 3328, 3446, 3564, 3682, 3800, 3950, 4100, 4250, 4400, 4550, 4700, 4850,  /* 0 - 31*/
            5000, 5187, 5375, 5562, 5750, 5937, 6125, 6312, 6500, 6625, 6750, 6875, 7000, 7125, 7250, 7375, 7500, 8062, 8625, 9187, 9750, 10312, 10875, 11437, 12000, /*32 - 56*/
        },
        /* tChordKB */
        {
            /* nK */
            -780,
            /* nB */
            1101,
        },
        /* tChordPntList[512] */
        {
            /* 0 */
            {
                /* nRg */
                1192682,
                /* nBg */
                218985,
            },
            /* 1 */
            {
                /* nRg */
                1162315,
                /* nBg */
                242127,
            },
            /* 2 */
            {
                /* nRg */
                1132787,
                /* nBg */
                264640,
            },
            /* 3 */
            {
                /* nRg */
                1104004,
                /* nBg */
                286576,
            },
            /* 4 */
            {
                /* nRg */
                1075891,
                /* nBg */
                308009,
            },
            /* 5 */
            {
                /* nRg */
                1048387,
                /* nBg */
                328970,
            },
            /* 6 */
            {
                /* nRg */
                1021439,
                /* nBg */
                349511,
            },
            /* 7 */
            {
                /* nRg */
                994962,
                /* nBg */
                369696,
            },
            /* 8 */
            {
                /* nRg */
                968937,
                /* nBg */
                389535,
            },
            /* 9 */
            {
                /* nRg */
                954624,
                /* nBg */
                400441,
            },
            /* 10 */
            {
                /* nRg */
                940426,
                /* nBg */
                411262,
            },
            /* 11 */
            {
                /* nRg */
                926333,
                /* nBg */
                422010,
            },
            /* 12 */
            {
                /* nRg */
                912324,
                /* nBg */
                432684,
            },
            /* 13 */
            {
                /* nRg */
                898409,
                /* nBg */
                443296,
            },
            /* 14 */
            {
                /* nRg */
                884568,
                /* nBg */
                453845,
            },
            /* 15 */
            {
                /* nRg */
                870800,
                /* nBg */
                464330,
            },
            /* 16 */
            {
                /* nRg */
                857096,
                /* nBg */
                474785,
            },
            /* 17 */
            {
                /* nRg */
                842489,
                /* nBg */
                485921,
            },
            /* 18 */
            {
                /* nRg */
                827935,
                /* nBg */
                497004,
            },
            /* 19 */
            {
                /* nRg */
                813433,
                /* nBg */
                508067,
            },
            /* 20 */
            {
                /* nRg */
                798962,
                /* nBg */
                519087,
            },
            /* 21 */
            {
                /* nRg */
                784534,
                /* nBg */
                530097,
            },
            /* 22 */
            {
                /* nRg */
                770116,
                /* nBg */
                541076,
            },
            /* 23 */
            {
                /* nRg */
                755719,
                /* nBg */
                552054,
            },
            /* 24 */
            {
                /* nRg */
                741322,
                /* nBg */
                563033,
            },
            /* 25 */
            {
                /* nRg */
                728299,
                /* nBg */
                572952,
            },
            /* 26 */
            {
                /* nRg */
                715265,
                /* nBg */
                582893,
            },
            /* 27 */
            {
                /* nRg */
                702221,
                /* nBg */
                592833,
            },
            /* 28 */
            {
                /* nRg */
                689156,
                /* nBg */
                602795,
            },
            /* 29 */
            {
                /* nRg */
                676069,
                /* nBg */
                612767,
            },
            /* 30 */
            {
                /* nRg */
                662941,
                /* nBg */
                622770,
            },
            /* 31 */
            {
                /* nRg */
                649782,
                /* nBg */
                632805,
            },
            /* 32 */
            {
                /* nRg */
                636570,
                /* nBg */
                642882,
            },
            /* 33 */
            {
                /* nRg */
                629963,
                /* nBg */
                647915,
            },
            /* 34 */
            {
                /* nRg */
                623336,
                /* nBg */
                652959,
            },
            /* 35 */
            {
                /* nRg */
                616699,
                /* nBg */
                658023,
            },
            /* 36 */
            {
                /* nRg */
                610051,
                /* nBg */
                663088,
            },
            /* 37 */
            {
                /* nRg */
                603382,
                /* nBg */
                668174,
            },
            /* 38 */
            {
                /* nRg */
                596692,
                /* nBg */
                673270,
            },
            /* 39 */
            {
                /* nRg */
                589992,
                /* nBg */
                678376,
            },
            /* 40 */
            {
                /* nRg */
                583270,
                /* nBg */
                683504,
            },
            /* 41 */
            {
                /* nRg */
                577346,
                /* nBg */
                688023,
            },
            /* 42 */
            {
                /* nRg */
                571401,
                /* nBg */
                692543,
            },
            /* 43 */
            {
                /* nRg */
                565445,
                /* nBg */
                697083,
            },
            /* 44 */
            {
                /* nRg */
                559478,
                /* nBg */
                701644,
            },
            /* 45 */
            {
                /* nRg */
                553480,
                /* nBg */
                706205,
            },
            /* 46 */
            {
                /* nRg */
                547472,
                /* nBg */
                710788,
            },
            /* 47 */
            {
                /* nRg */
                541453,
                /* nBg */
                715380,
            },
            /* 48 */
            {
                /* nRg */
                535403,
                /* nBg */
                719994,
            },
            /* 49 */
            {
                /* nRg */
                507941,
                /* nBg */
                740924,
            },
            /* 50 */
            {
                /* nRg */
                480007,
                /* nBg */
                762210,
            },
            /* 51 */
            {
                /* nRg */
                451548,
                /* nBg */
                783905,
            },
            /* 52 */
            {
                /* nRg */
                422503,
                /* nBg */
                806051,
            },
            /* 53 */
            {
                /* nRg */
                392786,
                /* nBg */
                828700,
            },
            /* 54 */
            {
                /* nRg */
                362314,
                /* nBg */
                851926,
            },
            /* 55 */
            {
                /* nRg */
                331014,
                /* nBg */
                875781,
            },
            /* 56 */
            {
                /* nRg */
                298792,
                /* nBg */
                900349,
            },
        },
        /* tArcPointList[512] */
        {
            /* 0 */
            {
                /* nRg */
                1206838,
                /* nBg */
                301675,
            },
            /* 1 */
            {
                /* nRg */
                1172675,
                /* nBg */
                293801,
            },
            /* 2 */
            {
                /* nRg */
                1137925,
                /* nBg */
                286974,
            },
            /* 3 */
            {
                /* nRg */
                1102609,
                /* nBg */
                281239,
            },
            /* 4 */
            {
                /* nRg */
                1066769,
                /* nBg */
                276604,
            },
            /* 5 */
            {
                /* nRg */
                1030436,
                /* nBg */
                273102,
            },
            /* 6 */
            {
                /* nRg */
                993631,
                /* nBg */
                270753,
            },
            /* 7 */
            {
                /* nRg */
                961439,
                /* nBg */
                282633,
            },
            /* 8 */
            {
                /* nRg */
                929594,
                /* nBg */
                295415,
            },
            /* 9 */
            {
                /* nRg */
                912051,
                /* nBg */
                302913,
            },
            /* 10 */
            {
                /* nRg */
                894624,
                /* nBg */
                310683,
            },
            /* 11 */
            {
                /* nRg */
                877323,
                /* nBg */
                318736,
            },
            /* 12 */
            {
                /* nRg */
                860147,
                /* nBg */
                327051,
            },
            /* 13 */
            {
                /* nRg */
                843108,
                /* nBg */
                335639,
            },
            /* 14 */
            {
                /* nRg */
                826204,
                /* nBg */
                344489,
            },
            /* 15 */
            {
                /* nRg */
                809438,
                /* nBg */
                353601,
            },
            /* 16 */
            {
                /* nRg */
                792828,
                /* nBg */
                362986,
            },
            /* 17 */
            {
                /* nRg */
                775202,
                /* nBg */
                373314,
            },
            /* 18 */
            {
                /* nRg */
                757764,
                /* nBg */
                383936,
            },
            /* 19 */
            {
                /* nRg */
                740504,
                /* nBg */
                394841,
            },
            /* 20 */
            {
                /* nRg */
                723423,
                /* nBg */
                406051,
            },
            /* 21 */
            {
                /* nRg */
                706541,
                /* nBg */
                417543,
            },
            /* 22 */
            {
                /* nRg */
                689858,
                /* nBg */
                429308,
            },
            /* 23 */
            {
                /* nRg */
                673364,
                /* nBg */
                441367,
            },
            /* 24 */
            {
                /* nRg */
                657090,
                /* nBg */
                453687,
            },
            /* 25 */
            {
                /* nRg */
                642536,
                /* nBg */
                465075,
            },
            /* 26 */
            {
                /* nRg */
                628160,
                /* nBg */
                476683,
            },
            /* 27 */
            {
                /* nRg */
                613973,
                /* nBg */
                488511,
            },
            /* 28 */
            {
                /* nRg */
                594760,
                /* nBg */
                494595,
            },
            /* 29 */
            {
                /* nRg */
                580840,
                /* nBg */
                506923,
            },
            /* 30 */
            {
                /* nRg */
                567119,
                /* nBg */
                519461,
            },
            /* 31 */
            {
                /* nRg */
                553578,
                /* nBg */
                532210,
            },
            /* 32 */
            {
                /* nRg */
                540238,
                /* nBg */
                545171,
            },
            /* 33 */
            {
                /* nRg */
                533652,
                /* nBg */
                551704,
            },
            /* 34 */
            {
                /* nRg */
                527119,
                /* nBg */
                558300,
            },
            /* 35 */
            {
                /* nRg */
                520639,
                /* nBg */
                564927,
            },
            /* 36 */
            {
                /* nRg */
                514212,
                /* nBg */
                571619,
            },
            /* 37 */
            {
                /* nRg */
                513582,
                /* nBg */
                583763,
            },
            /* 38 */
            {
                /* nRg */
                507301,
                /* nBg */
                590506,
            },
            /* 39 */
            {
                /* nRg */
                501062,
                /* nBg */
                597290,
            },
            /* 40 */
            {
                /* nRg */
                494875,
                /* nBg */
                604127,
            },
            /* 41 */
            {
                /* nRg */
                489486,
                /* nBg */
                610177,
            },
            /* 42 */
            {
                /* nRg */
                484128,
                /* nBg */
                616259,
            },
            /* 43 */
            {
                /* nRg */
                478822,
                /* nBg */
                622372,
            },
            /* 44 */
            {
                /* nRg */
                473547,
                /* nBg */
                628527,
            },
            /* 45 */
            {
                /* nRg */
                468315,
                /* nBg */
                634714,
            },
            /* 46 */
            {
                /* nRg */
                463125,
                /* nBg */
                640942,
            },
            /* 47 */
            {
                /* nRg */
                457987,
                /* nBg */
                647202,
            },
            /* 48 */
            {
                /* nRg */
                452880,
                /* nBg */
                653494,
            },
            /* 49 */
            {
                /* nRg */
                430430,
                /* nBg */
                682225,
            },
            /* 50 */
            {
                /* nRg */
                408850,
                /* nBg */
                711616,
            },
            /* 51 */
            {
                /* nRg */
                388172,
                /* nBg */
                741647,
            },
            /* 52 */
            {
                /* nRg */
                368407,
                /* nBg */
                772276,
            },
            /* 53 */
            {
                /* nRg */
                349564,
                /* nBg */
                803503,
            },
            /* 54 */
            {
                /* nRg */
                331675,
                /* nBg */
                835264,
            },
            /* 55 */
            {
                /* nRg */
                314741,
                /* nBg */
                867560,
            },
            /* 56 */
            {
                /* nRg */
                298792,
                /* nBg */
                900349,
            },
        },
        /* tRadiusLineList[512] */
        {
            /* 0 */
            {
                /* nK */
                5981,
                /* nB */
                -6589,
            },
            /* 1 */
            {
                /* nK */
                5107,
                /* nB */
                -5425,
            },
            /* 2 */
            {
                /* nK */
                4450,
                /* nB */
                -4549,
            },
            /* 3 */
            {
                /* nK */
                3937,
                /* nB */
                -3865,
            },
            /* 4 */
            {
                /* nK */
                3524,
                /* nB */
                -3315,
            },
            /* 5 */
            {
                /* nK */
                3185,
                /* nB */
                -2863,
            },
            /* 6 */
            {
                /* nK */
                2901,
                /* nB */
                -2484,
            },
            /* 7 */
            {
                /* nK */
                2658,
                /* nB */
                -2161,
            },
            /* 8 */
            {
                /* nK */
                2449,
                /* nB */
                -1883,
            },
            /* 9 */
            {
                /* nK */
                2345,
                /* nB */
                -1744,
            },
            /* 10 */
            {
                /* nK */
                2248,
                /* nB */
                -1614,
            },
            /* 11 */
            {
                /* nK */
                2157,
                /* nB */
                -1494,
            },
            /* 12 */
            {
                /* nK */
                2072,
                /* nB */
                -1381,
            },
            /* 13 */
            {
                /* nK */
                1993,
                /* nB */
                -1274,
            },
            /* 14 */
            {
                /* nK */
                1918,
                /* nB */
                -1175,
            },
            /* 15 */
            {
                /* nK */
                1847,
                /* nB */
                -1081,
            },
            /* 16 */
            {
                /* nK */
                1781,
                /* nB */
                -992,
            },
            /* 17 */
            {
                /* nK */
                1713,
                /* nB */
                -902,
            },
            /* 18 */
            {
                /* nK */
                1650,
                /* nB */
                -817,
            },
            /* 19 */
            {
                /* nK */
                1589,
                /* nB */
                -737,
            },
            /* 20 */
            {
                /* nK */
                1532,
                /* nB */
                -660,
            },
            /* 21 */
            {
                /* nK */
                1477,
                /* nB */
                -588,
            },
            /* 22 */
            {
                /* nK */
                1425,
                /* nB */
                -518,
            },
            /* 23 */
            {
                /* nK */
                1376,
                /* nB */
                -452,
            },
            /* 24 */
            {
                /* nK */
                1329,
                /* nB */
                -389,
            },
            /* 25 */
            {
                /* nK */
                1288,
                /* nB */
                -335,
            },
            /* 26 */
            {
                /* nK */
                1248,
                /* nB */
                -282,
            },
            /* 27 */
            {
                /* nK */
                1210,
                /* nB */
                -231,
            },
            /* 28 */
            {
                /* nK */
                1173,
                /* nB */
                -182,
            },
            /* 29 */
            {
                /* nK */
                1138,
                /* nB */
                -135,
            },
            /* 30 */
            {
                /* nK */
                1103,
                /* nB */
                -89,
            },
            /* 31 */
            {
                /* nK */
                1070,
                /* nB */
                -45,
            },
            /* 32 */
            {
                /* nK */
                1038,
                /* nB */
                -2,
            },
            /* 33 */
            {
                /* nK */
                1022,
                /* nB */
                18,
            },
            /* 34 */
            {
                /* nK */
                1007,
                /* nB */
                38,
            },
            /* 35 */
            {
                /* nK */
                992,
                /* nB */
                58,
            },
            /* 36 */
            {
                /* nK */
                977,
                /* nB */
                78,
            },
            /* 37 */
            {
                /* nK */
                962,
                /* nB */
                98,
            },
            /* 38 */
            {
                /* nK */
                948,
                /* nB */
                117,
            },
            /* 39 */
            {
                /* nK */
                933,
                /* nB */
                137,
            },
            /* 40 */
            {
                /* nK */
                919,
                /* nB */
                155,
            },
            /* 41 */
            {
                /* nK */
                907,
                /* nB */
                172,
            },
            /* 42 */
            {
                /* nK */
                895,
                /* nB */
                188,
            },
            /* 43 */
            {
                /* nK */
                883,
                /* nB */
                204,
            },
            /* 44 */
            {
                /* nK */
                871,
                /* nB */
                220,
            },
            /* 45 */
            {
                /* nK */
                859,
                /* nB */
                235,
            },
            /* 46 */
            {
                /* nK */
                847,
                /* nB */
                251,
            },
            /* 47 */
            {
                /* nK */
                836,
                /* nB */
                266,
            },
            /* 48 */
            {
                /* nK */
                825,
                /* nB */
                281,
            },
            /* 49 */
            {
                /* nK */
                775,
                /* nB */
                347,
            },
            /* 50 */
            {
                /* nK */
                728,
                /* nB */
                411,
            },
            /* 51 */
            {
                /* nK */
                682,
                /* nB */
                471,
            },
            /* 52 */
            {
                /* nK */
                639,
                /* nB */
                529,
            },
            /* 53 */
            {
                /* nK */
                597,
                /* nB */
                585,
            },
            /* 54 */
            {
                /* nK */
                556,
                /* nB */
                639,
            },
            /* 55 */
            {
                /* nK */
                517,
                /* nB */
                691,
            },
            /* 56 */
            {
                /* nK */
                479,
                /* nB */
                742,
            },
        },
        /* tInLeftBorderPntList[512] */
        {
            /* 0 */
            {
                /* nRg */
                1190910,
                /* nBg */
                208656,
            },
            /* 1 */
            {
                /* nRg */
                1154126,
                /* nBg */
                201264,
            },
            /* 2 */
            {
                /* nRg */
                1116765,
                /* nBg */
                195014,
            },
            /* 3 */
            {
                /* nRg */
                1078859,
                /* nBg */
                189908,
            },
            /* 4 */
            {
                /* nRg */
                1040439,
                /* nBg */
                185975,
            },
            /* 5 */
            {
                /* nRg */
                1001547,
                /* nBg */
                183260,
            },
            /* 6 */
            {
                /* nRg */
                962226,
                /* nBg */
                181760,
            },
            /* 7 */
            {
                /* nRg */
                927528,
                /* nBg */
                194563,
            },
            /* 8 */
            {
                /* nRg */
                893198,
                /* nBg */
                208352,
            },
            /* 9 */
            {
                /* nRg */
                874292,
                /* nBg */
                216426,
            },
            /* 10 */
            {
                /* nRg */
                855502,
                /* nBg */
                224804,
            },
            /* 11 */
            {
                /* nRg */
                836858,
                /* nBg */
                233476,
            },
            /* 12 */
            {
                /* nRg */
                818351,
                /* nBg */
                242441,
            },
            /* 13 */
            {
                /* nRg */
                799980,
                /* nBg */
                251690,
            },
            /* 14 */
            {
                /* nRg */
                781766,
                /* nBg */
                261232,
            },
            /* 15 */
            {
                /* nRg */
                763699,
                /* nBg */
                271057,
            },
            /* 16 */
            {
                /* nRg */
                745789,
                /* nBg */
                281165,
            },
            /* 17 */
            {
                /* nRg */
                726799,
                /* nBg */
                292301,
            },
            /* 18 */
            {
                /* nRg */
                707999,
                /* nBg */
                303741,
            },
            /* 19 */
            {
                /* nRg */
                689397,
                /* nBg */
                315506,
            },
            /* 20 */
            {
                /* nRg */
                672345,
                /* nBg */
                329608,
            },
            /* 21 */
            {
                /* nRg */
                657229,
                /* nBg */
                346374,
            },
            /* 22 */
            {
                /* nRg */
                639353,
                /* nBg */
                358986,
            },
            /* 23 */
            {
                /* nRg */
                622557,
                /* nBg */
                373072,
            },
            /* 24 */
            {
                /* nRg */
                610500,
                /* nBg */
                393221,
            },
            /* 25 */
            {
                /* nRg */
                598556,
                /* nBg */
                409759,
            },
            /* 26 */
            {
                /* nRg */
                589198,
                /* nBg */
                429170,
            },
            /* 27 */
            {
                /* nRg */
                575181,
                /* nBg */
                442650,
            },
            /* 28 */
            {
                /* nRg */
                561143,
                /* nBg */
                456055,
            },
            /* 29 */
            {
                /* nRg */
                547529,
                /* nBg */
                469891,
            },
            /* 30 */
            {
                /* nRg */
                533618,
                /* nBg */
                483355,
            },
            /* 31 */
            {
                /* nRg */
                519703,
                /* nBg */
                496790,
            },
            /* 32 */
            {
                /* nRg */
                500866,
                /* nBg */
                505240,
            },
            /* 33 */
            {
                /* nRg */
                486129,
                /* nBg */
                504225,
            },
            /* 34 */
            {
                /* nRg */
                473282,
                /* nBg */
                505317,
            },
            /* 35 */
            {
                /* nRg */
                462442,
                /* nBg */
                508532,
            },
            /* 36 */
            {
                /* nRg */
                451653,
                /* nBg */
                511915,
            },
            /* 37 */
            {
                /* nRg */
                444827,
                /* nBg */
                519129,
            },
            /* 38 */
            {
                /* nRg */
                438053,
                /* nBg */
                526396,
            },
            /* 39 */
            {
                /* nRg */
                431332,
                /* nBg */
                533704,
            },
            /* 40 */
            {
                /* nRg */
                424663,
                /* nBg */
                541076,
            },
            /* 41 */
            {
                /* nRg */
                418843,
                /* nBg */
                547587,
            },
            /* 42 */
            {
                /* nRg */
                413076,
                /* nBg */
                554151,
            },
            /* 43 */
            {
                /* nRg */
                407351,
                /* nBg */
                560737,
            },
            /* 44 */
            {
                /* nRg */
                401668,
                /* nBg */
                567374,
            },
            /* 45 */
            {
                /* nRg */
                396037,
                /* nBg */
                574043,
            },
            /* 46 */
            {
                /* nRg */
                390437,
                /* nBg */
                580754,
            },
            /* 47 */
            {
                /* nRg */
                384890,
                /* nBg */
                587496,
            },
            /* 48 */
            {
                /* nRg */
                379396,
                /* nBg */
                594280,
            },
            /* 49 */
            {
                /* nRg */
                355195,
                /* nBg */
                625245,
            },
            /* 50 */
            {
                /* nRg */
                331948,
                /* nBg */
                656922,
            },
            /* 51 */
            {
                /* nRg */
                309655,
                /* nBg */
                689292,
            },
            /* 52 */
            {
                /* nRg */
                288348,
                /* nBg */
                722312,
            },
            /* 53 */
            {
                /* nRg */
                268047,
                /* nBg */
                755960,
            },
            /* 54 */
            {
                /* nRg */
                248754,
                /* nBg */
                790196,
            },
            /* 55 */
            {
                /* nRg */
                230508,
                /* nBg */
                824999,
            },
            /* 56 */
            {
                /* nRg */
                213322,
                /* nBg */
                860336,
            },
        },
        /* tInRightBorderPntList[512] */
        {
            /* 0 */
            {
                /* nRg */
                1220993,
                /* nBg */
                384356,
            },
            /* 1 */
            {
                /* nRg */
                1189169,
                /* nBg */
                376051,
            },
            /* 2 */
            {
                /* nRg */
                1156737,
                /* nBg */
                368732,
            },
            /* 3 */
            {
                /* nRg */
                1123727,
                /* nBg */
                362419,
            },
            /* 4 */
            {
                /* nRg */
                1090173,
                /* nBg */
                357155,
            },
            /* 5 */
            {
                /* nRg */
                1056105,
                /* nBg */
                352961,
            },
            /* 6 */
            {
                /* nRg */
                1021554,
                /* nBg */
                349857,
            },
            /* 7 */
            {
                /* nRg */
                991586,
                /* nBg */
                360920,
            },
            /* 8 */
            {
                /* nRg */
                961943,
                /* nBg */
                372821,
            },
            /* 9 */
            {
                /* nRg */
                945606,
                /* nBg */
                379794,
            },
            /* 10 */
            {
                /* nRg */
                929384,
                /* nBg */
                387029,
            },
            /* 11 */
            {
                /* nRg */
                913289,
                /* nBg */
                394516,
            },
            /* 12 */
            {
                /* nRg */
                897298,
                /* nBg */
                402255,
            },
            /* 13 */
            {
                /* nRg */
                881433,
                /* nBg */
                410245,
            },
            /* 14 */
            {
                /* nRg */
                865704,
                /* nBg */
                418487,
            },
            /* 15 */
            {
                /* nRg */
                850102,
                /* nBg */
                426980,
            },
            /* 16 */
            {
                /* nRg */
                834635,
                /* nBg */
                435704,
            },
            /* 17 */
            {
                /* nRg */
                818235,
                /* nBg */
                445320,
            },
            /* 18 */
            {
                /* nRg */
                801993,
                /* nBg */
                455208,
            },
            /* 19 */
            {
                /* nRg */
                785929,
                /* nBg */
                465369,
            },
            /* 20 */
            {
                /* nRg */
                770032,
                /* nBg */
                475791,
            },
            /* 21 */
            {
                /* nRg */
                754314,
                /* nBg */
                486487,
            },
            /* 22 */
            {
                /* nRg */
                738785,
                /* nBg */
                497444,
            },
            /* 23 */
            {
                /* nRg */
                723434,
                /* nBg */
                508664,
            },
            /* 24 */
            {
                /* nRg */
                708282,
                /* nBg */
                520146,
            },
            /* 25 */
            {
                /* nRg */
                694745,
                /* nBg */
                530737,
            },
            /* 26 */
            {
                /* nRg */
                681365,
                /* nBg */
                541548,
            },
            /* 27 */
            {
                /* nRg */
                668142,
                /* nBg */
                552547,
            },
            /* 28 */
            {
                /* nRg */
                655098,
                /* nBg */
                563756,
            },
            /* 29 */
            {
                /* nRg */
                642232,
                /* nBg */
                575165,
            },
            /* 30 */
            {
                /* nRg */
                629534,
                /* nBg */
                586762,
            },
            /* 31 */
            {
                /* nRg */
                617014,
                /* nBg */
                598548,
            },
            /* 32 */
            {
                /* nRg */
                604672,
                /* nBg */
                610533,
            },
            /* 33 */
            {
                /* nRg */
                598590,
                /* nBg */
                616573,
            },
            /* 34 */
            {
                /* nRg */
                592550,
                /* nBg */
                622665,
            },
            /* 35 */
            {
                /* nRg */
                586552,
                /* nBg */
                628810,
            },
            /* 36 */
            {
                /* nRg */
                580607,
                /* nBg */
                634997,
            },
            /* 37 */
            {
                /* nRg */
                574704,
                /* nBg */
                641225,
            },
            /* 38 */
            {
                /* nRg */
                568852,
                /* nBg */
                647496,
            },
            /* 39 */
            {
                /* nRg */
                563043,
                /* nBg */
                653808,
            },
            /* 40 */
            {
                /* nRg */
                557287,
                /* nBg */
                660173,
            },
            /* 41 */
            {
                /* nRg */
                552274,
                /* nBg */
                665804,
            },
            /* 42 */
            {
                /* nRg */
                547283,
                /* nBg */
                671466,
            },
            /* 43 */
            {
                /* nRg */
                542344,
                /* nBg */
                677160,
            },
            /* 44 */
            {
                /* nRg */
                537437,
                /* nBg */
                682885,
            },
            /* 45 */
            {
                /* nRg */
                532572,
                /* nBg */
                688652,
            },
            /* 46 */
            {
                /* nRg */
                527738,
                /* nBg */
                694440,
            },
            /* 47 */
            {
                /* nRg */
                522946,
                /* nBg */
                700271,
            },
            /* 48 */
            {
                /* nRg */
                518196,
                /* nBg */
                706122,
            },
            /* 49 */
            {
                /* nRg */
                497298,
                /* nBg */
                732871,
            },
            /* 50 */
            {
                /* nRg */
                477217,
                /* nBg */
                760228,
            },
            /* 51 */
            {
                /* nRg */
                457966,
                /* nBg */
                788183,
            },
            /* 52 */
            {
                /* nRg */
                439563,
                /* nBg */
                816704,
            },
            /* 53 */
            {
                /* nRg */
                422031,
                /* nBg */
                845760,
            },
            /* 54 */
            {
                /* nRg */
                405369,
                /* nBg */
                875330,
            },
            /* 55 */
            {
                /* nRg */
                389609,
                /* nBg */
                905393,
            },
            /* 56 */
            {
                /* nRg */
                374761,
                /* nBg */
                935907,
            },
        },
        /* tOutLeftBorderPntList[512] */
        {
            /* 0 */
            {
                /* nRg */
                1180288,
                /* nBg */
                146643,
            },
            /* 1 */
            {
                /* nRg */
                1141763,
                /* nBg */
                139586,
            },
            /* 2 */
            {
                /* nRg */
                1102662,
                /* nBg */
                133693,
            },
            /* 3 */
            {
                /* nRg */
                1063025,
                /* nBg */
                129017,
            },
            /* 4 */
            {
                /* nRg */
                1022886,
                /* nBg */
                125567,
            },
            /* 5 */
            {
                /* nRg */
                982296,
                /* nBg */
                123365,
            },
            /* 6 */
            {
                /* nRg */
                941286,
                /* nBg */
                122442,
            },
            /* 7 */
            {
                /* nRg */
                904911,
                /* nBg */
                135854,
            },
            /* 8 */
            {
                /* nRg */
                868934,
                /* nBg */
                150303,
            },
            /* 9 */
            {
                /* nRg */
                849116,
                /* nBg */
                158765,
            },
            /* 10 */
            {
                /* nRg */
                829434,
                /* nBg */
                167552,
            },
            /* 11 */
            {
                /* nRg */
                809889,
                /* nBg */
                176633,
            },
            /* 12 */
            {
                /* nRg */
                790480,
                /* nBg */
                186028,
            },
            /* 13 */
            {
                /* nRg */
                771238,
                /* nBg */
                195727,
            },
            /* 14 */
            {
                /* nRg */
                752144,
                /* nBg */
                205731,
            },
            /* 15 */
            {
                /* nRg */
                733206,
                /* nBg */
                216028,
            },
            /* 16 */
            {
                /* nRg */
                714437,
                /* nBg */
                226618,
            },
            /* 17 */
            {
                /* nRg */
                694524,
                /* nBg */
                238289,
            },
            /* 18 */
            {
                /* nRg */
                674822,
                /* nBg */
                250285,
            },
            /* 19 */
            {
                /* nRg */
                655329,
                /* nBg */
                262616,
            },
            /* 20 */
            {
                /* nRg */
                636943,
                /* nBg */
                276631,
            },
            /* 21 */
            {
                /* nRg */
                622430,
                /* nBg */
                296147,
            },
            /* 22 */
            {
                /* nRg */
                610084,
                /* nBg */
                318227,
            },
            /* 23 */
            {
                /* nRg */
                596715,
                /* nBg */
                338330,
            },
            /* 24 */
            {
                /* nRg */
                578710,
                /* nBg */
                351963,
            },
            /* 25 */
            {
                /* nRg */
                562625,
                /* nBg */
                364547,
            },
            /* 26 */
            {
                /* nRg */
                546725,
                /* nBg */
                377389,
            },
            /* 27 */
            {
                /* nRg */
                531030,
                /* nBg */
                390466,
            },
            /* 28 */
            {
                /* nRg */
                515530,
                /* nBg */
                403780,
            },
            /* 29 */
            {
                /* nRg */
                500235,
                /* nBg */
                417329,
            },
            /* 30 */
            {
                /* nRg */
                485156,
                /* nBg */
                431105,
            },
            /* 31 */
            {
                /* nRg */
                470283,
                /* nBg */
                445116,
            },
            /* 32 */
            {
                /* nRg */
                455624,
                /* nBg */
                459343,
            },
            /* 33 */
            {
                /* nRg */
                448388,
                /* nBg */
                466528,
            },
            /* 34 */
            {
                /* nRg */
                437809,
                /* nBg */
                470418,
            },
            /* 35 */
            {
                /* nRg */
                425624,
                /* nBg */
                472861,
            },
            /* 36 */
            {
                /* nRg */
                410510,
                /* nBg */
                472638,
            },
            /* 37 */
            {
                /* nRg */
                398983,
                /* nBg */
                476033,
            },
            /* 38 */
            {
                /* nRg */
                391884,
                /* nBg */
                483645,
            },
            /* 39 */
            {
                /* nRg */
                384838,
                /* nBg */
                491321,
            },
            /* 40 */
            {
                /* nRg */
                377854,
                /* nBg */
                499038,
            },
            /* 41 */
            {
                /* nRg */
                371762,
                /* nBg */
                505865,
            },
            /* 42 */
            {
                /* nRg */
                365712,
                /* nBg */
                512743,
            },
            /* 43 */
            {
                /* nRg */
                359714,
                /* nBg */
                519653,
            },
            /* 44 */
            {
                /* nRg */
                353758,
                /* nBg */
                526605,
            },
            /* 45 */
            {
                /* nRg */
                347844,
                /* nBg */
                533599,
            },
            /* 46 */
            {
                /* nRg */
                341983,
                /* nBg */
                540625,
            },
            /* 47 */
            {
                /* nRg */
                336173,
                /* nBg */
                547692,
            },
            /* 48 */
            {
                /* nRg */
                330406,
                /* nBg */
                554802,
            },
            /* 49 */
            {
                /* nRg */
                305041,
                /* nBg */
                587265,
            },
            /* 50 */
            {
                /* nRg */
                280672,
                /* nBg */
                620463,
            },
            /* 51 */
            {
                /* nRg */
                257310,
                /* nBg */
                654385,
            },
            /* 52 */
            {
                /* nRg */
                234975,
                /* nBg */
                688998,
            },
            /* 53 */
            {
                /* nRg */
                213700,
                /* nBg */
                724262,
            },
            /* 54 */
            {
                /* nRg */
                193483,
                /* nBg */
                760144,
            },
            /* 55 */
            {
                /* nRg */
                174357,
                /* nBg */
                796624,
            },
            /* 56 */
            {
                /* nRg */
                156332,
                /* nBg */
                833670,
            },
        },
        /* tOutRightBorderPntList[512] */
        {
            /* 0 */
            {
                /* nRg */
                1231605,
                /* nBg */
                446368,
            },
            /* 1 */
            {
                /* nRg */
                1201532,
                /* nBg */
                437739,
            },
            /* 2 */
            {
                /* nRg */
                1170840,
                /* nBg */
                430042,
            },
            /* 3 */
            {
                /* nRg */
                1139561,
                /* nBg */
                423310,
            },
            /* 4 */
            {
                /* nRg */
                1107726,
                /* nBg */
                417574,
            },
            /* 5 */
            {
                /* nRg */
                1075357,
                /* nBg */
                412856,
            },
            /* 6 */
            {
                /* nRg */
                1042494,
                /* nBg */
                409186,
            },
            /* 7 */
            {
                /* nRg */
                1014193,
                /* nBg */
                419630,
            },
            /* 8 */
            {
                /* nRg */
                986207,
                /* nBg */
                430860,
            },
            /* 9 */
            {
                /* nRg */
                970782,
                /* nBg */
                437455,
            },
            /* 10 */
            {
                /* nRg */
                955462,
                /* nBg */
                444282,
            },
            /* 11 */
            {
                /* nRg */
                940258,
                /* nBg */
                451360,
            },
            /* 12 */
            {
                /* nRg */
                925159,
                /* nBg */
                458668,
            },
            /* 13 */
            {
                /* nRg */
                910185,
                /* nBg */
                466207,
            },
            /* 14 */
            {
                /* nRg */
                895327,
                /* nBg */
                473988,
            },
            /* 15 */
            {
                /* nRg */
                880594,
                /* nBg */
                482009,
            },
            /* 16 */
            {
                /* nRg */
                865987,
                /* nBg */
                490251,
            },
            /* 17 */
            {
                /* nRg */
                850500,
                /* nBg */
                499332,
            },
            /* 18 */
            {
                /* nRg */
                835170,
                /* nBg */
                508664,
            },
            /* 19 */
            {
                /* nRg */
                819997,
                /* nBg */
                518259,
            },
            /* 20 */
            {
                /* nRg */
                804992,
                /* nBg */
                528105,
            },
            /* 21 */
            {
                /* nRg */
                790154,
                /* nBg */
                538203,
            },
            /* 22 */
            {
                /* nRg */
                775485,
                /* nBg */
                548552,
            },
            /* 23 */
            {
                /* nRg */
                760994,
                /* nBg */
                559143,
            },
            /* 24 */
            {
                /* nRg */
                746680,
                /* nBg */
                569985,
            },
            /* 25 */
            {
                /* nRg */
                733888,
                /* nBg */
                579988,
            },
            /* 26 */
            {
                /* nRg */
                721253,
                /* nBg */
                590191,
            },
            /* 27 */
            {
                /* nRg */
                708785,
                /* nBg */
                600582,
            },
            /* 28 */
            {
                /* nRg */
                696464,
                /* nBg */
                611163,
            },
            /* 29 */
            {
                /* nRg */
                684311,
                /* nBg */
                621931,
            },
            /* 30 */
            {
                /* nRg */
                672315,
                /* nBg */
                632889,
            },
            /* 31 */
            {
                /* nRg */
                660498,
                /* nBg */
                644014,
            },
            /* 32 */
            {
                /* nRg */
                648848,
                /* nBg */
                655329,
            },
            /* 33 */
            {
                /* nRg */
                643102,
                /* nBg */
                661043,
            },
            /* 34 */
            {
                /* nRg */
                637398,
                /* nBg */
                666800,
            },
            /* 35 */
            {
                /* nRg */
                631736,
                /* nBg */
                672588,
            },
            /* 36 */
            {
                /* nRg */
                626115,
                /* nBg */
                678429,
            },
            /* 37 */
            {
                /* nRg */
                620547,
                /* nBg */
                684311,
            },
            /* 38 */
            {
                /* nRg */
                615021,
                /* nBg */
                690236,
            },
            /* 39 */
            {
                /* nRg */
                609537,
                /* nBg */
                696202,
            },
            /* 40 */
            {
                /* nRg */
                604106,
                /* nBg */
                702210,
            },
            /* 41 */
            {
                /* nRg */
                599366,
                /* nBg */
                707527,
            },
            /* 42 */
            {
                /* nRg */
                594658,
                /* nBg */
                712874,
            },
            /* 43 */
            {
                /* nRg */
                589992,
                /* nBg */
                718254,
            },
            /* 44 */
            {
                /* nRg */
                585357,
                /* nBg */
                723664,
            },
            /* 45 */
            {
                /* nRg */
                580754,
                /* nBg */
                729096,
            },
            /* 46 */
            {
                /* nRg */
                576193,
                /* nBg */
                734569,
            },
            /* 47 */
            {
                /* nRg */
                571673,
                /* nBg */
                740074,
            },
            /* 48 */
            {
                /* nRg */
                567185,
                /* nBg */
                745600,
            },
            /* 49 */
            {
                /* nRg */
                547451,
                /* nBg */
                770850,
            },
            /* 50 */
            {
                /* nRg */
                528493,
                /* nBg */
                796687,
            },
            /* 51 */
            {
                /* nRg */
                510310,
                /* nBg */
                823080,
            },
            /* 52 */
            {
                /* nRg */
                492936,
                /* nBg */
                850018,
            },
            /* 53 */
            {
                /* nRg */
                476379,
                /* nBg */
                877448,
            },
            /* 54 */
            {
                /* nRg */
                460650,
                /* nBg */
                905372,
            },
            /* 55 */
            {
                /* nRg */
                445771,
                /* nBg */
                933757,
            },
            /* 56 */
            {
                /* nRg */
                431751,
                /* nBg */
                962582,
            },
        },
        /* nIllumNum */
        6,
        /* tIllumList[16] */
        {
            /* 0 */
            {
                /* szName[32] */
                "H",
                /* nCct */
                2300,
                /* nRadius */
                0,
                /* tCoord */
                {
                    /* nRg */
                    938476,
                    /* nBg */
                    316670,
                },
            },
            /* 1 */
            {
                /* szName[32] */
                "A",
                /* nCct */
                2856,
                /* nRadius */
                0,
                /* tCoord */
                {
                    /* nRg */
                    792723,
                    /* nBg */
                    362807,
                },
            },
            /* 2 */
            {
                /* szName[32] */
                "TL84",
                /* nCct */
                3800,
                /* nRadius */
                0,
                /* tCoord */
                {
                    /* nRg */
                    621806,
                    /* nBg */
                    407896,
                },
            },
            /* 3 */
            {
                /* szName[32] */
                "D50",
                /* nCct */
                5000,
                /* nRadius */
                0,
                /* tCoord */
                {
                    /* nRg */
                    562037,
                    /* nBg */
                    567280,
                },
            },
            /* 4 */
            {
                /* szName[32] */
                "D65",
                /* nCct */
                6500,
                /* nRadius */
                0,
                /* tCoord */
                {
                    /* nRg */
                    486539,
                    /* nBg */
                    596640,
                },
            },
            /* 5 */
            {
                /* szName[32] */
                "D75",
                /* nCct */
                7500,
                /* nRadius */
                0,
                /* tCoord */
                {
                    /* nRg */
                    470811,
                    /* nBg */
                    667943,
                },
            },
        },
        /* nExtIllumNum */
        1,
        /* tExtIllumList[32] */
        {
            /* 0 */
            {
                /* szName[32] */
                "CWF",
                /* nCct */
                4100,
                /* nRadius */
                31457,
                /* tCoord */
                {
                    /* nRg */
                    560988,
                    /* nBg */
                    402653,
                },
            },
        },
        /* nMLCNum */
        0,
        /* tMLCZoneList[15] */
        {
            /* 0 */
            {
                /* nEnable */
                0,
                /* szDescription[32] */
                "",
                /* nZoneType */
                0,
                /* tPoly */
                {
                    /* nPntCnt */
                    4,
                    /* tPntArray[15] */
                    {
                        /* 0 */
                        {
                            /* nRg */
                            0,
                            /* nBg */
                            0,
                        },
                        /* 1 */
                        {
                            /* nRg */
                            0,
                            /* nBg */
                            0,
                        },
                        /* 2 */
                        {
                            /* nRg */
                            0,
                            /* nBg */
                            0,
                        },
                        /* 3 */
                        {
                            /* nRg */
                            0,
                            /* nBg */
                            0,
                        },
                    },
                },
                /* nDetectionZoneGroupNum */
                0,
                /* tGroupDetectionZoneList[10] */
                {
                    /* 0 */
                    {
                        /* nDetectionZoneNum */
                        0,
                        /* tDetectionZoneList[5] */
                        {
                            /* 0 */
                            {
                                /* szDescription[32] */
                                "",
                                /* nZoneType */
                                0,
                                /* tPoly */
                                {
                                    /* nPntCnt */
                                    0,
                                    /* tPntArray[15] */
                                    {
                                        /* 0 */
                                        {
                                            /* nRg */
                                            0,
                                            /* nBg */
                                            0,
                                        },
                                    },
                                },
                                /* nLux[2] */
                                {0, 0, /*0 - 1*/},
                            },
                        },
                    },
                },
                /* nTrigerType1st */
                0,
                /* nTrigerType2nd */
                0,
                /* nTrigerType3rd */
                0,
                /* nTrigerValue1st[2] */
                {0, 0, /*0 - 1*/},
                /* nTrigerValue2nd[2] */
                {0, 0, /*0 - 1*/},
                /* nTrigerValue3rd */
                0,
            },
        },
        /* tInitParam */
        {
            /* tGains */
            {
                /* nGainR */
                500,
                /* nGainGr */
                256,
                /* nGainGb */
                256,
                /* nGainB */
                500,
            },
            /* nDampRatio */
            10000,
        },
        /* nMode */
        0,
        /* nIndex */
        0,
        /* nColorDiffCnt */
        10,
        /* nColorDiffList[16] */
        {1, 20, 40, 80, 150, 200, 350, 500, 800, 1024, 0, 0, 0, 0, 0, 0,/*0 - 15*/},
        /* nDampRatioList[16] */
        {1038090, 956147, 903718, 801289, 700000, 529145, 300000, 101145, 30000, 10485, 0, 0, 0, 0, 0, 0,/*0 - 15*/},
        /* nToleranceRg */
        3145,
        /* nToleranceBg */
        3145,
        /* nLuxVeryDarkStart */
        0,
        /* nLuxVeryDarkEnd */
        8192,
        /* nLuxDarkStart */
        10240,
        /* nLuxDarkEnd */
        51200,
        /* nLuxIndoorStart */
        61440,
        /* nLuxIndoorEnd */
        409600,
        /* nLuxTransInStart */
        460800,
        /* nLuxTransInEnd */
        972800,
        /* nLuxTransOutStart */
        1024000,
        /* nLuxTransOutEnd */
        1894400,
        /* nLuxOutdoorStart */
        1945600,
        /* nLuxOutdoorEnd */
        4044800,
        /* nLuxBrightStart */
        4096000,
        /* nLuxBrightEnd */
        10137600,
        /* nLuxVeryBrightStart */
        10240000,
        /* nCctMinInner */
        1500,
        /* nCctMaxInner */
        7800,
        /* nCctMinOuter */
        1300,
        /* nCctMaxOuter */
        10000,
        /* nCctSplitHtoA */
        2450,
        /* nCctSplitAtoF */
        3300,
        /* nCctSplitFtoD5 */
        4400,
        /* nCctSplitD5toD6 */
        5350,
        /* nCctSplitD6toS */
        6800,
        /* nGridWeightEnable */
        0,
        /* nGridWeightRow */
        9,
        /* nGridWeightColumn */
        9,
        /* nGridWeightTable[54][72] */
        {
            {1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, /*0 - 8*/},
            {1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, /*0 - 8*/},
            {1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, /*0 - 8*/},
            {1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, /*0 - 8*/},
            {1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, /*0 - 8*/},
            {1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, /*0 - 8*/},
            {1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, /*0 - 8*/},
            {1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, /*0 - 8*/},
            {1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, /*0 - 8*/},
        },
        /* nGrayZoneLuxWeight[24][8] */
        {
            {300, 300, 300, 200, 0, 0, 0, 0, /*0 - 7*/},
            {800, 800, 700, 400, 0, 0, 0, 0, /*0 - 7*/},
            {800, 800, 700, 400, 0, 0, 0, 0, /*0 - 7*/},
            {300, 100, 50, 50, 0, 0, 0, 0, /*0 - 7*/},
            {600, 150, 50, 50, 0, 0, 0, 0, /*0 - 7*/},
            {800, 600, 200, 200, 0, 0, 0, 0, /*0 - 7*/},
            {800, 600, 200, 200, 0, 0, 0, 0, /*0 - 7*/},
            {600, 150, 50, 50, 0, 0, 0, 0, /*0 - 7*/},
            {100, 100, 100, 0, 0, 0, 0, 0, /*0 - 7*/},
            {700, 700, 600, 500, 450, 450, 400, 400, /*0 - 7*/},
            {700, 700, 600, 500, 450, 450, 400, 400, /*0 - 7*/},
            {100, 10, 10, 0, 0, 0, 0, 0, /*0 - 7*/},
            {100, 100, 400, 400, 450, 450, 250, 200, /*0 - 7*/},
            {500, 800, 1000, 1000, 900, 900, 500, 400, /*0 - 7*/},
            {500, 800, 1000, 1000, 900, 900, 900, 900, /*0 - 7*/},
            {400, 10, 10, 0, 0, 0, 0, 0, /*0 - 7*/},
            {100, 300, 400, 500, 500, 400, 300, 300, /*0 - 7*/},
            {400, 1000, 1000, 1000, 1000, 700, 500, 450, /*0 - 7*/},
            {400, 1000, 1000, 1000, 1000, 700, 700, 700, /*0 - 7*/},
            {100, 10, 10, 0, 0, 0, 0, 0, /*0 - 7*/},
            {100, 200, 200, 200, 200, 200, 200, 200, /*0 - 7*/},
            {500, 1000, 500, 500, 500, 500, 400, 400, /*0 - 7*/},
            {500, 1000, 500, 500, 500, 500, 500, 600, /*0 - 7*/},
            {100, 200, 300, 300, 100, 0, 0, 0, /*0 - 7*/},
        },
        /* nExtIlllumLuxWeight[32][8] */
        {
            {1000, 1000, 1000, 200, 50, 0, 0, 0, /*0 - 7*/},
            {0, 0, 0, 0, 0, 0, 0, 0, /*0 - 7*/},
            {0, 0, 0, 0, 0, 0, 0, 0, /*0 - 7*/},
            {0, 0, 0, 0, 0, 0, 0, 0, /*0 - 7*/},
            {0, 0, 0, 0, 0, 0, 0, 0, /*0 - 7*/},
            {0, 0, 0, 0, 0, 0, 0, 0, /*0 - 7*/},
            {0, 0, 0, 0, 0, 0, 0, 0, /*0 - 7*/},
            {0, 0, 0, 0, 0, 0, 0, 0, /*0 - 7*/},
            {0, 0, 0, 0, 0, 0, 0, 0, /*0 - 7*/},
            {0, 0, 0, 0, 0, 0, 0, 0, /*0 - 7*/},
            {0, 0, 0, 0, 0, 0, 0, 0, /*0 - 7*/},
            {0, 0, 0, 0, 0, 0, 0, 0, /*0 - 7*/},
            {0, 0, 0, 0, 0, 0, 0, 0, /*0 - 7*/},
            {0, 0, 0, 0, 0, 0, 0, 0, /*0 - 7*/},
            {0, 0, 0, 0, 0, 0, 0, 0, /*0 - 7*/},
            {0, 0, 0, 0, 0, 0, 0, 0, /*0 - 7*/},
            {0, 0, 0, 0, 0, 0, 0, 0, /*0 - 7*/},
            {0, 0, 0, 0, 0, 0, 0, 0, /*0 - 7*/},
            {0, 0, 0, 0, 0, 0, 0, 0, /*0 - 7*/},
            {0, 0, 0, 0, 0, 0, 0, 0, /*0 - 7*/},
            {0, 0, 0, 0, 0, 0, 0, 0, /*0 - 7*/},
            {0, 0, 0, 0, 0, 0, 0, 0, /*0 - 7*/},
            {0, 0, 0, 0, 0, 0, 0, 0, /*0 - 7*/},
            {0, 0, 0, 0, 0, 0, 0, 0, /*0 - 7*/},
            {0, 0, 0, 0, 0, 0, 0, 0, /*0 - 7*/},
            {0, 0, 0, 0, 0, 0, 0, 0, /*0 - 7*/},
            {0, 0, 0, 0, 0, 0, 0, 0, /*0 - 7*/},
            {0, 0, 0, 0, 0, 0, 0, 0, /*0 - 7*/},
            {0, 0, 0, 0, 0, 0, 0, 0, /*0 - 7*/},
            {0, 0, 0, 0, 0, 0, 0, 0, /*0 - 7*/},
            {0, 0, 0, 0, 0, 0, 0, 0, /*0 - 7*/},
            {0, 0, 0, 0, 0, 0, 0, 0, /*0 - 7*/},
        },
        /* nLumaWeightNum */
        8,
        /* nLumaSplitList[32] */
        {51, 819, 3072, 8192, 20480, 40960, 92160, 215040, /*0 - 7*/},
        /* nLumaWeightList[8][32] */
        {
            {51, 102, 614, 1024, 1024, 1024, 1024, 512, /*0 - 7*/},
            {102, 205, 819, 1024, 1024, 1024, 1024, 512, /*0 - 7*/},
            {100, 200, 820, 1024, 1024, 1024, 1024, 820, /*0 - 7*/},
            {0, 200, 820, 1024, 1024, 1024, 1024, 820, /*0 - 7*/},
            {0, 200, 820, 1024, 1024, 1024, 1024, 820, /*0 - 7*/},
            {0, 200, 820, 1024, 1024, 1024, 1024, 820, /*0 - 7*/},
            {0, 200, 820, 1024, 1024, 1024, 1024, 820, /*0 - 7*/},
            {0, 200, 820, 1024, 1024, 1024, 1024, 820, /*0 - 7*/},
        },
        /* bMixLightEn */
        1,
        /* nMixLightProba_0_CctStd[8] */
        {400, 400, 400, 450, 600, 9998, 9998, 9998, /*0 - 7*/},
        /* nMixLightProba_100_CctStd[8] */
        {600, 600, 600, 800, 1000, 9999, 9999, 9999, /*0 - 7*/},
        /* nMixLightProba_100_SatDiscnt[8] */
        {100, 100, 100, 100, 100, 100, 100, 100, /*0 - 7*/},
        /* nMixLightKneeNum */
        8,
        /* nMixLightKneeCctList[32] */
        {2300, 2800, 3500, 4600, 5500, 6500, 7500, 8500, /*0 - 7*/},
        /* nMixLightKneeWtList[8][32] */
        {
            {100, 512, 614, 640, 820, 820, 820, 820, /*0 - 7*/},
            {100, 512, 614, 820, 820, 820, 820, 820, /*0 - 7*/},
            {205, 307, 310, 820, 820, 820, 820, 820, /*0 - 7*/},
            {205, 205, 310, 820, 820, 820, 820, 820, /*0 - 7*/},
            {407, 407, 512, 1024, 1024, 820, 820, 820, /*0 - 7*/},
            {512, 1024, 1024, 1024, 1024, 1024, 1024, 1024, /*0 - 7*/},
            {512, 1024, 1024, 1024, 1024, 1024, 1024, 1024, /*0 - 7*/},
            {512, 1024, 1024, 1024, 1024, 1024, 1024, 1024, /*0 - 7*/},
        },
        /* tDomParamList[4] */
        {
            /* 0 */
            {
                /* nDominantEnable */
                1,
                /* nDomMinCctThresh */
                2000,
                /* nDomMaxCctThresh */
                2500,
                /* nDom2AllRatioThresh */
                409,
                /* nDom2MinorRatioThresh */
                4096,
                /* nMinorWeight */
                0,
                /* nSmoothPercent */
                204,
            },
            /* 1 */
            {
                /* nDominantEnable */
                1,
                /* nDomMinCctThresh */
                2500,
                /* nDomMaxCctThresh */
                3300,
                /* nDom2AllRatioThresh */
                409,
                /* nDom2MinorRatioThresh */
                4096,
                /* nMinorWeight */
                0,
                /* nSmoothPercent */
                204,
            },
            /* 2 */
            {
                /* nDominantEnable */
                1,
                /* nDomMinCctThresh */
                3300,
                /* nDomMaxCctThresh */
                4600,
                /* nDom2AllRatioThresh */
                409,
                /* nDom2MinorRatioThresh */
                4096,
                /* nMinorWeight */
                0,
                /* nSmoothPercent */
                204,
            },
            /* 3 */
            {
                /* nDominantEnable */
                1,
                /* nDomMinCctThresh */
                4600,
                /* nDomMaxCctThresh */
                8500,
                /* nDom2AllRatioThresh */
                409,
                /* nDom2MinorRatioThresh */
                4096,
                /* nMinorWeight */
                0,
                /* nSmoothPercent */
                204,
            },
        },
        /* nTmpoStabTriggerAvgBlkWt */
        10,
        /* nPlanckianLocusProjEn */
        0,
        /* nPlanckianLocusNotProjLux */
        4096000,
        /* nPlanckianLocusFullProjLux */
        10240000,
        /* nSpatialEn */
        0,
        /* nSpatialSegmetNum */
        2,
        /* nSpatialStartLux[32] */
        {4096000, 11264000, /*0 - 1*/},
        /* nSpatialEndLux[32] */
        {10240000, 1024000000, /*0 - 1*/},
        /* nSpatialRg[32] */
        {488636, 622854, /*0 - 1*/},
        /* nSpatialBg[32] */
        {636485, 615514, /*0 - 1*/},
        /* nFusionGrayZoneConfid_0_AvgBlkWeight */
        20,
        /* nFusionGrayZoneConfid_100_AvgBlkWeight */
        500,
        /* nFusionSpatialConfid_0_Lux */
        5120000,
        /* nFusionSpatialConfid_100_Lux */
        15360000,
        /* nFusionWeightGrayZone */
        1024,
        /* nFusionWeightSpatial */
        0,
        /* nPreferEn */
        0,
        /* nPreferCctNum */
        10,
        /* nPreferSrcCctList[32] */
        {1800, 2300, 2500, 2850, 3800, 4400, 5350, 6500, 7500, 12000, /*0 - 9*/},
        /* nPreferDstCct[8][32] */
        {
            {1800, 2300, 2500, 2850, 4000, 4400, 5350, 6500, 7500, 12000, /*0 - 9*/},
            {1800, 2300, 2500, 2850, 3800, 4400, 5350, 6500, 7500, 12000, /*0 - 9*/},
            {1800, 2300, 2500, 2850, 3800, 4400, 5350, 6500, 7500, 12000, /*0 - 9*/},
            {1800, 2300, 2500, 2850, 3800, 4400, 5350, 6500, 7500, 12000, /*0 - 9*/},
            {1800, 2300, 2500, 2850, 3800, 4400, 5350, 6500, 7500, 12000, /*0 - 9*/},
            {1800, 2300, 2500, 2850, 3800, 4400, 5350, 6500, 7500, 12000, /*0 - 9*/},
            {1800, 2300, 2500, 2850, 3800, 4400, 5350, 6500, 7500, 12000, /*0 - 9*/},
            {1800, 2300, 2500, 2850, 3800, 4400, 5350, 6500, 7500, 12000, /*0 - 9*/},
        },
        /* nPreferGrShift[8][32] */
        {
            {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 9*/},
            {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 9*/},
            {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 9*/},
            {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 9*/},
            {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 9*/},
            {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 9*/},
            {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 9*/},
            {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 9*/},
        },
        /* nGreenCutEn */
        0,
        /* nGreenCutLuxListNum */
        0,
        /* nGreenCutCctListNum */
        0,
        /* nGreenCutLuxList */
        {0,0,0,0,0,0,0,0},
        /* nGreenCutWeight */
        {102,102,102,102,102,102,102,102},
        /* nGreenCutBreakAngle */
        {52429,52429,52429,52429,52429,52429,52429,52429},
        /* nGreenCutOffsetRg */
        {31457,31457,31457,31457,31457,31457,31457,31457},
        /* nGreenCutOffsetBg */
        {-20971,-20971,-20971,-20971,-20971,-20971,-20971,-20971},
        /* nGreenCutCctList */
        {0,0,0,0,0,0,0,0},
        /* nGreenCutCctDiscount */
        {1024,1024,1024,1024,1024,1024,1024,1024},
        /* nMultiCamSyncMode */
        0,
        /* tCctLimit */
        {
            /* nEnable */
            0,
            /* eType */
            /* 0:AX_AWB_CCT_LIMIT_TYPE_CCT, 1:AX_AWB_CCT_LIMIT_TYPE_MANUAL, 2:AX_AWB_CCT_LIMIT_TYPE_MAX */
            AX_AWB_CCT_LIMIT_TYPE_CCT,
            /* nLowCct */
            2800,
            /* nHighCct */
            10000,
            /* nLowRgLimit */
            0,
            /* nLowBgLimit */
            0,
            /* nHighRgLimit */
            0,
            /* nHighBgLimit */
            0,
        },
    },
};

static AX_ISP_IQ_CAF_PARAM_T caf_param_sdr = {
    /* nScanType */
    0,
    /* nGlobalScanStep */
    0,
    /* nSearchDownFrameTh */
    0,
    /* nSearchDownSerialSlopeTh */
    0,
    /* nSearchDownSingleSlopeTh */
    0,
    /* nSearchUpFrameTh */
    0,
    /* nSearchUpSerialSlopeTh */
    0,
    /* nSearchUpSingleSlopeTh */
    0,
    /* nSearchBigStepUpSlopeTh */
    0,
    /* nSearchBigStep */
    0,
    /* nSearchSmallStep */
    0,
    /* nSearchProbeStep */
    0,
    /* nSearchProbeFrameTh */
    0,
    /* bContinuousAfEn */
    0,
    /* nSceneChangeLumaTh */
    0,
    /* nSceneSettledLumaTh */
    0,
    /* nSceneSlowChangeLumaTh */
    0,
    /* nSceneFvChangeLumaTh */
    0,
    /* nSceneFvChangeRatioTh */
    0,
    /* nSpotlightZoomRatioTh */
    0,
    /* tWeight */
    {
        /* nMuX */
        0,
        /* nMuY */
        0,
        /* nSigmaX */
        0,
        /* nSigmaY */
        0,
        /* nCoeffV */
        0,
        /* nCoeffH1 */
        0,
        /* nCoeffH2 */
        0,
    },
};

static AX_ISP_IQ_DPC_PARAM_T dpc_param_sdr = {
    /* nDpcEnable */
    1,
    /* nStaticDpcEnable */
    0,
    /* nDynamicDpcEnable */
    1,
    /* nPreDetLevelSlope */
    64,
    /* nPreDetLevelOffset */
    0,
    /* nColorLimitEnable */
    0,
    /* nAutoMode */
    1,
    /* nRefMode */
    1,
    /* tDpcParam */
    {
        /* tHcgTable */
        {
            /* nSdpcLength */
            0,
            /* nSdpcBuffer[8192] */
            {0, /*0 - 0*/},
            /* nShotNoiseCoeffsA[4] */
            {146899483, 146899483, 146899483, 146899483, /*0 - 3*/},
            /* nShotNoiseCoeffsB[4] */
            {3351969, 3351969, 3351969, 3351969, /*0 - 3*/},
            /* nReadNoiseCoeffsA[4] */
            {5346073, 5346073, 5346073, 5346073, /*0 - 3*/},
            /* nReadNoiseCoeffsB[4] */
            {51579982, 51579982, 51579982, 51579982, /*0 - 3*/},
            /* nReadNoiseCoeffsC[4] */
            {-63181926, -63181926, -63181926, -63181926, /*0 - 3*/},
        },
        /* tLcgTable */
        {
            /* nSdpcLength */
            0,
            /* nSdpcBuffer[8192] */
            {0, /*0 - 0*/},
            /* nShotNoiseCoeffsA[4] */
            {35416523, 35417714, 35423902, 35421682, /*0 - 3*/},
            /* nShotNoiseCoeffsB[4] */
            {-386665, -459927, -439376, -288491, /*0 - 3*/},
            /* nReadNoiseCoeffsA[4] */
            {2034207, 2001760, 1973960, 2031931, /*0 - 3*/},
            /* nReadNoiseCoeffsB[4] */
            {18168345, 18213528, 18883095, 17734840, /*0 - 3*/},
            /* nReadNoiseCoeffsC[4] */
            {49967466, 53292461, 51951927, 54656236, /*0 - 3*/},
        },
    },
    /* tManualParam */
    {
        /* nNoiseRatio[2] */
        {1024, 1024, /*0 - 1*/},
        /* nDpType */
        0,
        /* nNonChwiseEn */
        0,
        /* nChwiseStr */
        0,
        /* nDetCoarseStr */
        200,
        /* nDetFineStr */
        48,
        /* nDynamicDpcStr */
        128,
        /* nEdgeStr */
        102,
        /* nHotColdTypeStr */
        32,
        /* nSupWinkStr */
        16,
        /* nDynamicDpClrLimOffset */
        {
            /* nUpperLimit */
            256,
            /* nLowerLimit */
            256,
        },
        /* nStaticDpClrLimOffset */
        {
            /* nUpperLimit */
            256,
            /* nLowerLimit */
            256,
        },
        /* nNormalPixDpClrLimOffset */
        {
            /* nUpperLimit */
            256,
            /* nLowerLimit */
            256,
        },
        /* nDynamicDpClrLimStr */
        128,
        /* nStaticDpClrLimStr */
        128,
        /* nNormalPixDpClrLimStr */
        128,
    },
    /* tAutoParam */
    {
        /* nParamGrpNum */
        4,
        /* nRefVal[16] */
        {1024, 65536, 262144, 524288, /*0 - 3*/},
        /* nNoiseRatio[2][16] */
        {
            {1024, 1024, 1024, 1024, /*0 - 3*/},
            {1024, 1024, 1024, 1024, /*0 - 3*/},
        },
        /* nDpType[16] */
        {0, 0, 0, 0, /*0 - 3*/},
        /* nNonChwiseEn[16] */
        {0, 0, 0, 0, /*0 - 3*/},
        /* nChwiseStr[16] */
        {20, 20, 20, 0, /*0 - 3*/},
        /* nDetCoarseStr[16] */
        {128, 128, 200, 200, /*0 - 3*/},
        /* nDetFineStr[16] */
        {48, 48, 48, 48, /*0 - 3*/},
        /* nDynamicDpcStr[16] */
        {0, 20, 164, 128, /*0 - 3*/},
        /* nEdgeStr[16] */
        {102, 102, 102, 102, /*0 - 3*/},
        /* nHotColdTypeStr[16] */
        {32, 32, 32, 32, /*0 - 3*/},
        /* nSupWinkStr[16] */
        {16, 16, 16, 16, /*0 - 3*/},
        /* nDynamicDpClrLimOffset[16] */
        {
            /* 0 */
            {
                /* nUpperLimit */
                256,
                /* nLowerLimit */
                256,
            },
            /* 1 */
            {
                /* nUpperLimit */
                256,
                /* nLowerLimit */
                256,
            },
            /* 2 */
            {
                /* nUpperLimit */
                256,
                /* nLowerLimit */
                256,
            },
            /* 3 */
            {
                /* nUpperLimit */
                256,
                /* nLowerLimit */
                256,
            },
        },
        /* nStaticDpClrLimOffset[16] */
        {
            /* 0 */
            {
                /* nUpperLimit */
                256,
                /* nLowerLimit */
                256,
            },
            /* 1 */
            {
                /* nUpperLimit */
                256,
                /* nLowerLimit */
                256,
            },
            /* 2 */
            {
                /* nUpperLimit */
                256,
                /* nLowerLimit */
                256,
            },
            /* 3 */
            {
                /* nUpperLimit */
                256,
                /* nLowerLimit */
                256,
            },
        },
        /* nNormalPixDpClrLimOffset[16] */
        {
            /* 0 */
            {
                /* nUpperLimit */
                256,
                /* nLowerLimit */
                256,
            },
            /* 1 */
            {
                /* nUpperLimit */
                256,
                /* nLowerLimit */
                256,
            },
            /* 2 */
            {
                /* nUpperLimit */
                256,
                /* nLowerLimit */
                256,
            },
            /* 3 */
            {
                /* nUpperLimit */
                256,
                /* nLowerLimit */
                256,
            },
        },
        /* nDynamicDpClrLimStr[16] */
        {128, 128, 128, 128, /*0 - 3*/},
        /* nStaticDpClrLimStr[16] */
        {128, 128, 128, 128, /*0 - 3*/},
        /* nNormalPixDpClrLimStr[16] */
        {128, 128, 128, 128, /*0 - 3*/},
    },
};

static AX_ISP_IQ_BLC_PARAM_T blc_param_sdr = {
    /* nBlcEnable */
    1,
    /* nAutoMode */
    1,
    /* tManualParam[4] */
    {
        /* 0 */
        {
            /* nSblRValue */
            49766,
            /* nSblGrValue */
            49971,
            /* nSblGbValue */
            49971,
            /* nSblBValue */
            49766,
        },
        /* 1 */
        {
            /* nSblRValue */
            0,
            /* nSblGrValue */
            0,
            /* nSblGbValue */
            0,
            /* nSblBValue */
            0,
        },
        /* 2 */
        {
            /* nSblRValue */
            0,
            /* nSblGrValue */
            0,
            /* nSblGbValue */
            0,
            /* nSblBValue */
            0,
        },
        /* 3 */
        {
            /* nSblRValue */
            0,
            /* nSblGrValue */
            0,
            /* nSblGbValue */
            0,
            /* nSblBValue */
            0,
        },
    },
    /* tAutoParam */
    {
        /* tHcgAutoTable */
        {
            /* nGainGrpNum */
            12,
            /* nExposeTimeGrpNum */
            1,
            /* nGain[16] */
            {1024, 8192, 16384, 24576, 32768, 49152, 65536, 98304, 196608, 294912, 344064, 393216, /*0 - 11*/},
            /* nExposeTime[10] */
            {1000, /*0 - 0*/},
            /* nAutoSblRValue[16][10] */
            {
                {50790, /*0 - 0*/},
                {50790, /*0 - 0*/},
                {50790, /*0 - 0*/},
                {50790, /*0 - 0*/},
                {49152, /*0 - 0*/},
                {49152, /*0 - 0*/},
                {49152, /*0 - 0*/},
                {49152, /*0 - 0*/},
                {49152, /*0 - 0*/},
                {49152, /*0 - 0*/},
                {49152, /*0 - 0*/},
                {49152, /*0 - 0*/},
            },
            /* nAutoSblGrValue[16][10] */
            {
                {50790, /*0 - 0*/},
                {50790, /*0 - 0*/},
                {50790, /*0 - 0*/},
                {50790, /*0 - 0*/},
                {49152, /*0 - 0*/},
                {49152, /*0 - 0*/},
                {49152, /*0 - 0*/},
                {49152, /*0 - 0*/},
                {49152, /*0 - 0*/},
                {49152, /*0 - 0*/},
                {49152, /*0 - 0*/},
                {49152, /*0 - 0*/},
            },
            /* nAutoSblGbValue[16][10] */
            {
                {50790, /*0 - 0*/},
                {50790, /*0 - 0*/},
                {50790, /*0 - 0*/},
                {50790, /*0 - 0*/},
                {49152, /*0 - 0*/},
                {49152, /*0 - 0*/},
                {49152, /*0 - 0*/},
                {49152, /*0 - 0*/},
                {49152, /*0 - 0*/},
                {49152, /*0 - 0*/},
                {49152, /*0 - 0*/},
                {49152, /*0 - 0*/},
            },
            /* nAutoSblBValue[16][10] */
            {
                {50790, /*0 - 0*/},
                {50790, /*0 - 0*/},
                {50790, /*0 - 0*/},
                {50790, /*0 - 0*/},
                {49152, /*0 - 0*/},
                {49152, /*0 - 0*/},
                {49152, /*0 - 0*/},
                {49152, /*0 - 0*/},
                {49152, /*0 - 0*/},
                {49152, /*0 - 0*/},
                {49152, /*0 - 0*/},
                {49152, /*0 - 0*/},
            },
        },
        /* tLcgAutoTable */
        {
            /* nGainGrpNum */
            5,
            /* nExposeTimeGrpNum */
            1,
            /* nGain[16] */
            {1024, 2048, 3072, 4096, 5120, /*0 - 4*/},
            /* nExposeTime[10] */
            {1000, /*0 - 0*/},
            /* nAutoSblRValue[16][10] */
            {
                {50790, /*0 - 0*/},
                {50790, /*0 - 0*/},
                {50790, /*0 - 0*/},
                {50790, /*0 - 0*/},
                {50790, /*0 - 0*/},
            },
            /* nAutoSblGrValue[16][10] */
            {
                {50790, /*0 - 0*/},
                {50790, /*0 - 0*/},
                {50790, /*0 - 0*/},
                {50790, /*0 - 0*/},
                {50790, /*0 - 0*/},
            },
            /* nAutoSblGbValue[16][10] */
            {
                {50790, /*0 - 0*/},
                {50790, /*0 - 0*/},
                {50790, /*0 - 0*/},
                {50790, /*0 - 0*/},
                {50790, /*0 - 0*/},
            },
            /* nAutoSblBValue[16][10] */
            {
                {50790, /*0 - 0*/},
                {50790, /*0 - 0*/},
                {50790, /*0 - 0*/},
                {50790, /*0 - 0*/},
                {50790, /*0 - 0*/},
            },
        },
    },
};

static AX_ISP_IQ_FPN_PARAM_T fpn_param_sdr = {
    /* nFpnEnable */
    /* 0:AX_FALSE, 1:AX_TRUE */
    AX_FALSE,
    /* nAutoMode */
    0,
    /* eFpnMode */
    /* 0:AX_ISP_IQ_FPN_MODE0, 1:AX_ISP_IQ_FPN_MODE1, 2:AX_ISP_IQ_FPN_MODE_MAX */
    AX_ISP_IQ_FPN_MODE1,
    /* tManualParam */
    {
        /* tFpnCaliFrame */
        {
            /* szFpnFrmName[128] */
            "dark_HCG_exp_20000_again_1.125_temp_20_framesize_8294400_cali.raw",
            /* nLcgHcgMode */
            0,
            /* nAgain */
            288,
            /* fIntTime */
            20000,
            /* nTemperature */
            20,
            /* nFrameSize */
            8294400,
            /* nFpnCorrGain */
            32,
            /* nFpnOffset */
            48,
        },
    },
    /* tAutoParam */
    {
        /* tHcgFpnCaliFrame */
        {
            /* nCaliFrameNum */
            2,
            /* tFpnCaliFrame[8] */
            {
                /* 0 */
                {
                    /* szFpnFrmName[128] */
                    "dark_HCG_exp_20000_again_1.125_temp_20_framesize_8294400_cali.raw",
                    /* nLcgHcgMode */
                    0,
                    /* nAgain */
                    288,
                    /* fIntTime */
                    20000,
                    /* nTemperature */
                    20,
                    /* nFrameSize */
                    8294400,
                    /* nFpnCorrGain */
                    32,
                    /* nFpnOffset */
                    48,
                },
                /* 1 */
                {
                    /* szFpnFrmName[128] */
                    "dark_HCG_exp_20000_again_15.5_temp_20_framesize_8294400_cali.raw",
                    /* nLcgHcgMode */
                    0,
                    /* nAgain */
                    3968,
                    /* fIntTime */
                    20000,
                    /* nTemperature */
                    20,
                    /* nFrameSize */
                    8294400,
                    /* nFpnCorrGain */
                    64,
                    /* nFpnOffset */
                    48,
                },
            },
        },
        /* tLcgFpnCaliFrame */
        {
            /* nCaliFrameNum */
            2,
            /* tFpnCaliFrame[8] */
            {
                /* 0 */
                {
                    /* szFpnFrmName[128] */
                    "dark_LCG_exp_20000_again_1.125_temp_20_framesize_8294400_cali.raw",
                    /* nLcgHcgMode */
                    1,
                    /* nAgain */
                    288,
                    /* fIntTime */
                    20000,
                    /* nTemperature */
                    20,
                    /* nFrameSize */
                    8294400,
                    /* nFpnCorrGain */
                    64,
                    /* nFpnOffset */
                    48,
                },
                /* 1 */
                {
                    /* szFpnFrmName[128] */
                    "dark_LCG_exp_20000_again_15.5_temp_20_framesize_8294400_cali.raw",
                    /* nLcgHcgMode */
                    1,
                    /* nAgain */
                    3968,
                    /* fIntTime */
                    20000,
                    /* nTemperature */
                    20,
                    /* nFrameSize */
                    8294400,
                    /* nFpnCorrGain */
                    64,
                    /* nFpnOffset */
                    48,
                },
            },
        },
    },
};

static AX_ISP_IQ_HDR_PARAM_T hdr_param_sdr = {
    /* nEnable */
    0,
    /* nAutoMode */
    0,
    /* nRefMode */
    0,
    /* nDebugMode */
    0,
    /* tMotDetParam */
    {
        /* nCoarseMotMaskRatio[3][2] */
        {
            {0, 0, /*0 - 1*/},
            {0, 0, /*0 - 1*/},
            {0, 0, /*0 - 1*/},
        },
        /* nMotMaskStrength[3] */
        {0, 0, 0, /*0 - 2*/},
        /* nMotIirRatio[3][2] */
        {
            {0, 0, /*0 - 1*/},
            {0, 0, /*0 - 1*/},
            {0, 0, /*0 - 1*/},
        },
    },
    /* tExpMaskParam */
    {
        /* nCoarseExpMaskRatio[3][2] */
        {
            {0, 0, /*0 - 1*/},
            {0, 0, /*0 - 1*/},
            {0, 0, /*0 - 1*/},
        },
        /* nExpIirRatio[3][2] */
        {
            {0, 0, /*0 - 1*/},
            {0, 0, /*0 - 1*/},
            {0, 0, /*0 - 1*/},
        },
        /* nExpYRatio[3] */
        {0, 0, 0, /*0 - 2*/},
        /* nExpWeightLut[3][257] */
        {
            {
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  /* 0 - 31*/
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  /* 32 - 63*/
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  /* 64 - 95*/
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  /* 96 - 127*/
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  /* 128 - 159*/
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  /* 160 - 191*/
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  /* 192 - 223*/
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  /* 224 - 255*/
                0, /*256 - 256*/
            },
            {
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  /* 0 - 31*/
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  /* 32 - 63*/
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  /* 64 - 95*/
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  /* 96 - 127*/
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  /* 128 - 159*/
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  /* 160 - 191*/
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  /* 192 - 223*/
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  /* 224 - 255*/
                0, /*256 - 256*/
            },
            {
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  /* 0 - 31*/
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  /* 32 - 63*/
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  /* 64 - 95*/
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  /* 96 - 127*/
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  /* 128 - 159*/
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  /* 160 - 191*/
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  /* 192 - 223*/
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  /* 224 - 255*/
                0, /*256 - 256*/
            },
        },
        /* nExpWeightGain[3] */
        {0, 0, 0, /*0 - 2*/},
    },
    /* tDgstParam */
    {
        /* nDeghostEnable */
        0,
        /* nDgstStrenThre[2] */
        {0, 0, /*0 - 1*/},
        /* nDgstStrenLimit[2] */
        {0, 0, /*0 - 1*/},
        /* nDgstBaseFid */
        0,
        /* nDgstManualGainEn */
        0,
        /* nDgstManualGain[3] */
        {0, 0, 0, /*0 - 2*/},
    },
    /* tFusionParam */
    {
        /* nFusionProtectThreshold[2] */
        {0, 0, /*0 - 1*/},
    },
    /* tHdrManualParam */
    {
        /* nNoiseLutScale */
        4096,
        /* nCoarseMotMaskNoiseLvl[3] */
        {0, 0, 0, /*0 - 2*/},
        /* nCoarseMotMaskSen[3] */
        {0, 0, 0, /*0 - 2*/},
        /* nCoarseExpMaskSen[3] */
        {0, 0, 0, /*0 - 2*/},
    },
    /* tHdrAutoParam */
    {
        /* nParamGrpNum */
        0,
        /* nRefVal[16] */
        {0, /*0 - 0*/},
        /* nNoiseLutScale[16] */
        {4096, },
        /* nCoarseMotMaskNoiseLvl[16][3] */
        {
            {0, 0, 0, /*0 - 2*/},
        },
        /* nCoarseMotMaskSen[16][3] */
        {
            {0, 0, 0, /*0 - 2*/},
        },
        /* nCoarseExpMaskSen[16][3] */
        {
            {0, 0, 0, /*0 - 2*/},
        },
    },
};

static AX_ISP_IQ_AINR_PARAM_T ainr_param_sdr = {
    /* nEnable */
    1,
    /* nAutoMode */
    1,
    /* bUpdateTable */
    1,
    /* nRefMode */
    1,
    /* nHdrMode */
    1,
    /* tManualParam */
    {
        /* szModelPath[256] */
        "/opt/etc/models/aiisp/IMX678_SDR_3840x2160_12b_HCG_ISP1_A22-32X_0000_00000452952_230821_AX650.axmodel",
        /* szModelName[256] */
        "IMX678_SDR_3840x2160_12b_HCG_ISP1_A22-32X_0000_00000452952_230821_AX650.axmodel",
        /* szTemporalBaseNrName[256] */
        "",
        /* szSpatialBaseNrName[256] */
        "",
        /* nBiasIn[4] */
        {1200, 1200, 1200, 1200, /*0 - 3*/},
        /* nBiasOut[4] */
        {0, 0, 0, 0, /*0 - 3*/},
        /* tNonSensParam */
        {
            /* nTNRStrLut[8][2] */
            {
                {0, 0, /*0 - 1*/},
                {15, 15, /*0 - 1*/},
                {20, 20, /*0 - 1*/},
                {25, 25, /*0 - 1*/},
                {127, 127, /*0 - 1*/},
                {229, 229, /*0 - 1*/},
                {242, 242, /*0 - 1*/},
                {255, 255, /*0 - 1*/},
            },
            /* nShpStrLut[8][2] */
            {
                {0, 255, /*0 - 1*/},
                {15, 240, /*0 - 1*/},
                {20, 235, /*0 - 1*/},
                {25, 230, /*0 - 1*/},
                {127, 128, /*0 - 1*/},
                {229, 25, /*0 - 1*/},
                {242, 13, /*0 - 1*/},
                {255, 0, /*0 - 1*/},
            },
            /* nTemporalFilterStrLut[8][2] */
            {
                {0, 0, /*0 - 1*/},
                {15, 15, /*0 - 1*/},
                {20, 20, /*0 - 1*/},
                {25, 25, /*0 - 1*/},
                {127, 127, /*0 - 1*/},
                {229, 229, /*0 - 1*/},
                {242, 242, /*0 - 1*/},
                {255, 255, /*0 - 1*/},
            },
            /* nTemporalFilterStrLut2[8][2] */
            {
                {0, 255, /*0 - 1*/},
                {15, 255, /*0 - 1*/},
                {20, 255, /*0 - 1*/},
                {25, 255, /*0 - 1*/},
                {127, 255, /*0 - 1*/},
                {229, 255, /*0 - 1*/},
                {242, 255, /*0 - 1*/},
                {255, 255, /*0 - 1*/},
            },
        },
        /* tFreqAffineParam */
        {
            /* nLowFreqNRAffine */
            0,
            /* nMidFreqNRAffine */
            0,
            /* nHighFreqNRAffine */
            0,
        },
        /* tFreqLutParam */
        {
            /* nLowFreqNRStrLut[8][2] */
            {
                {0, 255, /*0 - 1*/},
                {15, 255, /*0 - 1*/},
                {20, 255, /*0 - 1*/},
                {25, 255, /*0 - 1*/},
                {127, 255, /*0 - 1*/},
                {229, 255, /*0 - 1*/},
                {242, 255, /*0 - 1*/},
                {255, 255, /*0 - 1*/},
            },
            /* nMidFreqNRStrLut[8][2] */
            {
                {0, 63, /*0 - 1*/},
                {15, 63, /*0 - 1*/},
                {20, 63, /*0 - 1*/},
                {25, 102, /*0 - 1*/},
                {127, 255, /*0 - 1*/},
                {229, 255, /*0 - 1*/},
                {242, 255, /*0 - 1*/},
                {255, 255, /*0 - 1*/},
            },
            /* nHighFreqNRStrLut[8][2] */
            {
                {0, 0, /*0 - 1*/},
                {15, 0, /*0 - 1*/},
                {20, 0, /*0 - 1*/},
                {25, 4, /*0 - 1*/},
                {127, 96, /*0 - 1*/},
                {229, 189, /*0 - 1*/},
                {242, 201, /*0 - 1*/},
                {255, 213, /*0 - 1*/},
            },
        },
    },
    /* tAutoParam */
    {
        /* nAutoModelNum */
        4,
        /* tAutoModelTable[0] */
        {
            /* 0 */
            {
                /* tMeta */
                {
                    /* szModelPath[256] */
                    "/opt/etc/models/aiisp/IMX678_SDR_3840x2160_12b_HCG_ISP1_A22-32X_0000_00000452952_230821_AX650.axmodel",
                    /* szModelName[256] */
                    "IMX678_SDR_3840x2160_12b_HCG_ISP1_A22-32X_0000_00000452952_230821_AX650.axmodel",
                    /* szTemporalBaseNrName[256] */
                    "",
                    /* szSpatialBaseNrName[256] */
                    "",
                    /* nBiasIn[4] */
                    {800, 2400, 0, 0, /*0 - 3*/},
                    /* nBiasOut[4] */
                    {0, 0, 0, 0, /*0 - 3*/},
                    /* nHcgMode */
                    1,
                    /* nIsoThresholdMin */
                    2200,
                    /* nIsoThresholdMax */
                    3200,
                    /* nHdrRatioMin */
                    0,
                    /* nHdrRatioMax */
                    0,
                },
                /* tParam */
                {
                    /* tNonSensParam */
                    {
                        /* nParamGrpNum */
                        4,
                        /* nRefValStart[4] */
                        {1024, 1024, 1024, 1024, /*0 - 3*/},
                        /* nRefValEnd[4] */
                        {4294967295, 4294967295, 4294967295, 4294967295, /*0 - 3*/},
                        /* nTNRStrLut[4][8][2] */
                        {
                            /* nTNRStrLut 0 */
                            {
                                {0, 0, /*0 - 1*/},
                                {15, 15, /*0 - 1*/},
                                {20, 20, /*0 - 1*/},
                                {25, 25, /*0 - 1*/},
                                {127, 127, /*0 - 1*/},
                                {229, 229, /*0 - 1*/},
                                {242, 242, /*0 - 1*/},
                                {255, 255, /*0 - 1*/},
                            },
                            /* nTNRStrLut 1 */
                            {
                                {0, 0, /*0 - 1*/},
                                {15, 15, /*0 - 1*/},
                                {20, 20, /*0 - 1*/},
                                {25, 25, /*0 - 1*/},
                                {127, 127, /*0 - 1*/},
                                {229, 229, /*0 - 1*/},
                                {242, 242, /*0 - 1*/},
                                {255, 255, /*0 - 1*/},
                            },
                            /* nTNRStrLut 2 */
                            {
                                {0, 0, /*0 - 1*/},
                                {15, 15, /*0 - 1*/},
                                {20, 20, /*0 - 1*/},
                                {25, 25, /*0 - 1*/},
                                {127, 127, /*0 - 1*/},
                                {229, 229, /*0 - 1*/},
                                {242, 242, /*0 - 1*/},
                                {255, 255, /*0 - 1*/},
                            },
                            /* nTNRStrLut 3 */
                            {
                                {0, 0, /*0 - 1*/},
                                {15, 15, /*0 - 1*/},
                                {20, 20, /*0 - 1*/},
                                {25, 25, /*0 - 1*/},
                                {127, 127, /*0 - 1*/},
                                {229, 229, /*0 - 1*/},
                                {242, 242, /*0 - 1*/},
                                {255, 255, /*0 - 1*/},
                            },
                        },
                        /* nShpStrLut[4][8][2] */
                        {
                            /* nShpStrLut 0 */
                            {
                                {0, 255, /*0 - 1*/},
                                {15, 240, /*0 - 1*/},
                                {20, 235, /*0 - 1*/},
                                {25, 230, /*0 - 1*/},
                                {127, 128, /*0 - 1*/},
                                {229, 25, /*0 - 1*/},
                                {242, 13, /*0 - 1*/},
                                {255, 0, /*0 - 1*/},
                            },
                            /* nShpStrLut 1 */
                            {
                                {0, 255, /*0 - 1*/},
                                {15, 240, /*0 - 1*/},
                                {20, 235, /*0 - 1*/},
                                {25, 230, /*0 - 1*/},
                                {127, 128, /*0 - 1*/},
                                {229, 25, /*0 - 1*/},
                                {242, 13, /*0 - 1*/},
                                {255, 0, /*0 - 1*/},
                            },
                            /* nShpStrLut 2 */
                            {
                                {0, 255, /*0 - 1*/},
                                {15, 240, /*0 - 1*/},
                                {20, 235, /*0 - 1*/},
                                {25, 230, /*0 - 1*/},
                                {127, 128, /*0 - 1*/},
                                {229, 25, /*0 - 1*/},
                                {242, 13, /*0 - 1*/},
                                {255, 0, /*0 - 1*/},
                            },
                            /* nShpStrLut 3 */
                            {
                                {0, 255, /*0 - 1*/},
                                {15, 240, /*0 - 1*/},
                                {20, 235, /*0 - 1*/},
                                {25, 230, /*0 - 1*/},
                                {127, 128, /*0 - 1*/},
                                {229, 25, /*0 - 1*/},
                                {242, 13, /*0 - 1*/},
                                {255, 0, /*0 - 1*/},
                            },
                        },
                        /* nTemporalFilterStrLut[4][8][2] */
                        {
                            /* nTemporalFilterStrLut 0 */
                            {
                                {0, 0, /*0 - 1*/},
                                {15, 15, /*0 - 1*/},
                                {20, 20, /*0 - 1*/},
                                {25, 25, /*0 - 1*/},
                                {127, 127, /*0 - 1*/},
                                {229, 229, /*0 - 1*/},
                                {242, 242, /*0 - 1*/},
                                {255, 255, /*0 - 1*/},
                            },
                            /* nTemporalFilterStrLut 1 */
                            {
                                {0, 0, /*0 - 1*/},
                                {15, 15, /*0 - 1*/},
                                {20, 20, /*0 - 1*/},
                                {25, 25, /*0 - 1*/},
                                {127, 127, /*0 - 1*/},
                                {229, 229, /*0 - 1*/},
                                {242, 242, /*0 - 1*/},
                                {255, 255, /*0 - 1*/},
                            },
                            /* nTemporalFilterStrLut 2 */
                            {
                                {0, 0, /*0 - 1*/},
                                {15, 15, /*0 - 1*/},
                                {20, 20, /*0 - 1*/},
                                {25, 25, /*0 - 1*/},
                                {127, 127, /*0 - 1*/},
                                {229, 229, /*0 - 1*/},
                                {242, 242, /*0 - 1*/},
                                {255, 255, /*0 - 1*/},
                            },
                            /* nTemporalFilterStrLut 3 */
                            {
                                {0, 0, /*0 - 1*/},
                                {15, 15, /*0 - 1*/},
                                {20, 20, /*0 - 1*/},
                                {25, 25, /*0 - 1*/},
                                {127, 127, /*0 - 1*/},
                                {229, 229, /*0 - 1*/},
                                {242, 242, /*0 - 1*/},
                                {255, 255, /*0 - 1*/},
                            },
                        },
                        /* nTemporalFilterStrLut2[4][8][2] */
                        {
                            /* nTemporalFilterStrLut2 0 */
                            {
                                {0, 255, /*0 - 1*/},
                                {15, 255, /*0 - 1*/},
                                {20, 255, /*0 - 1*/},
                                {25, 255, /*0 - 1*/},
                                {127, 255, /*0 - 1*/},
                                {229, 255, /*0 - 1*/},
                                {242, 255, /*0 - 1*/},
                                {255, 255, /*0 - 1*/},
                            },
                            /* nTemporalFilterStrLut2 1 */
                            {
                                {0, 255, /*0 - 1*/},
                                {15, 255, /*0 - 1*/},
                                {20, 255, /*0 - 1*/},
                                {25, 255, /*0 - 1*/},
                                {127, 255, /*0 - 1*/},
                                {229, 255, /*0 - 1*/},
                                {242, 255, /*0 - 1*/},
                                {255, 255, /*0 - 1*/},
                            },
                            /* nTemporalFilterStrLut2 2 */
                            {
                                {0, 255, /*0 - 1*/},
                                {15, 255, /*0 - 1*/},
                                {20, 255, /*0 - 1*/},
                                {25, 255, /*0 - 1*/},
                                {127, 255, /*0 - 1*/},
                                {229, 255, /*0 - 1*/},
                                {242, 255, /*0 - 1*/},
                                {255, 255, /*0 - 1*/},
                            },
                            /* nTemporalFilterStrLut2 3 */
                            {
                                {0, 255, /*0 - 1*/},
                                {15, 255, /*0 - 1*/},
                                {20, 255, /*0 - 1*/},
                                {25, 255, /*0 - 1*/},
                                {127, 255, /*0 - 1*/},
                                {229, 255, /*0 - 1*/},
                                {242, 255, /*0 - 1*/},
                                {255, 255, /*0 - 1*/},
                            },
                        },
                    },
                    /* tFreqAffineParam */
                    {
                        /* nParamGrpNum */
                        4,
                        /* nRefVal[4] */
                        {1024, 1792, 4096, 4294967295, /*0 - 3*/},
                        /* nLowFreqNRAffine[4] */
                        {
                            0,
                            0,
                            0,
                            0,
                        },
                        /* nMidFreqNRAffine[4] */
                        {
                            0,
                            0,
                            0,
                            0,
                        },
                        /* nHighFreqNRAffine[4][4] */
                        {
                            0,
                            0,
                            0,
                            0,
                        },
                    },
                    /* tFreqLutParam */
                    {
                        /* nParamGrpNum */
                        4,
                        /* nRefValStart[4] */
                        {1024, 1024, 1024, 1024, /*0 - 3*/},
                        /* nRefValEnd[4] */
                        {4294967295, 4294967295, 4294967295, 4294967295, /*0 - 3*/},
                        /* nLowFreqNRStrLut[4][8][2] */
                        {
                            /* nLowFreqNRStrLut 0 */
                            {
                                {1, 128, /*0 - 1*/},
                                {16, 128, /*0 - 1*/},
                                {32, 128, /*0 - 1*/},
                                {51, 128, /*0 - 1*/},
                                {64, 128, /*0 - 1*/},
                                {85, 128, /*0 - 1*/},
                                {128, 128, /*0 - 1*/},
                                {255, 128, /*0 - 1*/},
                            },
                            /* nLowFreqNRStrLut 1 */
                            {
                                {1, 128, /*0 - 1*/},
                                {16, 128, /*0 - 1*/},
                                {32, 128, /*0 - 1*/},
                                {51, 128, /*0 - 1*/},
                                {64, 128, /*0 - 1*/},
                                {85, 128, /*0 - 1*/},
                                {128, 128, /*0 - 1*/},
                                {255, 128, /*0 - 1*/},
                            },
                            /* nLowFreqNRStrLut 2 */
                            {
                                {1, 128, /*0 - 1*/},
                                {16, 128, /*0 - 1*/},
                                {32, 128, /*0 - 1*/},
                                {51, 128, /*0 - 1*/},
                                {64, 128, /*0 - 1*/},
                                {85, 128, /*0 - 1*/},
                                {128, 128, /*0 - 1*/},
                                {255, 128, /*0 - 1*/},
                            },
                            /* nLowFreqNRStrLut 3 */
                            {
                                {1, 128, /*0 - 1*/},
                                {16, 128, /*0 - 1*/},
                                {32, 128, /*0 - 1*/},
                                {51, 128, /*0 - 1*/},
                                {64, 128, /*0 - 1*/},
                                {85, 128, /*0 - 1*/},
                                {128, 128, /*0 - 1*/},
                                {255, 128, /*0 - 1*/},
                            },
                        },
                        /* nMidFreqNRStrLut[4][8][2] */
                        {
                            /* nMidFreqNRStrLut 0 */
                            {
                                {0, 128, /*0 - 1*/},
                                {20, 160, /*0 - 1*/},
                                {45, 160, /*0 - 1*/},
                                {80, 180, /*0 - 1*/},
                                {133, 180, /*0 - 1*/},
                                {167, 180, /*0 - 1*/},
                                {209, 180, /*0 - 1*/},
                                {254, 180, /*0 - 1*/},
                            },
                            /* nMidFreqNRStrLut 1 */
                            {
                                {0, 128, /*0 - 1*/},
                                {20, 160, /*0 - 1*/},
                                {45, 160, /*0 - 1*/},
                                {80, 180, /*0 - 1*/},
                                {133, 180, /*0 - 1*/},
                                {167, 180, /*0 - 1*/},
                                {209, 180, /*0 - 1*/},
                                {254, 180, /*0 - 1*/},
                            },
                            /* nMidFreqNRStrLut 2 */
                            {
                                {0, 128, /*0 - 1*/},
                                {20, 160, /*0 - 1*/},
                                {45, 160, /*0 - 1*/},
                                {80, 180, /*0 - 1*/},
                                {133, 180, /*0 - 1*/},
                                {167, 180, /*0 - 1*/},
                                {209, 180, /*0 - 1*/},
                                {254, 180, /*0 - 1*/},
                            },
                            /* nMidFreqNRStrLut 3 */
                            {
                                {0, 128, /*0 - 1*/},
                                {20, 160, /*0 - 1*/},
                                {45, 160, /*0 - 1*/},
                                {80, 180, /*0 - 1*/},
                                {133, 180, /*0 - 1*/},
                                {167, 180, /*0 - 1*/},
                                {209, 180, /*0 - 1*/},
                                {254, 180, /*0 - 1*/},
                            },
                        },
                        /* nHighFreqNRStrLut[4][8][2] */
                        {
                            /* nHighFreqNRStrLut 0 */
                            {
                                {0, 80, /*0 - 1*/},
                                {10, 80, /*0 - 1*/},
                                {25, 80, /*0 - 1*/},
                                {50, 100, /*0 - 1*/},
                                {142, 120, /*0 - 1*/},
                                {181, 120, /*0 - 1*/},
                                {224, 140, /*0 - 1*/},
                                {255, 140, /*0 - 1*/},
                            },
                            /* nHighFreqNRStrLut 1 */
                            {
                                {0, 80, /*0 - 1*/},
                                {10, 80, /*0 - 1*/},
                                {25, 80, /*0 - 1*/},
                                {50, 100, /*0 - 1*/},
                                {142, 120, /*0 - 1*/},
                                {181, 120, /*0 - 1*/},
                                {224, 140, /*0 - 1*/},
                                {255, 140, /*0 - 1*/},
                            },
                            /* nHighFreqNRStrLut 2 */
                            {
                                {0, 80, /*0 - 1*/},
                                {10, 80, /*0 - 1*/},
                                {25, 80, /*0 - 1*/},
                                {50, 100, /*0 - 1*/},
                                {142, 120, /*0 - 1*/},
                                {181, 120, /*0 - 1*/},
                                {224, 140, /*0 - 1*/},
                                {255, 140, /*0 - 1*/},
                            },
                            /* nHighFreqNRStrLut 3 */
                            {
                                {0, 80, /*0 - 1*/},
                                {10, 80, /*0 - 1*/},
                                {25, 80, /*0 - 1*/},
                                {50, 100, /*0 - 1*/},
                                {142, 120, /*0 - 1*/},
                                {181, 120, /*0 - 1*/},
                                {224, 140, /*0 - 1*/},
                                {255, 140, /*0 - 1*/},
                            },
                        },
                    },
                },
            },
            /* 1 */
            {
                /* tMeta */
                {
                    /* szModelPath[256] */
                    "/opt/etc/models/aiisp/IMX678_SDR_3840x2160_12b_HCG_ISP1_A32-48X_0000_00000452953_230821_AX650.axmodel",
                    /* szModelName[256] */
                    "IMX678_SDR_3840x2160_12b_HCG_ISP1_A32-48X_0000_00000452953_230821_AX650.axmodel",
                    /* szTemporalBaseNrName[256] */
                    "",
                    /* szSpatialBaseNrName[256] */
                    "",
                    /* nBiasIn[4] */
                    {800, 2400, 0, 0, /*0 - 3*/},
                    /* nBiasOut[4] */
                    {0, 0, 0, 0, /*0 - 3*/},
                    /* nHcgMode */
                    1,
                    /* nIsoThresholdMin */
                    3200,
                    /* nIsoThresholdMax */
                    4800,
                    /* nHdrRatioMin */
                    0,
                    /* nHdrRatioMax */
                    0,
                },
                /* tParam */
                {
                    /* tNonSensParam */
                    {
                        /* nParamGrpNum */
                        4,
                        /* nRefValStart[4] */
                        {1024, 1024, 1024, 1024, /*0 - 3*/},
                        /* nRefValEnd[4] */
                        {4294967295, 4294967295, 4294967295, 4294967295, /*0 - 3*/},
                        /* nTNRStrLut[4][8][2] */
                        {
                            /* nTNRStrLut 0 */
                            {
                                {0, 0, /*0 - 1*/},
                                {15, 15, /*0 - 1*/},
                                {20, 20, /*0 - 1*/},
                                {25, 25, /*0 - 1*/},
                                {127, 127, /*0 - 1*/},
                                {229, 229, /*0 - 1*/},
                                {242, 242, /*0 - 1*/},
                                {255, 255, /*0 - 1*/},
                            },
                            /* nTNRStrLut 1 */
                            {
                                {0, 0, /*0 - 1*/},
                                {15, 15, /*0 - 1*/},
                                {20, 20, /*0 - 1*/},
                                {25, 25, /*0 - 1*/},
                                {127, 127, /*0 - 1*/},
                                {229, 229, /*0 - 1*/},
                                {242, 242, /*0 - 1*/},
                                {255, 255, /*0 - 1*/},
                            },
                            /* nTNRStrLut 2 */
                            {
                                {0, 0, /*0 - 1*/},
                                {15, 15, /*0 - 1*/},
                                {20, 20, /*0 - 1*/},
                                {25, 25, /*0 - 1*/},
                                {127, 127, /*0 - 1*/},
                                {229, 229, /*0 - 1*/},
                                {242, 242, /*0 - 1*/},
                                {255, 255, /*0 - 1*/},
                            },
                            /* nTNRStrLut 3 */
                            {
                                {0, 0, /*0 - 1*/},
                                {15, 15, /*0 - 1*/},
                                {20, 20, /*0 - 1*/},
                                {25, 25, /*0 - 1*/},
                                {127, 127, /*0 - 1*/},
                                {229, 229, /*0 - 1*/},
                                {242, 242, /*0 - 1*/},
                                {255, 255, /*0 - 1*/},
                            },
                        },
                        /* nShpStrLut[4][8][2] */
                        {
                            /* nShpStrLut 0 */
                            {
                                {0, 255, /*0 - 1*/},
                                {15, 240, /*0 - 1*/},
                                {20, 235, /*0 - 1*/},
                                {25, 230, /*0 - 1*/},
                                {127, 128, /*0 - 1*/},
                                {229, 25, /*0 - 1*/},
                                {242, 13, /*0 - 1*/},
                                {255, 0, /*0 - 1*/},
                            },
                            /* nShpStrLut 1 */
                            {
                                {0, 255, /*0 - 1*/},
                                {15, 240, /*0 - 1*/},
                                {20, 235, /*0 - 1*/},
                                {25, 230, /*0 - 1*/},
                                {127, 128, /*0 - 1*/},
                                {229, 25, /*0 - 1*/},
                                {242, 13, /*0 - 1*/},
                                {255, 0, /*0 - 1*/},
                            },
                            /* nShpStrLut 2 */
                            {
                                {0, 255, /*0 - 1*/},
                                {15, 240, /*0 - 1*/},
                                {20, 235, /*0 - 1*/},
                                {25, 230, /*0 - 1*/},
                                {127, 128, /*0 - 1*/},
                                {229, 25, /*0 - 1*/},
                                {242, 13, /*0 - 1*/},
                                {255, 0, /*0 - 1*/},
                            },
                            /* nShpStrLut 3 */
                            {
                                {0, 255, /*0 - 1*/},
                                {15, 240, /*0 - 1*/},
                                {20, 235, /*0 - 1*/},
                                {25, 230, /*0 - 1*/},
                                {127, 128, /*0 - 1*/},
                                {229, 25, /*0 - 1*/},
                                {242, 13, /*0 - 1*/},
                                {255, 0, /*0 - 1*/},
                            },
                        },
                        /* nTemporalFilterStrLut[4][8][2] */
                        {
                            /* nTemporalFilterStrLut 0 */
                            {
                                {0, 0, /*0 - 1*/},
                                {15, 15, /*0 - 1*/},
                                {20, 20, /*0 - 1*/},
                                {25, 25, /*0 - 1*/},
                                {127, 127, /*0 - 1*/},
                                {229, 229, /*0 - 1*/},
                                {242, 242, /*0 - 1*/},
                                {255, 255, /*0 - 1*/},
                            },
                            /* nTemporalFilterStrLut 1 */
                            {
                                {0, 0, /*0 - 1*/},
                                {15, 15, /*0 - 1*/},
                                {20, 20, /*0 - 1*/},
                                {25, 25, /*0 - 1*/},
                                {127, 127, /*0 - 1*/},
                                {229, 229, /*0 - 1*/},
                                {242, 242, /*0 - 1*/},
                                {255, 255, /*0 - 1*/},
                            },
                            /* nTemporalFilterStrLut 2 */
                            {
                                {0, 0, /*0 - 1*/},
                                {15, 15, /*0 - 1*/},
                                {20, 20, /*0 - 1*/},
                                {25, 25, /*0 - 1*/},
                                {127, 127, /*0 - 1*/},
                                {229, 229, /*0 - 1*/},
                                {242, 242, /*0 - 1*/},
                                {255, 255, /*0 - 1*/},
                            },
                            /* nTemporalFilterStrLut 3 */
                            {
                                {0, 0, /*0 - 1*/},
                                {15, 15, /*0 - 1*/},
                                {20, 20, /*0 - 1*/},
                                {25, 25, /*0 - 1*/},
                                {127, 127, /*0 - 1*/},
                                {229, 229, /*0 - 1*/},
                                {242, 242, /*0 - 1*/},
                                {255, 255, /*0 - 1*/},
                            },
                        },
                        /* nTemporalFilterStrLut2[4][8][2] */
                        {
                            /* nTemporalFilterStrLut2 0 */
                            {
                                {0, 255, /*0 - 1*/},
                                {15, 255, /*0 - 1*/},
                                {20, 255, /*0 - 1*/},
                                {25, 255, /*0 - 1*/},
                                {127, 255, /*0 - 1*/},
                                {229, 255, /*0 - 1*/},
                                {242, 255, /*0 - 1*/},
                                {255, 255, /*0 - 1*/},
                            },
                            /* nTemporalFilterStrLut2 1 */
                            {
                                {0, 255, /*0 - 1*/},
                                {15, 255, /*0 - 1*/},
                                {20, 255, /*0 - 1*/},
                                {25, 255, /*0 - 1*/},
                                {127, 255, /*0 - 1*/},
                                {229, 255, /*0 - 1*/},
                                {242, 255, /*0 - 1*/},
                                {255, 255, /*0 - 1*/},
                            },
                            /* nTemporalFilterStrLut2 2 */
                            {
                                {0, 255, /*0 - 1*/},
                                {15, 255, /*0 - 1*/},
                                {20, 255, /*0 - 1*/},
                                {25, 255, /*0 - 1*/},
                                {127, 255, /*0 - 1*/},
                                {229, 255, /*0 - 1*/},
                                {242, 255, /*0 - 1*/},
                                {255, 255, /*0 - 1*/},
                            },
                            /* nTemporalFilterStrLut2 3 */
                            {
                                {0, 255, /*0 - 1*/},
                                {15, 255, /*0 - 1*/},
                                {20, 255, /*0 - 1*/},
                                {25, 255, /*0 - 1*/},
                                {127, 255, /*0 - 1*/},
                                {229, 255, /*0 - 1*/},
                                {242, 255, /*0 - 1*/},
                                {255, 255, /*0 - 1*/},
                            },
                        },
                    },
                    /* tFreqAffineParam */
                    {
                        /* nParamGrpNum */
                        4,
                        /* nRefVal[4] */
                        {1024, 1792, 4096, 4294967295, /*0 - 3*/},
                        /* nLowFreqNRAffine[4] */
                        {
                            0,
                            0,
                            0,
                            0,
                        },
                        /* nMidFreqNRAffine[4] */
                        {
                            0,
                            0,
                            0,
                            0,
                        },
                        /* nHighFreqNRAffine[4][4] */
                        {
                            0,
                            0,
                            0,
                            0,
                        },
                    },
                    /* tFreqLutParam */
                    {
                        /* nParamGrpNum */
                        4,
                        /* nRefValStart[4] */
                        {1024, 1024, 1024, 1024, /*0 - 3*/},
                        /* nRefValEnd[4] */
                        {4294967295, 4294967295, 4294967295, 4294967295, /*0 - 3*/},
                        /* nLowFreqNRStrLut[4][8][2] */
                        {
                            /* nLowFreqNRStrLut 0 */
                            {
                                {1, 128, /*0 - 1*/},
                                {16, 128, /*0 - 1*/},
                                {32, 128, /*0 - 1*/},
                                {51, 128, /*0 - 1*/},
                                {64, 128, /*0 - 1*/},
                                {85, 128, /*0 - 1*/},
                                {128, 128, /*0 - 1*/},
                                {255, 128, /*0 - 1*/},
                            },
                            /* nLowFreqNRStrLut 1 */
                            {
                                {1, 128, /*0 - 1*/},
                                {16, 128, /*0 - 1*/},
                                {32, 128, /*0 - 1*/},
                                {51, 128, /*0 - 1*/},
                                {64, 128, /*0 - 1*/},
                                {85, 128, /*0 - 1*/},
                                {128, 128, /*0 - 1*/},
                                {255, 128, /*0 - 1*/},
                            },
                            /* nLowFreqNRStrLut 2 */
                            {
                                {1, 128, /*0 - 1*/},
                                {16, 128, /*0 - 1*/},
                                {32, 128, /*0 - 1*/},
                                {51, 128, /*0 - 1*/},
                                {64, 128, /*0 - 1*/},
                                {85, 128, /*0 - 1*/},
                                {128, 128, /*0 - 1*/},
                                {255, 128, /*0 - 1*/},
                            },
                            /* nLowFreqNRStrLut 3 */
                            {
                                {1, 128, /*0 - 1*/},
                                {16, 128, /*0 - 1*/},
                                {32, 128, /*0 - 1*/},
                                {51, 128, /*0 - 1*/},
                                {64, 128, /*0 - 1*/},
                                {85, 128, /*0 - 1*/},
                                {128, 128, /*0 - 1*/},
                                {255, 128, /*0 - 1*/},
                            },
                        },
                        /* nMidFreqNRStrLut[4][8][2] */
                        {
                            /* nMidFreqNRStrLut 0 */
                            {
                                {0, 128, /*0 - 1*/},
                                {20, 160, /*0 - 1*/},
                                {45, 160, /*0 - 1*/},
                                {80, 180, /*0 - 1*/},
                                {133, 180, /*0 - 1*/},
                                {167, 180, /*0 - 1*/},
                                {209, 180, /*0 - 1*/},
                                {254, 180, /*0 - 1*/},
                            },
                            /* nMidFreqNRStrLut 1 */
                            {
                                {0, 128, /*0 - 1*/},
                                {20, 160, /*0 - 1*/},
                                {45, 160, /*0 - 1*/},
                                {80, 180, /*0 - 1*/},
                                {133, 180, /*0 - 1*/},
                                {167, 180, /*0 - 1*/},
                                {209, 180, /*0 - 1*/},
                                {254, 180, /*0 - 1*/},
                            },
                            /* nMidFreqNRStrLut 2 */
                            {
                                {0, 128, /*0 - 1*/},
                                {20, 160, /*0 - 1*/},
                                {45, 160, /*0 - 1*/},
                                {80, 180, /*0 - 1*/},
                                {133, 180, /*0 - 1*/},
                                {167, 180, /*0 - 1*/},
                                {209, 180, /*0 - 1*/},
                                {254, 180, /*0 - 1*/},
                            },
                            /* nMidFreqNRStrLut 3 */
                            {
                                {0, 128, /*0 - 1*/},
                                {20, 160, /*0 - 1*/},
                                {45, 160, /*0 - 1*/},
                                {80, 180, /*0 - 1*/},
                                {133, 180, /*0 - 1*/},
                                {167, 180, /*0 - 1*/},
                                {209, 180, /*0 - 1*/},
                                {254, 180, /*0 - 1*/},
                            },
                        },
                        /* nHighFreqNRStrLut[4][8][2] */
                        {
                            /* nHighFreqNRStrLut 0 */
                            {
                                {0, 100, /*0 - 1*/},
                                {10, 100, /*0 - 1*/},
                                {25, 100, /*0 - 1*/},
                                {50, 120, /*0 - 1*/},
                                {142, 120, /*0 - 1*/},
                                {181, 140, /*0 - 1*/},
                                {224, 160, /*0 - 1*/},
                                {255, 160, /*0 - 1*/},
                            },
                            /* nHighFreqNRStrLut 1 */
                            {
                                {0, 100, /*0 - 1*/},
                                {10, 100, /*0 - 1*/},
                                {25, 100, /*0 - 1*/},
                                {50, 120, /*0 - 1*/},
                                {142, 120, /*0 - 1*/},
                                {181, 140, /*0 - 1*/},
                                {224, 160, /*0 - 1*/},
                                {255, 160, /*0 - 1*/},
                            },
                            /* nHighFreqNRStrLut 2 */
                            {
                                {0, 100, /*0 - 1*/},
                                {10, 100, /*0 - 1*/},
                                {25, 100, /*0 - 1*/},
                                {50, 120, /*0 - 1*/},
                                {142, 120, /*0 - 1*/},
                                {181, 140, /*0 - 1*/},
                                {224, 160, /*0 - 1*/},
                                {255, 160, /*0 - 1*/},
                            },
                            /* nHighFreqNRStrLut 3 */
                            {
                                {0, 100, /*0 - 1*/},
                                {10, 100, /*0 - 1*/},
                                {25, 100, /*0 - 1*/},
                                {50, 120, /*0 - 1*/},
                                {142, 120, /*0 - 1*/},
                                {181, 140, /*0 - 1*/},
                                {224, 160, /*0 - 1*/},
                                {255, 160, /*0 - 1*/},
                            },
                        },
                    },
                },
            },
            /* 2 */
            {
                /* tMeta */
                {
                    /* szModelPath[256] */
                    "/opt/etc/models/aiisp/IMX678_SDR_3840x2160_12b_HCG_ISP1_A48-48X_I1-3X_0000_00000473680_230906_AX650.axmodel",
                    /* szModelName[256] */
                    "IMX678_SDR_3840x2160_12b_HCG_ISP1_A48-48X_I1-3X_0000_00000473680_230906_AX650.axmodel",
                    /* szTemporalBaseNrName[256] */
                    "",
                    /* szSpatialBaseNrName[256] */
                    "",
                    /* nBiasIn[4] */
                    {800, 2400, 0, 0, /*0 - 3*/},
                    /* nBiasOut[4] */
                    {0, 0, 0, 0, /*0 - 3*/},
                    /* nHcgMode */
                    1,
                    /* nIsoThresholdMin */
                    4800,
                    /* nIsoThresholdMax */
                    19200,
                    /* nHdrRatioMin */
                    0,
                    /* nHdrRatioMax */
                    0,
                },
                /* tParam */
                {
                    /* tNonSensParam */
                    {
                        /* nParamGrpNum */
                        4,
                        /* nRefValStart[4] */
                        {1024, 1024, 1024, 1024, /*0 - 3*/},
                        /* nRefValEnd[4] */
                        {4294967295, 4294967295, 4294967295, 4294967295, /*0 - 3*/},
                        /* nTNRStrLut[4][8][2] */
                        {
                            /* nTNRStrLut 0 */
                            {
                                {0, 0, /*0 - 1*/},
                                {15, 15, /*0 - 1*/},
                                {20, 20, /*0 - 1*/},
                                {25, 25, /*0 - 1*/},
                                {127, 127, /*0 - 1*/},
                                {229, 229, /*0 - 1*/},
                                {242, 242, /*0 - 1*/},
                                {255, 255, /*0 - 1*/},
                            },
                            /* nTNRStrLut 1 */
                            {
                                {0, 0, /*0 - 1*/},
                                {15, 15, /*0 - 1*/},
                                {20, 20, /*0 - 1*/},
                                {25, 25, /*0 - 1*/},
                                {127, 127, /*0 - 1*/},
                                {229, 229, /*0 - 1*/},
                                {242, 242, /*0 - 1*/},
                                {255, 255, /*0 - 1*/},
                            },
                            /* nTNRStrLut 2 */
                            {
                                {0, 0, /*0 - 1*/},
                                {15, 15, /*0 - 1*/},
                                {20, 20, /*0 - 1*/},
                                {25, 25, /*0 - 1*/},
                                {127, 127, /*0 - 1*/},
                                {229, 229, /*0 - 1*/},
                                {242, 242, /*0 - 1*/},
                                {255, 255, /*0 - 1*/},
                            },
                            /* nTNRStrLut 3 */
                            {
                                {0, 0, /*0 - 1*/},
                                {15, 15, /*0 - 1*/},
                                {20, 20, /*0 - 1*/},
                                {25, 25, /*0 - 1*/},
                                {127, 127, /*0 - 1*/},
                                {229, 229, /*0 - 1*/},
                                {242, 242, /*0 - 1*/},
                                {255, 255, /*0 - 1*/},
                            },
                        },
                        /* nShpStrLut[4][8][2] */
                        {
                            /* nShpStrLut 0 */
                            {
                                {0, 255, /*0 - 1*/},
                                {15, 240, /*0 - 1*/},
                                {20, 235, /*0 - 1*/},
                                {25, 230, /*0 - 1*/},
                                {127, 128, /*0 - 1*/},
                                {229, 25, /*0 - 1*/},
                                {242, 13, /*0 - 1*/},
                                {255, 0, /*0 - 1*/},
                            },
                            /* nShpStrLut 1 */
                            {
                                {0, 255, /*0 - 1*/},
                                {15, 240, /*0 - 1*/},
                                {20, 235, /*0 - 1*/},
                                {25, 230, /*0 - 1*/},
                                {127, 128, /*0 - 1*/},
                                {229, 25, /*0 - 1*/},
                                {242, 13, /*0 - 1*/},
                                {255, 0, /*0 - 1*/},
                            },
                            /* nShpStrLut 2 */
                            {
                                {0, 255, /*0 - 1*/},
                                {15, 240, /*0 - 1*/},
                                {20, 235, /*0 - 1*/},
                                {25, 230, /*0 - 1*/},
                                {127, 128, /*0 - 1*/},
                                {229, 25, /*0 - 1*/},
                                {242, 13, /*0 - 1*/},
                                {255, 0, /*0 - 1*/},
                            },
                            /* nShpStrLut 3 */
                            {
                                {0, 255, /*0 - 1*/},
                                {15, 240, /*0 - 1*/},
                                {20, 235, /*0 - 1*/},
                                {25, 230, /*0 - 1*/},
                                {127, 128, /*0 - 1*/},
                                {229, 25, /*0 - 1*/},
                                {242, 13, /*0 - 1*/},
                                {255, 0, /*0 - 1*/},
                            },
                        },
                        /* nTemporalFilterStrLut[4][8][2] */
                        {
                            /* nTemporalFilterStrLut 0 */
                            {
                                {0, 0, /*0 - 1*/},
                                {15, 15, /*0 - 1*/},
                                {20, 20, /*0 - 1*/},
                                {25, 25, /*0 - 1*/},
                                {127, 127, /*0 - 1*/},
                                {229, 229, /*0 - 1*/},
                                {242, 242, /*0 - 1*/},
                                {255, 255, /*0 - 1*/},
                            },
                            /* nTemporalFilterStrLut 1 */
                            {
                                {0, 0, /*0 - 1*/},
                                {15, 15, /*0 - 1*/},
                                {20, 20, /*0 - 1*/},
                                {25, 25, /*0 - 1*/},
                                {127, 127, /*0 - 1*/},
                                {229, 229, /*0 - 1*/},
                                {242, 242, /*0 - 1*/},
                                {255, 255, /*0 - 1*/},
                            },
                            /* nTemporalFilterStrLut 2 */
                            {
                                {0, 0, /*0 - 1*/},
                                {15, 15, /*0 - 1*/},
                                {20, 20, /*0 - 1*/},
                                {25, 25, /*0 - 1*/},
                                {127, 127, /*0 - 1*/},
                                {229, 229, /*0 - 1*/},
                                {242, 242, /*0 - 1*/},
                                {255, 255, /*0 - 1*/},
                            },
                            /* nTemporalFilterStrLut 3 */
                            {
                                {0, 0, /*0 - 1*/},
                                {15, 15, /*0 - 1*/},
                                {20, 20, /*0 - 1*/},
                                {25, 25, /*0 - 1*/},
                                {127, 127, /*0 - 1*/},
                                {229, 229, /*0 - 1*/},
                                {242, 242, /*0 - 1*/},
                                {255, 255, /*0 - 1*/},
                            },
                        },
                        /* nTemporalFilterStrLut2[4][8][2] */
                        {
                            /* nTemporalFilterStrLut2 0 */
                            {
                                {0, 255, /*0 - 1*/},
                                {15, 255, /*0 - 1*/},
                                {20, 255, /*0 - 1*/},
                                {25, 255, /*0 - 1*/},
                                {127, 255, /*0 - 1*/},
                                {229, 255, /*0 - 1*/},
                                {242, 255, /*0 - 1*/},
                                {255, 255, /*0 - 1*/},
                            },
                            /* nTemporalFilterStrLut2 1 */
                            {
                                {0, 255, /*0 - 1*/},
                                {15, 255, /*0 - 1*/},
                                {20, 255, /*0 - 1*/},
                                {25, 255, /*0 - 1*/},
                                {127, 255, /*0 - 1*/},
                                {229, 255, /*0 - 1*/},
                                {242, 255, /*0 - 1*/},
                                {255, 255, /*0 - 1*/},
                            },
                            /* nTemporalFilterStrLut2 2 */
                            {
                                {0, 255, /*0 - 1*/},
                                {15, 255, /*0 - 1*/},
                                {20, 255, /*0 - 1*/},
                                {25, 255, /*0 - 1*/},
                                {127, 255, /*0 - 1*/},
                                {229, 255, /*0 - 1*/},
                                {242, 255, /*0 - 1*/},
                                {255, 255, /*0 - 1*/},
                            },
                            /* nTemporalFilterStrLut2 3 */
                            {
                                {0, 255, /*0 - 1*/},
                                {15, 255, /*0 - 1*/},
                                {20, 255, /*0 - 1*/},
                                {25, 255, /*0 - 1*/},
                                {127, 255, /*0 - 1*/},
                                {229, 255, /*0 - 1*/},
                                {242, 255, /*0 - 1*/},
                                {255, 255, /*0 - 1*/},
                            },
                        },
                    },
                    /* tFreqAffineParam */
                    {
                        /* nParamGrpNum */
                        4,
                        /* nRefVal[4] */
                        {1024, 1792, 4096, 4294967295, /*0 - 3*/},
                        /* nLowFreqNRAffine[4] */
                        {
                            0,
                            0,
                            0,
                            0,
                        },
                        /* nMidFreqNRAffine[4] */
                        {
                            0,
                            0,
                            0,
                            0,
                        },
                        /* nHighFreqNRAffine[4][4] */
                        {
                            0,
                            0,
                            0,
                            0,
                        },
                    },
                    /* tFreqLutParam */
                    {
                        /* nParamGrpNum */
                        4,
                        /* nRefValStart[4] */
                        {133120, 266240, 399360, 532400, /*0 - 3*/},
                        /* nRefValEnd[4] */
                        {266240, 399360, 532480, 4294967295, /*0 - 3*/},
                        /* nLowFreqNRStrLut[4][8][2] */
                        {
                            /* nLowFreqNRStrLut 0 */
                            {
                                {1, 168, /*0 - 1*/},
                                {16, 168, /*0 - 1*/},
                                {32, 168, /*0 - 1*/},
                                {51, 168, /*0 - 1*/},
                                {64, 168, /*0 - 1*/},
                                {85, 168, /*0 - 1*/},
                                {128, 168, /*0 - 1*/},
                                {255, 168, /*0 - 1*/},
                            },
                            /* nLowFreqNRStrLut 1 */
                            {
                                {1, 168, /*0 - 1*/},
                                {16, 168, /*0 - 1*/},
                                {32, 168, /*0 - 1*/},
                                {51, 168, /*0 - 1*/},
                                {64, 168, /*0 - 1*/},
                                {85, 168, /*0 - 1*/},
                                {128, 168, /*0 - 1*/},
                                {255, 168, /*0 - 1*/},
                            },
                            /* nLowFreqNRStrLut 2 */
                            {
                                {1, 168, /*0 - 1*/},
                                {16, 168, /*0 - 1*/},
                                {32, 168, /*0 - 1*/},
                                {51, 168, /*0 - 1*/},
                                {64, 168, /*0 - 1*/},
                                {85, 168, /*0 - 1*/},
                                {128, 168, /*0 - 1*/},
                                {255, 168, /*0 - 1*/},
                            },
                            /* nLowFreqNRStrLut 3 */
                            {
                                {1, 168, /*0 - 1*/},
                                {16, 168, /*0 - 1*/},
                                {32, 168, /*0 - 1*/},
                                {51, 168, /*0 - 1*/},
                                {64, 168, /*0 - 1*/},
                                {85, 168, /*0 - 1*/},
                                {128, 168, /*0 - 1*/},
                                {255, 168, /*0 - 1*/},
                            },
                        },
                        /* nMidFreqNRStrLut[4][8][2] */
                        {
                            /* nMidFreqNRStrLut 0 */
                            {
                                {1, 168, /*0 - 1*/},
                                {16, 168, /*0 - 1*/},
                                {32, 168, /*0 - 1*/},
                                {51, 168, /*0 - 1*/},
                                {64, 168, /*0 - 1*/},
                                {85, 168, /*0 - 1*/},
                                {128, 168, /*0 - 1*/},
                                {255, 168, /*0 - 1*/},
                            },
                            /* nMidFreqNRStrLut 1 */
                            {
                                {1, 168, /*0 - 1*/},
                                {16, 168, /*0 - 1*/},
                                {32, 168, /*0 - 1*/},
                                {51, 168, /*0 - 1*/},
                                {64, 168, /*0 - 1*/},
                                {85, 168, /*0 - 1*/},
                                {128, 168, /*0 - 1*/},
                                {255, 168, /*0 - 1*/},
                            },
                            /* nMidFreqNRStrLut 2 */
                            {
                                {1, 168, /*0 - 1*/},
                                {16, 168, /*0 - 1*/},
                                {32, 168, /*0 - 1*/},
                                {51, 168, /*0 - 1*/},
                                {64, 168, /*0 - 1*/},
                                {85, 168, /*0 - 1*/},
                                {128, 168, /*0 - 1*/},
                                {255, 168, /*0 - 1*/},
                            },
                            /* nMidFreqNRStrLut 3 */
                            {
                                {1, 168, /*0 - 1*/},
                                {16, 168, /*0 - 1*/},
                                {32, 168, /*0 - 1*/},
                                {51, 168, /*0 - 1*/},
                                {64, 168, /*0 - 1*/},
                                {85, 168, /*0 - 1*/},
                                {128, 168, /*0 - 1*/},
                                {255, 168, /*0 - 1*/},
                            },
                        },
                        /* nHighFreqNRStrLut[4][8][2] */
                        {
                            /* nHighFreqNRStrLut 0 */
                            {
                                {0, 120, /*0 - 1*/},
                                {10, 120, /*0 - 1*/},
                                {25, 120, /*0 - 1*/},
                                {50, 120, /*0 - 1*/},
                                {142, 140, /*0 - 1*/},
                                {181, 160, /*0 - 1*/},
                                {224, 180, /*0 - 1*/},
                                {255, 200, /*0 - 1*/},
                            },
                            /* nHighFreqNRStrLut 1 */
                            {
                                {0, 120, /*0 - 1*/},
                                {10, 120, /*0 - 1*/},
                                {25, 120, /*0 - 1*/},
                                {50, 120, /*0 - 1*/},
                                {142, 140, /*0 - 1*/},
                                {181, 160, /*0 - 1*/},
                                {224, 180, /*0 - 1*/},
                                {255, 200, /*0 - 1*/},
                            },
                            /* nHighFreqNRStrLut 2 */
                            {
                                {0, 120, /*0 - 1*/},
                                {10, 120, /*0 - 1*/},
                                {25, 120, /*0 - 1*/},
                                {50, 120, /*0 - 1*/},
                                {142, 140, /*0 - 1*/},
                                {181, 160, /*0 - 1*/},
                                {224, 180, /*0 - 1*/},
                                {255, 200, /*0 - 1*/},
                            },
                            /* nHighFreqNRStrLut 3 */
                            {
                                {0, 120, /*0 - 1*/},
                                {10, 120, /*0 - 1*/},
                                {25, 120, /*0 - 1*/},
                                {50, 120, /*0 - 1*/},
                                {142, 140, /*0 - 1*/},
                                {181, 160, /*0 - 1*/},
                                {224, 180, /*0 - 1*/},
                                {255, 200, /*0 - 1*/},
                            },
                        },
                    },
                },
            },
            /* 3 */
            {
                /* tMeta */
                {
                    /* szModelPath[256] */
                    "/opt/etc/models/aiisp/IMX678_SDR_3840x2160_12b_HCG_ISP1_A48-48X_I3-8X_0000_00000473661_230906_AX650.axmodel",
                    /* szModelName[256] */
                    "IMX678_SDR_3840x2160_12b_HCG_ISP1_A48-48X_I3-8X_0000_00000473661_230906_AX650.axmodel",
                    /* szTemporalBaseNrName[256] */
                    "",
                    /* szSpatialBaseNrName[256] */
                    "",
                    /* nBiasIn[4] */
                    {800, 2400, 0, 0, /*0 - 3*/},
                    /* nBiasOut[4] */
                    {0, 0, 0, 0, /*0 - 3*/},
                    /* nHcgMode */
                    1,
                    /* nIsoThresholdMin */
                    19200,
                    /* nIsoThresholdMax */
                    38400,
                    /* nHdrRatioMin */
                    0,
                    /* nHdrRatioMax */
                    0,
                },
                /* tParam */
                {
                    /* tNonSensParam */
                    {
                        /* nParamGrpNum */
                        4,
                        /* nRefValStart[4] */
                        {1024, 1024, 1024, 1024, /*0 - 3*/},
                        /* nRefValEnd[4] */
                        {4294967295, 4294967295, 4294967295, 4294967295, /*0 - 3*/},
                        /* nTNRStrLut[4][8][2] */
                        {
                            /* nTNRStrLut 0 */
                            {
                                {0, 0, /*0 - 1*/},
                                {15, 15, /*0 - 1*/},
                                {20, 20, /*0 - 1*/},
                                {25, 25, /*0 - 1*/},
                                {127, 127, /*0 - 1*/},
                                {229, 229, /*0 - 1*/},
                                {242, 242, /*0 - 1*/},
                                {255, 255, /*0 - 1*/},
                            },
                            /* nTNRStrLut 1 */
                            {
                                {0, 0, /*0 - 1*/},
                                {15, 15, /*0 - 1*/},
                                {20, 20, /*0 - 1*/},
                                {25, 25, /*0 - 1*/},
                                {127, 127, /*0 - 1*/},
                                {229, 229, /*0 - 1*/},
                                {242, 242, /*0 - 1*/},
                                {255, 255, /*0 - 1*/},
                            },
                            /* nTNRStrLut 2 */
                            {
                                {0, 0, /*0 - 1*/},
                                {15, 15, /*0 - 1*/},
                                {20, 20, /*0 - 1*/},
                                {25, 25, /*0 - 1*/},
                                {127, 127, /*0 - 1*/},
                                {229, 229, /*0 - 1*/},
                                {242, 242, /*0 - 1*/},
                                {255, 255, /*0 - 1*/},
                            },
                            /* nTNRStrLut 3 */
                            {
                                {0, 0, /*0 - 1*/},
                                {15, 15, /*0 - 1*/},
                                {20, 20, /*0 - 1*/},
                                {25, 25, /*0 - 1*/},
                                {127, 127, /*0 - 1*/},
                                {229, 229, /*0 - 1*/},
                                {242, 242, /*0 - 1*/},
                                {255, 255, /*0 - 1*/},
                            },
                        },
                        /* nShpStrLut[4][8][2] */
                        {
                            /* nShpStrLut 0 */
                            {
                                {0, 255, /*0 - 1*/},
                                {15, 240, /*0 - 1*/},
                                {20, 235, /*0 - 1*/},
                                {25, 230, /*0 - 1*/},
                                {127, 128, /*0 - 1*/},
                                {229, 25, /*0 - 1*/},
                                {242, 13, /*0 - 1*/},
                                {255, 0, /*0 - 1*/},
                            },
                            /* nShpStrLut 1 */
                            {
                                {0, 255, /*0 - 1*/},
                                {15, 240, /*0 - 1*/},
                                {20, 235, /*0 - 1*/},
                                {25, 230, /*0 - 1*/},
                                {127, 128, /*0 - 1*/},
                                {229, 25, /*0 - 1*/},
                                {242, 13, /*0 - 1*/},
                                {255, 0, /*0 - 1*/},
                            },
                            /* nShpStrLut 2 */
                            {
                                {0, 255, /*0 - 1*/},
                                {15, 240, /*0 - 1*/},
                                {20, 235, /*0 - 1*/},
                                {25, 230, /*0 - 1*/},
                                {127, 128, /*0 - 1*/},
                                {229, 25, /*0 - 1*/},
                                {242, 13, /*0 - 1*/},
                                {255, 0, /*0 - 1*/},
                            },
                            /* nShpStrLut 3 */
                            {
                                {0, 255, /*0 - 1*/},
                                {15, 240, /*0 - 1*/},
                                {20, 235, /*0 - 1*/},
                                {25, 230, /*0 - 1*/},
                                {127, 128, /*0 - 1*/},
                                {229, 25, /*0 - 1*/},
                                {242, 13, /*0 - 1*/},
                                {255, 0, /*0 - 1*/},
                            },
                        },
                        /* nTemporalFilterStrLut[4][8][2] */
                        {
                            /* nTemporalFilterStrLut 0 */
                            {
                                {0, 0, /*0 - 1*/},
                                {15, 15, /*0 - 1*/},
                                {20, 20, /*0 - 1*/},
                                {25, 25, /*0 - 1*/},
                                {127, 127, /*0 - 1*/},
                                {229, 229, /*0 - 1*/},
                                {242, 242, /*0 - 1*/},
                                {255, 255, /*0 - 1*/},
                            },
                            /* nTemporalFilterStrLut 1 */
                            {
                                {0, 0, /*0 - 1*/},
                                {15, 15, /*0 - 1*/},
                                {20, 20, /*0 - 1*/},
                                {25, 25, /*0 - 1*/},
                                {127, 127, /*0 - 1*/},
                                {229, 229, /*0 - 1*/},
                                {242, 242, /*0 - 1*/},
                                {255, 255, /*0 - 1*/},
                            },
                            /* nTemporalFilterStrLut 2 */
                            {
                                {0, 0, /*0 - 1*/},
                                {15, 15, /*0 - 1*/},
                                {20, 20, /*0 - 1*/},
                                {25, 25, /*0 - 1*/},
                                {127, 127, /*0 - 1*/},
                                {229, 229, /*0 - 1*/},
                                {242, 242, /*0 - 1*/},
                                {255, 255, /*0 - 1*/},
                            },
                            /* nTemporalFilterStrLut 3 */
                            {
                                {0, 0, /*0 - 1*/},
                                {15, 15, /*0 - 1*/},
                                {20, 20, /*0 - 1*/},
                                {25, 25, /*0 - 1*/},
                                {127, 127, /*0 - 1*/},
                                {229, 229, /*0 - 1*/},
                                {242, 242, /*0 - 1*/},
                                {255, 255, /*0 - 1*/},
                            },
                        },
                        /* nTemporalFilterStrLut2[4][8][2] */
                        {
                            /* nTemporalFilterStrLut2 0 */
                            {
                                {0, 255, /*0 - 1*/},
                                {15, 255, /*0 - 1*/},
                                {20, 255, /*0 - 1*/},
                                {25, 255, /*0 - 1*/},
                                {127, 255, /*0 - 1*/},
                                {229, 255, /*0 - 1*/},
                                {242, 255, /*0 - 1*/},
                                {255, 255, /*0 - 1*/},
                            },
                            /* nTemporalFilterStrLut2 1 */
                            {
                                {0, 255, /*0 - 1*/},
                                {15, 255, /*0 - 1*/},
                                {20, 255, /*0 - 1*/},
                                {25, 255, /*0 - 1*/},
                                {127, 255, /*0 - 1*/},
                                {229, 255, /*0 - 1*/},
                                {242, 255, /*0 - 1*/},
                                {255, 255, /*0 - 1*/},
                            },
                            /* nTemporalFilterStrLut2 2 */
                            {
                                {0, 255, /*0 - 1*/},
                                {15, 255, /*0 - 1*/},
                                {20, 255, /*0 - 1*/},
                                {25, 255, /*0 - 1*/},
                                {127, 255, /*0 - 1*/},
                                {229, 255, /*0 - 1*/},
                                {242, 255, /*0 - 1*/},
                                {255, 255, /*0 - 1*/},
                            },
                            /* nTemporalFilterStrLut2 3 */
                            {
                                {0, 255, /*0 - 1*/},
                                {15, 255, /*0 - 1*/},
                                {20, 255, /*0 - 1*/},
                                {25, 255, /*0 - 1*/},
                                {127, 255, /*0 - 1*/},
                                {229, 255, /*0 - 1*/},
                                {242, 255, /*0 - 1*/},
                                {255, 255, /*0 - 1*/},
                            },
                        },
                    },
                    /* tFreqAffineParam */
                    {
                        /* nParamGrpNum */
                        4,
                        /* nRefVal[4] */
                        {1024, 1792, 4096, 4294967295, /*0 - 3*/},
                        /* nLowFreqNRAffine[4] */
                        {
                            0,
                            0,
                            0,
                            0,
                        },
                        /* nMidFreqNRAffine[4] */
                        {
                            0,
                            0,
                            0,
                            0,
                        },
                        /* nHighFreqNRAffine[4][4] */
                        {
                            0,
                            0,
                            0,
                            0,
                        },
                    },
                    /* tFreqLutParam */
                    {
                        /* nParamGrpNum */
                        4,
                        /* nRefValStart[4] */
                        {1024, 1024, 1024, 1024, /*0 - 3*/},
                        /* nRefValEnd[4] */
                        {4294967295, 4294967295, 4294967295, 4294967295, /*0 - 3*/},
                        /* nLowFreqNRStrLut[4][8][2] */
                        {
                            /* nLowFreqNRStrLut 0 */
                            {
                                {0, 200, /*0 - 1*/},
                                {15, 200, /*0 - 1*/},
                                {20, 200, /*0 - 1*/},
                                {25, 200, /*0 - 1*/},
                                {127, 200, /*0 - 1*/},
                                {229, 200, /*0 - 1*/},
                                {242, 200, /*0 - 1*/},
                                {255, 200, /*0 - 1*/},
                            },
                            /* nLowFreqNRStrLut 1 */
                            {
                                {0, 200, /*0 - 1*/},
                                {15, 200, /*0 - 1*/},
                                {20, 200, /*0 - 1*/},
                                {25, 200, /*0 - 1*/},
                                {127, 200, /*0 - 1*/},
                                {229, 200, /*0 - 1*/},
                                {242, 200, /*0 - 1*/},
                                {255, 200, /*0 - 1*/},
                            },
                            /* nLowFreqNRStrLut 2 */
                            {
                                {0, 200, /*0 - 1*/},
                                {15, 200, /*0 - 1*/},
                                {20, 200, /*0 - 1*/},
                                {25, 200, /*0 - 1*/},
                                {127, 200, /*0 - 1*/},
                                {229, 200, /*0 - 1*/},
                                {242, 200, /*0 - 1*/},
                                {255, 200, /*0 - 1*/},
                            },
                            /* nLowFreqNRStrLut 3 */
                            {
                                {0, 200, /*0 - 1*/},
                                {15, 200, /*0 - 1*/},
                                {20, 200, /*0 - 1*/},
                                {25, 200, /*0 - 1*/},
                                {127, 200, /*0 - 1*/},
                                {229, 200, /*0 - 1*/},
                                {242, 200, /*0 - 1*/},
                                {255, 200, /*0 - 1*/},
                            },
                        },
                        /* nMidFreqNRStrLut[4][8][2] */
                        {
                            /* nMidFreqNRStrLut 0 */
                            {
                                {0, 128, /*0 - 1*/},
                                {29, 168, /*0 - 1*/},
                                {61, 188, /*0 - 1*/},
                                {97, 200, /*0 - 1*/},
                                {151, 200, /*0 - 1*/},
                                {210, 200, /*0 - 1*/},
                                {236, 200, /*0 - 1*/},
                                {255, 200, /*0 - 1*/},
                            },
                            /* nMidFreqNRStrLut 1 */
                            {
                                {0, 128, /*0 - 1*/},
                                {29, 168, /*0 - 1*/},
                                {61, 188, /*0 - 1*/},
                                {97, 200, /*0 - 1*/},
                                {151, 200, /*0 - 1*/},
                                {210, 200, /*0 - 1*/},
                                {236, 200, /*0 - 1*/},
                                {255, 200, /*0 - 1*/},
                            },
                            /* nMidFreqNRStrLut 2 */
                            {
                                {0, 128, /*0 - 1*/},
                                {29, 168, /*0 - 1*/},
                                {61, 188, /*0 - 1*/},
                                {97, 200, /*0 - 1*/},
                                {151, 200, /*0 - 1*/},
                                {210, 200, /*0 - 1*/},
                                {236, 200, /*0 - 1*/},
                                {255, 200, /*0 - 1*/},
                            },
                            /* nMidFreqNRStrLut 3 */
                            {
                                {0, 128, /*0 - 1*/},
                                {29, 168, /*0 - 1*/},
                                {61, 188, /*0 - 1*/},
                                {97, 200, /*0 - 1*/},
                                {151, 200, /*0 - 1*/},
                                {210, 200, /*0 - 1*/},
                                {236, 200, /*0 - 1*/},
                                {255, 200, /*0 - 1*/},
                            },
                        },
                        /* nHighFreqNRStrLut[4][8][2] */
                        {
                            /* nHighFreqNRStrLut 0 */
                            {
                                {0, 120, /*0 - 1*/},
                                {10, 120, /*0 - 1*/},
                                {21, 128, /*0 - 1*/},
                                {38, 148, /*0 - 1*/},
                                {142, 148, /*0 - 1*/},
                                {181, 148, /*0 - 1*/},
                                {224, 168, /*0 - 1*/},
                                {255, 180, /*0 - 1*/},
                            },
                            /* nHighFreqNRStrLut 1 */
                            {
                                {0, 120, /*0 - 1*/},
                                {10, 120, /*0 - 1*/},
                                {21, 128, /*0 - 1*/},
                                {38, 148, /*0 - 1*/},
                                {142, 148, /*0 - 1*/},
                                {181, 148, /*0 - 1*/},
                                {224, 168, /*0 - 1*/},
                                {255, 180, /*0 - 1*/},
                            },
                            /* nHighFreqNRStrLut 2 */
                            {
                                {0, 120, /*0 - 1*/},
                                {10, 120, /*0 - 1*/},
                                {21, 128, /*0 - 1*/},
                                {38, 148, /*0 - 1*/},
                                {142, 148, /*0 - 1*/},
                                {181, 148, /*0 - 1*/},
                                {224, 168, /*0 - 1*/},
                                {255, 180, /*0 - 1*/},
                            },
                            /* nHighFreqNRStrLut 3 */
                            {
                                {0, 120, /*0 - 1*/},
                                {10, 120, /*0 - 1*/},
                                {21, 128, /*0 - 1*/},
                                {38, 148, /*0 - 1*/},
                                {142, 148, /*0 - 1*/},
                                {181, 148, /*0 - 1*/},
                                {224, 168, /*0 - 1*/},
                                {255, 180, /*0 - 1*/},
                            },
                        },
                    },
                },
            },
        },
    },
};

static AX_ISP_IQ_AICE_PARAM_T aice_param_sdr = {
    /* nEnable */
    0,
    /* nAutoMode */
    0,
    /* bUpdateTable */
    0,
    /* nRefMode */
    1,
    /* tManualParam */
    {
        /* tMeta */
        {
            /* szModelPath */
            "/opt/etc/models/aiisp/IMX678_HDR_3840x2160_10b_LCG_ISP1_A1-4X_0000_00000638971_240111_AX650.axmodel",
            /* szModelName */
            "IMX678_HDR_3840x2160_10b_LCG_ISP1_A1-4X_0000_00000638971_240111_AX650.axmodel",
            "",
            /* szSpatialBaseNrName[256] */
            "",
            /* nBiasIn[4] */
            {1200, 1200, 1200, 1200, /*0 - 3*/},
            /* nBiasOut[4] */
            {0, 0, 0, 0, /*0 - 3*/},
        },
        /* tParams */
        {
            /* tNr */
            {
                /* tNonSensParam */
                {
                    /* nTNRStrLut[8][2] */
                    {
                        {0, 0, /*0 - 1*/},
                        {15, 15, /*0 - 1*/},
                        {20, 20, /*0 - 1*/},
                        {25, 25, /*0 - 1*/},
                        {127, 127, /*0 - 1*/},
                        {229, 229, /*0 - 1*/},
                        {242, 242, /*0 - 1*/},
                        {255, 255, /*0 - 1*/},
                    },
                    /* nShpStrLut[8][2] */
                    {
                        {0, 255, /*0 - 1*/},
                        {15, 240, /*0 - 1*/},
                        {20, 235, /*0 - 1*/},
                        {25, 230, /*0 - 1*/},
                        {127, 128, /*0 - 1*/},
                        {229, 25, /*0 - 1*/},
                        {242, 13, /*0 - 1*/},
                        {255, 0, /*0 - 1*/},
                    },
                    /* nTemporalFilterStrLut[8][2] */
                    {
                        {0, 255, /*0 - 1*/},
                        {15, 255, /*0 - 1*/},
                        {20, 255, /*0 - 1*/},
                        {25, 255, /*0 - 1*/},
                        {127, 255, /*0 - 1*/},
                        {229, 255, /*0 - 1*/},
                        {242, 255, /*0 - 1*/},
                        {255, 255, /*0 - 1*/},
                    },
                    /* nTemporalFilterStrLut2[8][2] */
                    {
                        {0, 255, /*0 - 1*/},
                        {15, 255, /*0 - 1*/},
                        {20, 255, /*0 - 1*/},
                        {25, 255, /*0 - 1*/},
                        {127, 255, /*0 - 1*/},
                        {229, 255, /*0 - 1*/},
                        {242, 255, /*0 - 1*/},
                        {255, 255, /*0 - 1*/},
                    },
                },
            },
            /* tCe */
            {
                /* nLumaStrLut[16][2] */
                {
                    {0, 255, /*0 - 1*/},
                    {25, 255, /*0 - 1*/},
                    {50, 255, /*0 - 1*/},
                    {75, 255, /*0 - 1*/},
                    {100, 255, /*0 - 1*/},
                    {125, 255, /*0 - 1*/},
                    {150, 255, /*0 - 1*/},
                    {175, 255, /*0 - 1*/},
                    {180, 255, /*0 - 1*/},
                    {190, 255, /*0 - 1*/},
                    {200, 255, /*0 - 1*/},
                    {210, 255, /*0 - 1*/},
                    {220, 255, /*0 - 1*/},
                    {230, 255, /*0 - 1*/},
                    {240, 255, /*0 - 1*/},
                    {250, 255, /*0 - 1*/},
                },
                /* nLowFreqAffine */
                0,
                /* nMidFreqAffine */
                0,
                /* nHighFreqAffine */
                0,
            },
        },
    },
    /* tAutoParam */
    {
        /* nAutoModelNum */
        0,
        /* tAutoModelTable[32] */
        {
            /* 0 */
            {
                /* tMeta */
                {
                    /* szModelPath */
                    "/opt/etc/models/aiisp/IMX678_HDR_3840x2160_10b_LCG_ISP1_A1-4X_0000_00000638971_240111_AX650.axmodel",
                    /* szModelName */
                    "IMX678_HDR_3840x2160_10b_LCG_ISP1_A1-4X_0000_00000638971_240111_AX650.axmodel",
                    /* szTemporalBaseNrName */
                    "",
                    /* szSpatialBaseNrName */
                    "",
                    /* nHcgMode */
                    0,
                    /* nIsoThresholdMin */
                    100,
                    /* nIsoThresholdMax */
                    630,
                    /* nHdrRatioMin */
                    0,
                    /* nHdrRatioMax */
                    0,
                    /* nBiasIn[4] */
                    {1200, 1200, 1200, 1200, /*0 - 3*/},
                    /* nBiasOut[4] */
                    {0, 0, 0, 0, /*0 - 3*/},
                },
                /* tParams */
                {
                    /* tNr */
                    {
                        /* tNonSensParam */
                        {
                            /* nParamGrpNum */
                            4,
                            /* nRefValStart[4] */
                            {1024, 1024, 1024, 1024, /*0 - 3*/},
                            /* nRefValEnd[4] */
                            {4294967295, 4294967295, 4294967295, 4294967295, /*0 - 3*/},
                            /* nTNRStrLut[4][8][2] */
                            {
                                /* nTNRStrLut 0 */
                                {
                                    {0, 0, /*0 - 1*/},
                                    {15, 15, /*0 - 1*/},
                                    {20, 20, /*0 - 1*/},
                                    {25, 25, /*0 - 1*/},
                                    {127, 127, /*0 - 1*/},
                                    {229, 229, /*0 - 1*/},
                                    {242, 242, /*0 - 1*/},
                                    {255, 255, /*0 - 1*/},
                                },
                                /* nTNRStrLut 1 */
                                {
                                    {0, 0, /*0 - 1*/},
                                    {15, 15, /*0 - 1*/},
                                    {20, 20, /*0 - 1*/},
                                    {25, 25, /*0 - 1*/},
                                    {127, 127, /*0 - 1*/},
                                    {229, 229, /*0 - 1*/},
                                    {242, 242, /*0 - 1*/},
                                    {255, 255, /*0 - 1*/},
                                },
                                /* nTNRStrLut 2 */
                                {
                                    {0, 0, /*0 - 1*/},
                                    {15, 15, /*0 - 1*/},
                                    {20, 20, /*0 - 1*/},
                                    {25, 25, /*0 - 1*/},
                                    {127, 127, /*0 - 1*/},
                                    {229, 229, /*0 - 1*/},
                                    {242, 242, /*0 - 1*/},
                                    {255, 255, /*0 - 1*/},
                                },
                                /* nTNRStrLut 3 */
                                {
                                    {0, 0, /*0 - 1*/},
                                    {15, 15, /*0 - 1*/},
                                    {20, 20, /*0 - 1*/},
                                    {25, 25, /*0 - 1*/},
                                    {127, 127, /*0 - 1*/},
                                    {229, 229, /*0 - 1*/},
                                    {242, 242, /*0 - 1*/},
                                    {255, 255, /*0 - 1*/},
                                },
                            },
                            /* nShpStrLut[4][8][2] */
                            {
                                /* nShpStrLut 0 */
                                {
                                    {0, 255, /*0 - 1*/},
                                    {15, 240, /*0 - 1*/},
                                    {20, 235, /*0 - 1*/},
                                    {25, 230, /*0 - 1*/},
                                    {127, 128, /*0 - 1*/},
                                    {229, 25, /*0 - 1*/},
                                    {242, 13, /*0 - 1*/},
                                    {255, 0, /*0 - 1*/},
                                },
                                /* nShpStrLut 1 */
                                {
                                    {0, 255, /*0 - 1*/},
                                    {15, 240, /*0 - 1*/},
                                    {20, 235, /*0 - 1*/},
                                    {25, 230, /*0 - 1*/},
                                    {127, 128, /*0 - 1*/},
                                    {229, 25, /*0 - 1*/},
                                    {242, 13, /*0 - 1*/},
                                    {255, 0, /*0 - 1*/},
                                },
                                /* nShpStrLut 2 */
                                {
                                    {0, 255, /*0 - 1*/},
                                    {15, 240, /*0 - 1*/},
                                    {20, 235, /*0 - 1*/},
                                    {25, 230, /*0 - 1*/},
                                    {127, 128, /*0 - 1*/},
                                    {229, 25, /*0 - 1*/},
                                    {242, 13, /*0 - 1*/},
                                    {255, 0, /*0 - 1*/},
                                },
                                /* nShpStrLut 3 */
                                {
                                    {0, 255, /*0 - 1*/},
                                    {15, 240, /*0 - 1*/},
                                    {20, 235, /*0 - 1*/},
                                    {25, 230, /*0 - 1*/},
                                    {127, 128, /*0 - 1*/},
                                    {229, 25, /*0 - 1*/},
                                    {242, 13, /*0 - 1*/},
                                    {255, 0, /*0 - 1*/},
                                },
                            },
                            /* nTemporalFilterStrLut[4][8][2] */
                            {
                                /* nTemporalFilterStrLut 0 */
                                {
                                    {0, 255, /*0 - 1*/},
                                    {15, 255, /*0 - 1*/},
                                    {20, 255, /*0 - 1*/},
                                    {25, 255, /*0 - 1*/},
                                    {127, 255, /*0 - 1*/},
                                    {229, 255, /*0 - 1*/},
                                    {242, 255, /*0 - 1*/},
                                    {255, 255, /*0 - 1*/},
                                },
                                /* nTemporalFilterStrLut 1 */
                                {
                                    {0, 255, /*0 - 1*/},
                                    {15, 255, /*0 - 1*/},
                                    {20, 255, /*0 - 1*/},
                                    {25, 255, /*0 - 1*/},
                                    {127, 255, /*0 - 1*/},
                                    {229, 255, /*0 - 1*/},
                                    {242, 255, /*0 - 1*/},
                                    {255, 255, /*0 - 1*/},
                                },
                                /* nTemporalFilterStrLut 2 */
                                {
                                    {0, 255, /*0 - 1*/},
                                    {15, 255, /*0 - 1*/},
                                    {20, 255, /*0 - 1*/},
                                    {25, 255, /*0 - 1*/},
                                    {127, 255, /*0 - 1*/},
                                    {229, 255, /*0 - 1*/},
                                    {242, 255, /*0 - 1*/},
                                    {255, 255, /*0 - 1*/},
                                },
                                /* nTemporalFilterStrLut 3 */
                                {
                                    {0, 255, /*0 - 1*/},
                                    {15, 255, /*0 - 1*/},
                                    {20, 255, /*0 - 1*/},
                                    {25, 255, /*0 - 1*/},
                                    {127, 255, /*0 - 1*/},
                                    {229, 255, /*0 - 1*/},
                                    {242, 255, /*0 - 1*/},
                                    {255, 255, /*0 - 1*/},
                                },
                            },
                            /* nTemporalFilterStrLut2[4][8][2] */
                            {
                                /* nTemporalFilterStrLut2 0 */
                                {
                                    {0, 255, /*0 - 1*/},
                                    {15, 255, /*0 - 1*/},
                                    {20, 255, /*0 - 1*/},
                                    {25, 255, /*0 - 1*/},
                                    {127, 255, /*0 - 1*/},
                                    {229, 255, /*0 - 1*/},
                                    {242, 255, /*0 - 1*/},
                                    {255, 255, /*0 - 1*/},
                                },
                                /* nTemporalFilterStrLut2 1 */
                                {
                                    {0, 255, /*0 - 1*/},
                                    {15, 255, /*0 - 1*/},
                                    {20, 255, /*0 - 1*/},
                                    {25, 255, /*0 - 1*/},
                                    {127, 255, /*0 - 1*/},
                                    {229, 255, /*0 - 1*/},
                                    {242, 255, /*0 - 1*/},
                                    {255, 255, /*0 - 1*/},
                                },
                                /* nTemporalFilterStrLut2 2 */
                                {
                                    {0, 255, /*0 - 1*/},
                                    {15, 255, /*0 - 1*/},
                                    {20, 255, /*0 - 1*/},
                                    {25, 255, /*0 - 1*/},
                                    {127, 255, /*0 - 1*/},
                                    {229, 255, /*0 - 1*/},
                                    {242, 255, /*0 - 1*/},
                                    {255, 255, /*0 - 1*/},
                                },
                                /* nTemporalFilterStrLut2 3 */
                                {
                                    {0, 255, /*0 - 1*/},
                                    {15, 255, /*0 - 1*/},
                                    {20, 255, /*0 - 1*/},
                                    {25, 255, /*0 - 1*/},
                                    {127, 255, /*0 - 1*/},
                                    {229, 255, /*0 - 1*/},
                                    {242, 255, /*0 - 1*/},
                                    {255, 255, /*0 - 1*/},
                                },
                            },
                        },
                    },
                    /* tCe */
                    {
                        /* tLumaLut */
                        {
                            /* nParamGrpNum */
                            4,
                            /* nRefValStart[4] */
                            {1024, 1024, 1024, 1024, /*0 - 3*/},
                            /* nRefValEnd[4] */
                            {4294967295, 4294967295, 4294967295, 4294967295, /*0 - 3*/},
                            /* nLumaStrLut[4][16][2] */
                            {
                                /* nLumaStrLut 0 */
                                {
                                    {0, 25, /*0 - 1*/},
                                    {25, 50, /*0 - 1*/},
                                    {50, 75, /*0 - 1*/},
                                    {75, 100, /*0 - 1*/},
                                    {100, 125, /*0 - 1*/},
                                    {125, 150, /*0 - 1*/},
                                    {150, 175, /*0 - 1*/},
                                    {175, 200, /*0 - 1*/},
                                    {180, 180, /*0 - 1*/},
                                    {190, 190, /*0 - 1*/},
                                    {200, 200, /*0 - 1*/},
                                    {210, 210, /*0 - 1*/},
                                    {220, 220, /*0 - 1*/},
                                    {230, 230, /*0 - 1*/},
                                    {240, 240, /*0 - 1*/},
                                    {250, 250, /*0 - 1*/},
                                },
                                /* nLumaStrLut 1 */
                                {
                                    {0, 25, /*0 - 1*/},
                                    {25, 50, /*0 - 1*/},
                                    {50, 75, /*0 - 1*/},
                                    {75, 100, /*0 - 1*/},
                                    {100, 125, /*0 - 1*/},
                                    {125, 150, /*0 - 1*/},
                                    {150, 175, /*0 - 1*/},
                                    {175, 200, /*0 - 1*/},
                                    {180, 180, /*0 - 1*/},
                                    {190, 190, /*0 - 1*/},
                                    {200, 200, /*0 - 1*/},
                                    {210, 210, /*0 - 1*/},
                                    {220, 220, /*0 - 1*/},
                                    {230, 230, /*0 - 1*/},
                                    {240, 240, /*0 - 1*/},
                                    {250, 250, /*0 - 1*/},
                                },
                                /* nLumaStrLut 2 */
                                {
                                    {0, 25, /*0 - 1*/},
                                    {25, 50, /*0 - 1*/},
                                    {50, 75, /*0 - 1*/},
                                    {75, 100, /*0 - 1*/},
                                    {100, 125, /*0 - 1*/},
                                    {125, 150, /*0 - 1*/},
                                    {150, 175, /*0 - 1*/},
                                    {175, 200, /*0 - 1*/},
                                    {180, 180, /*0 - 1*/},
                                    {190, 190, /*0 - 1*/},
                                    {200, 200, /*0 - 1*/},
                                    {210, 210, /*0 - 1*/},
                                    {220, 220, /*0 - 1*/},
                                    {230, 230, /*0 - 1*/},
                                    {240, 240, /*0 - 1*/},
                                    {250, 250, /*0 - 1*/},
                                },
                                /* nLumaStrLut 3 */
                                {
                                    {0, 25, /*0 - 1*/},
                                    {25, 50, /*0 - 1*/},
                                    {50, 75, /*0 - 1*/},
                                    {75, 100, /*0 - 1*/},
                                    {100, 125, /*0 - 1*/},
                                    {125, 150, /*0 - 1*/},
                                    {150, 175, /*0 - 1*/},
                                    {175, 200, /*0 - 1*/},
                                    {180, 180, /*0 - 1*/},
                                    {190, 190, /*0 - 1*/},
                                    {200, 200, /*0 - 1*/},
                                    {210, 210, /*0 - 1*/},
                                    {220, 220, /*0 - 1*/},
                                    {230, 230, /*0 - 1*/},
                                    {240, 240, /*0 - 1*/},
                                    {250, 250, /*0 - 1*/},
                                },
                            },
                        },
                        /* tFreqAffine */
                        {
                            /* nParamGrpNum */
                            4,
                            /* nRefVal[4] */
                            {1024, 1792, 4096, 4294967295, /*0 - 3*/},
                            /* nLowFreqAffine[4] */
                            {0, 0, 0, 0, /*0 - 3*/},
                            /* nMidFreqAffine[4] */
                            {0, 0, 0, 0, /*0 - 3*/},
                            /* nHighFreqAffine[4] */
                            {0, 0, 0, 0, /*0 - 3*/},
                        },
                    },
                },
            },
        },
    },
};

static AX_ISP_IQ_RAW3DNR_PARAM_T raw3dnr_param_sdr = {
    /* nRaw3dnrEn */
    1,
    /* nAutoMode */
    1,
    /* nRefMode */
    1,
    /* tGlb */
    {
        /* eTnrWorkMode */
        /* 0:AX_RAW3DNR_FIRST_3D, 1:AX_RAW3DNR_FIRST_2D, 2:AX_RAW3DNR_ONLY_2D */
        AX_RAW3DNR_FIRST_3D,
        /* nSfMeshEnable */
        0,
        /* nRefMaskUpdMode */
        0,
        /* nReadNoiseCoeffsHcg[3] */
        {5346073, 51579982, -63181926, /*0 - 2*/},
        /* nReadNoiseCoeffsLcg[3] */
        {3778668, 52490547, -41283111, /*0 - 2*/},
        /* nShotNoiseCoeffsHcg[2] */
        {146899483, 3351969, /*0 - 1*/},
        /* nShotNoiseCoeffsLcg[2] */
        {59058074, -1192880, /*0 - 1*/},
    },
    /* tManual */
    {
        /* nFpnRatio */
        26,
        /* tMtd */
        {
            /* eMtdMode */
            /* 0:AX_RAW3DNR_SOFT_THRES, 1:AX_RAW3DNR_HARD_THRES */
            AX_RAW3DNR_HARD_THRES,
            /* nMtdHardThres[3] */
            {170, 160, 150, /*0 - 2*/},
            /* nMtdLf0KernelWt[3] */
            {85, 85, 85, /*0 - 2*/},
            /* nMtdLf1KernelWt[3] */
            {85, 85, 85, /*0 - 2*/},
            /* nMtdLf0Wt[4] */
            {256, 256, 256, 256, /*0 - 3*/},
            /* nMtdLf0Amp[4] */
            {256, 256, 256, 256, /*0 - 3*/},
            /* nMtdLf1Wt */
            0,
            /* nMtdLf1Amp */
            0,
            /* tMtdHrParam */
            {
                /* nMtdLumaStrValue[17] */
                {470, 470, 470, 470, 470, 470, 470, 470, 470, 470, 470, 470, 470, 564, 564, 564, 564, /*0 - 16*/},
                /* nMtdLumaStrDelta[16] */
                {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, /*0 - 15*/},
                /* nMtdStrTf */
                64,
            },
        },
        /* tTd */
        {
            /* nMtdSoftMode */
            0,
            /* nMtdLimit */
            31,
            /* nMtdMotionCurve[2] */
            {0, 0, /*0 - 1*/},
            /* nMtdMotionLut[15] */
            {256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, /* 0 - 14 */},
            /* nMtdNoiseCoeffsL[17] */
            {256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, /*0 - 16*/},
            /* nMtdNoiseCoeffsS[17] */
            {256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, /*0 - 16*/},
            /* nMtdNoiseCoeffsVs[17] */
            {256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, /*0 - 16*/},
        },
        /* tSd */
        {
            /* nSfHighlightProtect */
            64000,
            /* nSfLumaThresDelta[16] */
            {0, 1, 2, 3, 4, 5, 6, 7, 7, 8, 10, 11, 12, 13, 14, 15, /*0 - 15*/},
            /* nSfKernelWt0[3] */
            {128, 128, 128, /*0 - 2*/},
            /* nSfKernelWt1[2] */
            {128, 128, /*0 - 1*/},
            /* nDetailBackfillDelta[16] */
            {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, /*0 - 15*/},
            /* nDetailBackfillResValue[17] */
            {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 16*/},
            /* nDetailBackfillRefValue[17] */
            {128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, /*0 - 16*/},
            /* nDetailBackfillRatioRes[4] */
            {128, 128, 128, 128, /*0 - 3*/},
            /* nDetailBackfillRatioRef[4] */
            {128, 128, 128, 128, /*0 - 3*/},
            /* tSfMeshStr */
            {
                /* nSfMeshStr[11][11] */
                {
                    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 10*/},
                    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 10*/},
                    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 10*/},
                    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 10*/},
                    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 10*/},
                    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 10*/},
                    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 10*/},
                    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 10*/},
                    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 10*/},
                    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 10*/},
                    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 10*/},
                },
            },
            /* tHrParam */
            {
                /* nSfDetailPreserve */
                0,
                /* nSfSmoothStr */
                64,
                /* nSfLumaThresValue[17] */
                {15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 64, 64, 64, 64, 64, 64, /*0 - 16*/},
                /* nSfMotionStrL[6] */
                {384, 384, 384, 384, 384, 384, /*0 - 5*/},
                /* nSfMotionStrS[6] */
                {256, 256, 256, 256, 256, 256, /*0 - 5*/},
                /* nSfMotionStrVs[6] */
                {256, 256, 256, 256, 256, 256, /*0 - 5*/},
                /* nSfNoiseCoeffsL[16] */
                {256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, /*0 - 15*/},
                /* nSfNoiseCoeffsS[16] */
                {256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, /*0 - 15*/},
                /* nSfNoiseCoeffsVs[16] */
                {256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, /*0 - 15*/},
            },
        },
    },
    /* tAuto */
    {
        /* nParamGrpNum */
        12,
        /* nRefVal[16] */
        {1024, 2048, 16384, 32768, 49152, 65024, 65536, 131072, 262144, 524288, 1048576, 2048000, /*0 - 11*/},
        /* nFpnRatio[16] */
        {26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, /*0 - 11*/},
        /* tMtd */
        {
            /* eMtdMode[16] */
            /* 0:AX_RAW3DNR_SOFT_THRES, 1:AX_RAW3DNR_HARD_THRES */
            {AX_RAW3DNR_HARD_THRES, AX_RAW3DNR_HARD_THRES, AX_RAW3DNR_HARD_THRES, AX_RAW3DNR_HARD_THRES, AX_RAW3DNR_HARD_THRES, AX_RAW3DNR_HARD_THRES, AX_RAW3DNR_HARD_THRES, AX_RAW3DNR_HARD_THRES, AX_RAW3DNR_HARD_THRES, AX_RAW3DNR_HARD_THRES, AX_RAW3DNR_HARD_THRES, AX_RAW3DNR_HARD_THRES, /*0 - 11*/},
            /* nMtdHardThres[16][3] */
            {
                {384, 320, 280, /*0 - 2*/},
                {384, 320, 280, /*0 - 2*/},
                {384, 320, 280, /*0 - 2*/},
                {384, 320, 280, /*0 - 2*/},
                {384, 320, 280, /*0 - 2*/},
                {320, 310, 280, /*0 - 2*/},
                {100, 50, 10, /*0 - 2*/},
                {100, 50, 10, /*0 - 2*/},
                {100, 50, 10, /*0 - 2*/},
                {100, 50, 10, /*0 - 2*/},
                {100, 50, 10, /*0 - 2*/},
                {100, 50, 10, /*0 - 2*/},
            },
            /* nMtdLf0KernelWt[16][3] */
            {
                {85, 85, 85, /*0 - 2*/},
                {85, 85, 85, /*0 - 2*/},
                {85, 85, 85, /*0 - 2*/},
                {85, 85, 85, /*0 - 2*/},
                {85, 85, 85, /*0 - 2*/},
                {85, 85, 85, /*0 - 2*/},
                {85, 85, 85, /*0 - 2*/},
                {85, 85, 85, /*0 - 2*/},
                {85, 85, 85, /*0 - 2*/},
                {85, 85, 85, /*0 - 2*/},
                {85, 85, 85, /*0 - 2*/},
                {85, 85, 85, /*0 - 2*/},
            },
            /* nMtdLf1KernelWt[16][3] */
            {
                {85, 85, 85, /*0 - 2*/},
                {85, 85, 85, /*0 - 2*/},
                {85, 85, 85, /*0 - 2*/},
                {85, 85, 85, /*0 - 2*/},
                {85, 85, 85, /*0 - 2*/},
                {85, 85, 85, /*0 - 2*/},
                {85, 85, 85, /*0 - 2*/},
                {85, 85, 85, /*0 - 2*/},
                {85, 85, 85, /*0 - 2*/},
                {85, 85, 85, /*0 - 2*/},
                {85, 85, 85, /*0 - 2*/},
                {85, 85, 85, /*0 - 2*/},
            },
            /* nMtdLf0Wt[16][4] */
            {
                {256, 256, 256, 256, /*0 - 3*/},
                {256, 256, 256, 256, /*0 - 3*/},
                {256, 256, 256, 256, /*0 - 3*/},
                {256, 256, 256, 256, /*0 - 3*/},
                {256, 256, 256, 256, /*0 - 3*/},
                {256, 256, 256, 256, /*0 - 3*/},
                {256, 256, 256, 256, /*0 - 3*/},
                {256, 256, 256, 256, /*0 - 3*/},
                {256, 256, 256, 256, /*0 - 3*/},
                {256, 256, 256, 256, /*0 - 3*/},
                {256, 256, 256, 256, /*0 - 3*/},
                {256, 256, 256, 256, /*0 - 3*/},
            },
            /* nMtdLf0Amp[16][4] */
            {
                {256, 256, 256, 256, /*0 - 3*/},
                {256, 256, 256, 256, /*0 - 3*/},
                {256, 256, 256, 256, /*0 - 3*/},
                {256, 256, 256, 256, /*0 - 3*/},
                {256, 256, 256, 256, /*0 - 3*/},
                {256, 256, 256, 256, /*0 - 3*/},
                {256, 256, 256, 256, /*0 - 3*/},
                {256, 256, 256, 256, /*0 - 3*/},
                {256, 256, 256, 256, /*0 - 3*/},
                {256, 256, 256, 256, /*0 - 3*/},
                {256, 256, 256, 256, /*0 - 3*/},
                {256, 256, 256, 256, /*0 - 3*/},
            },
            /* nMtdLf1Wt[16] */
            {256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, /*0 - 11*/},
            /* nMtdLf1Amp[16] */
            {256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, /*0 - 11*/},
            /* tHrAuto */
            {
                /* nHrGrpNum */
                1,
                /* nHrRefVal[4] */
                {1024, /*0 - 0*/},
                /* tHrParam */
                {
                    /* 0 */
                    {
                        /* nMtdLumaStrValue[16][17] */
                        {
                            {256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, /*0 - 16*/},
                            {256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, /*0 - 16*/},
                            {256, 256, 256, 256, 264, 264, 264, 264, 284, 284, 284, 284, 284, 284, 284, 284, 284, /*0 - 16*/},
                            {256, 256, 256, 256, 264, 264, 264, 264, 284, 284, 284, 284, 284, 284, 284, 284, 284, /*0 - 16*/},
                            {256, 256, 256, 256, 264, 264, 264, 264, 284, 284, 284, 284, 284, 284, 284, 284, 284, /*0 - 16*/},
                            {256, 256, 256, 256, 264, 264, 264, 264, 284, 284, 284, 284, 284, 284, 284, 284, 284, /*0 - 16*/},
                            {512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 564, 564, 564, 564, /*0 - 16*/},
                            {512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 564, 564, 564, 564, /*0 - 16*/},
                            {470, 470, 470, 470, 470, 470, 470, 470, 470, 470, 470, 470, 470, 528, 528, 528, 528, /*0 - 16*/},
                            {470, 470, 470, 470, 470, 470, 470, 470, 470, 470, 470, 470, 470, 564, 564, 564, 564, /*0 - 16*/},
                            {470, 470, 470, 470, 470, 470, 470, 470, 470, 470, 470, 470, 470, 564, 564, 564, 564, /*0 - 16*/},
                            {470, 470, 470, 470, 470, 470, 470, 470, 470, 470, 470, 470, 470, 564, 564, 564, 564, /*0 - 16*/},
                        },
                        /* nMtdLumaStrDelta[16][16] */
                        {
                            {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, /*0 - 15*/},
                            {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, /*0 - 15*/},
                            {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, /*0 - 15*/},
                            {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, /*0 - 15*/},
                            {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, /*0 - 15*/},
                            {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, /*0 - 15*/},
                            {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, /*0 - 15*/},
                            {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, /*0 - 15*/},
                            {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, /*0 - 15*/},
                            {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, /*0 - 15*/},
                            {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, /*0 - 15*/},
                            {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, /*0 - 15*/},
                        },
                        /* nMtdStrTf[16] */
                        {32, 32, 32, 32, 32, 64, 64, 64, 64, 64, 64, 64, /*0 - 11*/},
                    },
                },
            },
        },
        /* tTd */
        {
            /* nMtdSoftMode[16] */
            {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 11*/},
            /* nMtdLimit[16] */
            {31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, /*0 - 11*/},
            /* nMtdMotionCurve[16][2] */
            {
                {0, 0, /*0 - 1*/},
                {0, 0, /*0 - 1*/},
                {0, 0, /*0 - 1*/},
                {0, 0, /*0 - 1*/},
                {0, 0, /*0 - 1*/},
                {0, 0, /*0 - 1*/},
                {0, 0, /*0 - 1*/},
                {0, 0, /*0 - 1*/},
                {0, 0, /*0 - 1*/},
                {0, 0, /*0 - 1*/},
                {0, 0, /*0 - 1*/},
                {0, 0, /*0 - 1*/},
            },
            /* nMtdMotionLut[16][15] */
            {
                {256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, /*0 - 14*/},
                {256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, /*0 - 14*/},
                {256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, /*0 - 14*/},
                {256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, /*0 - 14*/},
                {256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, /*0 - 14*/},
                {256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, /*0 - 14*/},
                {256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, /*0 - 14*/},
                {256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, /*0 - 14*/},
                {256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, /*0 - 14*/},
                {256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, /*0 - 14*/},
                {256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, /*0 - 14*/},
                {256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, /*0 - 14*/},
            },
            /* nMtdNoiseCoeffsL[16][17] */
            {
                {256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, /*0 - 16*/},
                {256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, /*0 - 16*/},
                {256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, /*0 - 16*/},
                {256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, /*0 - 16*/},
                {256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, /*0 - 16*/},
                {256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, /*0 - 16*/},
                {256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, /*0 - 16*/},
                {256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, /*0 - 16*/},
                {256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, /*0 - 16*/},
                {256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, /*0 - 16*/},
                {256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, /*0 - 16*/},
                {256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, /*0 - 16*/},
            },
            /* nMtdNoiseCoeffsS[16][17] */
            {
                {256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, /*0 - 16*/},
                {256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, /*0 - 16*/},
                {256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, /*0 - 16*/},
                {256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, /*0 - 16*/},
                {256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, /*0 - 16*/},
                {256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, /*0 - 16*/},
                {256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, /*0 - 16*/},
                {256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, /*0 - 16*/},
                {256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, /*0 - 16*/},
                {256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, /*0 - 16*/},
                {256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, /*0 - 16*/},
                {256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, /*0 - 16*/},
            },
            /* nMtdNoiseCoeffsVs[16][17] */
            {
                {256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, /*0 - 16*/},
                {256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, /*0 - 16*/},
                {256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, /*0 - 16*/},
                {256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, /*0 - 16*/},
                {256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, /*0 - 16*/},
                {256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, /*0 - 16*/},
                {256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, /*0 - 16*/},
                {256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, /*0 - 16*/},
                {256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, /*0 - 16*/},
                {256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, /*0 - 16*/},
                {256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, /*0 - 16*/},
                {256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, /*0 - 16*/},
            },
        },
        /* tSd */
        {
            /* nSfHighlightProtect[16] */
            {64000, 64000, 64000, 64000, 64000, 64000, 64000, 64000, 64000, 64000, 64000, 64000, /*0 - 11*/},
            /* nSfLumaThresDelta[16][16] */
            {
                {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, /*0 - 15*/},
                {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, /*0 - 15*/},
                {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, /*0 - 15*/},
                {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, /*0 - 15*/},
                {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, /*0 - 15*/},
                {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, /*0 - 15*/},
                {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, /*0 - 15*/},
                {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, /*0 - 15*/},
                {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, /*0 - 15*/},
                {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, /*0 - 15*/},
                {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, /*0 - 15*/},
                {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, /*0 - 15*/},
            },
            /* nSfKernelWt0[16][3] */
            {
                {128, 128, 128, /*0 - 2*/},
                {128, 128, 128, /*0 - 2*/},
                {128, 128, 128, /*0 - 2*/},
                {128, 128, 128, /*0 - 2*/},
                {128, 128, 128, /*0 - 2*/},
                {128, 128, 128, /*0 - 2*/},
                {128, 128, 128, /*0 - 2*/},
                {128, 128, 128, /*0 - 2*/},
                {128, 128, 128, /*0 - 2*/},
                {128, 128, 128, /*0 - 2*/},
                {128, 128, 128, /*0 - 2*/},
                {128, 128, 128, /*0 - 2*/},
            },
            /* nSfKernelWt1[16][2] */
            {
                {128, 128, /*0 - 1*/},
                {128, 128, /*0 - 1*/},
                {128, 128, /*0 - 1*/},
                {128, 128, /*0 - 1*/},
                {128, 128, /*0 - 1*/},
                {128, 128, /*0 - 1*/},
                {128, 128, /*0 - 1*/},
                {128, 128, /*0 - 1*/},
                {128, 128, /*0 - 1*/},
                {128, 128, /*0 - 1*/},
                {128, 128, /*0 - 1*/},
                {128, 128, /*0 - 1*/},
            },
            /* nDetailBackfillDelta[16][16] */
            {
                {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, /*0 - 15*/},
                {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, /*0 - 15*/},
                {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, /*0 - 15*/},
                {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, /*0 - 15*/},
                {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, /*0 - 15*/},
                {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, /*0 - 15*/},
                {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, /*0 - 15*/},
                {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, /*0 - 15*/},
                {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, /*0 - 15*/},
                {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, /*0 - 15*/},
                {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, /*0 - 15*/},
                {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, /*0 - 15*/},
            },
            /* nDetailBackfillResValue[16][17] */
            {
                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 16*/},
                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 16*/},
                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 16*/},
                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 16*/},
                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 16*/},
                {0, 0, 4, 4, 12, 12, 24, 24, 32, 32, 32, 32, 24, 24, 24, 24, 24, /*0 - 16*/},
                {12, 12, 12, 12, 12, 12, 24, 24, 32, 32, 32, 32, 24, 24, 24, 24, 24, /*0 - 16*/},
                {16, 16, 16, 16, 24, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, /*0 - 16*/},
                {16, 16, 16, 16, 24, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, /*0 - 16*/},
                {16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 0, /*0 - 16*/},
                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 16*/},
                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 16*/},
            },
            /* nDetailBackfillRefValue[16][17] */
            {
                {128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, /*0 - 16*/},
                {128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, /*0 - 16*/},
                {128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, /*0 - 16*/},
                {128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, /*0 - 16*/},
                {128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, /*0 - 16*/},
                {128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, /*0 - 16*/},
                {128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, /*0 - 16*/},
                {128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, /*0 - 16*/},
                {128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, /*0 - 16*/},
                {128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, /*0 - 16*/},
                {128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, /*0 - 16*/},
                {128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, /*0 - 16*/},
            },
            /* nDetailBackfillRatioRes[16][4] */
            {
                {128, 128, 128, 128, /*0 - 3*/},
                {128, 128, 128, 128, /*0 - 3*/},
                {128, 128, 128, 128, /*0 - 3*/},
                {128, 128, 128, 128, /*0 - 3*/},
                {128, 128, 128, 128, /*0 - 3*/},
                {128, 128, 128, 128, /*0 - 3*/},
                {128, 128, 128, 128, /*0 - 3*/},
                {128, 128, 128, 128, /*0 - 3*/},
                {128, 128, 128, 128, /*0 - 3*/},
                {128, 128, 128, 128, /*0 - 3*/},
                {128, 128, 128, 128, /*0 - 3*/},
                {128, 128, 128, 128, /*0 - 3*/},
            },
            /* nDetailBackfillRatioRef[16][4] */
            {
                {128, 128, 128, 128, /*0 - 3*/},
                {128, 128, 128, 128, /*0 - 3*/},
                {128, 128, 128, 128, /*0 - 3*/},
                {128, 128, 128, 128, /*0 - 3*/},
                {128, 128, 128, 128, /*0 - 3*/},
                {128, 128, 128, 128, /*0 - 3*/},
                {128, 128, 128, 128, /*0 - 3*/},
                {128, 128, 128, 128, /*0 - 3*/},
                {128, 128, 128, 128, /*0 - 3*/},
                {128, 128, 128, 128, /*0 - 3*/},
                {128, 128, 128, 128, /*0 - 3*/},
                {128, 128, 128, 128, /*0 - 3*/},
            },
            /* tSfMeshStr[16] */
            {
                /* 0 */
                {
                    /* nSfMeshStr[11][11] */
                    {
                        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 10*/},
                        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 10*/},
                        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 10*/},
                        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 10*/},
                        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 10*/},
                        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 10*/},
                        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 10*/},
                        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 10*/},
                        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 10*/},
                        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 10*/},
                        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 10*/},
                    },
                },
                /* 1 */
                {
                    /* nSfMeshStr[11][11] */
                    {
                        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 10*/},
                        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 10*/},
                        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 10*/},
                        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 10*/},
                        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 10*/},
                        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 10*/},
                        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 10*/},
                        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 10*/},
                        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 10*/},
                        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 10*/},
                        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 10*/},
                    },
                },
                /* 2 */
                {
                    /* nSfMeshStr[11][11] */
                    {
                        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 10*/},
                        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 10*/},
                        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 10*/},
                        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 10*/},
                        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 10*/},
                        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 10*/},
                        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 10*/},
                        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 10*/},
                        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 10*/},
                        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 10*/},
                        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 10*/},
                    },
                },
                /* 3 */
                {
                    /* nSfMeshStr[11][11] */
                    {
                        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 10*/},
                        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 10*/},
                        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 10*/},
                        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 10*/},
                        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 10*/},
                        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 10*/},
                        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 10*/},
                        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 10*/},
                        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 10*/},
                        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 10*/},
                        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 10*/},
                    },
                },
                /* 4 */
                {
                    /* nSfMeshStr[11][11] */
                    {
                        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 10*/},
                        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 10*/},
                        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 10*/},
                        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 10*/},
                        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 10*/},
                        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 10*/},
                        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 10*/},
                        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 10*/},
                        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 10*/},
                        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 10*/},
                        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 10*/},
                    },
                },
                /* 5 */
                {
                    /* nSfMeshStr[11][11] */
                    {
                        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 10*/},
                        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 10*/},
                        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 10*/},
                        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 10*/},
                        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 10*/},
                        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 10*/},
                        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 10*/},
                        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 10*/},
                        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 10*/},
                        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 10*/},
                        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 10*/},
                    },
                },
                /* 6 */
                {
                    /* nSfMeshStr[11][11] */
                    {
                        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 10*/},
                        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 10*/},
                        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 10*/},
                        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 10*/},
                        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 10*/},
                        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 10*/},
                        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 10*/},
                        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 10*/},
                        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 10*/},
                        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 10*/},
                        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 10*/},
                    },
                },
                /* 7 */
                {
                    /* nSfMeshStr[11][11] */
                    {
                        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 10*/},
                        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 10*/},
                        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 10*/},
                        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 10*/},
                        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 10*/},
                        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 10*/},
                        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 10*/},
                        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 10*/},
                        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 10*/},
                        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 10*/},
                        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 10*/},
                    },
                },
                /* 8 */
                {
                    /* nSfMeshStr[11][11] */
                    {
                        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 10*/},
                        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 10*/},
                        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 10*/},
                        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 10*/},
                        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 10*/},
                        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 10*/},
                        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 10*/},
                        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 10*/},
                        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 10*/},
                        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 10*/},
                        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 10*/},
                    },
                },
                /* 9 */
                {
                    /* nSfMeshStr[11][11] */
                    {
                        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 10*/},
                        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 10*/},
                        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 10*/},
                        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 10*/},
                        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 10*/},
                        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 10*/},
                        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 10*/},
                        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 10*/},
                        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 10*/},
                        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 10*/},
                        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 10*/},
                    },
                },
                /* 10 */
                {
                    /* nSfMeshStr[11][11] */
                    {
                        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 10*/},
                        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 10*/},
                        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 10*/},
                        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 10*/},
                        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 10*/},
                        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 10*/},
                        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 10*/},
                        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 10*/},
                        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 10*/},
                        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 10*/},
                        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 10*/},
                    },
                },
                /* 11 */
                {
                    /* nSfMeshStr[11][11] */
                    {
                        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 10*/},
                        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 10*/},
                        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 10*/},
                        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 10*/},
                        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 10*/},
                        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 10*/},
                        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 10*/},
                        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 10*/},
                        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 10*/},
                        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 10*/},
                        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 10*/},
                    },
                },
            },
            /* tHrAuto */
            {
                /* nHrGrpNum */
                1,
                /* nHrRefVal[4] */
                {1024, /*0 - 0*/},
                /* tHrParam */
                {
                    /* 0 */
                    {
                        /* nSfDetailPreserve[16] */
                        {32, 32, 32, 32, 32, 32, 16, 16, 16, 16, 16, 16, /*0 - 11*/},
                        /* nSfSmoothStr[16] */
                        {96, 96, 96, 96, 96, 96, 96, 96, 96, 128, 128, 128, /*0 - 11*/},
                        /* nSfLumaThresValue[16][17] */
                        {
                            {256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, /*0 - 16*/},
                            {256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, /*0 - 16*/},
                            {256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, /*0 - 16*/},
                            {256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, /*0 - 16*/},
                            {256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, /*0 - 16*/},
                            {256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, /*0 - 16*/},
                            {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 16*/},
                            {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 64, 64, 64, 64, 64, 64, 64, /*0 - 16*/},
                            {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 64, 64, 64, 64, 64, 64, 64, /*0 - 16*/},
                            {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 64, 64, 64, 64, 64, 64, 64, /*0 - 16*/},
                            {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 64, 64, 64, 64, 64, 64, 64, /*0 - 16*/},
                            {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 64, 64, 64, 64, 64, 64, 64, /*0 - 16*/},
                        },
                        /* nSfMotionStrL[16][6] */
                        {
                            {256, 256, 256, 256, 256, 256, /*0 - 5*/},
                            {256, 256, 256, 256, 256, 256, /*0 - 5*/},
                            {256, 256, 256, 256, 256, 256, /*0 - 5*/},
                            {256, 256, 256, 256, 300, 300, /*0 - 5*/},
                            {284, 284, 284, 284, 320, 320, /*0 - 5*/},
                            {320, 320, 320, 320, 340, 340, /*0 - 5*/},
                            {256, 256, 256, 256, 256, 256, /*0 - 5*/},
                            {256, 256, 256, 256, 256, 256, /*0 - 5*/},
                            {200, 200, 220, 256, 256, 256, /*0 - 5*/},
                            {200, 200, 220, 256, 256, 256, /*0 - 5*/},
                            {200, 200, 220, 256, 256, 256, /*0 - 5*/},
                            {200, 200, 220, 256, 256, 256, /*0 - 5*/},
                        },
                        /* nSfMotionStrS[16][6] */
                        {
                            {256, 256, 256, 256, 256, 256, /*0 - 5*/},
                            {256, 256, 256, 256, 256, 256, /*0 - 5*/},
                            {256, 256, 256, 256, 256, 256, /*0 - 5*/},
                            {256, 256, 256, 256, 256, 256, /*0 - 5*/},
                            {256, 256, 256, 256, 256, 256, /*0 - 5*/},
                            {256, 256, 256, 256, 256, 256, /*0 - 5*/},
                            {256, 256, 256, 256, 256, 256, /*0 - 5*/},
                            {256, 256, 256, 256, 256, 256, /*0 - 5*/},
                            {256, 256, 256, 256, 256, 256, /*0 - 5*/},
                            {256, 256, 256, 256, 256, 256, /*0 - 5*/},
                            {256, 256, 256, 256, 256, 256, /*0 - 5*/},
                            {256, 256, 256, 256, 256, 256, /*0 - 5*/},
                        },
                        /* nSfMotionStrVs[16][6] */
                        {
                            {256, 256, 256, 256, 256, 256, /*0 - 5*/},
                            {256, 256, 256, 256, 256, 256, /*0 - 5*/},
                            {256, 256, 256, 256, 256, 256, /*0 - 5*/},
                            {256, 256, 256, 256, 256, 256, /*0 - 5*/},
                            {256, 256, 256, 256, 256, 256, /*0 - 5*/},
                            {256, 256, 256, 256, 256, 256, /*0 - 5*/},
                            {256, 256, 256, 256, 256, 256, /*0 - 5*/},
                            {256, 256, 256, 256, 256, 256, /*0 - 5*/},
                            {256, 256, 256, 256, 256, 256, /*0 - 5*/},
                            {256, 256, 256, 256, 256, 256, /*0 - 5*/},
                            {256, 256, 256, 256, 256, 256, /*0 - 5*/},
                            {256, 256, 256, 256, 256, 256, /*0 - 5*/},
                        },
                        /* nSfNoiseCoeffsL[16][16] */
                        {
                            {256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, /*0 - 15*/},
                            {256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, /*0 - 15*/},
                            {256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, /*0 - 15*/},
                            {256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, /*0 - 15*/},
                            {256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, /*0 - 15*/},
                            {256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, /*0 - 15*/},
                            {256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, /*0 - 15*/},
                            {256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, /*0 - 15*/},
                            {256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, /*0 - 15*/},
                            {256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, /*0 - 15*/},
                            {256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, /*0 - 15*/},
                            {256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, /*0 - 15*/},
                        },
                        /* nSfNoiseCoeffsS[16][16] */
                        {
                            {256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, /*0 - 15*/},
                            {256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, /*0 - 15*/},
                            {256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, /*0 - 15*/},
                            {256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, /*0 - 15*/},
                            {256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, /*0 - 15*/},
                            {256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, /*0 - 15*/},
                            {256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, /*0 - 15*/},
                            {256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, /*0 - 15*/},
                            {256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, /*0 - 15*/},
                            {256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, /*0 - 15*/},
                            {256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, /*0 - 15*/},
                            {256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, /*0 - 15*/},
                        },
                        /* nSfNoiseCoeffsVs[16][16] */
                        {
                            {256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, /*0 - 15*/},
                            {256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, /*0 - 15*/},
                            {256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, /*0 - 15*/},
                            {256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, /*0 - 15*/},
                            {256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, /*0 - 15*/},
                            {256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, /*0 - 15*/},
                            {256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, /*0 - 15*/},
                            {256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, /*0 - 15*/},
                            {256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, /*0 - 15*/},
                            {256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, /*0 - 15*/},
                            {256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, /*0 - 15*/},
                            {256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, /*0 - 15*/},
                        },
                    },
                },
            },
        },
    },
};

static AX_ISP_IQ_LSC_PARAM_T lsc_param_sdr = {
    /* nLscEn */
    0,
    /* nRefMode */
    0,
    /* nMeshMode */
    0,
    /* nAutoMode */
    0,
    /* nMeshRows */
    50,
    /* nMeshCols */
    45,
    /* tManualParam */
    {
        /* nLumaRatio */
        50,
        /* nColorRatio */
        100,
        /* nLumaMeshLut[52][98] */
        {
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
        },
        /* nRRMeshLut[52][98] */
        {
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
        },
        /* nGRMeshLut[52][98] */
        {
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
        },
        /* nGBMeshLut[52][98] */
        {
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
        },
        /* nBBMeshLut[52][98] */
        {
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
            {
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
            },
        },
    },
    /* tAutoParam */
    {
        /* nDampRatio */
        80,
        /* nToleranceRatio */
        10,
        /* tLumaParam */
        {
            /* nParamGrpNum */
            1,
            /* nRefValStart[16] */
            {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 15*/},
            /* nRefValEnd[16] */
            {1024, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 15*/},
            /* nLumaRatio[16] */
            {100, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 15*/},
            /* nLumaMeshLut[52][98] */
            {
                {
                    16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                    16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                },
                {
                    16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                    16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                },
                {
                    16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                    16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                },
                {
                    16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                    16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                },
                {
                    16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                    16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                },
                {
                    16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                    16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                },
                {
                    16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                    16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                },
                {
                    16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                    16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                },
                {
                    16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                    16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                },
                {
                    16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                    16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                },
                {
                    16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                    16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                },
                {
                    16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                    16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                },
                {
                    16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                    16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                },
                {
                    16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                    16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                },
                {
                    16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                    16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                },
                {
                    16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                    16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                },
                {
                    16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                    16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                },
                {
                    16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                    16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                },
                {
                    16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                    16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                },
                {
                    16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                    16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                },
                {
                    16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                    16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                },
                {
                    16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                    16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                },
                {
                    16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                    16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                },
                {
                    16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                    16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                },
                {
                    16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                    16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                },
                {
                    16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                    16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                },
                {
                    16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                    16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                },
                {
                    16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                    16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                },
                {
                    16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                    16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                },
                {
                    16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                    16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                },
                {
                    16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                    16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                },
                {
                    16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                    16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                },
                {
                    16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                    16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                },
                {
                    16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                    16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                },
                {
                    16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                    16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                },
                {
                    16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                    16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                },
                {
                    16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                    16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                },
                {
                    16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                    16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                },
                {
                    16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                    16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                },
                {
                    16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                    16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                },
                {
                    16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                    16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                },
                {
                    16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                    16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                },
                {
                    16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                    16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                },
                {
                    16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                    16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                },
                {
                    16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                    16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                },
                {
                    16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                    16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                },
                {
                    16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                    16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                },
                {
                    16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                    16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                },
                {
                    16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                    16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                },
                {
                    16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                    16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                },
            },
        },
        /* tColTempParam */
        {
            /* nColTempNum */
            1,
            /* nRefColorTempStart[10] */
            {3800, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 9*/},
            /* nRefColorTempEnd[10] */
            {6500, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 9*/},
            /* nColorTemp[10] */
            {4800, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 9*/},
            /* nColorRatio[10] */
            {100, /*0 - 0*/},
            /* nRRMeshLut[10][52][98] */
            {
                /* nRRMeshLut 0 */
                {
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                },
            },
            /* nGRMeshLut[10][52][98] */
            {
                /* nGRMeshLut 0 */
                {
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                },
            },
            /* nGBMeshLut[10][52][98] */
            {
                /* nGBMeshLut 0 */
                {
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                },
            },
            /* nBBMeshLut[10][52][98] */
            {
                /* nBBMeshLut 0 */
                {
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                    {
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384,  /* 0 - 31*/
                        16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*32 - 44*/
                    },
                },
            },
        },
    },
};

static AX_ISP_IQ_RLTM_PARAM_T rltm_param_sdr = {
    /* nRltmEn */
    1,
    /* nMultiCamSyncMode */
    0,
    /* nAutoMode */
    1,
    /* nRefMode */
    1,
    /* tTempoFilter */
    {
        /* nAlpha */
        512,
        /* nReset */
        0,
        /* nStopUpdating */
        0,
    },
    /* tLumaWt */
    {
        /* nLumaWeight[5] */
        {0, 0, 0, 0, 128, /*0 - 4*/},
    },
    /* tMultiHistWt */
    {
        /* nMode */
        0,
        /* nRegionNum */
        0,
        /* nHistWtNum */
        1,
        /* tRoi */
        {
            /* nTop */
            0,
            /* nBottom */
            0,
            /* nLeft */
            0,
            /* nRight */
            0,
        },
        /* tHistWt */
        {
            /* nHistogramWeight[63] */
            {
                1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
            },
        },
        /* nFlagHistId[16][16] */
        {
            {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 15*/},
            {0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 15*/},
            {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 15*/},
            {0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 15*/},
            {0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 15*/},
            {0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 15*/},
            {0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 15*/},
            {0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 15*/},
            {0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 15*/},
            {0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 15*/},
            {0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 15*/},
            {0, 1, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 15*/},
            {0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 15*/},
            {0, 1, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 15*/},
            {0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 15*/},
            {0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 15*/},
        },
    },
    /* tManualParam */
    {
        /* nHighlightSup */
        35,
        /* nKMax */
        60000,
        /* nPreGamma */
        32,
        /* nPostGamma */
        42,
        /* nDynamicRangeUpper */
        128,
        /* nDynamicRangeLower */
        0,
        /* nExtraDgain */
        16,
        /* nWinSize */
        512,
        /* nLog10Offset */
        64,
        /* nBaseGain */
        64,
        /* nDitherMode */
        0,
        /* nDitherScale */
        64,
        /* nGtmSwEn */
        0,
        /* nGtmSwDgain */
        256,
        /* nHistWtBrightLow[4] */
        {0, 0, 0, 0, /*0 - 3*/},
        /* nHistWtBrightHigh[4] */
        {0, 0, 0, 0, /*0 - 3*/},
        /* nHistWtThreshold[4] */
        {129, 129, 129, 129, /*0 - 3*/},
        /* nSCurveList[1025] */
        {
            0, 32, 64, 96, 128, 160, 192, 224, 256, 288, 320, 352, 384, 416, 448, 480, 512, 544, 576, 608, 640, 672, 704, 736, 768, 800, 832, 864, 896, 928, 960, 992,  /* 0 - 31*/
            1024, 1056, 1088, 1120, 1152, 1184, 1216, 1248, 1280, 1312, 1344, 1376, 1408, 1440, 1472, 1504, 1536, 1568, 1600, 1632, 1664, 1696, 1728, 1760, 1792, 1824, 1856, 1888, 1920, 1952, 1984, 2016,  /* 32 - 63*/
            2048, 2080, 2112, 2144, 2176, 2208, 2240, 2272, 2304, 2336, 2368, 2400, 2432, 2464, 2496, 2528, 2560, 2592, 2624, 2656, 2688, 2720, 2752, 2784, 2816, 2848, 2880, 2912, 2944, 2976, 3008, 3040,  /* 64 - 95*/
            3072, 3104, 3136, 3168, 3200, 3232, 3264, 3296, 3328, 3360, 3392, 3424, 3456, 3488, 3520, 3552, 3584, 3616, 3648, 3680, 3712, 3744, 3776, 3808, 3840, 3872, 3904, 3936, 3968, 4000, 4032, 4064,  /* 96 - 127*/
            4096, 4128, 4160, 4192, 4224, 4256, 4288, 4320, 4352, 4384, 4416, 4448, 4480, 4512, 4544, 4576, 4608, 4640, 4672, 4704, 4736, 4768, 4800, 4832, 4864, 4896, 4928, 4960, 4992, 5024, 5056, 5088,  /* 128 - 159*/
            5120, 5152, 5184, 5216, 5248, 5280, 5312, 5344, 5376, 5408, 5440, 5472, 5504, 5536, 5568, 5600, 5632, 5664, 5696, 5728, 5760, 5792, 5824, 5856, 5888, 5920, 5952, 5984, 6016, 6048, 6080, 6112,  /* 160 - 191*/
            6144, 6176, 6208, 6240, 6272, 6304, 6336, 6368, 6400, 6432, 6464, 6496, 6528, 6560, 6592, 6624, 6656, 6688, 6720, 6752, 6784, 6816, 6848, 6880, 6912, 6944, 6976, 7008, 7040, 7072, 7104, 7136,  /* 192 - 223*/
            7168, 7200, 7232, 7264, 7296, 7328, 7360, 7392, 7424, 7456, 7488, 7520, 7552, 7584, 7616, 7648, 7680, 7712, 7744, 7776, 7808, 7840, 7872, 7904, 7936, 7968, 8000, 8032, 8064, 8096, 8128, 8160,  /* 224 - 255*/
            8192, 8224, 8256, 8288, 8320, 8352, 8384, 8416, 8448, 8480, 8512, 8544, 8576, 8608, 8640, 8672, 8704, 8736, 8768, 8800, 8832, 8864, 8896, 8928, 8960, 8992, 9024, 9056, 9088, 9120, 9152, 9184,  /* 256 - 287*/
            9216, 9248, 9280, 9312, 9344, 9376, 9408, 9440, 9472, 9504, 9536, 9568, 9600, 9632, 9664, 9696, 9728, 9760, 9792, 9824, 9856, 9888, 9920, 9952, 9984, 10016, 10048, 10080, 10112, 10144, 10176, 10208,  /* 288 - 319*/
            10240, 10272, 10304, 10336, 10368, 10400, 10432, 10464, 10496, 10528, 10560, 10592, 10624, 10656, 10688, 10720, 10752, 10784, 10816, 10848, 10880, 10912, 10944, 10976, 11008, 11040, 11072, 11104, 11136, 11168, 11200, 11232,  /* 320 - 351*/
            11264, 11296, 11328, 11360, 11392, 11424, 11456, 11488, 11520, 11552, 11584, 11616, 11648, 11680, 11712, 11744, 11776, 11808, 11840, 11872, 11904, 11936, 11968, 12000, 12032, 12064, 12096, 12128, 12160, 12192, 12224, 12256,  /* 352 - 383*/
            12288, 12320, 12352, 12384, 12416, 12448, 12480, 12512, 12544, 12576, 12608, 12640, 12672, 12704, 12736, 12768, 12800, 12832, 12864, 12896, 12928, 12960, 12992, 13024, 13056, 13088, 13120, 13152, 13184, 13216, 13248, 13280,  /* 384 - 415*/
            13312, 13344, 13376, 13408, 13440, 13472, 13504, 13536, 13568, 13600, 13632, 13664, 13696, 13728, 13760, 13792, 13824, 13856, 13888, 13920, 13952, 13984, 14016, 14048, 14080, 14112, 14144, 14176, 14208, 14240, 14272, 14304,  /* 416 - 447*/
            14336, 14368, 14400, 14432, 14464, 14496, 14528, 14560, 14592, 14624, 14656, 14688, 14720, 14752, 14784, 14816, 14848, 14880, 14912, 14944, 14976, 15008, 15040, 15072, 15104, 15136, 15168, 15200, 15232, 15264, 15296, 15328,  /* 448 - 479*/
            15360, 15392, 15424, 15456, 15488, 15520, 15552, 15584, 15616, 15648, 15680, 15712, 15744, 15776, 15808, 15840, 15872, 15904, 15936, 15968, 16000, 16032, 16064, 16096, 16128, 16160, 16192, 16224, 16256, 16288, 16320, 16352,  /* 480 - 511*/
            16384, 16416, 16448, 16480, 16512, 16544, 16576, 16608, 16640, 16672, 16704, 16736, 16768, 16800, 16832, 16864, 16896, 16928, 16960, 16992, 17024, 17056, 17088, 17120, 17152, 17184, 17216, 17248, 17280, 17312, 17344, 17376,  /* 512 - 543*/
            17408, 17440, 17472, 17504, 17536, 17568, 17600, 17632, 17664, 17696, 17728, 17760, 17792, 17824, 17856, 17888, 17920, 17952, 17984, 18016, 18048, 18080, 18112, 18144, 18176, 18208, 18240, 18272, 18304, 18336, 18368, 18400,  /* 544 - 575*/
            18432, 18464, 18496, 18528, 18560, 18592, 18624, 18656, 18688, 18720, 18752, 18784, 18816, 18848, 18880, 18912, 18944, 18976, 19008, 19040, 19072, 19104, 19136, 19168, 19200, 19232, 19264, 19296, 19328, 19360, 19392, 19424,  /* 576 - 607*/
            19456, 19488, 19520, 19552, 19584, 19616, 19648, 19680, 19712, 19744, 19776, 19808, 19840, 19872, 19904, 19936, 19968, 20000, 20032, 20064, 20096, 20128, 20160, 20192, 20224, 20256, 20288, 20320, 20352, 20384, 20416, 20448,  /* 608 - 639*/
            20480, 20512, 20544, 20576, 20608, 20640, 20672, 20704, 20736, 20768, 20800, 20832, 20864, 20896, 20928, 20960, 20992, 21024, 21056, 21088, 21120, 21152, 21184, 21216, 21248, 21280, 21312, 21344, 21376, 21408, 21440, 21472,  /* 640 - 671*/
            21504, 21536, 21568, 21600, 21632, 21664, 21696, 21728, 21760, 21792, 21824, 21856, 21888, 21920, 21952, 21984, 22016, 22048, 22080, 22112, 22144, 22176, 22208, 22240, 22272, 22304, 22336, 22368, 22400, 22432, 22464, 22496,  /* 672 - 703*/
            22528, 22560, 22592, 22624, 22656, 22688, 22720, 22752, 22784, 22816, 22848, 22880, 22912, 22944, 22976, 23008, 23040, 23072, 23104, 23136, 23168, 23200, 23232, 23264, 23296, 23328, 23360, 23392, 23424, 23456, 23488, 23520,  /* 704 - 735*/
            23552, 23584, 23616, 23648, 23680, 23712, 23744, 23776, 23808, 23840, 23872, 23904, 23936, 23968, 24000, 24032, 24064, 24096, 24128, 24160, 24192, 24224, 24256, 24288, 24320, 24352, 24384, 24416, 24448, 24480, 24512, 24544,  /* 736 - 767*/
            24576, 24608, 24640, 24672, 24704, 24736, 24768, 24800, 24832, 24864, 24896, 24928, 24960, 24992, 25024, 25056, 25088, 25120, 25152, 25184, 25216, 25248, 25280, 25312, 25344, 25376, 25408, 25440, 25472, 25504, 25536, 25568,  /* 768 - 799*/
            25600, 25632, 25664, 25696, 25728, 25760, 25792, 25824, 25856, 25888, 25920, 25952, 25984, 26016, 26048, 26080, 26112, 26144, 26176, 26208, 26240, 26272, 26304, 26336, 26368, 26400, 26432, 26464, 26496, 26528, 26560, 26592,  /* 800 - 831*/
            26624, 26656, 26688, 26720, 26752, 26784, 26816, 26848, 26880, 26912, 26944, 26976, 27008, 27040, 27072, 27104, 27136, 27168, 27200, 27232, 27264, 27296, 27328, 27360, 27392, 27424, 27456, 27488, 27520, 27552, 27584, 27616,  /* 832 - 863*/
            27648, 27680, 27712, 27744, 27776, 27808, 27840, 27872, 27904, 27936, 27968, 28000, 28032, 28064, 28096, 28128, 28160, 28192, 28224, 28256, 28288, 28320, 28352, 28384, 28416, 28448, 28480, 28512, 28544, 28576, 28608, 28640,  /* 864 - 895*/
            28672, 28704, 28736, 28768, 28800, 28832, 28864, 28896, 28928, 28960, 28992, 29024, 29056, 29088, 29120, 29152, 29184, 29216, 29248, 29280, 29312, 29344, 29376, 29408, 29440, 29472, 29504, 29536, 29568, 29600, 29632, 29664,  /* 896 - 927*/
            29696, 29728, 29760, 29792, 29824, 29856, 29888, 29920, 29952, 29984, 30016, 30048, 30080, 30112, 30144, 30176, 30208, 30240, 30272, 30304, 30336, 30368, 30400, 30432, 30464, 30496, 30528, 30560, 30592, 30624, 30656, 30688,  /* 928 - 959*/
            30720, 30752, 30784, 30816, 30848, 30880, 30912, 30944, 30976, 31008, 31040, 31072, 31104, 31136, 31168, 31200, 31232, 31264, 31296, 31328, 31360, 31392, 31424, 31456, 31488, 31520, 31552, 31584, 31616, 31648, 31680, 31712,  /* 960 - 991*/
            31744, 31776, 31808, 31840, 31872, 31904, 31936, 31968, 32000, 32032, 32064, 32096, 32128, 32160, 32192, 32224, 32256, 32288, 32320, 32352, 32384, 32416, 32448, 32480, 32512, 32544, 32576, 32608, 32640, 32672, 32704, 32736,  /* 992 - 1023*/
            32768, /*1024 - 1024*/
        },
        /* tHistWt[16] */
        {
            /* 0 */
            {
                /* nHistogramWeight[63] */
                {
                    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                },
            },
            /* 1 */
            {
                /* nHistogramWeight[63] */
                {
                    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                },
            },
            /* 2 */
            {
                /* nHistogramWeight[63] */
                {
                    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                },
            },
            /* 3 */
            {
                /* nHistogramWeight[63] */
                {
                    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                },
            },
            /* 4 */
            {
                /* nHistogramWeight[63] */
                {
                    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                },
            },
            /* 5 */
            {
                /* nHistogramWeight[63] */
                {
                    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                },
            },
            /* 6 */
            {
                /* nHistogramWeight[63] */
                {
                    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                },
            },
            /* 7 */
            {
                /* nHistogramWeight[63] */
                {
                    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                },
            },
            /* 8 */
            {
                /* nHistogramWeight[63] */
                {
                    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                },
            },
            /* 9 */
            {
                /* nHistogramWeight[63] */
                {
                    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                },
            },
            /* 10 */
            {
                /* nHistogramWeight[63] */
                {
                    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                },
            },
            /* 11 */
            {
                /* nHistogramWeight[63] */
                {
                    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                },
            },
            /* 12 */
            {
                /* nHistogramWeight[63] */
                {
                    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                },
            },
            /* 13 */
            {
                /* nHistogramWeight[63] */
                {
                    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                },
            },
            /* 14 */
            {
                /* nHistogramWeight[63] */
                {
                    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                },
            },
            /* 15 */
            {
                /* nHistogramWeight[63] */
                {
                    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                },
            },
        },
        /* tHrParam */
        {
            /* nLocalFactor */
            80,
            /* nRltmStrength */
            16,
            /* nContrastStrength */
            30,
        },
    },
    /* tAutoParam */
    {
        /* nParamGrpNum */
        12,
        /* nRefVal[16] */
        {1024, 2048, 16384, 32768, 49152, 65536, 65537, 131072, 262144, 524288, 1048576, 2048000, /*0 - 11*/},
        /* nHighlightSup[16] */
        {42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, /*0 - 11*/},
        /* nKMax[16] */
        {60000, 60000, 60000, 60000, 60000, 60000, 60000, 60000, 60000, 60000, 60000, 60000, /*0 - 11*/},
        /* nPreGamma[16] */
        {32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, /*0 - 11*/},
        /* nPostGamma[16] */
        {42, 42, 42, 42, 42, 42, 42, 42, 42, 44, 48, 48, /*0 - 11*/},
        /* nDynamicRangeUpper[16] */
        {128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, /*0 - 11*/},
        /* nDynamicRangeLower[16] */
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 11*/},
        /* nExtraDgain[16] */
        {16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, /*0 - 11*/},
        /* nWinSize[16] */
        {512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, /*0 - 11*/},
        /* nLog10Offset[16] */
        {64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, /*0 - 11*/},
        /* nBaseGain[16] */
        {64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, /*0 - 11*/},
        /* nDitherMode[16] */
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 11*/},
        /* nDitherScale[16] */
        {64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, /*0 - 11*/},
        /* nGtmSwEn[16] */
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 11*/},
        /* nGtmSwDgain[16] */
        {256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, /*0 - 11*/},
        /* nHistWtBrightLow[16][4] */
        {
            {0, 0, 0, 0, /*0 - 3*/},
            {0, 0, 0, 0, /*0 - 3*/},
            {0, 0, 0, 0, /*0 - 3*/},
            {0, 0, 0, 0, /*0 - 3*/},
            {0, 0, 0, 0, /*0 - 3*/},
            {0, 0, 0, 0, /*0 - 3*/},
            {0, 0, 0, 0, /*0 - 3*/},
            {0, 0, 0, 0, /*0 - 3*/},
            {0, 0, 0, 0, /*0 - 3*/},
            {0, 0, 0, 0, /*0 - 3*/},
            {0, 0, 0, 0, /*0 - 3*/},
            {0, 0, 0, 0, /*0 - 3*/},
        },
        /* nHistWtBrightHigh[16][4] */
        {
            {0, 0, 0, 0, /*0 - 3*/},
            {0, 0, 0, 0, /*0 - 3*/},
            {0, 0, 0, 0, /*0 - 3*/},
            {0, 0, 0, 0, /*0 - 3*/},
            {0, 0, 0, 0, /*0 - 3*/},
            {0, 0, 0, 0, /*0 - 3*/},
            {0, 0, 0, 0, /*0 - 3*/},
            {0, 0, 0, 0, /*0 - 3*/},
            {0, 0, 0, 0, /*0 - 3*/},
            {0, 0, 0, 0, /*0 - 3*/},
            {0, 0, 0, 0, /*0 - 3*/},
            {0, 0, 0, 0, /*0 - 3*/},
        },
        /* nHistWtThreshold[16][4] */
        {
            {129, 129, 129, 129, /*0 - 3*/},
            {129, 129, 129, 129, /*0 - 3*/},
            {129, 129, 129, 129, /*0 - 3*/},
            {129, 129, 129, 129, /*0 - 3*/},
            {129, 129, 129, 129, /*0 - 3*/},
            {129, 129, 129, 129, /*0 - 3*/},
            {129, 129, 129, 129, /*0 - 3*/},
            {129, 129, 129, 129, /*0 - 3*/},
            {129, 129, 129, 129, /*0 - 3*/},
            {129, 129, 129, 129, /*0 - 3*/},
            {129, 129, 129, 129, /*0 - 3*/},
            {129, 129, 129, 129, /*0 - 3*/},
        },
        /* nSCurveList[16][1025] */
        {
            {
                0, 109, 190, 264, 332, 397, 460, 521, 580, 638, 694, 749, 804, 857, 910, 962, 1013, 1063, 1114, 1163, 1212, 1261, 1309, 1356, 1403, 1450, 1497, 1543, 1589, 1634, 1679, 1724,  /* 0 - 31*/
                1769, 1813, 1857, 1901, 1945, 1988, 2031, 2074, 2117, 2159, 2201, 2243, 2285, 2327, 2368, 2410, 2451, 2492, 2533, 2573, 2614, 2654, 2694, 2735, 2774, 2814, 2854, 2893, 2933, 2972, 3011, 3050,  /* 32 - 63*/
                3089, 3128, 3166, 3205, 3243, 3282, 3320, 3358, 3396, 3434, 3472, 3509, 3547, 3584, 3622, 3659, 3696, 3733, 3770, 3807, 3844, 3881, 3918, 3954, 3991, 4027, 4064, 4100, 4136, 4172, 4208, 4244,  /* 64 - 95*/
                4280, 4316, 4352, 4387, 4423, 4459, 4494, 4529, 4565, 4600, 4635, 4670, 4705, 4741, 4775, 4810, 4845, 4880, 4915, 4949, 4984, 5018, 5053, 5087, 5122, 5156, 5190, 5224, 5259, 5293, 5327, 5361,  /* 96 - 127*/
                5395, 5428, 5462, 5496, 5530, 5563, 5597, 5631, 5664, 5698, 5731, 5764, 5798, 5831, 5864, 5897, 5931, 5964, 5997, 6030, 6063, 6096, 6129, 6161, 6194, 6227, 6260, 6292, 6325, 6358, 6390, 6423,  /* 128 - 159*/
                6455, 6488, 6520, 6552, 6585, 6617, 6649, 6681, 6713, 6746, 6778, 6810, 6842, 6874, 6906, 6938, 6969, 7001, 7033, 7065, 7097, 7128, 7160, 7192, 7223, 7255, 7286, 7318, 7349, 7381, 7412, 7443,  /* 160 - 191*/
                7475, 7506, 7537, 7568, 7600, 7631, 7662, 7693, 7724, 7755, 7786, 7817, 7848, 7879, 7910, 7941, 7972, 8003, 8033, 8064, 8095, 8126, 8156, 8187, 8217, 8248, 8279, 8309, 8340, 8370, 8401, 8431,  /* 192 - 223*/
                8461, 8492, 8522, 8552, 8583, 8613, 8643, 8673, 8704, 8734, 8764, 8794, 8824, 8854, 8884, 8914, 8944, 8974, 9004, 9034, 9064, 9094, 9124, 9153, 9183, 9213, 9243, 9273, 9302, 9332, 9362, 9391,  /* 224 - 255*/
                9421, 9450, 9480, 9510, 9539, 9569, 9598, 9628, 9658, 9687, 9717, 9746, 9776, 9806, 9835, 9865, 9894, 9924, 9954, 9983, 10013, 10042, 10072, 10102, 10131, 10161, 10190, 10220, 10250, 10279, 10309, 10338,  /* 256 - 287*/
                10368, 10398, 10427, 10457, 10486, 10516, 10546, 10575, 10605, 10634, 10664, 10694, 10723, 10753, 10782, 10812, 10842, 10871, 10901, 10930, 10960, 10990, 11019, 11049, 11078, 11108, 11138, 11167, 11197, 11226, 11256, 11286,  /* 288 - 319*/
                11315, 11345, 11374, 11404, 11434, 11463, 11493, 11522, 11552, 11582, 11611, 11641, 11670, 11700, 11730, 11759, 11789, 11818, 11848, 11878, 11907, 11937, 11966, 11996, 12026, 12055, 12085, 12114, 12144, 12174, 12203, 12233,  /* 320 - 351*/
                12262, 12292, 12322, 12351, 12381, 12410, 12440, 12470, 12499, 12529, 12558, 12588, 12618, 12647, 12677, 12706, 12736, 12766, 12795, 12825, 12854, 12884, 12914, 12943, 12973, 13002, 13032, 13062, 13091, 13121, 13150, 13180,  /* 352 - 383*/
                13210, 13239, 13269, 13298, 13328, 13358, 13387, 13417, 13446, 13476, 13506, 13535, 13565, 13594, 13624, 13654, 13683, 13713, 13742, 13772, 13802, 13831, 13861, 13890, 13920, 13950, 13979, 14009, 14038, 14068, 14098, 14127,  /* 384 - 415*/
                14157, 14186, 14216, 14246, 14275, 14305, 14334, 14364, 14394, 14423, 14453, 14482, 14512, 14542, 14571, 14601, 14630, 14660, 14690, 14719, 14749, 14778, 14808, 14838, 14867, 14897, 14926, 14956, 14986, 15015, 15045, 15074,  /* 416 - 447*/
                15104, 15134, 15163, 15193, 15222, 15252, 15282, 15311, 15341, 15370, 15400, 15430, 15459, 15489, 15518, 15548, 15578, 15607, 15637, 15666, 15696, 15726, 15755, 15785, 15814, 15844, 15874, 15903, 15933, 15962, 15992, 16022,  /* 448 - 479*/
                16051, 16081, 16110, 16140, 16170, 16199, 16229, 16258, 16288, 16318, 16347, 16377, 16406, 16436, 16466, 16495, 16525, 16554, 16584, 16614, 16643, 16673, 16702, 16732, 16762, 16791, 16821, 16850, 16880, 16910, 16939, 16969,  /* 480 - 511*/
                16998, 17028, 17058, 17087, 17117, 17146, 17176, 17206, 17235, 17265, 17294, 17324, 17354, 17383, 17413, 17442, 17472, 17502, 17531, 17561, 17590, 17620, 17650, 17679, 17709, 17738, 17768, 17798, 17827, 17857, 17886, 17916,  /* 512 - 543*/
                17946, 17975, 18005, 18034, 18064, 18094, 18123, 18153, 18182, 18212, 18242, 18271, 18301, 18330, 18360, 18390, 18419, 18449, 18478, 18508, 18538, 18567, 18597, 18626, 18656, 18686, 18715, 18745, 18774, 18804, 18834, 18863,  /* 544 - 575*/
                18893, 18922, 18952, 18982, 19011, 19041, 19070, 19100, 19130, 19159, 19189, 19218, 19248, 19278, 19307, 19337, 19366, 19396, 19426, 19455, 19485, 19514, 19544, 19574, 19603, 19633, 19662, 19692, 19722, 19751, 19781, 19810,  /* 576 - 607*/
                19840, 19870, 19899, 19929, 19958, 19988, 20018, 20047, 20077, 20106, 20136, 20166, 20195, 20225, 20254, 20284, 20314, 20343, 20373, 20402, 20432, 20462, 20491, 20521, 20550, 20580, 20610, 20639, 20669, 20698, 20728, 20758,  /* 608 - 639*/
                20787, 20817, 20846, 20876, 20906, 20935, 20965, 20994, 21024, 21054, 21083, 21113, 21142, 21172, 21202, 21231, 21261, 21290, 21320, 21350, 21379, 21409, 21438, 21468, 21498, 21527, 21557, 21586, 21616, 21646, 21675, 21705,  /* 640 - 671*/
                21734, 21764, 21794, 21823, 21853, 21882, 21912, 21942, 21971, 22001, 22030, 22060, 22090, 22119, 22149, 22178, 22208, 22238, 22267, 22297, 22326, 22356, 22386, 22415, 22445, 22474, 22504, 22534, 22563, 22593, 22622, 22652,  /* 672 - 703*/
                22682, 22711, 22741, 22770, 22800, 22830, 22859, 22889, 22918, 22948, 22978, 23007, 23037, 23066, 23096, 23126, 23155, 23185, 23214, 23244, 23274, 23303, 23333, 23362, 23392, 23422, 23451, 23481, 23510, 23540, 23570, 23599,  /* 704 - 735*/
                23629, 23658, 23688, 23718, 23747, 23777, 23806, 23836, 23866, 23895, 23925, 23954, 23984, 24014, 24043, 24073, 24102, 24132, 24162, 24191, 24221, 24250, 24280, 24310, 24339, 24369, 24398, 24428, 24458, 24487, 24517, 24546,  /* 736 - 767*/
                24576, 24606, 24635, 24665, 24694, 24724, 24754, 24783, 24813, 24843, 24872, 24902, 24932, 24962, 24991, 25021, 25051, 25080, 25110, 25140, 25170, 25200, 25229, 25259, 25289, 25319, 25349, 25378, 25408, 25438, 25468, 25498,  /* 768 - 799*/
                25528, 25558, 25588, 25618, 25648, 25677, 25707, 25737, 25767, 25797, 25827, 25857, 25887, 25917, 25947, 25977, 26008, 26038, 26068, 26098, 26128, 26158, 26188, 26218, 26248, 26279, 26309, 26339, 26369, 26399, 26430, 26460,  /* 800 - 831*/
                26490, 26520, 26551, 26581, 26611, 26641, 26672, 26702, 26732, 26763, 26793, 26823, 26854, 26884, 26915, 26945, 26975, 27006, 27036, 27067, 27097, 27128, 27158, 27189, 27219, 27250, 27281, 27311, 27342, 27372, 27403, 27434,  /* 832 - 863*/
                27464, 27495, 27526, 27556, 27587, 27618, 27649, 27679, 27710, 27741, 27772, 27802, 27833, 27864, 27895, 27926, 27957, 27988, 28019, 28050, 28081, 28112, 28143, 28174, 28205, 28236, 28267, 28298, 28329, 28360, 28391, 28422,  /* 864 - 895*/
                28453, 28485, 28516, 28547, 28578, 28610, 28641, 28672, 28703, 28735, 28766, 28798, 28829, 28860, 28892, 28923, 28955, 28986, 29018, 29049, 29081, 29112, 29144, 29176, 29207, 29239, 29271, 29303, 29334, 29366, 29398, 29430,  /* 896 - 927*/
                29461, 29493, 29525, 29557, 29589, 29621, 29653, 29685, 29717, 29749, 29781, 29814, 29846, 29878, 29910, 29942, 29975, 30007, 30039, 30072, 30104, 30137, 30169, 30201, 30234, 30267, 30299, 30332, 30365, 30397, 30430, 30463,  /* 928 - 959*/
                30496, 30528, 30561, 30594, 30627, 30660, 30693, 30726, 30760, 30793, 30826, 30859, 30893, 30926, 30960, 30993, 31027, 31060, 31094, 31127, 31161, 31195, 31229, 31263, 31297, 31331, 31365, 31399, 31433, 31468, 31502, 31537,  /* 960 - 991*/
                31571, 31606, 31641, 31675, 31710, 31745, 31780, 31816, 31851, 31886, 31922, 31957, 31993, 32029, 32065, 32101, 32138, 32174, 32211, 32248, 32285, 32322, 32360, 32398, 32436, 32475, 32514, 32553, 32593, 32634, 32676, 32719,  /* 992 - 1023*/
                32768, /*1024 - 1024*/
            },
            {
                0, 109, 190, 264, 332, 397, 460, 521, 580, 638, 694, 749, 804, 857, 910, 962, 1013, 1063, 1114, 1163, 1212, 1261, 1309, 1356, 1403, 1450, 1497, 1543, 1589, 1634, 1679, 1724,  /* 0 - 31*/
                1769, 1813, 1857, 1901, 1945, 1988, 2031, 2074, 2117, 2159, 2201, 2243, 2285, 2327, 2368, 2410, 2451, 2492, 2533, 2573, 2614, 2654, 2694, 2735, 2774, 2814, 2854, 2893, 2933, 2972, 3011, 3050,  /* 32 - 63*/
                3089, 3128, 3166, 3205, 3243, 3282, 3320, 3358, 3396, 3434, 3472, 3509, 3547, 3584, 3622, 3659, 3696, 3733, 3770, 3807, 3844, 3881, 3918, 3954, 3991, 4027, 4064, 4100, 4136, 4172, 4208, 4244,  /* 64 - 95*/
                4280, 4316, 4352, 4387, 4423, 4459, 4494, 4529, 4565, 4600, 4635, 4670, 4705, 4741, 4775, 4810, 4845, 4880, 4915, 4949, 4984, 5018, 5053, 5087, 5122, 5156, 5190, 5224, 5259, 5293, 5327, 5361,  /* 96 - 127*/
                5395, 5428, 5462, 5496, 5530, 5563, 5597, 5631, 5664, 5698, 5731, 5764, 5798, 5831, 5864, 5897, 5931, 5964, 5997, 6030, 6063, 6096, 6129, 6161, 6194, 6227, 6260, 6292, 6325, 6358, 6390, 6423,  /* 128 - 159*/
                6455, 6488, 6520, 6552, 6585, 6617, 6649, 6681, 6713, 6746, 6778, 6810, 6842, 6874, 6906, 6938, 6969, 7001, 7033, 7065, 7097, 7128, 7160, 7192, 7223, 7255, 7286, 7318, 7349, 7381, 7412, 7443,  /* 160 - 191*/
                7475, 7506, 7537, 7568, 7600, 7631, 7662, 7693, 7724, 7755, 7786, 7817, 7848, 7879, 7910, 7941, 7972, 8003, 8033, 8064, 8095, 8126, 8156, 8187, 8217, 8248, 8279, 8309, 8340, 8370, 8401, 8431,  /* 192 - 223*/
                8461, 8492, 8522, 8552, 8583, 8613, 8643, 8673, 8704, 8734, 8764, 8794, 8824, 8854, 8884, 8914, 8944, 8974, 9004, 9034, 9064, 9094, 9124, 9153, 9183, 9213, 9243, 9273, 9302, 9332, 9362, 9391,  /* 224 - 255*/
                9421, 9450, 9480, 9510, 9539, 9569, 9598, 9628, 9658, 9687, 9717, 9746, 9776, 9806, 9835, 9865, 9894, 9924, 9954, 9983, 10013, 10042, 10072, 10102, 10131, 10161, 10190, 10220, 10250, 10279, 10309, 10338,  /* 256 - 287*/
                10368, 10398, 10427, 10457, 10486, 10516, 10546, 10575, 10605, 10634, 10664, 10694, 10723, 10753, 10782, 10812, 10842, 10871, 10901, 10930, 10960, 10990, 11019, 11049, 11078, 11108, 11138, 11167, 11197, 11226, 11256, 11286,  /* 288 - 319*/
                11315, 11345, 11374, 11404, 11434, 11463, 11493, 11522, 11552, 11582, 11611, 11641, 11670, 11700, 11730, 11759, 11789, 11818, 11848, 11878, 11907, 11937, 11966, 11996, 12026, 12055, 12085, 12114, 12144, 12174, 12203, 12233,  /* 320 - 351*/
                12262, 12292, 12322, 12351, 12381, 12410, 12440, 12470, 12499, 12529, 12558, 12588, 12618, 12647, 12677, 12706, 12736, 12766, 12795, 12825, 12854, 12884, 12914, 12943, 12973, 13002, 13032, 13062, 13091, 13121, 13150, 13180,  /* 352 - 383*/
                13210, 13239, 13269, 13298, 13328, 13358, 13387, 13417, 13446, 13476, 13506, 13535, 13565, 13594, 13624, 13654, 13683, 13713, 13742, 13772, 13802, 13831, 13861, 13890, 13920, 13950, 13979, 14009, 14038, 14068, 14098, 14127,  /* 384 - 415*/
                14157, 14186, 14216, 14246, 14275, 14305, 14334, 14364, 14394, 14423, 14453, 14482, 14512, 14542, 14571, 14601, 14630, 14660, 14690, 14719, 14749, 14778, 14808, 14838, 14867, 14897, 14926, 14956, 14986, 15015, 15045, 15074,  /* 416 - 447*/
                15104, 15134, 15163, 15193, 15222, 15252, 15282, 15311, 15341, 15370, 15400, 15430, 15459, 15489, 15518, 15548, 15578, 15607, 15637, 15666, 15696, 15726, 15755, 15785, 15814, 15844, 15874, 15903, 15933, 15962, 15992, 16022,  /* 448 - 479*/
                16051, 16081, 16110, 16140, 16170, 16199, 16229, 16258, 16288, 16318, 16347, 16377, 16406, 16436, 16466, 16495, 16525, 16554, 16584, 16614, 16643, 16673, 16702, 16732, 16762, 16791, 16821, 16850, 16880, 16910, 16939, 16969,  /* 480 - 511*/
                16998, 17028, 17058, 17087, 17117, 17146, 17176, 17206, 17235, 17265, 17294, 17324, 17354, 17383, 17413, 17442, 17472, 17502, 17531, 17561, 17590, 17620, 17650, 17679, 17709, 17738, 17768, 17798, 17827, 17857, 17886, 17916,  /* 512 - 543*/
                17946, 17975, 18005, 18034, 18064, 18094, 18123, 18153, 18182, 18212, 18242, 18271, 18301, 18330, 18360, 18390, 18419, 18449, 18478, 18508, 18538, 18567, 18597, 18626, 18656, 18686, 18715, 18745, 18774, 18804, 18834, 18863,  /* 544 - 575*/
                18893, 18922, 18952, 18982, 19011, 19041, 19070, 19100, 19130, 19159, 19189, 19218, 19248, 19278, 19307, 19337, 19366, 19396, 19426, 19455, 19485, 19514, 19544, 19574, 19603, 19633, 19662, 19692, 19722, 19751, 19781, 19810,  /* 576 - 607*/
                19840, 19870, 19899, 19929, 19958, 19988, 20018, 20047, 20077, 20106, 20136, 20166, 20195, 20225, 20254, 20284, 20314, 20343, 20373, 20402, 20432, 20462, 20491, 20521, 20550, 20580, 20610, 20639, 20669, 20698, 20728, 20758,  /* 608 - 639*/
                20787, 20817, 20846, 20876, 20906, 20935, 20965, 20994, 21024, 21054, 21083, 21113, 21142, 21172, 21202, 21231, 21261, 21290, 21320, 21350, 21379, 21409, 21438, 21468, 21498, 21527, 21557, 21586, 21616, 21646, 21675, 21705,  /* 640 - 671*/
                21734, 21764, 21794, 21823, 21853, 21882, 21912, 21942, 21971, 22001, 22030, 22060, 22090, 22119, 22149, 22178, 22208, 22238, 22267, 22297, 22326, 22356, 22386, 22415, 22445, 22474, 22504, 22534, 22563, 22593, 22622, 22652,  /* 672 - 703*/
                22682, 22711, 22741, 22770, 22800, 22830, 22859, 22889, 22918, 22948, 22978, 23007, 23037, 23066, 23096, 23126, 23155, 23185, 23214, 23244, 23274, 23303, 23333, 23362, 23392, 23422, 23451, 23481, 23510, 23540, 23570, 23599,  /* 704 - 735*/
                23629, 23658, 23688, 23718, 23747, 23777, 23806, 23836, 23866, 23895, 23925, 23954, 23984, 24014, 24043, 24073, 24102, 24132, 24162, 24191, 24221, 24250, 24280, 24310, 24339, 24369, 24398, 24428, 24458, 24487, 24517, 24546,  /* 736 - 767*/
                24576, 24606, 24635, 24665, 24694, 24724, 24754, 24783, 24813, 24843, 24872, 24902, 24932, 24962, 24991, 25021, 25051, 25080, 25110, 25140, 25170, 25200, 25229, 25259, 25289, 25319, 25349, 25378, 25408, 25438, 25468, 25498,  /* 768 - 799*/
                25528, 25558, 25588, 25618, 25648, 25677, 25707, 25737, 25767, 25797, 25827, 25857, 25887, 25917, 25947, 25977, 26008, 26038, 26068, 26098, 26128, 26158, 26188, 26218, 26248, 26279, 26309, 26339, 26369, 26399, 26430, 26460,  /* 800 - 831*/
                26490, 26520, 26551, 26581, 26611, 26641, 26672, 26702, 26732, 26763, 26793, 26823, 26854, 26884, 26915, 26945, 26975, 27006, 27036, 27067, 27097, 27128, 27158, 27189, 27219, 27250, 27281, 27311, 27342, 27372, 27403, 27434,  /* 832 - 863*/
                27464, 27495, 27526, 27556, 27587, 27618, 27649, 27679, 27710, 27741, 27772, 27802, 27833, 27864, 27895, 27926, 27957, 27988, 28019, 28050, 28081, 28112, 28143, 28174, 28205, 28236, 28267, 28298, 28329, 28360, 28391, 28422,  /* 864 - 895*/
                28453, 28485, 28516, 28547, 28578, 28610, 28641, 28672, 28703, 28735, 28766, 28798, 28829, 28860, 28892, 28923, 28955, 28986, 29018, 29049, 29081, 29112, 29144, 29176, 29207, 29239, 29271, 29303, 29334, 29366, 29398, 29430,  /* 896 - 927*/
                29461, 29493, 29525, 29557, 29589, 29621, 29653, 29685, 29717, 29749, 29781, 29814, 29846, 29878, 29910, 29942, 29975, 30007, 30039, 30072, 30104, 30137, 30169, 30201, 30234, 30267, 30299, 30332, 30365, 30397, 30430, 30463,  /* 928 - 959*/
                30496, 30528, 30561, 30594, 30627, 30660, 30693, 30726, 30760, 30793, 30826, 30859, 30893, 30926, 30960, 30993, 31027, 31060, 31094, 31127, 31161, 31195, 31229, 31263, 31297, 31331, 31365, 31399, 31433, 31468, 31502, 31537,  /* 960 - 991*/
                31571, 31606, 31641, 31675, 31710, 31745, 31780, 31816, 31851, 31886, 31922, 31957, 31993, 32029, 32065, 32101, 32138, 32174, 32211, 32248, 32285, 32322, 32360, 32398, 32436, 32475, 32514, 32553, 32593, 32634, 32676, 32719,  /* 992 - 1023*/
                32768, /*1024 - 1024*/
            },
            {
                0, 109, 190, 264, 332, 397, 460, 521, 580, 638, 694, 749, 804, 857, 910, 962, 1013, 1063, 1114, 1163, 1212, 1261, 1309, 1356, 1403, 1450, 1497, 1543, 1589, 1634, 1679, 1724,  /* 0 - 31*/
                1769, 1813, 1857, 1901, 1945, 1988, 2031, 2074, 2117, 2159, 2201, 2243, 2285, 2327, 2368, 2410, 2451, 2492, 2533, 2573, 2614, 2654, 2694, 2735, 2774, 2814, 2854, 2893, 2933, 2972, 3011, 3050,  /* 32 - 63*/
                3089, 3128, 3166, 3205, 3243, 3282, 3320, 3358, 3396, 3434, 3472, 3509, 3547, 3584, 3622, 3659, 3696, 3733, 3770, 3807, 3844, 3881, 3918, 3954, 3991, 4027, 4064, 4100, 4136, 4172, 4208, 4244,  /* 64 - 95*/
                4280, 4316, 4352, 4387, 4423, 4459, 4494, 4529, 4565, 4600, 4635, 4670, 4705, 4741, 4775, 4810, 4845, 4880, 4915, 4949, 4984, 5018, 5053, 5087, 5122, 5156, 5190, 5224, 5259, 5293, 5327, 5361,  /* 96 - 127*/
                5395, 5428, 5462, 5496, 5530, 5563, 5597, 5631, 5664, 5698, 5731, 5764, 5798, 5831, 5864, 5897, 5931, 5964, 5997, 6030, 6063, 6096, 6129, 6161, 6194, 6227, 6260, 6292, 6325, 6358, 6390, 6423,  /* 128 - 159*/
                6455, 6488, 6520, 6552, 6585, 6617, 6649, 6681, 6713, 6746, 6778, 6810, 6842, 6874, 6906, 6938, 6969, 7001, 7033, 7065, 7097, 7128, 7160, 7192, 7223, 7255, 7286, 7318, 7349, 7381, 7412, 7443,  /* 160 - 191*/
                7475, 7506, 7537, 7568, 7600, 7631, 7662, 7693, 7724, 7755, 7786, 7817, 7848, 7879, 7910, 7941, 7972, 8003, 8033, 8064, 8095, 8126, 8156, 8187, 8217, 8248, 8279, 8309, 8340, 8370, 8401, 8431,  /* 192 - 223*/
                8461, 8492, 8522, 8552, 8583, 8613, 8643, 8673, 8704, 8734, 8764, 8794, 8824, 8854, 8884, 8914, 8944, 8974, 9004, 9034, 9064, 9094, 9124, 9153, 9183, 9213, 9243, 9273, 9302, 9332, 9362, 9391,  /* 224 - 255*/
                9421, 9450, 9480, 9510, 9539, 9569, 9598, 9628, 9658, 9687, 9717, 9746, 9776, 9806, 9835, 9865, 9894, 9924, 9954, 9983, 10013, 10042, 10072, 10102, 10131, 10161, 10190, 10220, 10250, 10279, 10309, 10338,  /* 256 - 287*/
                10368, 10398, 10427, 10457, 10486, 10516, 10546, 10575, 10605, 10634, 10664, 10694, 10723, 10753, 10782, 10812, 10842, 10871, 10901, 10930, 10960, 10990, 11019, 11049, 11078, 11108, 11138, 11167, 11197, 11226, 11256, 11286,  /* 288 - 319*/
                11315, 11345, 11374, 11404, 11434, 11463, 11493, 11522, 11552, 11582, 11611, 11641, 11670, 11700, 11730, 11759, 11789, 11818, 11848, 11878, 11907, 11937, 11966, 11996, 12026, 12055, 12085, 12114, 12144, 12174, 12203, 12233,  /* 320 - 351*/
                12262, 12292, 12322, 12351, 12381, 12410, 12440, 12470, 12499, 12529, 12558, 12588, 12618, 12647, 12677, 12706, 12736, 12766, 12795, 12825, 12854, 12884, 12914, 12943, 12973, 13002, 13032, 13062, 13091, 13121, 13150, 13180,  /* 352 - 383*/
                13210, 13239, 13269, 13298, 13328, 13358, 13387, 13417, 13446, 13476, 13506, 13535, 13565, 13594, 13624, 13654, 13683, 13713, 13742, 13772, 13802, 13831, 13861, 13890, 13920, 13950, 13979, 14009, 14038, 14068, 14098, 14127,  /* 384 - 415*/
                14157, 14186, 14216, 14246, 14275, 14305, 14334, 14364, 14394, 14423, 14453, 14482, 14512, 14542, 14571, 14601, 14630, 14660, 14690, 14719, 14749, 14778, 14808, 14838, 14867, 14897, 14926, 14956, 14986, 15015, 15045, 15074,  /* 416 - 447*/
                15104, 15134, 15163, 15193, 15222, 15252, 15282, 15311, 15341, 15370, 15400, 15430, 15459, 15489, 15518, 15548, 15578, 15607, 15637, 15666, 15696, 15726, 15755, 15785, 15814, 15844, 15874, 15903, 15933, 15962, 15992, 16022,  /* 448 - 479*/
                16051, 16081, 16110, 16140, 16170, 16199, 16229, 16258, 16288, 16318, 16347, 16377, 16406, 16436, 16466, 16495, 16525, 16554, 16584, 16614, 16643, 16673, 16702, 16732, 16762, 16791, 16821, 16850, 16880, 16910, 16939, 16969,  /* 480 - 511*/
                16998, 17028, 17058, 17087, 17117, 17146, 17176, 17206, 17235, 17265, 17294, 17324, 17354, 17383, 17413, 17442, 17472, 17502, 17531, 17561, 17590, 17620, 17650, 17679, 17709, 17738, 17768, 17798, 17827, 17857, 17886, 17916,  /* 512 - 543*/
                17946, 17975, 18005, 18034, 18064, 18094, 18123, 18153, 18182, 18212, 18242, 18271, 18301, 18330, 18360, 18390, 18419, 18449, 18478, 18508, 18538, 18567, 18597, 18626, 18656, 18686, 18715, 18745, 18774, 18804, 18834, 18863,  /* 544 - 575*/
                18893, 18922, 18952, 18982, 19011, 19041, 19070, 19100, 19130, 19159, 19189, 19218, 19248, 19278, 19307, 19337, 19366, 19396, 19426, 19455, 19485, 19514, 19544, 19574, 19603, 19633, 19662, 19692, 19722, 19751, 19781, 19810,  /* 576 - 607*/
                19840, 19870, 19899, 19929, 19958, 19988, 20018, 20047, 20077, 20106, 20136, 20166, 20195, 20225, 20254, 20284, 20314, 20343, 20373, 20402, 20432, 20462, 20491, 20521, 20550, 20580, 20610, 20639, 20669, 20698, 20728, 20758,  /* 608 - 639*/
                20787, 20817, 20846, 20876, 20906, 20935, 20965, 20994, 21024, 21054, 21083, 21113, 21142, 21172, 21202, 21231, 21261, 21290, 21320, 21350, 21379, 21409, 21438, 21468, 21498, 21527, 21557, 21586, 21616, 21646, 21675, 21705,  /* 640 - 671*/
                21734, 21764, 21794, 21823, 21853, 21882, 21912, 21942, 21971, 22001, 22030, 22060, 22090, 22119, 22149, 22178, 22208, 22238, 22267, 22297, 22326, 22356, 22386, 22415, 22445, 22474, 22504, 22534, 22563, 22593, 22622, 22652,  /* 672 - 703*/
                22682, 22711, 22741, 22770, 22800, 22830, 22859, 22889, 22918, 22948, 22978, 23007, 23037, 23066, 23096, 23126, 23155, 23185, 23214, 23244, 23274, 23303, 23333, 23362, 23392, 23422, 23451, 23481, 23510, 23540, 23570, 23599,  /* 704 - 735*/
                23629, 23658, 23688, 23718, 23747, 23777, 23806, 23836, 23866, 23895, 23925, 23954, 23984, 24014, 24043, 24073, 24102, 24132, 24162, 24191, 24221, 24250, 24280, 24310, 24339, 24369, 24398, 24428, 24458, 24487, 24517, 24546,  /* 736 - 767*/
                24576, 24606, 24635, 24665, 24694, 24724, 24754, 24783, 24813, 24843, 24872, 24902, 24932, 24962, 24991, 25021, 25051, 25080, 25110, 25140, 25170, 25200, 25229, 25259, 25289, 25319, 25349, 25378, 25408, 25438, 25468, 25498,  /* 768 - 799*/
                25528, 25558, 25588, 25618, 25648, 25677, 25707, 25737, 25767, 25797, 25827, 25857, 25887, 25917, 25947, 25977, 26008, 26038, 26068, 26098, 26128, 26158, 26188, 26218, 26248, 26279, 26309, 26339, 26369, 26399, 26430, 26460,  /* 800 - 831*/
                26490, 26520, 26551, 26581, 26611, 26641, 26672, 26702, 26732, 26763, 26793, 26823, 26854, 26884, 26915, 26945, 26975, 27006, 27036, 27067, 27097, 27128, 27158, 27189, 27219, 27250, 27281, 27311, 27342, 27372, 27403, 27434,  /* 832 - 863*/
                27464, 27495, 27526, 27556, 27587, 27618, 27649, 27679, 27710, 27741, 27772, 27802, 27833, 27864, 27895, 27926, 27957, 27988, 28019, 28050, 28081, 28112, 28143, 28174, 28205, 28236, 28267, 28298, 28329, 28360, 28391, 28422,  /* 864 - 895*/
                28453, 28485, 28516, 28547, 28578, 28610, 28641, 28672, 28703, 28735, 28766, 28798, 28829, 28860, 28892, 28923, 28955, 28986, 29018, 29049, 29081, 29112, 29144, 29176, 29207, 29239, 29271, 29303, 29334, 29366, 29398, 29430,  /* 896 - 927*/
                29461, 29493, 29525, 29557, 29589, 29621, 29653, 29685, 29717, 29749, 29781, 29814, 29846, 29878, 29910, 29942, 29975, 30007, 30039, 30072, 30104, 30137, 30169, 30201, 30234, 30267, 30299, 30332, 30365, 30397, 30430, 30463,  /* 928 - 959*/
                30496, 30528, 30561, 30594, 30627, 30660, 30693, 30726, 30760, 30793, 30826, 30859, 30893, 30926, 30960, 30993, 31027, 31060, 31094, 31127, 31161, 31195, 31229, 31263, 31297, 31331, 31365, 31399, 31433, 31468, 31502, 31537,  /* 960 - 991*/
                31571, 31606, 31641, 31675, 31710, 31745, 31780, 31816, 31851, 31886, 31922, 31957, 31993, 32029, 32065, 32101, 32138, 32174, 32211, 32248, 32285, 32322, 32360, 32398, 32436, 32475, 32514, 32553, 32593, 32634, 32676, 32719,  /* 992 - 1023*/
                32768, /*1024 - 1024*/
            },
            {
                0, 109, 190, 264, 332, 397, 460, 521, 580, 638, 694, 749, 804, 857, 910, 962, 1013, 1063, 1114, 1163, 1212, 1261, 1309, 1356, 1403, 1450, 1497, 1543, 1589, 1634, 1679, 1724,  /* 0 - 31*/
                1769, 1813, 1857, 1901, 1945, 1988, 2031, 2074, 2117, 2159, 2201, 2243, 2285, 2327, 2368, 2410, 2451, 2492, 2533, 2573, 2614, 2654, 2694, 2735, 2774, 2814, 2854, 2893, 2933, 2972, 3011, 3050,  /* 32 - 63*/
                3089, 3128, 3166, 3205, 3243, 3282, 3320, 3358, 3396, 3434, 3472, 3509, 3547, 3584, 3622, 3659, 3696, 3733, 3770, 3807, 3844, 3881, 3918, 3954, 3991, 4027, 4064, 4100, 4136, 4172, 4208, 4244,  /* 64 - 95*/
                4280, 4316, 4352, 4387, 4423, 4459, 4494, 4529, 4565, 4600, 4635, 4670, 4705, 4741, 4775, 4810, 4845, 4880, 4915, 4949, 4984, 5018, 5053, 5087, 5122, 5156, 5190, 5224, 5259, 5293, 5327, 5361,  /* 96 - 127*/
                5395, 5428, 5462, 5496, 5530, 5563, 5597, 5631, 5664, 5698, 5731, 5764, 5798, 5831, 5864, 5897, 5931, 5964, 5997, 6030, 6063, 6096, 6129, 6161, 6194, 6227, 6260, 6292, 6325, 6358, 6390, 6423,  /* 128 - 159*/
                6455, 6488, 6520, 6552, 6585, 6617, 6649, 6681, 6713, 6746, 6778, 6810, 6842, 6874, 6906, 6938, 6969, 7001, 7033, 7065, 7097, 7128, 7160, 7192, 7223, 7255, 7286, 7318, 7349, 7381, 7412, 7443,  /* 160 - 191*/
                7475, 7506, 7537, 7568, 7600, 7631, 7662, 7693, 7724, 7755, 7786, 7817, 7848, 7879, 7910, 7941, 7972, 8003, 8033, 8064, 8095, 8126, 8156, 8187, 8217, 8248, 8279, 8309, 8340, 8370, 8401, 8431,  /* 192 - 223*/
                8461, 8492, 8522, 8552, 8583, 8613, 8643, 8673, 8704, 8734, 8764, 8794, 8824, 8854, 8884, 8914, 8944, 8974, 9004, 9034, 9064, 9094, 9124, 9153, 9183, 9213, 9243, 9273, 9302, 9332, 9362, 9391,  /* 224 - 255*/
                9421, 9450, 9480, 9510, 9539, 9569, 9598, 9628, 9658, 9687, 9717, 9746, 9776, 9806, 9835, 9865, 9894, 9924, 9954, 9983, 10013, 10042, 10072, 10102, 10131, 10161, 10190, 10220, 10250, 10279, 10309, 10338,  /* 256 - 287*/
                10368, 10398, 10427, 10457, 10486, 10516, 10546, 10575, 10605, 10634, 10664, 10694, 10723, 10753, 10782, 10812, 10842, 10871, 10901, 10930, 10960, 10990, 11019, 11049, 11078, 11108, 11138, 11167, 11197, 11226, 11256, 11286,  /* 288 - 319*/
                11315, 11345, 11374, 11404, 11434, 11463, 11493, 11522, 11552, 11582, 11611, 11641, 11670, 11700, 11730, 11759, 11789, 11818, 11848, 11878, 11907, 11937, 11966, 11996, 12026, 12055, 12085, 12114, 12144, 12174, 12203, 12233,  /* 320 - 351*/
                12262, 12292, 12322, 12351, 12381, 12410, 12440, 12470, 12499, 12529, 12558, 12588, 12618, 12647, 12677, 12706, 12736, 12766, 12795, 12825, 12854, 12884, 12914, 12943, 12973, 13002, 13032, 13062, 13091, 13121, 13150, 13180,  /* 352 - 383*/
                13210, 13239, 13269, 13298, 13328, 13358, 13387, 13417, 13446, 13476, 13506, 13535, 13565, 13594, 13624, 13654, 13683, 13713, 13742, 13772, 13802, 13831, 13861, 13890, 13920, 13950, 13979, 14009, 14038, 14068, 14098, 14127,  /* 384 - 415*/
                14157, 14186, 14216, 14246, 14275, 14305, 14334, 14364, 14394, 14423, 14453, 14482, 14512, 14542, 14571, 14601, 14630, 14660, 14690, 14719, 14749, 14778, 14808, 14838, 14867, 14897, 14926, 14956, 14986, 15015, 15045, 15074,  /* 416 - 447*/
                15104, 15134, 15163, 15193, 15222, 15252, 15282, 15311, 15341, 15370, 15400, 15430, 15459, 15489, 15518, 15548, 15578, 15607, 15637, 15666, 15696, 15726, 15755, 15785, 15814, 15844, 15874, 15903, 15933, 15962, 15992, 16022,  /* 448 - 479*/
                16051, 16081, 16110, 16140, 16170, 16199, 16229, 16258, 16288, 16318, 16347, 16377, 16406, 16436, 16466, 16495, 16525, 16554, 16584, 16614, 16643, 16673, 16702, 16732, 16762, 16791, 16821, 16850, 16880, 16910, 16939, 16969,  /* 480 - 511*/
                16998, 17028, 17058, 17087, 17117, 17146, 17176, 17206, 17235, 17265, 17294, 17324, 17354, 17383, 17413, 17442, 17472, 17502, 17531, 17561, 17590, 17620, 17650, 17679, 17709, 17738, 17768, 17798, 17827, 17857, 17886, 17916,  /* 512 - 543*/
                17946, 17975, 18005, 18034, 18064, 18094, 18123, 18153, 18182, 18212, 18242, 18271, 18301, 18330, 18360, 18390, 18419, 18449, 18478, 18508, 18538, 18567, 18597, 18626, 18656, 18686, 18715, 18745, 18774, 18804, 18834, 18863,  /* 544 - 575*/
                18893, 18922, 18952, 18982, 19011, 19041, 19070, 19100, 19130, 19159, 19189, 19218, 19248, 19278, 19307, 19337, 19366, 19396, 19426, 19455, 19485, 19514, 19544, 19574, 19603, 19633, 19662, 19692, 19722, 19751, 19781, 19810,  /* 576 - 607*/
                19840, 19870, 19899, 19929, 19958, 19988, 20018, 20047, 20077, 20106, 20136, 20166, 20195, 20225, 20254, 20284, 20314, 20343, 20373, 20402, 20432, 20462, 20491, 20521, 20550, 20580, 20610, 20639, 20669, 20698, 20728, 20758,  /* 608 - 639*/
                20787, 20817, 20846, 20876, 20906, 20935, 20965, 20994, 21024, 21054, 21083, 21113, 21142, 21172, 21202, 21231, 21261, 21290, 21320, 21350, 21379, 21409, 21438, 21468, 21498, 21527, 21557, 21586, 21616, 21646, 21675, 21705,  /* 640 - 671*/
                21734, 21764, 21794, 21823, 21853, 21882, 21912, 21942, 21971, 22001, 22030, 22060, 22090, 22119, 22149, 22178, 22208, 22238, 22267, 22297, 22326, 22356, 22386, 22415, 22445, 22474, 22504, 22534, 22563, 22593, 22622, 22652,  /* 672 - 703*/
                22682, 22711, 22741, 22770, 22800, 22830, 22859, 22889, 22918, 22948, 22978, 23007, 23037, 23066, 23096, 23126, 23155, 23185, 23214, 23244, 23274, 23303, 23333, 23362, 23392, 23422, 23451, 23481, 23510, 23540, 23570, 23599,  /* 704 - 735*/
                23629, 23658, 23688, 23718, 23747, 23777, 23806, 23836, 23866, 23895, 23925, 23954, 23984, 24014, 24043, 24073, 24102, 24132, 24162, 24191, 24221, 24250, 24280, 24310, 24339, 24369, 24398, 24428, 24458, 24487, 24517, 24546,  /* 736 - 767*/
                24576, 24606, 24635, 24665, 24694, 24724, 24754, 24783, 24813, 24843, 24872, 24902, 24932, 24962, 24991, 25021, 25051, 25080, 25110, 25140, 25170, 25200, 25229, 25259, 25289, 25319, 25349, 25378, 25408, 25438, 25468, 25498,  /* 768 - 799*/
                25528, 25558, 25588, 25618, 25648, 25677, 25707, 25737, 25767, 25797, 25827, 25857, 25887, 25917, 25947, 25977, 26008, 26038, 26068, 26098, 26128, 26158, 26188, 26218, 26248, 26279, 26309, 26339, 26369, 26399, 26430, 26460,  /* 800 - 831*/
                26490, 26520, 26551, 26581, 26611, 26641, 26672, 26702, 26732, 26763, 26793, 26823, 26854, 26884, 26915, 26945, 26975, 27006, 27036, 27067, 27097, 27128, 27158, 27189, 27219, 27250, 27281, 27311, 27342, 27372, 27403, 27434,  /* 832 - 863*/
                27464, 27495, 27526, 27556, 27587, 27618, 27649, 27679, 27710, 27741, 27772, 27802, 27833, 27864, 27895, 27926, 27957, 27988, 28019, 28050, 28081, 28112, 28143, 28174, 28205, 28236, 28267, 28298, 28329, 28360, 28391, 28422,  /* 864 - 895*/
                28453, 28485, 28516, 28547, 28578, 28610, 28641, 28672, 28703, 28735, 28766, 28798, 28829, 28860, 28892, 28923, 28955, 28986, 29018, 29049, 29081, 29112, 29144, 29176, 29207, 29239, 29271, 29303, 29334, 29366, 29398, 29430,  /* 896 - 927*/
                29461, 29493, 29525, 29557, 29589, 29621, 29653, 29685, 29717, 29749, 29781, 29814, 29846, 29878, 29910, 29942, 29975, 30007, 30039, 30072, 30104, 30137, 30169, 30201, 30234, 30267, 30299, 30332, 30365, 30397, 30430, 30463,  /* 928 - 959*/
                30496, 30528, 30561, 30594, 30627, 30660, 30693, 30726, 30760, 30793, 30826, 30859, 30893, 30926, 30960, 30993, 31027, 31060, 31094, 31127, 31161, 31195, 31229, 31263, 31297, 31331, 31365, 31399, 31433, 31468, 31502, 31537,  /* 960 - 991*/
                31571, 31606, 31641, 31675, 31710, 31745, 31780, 31816, 31851, 31886, 31922, 31957, 31993, 32029, 32065, 32101, 32138, 32174, 32211, 32248, 32285, 32322, 32360, 32398, 32436, 32475, 32514, 32553, 32593, 32634, 32676, 32719,  /* 992 - 1023*/
                32768, /*1024 - 1024*/
            },
            {
                0, 75, 136, 194, 248, 301, 352, 403, 452, 500, 548, 595, 641, 687, 732, 777, 822, 866, 910, 953, 997, 1040, 1082, 1125, 1167, 1209, 1250, 1292, 1333, 1374, 1415, 1455,  /* 0 - 31*/
                1496, 1536, 1576, 1616, 1656, 1695, 1735, 1774, 1814, 1853, 1892, 1930, 1969, 2008, 2046, 2085, 2123, 2161, 2199, 2237, 2275, 2313, 2350, 2388, 2425, 2463, 2500, 2537, 2574, 2611, 2648, 2685,  /* 32 - 63*/
                2722, 2758, 2795, 2831, 2868, 2904, 2941, 2977, 3013, 3049, 3085, 3121, 3157, 3193, 3229, 3264, 3300, 3336, 3371, 3407, 3442, 3477, 3513, 3548, 3583, 3618, 3653, 3688, 3723, 3758, 3793, 3828,  /* 64 - 95*/
                3863, 3898, 3932, 3967, 4001, 4036, 4070, 4105, 4139, 4174, 4208, 4242, 4276, 4311, 4345, 4379, 4413, 4447, 4481, 4515, 4549, 4582, 4616, 4650, 4684, 4717, 4751, 4785, 4818, 4852, 4885, 4919,  /* 96 - 127*/
                4952, 4986, 5019, 5052, 5086, 5119, 5152, 5185, 5218, 5252, 5285, 5318, 5351, 5384, 5417, 5450, 5483, 5515, 5548, 5581, 5614, 5647, 5679, 5712, 5745, 5777, 5810, 5842, 5875, 5907, 5940, 5972,  /* 128 - 159*/
                6005, 6037, 6070, 6102, 6134, 6167, 6199, 6231, 6263, 6295, 6328, 6360, 6392, 6424, 6456, 6488, 6520, 6552, 6584, 6616, 6648, 6680, 6711, 6743, 6775, 6807, 6839, 6870, 6902, 6934, 6966, 6997,  /* 160 - 191*/
                7029, 7060, 7092, 7124, 7155, 7187, 7218, 7250, 7281, 7312, 7344, 7375, 7407, 7438, 7469, 7501, 7532, 7563, 7594, 7626, 7657, 7688, 7719, 7750, 7781, 7813, 7844, 7875, 7906, 7937, 7968, 7999,  /* 192 - 223*/
                8030, 8061, 8092, 8122, 8153, 8184, 8215, 8246, 8277, 8308, 8338, 8369, 8400, 8431, 8461, 8492, 8523, 8553, 8584, 8615, 8645, 8676, 8706, 8737, 8767, 8798, 8829, 8859, 8889, 8920, 8950, 8981,  /* 224 - 255*/
                9011, 9042, 9072, 9102, 9133, 9163, 9194, 9224, 9254, 9285, 9315, 9346, 9376, 9406, 9437, 9467, 9498, 9528, 9558, 9589, 9619, 9650, 9680, 9710, 9741, 9771, 9802, 9832, 9862, 9893, 9923, 9954,  /* 256 - 287*/
                9984, 10014, 10045, 10075, 10106, 10136, 10166, 10197, 10227, 10258, 10288, 10318, 10349, 10379, 10410, 10440, 10470, 10501, 10531, 10562, 10592, 10622, 10653, 10683, 10714, 10744, 10774, 10805, 10835, 10866, 10896, 10926,  /* 288 - 319*/
                10957, 10987, 11018, 11048, 11078, 11109, 11139, 11170, 11200, 11230, 11261, 11291, 11322, 11352, 11382, 11413, 11443, 11474, 11504, 11534, 11565, 11595, 11626, 11656, 11686, 11717, 11747, 11778, 11808, 11838, 11869, 11899,  /* 320 - 351*/
                11930, 11960, 11990, 12021, 12051, 12082, 12112, 12142, 12173, 12203, 12234, 12264, 12294, 12325, 12355, 12386, 12416, 12446, 12477, 12507, 12538, 12568, 12598, 12629, 12659, 12690, 12720, 12750, 12781, 12811, 12842, 12872,  /* 352 - 383*/
                12902, 12933, 12963, 12994, 13024, 13054, 13085, 13115, 13146, 13176, 13206, 13237, 13267, 13298, 13328, 13358, 13389, 13419, 13450, 13480, 13510, 13541, 13571, 13602, 13632, 13662, 13693, 13723, 13754, 13784, 13814, 13845,  /* 384 - 415*/
                13875, 13906, 13936, 13966, 13997, 14027, 14058, 14088, 14118, 14149, 14179, 14210, 14240, 14270, 14301, 14331, 14362, 14392, 14422, 14453, 14483, 14514, 14544, 14574, 14605, 14635, 14666, 14696, 14726, 14757, 14787, 14818,  /* 416 - 447*/
                14848, 14878, 14909, 14939, 14970, 15000, 15030, 15061, 15091, 15122, 15152, 15182, 15213, 15243, 15274, 15304, 15334, 15365, 15395, 15426, 15456, 15486, 15517, 15547, 15578, 15608, 15638, 15669, 15699, 15730, 15760, 15790,  /* 448 - 479*/
                15821, 15851, 15882, 15912, 15942, 15973, 16003, 16034, 16064, 16094, 16125, 16155, 16186, 16216, 16246, 16277, 16307, 16338, 16368, 16398, 16429, 16459, 16490, 16520, 16550, 16581, 16611, 16642, 16672, 16702, 16733, 16763,  /* 480 - 511*/
                16794, 16824, 16854, 16885, 16915, 16946, 16976, 17006, 17037, 17067, 17098, 17128, 17158, 17189, 17219, 17250, 17280, 17310, 17341, 17371, 17402, 17432, 17462, 17493, 17523, 17554, 17584, 17614, 17645, 17675, 17706, 17736,  /* 512 - 543*/
                17766, 17797, 17827, 17858, 17888, 17918, 17949, 17979, 18010, 18040, 18070, 18101, 18131, 18162, 18192, 18222, 18253, 18283, 18314, 18344, 18374, 18405, 18435, 18466, 18496, 18526, 18557, 18587, 18618, 18648, 18678, 18709,  /* 544 - 575*/
                18739, 18770, 18800, 18830, 18861, 18891, 18922, 18952, 18982, 19013, 19043, 19074, 19104, 19134, 19165, 19195, 19226, 19256, 19286, 19317, 19347, 19378, 19408, 19438, 19469, 19499, 19530, 19560, 19590, 19621, 19651, 19682,  /* 576 - 607*/
                19712, 19742, 19773, 19803, 19834, 19864, 19894, 19925, 19955, 19986, 20016, 20046, 20077, 20107, 20138, 20168, 20198, 20229, 20259, 20290, 20320, 20350, 20381, 20411, 20442, 20472, 20502, 20533, 20563, 20594, 20624, 20654,  /* 608 - 639*/
                20685, 20715, 20746, 20776, 20806, 20837, 20867, 20898, 20928, 20958, 20989, 21019, 21050, 21080, 21110, 21141, 21171, 21202, 21232, 21262, 21293, 21323, 21354, 21384, 21414, 21445, 21475, 21506, 21536, 21566, 21597, 21627,  /* 640 - 671*/
                21658, 21688, 21718, 21749, 21779, 21810, 21840, 21870, 21901, 21931, 21962, 21992, 22022, 22053, 22083, 22114, 22144, 22174, 22205, 22235, 22266, 22296, 22326, 22357, 22387, 22418, 22448, 22478, 22509, 22539, 22570, 22600,  /* 672 - 703*/
                22630, 22661, 22691, 22722, 22752, 22782, 22813, 22843, 22874, 22904, 22934, 22965, 22995, 23026, 23056, 23086, 23117, 23147, 23178, 23208, 23238, 23269, 23299, 23330, 23360, 23390, 23421, 23451, 23482, 23512, 23542, 23573,  /* 704 - 735*/
                23603, 23634, 23664, 23694, 23725, 23755, 23786, 23816, 23846, 23877, 23907, 23938, 23968, 23998, 24029, 24059, 24090, 24120, 24150, 24181, 24211, 24242, 24272, 24302, 24333, 24363, 24394, 24424, 24454, 24485, 24515, 24546,  /* 736 - 767*/
                24576, 24606, 24637, 24667, 24698, 24728, 24759, 24789, 24819, 24850, 24880, 24911, 24941, 24972, 25002, 25033, 25063, 25094, 25124, 25155, 25185, 25216, 25246, 25277, 25307, 25338, 25368, 25399, 25430, 25460, 25491, 25521,  /* 768 - 799*/
                25552, 25583, 25613, 25644, 25674, 25705, 25736, 25766, 25797, 25828, 25858, 25889, 25920, 25950, 25981, 26012, 26043, 26073, 26104, 26135, 26165, 26196, 26227, 26258, 26289, 26319, 26350, 26381, 26412, 26443, 26473, 26504,  /* 800 - 831*/
                26535, 26566, 26597, 26628, 26658, 26689, 26720, 26751, 26782, 26813, 26844, 26875, 26906, 26937, 26968, 26999, 27029, 27060, 27091, 27122, 27153, 27184, 27216, 27247, 27278, 27309, 27340, 27371, 27402, 27433, 27464, 27495,  /* 832 - 863*/
                27526, 27557, 27589, 27620, 27651, 27682, 27713, 27744, 27776, 27807, 27838, 27869, 27900, 27932, 27963, 27994, 28026, 28057, 28088, 28119, 28151, 28182, 28213, 28245, 28276, 28308, 28339, 28370, 28402, 28433, 28465, 28496,  /* 864 - 895*/
                28528, 28559, 28591, 28622, 28654, 28685, 28717, 28748, 28780, 28811, 28843, 28875, 28906, 28938, 28969, 29001, 29033, 29064, 29096, 29128, 29160, 29191, 29223, 29255, 29287, 29318, 29350, 29382, 29414, 29446, 29478, 29510,  /* 896 - 927*/
                29542, 29574, 29605, 29637, 29669, 29701, 29733, 29766, 29798, 29830, 29862, 29894, 29926, 29958, 29990, 30022, 30055, 30087, 30119, 30151, 30184, 30216, 30248, 30281, 30313, 30346, 30378, 30410, 30443, 30475, 30508, 30540,  /* 928 - 959*/
                30573, 30606, 30638, 30671, 30704, 30736, 30769, 30802, 30835, 30867, 30900, 30933, 30966, 30999, 31032, 31065, 31098, 31131, 31164, 31197, 31230, 31264, 31297, 31330, 31364, 31397, 31430, 31464, 31497, 31531, 31564, 31598,  /* 960 - 991*/
                31632, 31666, 31699, 31733, 31767, 31801, 31835, 31869, 31904, 31938, 31972, 32006, 32041, 32076, 32110, 32145, 32180, 32215, 32250, 32285, 32321, 32356, 32392, 32428, 32464, 32500, 32536, 32573, 32610, 32648, 32686, 32726,  /* 992 - 1023*/
                32768, /*1024 - 1024*/
            },
            {
                0, 75, 136, 194, 248, 301, 352, 403, 452, 500, 548, 595, 641, 687, 732, 777, 822, 866, 910, 953, 997, 1040, 1082, 1125, 1167, 1209, 1250, 1292, 1333, 1374, 1415, 1455,  /* 0 - 31*/
                1496, 1536, 1576, 1616, 1656, 1695, 1735, 1774, 1814, 1853, 1892, 1930, 1969, 2008, 2046, 2085, 2123, 2161, 2199, 2237, 2275, 2313, 2350, 2388, 2425, 2463, 2500, 2537, 2574, 2611, 2648, 2685,  /* 32 - 63*/
                2722, 2758, 2795, 2831, 2868, 2904, 2941, 2977, 3013, 3049, 3085, 3121, 3157, 3193, 3229, 3264, 3300, 3336, 3371, 3407, 3442, 3477, 3513, 3548, 3583, 3618, 3653, 3688, 3723, 3758, 3793, 3828,  /* 64 - 95*/
                3863, 3898, 3932, 3967, 4001, 4036, 4070, 4105, 4139, 4174, 4208, 4242, 4276, 4311, 4345, 4379, 4413, 4447, 4481, 4515, 4549, 4582, 4616, 4650, 4684, 4717, 4751, 4785, 4818, 4852, 4885, 4919,  /* 96 - 127*/
                4952, 4986, 5019, 5052, 5086, 5119, 5152, 5185, 5218, 5252, 5285, 5318, 5351, 5384, 5417, 5450, 5483, 5515, 5548, 5581, 5614, 5647, 5679, 5712, 5745, 5777, 5810, 5842, 5875, 5907, 5940, 5972,  /* 128 - 159*/
                6005, 6037, 6070, 6102, 6134, 6167, 6199, 6231, 6263, 6295, 6328, 6360, 6392, 6424, 6456, 6488, 6520, 6552, 6584, 6616, 6648, 6680, 6711, 6743, 6775, 6807, 6839, 6870, 6902, 6934, 6966, 6997,  /* 160 - 191*/
                7029, 7060, 7092, 7124, 7155, 7187, 7218, 7250, 7281, 7312, 7344, 7375, 7407, 7438, 7469, 7501, 7532, 7563, 7594, 7626, 7657, 7688, 7719, 7750, 7781, 7813, 7844, 7875, 7906, 7937, 7968, 7999,  /* 192 - 223*/
                8030, 8061, 8092, 8122, 8153, 8184, 8215, 8246, 8277, 8308, 8338, 8369, 8400, 8431, 8461, 8492, 8523, 8553, 8584, 8615, 8645, 8676, 8706, 8737, 8767, 8798, 8829, 8859, 8889, 8920, 8950, 8981,  /* 224 - 255*/
                9011, 9042, 9072, 9102, 9133, 9163, 9194, 9224, 9254, 9285, 9315, 9346, 9376, 9406, 9437, 9467, 9498, 9528, 9558, 9589, 9619, 9650, 9680, 9710, 9741, 9771, 9802, 9832, 9862, 9893, 9923, 9954,  /* 256 - 287*/
                9984, 10014, 10045, 10075, 10106, 10136, 10166, 10197, 10227, 10258, 10288, 10318, 10349, 10379, 10410, 10440, 10470, 10501, 10531, 10562, 10592, 10622, 10653, 10683, 10714, 10744, 10774, 10805, 10835, 10866, 10896, 10926,  /* 288 - 319*/
                10957, 10987, 11018, 11048, 11078, 11109, 11139, 11170, 11200, 11230, 11261, 11291, 11322, 11352, 11382, 11413, 11443, 11474, 11504, 11534, 11565, 11595, 11626, 11656, 11686, 11717, 11747, 11778, 11808, 11838, 11869, 11899,  /* 320 - 351*/
                11930, 11960, 11990, 12021, 12051, 12082, 12112, 12142, 12173, 12203, 12234, 12264, 12294, 12325, 12355, 12386, 12416, 12446, 12477, 12507, 12538, 12568, 12598, 12629, 12659, 12690, 12720, 12750, 12781, 12811, 12842, 12872,  /* 352 - 383*/
                12902, 12933, 12963, 12994, 13024, 13054, 13085, 13115, 13146, 13176, 13206, 13237, 13267, 13298, 13328, 13358, 13389, 13419, 13450, 13480, 13510, 13541, 13571, 13602, 13632, 13662, 13693, 13723, 13754, 13784, 13814, 13845,  /* 384 - 415*/
                13875, 13906, 13936, 13966, 13997, 14027, 14058, 14088, 14118, 14149, 14179, 14210, 14240, 14270, 14301, 14331, 14362, 14392, 14422, 14453, 14483, 14514, 14544, 14574, 14605, 14635, 14666, 14696, 14726, 14757, 14787, 14818,  /* 416 - 447*/
                14848, 14878, 14909, 14939, 14970, 15000, 15030, 15061, 15091, 15122, 15152, 15182, 15213, 15243, 15274, 15304, 15334, 15365, 15395, 15426, 15456, 15486, 15517, 15547, 15578, 15608, 15638, 15669, 15699, 15730, 15760, 15790,  /* 448 - 479*/
                15821, 15851, 15882, 15912, 15942, 15973, 16003, 16034, 16064, 16094, 16125, 16155, 16186, 16216, 16246, 16277, 16307, 16338, 16368, 16398, 16429, 16459, 16490, 16520, 16550, 16581, 16611, 16642, 16672, 16702, 16733, 16763,  /* 480 - 511*/
                16794, 16824, 16854, 16885, 16915, 16946, 16976, 17006, 17037, 17067, 17098, 17128, 17158, 17189, 17219, 17250, 17280, 17310, 17341, 17371, 17402, 17432, 17462, 17493, 17523, 17554, 17584, 17614, 17645, 17675, 17706, 17736,  /* 512 - 543*/
                17766, 17797, 17827, 17858, 17888, 17918, 17949, 17979, 18010, 18040, 18070, 18101, 18131, 18162, 18192, 18222, 18253, 18283, 18314, 18344, 18374, 18405, 18435, 18466, 18496, 18526, 18557, 18587, 18618, 18648, 18678, 18709,  /* 544 - 575*/
                18739, 18770, 18800, 18830, 18861, 18891, 18922, 18952, 18982, 19013, 19043, 19074, 19104, 19134, 19165, 19195, 19226, 19256, 19286, 19317, 19347, 19378, 19408, 19438, 19469, 19499, 19530, 19560, 19590, 19621, 19651, 19682,  /* 576 - 607*/
                19712, 19742, 19773, 19803, 19834, 19864, 19894, 19925, 19955, 19986, 20016, 20046, 20077, 20107, 20138, 20168, 20198, 20229, 20259, 20290, 20320, 20350, 20381, 20411, 20442, 20472, 20502, 20533, 20563, 20594, 20624, 20654,  /* 608 - 639*/
                20685, 20715, 20746, 20776, 20806, 20837, 20867, 20898, 20928, 20958, 20989, 21019, 21050, 21080, 21110, 21141, 21171, 21202, 21232, 21262, 21293, 21323, 21354, 21384, 21414, 21445, 21475, 21506, 21536, 21566, 21597, 21627,  /* 640 - 671*/
                21658, 21688, 21718, 21749, 21779, 21810, 21840, 21870, 21901, 21931, 21962, 21992, 22022, 22053, 22083, 22114, 22144, 22174, 22205, 22235, 22266, 22296, 22326, 22357, 22387, 22418, 22448, 22478, 22509, 22539, 22570, 22600,  /* 672 - 703*/
                22630, 22661, 22691, 22722, 22752, 22782, 22813, 22843, 22874, 22904, 22934, 22965, 22995, 23026, 23056, 23086, 23117, 23147, 23178, 23208, 23238, 23269, 23299, 23330, 23360, 23390, 23421, 23451, 23482, 23512, 23542, 23573,  /* 704 - 735*/
                23603, 23634, 23664, 23694, 23725, 23755, 23786, 23816, 23846, 23877, 23907, 23938, 23968, 23998, 24029, 24059, 24090, 24120, 24150, 24181, 24211, 24242, 24272, 24302, 24333, 24363, 24394, 24424, 24454, 24485, 24515, 24546,  /* 736 - 767*/
                24576, 24606, 24637, 24667, 24698, 24728, 24759, 24789, 24819, 24850, 24880, 24911, 24941, 24972, 25002, 25033, 25063, 25094, 25124, 25155, 25185, 25216, 25246, 25277, 25307, 25338, 25368, 25399, 25430, 25460, 25491, 25521,  /* 768 - 799*/
                25552, 25583, 25613, 25644, 25674, 25705, 25736, 25766, 25797, 25828, 25858, 25889, 25920, 25950, 25981, 26012, 26043, 26073, 26104, 26135, 26165, 26196, 26227, 26258, 26289, 26319, 26350, 26381, 26412, 26443, 26473, 26504,  /* 800 - 831*/
                26535, 26566, 26597, 26628, 26658, 26689, 26720, 26751, 26782, 26813, 26844, 26875, 26906, 26937, 26968, 26999, 27029, 27060, 27091, 27122, 27153, 27184, 27216, 27247, 27278, 27309, 27340, 27371, 27402, 27433, 27464, 27495,  /* 832 - 863*/
                27526, 27557, 27589, 27620, 27651, 27682, 27713, 27744, 27776, 27807, 27838, 27869, 27900, 27932, 27963, 27994, 28026, 28057, 28088, 28119, 28151, 28182, 28213, 28245, 28276, 28308, 28339, 28370, 28402, 28433, 28465, 28496,  /* 864 - 895*/
                28528, 28559, 28591, 28622, 28654, 28685, 28717, 28748, 28780, 28811, 28843, 28875, 28906, 28938, 28969, 29001, 29033, 29064, 29096, 29128, 29160, 29191, 29223, 29255, 29287, 29318, 29350, 29382, 29414, 29446, 29478, 29510,  /* 896 - 927*/
                29542, 29574, 29605, 29637, 29669, 29701, 29733, 29766, 29798, 29830, 29862, 29894, 29926, 29958, 29990, 30022, 30055, 30087, 30119, 30151, 30184, 30216, 30248, 30281, 30313, 30346, 30378, 30410, 30443, 30475, 30508, 30540,  /* 928 - 959*/
                30573, 30606, 30638, 30671, 30704, 30736, 30769, 30802, 30835, 30867, 30900, 30933, 30966, 30999, 31032, 31065, 31098, 31131, 31164, 31197, 31230, 31264, 31297, 31330, 31364, 31397, 31430, 31464, 31497, 31531, 31564, 31598,  /* 960 - 991*/
                31632, 31666, 31699, 31733, 31767, 31801, 31835, 31869, 31904, 31938, 31972, 32006, 32041, 32076, 32110, 32145, 32180, 32215, 32250, 32285, 32321, 32356, 32392, 32428, 32464, 32500, 32536, 32573, 32610, 32648, 32686, 32726,  /* 992 - 1023*/
                32768, /*1024 - 1024*/
            },
            {
                0, 75, 136, 194, 248, 301, 352, 403, 452, 500, 548, 595, 641, 687, 732, 777, 822, 866, 910, 953, 997, 1040, 1082, 1125, 1167, 1209, 1250, 1292, 1333, 1374, 1415, 1455,  /* 0 - 31*/
                1496, 1536, 1576, 1616, 1656, 1695, 1735, 1774, 1814, 1853, 1892, 1930, 1969, 2008, 2046, 2085, 2123, 2161, 2199, 2237, 2275, 2313, 2350, 2388, 2425, 2463, 2500, 2537, 2574, 2611, 2648, 2685,  /* 32 - 63*/
                2722, 2758, 2795, 2831, 2868, 2904, 2941, 2977, 3013, 3049, 3085, 3121, 3157, 3193, 3229, 3264, 3300, 3336, 3371, 3407, 3442, 3477, 3513, 3548, 3583, 3618, 3653, 3688, 3723, 3758, 3793, 3828,  /* 64 - 95*/
                3863, 3898, 3932, 3967, 4001, 4036, 4070, 4105, 4139, 4174, 4208, 4242, 4276, 4311, 4345, 4379, 4413, 4447, 4481, 4515, 4549, 4582, 4616, 4650, 4684, 4717, 4751, 4785, 4818, 4852, 4885, 4919,  /* 96 - 127*/
                4952, 4986, 5019, 5052, 5086, 5119, 5152, 5185, 5218, 5252, 5285, 5318, 5351, 5384, 5417, 5450, 5483, 5515, 5548, 5581, 5614, 5647, 5679, 5712, 5745, 5777, 5810, 5842, 5875, 5907, 5940, 5972,  /* 128 - 159*/
                6005, 6037, 6070, 6102, 6134, 6167, 6199, 6231, 6263, 6295, 6328, 6360, 6392, 6424, 6456, 6488, 6520, 6552, 6584, 6616, 6648, 6680, 6711, 6743, 6775, 6807, 6839, 6870, 6902, 6934, 6966, 6997,  /* 160 - 191*/
                7029, 7060, 7092, 7124, 7155, 7187, 7218, 7250, 7281, 7312, 7344, 7375, 7407, 7438, 7469, 7501, 7532, 7563, 7594, 7626, 7657, 7688, 7719, 7750, 7781, 7813, 7844, 7875, 7906, 7937, 7968, 7999,  /* 192 - 223*/
                8030, 8061, 8092, 8122, 8153, 8184, 8215, 8246, 8277, 8308, 8338, 8369, 8400, 8431, 8461, 8492, 8523, 8553, 8584, 8615, 8645, 8676, 8706, 8737, 8767, 8798, 8829, 8859, 8889, 8920, 8950, 8981,  /* 224 - 255*/
                9011, 9042, 9072, 9102, 9133, 9163, 9194, 9224, 9254, 9285, 9315, 9346, 9376, 9406, 9437, 9467, 9498, 9528, 9558, 9589, 9619, 9650, 9680, 9710, 9741, 9771, 9802, 9832, 9862, 9893, 9923, 9954,  /* 256 - 287*/
                9984, 10014, 10045, 10075, 10106, 10136, 10166, 10197, 10227, 10258, 10288, 10318, 10349, 10379, 10410, 10440, 10470, 10501, 10531, 10562, 10592, 10622, 10653, 10683, 10714, 10744, 10774, 10805, 10835, 10866, 10896, 10926,  /* 288 - 319*/
                10957, 10987, 11018, 11048, 11078, 11109, 11139, 11170, 11200, 11230, 11261, 11291, 11322, 11352, 11382, 11413, 11443, 11474, 11504, 11534, 11565, 11595, 11626, 11656, 11686, 11717, 11747, 11778, 11808, 11838, 11869, 11899,  /* 320 - 351*/
                11930, 11960, 11990, 12021, 12051, 12082, 12112, 12142, 12173, 12203, 12234, 12264, 12294, 12325, 12355, 12386, 12416, 12446, 12477, 12507, 12538, 12568, 12598, 12629, 12659, 12690, 12720, 12750, 12781, 12811, 12842, 12872,  /* 352 - 383*/
                12902, 12933, 12963, 12994, 13024, 13054, 13085, 13115, 13146, 13176, 13206, 13237, 13267, 13298, 13328, 13358, 13389, 13419, 13450, 13480, 13510, 13541, 13571, 13602, 13632, 13662, 13693, 13723, 13754, 13784, 13814, 13845,  /* 384 - 415*/
                13875, 13906, 13936, 13966, 13997, 14027, 14058, 14088, 14118, 14149, 14179, 14210, 14240, 14270, 14301, 14331, 14362, 14392, 14422, 14453, 14483, 14514, 14544, 14574, 14605, 14635, 14666, 14696, 14726, 14757, 14787, 14818,  /* 416 - 447*/
                14848, 14878, 14909, 14939, 14970, 15000, 15030, 15061, 15091, 15122, 15152, 15182, 15213, 15243, 15274, 15304, 15334, 15365, 15395, 15426, 15456, 15486, 15517, 15547, 15578, 15608, 15638, 15669, 15699, 15730, 15760, 15790,  /* 448 - 479*/
                15821, 15851, 15882, 15912, 15942, 15973, 16003, 16034, 16064, 16094, 16125, 16155, 16186, 16216, 16246, 16277, 16307, 16338, 16368, 16398, 16429, 16459, 16490, 16520, 16550, 16581, 16611, 16642, 16672, 16702, 16733, 16763,  /* 480 - 511*/
                16794, 16824, 16854, 16885, 16915, 16946, 16976, 17006, 17037, 17067, 17098, 17128, 17158, 17189, 17219, 17250, 17280, 17310, 17341, 17371, 17402, 17432, 17462, 17493, 17523, 17554, 17584, 17614, 17645, 17675, 17706, 17736,  /* 512 - 543*/
                17766, 17797, 17827, 17858, 17888, 17918, 17949, 17979, 18010, 18040, 18070, 18101, 18131, 18162, 18192, 18222, 18253, 18283, 18314, 18344, 18374, 18405, 18435, 18466, 18496, 18526, 18557, 18587, 18618, 18648, 18678, 18709,  /* 544 - 575*/
                18739, 18770, 18800, 18830, 18861, 18891, 18922, 18952, 18982, 19013, 19043, 19074, 19104, 19134, 19165, 19195, 19226, 19256, 19286, 19317, 19347, 19378, 19408, 19438, 19469, 19499, 19530, 19560, 19590, 19621, 19651, 19682,  /* 576 - 607*/
                19712, 19742, 19773, 19803, 19834, 19864, 19894, 19925, 19955, 19986, 20016, 20046, 20077, 20107, 20138, 20168, 20198, 20229, 20259, 20290, 20320, 20350, 20381, 20411, 20442, 20472, 20502, 20533, 20563, 20594, 20624, 20654,  /* 608 - 639*/
                20685, 20715, 20746, 20776, 20806, 20837, 20867, 20898, 20928, 20958, 20989, 21019, 21050, 21080, 21110, 21141, 21171, 21202, 21232, 21262, 21293, 21323, 21354, 21384, 21414, 21445, 21475, 21506, 21536, 21566, 21597, 21627,  /* 640 - 671*/
                21658, 21688, 21718, 21749, 21779, 21810, 21840, 21870, 21901, 21931, 21962, 21992, 22022, 22053, 22083, 22114, 22144, 22174, 22205, 22235, 22266, 22296, 22326, 22357, 22387, 22418, 22448, 22478, 22509, 22539, 22570, 22600,  /* 672 - 703*/
                22630, 22661, 22691, 22722, 22752, 22782, 22813, 22843, 22874, 22904, 22934, 22965, 22995, 23026, 23056, 23086, 23117, 23147, 23178, 23208, 23238, 23269, 23299, 23330, 23360, 23390, 23421, 23451, 23482, 23512, 23542, 23573,  /* 704 - 735*/
                23603, 23634, 23664, 23694, 23725, 23755, 23786, 23816, 23846, 23877, 23907, 23938, 23968, 23998, 24029, 24059, 24090, 24120, 24150, 24181, 24211, 24242, 24272, 24302, 24333, 24363, 24394, 24424, 24454, 24485, 24515, 24546,  /* 736 - 767*/
                24576, 24606, 24637, 24667, 24698, 24728, 24759, 24789, 24819, 24850, 24880, 24911, 24941, 24972, 25002, 25033, 25063, 25094, 25124, 25155, 25185, 25216, 25246, 25277, 25307, 25338, 25368, 25399, 25430, 25460, 25491, 25521,  /* 768 - 799*/
                25552, 25583, 25613, 25644, 25674, 25705, 25736, 25766, 25797, 25828, 25858, 25889, 25920, 25950, 25981, 26012, 26043, 26073, 26104, 26135, 26165, 26196, 26227, 26258, 26289, 26319, 26350, 26381, 26412, 26443, 26473, 26504,  /* 800 - 831*/
                26535, 26566, 26597, 26628, 26658, 26689, 26720, 26751, 26782, 26813, 26844, 26875, 26906, 26937, 26968, 26999, 27029, 27060, 27091, 27122, 27153, 27184, 27216, 27247, 27278, 27309, 27340, 27371, 27402, 27433, 27464, 27495,  /* 832 - 863*/
                27526, 27557, 27589, 27620, 27651, 27682, 27713, 27744, 27776, 27807, 27838, 27869, 27900, 27932, 27963, 27994, 28026, 28057, 28088, 28119, 28151, 28182, 28213, 28245, 28276, 28308, 28339, 28370, 28402, 28433, 28465, 28496,  /* 864 - 895*/
                28528, 28559, 28591, 28622, 28654, 28685, 28717, 28748, 28780, 28811, 28843, 28875, 28906, 28938, 28969, 29001, 29033, 29064, 29096, 29128, 29160, 29191, 29223, 29255, 29287, 29318, 29350, 29382, 29414, 29446, 29478, 29510,  /* 896 - 927*/
                29542, 29574, 29605, 29637, 29669, 29701, 29733, 29766, 29798, 29830, 29862, 29894, 29926, 29958, 29990, 30022, 30055, 30087, 30119, 30151, 30184, 30216, 30248, 30281, 30313, 30346, 30378, 30410, 30443, 30475, 30508, 30540,  /* 928 - 959*/
                30573, 30606, 30638, 30671, 30704, 30736, 30769, 30802, 30835, 30867, 30900, 30933, 30966, 30999, 31032, 31065, 31098, 31131, 31164, 31197, 31230, 31264, 31297, 31330, 31364, 31397, 31430, 31464, 31497, 31531, 31564, 31598,  /* 960 - 991*/
                31632, 31666, 31699, 31733, 31767, 31801, 31835, 31869, 31904, 31938, 31972, 32006, 32041, 32076, 32110, 32145, 32180, 32215, 32250, 32285, 32321, 32356, 32392, 32428, 32464, 32500, 32536, 32573, 32610, 32648, 32686, 32726,  /* 992 - 1023*/
                32768, /*1024 - 1024*/
            },
            {
                0, 75, 136, 194, 248, 301, 352, 403, 452, 500, 548, 595, 641, 687, 732, 777, 822, 866, 910, 953, 997, 1040, 1082, 1125, 1167, 1209, 1250, 1292, 1333, 1374, 1415, 1455,  /* 0 - 31*/
                1496, 1536, 1576, 1616, 1656, 1695, 1735, 1774, 1814, 1853, 1892, 1930, 1969, 2008, 2046, 2085, 2123, 2161, 2199, 2237, 2275, 2313, 2350, 2388, 2425, 2463, 2500, 2537, 2574, 2611, 2648, 2685,  /* 32 - 63*/
                2722, 2758, 2795, 2831, 2868, 2904, 2941, 2977, 3013, 3049, 3085, 3121, 3157, 3193, 3229, 3264, 3300, 3336, 3371, 3407, 3442, 3477, 3513, 3548, 3583, 3618, 3653, 3688, 3723, 3758, 3793, 3828,  /* 64 - 95*/
                3863, 3898, 3932, 3967, 4001, 4036, 4070, 4105, 4139, 4174, 4208, 4242, 4276, 4311, 4345, 4379, 4413, 4447, 4481, 4515, 4549, 4582, 4616, 4650, 4684, 4717, 4751, 4785, 4818, 4852, 4885, 4919,  /* 96 - 127*/
                4952, 4986, 5019, 5052, 5086, 5119, 5152, 5185, 5218, 5252, 5285, 5318, 5351, 5384, 5417, 5450, 5483, 5515, 5548, 5581, 5614, 5647, 5679, 5712, 5745, 5777, 5810, 5842, 5875, 5907, 5940, 5972,  /* 128 - 159*/
                6005, 6037, 6070, 6102, 6134, 6167, 6199, 6231, 6263, 6295, 6328, 6360, 6392, 6424, 6456, 6488, 6520, 6552, 6584, 6616, 6648, 6680, 6711, 6743, 6775, 6807, 6839, 6870, 6902, 6934, 6966, 6997,  /* 160 - 191*/
                7029, 7060, 7092, 7124, 7155, 7187, 7218, 7250, 7281, 7312, 7344, 7375, 7407, 7438, 7469, 7501, 7532, 7563, 7594, 7626, 7657, 7688, 7719, 7750, 7781, 7813, 7844, 7875, 7906, 7937, 7968, 7999,  /* 192 - 223*/
                8030, 8061, 8092, 8122, 8153, 8184, 8215, 8246, 8277, 8308, 8338, 8369, 8400, 8431, 8461, 8492, 8523, 8553, 8584, 8615, 8645, 8676, 8706, 8737, 8767, 8798, 8829, 8859, 8889, 8920, 8950, 8981,  /* 224 - 255*/
                9011, 9042, 9072, 9102, 9133, 9163, 9194, 9224, 9254, 9285, 9315, 9346, 9376, 9406, 9437, 9467, 9498, 9528, 9558, 9589, 9619, 9650, 9680, 9710, 9741, 9771, 9802, 9832, 9862, 9893, 9923, 9954,  /* 256 - 287*/
                9984, 10014, 10045, 10075, 10106, 10136, 10166, 10197, 10227, 10258, 10288, 10318, 10349, 10379, 10410, 10440, 10470, 10501, 10531, 10562, 10592, 10622, 10653, 10683, 10714, 10744, 10774, 10805, 10835, 10866, 10896, 10926,  /* 288 - 319*/
                10957, 10987, 11018, 11048, 11078, 11109, 11139, 11170, 11200, 11230, 11261, 11291, 11322, 11352, 11382, 11413, 11443, 11474, 11504, 11534, 11565, 11595, 11626, 11656, 11686, 11717, 11747, 11778, 11808, 11838, 11869, 11899,  /* 320 - 351*/
                11930, 11960, 11990, 12021, 12051, 12082, 12112, 12142, 12173, 12203, 12234, 12264, 12294, 12325, 12355, 12386, 12416, 12446, 12477, 12507, 12538, 12568, 12598, 12629, 12659, 12690, 12720, 12750, 12781, 12811, 12842, 12872,  /* 352 - 383*/
                12902, 12933, 12963, 12994, 13024, 13054, 13085, 13115, 13146, 13176, 13206, 13237, 13267, 13298, 13328, 13358, 13389, 13419, 13450, 13480, 13510, 13541, 13571, 13602, 13632, 13662, 13693, 13723, 13754, 13784, 13814, 13845,  /* 384 - 415*/
                13875, 13906, 13936, 13966, 13997, 14027, 14058, 14088, 14118, 14149, 14179, 14210, 14240, 14270, 14301, 14331, 14362, 14392, 14422, 14453, 14483, 14514, 14544, 14574, 14605, 14635, 14666, 14696, 14726, 14757, 14787, 14818,  /* 416 - 447*/
                14848, 14878, 14909, 14939, 14970, 15000, 15030, 15061, 15091, 15122, 15152, 15182, 15213, 15243, 15274, 15304, 15334, 15365, 15395, 15426, 15456, 15486, 15517, 15547, 15578, 15608, 15638, 15669, 15699, 15730, 15760, 15790,  /* 448 - 479*/
                15821, 15851, 15882, 15912, 15942, 15973, 16003, 16034, 16064, 16094, 16125, 16155, 16186, 16216, 16246, 16277, 16307, 16338, 16368, 16398, 16429, 16459, 16490, 16520, 16550, 16581, 16611, 16642, 16672, 16702, 16733, 16763,  /* 480 - 511*/
                16794, 16824, 16854, 16885, 16915, 16946, 16976, 17006, 17037, 17067, 17098, 17128, 17158, 17189, 17219, 17250, 17280, 17310, 17341, 17371, 17402, 17432, 17462, 17493, 17523, 17554, 17584, 17614, 17645, 17675, 17706, 17736,  /* 512 - 543*/
                17766, 17797, 17827, 17858, 17888, 17918, 17949, 17979, 18010, 18040, 18070, 18101, 18131, 18162, 18192, 18222, 18253, 18283, 18314, 18344, 18374, 18405, 18435, 18466, 18496, 18526, 18557, 18587, 18618, 18648, 18678, 18709,  /* 544 - 575*/
                18739, 18770, 18800, 18830, 18861, 18891, 18922, 18952, 18982, 19013, 19043, 19074, 19104, 19134, 19165, 19195, 19226, 19256, 19286, 19317, 19347, 19378, 19408, 19438, 19469, 19499, 19530, 19560, 19590, 19621, 19651, 19682,  /* 576 - 607*/
                19712, 19742, 19773, 19803, 19834, 19864, 19894, 19925, 19955, 19986, 20016, 20046, 20077, 20107, 20138, 20168, 20198, 20229, 20259, 20290, 20320, 20350, 20381, 20411, 20442, 20472, 20502, 20533, 20563, 20594, 20624, 20654,  /* 608 - 639*/
                20685, 20715, 20746, 20776, 20806, 20837, 20867, 20898, 20928, 20958, 20989, 21019, 21050, 21080, 21110, 21141, 21171, 21202, 21232, 21262, 21293, 21323, 21354, 21384, 21414, 21445, 21475, 21506, 21536, 21566, 21597, 21627,  /* 640 - 671*/
                21658, 21688, 21718, 21749, 21779, 21810, 21840, 21870, 21901, 21931, 21962, 21992, 22022, 22053, 22083, 22114, 22144, 22174, 22205, 22235, 22266, 22296, 22326, 22357, 22387, 22418, 22448, 22478, 22509, 22539, 22570, 22600,  /* 672 - 703*/
                22630, 22661, 22691, 22722, 22752, 22782, 22813, 22843, 22874, 22904, 22934, 22965, 22995, 23026, 23056, 23086, 23117, 23147, 23178, 23208, 23238, 23269, 23299, 23330, 23360, 23390, 23421, 23451, 23482, 23512, 23542, 23573,  /* 704 - 735*/
                23603, 23634, 23664, 23694, 23725, 23755, 23786, 23816, 23846, 23877, 23907, 23938, 23968, 23998, 24029, 24059, 24090, 24120, 24150, 24181, 24211, 24242, 24272, 24302, 24333, 24363, 24394, 24424, 24454, 24485, 24515, 24546,  /* 736 - 767*/
                24576, 24606, 24637, 24667, 24698, 24728, 24759, 24789, 24819, 24850, 24880, 24911, 24941, 24972, 25002, 25033, 25063, 25094, 25124, 25155, 25185, 25216, 25246, 25277, 25307, 25338, 25368, 25399, 25430, 25460, 25491, 25521,  /* 768 - 799*/
                25552, 25583, 25613, 25644, 25674, 25705, 25736, 25766, 25797, 25828, 25858, 25889, 25920, 25950, 25981, 26012, 26043, 26073, 26104, 26135, 26165, 26196, 26227, 26258, 26289, 26319, 26350, 26381, 26412, 26443, 26473, 26504,  /* 800 - 831*/
                26535, 26566, 26597, 26628, 26658, 26689, 26720, 26751, 26782, 26813, 26844, 26875, 26906, 26937, 26968, 26999, 27029, 27060, 27091, 27122, 27153, 27184, 27216, 27247, 27278, 27309, 27340, 27371, 27402, 27433, 27464, 27495,  /* 832 - 863*/
                27526, 27557, 27589, 27620, 27651, 27682, 27713, 27744, 27776, 27807, 27838, 27869, 27900, 27932, 27963, 27994, 28026, 28057, 28088, 28119, 28151, 28182, 28213, 28245, 28276, 28308, 28339, 28370, 28402, 28433, 28465, 28496,  /* 864 - 895*/
                28528, 28559, 28591, 28622, 28654, 28685, 28717, 28748, 28780, 28811, 28843, 28875, 28906, 28938, 28969, 29001, 29033, 29064, 29096, 29128, 29160, 29191, 29223, 29255, 29287, 29318, 29350, 29382, 29414, 29446, 29478, 29510,  /* 896 - 927*/
                29542, 29574, 29605, 29637, 29669, 29701, 29733, 29766, 29798, 29830, 29862, 29894, 29926, 29958, 29990, 30022, 30055, 30087, 30119, 30151, 30184, 30216, 30248, 30281, 30313, 30346, 30378, 30410, 30443, 30475, 30508, 30540,  /* 928 - 959*/
                30573, 30606, 30638, 30671, 30704, 30736, 30769, 30802, 30835, 30867, 30900, 30933, 30966, 30999, 31032, 31065, 31098, 31131, 31164, 31197, 31230, 31264, 31297, 31330, 31364, 31397, 31430, 31464, 31497, 31531, 31564, 31598,  /* 960 - 991*/
                31632, 31666, 31699, 31733, 31767, 31801, 31835, 31869, 31904, 31938, 31972, 32006, 32041, 32076, 32110, 32145, 32180, 32215, 32250, 32285, 32321, 32356, 32392, 32428, 32464, 32500, 32536, 32573, 32610, 32648, 32686, 32726,  /* 992 - 1023*/
                32768, /*1024 - 1024*/
            },
            {
                0, 32, 64, 96, 128, 160, 192, 224, 256, 288, 320, 352, 384, 416, 448, 480, 512, 544, 576, 608, 640, 672, 704, 736, 768, 800, 832, 864, 896, 928, 960, 992,  /* 0 - 31*/
                1024, 1056, 1088, 1120, 1152, 1184, 1216, 1248, 1280, 1312, 1344, 1376, 1408, 1440, 1472, 1504, 1536, 1568, 1600, 1632, 1664, 1696, 1728, 1760, 1792, 1824, 1856, 1888, 1920, 1952, 1984, 2016,  /* 32 - 63*/
                2048, 2080, 2112, 2144, 2176, 2208, 2240, 2272, 2304, 2336, 2368, 2400, 2432, 2464, 2496, 2528, 2560, 2592, 2624, 2656, 2688, 2720, 2752, 2784, 2816, 2848, 2880, 2912, 2944, 2976, 3008, 3040,  /* 64 - 95*/
                3072, 3104, 3136, 3168, 3200, 3232, 3264, 3296, 3328, 3360, 3392, 3424, 3456, 3488, 3520, 3552, 3584, 3616, 3648, 3680, 3712, 3744, 3776, 3808, 3840, 3872, 3904, 3936, 3968, 4000, 4032, 4064,  /* 96 - 127*/
                4096, 4128, 4160, 4192, 4224, 4256, 4288, 4320, 4352, 4384, 4416, 4448, 4480, 4512, 4544, 4576, 4608, 4640, 4672, 4704, 4736, 4768, 4800, 4832, 4864, 4896, 4928, 4960, 4992, 5024, 5056, 5088,  /* 128 - 159*/
                5120, 5152, 5184, 5216, 5248, 5280, 5312, 5344, 5376, 5408, 5440, 5472, 5504, 5536, 5568, 5600, 5632, 5664, 5696, 5728, 5760, 5792, 5824, 5856, 5888, 5920, 5952, 5984, 6016, 6048, 6080, 6112,  /* 160 - 191*/
                6144, 6176, 6208, 6240, 6272, 6304, 6336, 6368, 6400, 6432, 6464, 6496, 6528, 6560, 6592, 6624, 6656, 6688, 6720, 6752, 6784, 6816, 6848, 6880, 6912, 6944, 6976, 7008, 7040, 7072, 7104, 7136,  /* 192 - 223*/
                7168, 7200, 7232, 7264, 7296, 7328, 7360, 7392, 7424, 7456, 7488, 7520, 7552, 7584, 7616, 7648, 7680, 7712, 7744, 7776, 7808, 7840, 7872, 7904, 7936, 7968, 8000, 8032, 8064, 8096, 8128, 8160,  /* 224 - 255*/
                8192, 8224, 8256, 8288, 8320, 8352, 8384, 8416, 8448, 8480, 8512, 8544, 8576, 8608, 8640, 8672, 8704, 8736, 8768, 8800, 8832, 8864, 8896, 8928, 8960, 8992, 9024, 9056, 9088, 9120, 9152, 9184,  /* 256 - 287*/
                9216, 9248, 9280, 9312, 9344, 9376, 9408, 9440, 9472, 9504, 9536, 9568, 9600, 9632, 9664, 9696, 9728, 9760, 9792, 9824, 9856, 9888, 9920, 9952, 9984, 10016, 10048, 10080, 10112, 10144, 10176, 10208,  /* 288 - 319*/
                10240, 10272, 10304, 10336, 10368, 10400, 10432, 10464, 10496, 10528, 10560, 10592, 10624, 10656, 10688, 10720, 10752, 10784, 10816, 10848, 10880, 10912, 10944, 10976, 11008, 11040, 11072, 11104, 11136, 11168, 11200, 11232,  /* 320 - 351*/
                11264, 11296, 11328, 11360, 11392, 11424, 11456, 11488, 11520, 11552, 11584, 11616, 11648, 11680, 11712, 11744, 11776, 11808, 11840, 11872, 11904, 11936, 11968, 12000, 12032, 12064, 12096, 12128, 12160, 12192, 12224, 12256,  /* 352 - 383*/
                12288, 12320, 12352, 12384, 12416, 12448, 12480, 12512, 12544, 12576, 12608, 12640, 12672, 12704, 12736, 12768, 12800, 12832, 12864, 12896, 12928, 12960, 12992, 13024, 13056, 13088, 13120, 13152, 13184, 13216, 13248, 13280,  /* 384 - 415*/
                13312, 13344, 13376, 13408, 13440, 13472, 13504, 13536, 13568, 13600, 13632, 13664, 13696, 13728, 13760, 13792, 13824, 13856, 13888, 13920, 13952, 13984, 14016, 14048, 14080, 14112, 14144, 14176, 14208, 14240, 14272, 14304,  /* 416 - 447*/
                14336, 14368, 14400, 14432, 14464, 14496, 14528, 14560, 14592, 14624, 14656, 14688, 14720, 14752, 14784, 14816, 14848, 14880, 14912, 14944, 14976, 15008, 15040, 15072, 15104, 15136, 15168, 15200, 15232, 15264, 15296, 15328,  /* 448 - 479*/
                15360, 15392, 15424, 15456, 15488, 15520, 15552, 15584, 15616, 15648, 15680, 15712, 15744, 15776, 15808, 15840, 15872, 15904, 15936, 15968, 16000, 16032, 16064, 16096, 16128, 16160, 16192, 16224, 16256, 16288, 16320, 16352,  /* 480 - 511*/
                16384, 16416, 16448, 16480, 16512, 16544, 16576, 16608, 16640, 16672, 16704, 16736, 16768, 16800, 16832, 16864, 16896, 16928, 16960, 16992, 17024, 17056, 17088, 17120, 17152, 17184, 17216, 17248, 17280, 17312, 17344, 17376,  /* 512 - 543*/
                17408, 17440, 17472, 17504, 17536, 17568, 17600, 17632, 17664, 17696, 17728, 17760, 17792, 17824, 17856, 17888, 17920, 17952, 17984, 18016, 18048, 18080, 18112, 18144, 18176, 18208, 18240, 18272, 18304, 18336, 18368, 18400,  /* 544 - 575*/
                18432, 18464, 18496, 18528, 18560, 18592, 18624, 18656, 18688, 18720, 18752, 18784, 18816, 18848, 18880, 18912, 18944, 18976, 19008, 19040, 19072, 19104, 19136, 19168, 19200, 19232, 19264, 19296, 19328, 19360, 19392, 19424,  /* 576 - 607*/
                19456, 19488, 19520, 19552, 19584, 19616, 19648, 19680, 19712, 19744, 19776, 19808, 19840, 19872, 19904, 19936, 19968, 20000, 20032, 20064, 20096, 20128, 20160, 20192, 20224, 20256, 20288, 20320, 20352, 20384, 20416, 20448,  /* 608 - 639*/
                20480, 20512, 20544, 20576, 20608, 20640, 20672, 20704, 20736, 20768, 20800, 20832, 20864, 20896, 20928, 20960, 20992, 21024, 21056, 21088, 21120, 21152, 21184, 21216, 21248, 21280, 21312, 21344, 21376, 21408, 21440, 21472,  /* 640 - 671*/
                21504, 21536, 21568, 21600, 21632, 21664, 21696, 21728, 21760, 21792, 21824, 21856, 21888, 21920, 21952, 21984, 22016, 22048, 22080, 22112, 22144, 22176, 22208, 22240, 22272, 22304, 22336, 22368, 22400, 22432, 22464, 22496,  /* 672 - 703*/
                22528, 22560, 22592, 22624, 22656, 22688, 22720, 22752, 22784, 22816, 22848, 22880, 22912, 22944, 22976, 23008, 23040, 23072, 23104, 23136, 23168, 23200, 23232, 23264, 23296, 23328, 23360, 23392, 23424, 23456, 23488, 23520,  /* 704 - 735*/
                23552, 23584, 23616, 23648, 23680, 23712, 23744, 23776, 23808, 23840, 23872, 23904, 23936, 23968, 24000, 24032, 24064, 24096, 24128, 24160, 24192, 24224, 24256, 24288, 24320, 24352, 24384, 24416, 24448, 24480, 24512, 24544,  /* 736 - 767*/
                24576, 24608, 24640, 24672, 24704, 24736, 24768, 24800, 24832, 24864, 24896, 24928, 24960, 24992, 25024, 25056, 25088, 25120, 25152, 25184, 25216, 25248, 25280, 25312, 25344, 25376, 25408, 25440, 25472, 25504, 25536, 25568,  /* 768 - 799*/
                25600, 25632, 25664, 25696, 25728, 25760, 25792, 25824, 25856, 25888, 25920, 25952, 25984, 26016, 26048, 26080, 26112, 26144, 26176, 26208, 26240, 26272, 26304, 26336, 26368, 26400, 26432, 26464, 26496, 26528, 26560, 26592,  /* 800 - 831*/
                26624, 26656, 26688, 26720, 26752, 26784, 26816, 26848, 26880, 26912, 26944, 26976, 27008, 27040, 27072, 27104, 27136, 27168, 27200, 27232, 27264, 27296, 27328, 27360, 27392, 27424, 27456, 27488, 27520, 27552, 27584, 27616,  /* 832 - 863*/
                27648, 27680, 27712, 27744, 27776, 27808, 27840, 27872, 27904, 27936, 27968, 28000, 28032, 28064, 28096, 28128, 28160, 28192, 28224, 28256, 28288, 28320, 28352, 28384, 28416, 28448, 28480, 28512, 28544, 28576, 28608, 28640,  /* 864 - 895*/
                28672, 28704, 28736, 28768, 28800, 28832, 28864, 28896, 28928, 28960, 28992, 29024, 29056, 29088, 29120, 29152, 29184, 29216, 29248, 29280, 29312, 29344, 29376, 29408, 29440, 29472, 29504, 29536, 29568, 29600, 29632, 29664,  /* 896 - 927*/
                29696, 29728, 29760, 29792, 29824, 29856, 29888, 29920, 29952, 29984, 30016, 30048, 30080, 30112, 30144, 30176, 30208, 30240, 30272, 30304, 30336, 30368, 30400, 30432, 30464, 30496, 30528, 30560, 30592, 30624, 30656, 30688,  /* 928 - 959*/
                30720, 30752, 30784, 30816, 30848, 30880, 30912, 30944, 30976, 31008, 31040, 31072, 31104, 31136, 31168, 31200, 31232, 31264, 31296, 31328, 31360, 31392, 31424, 31456, 31488, 31520, 31552, 31584, 31616, 31648, 31680, 31712,  /* 960 - 991*/
                31744, 31776, 31808, 31840, 31872, 31904, 31936, 31968, 32000, 32032, 32064, 32096, 32128, 32160, 32192, 32224, 32256, 32288, 32320, 32352, 32384, 32416, 32448, 32480, 32512, 32544, 32576, 32608, 32640, 32672, 32704, 32736,  /* 992 - 1023*/
                32768, /*1024 - 1024*/
            },
            {
                0, 32, 64, 96, 128, 160, 192, 224, 256, 288, 320, 352, 384, 416, 448, 480, 512, 544, 576, 608, 640, 672, 704, 736, 768, 800, 832, 864, 896, 928, 960, 992,  /* 0 - 31*/
                1024, 1056, 1088, 1120, 1152, 1184, 1216, 1248, 1280, 1312, 1344, 1376, 1408, 1440, 1472, 1504, 1536, 1568, 1600, 1632, 1664, 1696, 1728, 1760, 1792, 1824, 1856, 1888, 1920, 1952, 1984, 2016,  /* 32 - 63*/
                2048, 2080, 2112, 2144, 2176, 2208, 2240, 2272, 2304, 2336, 2368, 2400, 2432, 2464, 2496, 2528, 2560, 2592, 2624, 2656, 2688, 2720, 2752, 2784, 2816, 2848, 2880, 2912, 2944, 2976, 3008, 3040,  /* 64 - 95*/
                3072, 3104, 3136, 3168, 3200, 3232, 3264, 3296, 3328, 3360, 3392, 3424, 3456, 3488, 3520, 3552, 3584, 3616, 3648, 3680, 3712, 3744, 3776, 3808, 3840, 3872, 3904, 3936, 3968, 4000, 4032, 4064,  /* 96 - 127*/
                4096, 4128, 4160, 4192, 4224, 4256, 4288, 4320, 4352, 4384, 4416, 4448, 4480, 4512, 4544, 4576, 4608, 4640, 4672, 4704, 4736, 4768, 4800, 4832, 4864, 4896, 4928, 4960, 4992, 5024, 5056, 5088,  /* 128 - 159*/
                5120, 5152, 5184, 5216, 5248, 5280, 5312, 5344, 5376, 5408, 5440, 5472, 5504, 5536, 5568, 5600, 5632, 5664, 5696, 5728, 5760, 5792, 5824, 5856, 5888, 5920, 5952, 5984, 6016, 6048, 6080, 6112,  /* 160 - 191*/
                6144, 6176, 6208, 6240, 6272, 6304, 6336, 6368, 6400, 6432, 6464, 6496, 6528, 6560, 6592, 6624, 6656, 6688, 6720, 6752, 6784, 6816, 6848, 6880, 6912, 6944, 6976, 7008, 7040, 7072, 7104, 7136,  /* 192 - 223*/
                7168, 7200, 7232, 7264, 7296, 7328, 7360, 7392, 7424, 7456, 7488, 7520, 7552, 7584, 7616, 7648, 7680, 7712, 7744, 7776, 7808, 7840, 7872, 7904, 7936, 7968, 8000, 8032, 8064, 8096, 8128, 8160,  /* 224 - 255*/
                8192, 8224, 8256, 8288, 8320, 8352, 8384, 8416, 8448, 8480, 8512, 8544, 8576, 8608, 8640, 8672, 8704, 8736, 8768, 8800, 8832, 8864, 8896, 8928, 8960, 8992, 9024, 9056, 9088, 9120, 9152, 9184,  /* 256 - 287*/
                9216, 9248, 9280, 9312, 9344, 9376, 9408, 9440, 9472, 9504, 9536, 9568, 9600, 9632, 9664, 9696, 9728, 9760, 9792, 9824, 9856, 9888, 9920, 9952, 9984, 10016, 10048, 10080, 10112, 10144, 10176, 10208,  /* 288 - 319*/
                10240, 10272, 10304, 10336, 10368, 10400, 10432, 10464, 10496, 10528, 10560, 10592, 10624, 10656, 10688, 10720, 10752, 10784, 10816, 10848, 10880, 10912, 10944, 10976, 11008, 11040, 11072, 11104, 11136, 11168, 11200, 11232,  /* 320 - 351*/
                11264, 11296, 11328, 11360, 11392, 11424, 11456, 11488, 11520, 11552, 11584, 11616, 11648, 11680, 11712, 11744, 11776, 11808, 11840, 11872, 11904, 11936, 11968, 12000, 12032, 12064, 12096, 12128, 12160, 12192, 12224, 12256,  /* 352 - 383*/
                12288, 12320, 12352, 12384, 12416, 12448, 12480, 12512, 12544, 12576, 12608, 12640, 12672, 12704, 12736, 12768, 12800, 12832, 12864, 12896, 12928, 12960, 12992, 13024, 13056, 13088, 13120, 13152, 13184, 13216, 13248, 13280,  /* 384 - 415*/
                13312, 13344, 13376, 13408, 13440, 13472, 13504, 13536, 13568, 13600, 13632, 13664, 13696, 13728, 13760, 13792, 13824, 13856, 13888, 13920, 13952, 13984, 14016, 14048, 14080, 14112, 14144, 14176, 14208, 14240, 14272, 14304,  /* 416 - 447*/
                14336, 14368, 14400, 14432, 14464, 14496, 14528, 14560, 14592, 14624, 14656, 14688, 14720, 14752, 14784, 14816, 14848, 14880, 14912, 14944, 14976, 15008, 15040, 15072, 15104, 15136, 15168, 15200, 15232, 15264, 15296, 15328,  /* 448 - 479*/
                15360, 15392, 15424, 15456, 15488, 15520, 15552, 15584, 15616, 15648, 15680, 15712, 15744, 15776, 15808, 15840, 15872, 15904, 15936, 15968, 16000, 16032, 16064, 16096, 16128, 16160, 16192, 16224, 16256, 16288, 16320, 16352,  /* 480 - 511*/
                16384, 16416, 16448, 16480, 16512, 16544, 16576, 16608, 16640, 16672, 16704, 16736, 16768, 16800, 16832, 16864, 16896, 16928, 16960, 16992, 17024, 17056, 17088, 17120, 17152, 17184, 17216, 17248, 17280, 17312, 17344, 17376,  /* 512 - 543*/
                17408, 17440, 17472, 17504, 17536, 17568, 17600, 17632, 17664, 17696, 17728, 17760, 17792, 17824, 17856, 17888, 17920, 17952, 17984, 18016, 18048, 18080, 18112, 18144, 18176, 18208, 18240, 18272, 18304, 18336, 18368, 18400,  /* 544 - 575*/
                18432, 18464, 18496, 18528, 18560, 18592, 18624, 18656, 18688, 18720, 18752, 18784, 18816, 18848, 18880, 18912, 18944, 18976, 19008, 19040, 19072, 19104, 19136, 19168, 19200, 19232, 19264, 19296, 19328, 19360, 19392, 19424,  /* 576 - 607*/
                19456, 19488, 19520, 19552, 19584, 19616, 19648, 19680, 19712, 19744, 19776, 19808, 19840, 19872, 19904, 19936, 19968, 20000, 20032, 20064, 20096, 20128, 20160, 20192, 20224, 20256, 20288, 20320, 20352, 20384, 20416, 20448,  /* 608 - 639*/
                20480, 20512, 20544, 20576, 20608, 20640, 20672, 20704, 20736, 20768, 20800, 20832, 20864, 20896, 20928, 20960, 20992, 21024, 21056, 21088, 21120, 21152, 21184, 21216, 21248, 21280, 21312, 21344, 21376, 21408, 21440, 21472,  /* 640 - 671*/
                21504, 21536, 21568, 21600, 21632, 21664, 21696, 21728, 21760, 21792, 21824, 21856, 21888, 21920, 21952, 21984, 22016, 22048, 22080, 22112, 22144, 22176, 22208, 22240, 22272, 22304, 22336, 22368, 22400, 22432, 22464, 22496,  /* 672 - 703*/
                22528, 22560, 22592, 22624, 22656, 22688, 22720, 22752, 22784, 22816, 22848, 22880, 22912, 22944, 22976, 23008, 23040, 23072, 23104, 23136, 23168, 23200, 23232, 23264, 23296, 23328, 23360, 23392, 23424, 23456, 23488, 23520,  /* 704 - 735*/
                23552, 23584, 23616, 23648, 23680, 23712, 23744, 23776, 23808, 23840, 23872, 23904, 23936, 23968, 24000, 24032, 24064, 24096, 24128, 24160, 24192, 24224, 24256, 24288, 24320, 24352, 24384, 24416, 24448, 24480, 24512, 24544,  /* 736 - 767*/
                24576, 24608, 24640, 24672, 24704, 24736, 24768, 24800, 24832, 24864, 24896, 24928, 24960, 24992, 25024, 25056, 25088, 25120, 25152, 25184, 25216, 25248, 25280, 25312, 25344, 25376, 25408, 25440, 25472, 25504, 25536, 25568,  /* 768 - 799*/
                25600, 25632, 25664, 25696, 25728, 25760, 25792, 25824, 25856, 25888, 25920, 25952, 25984, 26016, 26048, 26080, 26112, 26144, 26176, 26208, 26240, 26272, 26304, 26336, 26368, 26400, 26432, 26464, 26496, 26528, 26560, 26592,  /* 800 - 831*/
                26624, 26656, 26688, 26720, 26752, 26784, 26816, 26848, 26880, 26912, 26944, 26976, 27008, 27040, 27072, 27104, 27136, 27168, 27200, 27232, 27264, 27296, 27328, 27360, 27392, 27424, 27456, 27488, 27520, 27552, 27584, 27616,  /* 832 - 863*/
                27648, 27680, 27712, 27744, 27776, 27808, 27840, 27872, 27904, 27936, 27968, 28000, 28032, 28064, 28096, 28128, 28160, 28192, 28224, 28256, 28288, 28320, 28352, 28384, 28416, 28448, 28480, 28512, 28544, 28576, 28608, 28640,  /* 864 - 895*/
                28672, 28704, 28736, 28768, 28800, 28832, 28864, 28896, 28928, 28960, 28992, 29024, 29056, 29088, 29120, 29152, 29184, 29216, 29248, 29280, 29312, 29344, 29376, 29408, 29440, 29472, 29504, 29536, 29568, 29600, 29632, 29664,  /* 896 - 927*/
                29696, 29728, 29760, 29792, 29824, 29856, 29888, 29920, 29952, 29984, 30016, 30048, 30080, 30112, 30144, 30176, 30208, 30240, 30272, 30304, 30336, 30368, 30400, 30432, 30464, 30496, 30528, 30560, 30592, 30624, 30656, 30688,  /* 928 - 959*/
                30720, 30752, 30784, 30816, 30848, 30880, 30912, 30944, 30976, 31008, 31040, 31072, 31104, 31136, 31168, 31200, 31232, 31264, 31296, 31328, 31360, 31392, 31424, 31456, 31488, 31520, 31552, 31584, 31616, 31648, 31680, 31712,  /* 960 - 991*/
                31744, 31776, 31808, 31840, 31872, 31904, 31936, 31968, 32000, 32032, 32064, 32096, 32128, 32160, 32192, 32224, 32256, 32288, 32320, 32352, 32384, 32416, 32448, 32480, 32512, 32544, 32576, 32608, 32640, 32672, 32704, 32736,  /* 992 - 1023*/
                32768, /*1024 - 1024*/
            },
            {
                0, 32, 64, 96, 128, 160, 192, 224, 256, 288, 320, 352, 384, 416, 448, 480, 512, 544, 576, 608, 640, 672, 704, 736, 768, 800, 832, 864, 896, 928, 960, 992,  /* 0 - 31*/
                1024, 1056, 1088, 1120, 1152, 1184, 1216, 1248, 1280, 1312, 1344, 1376, 1408, 1440, 1472, 1504, 1536, 1568, 1600, 1632, 1664, 1696, 1728, 1760, 1792, 1824, 1856, 1888, 1920, 1952, 1984, 2016,  /* 32 - 63*/
                2048, 2080, 2112, 2144, 2176, 2208, 2240, 2272, 2304, 2336, 2368, 2400, 2432, 2464, 2496, 2528, 2560, 2592, 2624, 2656, 2688, 2720, 2752, 2784, 2816, 2848, 2880, 2912, 2944, 2976, 3008, 3040,  /* 64 - 95*/
                3072, 3104, 3136, 3168, 3200, 3232, 3264, 3296, 3328, 3360, 3392, 3424, 3456, 3488, 3520, 3552, 3584, 3616, 3648, 3680, 3712, 3744, 3776, 3808, 3840, 3872, 3904, 3936, 3968, 4000, 4032, 4064,  /* 96 - 127*/
                4096, 4128, 4160, 4192, 4224, 4256, 4288, 4320, 4352, 4384, 4416, 4448, 4480, 4512, 4544, 4576, 4608, 4640, 4672, 4704, 4736, 4768, 4800, 4832, 4864, 4896, 4928, 4960, 4992, 5024, 5056, 5088,  /* 128 - 159*/
                5120, 5152, 5184, 5216, 5248, 5280, 5312, 5344, 5376, 5408, 5440, 5472, 5504, 5536, 5568, 5600, 5632, 5664, 5696, 5728, 5760, 5792, 5824, 5856, 5888, 5920, 5952, 5984, 6016, 6048, 6080, 6112,  /* 160 - 191*/
                6144, 6176, 6208, 6240, 6272, 6304, 6336, 6368, 6400, 6432, 6464, 6496, 6528, 6560, 6592, 6624, 6656, 6688, 6720, 6752, 6784, 6816, 6848, 6880, 6912, 6944, 6976, 7008, 7040, 7072, 7104, 7136,  /* 192 - 223*/
                7168, 7200, 7232, 7264, 7296, 7328, 7360, 7392, 7424, 7456, 7488, 7520, 7552, 7584, 7616, 7648, 7680, 7712, 7744, 7776, 7808, 7840, 7872, 7904, 7936, 7968, 8000, 8032, 8064, 8096, 8128, 8160,  /* 224 - 255*/
                8192, 8224, 8256, 8288, 8320, 8352, 8384, 8416, 8448, 8480, 8512, 8544, 8576, 8608, 8640, 8672, 8704, 8736, 8768, 8800, 8832, 8864, 8896, 8928, 8960, 8992, 9024, 9056, 9088, 9120, 9152, 9184,  /* 256 - 287*/
                9216, 9248, 9280, 9312, 9344, 9376, 9408, 9440, 9472, 9504, 9536, 9568, 9600, 9632, 9664, 9696, 9728, 9760, 9792, 9824, 9856, 9888, 9920, 9952, 9984, 10016, 10048, 10080, 10112, 10144, 10176, 10208,  /* 288 - 319*/
                10240, 10272, 10304, 10336, 10368, 10400, 10432, 10464, 10496, 10528, 10560, 10592, 10624, 10656, 10688, 10720, 10752, 10784, 10816, 10848, 10880, 10912, 10944, 10976, 11008, 11040, 11072, 11104, 11136, 11168, 11200, 11232,  /* 320 - 351*/
                11264, 11296, 11328, 11360, 11392, 11424, 11456, 11488, 11520, 11552, 11584, 11616, 11648, 11680, 11712, 11744, 11776, 11808, 11840, 11872, 11904, 11936, 11968, 12000, 12032, 12064, 12096, 12128, 12160, 12192, 12224, 12256,  /* 352 - 383*/
                12288, 12320, 12352, 12384, 12416, 12448, 12480, 12512, 12544, 12576, 12608, 12640, 12672, 12704, 12736, 12768, 12800, 12832, 12864, 12896, 12928, 12960, 12992, 13024, 13056, 13088, 13120, 13152, 13184, 13216, 13248, 13280,  /* 384 - 415*/
                13312, 13344, 13376, 13408, 13440, 13472, 13504, 13536, 13568, 13600, 13632, 13664, 13696, 13728, 13760, 13792, 13824, 13856, 13888, 13920, 13952, 13984, 14016, 14048, 14080, 14112, 14144, 14176, 14208, 14240, 14272, 14304,  /* 416 - 447*/
                14336, 14368, 14400, 14432, 14464, 14496, 14528, 14560, 14592, 14624, 14656, 14688, 14720, 14752, 14784, 14816, 14848, 14880, 14912, 14944, 14976, 15008, 15040, 15072, 15104, 15136, 15168, 15200, 15232, 15264, 15296, 15328,  /* 448 - 479*/
                15360, 15392, 15424, 15456, 15488, 15520, 15552, 15584, 15616, 15648, 15680, 15712, 15744, 15776, 15808, 15840, 15872, 15904, 15936, 15968, 16000, 16032, 16064, 16096, 16128, 16160, 16192, 16224, 16256, 16288, 16320, 16352,  /* 480 - 511*/
                16384, 16416, 16448, 16480, 16512, 16544, 16576, 16608, 16640, 16672, 16704, 16736, 16768, 16800, 16832, 16864, 16896, 16928, 16960, 16992, 17024, 17056, 17088, 17120, 17152, 17184, 17216, 17248, 17280, 17312, 17344, 17376,  /* 512 - 543*/
                17408, 17440, 17472, 17504, 17536, 17568, 17600, 17632, 17664, 17696, 17728, 17760, 17792, 17824, 17856, 17888, 17920, 17952, 17984, 18016, 18048, 18080, 18112, 18144, 18176, 18208, 18240, 18272, 18304, 18336, 18368, 18400,  /* 544 - 575*/
                18432, 18464, 18496, 18528, 18560, 18592, 18624, 18656, 18688, 18720, 18752, 18784, 18816, 18848, 18880, 18912, 18944, 18976, 19008, 19040, 19072, 19104, 19136, 19168, 19200, 19232, 19264, 19296, 19328, 19360, 19392, 19424,  /* 576 - 607*/
                19456, 19488, 19520, 19552, 19584, 19616, 19648, 19680, 19712, 19744, 19776, 19808, 19840, 19872, 19904, 19936, 19968, 20000, 20032, 20064, 20096, 20128, 20160, 20192, 20224, 20256, 20288, 20320, 20352, 20384, 20416, 20448,  /* 608 - 639*/
                20480, 20512, 20544, 20576, 20608, 20640, 20672, 20704, 20736, 20768, 20800, 20832, 20864, 20896, 20928, 20960, 20992, 21024, 21056, 21088, 21120, 21152, 21184, 21216, 21248, 21280, 21312, 21344, 21376, 21408, 21440, 21472,  /* 640 - 671*/
                21504, 21536, 21568, 21600, 21632, 21664, 21696, 21728, 21760, 21792, 21824, 21856, 21888, 21920, 21952, 21984, 22016, 22048, 22080, 22112, 22144, 22176, 22208, 22240, 22272, 22304, 22336, 22368, 22400, 22432, 22464, 22496,  /* 672 - 703*/
                22528, 22560, 22592, 22624, 22656, 22688, 22720, 22752, 22784, 22816, 22848, 22880, 22912, 22944, 22976, 23008, 23040, 23072, 23104, 23136, 23168, 23200, 23232, 23264, 23296, 23328, 23360, 23392, 23424, 23456, 23488, 23520,  /* 704 - 735*/
                23552, 23584, 23616, 23648, 23680, 23712, 23744, 23776, 23808, 23840, 23872, 23904, 23936, 23968, 24000, 24032, 24064, 24096, 24128, 24160, 24192, 24224, 24256, 24288, 24320, 24352, 24384, 24416, 24448, 24480, 24512, 24544,  /* 736 - 767*/
                24576, 24608, 24640, 24672, 24704, 24736, 24768, 24800, 24832, 24864, 24896, 24928, 24960, 24992, 25024, 25056, 25088, 25120, 25152, 25184, 25216, 25248, 25280, 25312, 25344, 25376, 25408, 25440, 25472, 25504, 25536, 25568,  /* 768 - 799*/
                25600, 25632, 25664, 25696, 25728, 25760, 25792, 25824, 25856, 25888, 25920, 25952, 25984, 26016, 26048, 26080, 26112, 26144, 26176, 26208, 26240, 26272, 26304, 26336, 26368, 26400, 26432, 26464, 26496, 26528, 26560, 26592,  /* 800 - 831*/
                26624, 26656, 26688, 26720, 26752, 26784, 26816, 26848, 26880, 26912, 26944, 26976, 27008, 27040, 27072, 27104, 27136, 27168, 27200, 27232, 27264, 27296, 27328, 27360, 27392, 27424, 27456, 27488, 27520, 27552, 27584, 27616,  /* 832 - 863*/
                27648, 27680, 27712, 27744, 27776, 27808, 27840, 27872, 27904, 27936, 27968, 28000, 28032, 28064, 28096, 28128, 28160, 28192, 28224, 28256, 28288, 28320, 28352, 28384, 28416, 28448, 28480, 28512, 28544, 28576, 28608, 28640,  /* 864 - 895*/
                28672, 28704, 28736, 28768, 28800, 28832, 28864, 28896, 28928, 28960, 28992, 29024, 29056, 29088, 29120, 29152, 29184, 29216, 29248, 29280, 29312, 29344, 29376, 29408, 29440, 29472, 29504, 29536, 29568, 29600, 29632, 29664,  /* 896 - 927*/
                29696, 29728, 29760, 29792, 29824, 29856, 29888, 29920, 29952, 29984, 30016, 30048, 30080, 30112, 30144, 30176, 30208, 30240, 30272, 30304, 30336, 30368, 30400, 30432, 30464, 30496, 30528, 30560, 30592, 30624, 30656, 30688,  /* 928 - 959*/
                30720, 30752, 30784, 30816, 30848, 30880, 30912, 30944, 30976, 31008, 31040, 31072, 31104, 31136, 31168, 31200, 31232, 31264, 31296, 31328, 31360, 31392, 31424, 31456, 31488, 31520, 31552, 31584, 31616, 31648, 31680, 31712,  /* 960 - 991*/
                31744, 31776, 31808, 31840, 31872, 31904, 31936, 31968, 32000, 32032, 32064, 32096, 32128, 32160, 32192, 32224, 32256, 32288, 32320, 32352, 32384, 32416, 32448, 32480, 32512, 32544, 32576, 32608, 32640, 32672, 32704, 32736,  /* 992 - 1023*/
                32768, /*1024 - 1024*/
            },
            {
                0, 32, 64, 96, 128, 160, 192, 224, 256, 288, 320, 352, 384, 416, 448, 480, 512, 544, 576, 608, 640, 672, 704, 736, 768, 800, 832, 864, 896, 928, 960, 992,  /* 0 - 31*/
                1024, 1056, 1088, 1120, 1152, 1184, 1216, 1248, 1280, 1312, 1344, 1376, 1408, 1440, 1472, 1504, 1536, 1568, 1600, 1632, 1664, 1696, 1728, 1760, 1792, 1824, 1856, 1888, 1920, 1952, 1984, 2016,  /* 32 - 63*/
                2048, 2080, 2112, 2144, 2176, 2208, 2240, 2272, 2304, 2336, 2368, 2400, 2432, 2464, 2496, 2528, 2560, 2592, 2624, 2656, 2688, 2720, 2752, 2784, 2816, 2848, 2880, 2912, 2944, 2976, 3008, 3040,  /* 64 - 95*/
                3072, 3104, 3136, 3168, 3200, 3232, 3264, 3296, 3328, 3360, 3392, 3424, 3456, 3488, 3520, 3552, 3584, 3616, 3648, 3680, 3712, 3744, 3776, 3808, 3840, 3872, 3904, 3936, 3968, 4000, 4032, 4064,  /* 96 - 127*/
                4096, 4128, 4160, 4192, 4224, 4256, 4288, 4320, 4352, 4384, 4416, 4448, 4480, 4512, 4544, 4576, 4608, 4640, 4672, 4704, 4736, 4768, 4800, 4832, 4864, 4896, 4928, 4960, 4992, 5024, 5056, 5088,  /* 128 - 159*/
                5120, 5152, 5184, 5216, 5248, 5280, 5312, 5344, 5376, 5408, 5440, 5472, 5504, 5536, 5568, 5600, 5632, 5664, 5696, 5728, 5760, 5792, 5824, 5856, 5888, 5920, 5952, 5984, 6016, 6048, 6080, 6112,  /* 160 - 191*/
                6144, 6176, 6208, 6240, 6272, 6304, 6336, 6368, 6400, 6432, 6464, 6496, 6528, 6560, 6592, 6624, 6656, 6688, 6720, 6752, 6784, 6816, 6848, 6880, 6912, 6944, 6976, 7008, 7040, 7072, 7104, 7136,  /* 192 - 223*/
                7168, 7200, 7232, 7264, 7296, 7328, 7360, 7392, 7424, 7456, 7488, 7520, 7552, 7584, 7616, 7648, 7680, 7712, 7744, 7776, 7808, 7840, 7872, 7904, 7936, 7968, 8000, 8032, 8064, 8096, 8128, 8160,  /* 224 - 255*/
                8192, 8224, 8256, 8288, 8320, 8352, 8384, 8416, 8448, 8480, 8512, 8544, 8576, 8608, 8640, 8672, 8704, 8736, 8768, 8800, 8832, 8864, 8896, 8928, 8960, 8992, 9024, 9056, 9088, 9120, 9152, 9184,  /* 256 - 287*/
                9216, 9248, 9280, 9312, 9344, 9376, 9408, 9440, 9472, 9504, 9536, 9568, 9600, 9632, 9664, 9696, 9728, 9760, 9792, 9824, 9856, 9888, 9920, 9952, 9984, 10016, 10048, 10080, 10112, 10144, 10176, 10208,  /* 288 - 319*/
                10240, 10272, 10304, 10336, 10368, 10400, 10432, 10464, 10496, 10528, 10560, 10592, 10624, 10656, 10688, 10720, 10752, 10784, 10816, 10848, 10880, 10912, 10944, 10976, 11008, 11040, 11072, 11104, 11136, 11168, 11200, 11232,  /* 320 - 351*/
                11264, 11296, 11328, 11360, 11392, 11424, 11456, 11488, 11520, 11552, 11584, 11616, 11648, 11680, 11712, 11744, 11776, 11808, 11840, 11872, 11904, 11936, 11968, 12000, 12032, 12064, 12096, 12128, 12160, 12192, 12224, 12256,  /* 352 - 383*/
                12288, 12320, 12352, 12384, 12416, 12448, 12480, 12512, 12544, 12576, 12608, 12640, 12672, 12704, 12736, 12768, 12800, 12832, 12864, 12896, 12928, 12960, 12992, 13024, 13056, 13088, 13120, 13152, 13184, 13216, 13248, 13280,  /* 384 - 415*/
                13312, 13344, 13376, 13408, 13440, 13472, 13504, 13536, 13568, 13600, 13632, 13664, 13696, 13728, 13760, 13792, 13824, 13856, 13888, 13920, 13952, 13984, 14016, 14048, 14080, 14112, 14144, 14176, 14208, 14240, 14272, 14304,  /* 416 - 447*/
                14336, 14368, 14400, 14432, 14464, 14496, 14528, 14560, 14592, 14624, 14656, 14688, 14720, 14752, 14784, 14816, 14848, 14880, 14912, 14944, 14976, 15008, 15040, 15072, 15104, 15136, 15168, 15200, 15232, 15264, 15296, 15328,  /* 448 - 479*/
                15360, 15392, 15424, 15456, 15488, 15520, 15552, 15584, 15616, 15648, 15680, 15712, 15744, 15776, 15808, 15840, 15872, 15904, 15936, 15968, 16000, 16032, 16064, 16096, 16128, 16160, 16192, 16224, 16256, 16288, 16320, 16352,  /* 480 - 511*/
                16384, 16416, 16448, 16480, 16512, 16544, 16576, 16608, 16640, 16672, 16704, 16736, 16768, 16800, 16832, 16864, 16896, 16928, 16960, 16992, 17024, 17056, 17088, 17120, 17152, 17184, 17216, 17248, 17280, 17312, 17344, 17376,  /* 512 - 543*/
                17408, 17440, 17472, 17504, 17536, 17568, 17600, 17632, 17664, 17696, 17728, 17760, 17792, 17824, 17856, 17888, 17920, 17952, 17984, 18016, 18048, 18080, 18112, 18144, 18176, 18208, 18240, 18272, 18304, 18336, 18368, 18400,  /* 544 - 575*/
                18432, 18464, 18496, 18528, 18560, 18592, 18624, 18656, 18688, 18720, 18752, 18784, 18816, 18848, 18880, 18912, 18944, 18976, 19008, 19040, 19072, 19104, 19136, 19168, 19200, 19232, 19264, 19296, 19328, 19360, 19392, 19424,  /* 576 - 607*/
                19456, 19488, 19520, 19552, 19584, 19616, 19648, 19680, 19712, 19744, 19776, 19808, 19840, 19872, 19904, 19936, 19968, 20000, 20032, 20064, 20096, 20128, 20160, 20192, 20224, 20256, 20288, 20320, 20352, 20384, 20416, 20448,  /* 608 - 639*/
                20480, 20512, 20544, 20576, 20608, 20640, 20672, 20704, 20736, 20768, 20800, 20832, 20864, 20896, 20928, 20960, 20992, 21024, 21056, 21088, 21120, 21152, 21184, 21216, 21248, 21280, 21312, 21344, 21376, 21408, 21440, 21472,  /* 640 - 671*/
                21504, 21536, 21568, 21600, 21632, 21664, 21696, 21728, 21760, 21792, 21824, 21856, 21888, 21920, 21952, 21984, 22016, 22048, 22080, 22112, 22144, 22176, 22208, 22240, 22272, 22304, 22336, 22368, 22400, 22432, 22464, 22496,  /* 672 - 703*/
                22528, 22560, 22592, 22624, 22656, 22688, 22720, 22752, 22784, 22816, 22848, 22880, 22912, 22944, 22976, 23008, 23040, 23072, 23104, 23136, 23168, 23200, 23232, 23264, 23296, 23328, 23360, 23392, 23424, 23456, 23488, 23520,  /* 704 - 735*/
                23552, 23584, 23616, 23648, 23680, 23712, 23744, 23776, 23808, 23840, 23872, 23904, 23936, 23968, 24000, 24032, 24064, 24096, 24128, 24160, 24192, 24224, 24256, 24288, 24320, 24352, 24384, 24416, 24448, 24480, 24512, 24544,  /* 736 - 767*/
                24576, 24608, 24640, 24672, 24704, 24736, 24768, 24800, 24832, 24864, 24896, 24928, 24960, 24992, 25024, 25056, 25088, 25120, 25152, 25184, 25216, 25248, 25280, 25312, 25344, 25376, 25408, 25440, 25472, 25504, 25536, 25568,  /* 768 - 799*/
                25600, 25632, 25664, 25696, 25728, 25760, 25792, 25824, 25856, 25888, 25920, 25952, 25984, 26016, 26048, 26080, 26112, 26144, 26176, 26208, 26240, 26272, 26304, 26336, 26368, 26400, 26432, 26464, 26496, 26528, 26560, 26592,  /* 800 - 831*/
                26624, 26656, 26688, 26720, 26752, 26784, 26816, 26848, 26880, 26912, 26944, 26976, 27008, 27040, 27072, 27104, 27136, 27168, 27200, 27232, 27264, 27296, 27328, 27360, 27392, 27424, 27456, 27488, 27520, 27552, 27584, 27616,  /* 832 - 863*/
                27648, 27680, 27712, 27744, 27776, 27808, 27840, 27872, 27904, 27936, 27968, 28000, 28032, 28064, 28096, 28128, 28160, 28192, 28224, 28256, 28288, 28320, 28352, 28384, 28416, 28448, 28480, 28512, 28544, 28576, 28608, 28640,  /* 864 - 895*/
                28672, 28704, 28736, 28768, 28800, 28832, 28864, 28896, 28928, 28960, 28992, 29024, 29056, 29088, 29120, 29152, 29184, 29216, 29248, 29280, 29312, 29344, 29376, 29408, 29440, 29472, 29504, 29536, 29568, 29600, 29632, 29664,  /* 896 - 927*/
                29696, 29728, 29760, 29792, 29824, 29856, 29888, 29920, 29952, 29984, 30016, 30048, 30080, 30112, 30144, 30176, 30208, 30240, 30272, 30304, 30336, 30368, 30400, 30432, 30464, 30496, 30528, 30560, 30592, 30624, 30656, 30688,  /* 928 - 959*/
                30720, 30752, 30784, 30816, 30848, 30880, 30912, 30944, 30976, 31008, 31040, 31072, 31104, 31136, 31168, 31200, 31232, 31264, 31296, 31328, 31360, 31392, 31424, 31456, 31488, 31520, 31552, 31584, 31616, 31648, 31680, 31712,  /* 960 - 991*/
                31744, 31776, 31808, 31840, 31872, 31904, 31936, 31968, 32000, 32032, 32064, 32096, 32128, 32160, 32192, 32224, 32256, 32288, 32320, 32352, 32384, 32416, 32448, 32480, 32512, 32544, 32576, 32608, 32640, 32672, 32704, 32736,  /* 992 - 1023*/
                32768, /*1024 - 1024*/
            },
        },
        /* tHistWt */
        {
            /* 0 */
            {
                /* 0 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
                /* 1 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
                /* 2 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
                /* 3 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
                /* 4 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
                /* 5 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
                /* 6 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
                /* 7 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
                /* 8 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
                /* 9 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
                /* 10 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
                /* 11 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
                /* 12 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
                /* 13 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
                /* 14 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
                /* 15 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
            },
            /* 1 */
            {
                /* 0 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
                /* 1 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
                /* 2 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
                /* 3 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
                /* 4 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
                /* 5 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
                /* 6 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
                /* 7 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
                /* 8 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
                /* 9 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
                /* 10 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
                /* 11 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
                /* 12 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
                /* 13 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
                /* 14 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
                /* 15 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
            },
            /* 2 */
            {
                /* 0 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
                /* 1 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
                /* 2 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
                /* 3 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
                /* 4 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
                /* 5 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
                /* 6 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
                /* 7 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
                /* 8 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
                /* 9 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
                /* 10 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
                /* 11 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
                /* 12 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
                /* 13 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
                /* 14 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
                /* 15 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
            },
            /* 3 */
            {
                /* 0 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
                /* 1 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
                /* 2 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
                /* 3 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
                /* 4 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
                /* 5 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
                /* 6 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
                /* 7 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
                /* 8 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
                /* 9 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
                /* 10 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
                /* 11 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
                /* 12 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
                /* 13 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
                /* 14 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
                /* 15 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
            },
            /* 4 */
            {
                /* 0 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
                /* 1 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
                /* 2 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
                /* 3 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
                /* 4 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
                /* 5 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
                /* 6 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
                /* 7 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
                /* 8 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
                /* 9 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
                /* 10 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
                /* 11 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
                /* 12 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
                /* 13 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
                /* 14 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
                /* 15 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
            },
            /* 5 */
            {
                /* 0 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
                /* 1 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
                /* 2 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
                /* 3 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
                /* 4 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
                /* 5 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
                /* 6 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
                /* 7 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
                /* 8 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
                /* 9 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
                /* 10 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
                /* 11 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
                /* 12 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
                /* 13 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
                /* 14 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
                /* 15 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
            },
            /* 6 */
            {
                /* 0 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
                /* 1 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
                /* 2 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
                /* 3 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
                /* 4 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
                /* 5 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
                /* 6 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
                /* 7 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
                /* 8 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
                /* 9 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
                /* 10 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
                /* 11 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
                /* 12 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
                /* 13 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
                /* 14 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
                /* 15 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
            },
            /* 7 */
            {
                /* 0 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
                /* 1 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
                /* 2 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
                /* 3 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
                /* 4 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
                /* 5 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
                /* 6 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
                /* 7 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
                /* 8 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
                /* 9 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
                /* 10 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
                /* 11 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
                /* 12 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
                /* 13 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
                /* 14 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
                /* 15 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
            },
            /* 8 */
            {
                /* 0 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
                /* 1 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
                /* 2 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
                /* 3 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
                /* 4 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
                /* 5 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
                /* 6 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
                /* 7 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
                /* 8 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
                /* 9 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
                /* 10 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
                /* 11 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
                /* 12 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
                /* 13 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
                /* 14 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
                /* 15 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
            },
            /* 9 */
            {
                /* 0 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
                /* 1 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
                /* 2 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
                /* 3 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
                /* 4 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
                /* 5 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
                /* 6 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
                /* 7 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
                /* 8 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
                /* 9 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
                /* 10 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
                /* 11 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
                /* 12 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
                /* 13 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
                /* 14 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
                /* 15 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
            },
            /* 10 */
            {
                /* 0 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
                /* 1 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
                /* 2 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
                /* 3 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
                /* 4 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
                /* 5 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
                /* 6 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
                /* 7 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
                /* 8 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
                /* 9 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
                /* 10 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
                /* 11 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
                /* 12 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
                /* 13 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
                /* 14 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
                /* 15 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
            },
            /* 11 */
            {
                /* 0 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
                /* 1 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
                /* 2 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
                /* 3 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
                /* 4 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
                /* 5 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
                /* 6 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
                /* 7 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
                /* 8 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
                /* 9 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
                /* 10 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
                /* 11 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
                /* 12 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
                /* 13 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
                /* 14 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
                /* 15 */
                {
                    /* nHistogramWeight[63] */
                    {
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 0 - 31*/
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*32 - 62*/
                    },
                },
            },
        },
        /* tHrAuto */
        {
            /* nHrGrpNum */
            1,
            /* nHrRefVal[4] */
            {1024, /*0 - 0*/},
            /* tHrParam */
            {
                /* 0 */
                {
                    /* nLocalFactor[16] */
                    {80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, /*0 - 11*/},
                    /* nRltmStrength[16] */
                    {64, 60, 52, 44, 44, 42, 42, 42, 40, 40, 40, 40, /*0 - 11*/},
                    /* nContrastStrength[16] */
                    {64, 60, 48, 48, 48, 44, 44, 38, 38, 32, 32, 32, /*0 - 11*/},
                },
            },
        },
    },
};

static AX_ISP_IQ_DEMOSAIC_PARAM_T demosaic_param_sdr = {
    /* nDemosaicEn */
    1,
    /* nFcCorEnable */
    1,
    /* nAutoMode */
    0,
    /* nRefMode */
    0,
    /* nMode */
    0,
    /* tManualParam */
    {
        /* nEdgeDirectEstStrength */
        64,
        /* nFcCorUvLevel */
        16,
        /* nFcCorLevel */
        0,
    },
    /* tAutoParam */
    {
        /* nParamGrpNum */
        2,
        /* nRefVal[16] */
        {1024, 2048, /*0 - 1*/},
        /* nEdgeDirectEstStrength[16] */
        {64, 64, /*0 - 1*/},
        /* nFcCorUvLevel[16] */
        {16, 16, /*0 - 1*/},
        /* nFcCorLevel[16] */
        {0, 0, /*0 - 1*/},
    },
};

static AX_ISP_IQ_GIC_PARAM_T gic_param_sdr = {
    /* nGicEnable */
    0,
    /* nAutoMode */
    0,
    /* nRefMode */
    0,
    /* tManualParam */
    {
        /* nGicStrengthLut[9] */
        {128, 128, 128, 128, 128, 128, 128, 128, 128, /*0 - 8*/},
    },
    /* tAutoParam */
    {
        /* nParamGrpNum */
        2,
        /* nRefVal[16] */
        {1024, 2048, /*0 - 1*/},
        /* nGicStrengthLut[16][9] */
        {
            {128, 128, 128, 128, 128, 128, 128, 128, 128, /*0 - 8*/},
            {128, 128, 128, 128, 128, 128, 128, 128, 128, /*0 - 8*/},
        },
    },
};

static AX_ISP_IQ_DEPURPLE_PARAM_T depurple_param_sdr = {
    /* nEnable */
    1,
    /* nAutoMode */
    0,
    /* nRefMode */
    0,
    /* nDepurpleMode */
    1,
    /* tDetParam */
    {
        /* nDetMode */
        0,
        /* nDetEdgeSlope */
        256,
        /* nDetEdgeOffset */
        0,
    },
    /* tCmpParam */
    {
        /* nCompUvLevel */
        128,
    },
    /* tManualParam */
    {
        /* nCompStrength */
        128,
        /* nCompTargetLuma[8] */
        {0, 16, 32, 48, 64, 80, 96, 128, /*0 - 7*/},
        /* nCompTargetHue[16] */
        {90, 90, 90, 64, 32, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 64, /*0 - 15*/},
        /* nCompTargetSat[6] */
        {0, 64, 96, 128, 128, 128, /*0 - 5*/},
        /* nDetStrength */
        96,
        /* nDetColorCenter[4][3] */
        {
            {0, 0, 0, /*0 - 2*/},
            {0, 0, 0, /*0 - 2*/},
            {0, 0, 0, /*0 - 2*/},
            {0, 0, 0, /*0 - 2*/},
        },
        /* nDetColorRadius[4][3] */
        {
            {0, 0, 0, /*0 - 2*/},
            {0, 0, 0, /*0 - 2*/},
            {0, 0, 0, /*0 - 2*/},
            {0, 0, 0, /*0 - 2*/},
        },
        /* nDetGetcompcolorGrad[4][3] */
        {
            {4, 4, 4, /*0 - 2*/},
            {4, 4, 4, /*0 - 2*/},
            {4, 4, 4, /*0 - 2*/},
            {4, 4, 4, /*0 - 2*/},
        },
    },
    /* tAutoParam */
    {
        /* nParamGrpNum */
        16,
        /* nRefVal[16] */
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 15*/},
        /* nCompStrength[16] */
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 15*/},
        /* nCompTargetLuma[16][8] */
        {
            {0, 16, 32, 48, 64, 80, 96, 128, /*0 - 7*/},
            {0, 16, 32, 48, 64, 80, 96, 128, /*0 - 7*/},
            {0, 16, 32, 48, 64, 80, 96, 128, /*0 - 7*/},
            {0, 16, 32, 48, 64, 80, 96, 128, /*0 - 7*/},
        },
        /* nCompTargetHue[16][16] */
        {
            {128, 8, 16, 24, 32, 40, 48, 56, 64, 72, 80, 88, 96, 104, 112, 128, /*0 - 15*/},
            {128, 8, 16, 24, 32, 40, 48, 56, 64, 72, 80, 88, 96, 104, 112, 128, /*0 - 15*/},
            {128, 8, 16, 24, 32, 40, 48, 56, 64, 72, 80, 88, 96, 104, 112, 128, /*0 - 15*/},
            {128, 8, 16, 24, 32, 40, 48, 56, 64, 72, 80, 88, 96, 104, 112, 128, /*0 - 15*/},
        },
        /* nCompTargetSat[16][6] */
        {
            {0, 64, 96, 128, 128, 128, /*0 - 5*/},
            {0, 64, 96, 128, 128, 128, /*0 - 5*/},
            {0, 64, 96, 128, 128, 128, /*0 - 5*/},
            {0, 64, 96, 128, 128, 128, /*0 - 5*/},
        },
        /* nDetStrength[16] */
        {96, /*0 - 0*/},
        /* nDetColorCenter[16][4][3] */
        {
            {
                {0, 0, 0, /*0 - 2*/},
                {0, 0, 0, /*0 - 2*/},
                {0, 0, 0, /*0 - 2*/},
                {0, 0, 0, /*0 - 2*/},
            },
        },
        /* nDetColorRadius[16][4][3] */
        {
            {
                {0, 0, 0, /*0 - 2*/},
                {0, 0, 0, /*0 - 2*/},
                {0, 0, 0, /*0 - 2*/},
                {0, 0, 0, /*0 - 2*/},
            },
        },
        /* nDetGetcompcolorGrad[16][4][3] */
        {
            {
                {4, 4, 4, /*0 - 2*/},
                {4, 4, 4, /*0 - 2*/},
                {4, 4, 4, /*0 - 2*/},
                {4, 4, 4, /*0 - 2*/},
            },
        },
    },
};

static AX_ISP_IQ_CC_PARAM_T cc_param_sdr = {
    /* nCcEn */
    1,
    /* nCcMode */
    1,
    /* nAutoMode */
    1,
    /* nRefMode */
    1,
    /* nLumaRatio[2] */
    {32, 32, /*0 - 1*/},
    /* tManualParam */
    {
        /* nCcmCtrlLevel */
        256,
        /* nCcmSat */
        0,
        /* nCcmHue */
        0,
        /* nCcmMatrix[6] */
        {-241, -22, -152, -17, -18, -302, /*0 - 5*/},
        /* nXcmCtrlLevel[16] */
        {256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, /*0 - 15*/},
        /* nXcmSat[16] */
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 15*/},
        /* nXcmHue[16] */
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 15*/},
    },
    /* tAutoParam */
    {
        /* nLightSourceRatio */
        0,
        /* tColorTempAuto */
        {
            /* nCtNum */
            7,
            /* nRefValCt[12] */
            {2300, 2800, 3800, 4150, 5000, 6500, 7500, /*0 - 6*/},
            /* nLuxGainNum */
            2,
            /* nRefValLuxGain[12][5] */
            {
                {1024, 65535, /*0 - 1*/},
                {1024, 65535, /*0 - 1*/},
                {1024, 65535, /*0 - 1*/},
                {1024, 65535, /*0 - 1*/},
                {1024, 65535, /*0 - 1*/},
                {1024, 65535, /*0 - 1*/},
                {1024, 65535, /*0 - 1*/},
            },
            /* nCcmCtrlLevel[12][5] */
            {
                {256, 256, /*0 - 1*/},
                {256, 256, /*0 - 1*/},
                {256, 256, /*0 - 1*/},
                {256, 256, /*0 - 1*/},
                {256, 256, /*0 - 1*/},
                {256, 256, /*0 - 1*/},
                {256, 256, /*0 - 1*/},
            },
            /* nCcmSat[12][5] */
            {
                {0, 0, /*0 - 1*/},
                {0, 0, /*0 - 1*/},
                {0, 0, /*0 - 1*/},
                {0, 0, /*0 - 1*/},
                {0, 0, /*0 - 1*/},
                {0, 0, /*0 - 1*/},
                {0, 0, /*0 - 1*/},
            },
            /* nCcmHue[12][5] */
            {
                {0, 0, /*0 - 1*/},
                {0, 0, /*0 - 1*/},
                {0, 0, /*0 - 1*/},
                {0, 0, /*0 - 1*/},
                {0, 0, /*0 - 1*/},
                {0, 0, /*0 - 1*/},
                {0, 0, /*0 - 1*/},
            },
            /* nCcmMatrix[12][5][6] */
            {
                /* nCcmMatrix 0 */
                {
                    {96, -213, -259, 21, -214, -646, /*0 - 5*/},
                    {96, -213, -259, 21, -214, -646, /*0 - 5*/},
                },
                /* nCcmMatrix 1 */
                {
                    {23, -149, -206, -36, -98, -492, /*0 - 5*/},
                    {23, -149, -206, -36, -98, -492, /*0 - 5*/},
                },
                /* nCcmMatrix 2 */
                {
                    {-194, -31, -147, -25, -18, -312, /*0 - 5*/},
                    {-194, -31, -147, -25, -18, -312, /*0 - 5*/},
                },
                /* nCcmMatrix 3 */
                {
                    {-286, -17, -161, -13, -21, -296, /*0 - 5*/},
                    {-286, -17, -161, -13, -21, -296, /*0 - 5*/},
                },
                /* nCcmMatrix 4 */
                {
                    {-205, -8, -98, -76, 3, -254, /*0 - 5*/},
                    {-205, -8, -98, -76, 3, -254, /*0 - 5*/},
                },
                /* nCcmMatrix 5 */
                {
                    {-168, -40, -139, -87, -12, -240, /*0 - 5*/},
                    {-168, -40, -139, -87, -12, -240, /*0 - 5*/},
                },
                /* nCcmMatrix 6 */
                {
                    {-234, 2, -91, -70, 4, -207, /*0 - 5*/},
                    {-234, 2, -91, -70, 4, -207, /*0 - 5*/},
                },
            },
            /* nXcmCtrlLevel[12][5][16] */
            {
                /* nXcmCtrlLevel 0 */
                {
                    {256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, /*0 - 15*/},
                    {256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, /*0 - 15*/},
                },
                /* nXcmCtrlLevel 1 */
                {
                    {256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, /*0 - 15*/},
                    {256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, /*0 - 15*/},
                },
                /* nXcmCtrlLevel 2 */
                {
                    {256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, /*0 - 15*/},
                    {256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, /*0 - 15*/},
                },
                /* nXcmCtrlLevel 3 */
                {
                    {256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, /*0 - 15*/},
                    {256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, /*0 - 15*/},
                },
                /* nXcmCtrlLevel 4 */
                {
                    {256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, /*0 - 15*/},
                    {256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, /*0 - 15*/},
                },
                /* nXcmCtrlLevel 5 */
                {
                    {256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, /*0 - 15*/},
                    {256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, /*0 - 15*/},
                },
                /* nXcmCtrlLevel 6 */
                {
                    {256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, /*0 - 15*/},
                    {256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, /*0 - 15*/},
                },
            },
            /* nXcmSat[12][5][16] */
            {
                /* nXcmSat 0 */
                {
                    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 15*/},
                    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 15*/},
                },
                /* nXcmSat 1 */
                {
                    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 15*/},
                    {0, 0, 0, 0, 0, 0, -6, -6, -12, -12, -6, 0, 0, 0, 0, 0, /*0 - 15*/},
                },
                /* nXcmSat 2 */
                {
                    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 15*/},
                    {0, 0, 0, 0, 0, 0, -12, -24, -24, -24, -12, 0, 0, 0, 0, 0, /*0 - 15*/},
                },
                /* nXcmSat 3 */
                {
                    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 15*/},
                    {0, 0, 0, 0, 0, 0, -6, -12, -12, -6, 0, 0, 0, 0, 0, 0, /*0 - 15*/},
                },
                /* nXcmSat 4 */
                {
                    {0, 0, 0, 0, 0, 0, 0, 0, 4, 8, 8, 8, 4, 0, 0, 0, /*0 - 15*/},
                    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 15*/},
                },
                /* nXcmSat 5 */
                {
                    {0, 0, 0, 0, 0, 0, 0, 0, 4, 8, 8, 8, 4, 0, 0, 0, /*0 - 15*/},
                    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 15*/},
                },
                /* nXcmSat 6 */
                {
                    {0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 8, 8, 4, 0, 0, 0, /*0 - 15*/},
                    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 15*/},
                },
            },
            /* nXcmHue[12][5][16] */
            {
                /* nXcmHue 0 */
                {
                    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 15*/},
                    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 15*/},
                },
                /* nXcmHue 1 */
                {
                    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 15*/},
                    {0, 0, 0, 0, 0, -160, -320, -160, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 15*/},
                },
                /* nXcmHue 2 */
                {
                    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 15*/},
                    {0, 0, 0, 0, 0, -320, -640, -320, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 15*/},
                },
                /* nXcmHue 3 */
                {
                    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 15*/},
                    {0, 0, 0, 0, 0, -160, -320, -160, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 15*/},
                },
                /* nXcmHue 4 */
                {
                    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 15*/},
                    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 15*/},
                },
                /* nXcmHue 5 */
                {
                    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 15*/},
                    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 15*/},
                },
                /* nXcmHue 6 */
                {
                    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 15*/},
                    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 15*/},
                },
            },
        },
        /* tLightSourceAuto */
        {
            /* nLightSource */
            0,
            /* nLightSourceNum */
            0,
            /* nCcmCtrlLevel[12] */
            {0, /*0 - 0*/},
            /* nCcmSat[12] */
            {0, /*0 - 0*/},
            /* nCcmHue[12] */
            {0, /*0 - 0*/},
            /* nCcmMatrix[12][6] */
            {
                {0, 0, 0, 0, 0, 0, /*0 - 5*/},
            },
            /* nXcmCtrlLevel[12][16] */
            {
                {256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, /*0 - 15*/},
            },
            /* nXcmSat[12][16] */
            {
                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 15*/},
            },
            /* nXcmHue[12][16] */
            {
                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 15*/},
            },
        },
    },
};

static AX_ISP_IQ_3DLUT_PARAM_T isp_3dlut_param_sdr = {
    /* n3DlutEn */
    0,
    /* nAutoMode */
    0,
    /* nRefMode */
    1,
    /* nConvergeSpeed */
    5,
    /* nGainTrigger */
    10240,
    /* nLuxTrigger */
    10240,
    /* nCctTrigger */
    50,
    /* eLutSizeType */
    /* 17:AX_ISP_3DLUT_LUT_SIZE_TYPE_17, 21:AX_ISP_3DLUT_LUT_SIZE_TYPE_21, 27:AX_ISP_3DLUT_LUT_SIZE_TYPE_27 */
    AX_ISP_3DLUT_LUT_SIZE_TYPE_17,
    /* tManualParam */
    {
        /* tAnchorTbl */
        {
            /* nHueTbl[16][24] */
            {
                {0, 1920, 3840, 5760, 7680, 9600, 11520, 13440, 15360, 17280, 19200, 21120, 23040, 24960, 26880, 28800, 30720, 32640, 34560, 36480, 38400, 40320, 42240, 44160, /*0 - 23*/},
                {0, 1920, 3840, 5760, 7680, 9600, 11520, 13440, 15360, 17280, 19200, 21120, 23040, 24960, 26880, 28800, 30720, 32640, 34560, 36480, 38400, 40320, 42240, 44160, /*0 - 23*/},
                {0, 1920, 3840, 5760, 7680, 9600, 11520, 13440, 15360, 17280, 19200, 21120, 23040, 24960, 26880, 28800, 30720, 32640, 34560, 36480, 38400, 40320, 42240, 44160, /*0 - 23*/},
                {0, 1920, 3840, 5760, 7680, 9600, 11520, 13440, 15360, 17280, 19200, 21120, 23040, 24960, 26880, 28800, 30720, 32640, 34560, 36480, 38400, 40320, 42240, 44160, /*0 - 23*/},
                {0, 1920, 3840, 5760, 7680, 9600, 11520, 13440, 15360, 17280, 19200, 21120, 23040, 24960, 26880, 28800, 30720, 32640, 34560, 36480, 38400, 40320, 42240, 44160, /*0 - 23*/},
                {0, 1920, 3840, 5760, 7680, 9600, 11520, 13440, 15360, 17280, 19200, 21120, 23040, 24960, 26880, 28800, 30720, 32640, 34560, 36480, 38400, 40320, 42240, 44160, /*0 - 23*/},
                {0, 1920, 3840, 5760, 7680, 9600, 11520, 13440, 15360, 17280, 19200, 21120, 23040, 24960, 26880, 28800, 30720, 32640, 34560, 36480, 38400, 40320, 42240, 44160, /*0 - 23*/},
                {0, 1920, 3840, 5760, 7680, 9600, 11520, 13440, 15360, 17280, 19200, 21120, 23040, 24960, 26880, 28800, 30720, 32640, 34560, 36480, 38400, 40320, 42240, 44160, /*0 - 23*/},
                {0, 1920, 3840, 5760, 7680, 9600, 11520, 13440, 15360, 17280, 19200, 21120, 23040, 24960, 26880, 28800, 30720, 32640, 34560, 36480, 38400, 40320, 42240, 44160, /*0 - 23*/},
                {0, 1920, 3840, 5760, 7680, 9600, 11520, 13440, 15360, 17280, 19200, 21120, 23040, 24960, 26880, 28800, 30720, 32640, 34560, 36480, 38400, 40320, 42240, 44160, /*0 - 23*/},
                {0, 1920, 3840, 5760, 7680, 9600, 11520, 13440, 15360, 17280, 19200, 21120, 23040, 24960, 26880, 28800, 30720, 32640, 34560, 36480, 38400, 40320, 42240, 44160, /*0 - 23*/},
                {0, 1920, 3840, 5760, 7680, 9600, 11520, 13440, 15360, 17280, 19200, 21120, 23040, 24960, 26880, 28800, 30720, 32640, 34560, 36480, 38400, 40320, 42240, 44160, /*0 - 23*/},
                {0, 1920, 3840, 5760, 7680, 9600, 11520, 13440, 15360, 17280, 19200, 21120, 23040, 24960, 26880, 28800, 30720, 32640, 34560, 36480, 38400, 40320, 42240, 44160, /*0 - 23*/},
                {0, 1920, 3840, 5760, 7680, 9600, 11520, 13440, 15360, 17280, 19200, 21120, 23040, 24960, 26880, 28800, 30720, 32640, 34560, 36480, 38400, 40320, 42240, 44160, /*0 - 23*/},
                {0, 1920, 3840, 5760, 7680, 9600, 11520, 13440, 15360, 17280, 19200, 21120, 23040, 24960, 26880, 28800, 30720, 32640, 34560, 36480, 38400, 40320, 42240, 44160, /*0 - 23*/},
                {0, 1920, 3840, 5760, 7680, 9600, 11520, 13440, 15360, 17280, 19200, 21120, 23040, 24960, 26880, 28800, 30720, 32640, 34560, 36480, 38400, 40320, 42240, 44160, /*0 - 23*/},
            },
            /* nSatTbl[16][24] */
            {
                {4096, 4096, 4096, 4096, 4096, 4096, 4096, 4096, 4096, 4096, 4096, 4096, 4096, 4096, 4096, 4096, 4096, 4096, 4096, 4096, 4096, 4096, 4096, 4096, /*0 - 23*/},
                {8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192, /*0 - 23*/},
                {12288, 12288, 12288, 12288, 12288, 12288, 12288, 12288, 12288, 12288, 12288, 12288, 12288, 12288, 12288, 12288, 12288, 12288, 12288, 12288, 12288, 12288, 12288, 12288, /*0 - 23*/},
                {16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*0 - 23*/},
                {20480, 20480, 20480, 20480, 20480, 20480, 20480, 20480, 20480, 20480, 20480, 20480, 20480, 20480, 20480, 20480, 20480, 20480, 20480, 20480, 20480, 20480, 20480, 20480, /*0 - 23*/},
                {24576, 24576, 24576, 24576, 24576, 24576, 24576, 24576, 24576, 24576, 24576, 24576, 24576, 24576, 24576, 24576, 24576, 24576, 24576, 24576, 24576, 24576, 24576, 24576, /*0 - 23*/},
                {28672, 28672, 28672, 28672, 28672, 28672, 28672, 28672, 28672, 28672, 28672, 28672, 28672, 28672, 28672, 28672, 28672, 28672, 28672, 28672, 28672, 28672, 28672, 28672, /*0 - 23*/},
                {32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, /*0 - 23*/},
                {36864, 36864, 36864, 36864, 36864, 36864, 36864, 36864, 36864, 36864, 36864, 36864, 36864, 36864, 36864, 36864, 36864, 36864, 36864, 36864, 36864, 36864, 36864, 36864, /*0 - 23*/},
                {40960, 40960, 40960, 40960, 40960, 40960, 40960, 40960, 40960, 40960, 40960, 40960, 40960, 40960, 40960, 40960, 40960, 40960, 40960, 40960, 40960, 40960, 40960, 40960, /*0 - 23*/},
                {45056, 45056, 45056, 45056, 45056, 45056, 45056, 45056, 45056, 45056, 45056, 45056, 45056, 45056, 45056, 45056, 45056, 45056, 45056, 45056, 45056, 45056, 45056, 45056, /*0 - 23*/},
                {49152, 49152, 49152, 49152, 49152, 49152, 49152, 49152, 49152, 49152, 49152, 49152, 49152, 49152, 49152, 49152, 49152, 49152, 49152, 49152, 49152, 49152, 49152, 49152, /*0 - 23*/},
                {53248, 53248, 53248, 53248, 53248, 53248, 53248, 53248, 53248, 53248, 53248, 53248, 53248, 53248, 53248, 53248, 53248, 53248, 53248, 53248, 53248, 53248, 53248, 53248, /*0 - 23*/},
                {57344, 57344, 57344, 57344, 57344, 57344, 57344, 57344, 57344, 57344, 57344, 57344, 57344, 57344, 57344, 57344, 57344, 57344, 57344, 57344, 57344, 57344, 57344, 57344, /*0 - 23*/},
                {61440, 61440, 61440, 61440, 61440, 61440, 61440, 61440, 61440, 61440, 61440, 61440, 61440, 61440, 61440, 61440, 61440, 61440, 61440, 61440, 61440, 61440, 61440, 61440, /*0 - 23*/},
                {65536, 65536, 65536, 65536, 65536, 65536, 65536, 65536, 65536, 65536, 65536, 65536, 65536, 65536, 65536, 65536, 65536, 65536, 65536, 65536, 65536, 65536, 65536, 65536, /*0 - 23*/},
            },
        },
    },
    /* tAutoParam */
    {
        /* nRefListNum */
        2,
        /* tRefTbl[12] */
        {
            /* 0 */
            {
                /* nRefStartVal */
                1,
                /* nRefEndVal */
                1,
                /* nCctListNum */
                2,
                /* tCctTbl[16] */
                {
                    /* 0 */
                    {
                        /* nCctStart */
                        1,
                        /* nCctEnd */
                        1,
                        /* tAnchorTbl */
                        {
                            /* nHueTbl[16][24] */
                            {
                                {0, 1920, 3840, 5760, 7680, 9600, 11520, 13440, 15360, 17280, 19200, 21120, 23040, 24960, 26880, 28800, 30720, 32640, 34560, 36480, 38400, 40320, 42240, 44160, /*0 - 23*/},
                                {0, 1920, 3840, 5760, 7680, 9600, 11520, 13440, 15360, 17280, 19200, 21120, 23040, 24960, 26880, 28800, 30720, 32640, 34560, 36480, 38400, 40320, 42240, 44160, /*0 - 23*/},
                                {0, 1920, 3840, 5760, 7680, 9600, 11520, 13440, 15360, 17280, 19200, 21120, 23040, 24960, 26880, 28800, 30720, 32640, 34560, 36480, 38400, 40320, 42240, 44160, /*0 - 23*/},
                                {0, 1920, 3840, 5760, 7680, 9600, 11520, 13440, 15360, 17280, 19200, 21120, 23040, 24960, 26880, 28800, 30720, 32640, 34560, 36480, 38400, 40320, 42240, 44160, /*0 - 23*/},
                                {0, 1920, 3840, 5760, 7680, 9600, 11520, 13440, 15360, 17280, 19200, 21120, 23040, 24960, 26880, 28800, 30720, 32640, 34560, 36480, 38400, 40320, 42240, 44160, /*0 - 23*/},
                                {0, 1920, 3840, 5760, 7680, 9600, 11520, 13440, 15360, 17280, 19200, 21120, 23040, 24960, 26880, 28800, 30720, 32640, 34560, 36480, 38400, 40320, 42240, 44160, /*0 - 23*/},
                                {0, 1920, 3840, 5760, 7680, 9600, 11520, 13440, 15360, 17280, 19200, 21120, 23040, 24960, 26880, 28800, 30720, 32640, 34560, 36480, 38400, 40320, 42240, 44160, /*0 - 23*/},
                                {0, 1920, 3840, 5760, 7680, 9600, 11520, 13440, 15360, 17280, 19200, 21120, 23040, 24960, 26880, 28800, 30720, 32640, 34560, 36480, 38400, 40320, 42240, 44160, /*0 - 23*/},
                                {0, 1920, 3840, 5760, 7680, 9600, 11520, 13440, 15360, 17280, 19200, 21120, 23040, 24960, 26880, 28800, 30720, 32640, 34560, 36480, 38400, 40320, 42240, 44160, /*0 - 23*/},
                                {0, 1920, 3840, 5760, 7680, 9600, 11520, 13440, 15360, 17280, 19200, 21120, 23040, 24960, 26880, 28800, 30720, 32640, 34560, 36480, 38400, 40320, 42240, 44160, /*0 - 23*/},
                                {0, 1920, 3840, 5760, 7680, 9600, 11520, 13440, 15360, 17280, 19200, 21120, 23040, 24960, 26880, 28800, 30720, 32640, 34560, 36480, 38400, 40320, 42240, 44160, /*0 - 23*/},
                                {0, 1920, 3840, 5760, 7680, 9600, 11520, 13440, 15360, 17280, 19200, 21120, 23040, 24960, 26880, 28800, 30720, 32640, 34560, 36480, 38400, 40320, 42240, 44160, /*0 - 23*/},
                                {0, 1920, 3840, 5760, 7680, 9600, 11520, 13440, 15360, 17280, 19200, 21120, 23040, 24960, 26880, 28800, 30720, 32640, 34560, 36480, 38400, 40320, 42240, 44160, /*0 - 23*/},
                                {0, 1920, 3840, 5760, 7680, 9600, 11520, 13440, 15360, 17280, 19200, 21120, 23040, 24960, 26880, 28800, 30720, 32640, 34560, 36480, 38400, 40320, 42240, 44160, /*0 - 23*/},
                                {0, 1920, 3840, 5760, 7680, 9600, 11520, 13440, 15360, 17280, 19200, 21120, 23040, 24960, 26880, 28800, 30720, 32640, 34560, 36480, 38400, 40320, 42240, 44160, /*0 - 23*/},
                                {0, 1920, 3840, 5760, 7680, 9600, 11520, 13440, 15360, 17280, 19200, 21120, 23040, 24960, 26880, 28800, 30720, 32640, 34560, 36480, 38400, 40320, 42240, 44160, /*0 - 23*/},
                            },
                            /* nSatTbl[16][24] */
                            {
                                {4096, 4096, 4096, 4096, 4096, 4096, 4096, 4096, 4096, 4096, 4096, 4096, 4096, 4096, 4096, 4096, 4096, 4096, 4096, 4096, 4096, 4096, 4096, 4096, /*0 - 23*/},
                                {8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192, /*0 - 23*/},
                                {12288, 12288, 12288, 12288, 12288, 12288, 12288, 12288, 12288, 12288, 12288, 12288, 12288, 12288, 12288, 12288, 12288, 12288, 12288, 12288, 12288, 12288, 12288, 12288, /*0 - 23*/},
                                {16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, 16384, /*0 - 23*/},
                                {20480, 20480, 20480, 20480, 20480, 20480, 20480, 20480, 20480, 20480, 20480, 20480, 20480, 20480, 20480, 20480, 20480, 20480, 20480, 20480, 20480, 20480, 20480, 20480, /*0 - 23*/},
                                {24576, 24576, 24576, 24576, 24576, 24576, 24576, 24576, 24576, 24576, 24576, 24576, 24576, 24576, 24576, 24576, 24576, 24576, 24576, 24576, 24576, 24576, 24576, 24576, /*0 - 23*/},
                                {28672, 28672, 28672, 28672, 28672, 28672, 28672, 28672, 28672, 28672, 28672, 28672, 28672, 28672, 28672, 28672, 28672, 28672, 28672, 28672, 28672, 28672, 28672, 28672, /*0 - 23*/},
                                {32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, 32768, /*0 - 23*/},
                                {36864, 36864, 36864, 36864, 36864, 36864, 36864, 36864, 36864, 36864, 36864, 36864, 36864, 36864, 36864, 36864, 36864, 36864, 36864, 36864, 36864, 36864, 36864, 36864, /*0 - 23*/},
                                {40960, 40960, 40960, 40960, 40960, 40960, 40960, 40960, 40960, 40960, 40960, 40960, 40960, 40960, 40960, 40960, 40960, 40960, 40960, 40960, 40960, 40960, 40960, 40960, /*0 - 23*/},
                                {45056, 45056, 45056, 45056, 45056, 45056, 45056, 45056, 45056, 45056, 45056, 45056, 45056, 45056, 45056, 45056, 45056, 45056, 45056, 45056, 45056, 45056, 45056, 45056, /*0 - 23*/},
                                {49152, 49152, 49152, 49152, 49152, 49152, 49152, 49152, 49152, 49152, 49152, 49152, 49152, 49152, 49152, 49152, 49152, 49152, 49152, 49152, 49152, 49152, 49152, 49152, /*0 - 23*/},
                                {53248, 53248, 53248, 53248, 53248, 53248, 53248, 53248, 53248, 53248, 53248, 53248, 53248, 53248, 53248, 53248, 53248, 53248, 53248, 53248, 53248, 53248, 53248, 53248, /*0 - 23*/},
                                {57344, 57344, 57344, 57344, 57344, 57344, 57344, 57344, 57344, 57344, 57344, 57344, 57344, 57344, 57344, 57344, 57344, 57344, 57344, 57344, 57344, 57344, 57344, 57344, /*0 - 23*/},
                                {61440, 61440, 61440, 61440, 61440, 61440, 61440, 61440, 61440, 61440, 61440, 61440, 61440, 61440, 61440, 61440, 61440, 61440, 61440, 61440, 61440, 61440, 61440, 61440, /*0 - 23*/},
                                {65536, 65536, 65536, 65536, 65536, 65536, 65536, 65536, 65536, 65536, 65536, 65536, 65536, 65536, 65536, 65536, 65536, 65536, 65536, 65536, 65536, 65536, 65536, 65536, /*0 - 23*/},
                            },
                        },
                    },
                    /* 1 */
                    {
                        /* nCctStart */
                        0,
                        /* nCctEnd */
                        0,
                        /* tAnchorTbl */
                        {
                            /* nHueTbl[16][24] */
                            {
                                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 23*/},
                                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 23*/},
                                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 23*/},
                                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 23*/},
                                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 23*/},
                                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 23*/},
                                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 23*/},
                                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 23*/},
                                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 23*/},
                                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 23*/},
                                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 23*/},
                                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 23*/},
                                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 23*/},
                                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 23*/},
                                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 23*/},
                                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 23*/},
                            },
                            /* nSatTbl[16][24] */
                            {
                                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 23*/},
                                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 23*/},
                                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 23*/},
                                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 23*/},
                                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 23*/},
                                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 23*/},
                                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 23*/},
                                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 23*/},
                                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 23*/},
                                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 23*/},
                                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 23*/},
                                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 23*/},
                                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 23*/},
                                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 23*/},
                                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 23*/},
                                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 23*/},
                            },
                        },
                    },
                },
            },
            /* 1 */
            {
                /* nRefStartVal */
                0,
                /* nRefEndVal */
                0,
                /* nCctListNum */
                0,
                /* tCctTbl[16] */
                {
                    /* 0 */
                    {
                        /* nCctStart */
                        0,
                        /* nCctEnd */
                        0,
                        /* tAnchorTbl */
                        {
                            /* nHueTbl[16][24] */
                            {
                                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 23*/},
                                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 23*/},
                                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 23*/},
                                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 23*/},
                                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 23*/},
                                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 23*/},
                                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 23*/},
                                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 23*/},
                                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 23*/},
                                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 23*/},
                                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 23*/},
                                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 23*/},
                                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 23*/},
                                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 23*/},
                                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 23*/},
                                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 23*/},
                            },
                            /* nSatTbl[16][24] */
                            {
                                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 23*/},
                                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 23*/},
                                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 23*/},
                                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 23*/},
                                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 23*/},
                                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 23*/},
                                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 23*/},
                                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 23*/},
                                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 23*/},
                                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 23*/},
                                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 23*/},
                                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 23*/},
                                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 23*/},
                                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 23*/},
                                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 23*/},
                                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 23*/},
                            },
                        },
                    },
                },
            },
        },
    },
};

static AX_ISP_IQ_GAMMA_PARAM_T gamma_param_sdr = {
    /* nGammaEn */
    1,
    /* nAutoMode */
    1,
    /* nRefMode */
    0,
    /* eGammaMode */
    /* 0:AX_ISP_GAM_USER_GAMMA, 1:AX_ISP_GAM_PRESET_GAMMA */
    AX_ISP_GAM_PRESET_GAMMA,
    /* ePresetGammaType */
    /* 0:AX_ISP_GAM_LINEAR, 1:AX_ISP_GAM_BT709, 2:AX_ISP_GAM_SRGB, 10:AX_ISP_GAM_AX_GAM0, 11:AX_ISP_GAM_AX_GAM1, 12:AX_ISP_GAM_AX_GAM2 */
    AX_ISP_GAM_AX_GAM2,
    /* tManualParam */
    {
        /* eLutMode */
        /* 0:AX_ISP_GAM_LUT_LINEAR, 1:AX_ISP_GAM_EXPONENTIAL */
        AX_ISP_GAM_EXPONENTIAL,
        /* tGammaLut */
        {
            /* nLut[129] */
            {
                0, 13, 26, 40, 53, 67, 81, 95, 109, 123, 137, 152, 166, 181, 195, 210, 225, 240, 255, 270, 285, 301, 316, 332, 348, 364, 379, 395, 412, 428, 444, 461,  /* 0 - 31*/
                477, 511, 544, 579, 613, 648, 684, 720, 756, 793, 830, 868, 906, 944, 983, 1022, 1061, 1142, 1224, 1307, 1392, 1479, 1567, 1657, 1748, 1841, 1935, 2030, 2127, 2226, 2326, 2427,  /* 32 - 63*/
                2530, 2734, 2934, 3129, 3321, 3508, 3691, 3871, 4047, 4220, 4389, 4555, 4718, 4878, 5034, 5188, 5340, 5634, 5918, 6193, 6459, 6717, 6966, 7208, 7442, 7669, 7890, 8105, 8313, 8516, 8713, 8905,  /* 64 - 95*/
                9092, 9451, 9792, 10116, 10424, 10718, 10998, 11265, 11520, 11764, 11997, 12220, 12433, 12638, 12834, 13021, 13202, 13540, 13852, 14139, 14405, 14650, 14877, 15086, 15280, 15459, 15624, 15777, 15918, 16049, 16169, 16280,  /* 96 - 127*/
                16383, /*128 - 128*/
            },
        },
    },
    /* tAutoParam */
    {
        /* eLutMode */
        /* 0:AX_ISP_GAM_LUT_LINEAR, 1:AX_ISP_GAM_EXPONENTIAL */
        AX_ISP_GAM_EXPONENTIAL,
        /* nParamGrpNum */
        3,
        /* nRefValStart[3] */
        {0, 10, 200, /*0 - 2*/},
        /* nRefValEnd[3] */
        {10, 200, 1000, /*0 - 2*/},
        /* tGammaLut[3] */
        {
            /* 0 */
            {
                /* nLut[129] */
                {
                    0, 13, 26, 40, 53, 67, 81, 95, 109, 123, 137, 152, 166, 181, 195, 210, 225, 240, 255, 270, 285, 301, 316, 332, 348, 364, 379, 395, 412, 428, 444, 461,  /* 0 - 31*/
                    477, 511, 544, 579, 613, 648, 684, 720, 756, 793, 830, 868, 906, 944, 983, 1022, 1061, 1142, 1224, 1307, 1392, 1479, 1567, 1657, 1748, 1841, 1935, 2030, 2127, 2226, 2326, 2427,  /* 32 - 63*/
                    2530, 2734, 2934, 3129, 3321, 3508, 3691, 3871, 4047, 4220, 4389, 4555, 4718, 4878, 5034, 5188, 5340, 5634, 5918, 6193, 6459, 6717, 6966, 7208, 7442, 7669, 7890, 8105, 8313, 8516, 8713, 8905,  /* 64 - 95*/
                    9092, 9451, 9792, 10116, 10424, 10718, 10998, 11265, 11520, 11764, 11997, 12220, 12433, 12638, 12834, 13021, 13202, 13540, 13852, 14139, 14405, 14650, 14877, 15086, 15280, 15459, 15624, 15777, 15918, 16049, 16169, 16280,  /* 96 - 127*/
                    16383, /*128 - 128*/
                },
            },
            /* 1 */
            {
                /* nLut[129] */
                {
                    0, 13, 26, 40, 53, 67, 81, 95, 109, 123, 137, 152, 166, 181, 195, 210, 225, 240, 255, 270, 285, 301, 316, 332, 348, 364, 379, 395, 412, 428, 444, 461,  /* 0 - 31*/
                    477, 511, 544, 579, 613, 648, 684, 720, 756, 793, 830, 868, 906, 944, 983, 1022, 1061, 1142, 1224, 1307, 1392, 1479, 1567, 1657, 1748, 1841, 1935, 2030, 2127, 2226, 2326, 2427,  /* 32 - 63*/
                    2530, 2734, 2934, 3129, 3321, 3508, 3691, 3871, 4047, 4220, 4389, 4555, 4718, 4878, 5034, 5188, 5340, 5634, 5918, 6193, 6459, 6717, 6966, 7208, 7442, 7669, 7890, 8105, 8313, 8516, 8713, 8905,  /* 64 - 95*/
                    9092, 9451, 9792, 10116, 10424, 10718, 10998, 11265, 11520, 11764, 11997, 12220, 12433, 12638, 12834, 13021, 13202, 13540, 13852, 14139, 14405, 14650, 14877, 15086, 15280, 15459, 15624, 15777, 15918, 16049, 16169, 16280,  /* 96 - 127*/
                    16383, /*128 - 128*/
                },
            },
            /* 2 */
            {
                /* nLut[129] */
                {
                    0, 13, 26, 40, 53, 67, 81, 95, 109, 123, 137, 152, 166, 181, 195, 210, 225, 240, 255, 270, 285, 301, 316, 332, 348, 364, 379, 395, 412, 428, 444, 461,  /* 0 - 31*/
                    477, 511, 544, 579, 613, 648, 684, 720, 756, 793, 830, 868, 906, 944, 983, 1022, 1061, 1142, 1224, 1307, 1392, 1479, 1567, 1657, 1748, 1841, 1935, 2030, 2127, 2226, 2326, 2427,  /* 32 - 63*/
                    2530, 2734, 2934, 3129, 3321, 3508, 3691, 3871, 4047, 4220, 4389, 4555, 4718, 4878, 5034, 5188, 5340, 5634, 5918, 6193, 6459, 6717, 6966, 7208, 7442, 7669, 7890, 8105, 8313, 8516, 8713, 8905,  /* 64 - 95*/
                    9092, 9451, 9792, 10116, 10424, 10718, 10998, 11265, 11520, 11764, 11997, 12220, 12433, 12638, 12834, 13021, 13202, 13540, 13852, 14139, 14405, 14650, 14877, 15086, 15280, 15459, 15624, 15777, 15918, 16049, 16169, 16280,  /* 96 - 127*/
                    16383, /*128 - 128*/
                },
            },
        },
    },
};

static AX_ISP_IQ_DEHAZE_PARAM_T dehaze_param_sdr = {
    /* nDehazeEn */
    0,
    /* nAutoMode */
    0,
    /* nRefMode */
    1,
    /* tGlbParam */
    {
        /* nGrayMode */
        0,
        /* nGrayRatio[3] */
        {64, 127, 64, /*0 - 2*/},
        /* nAirReflect */
        52224,
        /* nStrengthLimit */
        26,
    },
    /* tManualParam */
    {
        /* nEffectStrength */
        102,
        /* nGrayDcRatio */
        64,
    },
    /* tAutoParam */
    {
        /* nParamGrpNum */
        0,
        /* nRefVal[16] */
        {1024, 2048, 253952, 8192, 10240, 12288, 14336, 15872, 31744, 63488, 95232, 126976, 158720, 190464, 222208, 253952/*0 - 15*/},
        /* nEffectStrength[16] */
        {204, 210, 224, 204, 204, 204, 204, 204, 204, 204, 204, 204, 204, 204, 204, 204,/*0 - 15*/},
        /* nGrayDcRatio */
        {64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,/*0 - 15*/},
    },
};

static AX_ISP_IQ_CSC_PARAM_T csc_param_sdr = {
    /* nMatrix[3][3] */
    {
        {76, 150, 29, /*0 - 2*/},
        {-43, -85, 128, /*0 - 2*/},
        {128, -107, -21, /*0 - 2*/},
    },
    /* eColorSpaceMode */
    /* 0:AX_ISP_CSC_USER, 1:AX_ISP_CSC_BT601, 2:AX_ISP_CSC_BT709, 3:AX_ISP_CSC_BT2020, 4:AX_ISP_CSC_BUTT */
    AX_ISP_CSC_USER,
    /* eUvSeqSel */
    /* 0:AX_ISP_NV12_SEL, 1:AX_ISP_NV21_SEL */
    AX_ISP_NV12_SEL,
};

static AX_ISP_IQ_CA_PARAM_T ca_param_sdr = {
    /* nCppEn */
    1,
    /* nAutoMode */
    0,
    /* nRefMode */
    0,
    /* tManualParam */
    {
        /* nCtrlLevel */
        256,
        /* nSat */
        0,
        /* nHue */
        0,
        /* nCmtx[2][2] */
        {
            {256, 0, /*0 - 1*/},
            {0, 256, /*0 - 1*/},
        },
    },
    /* tAutoParam */
    {
        /* nParamGrpNumCt */
        0,
        /* nParamGrpNumLG */
        0,
        /* nRefValCt[12] */
        {0, /*0 - 0*/},
        /* nRefValLG[12][5] */
        {
            {0, /*0 - 0*/},
        },
        /* nCmtx[12][5][4] */
        {
            /* nCmtx 0 */
            {
                {0, 0, 0, 0, /*0 - 3*/},
            },
        },
    },
};

static AX_ISP_IQ_YNR_PARAM_T ynr_param_sdr = {
    /* nYnrEn */
    1,
    /* nDbpcEn */
    0,
    /* nColorTargetEn */
    0,
    /* nAutoMode */
    1,
    /* nRefMode */
    1,
    /* nIoFlag */
    0,
    /* tManualParam */
    {
        /* nYnrCenter[3] */
        {0, 0, 0, /*0 - 2*/},
        /* nYnrRadius[3] */
        {0, 0, 0, /*0 - 2*/},
        /* nYnrSmooth[3] */
        {0, 0, 0, /*0 - 2*/},
        /* nYnrLevel[2] */
        {255, 0, /*0 - 1*/},
        /* nYnrInvNrLut[4] */
        {436, 744, 936, 1244, /*0 - 3*/},
        /* nDbpcEdSlope */
        0,
        /* nDbpcEdOffsetLut[4] */
        {0, 0, 0, 0, /*0 - 3*/},
    },
    /* tAutoParam */
    {
        /* nParamGrpNum */
        12,
        /* nRefVal[16] */
        {1024, 2048, 16384, 32768, 49152, 65536, 65537, 131072, 262144, 393216, 524288, 1048576, /*0 - 11*/},
        /* nYnrCenter[16][3] */
        {
            {0, 0, 0, /*0 - 2*/},
            {0, 0, 0, /*0 - 2*/},
            {0, 0, 0, /*0 - 2*/},
            {0, 0, 0, /*0 - 2*/},
            {0, 0, 0, /*0 - 2*/},
            {0, 0, 0, /*0 - 2*/},
            {0, 0, 0, /*0 - 2*/},
            {0, 0, 0, /*0 - 2*/},
            {0, 0, 0, /*0 - 2*/},
            {0, 0, 0, /*0 - 2*/},
            {0, 0, 0, /*0 - 2*/},
            {0, 0, 0, /*0 - 2*/},
        },
        /* nYnrRadius[16][3] */
        {
            {0, 0, 0, /*0 - 2*/},
            {0, 0, 0, /*0 - 2*/},
            {0, 0, 0, /*0 - 2*/},
            {0, 0, 0, /*0 - 2*/},
            {0, 0, 0, /*0 - 2*/},
            {0, 0, 0, /*0 - 2*/},
            {0, 0, 0, /*0 - 2*/},
            {0, 0, 0, /*0 - 2*/},
            {0, 0, 0, /*0 - 2*/},
            {0, 0, 0, /*0 - 2*/},
            {0, 0, 0, /*0 - 2*/},
            {0, 0, 0, /*0 - 2*/},
        },
        /* nYnrSmooth[16][3] */
        {
            {0, 0, 0, /*0 - 2*/},
            {0, 0, 0, /*0 - 2*/},
            {0, 0, 0, /*0 - 2*/},
            {0, 0, 0, /*0 - 2*/},
            {0, 0, 0, /*0 - 2*/},
            {0, 0, 0, /*0 - 2*/},
            {0, 0, 0, /*0 - 2*/},
            {0, 0, 0, /*0 - 2*/},
            {0, 0, 0, /*0 - 2*/},
            {0, 0, 0, /*0 - 2*/},
            {0, 0, 0, /*0 - 2*/},
            {0, 0, 0, /*0 - 2*/},
        },
        /* nYnrLevel[16][2] */
        {
            {255, 0, /*0 - 1*/},
            {255, 0, /*0 - 1*/},
            {255, 0, /*0 - 1*/},
            {255, 0, /*0 - 1*/},
            {255, 0, /*0 - 1*/},
            {255, 0, /*0 - 1*/},
            {255, 0, /*0 - 1*/},
            {255, 0, /*0 - 1*/},
            {255, 0, /*0 - 1*/},
            {255, 0, /*0 - 1*/},
            {255, 0, /*0 - 1*/},
            {255, 0, /*0 - 1*/},
        },
        /* nYnrInvNrLut[16][4] */
        {
            {2047, 2047, 2047, 2047, /*0 - 3*/},
            {2047, 2047, 2047, 2047, /*0 - 3*/},
            {2047, 2047, 2047, 2047, /*0 - 3*/},
            {2047, 2047, 2047, 2047, /*0 - 3*/},
            {2047, 2047, 2047, 2047, /*0 - 3*/},
            {2047, 2047, 2047, 2047, /*0 - 3*/},
            {2047, 2047, 2047, 2047, /*0 - 3*/},
            {128, 436, 706, 1167, /*0 - 3*/},
            {128, 436, 706, 1167, /*0 - 3*/},
            {128, 436, 706, 1167, /*0 - 3*/},
            {128, 436, 706, 1167, /*0 - 3*/},
            {128, 436, 706, 1167, /*0 - 3*/},
        },
        /* nDbpcEdSlope[16] */
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 11*/},
        /* nDbpcEdOffsetLut[16][4] */
        {
            {0, 0, 0, 0, /*0 - 3*/},
            {0, 0, 0, 0, /*0 - 3*/},
            {0, 0, 0, 0, /*0 - 3*/},
            {0, 0, 0, 0, /*0 - 3*/},
            {0, 0, 0, 0, /*0 - 3*/},
            {0, 0, 0, 0, /*0 - 3*/},
            {0, 0, 0, 0, /*0 - 3*/},
            {0, 0, 0, 0, /*0 - 3*/},
            {0, 0, 0, 0, /*0 - 3*/},
            {0, 0, 0, 0, /*0 - 3*/},
            {0, 0, 0, 0, /*0 - 3*/},
            {0, 0, 0, 0, /*0 - 3*/},
        },
    },
};

static AX_ISP_IQ_AYNR_PARAM_T aynr_param_sdr = {
    /* nAutoMode */
    1,
    /* nRefMode */
    1,
    /* nFilterMode[4] */
    {0, 0, 0, 1, /*0 - 3*/},
    /* tManualParam */
    {
        /* nAYnrEdDeteThre[4] */
        {0, 0, 0, 0, /*0 - 3*/},
        /* nAYnrEdDeteSlope[4] */
        {1280, 1280, 1280, 1280, /*0 - 3*/},
        /* nAYnrIddDetStren[4] */
        {224, 224, 224, 224, /*0 - 3*/},
        /* tAYnrFilterCoef[4] */
        {
            /* 0 */
            {
                /* nAYnrFilter0Coef[25] */
                {0, 1, 1, 1, 0, 1, 11, 30, 11, 1, 1, 30, 80, 30, 1, 1, 11, 30, 11, 1, 0, 1, 1, 1, 0, /*0 - 24*/},
                /* nAYnrFilter1Coef[25] */
                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 56, 136, 56, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 24*/},
                /* nAYnrFilter2Coef[25] */
                {0, 0, 4, 0, 0, 0, 0, 56, 0, 0, 0, 0, 136, 0, 0, 0, 0, 56, 0, 0, 0, 0, 4, 0, 0, /*0 - 24*/},
                /* nAYnrFilter3Coef[25] */
                {0, 0, 0, 0, 0, 0, 0, 0, 32, 0, 0, 0, 192, 0, 0, 0, 32, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 24*/},
                /* nAYnrFilter4Coef[25] */
                {0, 0, 0, 0, 0, 0, 32, 0, 0, 0, 0, 0, 192, 0, 0, 0, 0, 0, 32, 0, 0, 0, 0, 0, 0, /*0 - 24*/},
            },
            /* 1 */
            {
                /* nAYnrFilter0Coef[25] */
                {0, 1, 1, 1, 0, 1, 11, 30, 11, 1, 1, 30, 80, 30, 1, 1, 11, 30, 11, 1, 0, 1, 1, 1, 0, /*0 - 24*/},
                /* nAYnrFilter1Coef[25] */
                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 56, 136, 56, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 24*/},
                /* nAYnrFilter2Coef[25] */
                {0, 0, 4, 0, 0, 0, 0, 56, 0, 0, 0, 0, 136, 0, 0, 0, 0, 56, 0, 0, 0, 0, 4, 0, 0, /*0 - 24*/},
                /* nAYnrFilter3Coef[25] */
                {0, 0, 0, 0, 0, 0, 0, 0, 32, 0, 0, 0, 192, 0, 0, 0, 32, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 24*/},
                /* nAYnrFilter4Coef[25] */
                {0, 0, 0, 0, 0, 0, 32, 0, 0, 0, 0, 0, 192, 0, 0, 0, 0, 0, 32, 0, 0, 0, 0, 0, 0, /*0 - 24*/},
            },
            /* 2 */
            {
                /* nAYnrFilter0Coef[25] */
                {0, 1, 1, 1, 0, 1, 11, 30, 11, 1, 1, 30, 80, 30, 1, 1, 11, 30, 11, 1, 0, 1, 1, 1, 0, /*0 - 24*/},
                /* nAYnrFilter1Coef[25] */
                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 56, 136, 56, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 24*/},
                /* nAYnrFilter2Coef[25] */
                {0, 0, 4, 0, 0, 0, 0, 56, 0, 0, 0, 0, 136, 0, 0, 0, 0, 56, 0, 0, 0, 0, 4, 0, 0, /*0 - 24*/},
                /* nAYnrFilter3Coef[25] */
                {0, 0, 0, 0, 0, 0, 0, 0, 32, 0, 0, 0, 192, 0, 0, 0, 32, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 24*/},
                /* nAYnrFilter4Coef[25] */
                {0, 0, 0, 0, 0, 0, 32, 0, 0, 0, 0, 0, 192, 0, 0, 0, 0, 0, 32, 0, 0, 0, 0, 0, 0, /*0 - 24*/},
            },
            /* 3 */
            {
                /* nAYnrFilter0Coef[25] */
                {0, 1, 1, 1, 0, 1, 11, 30, 11, 1, 1, 30, 80, 30, 1, 1, 11, 30, 11, 1, 0, 1, 1, 1, 0, /*0 - 24*/},
                /* nAYnrFilter1Coef[25] */
                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 56, 136, 56, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 24*/},
                /* nAYnrFilter2Coef[25] */
                {0, 0, 4, 0, 0, 0, 0, 56, 0, 0, 0, 0, 136, 0, 0, 0, 0, 56, 0, 0, 0, 0, 4, 0, 0, /*0 - 24*/},
                /* nAYnrFilter3Coef[25] */
                {0, 0, 0, 0, 0, 0, 0, 0, 32, 0, 0, 0, 192, 0, 0, 0, 32, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 24*/},
                /* nAYnrFilter4Coef[25] */
                {0, 0, 0, 0, 0, 0, 32, 0, 0, 0, 0, 0, 192, 0, 0, 0, 0, 0, 32, 0, 0, 0, 0, 0, 0, /*0 - 24*/},
            },
        },
        /* tAYnrFilterStrength[4] */
        {
            /* 0 */
            {
                /* nAYnrFilter0Strength */
                128,
                /* nAYnrFilter1Strength[2] */
                {192, 16, /*0 - 1*/},
                /* nAYnrFilter2Strength[2] */
                {192, 16, /*0 - 1*/},
                /* nAYnrFilter3Strength[2] */
                {192, 16, /*0 - 1*/},
                /* nAYnrFilter4Strength[2] */
                {192, 16, /*0 - 1*/},
            },
            /* 1 */
            {
                /* nAYnrFilter0Strength */
                128,
                /* nAYnrFilter1Strength[2] */
                {192, 16, /*0 - 1*/},
                /* nAYnrFilter2Strength[2] */
                {192, 16, /*0 - 1*/},
                /* nAYnrFilter3Strength[2] */
                {192, 16, /*0 - 1*/},
                /* nAYnrFilter4Strength[2] */
                {192, 16, /*0 - 1*/},
            },
            /* 2 */
            {
                /* nAYnrFilter0Strength */
                128,
                /* nAYnrFilter1Strength[2] */
                {192, 16, /*0 - 1*/},
                /* nAYnrFilter2Strength[2] */
                {192, 16, /*0 - 1*/},
                /* nAYnrFilter3Strength[2] */
                {192, 16, /*0 - 1*/},
                /* nAYnrFilter4Strength[2] */
                {192, 16, /*0 - 1*/},
            },
            /* 3 */
            {
                /* nAYnrFilter0Strength */
                256,
                /* nAYnrFilter1Strength[2] */
                {256, 0, /*0 - 1*/},
                /* nAYnrFilter2Strength[2] */
                {256, 0, /*0 - 1*/},
                /* nAYnrFilter3Strength[2] */
                {128, 64, /*0 - 1*/},
                /* nAYnrFilter4Strength[2] */
                {128, 64, /*0 - 1*/},
            },
        },
        /* nAYnrStrength[4] */
        {256, 164, 128, 0, /*0 - 3*/},
    },
    /* tAutoParam */
    {
        /* nParamGrpNum */
        12,
        /* nRefVal[16] */
        {1024, 2048, 16384, 32768, 49152, 65536, 65537, 131072, 262144, 393216, 524288, 1048576, /*0 - 11*/},
        /* nAYnrEdDeteThre[16][4] */
        {
            {0, 0, 0, 0, /*0 - 3*/},
            {0, 0, 0, 0, /*0 - 3*/},
            {0, 0, 0, 0, /*0 - 3*/},
            {0, 0, 0, 0, /*0 - 3*/},
            {0, 0, 0, 0, /*0 - 3*/},
            {0, 0, 0, 0, /*0 - 3*/},
            {0, 0, 0, 0, /*0 - 3*/},
            {0, 0, 0, 0, /*0 - 3*/},
            {0, 0, 0, 0, /*0 - 3*/},
            {0, 0, 0, 0, /*0 - 3*/},
            {0, 0, 0, 0, /*0 - 3*/},
            {0, 0, 0, 0, /*0 - 3*/},
        },
        /* nAYnrEdDeteSlope[16][4] */
        {
            {1280, 1280, 1280, 1280, /*0 - 3*/},
            {1280, 1280, 1280, 1280, /*0 - 3*/},
            {1280, 1280, 1280, 1280, /*0 - 3*/},
            {1280, 1280, 65535, 65535, /*0 - 3*/},
            {1280, 1280, 65535, 65535, /*0 - 3*/},
            {1280, 1280, 65535, 65535, /*0 - 3*/},
            {1280, 1280, 65535, 65535, /*0 - 3*/},
            {1280, 1280, 65535, 65535, /*0 - 3*/},
            {1280, 1280, 65535, 65535, /*0 - 3*/},
            {1280, 1280, 65535, 65535, /*0 - 3*/},
            {1280, 1280, 65535, 65535, /*0 - 3*/},
            {1280, 1280, 65535, 65535, /*0 - 3*/},
        },
        /* nAYnrIddDetStren[16][4] */
        {
            {224, 224, 224, 224, /*0 - 3*/},
            {224, 224, 224, 224, /*0 - 3*/},
            {224, 224, 224, 224, /*0 - 3*/},
            {224, 224, 224, 224, /*0 - 3*/},
            {224, 224, 224, 224, /*0 - 3*/},
            {224, 224, 224, 224, /*0 - 3*/},
            {224, 224, 224, 224, /*0 - 3*/},
            {224, 224, 224, 224, /*0 - 3*/},
            {224, 224, 224, 224, /*0 - 3*/},
            {224, 224, 224, 224, /*0 - 3*/},
            {224, 224, 224, 224, /*0 - 3*/},
            {224, 224, 224, 224, /*0 - 3*/},
        },
        /* tAYnrFilterCoef */
        {
            /* 0 */
            {
                /* 0 */
                {
                    /* nAYnrFilter0Coef[25] */
                    {0, 1, 1, 1, 0, 1, 11, 30, 11, 1, 1, 30, 80, 30, 1, 1, 11, 30, 11, 1, 0, 1, 1, 1, 0, /*0 - 24*/},
                    /* nAYnrFilter1Coef[25] */
                    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 56, 136, 56, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 24*/},
                    /* nAYnrFilter2Coef[25] */
                    {0, 0, 4, 0, 0, 0, 0, 56, 0, 0, 0, 0, 136, 0, 0, 0, 0, 56, 0, 0, 0, 0, 4, 0, 0, /*0 - 24*/},
                    /* nAYnrFilter3Coef[25] */
                    {0, 0, 0, 0, 0, 0, 0, 0, 32, 0, 0, 0, 192, 0, 0, 0, 32, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 24*/},
                    /* nAYnrFilter4Coef[25] */
                    {0, 0, 0, 0, 0, 0, 32, 0, 0, 0, 0, 0, 192, 0, 0, 0, 0, 0, 32, 0, 0, 0, 0, 0, 0, /*0 - 24*/},
                },
                /* 1 */
                {
                    /* nAYnrFilter0Coef[25] */
                    {0, 1, 1, 1, 0, 1, 11, 30, 11, 1, 1, 30, 80, 30, 1, 1, 11, 30, 11, 1, 0, 1, 1, 1, 0, /*0 - 24*/},
                    /* nAYnrFilter1Coef[25] */
                    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 56, 136, 56, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 24*/},
                    /* nAYnrFilter2Coef[25] */
                    {0, 0, 4, 0, 0, 0, 0, 56, 0, 0, 0, 0, 136, 0, 0, 0, 0, 56, 0, 0, 0, 0, 4, 0, 0, /*0 - 24*/},
                    /* nAYnrFilter3Coef[25] */
                    {0, 0, 0, 0, 0, 0, 0, 0, 32, 0, 0, 0, 192, 0, 0, 0, 32, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 24*/},
                    /* nAYnrFilter4Coef[25] */
                    {0, 0, 0, 0, 0, 0, 32, 0, 0, 0, 0, 0, 192, 0, 0, 0, 0, 0, 32, 0, 0, 0, 0, 0, 0, /*0 - 24*/},
                },
                /* 2 */
                {
                    /* nAYnrFilter0Coef[25] */
                    {0, 1, 1, 1, 0, 1, 11, 30, 11, 1, 1, 30, 80, 30, 1, 1, 11, 30, 11, 1, 0, 1, 1, 1, 0, /*0 - 24*/},
                    /* nAYnrFilter1Coef[25] */
                    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 56, 136, 56, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 24*/},
                    /* nAYnrFilter2Coef[25] */
                    {0, 0, 4, 0, 0, 0, 0, 56, 0, 0, 0, 0, 136, 0, 0, 0, 0, 56, 0, 0, 0, 0, 4, 0, 0, /*0 - 24*/},
                    /* nAYnrFilter3Coef[25] */
                    {0, 0, 0, 0, 0, 0, 0, 0, 32, 0, 0, 0, 192, 0, 0, 0, 32, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 24*/},
                    /* nAYnrFilter4Coef[25] */
                    {0, 0, 0, 0, 0, 0, 32, 0, 0, 0, 0, 0, 192, 0, 0, 0, 0, 0, 32, 0, 0, 0, 0, 0, 0, /*0 - 24*/},
                },
                /* 3 */
                {
                    /* nAYnrFilter0Coef[25] */
                    {0, 1, 1, 1, 0, 1, 11, 30, 11, 1, 1, 30, 80, 30, 1, 1, 11, 30, 11, 1, 0, 1, 1, 1, 0, /*0 - 24*/},
                    /* nAYnrFilter1Coef[25] */
                    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 56, 136, 56, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 24*/},
                    /* nAYnrFilter2Coef[25] */
                    {0, 0, 4, 0, 0, 0, 0, 56, 0, 0, 0, 0, 136, 0, 0, 0, 0, 56, 0, 0, 0, 0, 4, 0, 0, /*0 - 24*/},
                    /* nAYnrFilter3Coef[25] */
                    {0, 0, 0, 0, 0, 0, 0, 0, 32, 0, 0, 0, 192, 0, 0, 0, 32, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 24*/},
                    /* nAYnrFilter4Coef[25] */
                    {0, 0, 0, 0, 0, 0, 32, 0, 0, 0, 0, 0, 192, 0, 0, 0, 0, 0, 32, 0, 0, 0, 0, 0, 0, /*0 - 24*/},
                },
            },
            /* 1 */
            {
                /* 0 */
                {
                    /* nAYnrFilter0Coef[25] */
                    {0, 1, 1, 1, 0, 1, 11, 30, 11, 1, 1, 30, 80, 30, 1, 1, 11, 30, 11, 1, 0, 1, 1, 1, 0, /*0 - 24*/},
                    /* nAYnrFilter1Coef[25] */
                    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 56, 136, 56, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 24*/},
                    /* nAYnrFilter2Coef[25] */
                    {0, 0, 4, 0, 0, 0, 0, 56, 0, 0, 0, 0, 136, 0, 0, 0, 0, 56, 0, 0, 0, 0, 4, 0, 0, /*0 - 24*/},
                    /* nAYnrFilter3Coef[25] */
                    {0, 0, 0, 0, 0, 0, 0, 0, 32, 0, 0, 0, 192, 0, 0, 0, 32, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 24*/},
                    /* nAYnrFilter4Coef[25] */
                    {0, 0, 0, 0, 0, 0, 32, 0, 0, 0, 0, 0, 192, 0, 0, 0, 0, 0, 32, 0, 0, 0, 0, 0, 0, /*0 - 24*/},
                },
                /* 1 */
                {
                    /* nAYnrFilter0Coef[25] */
                    {0, 1, 1, 1, 0, 1, 11, 30, 11, 1, 1, 30, 80, 30, 1, 1, 11, 30, 11, 1, 0, 1, 1, 1, 0, /*0 - 24*/},
                    /* nAYnrFilter1Coef[25] */
                    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 56, 136, 56, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 24*/},
                    /* nAYnrFilter2Coef[25] */
                    {0, 0, 4, 0, 0, 0, 0, 56, 0, 0, 0, 0, 136, 0, 0, 0, 0, 56, 0, 0, 0, 0, 4, 0, 0, /*0 - 24*/},
                    /* nAYnrFilter3Coef[25] */
                    {0, 0, 0, 0, 0, 0, 0, 0, 32, 0, 0, 0, 192, 0, 0, 0, 32, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 24*/},
                    /* nAYnrFilter4Coef[25] */
                    {0, 0, 0, 0, 0, 0, 32, 0, 0, 0, 0, 0, 192, 0, 0, 0, 0, 0, 32, 0, 0, 0, 0, 0, 0, /*0 - 24*/},
                },
                /* 2 */
                {
                    /* nAYnrFilter0Coef[25] */
                    {0, 1, 1, 1, 0, 1, 11, 30, 11, 1, 1, 30, 80, 30, 1, 1, 11, 30, 11, 1, 0, 1, 1, 1, 0, /*0 - 24*/},
                    /* nAYnrFilter1Coef[25] */
                    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 56, 136, 56, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 24*/},
                    /* nAYnrFilter2Coef[25] */
                    {0, 0, 4, 0, 0, 0, 0, 56, 0, 0, 0, 0, 136, 0, 0, 0, 0, 56, 0, 0, 0, 0, 4, 0, 0, /*0 - 24*/},
                    /* nAYnrFilter3Coef[25] */
                    {0, 0, 0, 0, 0, 0, 0, 0, 32, 0, 0, 0, 192, 0, 0, 0, 32, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 24*/},
                    /* nAYnrFilter4Coef[25] */
                    {0, 0, 0, 0, 0, 0, 32, 0, 0, 0, 0, 0, 192, 0, 0, 0, 0, 0, 32, 0, 0, 0, 0, 0, 0, /*0 - 24*/},
                },
                /* 3 */
                {
                    /* nAYnrFilter0Coef[25] */
                    {0, 1, 1, 1, 0, 1, 11, 30, 11, 1, 1, 30, 80, 30, 1, 1, 11, 30, 11, 1, 0, 1, 1, 1, 0, /*0 - 24*/},
                    /* nAYnrFilter1Coef[25] */
                    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 56, 136, 56, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 24*/},
                    /* nAYnrFilter2Coef[25] */
                    {0, 0, 4, 0, 0, 0, 0, 56, 0, 0, 0, 0, 136, 0, 0, 0, 0, 56, 0, 0, 0, 0, 4, 0, 0, /*0 - 24*/},
                    /* nAYnrFilter3Coef[25] */
                    {0, 0, 0, 0, 0, 0, 0, 0, 32, 0, 0, 0, 192, 0, 0, 0, 32, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 24*/},
                    /* nAYnrFilter4Coef[25] */
                    {0, 0, 0, 0, 0, 0, 32, 0, 0, 0, 0, 0, 192, 0, 0, 0, 0, 0, 32, 0, 0, 0, 0, 0, 0, /*0 - 24*/},
                },
            },
            /* 2 */
            {
                /* 0 */
                {
                    /* nAYnrFilter0Coef[25] */
                    {0, 1, 1, 1, 0, 1, 11, 30, 11, 1, 1, 30, 80, 30, 1, 1, 11, 30, 11, 1, 0, 1, 1, 1, 0, /*0 - 24*/},
                    /* nAYnrFilter1Coef[25] */
                    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 56, 136, 56, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 24*/},
                    /* nAYnrFilter2Coef[25] */
                    {0, 0, 4, 0, 0, 0, 0, 56, 0, 0, 0, 0, 136, 0, 0, 0, 0, 56, 0, 0, 0, 0, 4, 0, 0, /*0 - 24*/},
                    /* nAYnrFilter3Coef[25] */
                    {0, 0, 0, 0, 0, 0, 0, 0, 32, 0, 0, 0, 192, 0, 0, 0, 32, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 24*/},
                    /* nAYnrFilter4Coef[25] */
                    {0, 0, 0, 0, 0, 0, 32, 0, 0, 0, 0, 0, 192, 0, 0, 0, 0, 0, 32, 0, 0, 0, 0, 0, 0, /*0 - 24*/},
                },
                /* 1 */
                {
                    /* nAYnrFilter0Coef[25] */
                    {0, 1, 1, 1, 0, 1, 11, 30, 11, 1, 1, 30, 80, 30, 1, 1, 11, 30, 11, 1, 0, 1, 1, 1, 0, /*0 - 24*/},
                    /* nAYnrFilter1Coef[25] */
                    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 56, 136, 56, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 24*/},
                    /* nAYnrFilter2Coef[25] */
                    {0, 0, 4, 0, 0, 0, 0, 56, 0, 0, 0, 0, 136, 0, 0, 0, 0, 56, 0, 0, 0, 0, 4, 0, 0, /*0 - 24*/},
                    /* nAYnrFilter3Coef[25] */
                    {0, 0, 0, 0, 0, 0, 0, 0, 32, 0, 0, 0, 192, 0, 0, 0, 32, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 24*/},
                    /* nAYnrFilter4Coef[25] */
                    {0, 0, 0, 0, 0, 0, 32, 0, 0, 0, 0, 0, 192, 0, 0, 0, 0, 0, 32, 0, 0, 0, 0, 0, 0, /*0 - 24*/},
                },
                /* 2 */
                {
                    /* nAYnrFilter0Coef[25] */
                    {0, 1, 1, 1, 0, 1, 11, 30, 11, 1, 1, 30, 80, 30, 1, 1, 11, 30, 11, 1, 0, 1, 1, 1, 0, /*0 - 24*/},
                    /* nAYnrFilter1Coef[25] */
                    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 56, 136, 56, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 24*/},
                    /* nAYnrFilter2Coef[25] */
                    {0, 0, 4, 0, 0, 0, 0, 56, 0, 0, 0, 0, 136, 0, 0, 0, 0, 56, 0, 0, 0, 0, 4, 0, 0, /*0 - 24*/},
                    /* nAYnrFilter3Coef[25] */
                    {0, 0, 0, 0, 0, 0, 0, 0, 32, 0, 0, 0, 192, 0, 0, 0, 32, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 24*/},
                    /* nAYnrFilter4Coef[25] */
                    {0, 0, 0, 0, 0, 0, 32, 0, 0, 0, 0, 0, 192, 0, 0, 0, 0, 0, 32, 0, 0, 0, 0, 0, 0, /*0 - 24*/},
                },
                /* 3 */
                {
                    /* nAYnrFilter0Coef[25] */
                    {0, 1, 1, 1, 0, 1, 11, 30, 11, 1, 1, 30, 80, 30, 1, 1, 11, 30, 11, 1, 0, 1, 1, 1, 0, /*0 - 24*/},
                    /* nAYnrFilter1Coef[25] */
                    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 56, 136, 56, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 24*/},
                    /* nAYnrFilter2Coef[25] */
                    {0, 0, 4, 0, 0, 0, 0, 56, 0, 0, 0, 0, 136, 0, 0, 0, 0, 56, 0, 0, 0, 0, 4, 0, 0, /*0 - 24*/},
                    /* nAYnrFilter3Coef[25] */
                    {0, 0, 0, 0, 0, 0, 0, 0, 32, 0, 0, 0, 192, 0, 0, 0, 32, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 24*/},
                    /* nAYnrFilter4Coef[25] */
                    {0, 0, 0, 0, 0, 0, 32, 0, 0, 0, 0, 0, 192, 0, 0, 0, 0, 0, 32, 0, 0, 0, 0, 0, 0, /*0 - 24*/},
                },
            },
            /* 3 */
            {
                /* 0 */
                {
                    /* nAYnrFilter0Coef[25] */
                    {0, 1, 1, 1, 0, 1, 11, 30, 11, 1, 1, 30, 80, 30, 1, 1, 11, 30, 11, 1, 0, 1, 1, 1, 0, /*0 - 24*/},
                    /* nAYnrFilter1Coef[25] */
                    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 56, 136, 56, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 24*/},
                    /* nAYnrFilter2Coef[25] */
                    {0, 0, 4, 0, 0, 0, 0, 56, 0, 0, 0, 0, 136, 0, 0, 0, 0, 56, 0, 0, 0, 0, 4, 0, 0, /*0 - 24*/},
                    /* nAYnrFilter3Coef[25] */
                    {0, 0, 0, 0, 0, 0, 0, 0, 32, 0, 0, 0, 192, 0, 0, 0, 32, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 24*/},
                    /* nAYnrFilter4Coef[25] */
                    {0, 0, 0, 0, 0, 0, 32, 0, 0, 0, 0, 0, 192, 0, 0, 0, 0, 0, 32, 0, 0, 0, 0, 0, 0, /*0 - 24*/},
                },
                /* 1 */
                {
                    /* nAYnrFilter0Coef[25] */
                    {0, 1, 1, 1, 0, 1, 11, 30, 11, 1, 1, 30, 80, 30, 1, 1, 11, 30, 11, 1, 0, 1, 1, 1, 0, /*0 - 24*/},
                    /* nAYnrFilter1Coef[25] */
                    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 56, 136, 56, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 24*/},
                    /* nAYnrFilter2Coef[25] */
                    {0, 0, 4, 0, 0, 0, 0, 56, 0, 0, 0, 0, 136, 0, 0, 0, 0, 56, 0, 0, 0, 0, 4, 0, 0, /*0 - 24*/},
                    /* nAYnrFilter3Coef[25] */
                    {0, 0, 0, 0, 0, 0, 0, 0, 32, 0, 0, 0, 192, 0, 0, 0, 32, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 24*/},
                    /* nAYnrFilter4Coef[25] */
                    {0, 0, 0, 0, 0, 0, 32, 0, 0, 0, 0, 0, 192, 0, 0, 0, 0, 0, 32, 0, 0, 0, 0, 0, 0, /*0 - 24*/},
                },
                /* 2 */
                {
                    /* nAYnrFilter0Coef[25] */
                    {0, 1, 1, 1, 0, 1, 11, 30, 11, 1, 1, 30, 80, 30, 1, 1, 11, 30, 11, 1, 0, 1, 1, 1, 0, /*0 - 24*/},
                    /* nAYnrFilter1Coef[25] */
                    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 56, 136, 56, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 24*/},
                    /* nAYnrFilter2Coef[25] */
                    {0, 0, 4, 0, 0, 0, 0, 56, 0, 0, 0, 0, 136, 0, 0, 0, 0, 56, 0, 0, 0, 0, 4, 0, 0, /*0 - 24*/},
                    /* nAYnrFilter3Coef[25] */
                    {0, 0, 0, 0, 0, 0, 0, 0, 32, 0, 0, 0, 192, 0, 0, 0, 32, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 24*/},
                    /* nAYnrFilter4Coef[25] */
                    {0, 0, 0, 0, 0, 0, 32, 0, 0, 0, 0, 0, 192, 0, 0, 0, 0, 0, 32, 0, 0, 0, 0, 0, 0, /*0 - 24*/},
                },
                /* 3 */
                {
                    /* nAYnrFilter0Coef[25] */
                    {0, 1, 1, 1, 0, 1, 11, 30, 11, 1, 1, 30, 80, 30, 1, 1, 11, 30, 11, 1, 0, 1, 1, 1, 0, /*0 - 24*/},
                    /* nAYnrFilter1Coef[25] */
                    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 56, 136, 56, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 24*/},
                    /* nAYnrFilter2Coef[25] */
                    {0, 0, 4, 0, 0, 0, 0, 56, 0, 0, 0, 0, 136, 0, 0, 0, 0, 56, 0, 0, 0, 0, 4, 0, 0, /*0 - 24*/},
                    /* nAYnrFilter3Coef[25] */
                    {0, 0, 0, 0, 0, 0, 0, 0, 32, 0, 0, 0, 192, 0, 0, 0, 32, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 24*/},
                    /* nAYnrFilter4Coef[25] */
                    {0, 0, 0, 0, 0, 0, 32, 0, 0, 0, 0, 0, 192, 0, 0, 0, 0, 0, 32, 0, 0, 0, 0, 0, 0, /*0 - 24*/},
                },
            },
            /* 4 */
            {
                /* 0 */
                {
                    /* nAYnrFilter0Coef[25] */
                    {0, 1, 1, 1, 0, 1, 11, 30, 11, 1, 1, 30, 80, 30, 1, 1, 11, 30, 11, 1, 0, 1, 1, 1, 0, /*0 - 24*/},
                    /* nAYnrFilter1Coef[25] */
                    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 56, 136, 56, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 24*/},
                    /* nAYnrFilter2Coef[25] */
                    {0, 0, 4, 0, 0, 0, 0, 56, 0, 0, 0, 0, 136, 0, 0, 0, 0, 56, 0, 0, 0, 0, 4, 0, 0, /*0 - 24*/},
                    /* nAYnrFilter3Coef[25] */
                    {0, 0, 0, 0, 0, 0, 0, 0, 32, 0, 0, 0, 192, 0, 0, 0, 32, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 24*/},
                    /* nAYnrFilter4Coef[25] */
                    {0, 0, 0, 0, 0, 0, 32, 0, 0, 0, 0, 0, 192, 0, 0, 0, 0, 0, 32, 0, 0, 0, 0, 0, 0, /*0 - 24*/},
                },
                /* 1 */
                {
                    /* nAYnrFilter0Coef[25] */
                    {0, 1, 1, 1, 0, 1, 11, 30, 11, 1, 1, 30, 80, 30, 1, 1, 11, 30, 11, 1, 0, 1, 1, 1, 0, /*0 - 24*/},
                    /* nAYnrFilter1Coef[25] */
                    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 56, 136, 56, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 24*/},
                    /* nAYnrFilter2Coef[25] */
                    {0, 0, 4, 0, 0, 0, 0, 56, 0, 0, 0, 0, 136, 0, 0, 0, 0, 56, 0, 0, 0, 0, 4, 0, 0, /*0 - 24*/},
                    /* nAYnrFilter3Coef[25] */
                    {0, 0, 0, 0, 0, 0, 0, 0, 32, 0, 0, 0, 192, 0, 0, 0, 32, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 24*/},
                    /* nAYnrFilter4Coef[25] */
                    {0, 0, 0, 0, 0, 0, 32, 0, 0, 0, 0, 0, 192, 0, 0, 0, 0, 0, 32, 0, 0, 0, 0, 0, 0, /*0 - 24*/},
                },
                /* 2 */
                {
                    /* nAYnrFilter0Coef[25] */
                    {0, 1, 1, 1, 0, 1, 11, 30, 11, 1, 1, 30, 80, 30, 1, 1, 11, 30, 11, 1, 0, 1, 1, 1, 0, /*0 - 24*/},
                    /* nAYnrFilter1Coef[25] */
                    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 56, 136, 56, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 24*/},
                    /* nAYnrFilter2Coef[25] */
                    {0, 0, 4, 0, 0, 0, 0, 56, 0, 0, 0, 0, 136, 0, 0, 0, 0, 56, 0, 0, 0, 0, 4, 0, 0, /*0 - 24*/},
                    /* nAYnrFilter3Coef[25] */
                    {0, 0, 0, 0, 0, 0, 0, 0, 32, 0, 0, 0, 192, 0, 0, 0, 32, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 24*/},
                    /* nAYnrFilter4Coef[25] */
                    {0, 0, 0, 0, 0, 0, 32, 0, 0, 0, 0, 0, 192, 0, 0, 0, 0, 0, 32, 0, 0, 0, 0, 0, 0, /*0 - 24*/},
                },
                /* 3 */
                {
                    /* nAYnrFilter0Coef[25] */
                    {0, 1, 1, 1, 0, 1, 11, 30, 11, 1, 1, 30, 80, 30, 1, 1, 11, 30, 11, 1, 0, 1, 1, 1, 0, /*0 - 24*/},
                    /* nAYnrFilter1Coef[25] */
                    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 56, 136, 56, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 24*/},
                    /* nAYnrFilter2Coef[25] */
                    {0, 0, 4, 0, 0, 0, 0, 56, 0, 0, 0, 0, 136, 0, 0, 0, 0, 56, 0, 0, 0, 0, 4, 0, 0, /*0 - 24*/},
                    /* nAYnrFilter3Coef[25] */
                    {0, 0, 0, 0, 0, 0, 0, 0, 32, 0, 0, 0, 192, 0, 0, 0, 32, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 24*/},
                    /* nAYnrFilter4Coef[25] */
                    {0, 0, 0, 0, 0, 0, 32, 0, 0, 0, 0, 0, 192, 0, 0, 0, 0, 0, 32, 0, 0, 0, 0, 0, 0, /*0 - 24*/},
                },
            },
            /* 5 */
            {
                /* 0 */
                {
                    /* nAYnrFilter0Coef[25] */
                    {0, 1, 1, 1, 0, 1, 11, 30, 11, 1, 1, 30, 80, 30, 1, 1, 11, 30, 11, 1, 0, 1, 1, 1, 0, /*0 - 24*/},
                    /* nAYnrFilter1Coef[25] */
                    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 56, 136, 56, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 24*/},
                    /* nAYnrFilter2Coef[25] */
                    {0, 0, 4, 0, 0, 0, 0, 56, 0, 0, 0, 0, 136, 0, 0, 0, 0, 56, 0, 0, 0, 0, 4, 0, 0, /*0 - 24*/},
                    /* nAYnrFilter3Coef[25] */
                    {0, 0, 0, 0, 0, 0, 0, 0, 32, 0, 0, 0, 192, 0, 0, 0, 32, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 24*/},
                    /* nAYnrFilter4Coef[25] */
                    {0, 0, 0, 0, 0, 0, 32, 0, 0, 0, 0, 0, 192, 0, 0, 0, 0, 0, 32, 0, 0, 0, 0, 0, 0, /*0 - 24*/},
                },
                /* 1 */
                {
                    /* nAYnrFilter0Coef[25] */
                    {0, 1, 1, 1, 0, 1, 11, 30, 11, 1, 1, 30, 80, 30, 1, 1, 11, 30, 11, 1, 0, 1, 1, 1, 0, /*0 - 24*/},
                    /* nAYnrFilter1Coef[25] */
                    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 56, 136, 56, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 24*/},
                    /* nAYnrFilter2Coef[25] */
                    {0, 0, 4, 0, 0, 0, 0, 56, 0, 0, 0, 0, 136, 0, 0, 0, 0, 56, 0, 0, 0, 0, 4, 0, 0, /*0 - 24*/},
                    /* nAYnrFilter3Coef[25] */
                    {0, 0, 0, 0, 0, 0, 0, 0, 32, 0, 0, 0, 192, 0, 0, 0, 32, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 24*/},
                    /* nAYnrFilter4Coef[25] */
                    {0, 0, 0, 0, 0, 0, 32, 0, 0, 0, 0, 0, 192, 0, 0, 0, 0, 0, 32, 0, 0, 0, 0, 0, 0, /*0 - 24*/},
                },
                /* 2 */
                {
                    /* nAYnrFilter0Coef[25] */
                    {0, 1, 1, 1, 0, 1, 11, 30, 11, 1, 1, 30, 80, 30, 1, 1, 11, 30, 11, 1, 0, 1, 1, 1, 0, /*0 - 24*/},
                    /* nAYnrFilter1Coef[25] */
                    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 56, 136, 56, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 24*/},
                    /* nAYnrFilter2Coef[25] */
                    {0, 0, 4, 0, 0, 0, 0, 56, 0, 0, 0, 0, 136, 0, 0, 0, 0, 56, 0, 0, 0, 0, 4, 0, 0, /*0 - 24*/},
                    /* nAYnrFilter3Coef[25] */
                    {0, 0, 0, 0, 0, 0, 0, 0, 32, 0, 0, 0, 192, 0, 0, 0, 32, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 24*/},
                    /* nAYnrFilter4Coef[25] */
                    {0, 0, 0, 0, 0, 0, 32, 0, 0, 0, 0, 0, 192, 0, 0, 0, 0, 0, 32, 0, 0, 0, 0, 0, 0, /*0 - 24*/},
                },
                /* 3 */
                {
                    /* nAYnrFilter0Coef[25] */
                    {0, 1, 1, 1, 0, 1, 11, 30, 11, 1, 1, 30, 80, 30, 1, 1, 11, 30, 11, 1, 0, 1, 1, 1, 0, /*0 - 24*/},
                    /* nAYnrFilter1Coef[25] */
                    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 56, 136, 56, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 24*/},
                    /* nAYnrFilter2Coef[25] */
                    {0, 0, 4, 0, 0, 0, 0, 56, 0, 0, 0, 0, 136, 0, 0, 0, 0, 56, 0, 0, 0, 0, 4, 0, 0, /*0 - 24*/},
                    /* nAYnrFilter3Coef[25] */
                    {0, 0, 0, 0, 0, 0, 0, 0, 32, 0, 0, 0, 192, 0, 0, 0, 32, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 24*/},
                    /* nAYnrFilter4Coef[25] */
                    {0, 0, 0, 0, 0, 0, 32, 0, 0, 0, 0, 0, 192, 0, 0, 0, 0, 0, 32, 0, 0, 0, 0, 0, 0, /*0 - 24*/},
                },
            },
            /* 6 */
            {
                /* 0 */
                {
                    /* nAYnrFilter0Coef[25] */
                    {0, 1, 1, 1, 0, 1, 11, 30, 11, 1, 1, 30, 80, 30, 1, 1, 11, 30, 11, 1, 0, 1, 1, 1, 0, /*0 - 24*/},
                    /* nAYnrFilter1Coef[25] */
                    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 56, 136, 56, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 24*/},
                    /* nAYnrFilter2Coef[25] */
                    {0, 0, 4, 0, 0, 0, 0, 56, 0, 0, 0, 0, 136, 0, 0, 0, 0, 56, 0, 0, 0, 0, 4, 0, 0, /*0 - 24*/},
                    /* nAYnrFilter3Coef[25] */
                    {0, 0, 0, 0, 0, 0, 0, 0, 32, 0, 0, 0, 192, 0, 0, 0, 32, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 24*/},
                    /* nAYnrFilter4Coef[25] */
                    {0, 0, 0, 0, 0, 0, 32, 0, 0, 0, 0, 0, 192, 0, 0, 0, 0, 0, 32, 0, 0, 0, 0, 0, 0, /*0 - 24*/},
                },
                /* 1 */
                {
                    /* nAYnrFilter0Coef[25] */
                    {0, 1, 1, 1, 0, 1, 11, 30, 11, 1, 1, 30, 80, 30, 1, 1, 11, 30, 11, 1, 0, 1, 1, 1, 0, /*0 - 24*/},
                    /* nAYnrFilter1Coef[25] */
                    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 56, 136, 56, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 24*/},
                    /* nAYnrFilter2Coef[25] */
                    {0, 0, 4, 0, 0, 0, 0, 56, 0, 0, 0, 0, 136, 0, 0, 0, 0, 56, 0, 0, 0, 0, 4, 0, 0, /*0 - 24*/},
                    /* nAYnrFilter3Coef[25] */
                    {0, 0, 0, 0, 0, 0, 0, 0, 32, 0, 0, 0, 192, 0, 0, 0, 32, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 24*/},
                    /* nAYnrFilter4Coef[25] */
                    {0, 0, 0, 0, 0, 0, 32, 0, 0, 0, 0, 0, 192, 0, 0, 0, 0, 0, 32, 0, 0, 0, 0, 0, 0, /*0 - 24*/},
                },
                /* 2 */
                {
                    /* nAYnrFilter0Coef[25] */
                    {0, 1, 1, 1, 0, 1, 11, 30, 11, 1, 1, 30, 80, 30, 1, 1, 11, 30, 11, 1, 0, 1, 1, 1, 0, /*0 - 24*/},
                    /* nAYnrFilter1Coef[25] */
                    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 56, 136, 56, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 24*/},
                    /* nAYnrFilter2Coef[25] */
                    {0, 0, 4, 0, 0, 0, 0, 56, 0, 0, 0, 0, 136, 0, 0, 0, 0, 56, 0, 0, 0, 0, 4, 0, 0, /*0 - 24*/},
                    /* nAYnrFilter3Coef[25] */
                    {0, 0, 0, 0, 0, 0, 0, 0, 32, 0, 0, 0, 192, 0, 0, 0, 32, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 24*/},
                    /* nAYnrFilter4Coef[25] */
                    {0, 0, 0, 0, 0, 0, 32, 0, 0, 0, 0, 0, 192, 0, 0, 0, 0, 0, 32, 0, 0, 0, 0, 0, 0, /*0 - 24*/},
                },
                /* 3 */
                {
                    /* nAYnrFilter0Coef[25] */
                    {0, 1, 1, 1, 0, 1, 11, 30, 11, 1, 1, 30, 80, 30, 1, 1, 11, 30, 11, 1, 0, 1, 1, 1, 0, /*0 - 24*/},
                    /* nAYnrFilter1Coef[25] */
                    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 56, 136, 56, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 24*/},
                    /* nAYnrFilter2Coef[25] */
                    {0, 0, 4, 0, 0, 0, 0, 56, 0, 0, 0, 0, 136, 0, 0, 0, 0, 56, 0, 0, 0, 0, 4, 0, 0, /*0 - 24*/},
                    /* nAYnrFilter3Coef[25] */
                    {0, 0, 0, 0, 0, 0, 0, 0, 32, 0, 0, 0, 192, 0, 0, 0, 32, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 24*/},
                    /* nAYnrFilter4Coef[25] */
                    {0, 0, 0, 0, 0, 0, 32, 0, 0, 0, 0, 0, 192, 0, 0, 0, 0, 0, 32, 0, 0, 0, 0, 0, 0, /*0 - 24*/},
                },
            },
            /* 7 */
            {
                /* 0 */
                {
                    /* nAYnrFilter0Coef[25] */
                    {0, 1, 1, 1, 0, 1, 11, 30, 11, 1, 1, 30, 80, 30, 1, 1, 11, 30, 11, 1, 0, 1, 1, 1, 0, /*0 - 24*/},
                    /* nAYnrFilter1Coef[25] */
                    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 56, 136, 56, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 24*/},
                    /* nAYnrFilter2Coef[25] */
                    {0, 0, 4, 0, 0, 0, 0, 56, 0, 0, 0, 0, 136, 0, 0, 0, 0, 56, 0, 0, 0, 0, 4, 0, 0, /*0 - 24*/},
                    /* nAYnrFilter3Coef[25] */
                    {0, 0, 0, 0, 0, 0, 0, 0, 32, 0, 0, 0, 192, 0, 0, 0, 32, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 24*/},
                    /* nAYnrFilter4Coef[25] */
                    {0, 0, 0, 0, 0, 0, 32, 0, 0, 0, 0, 0, 192, 0, 0, 0, 0, 0, 32, 0, 0, 0, 0, 0, 0, /*0 - 24*/},
                },
                /* 1 */
                {
                    /* nAYnrFilter0Coef[25] */
                    {0, 1, 1, 1, 0, 1, 11, 30, 11, 1, 1, 30, 80, 30, 1, 1, 11, 30, 11, 1, 0, 1, 1, 1, 0, /*0 - 24*/},
                    /* nAYnrFilter1Coef[25] */
                    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 56, 136, 56, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 24*/},
                    /* nAYnrFilter2Coef[25] */
                    {0, 0, 4, 0, 0, 0, 0, 56, 0, 0, 0, 0, 136, 0, 0, 0, 0, 56, 0, 0, 0, 0, 4, 0, 0, /*0 - 24*/},
                    /* nAYnrFilter3Coef[25] */
                    {0, 0, 0, 0, 0, 0, 0, 0, 32, 0, 0, 0, 192, 0, 0, 0, 32, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 24*/},
                    /* nAYnrFilter4Coef[25] */
                    {0, 0, 0, 0, 0, 0, 32, 0, 0, 0, 0, 0, 192, 0, 0, 0, 0, 0, 32, 0, 0, 0, 0, 0, 0, /*0 - 24*/},
                },
                /* 2 */
                {
                    /* nAYnrFilter0Coef[25] */
                    {0, 1, 1, 1, 0, 1, 11, 30, 11, 1, 1, 30, 80, 30, 1, 1, 11, 30, 11, 1, 0, 1, 1, 1, 0, /*0 - 24*/},
                    /* nAYnrFilter1Coef[25] */
                    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 56, 136, 56, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 24*/},
                    /* nAYnrFilter2Coef[25] */
                    {0, 0, 4, 0, 0, 0, 0, 56, 0, 0, 0, 0, 136, 0, 0, 0, 0, 56, 0, 0, 0, 0, 4, 0, 0, /*0 - 24*/},
                    /* nAYnrFilter3Coef[25] */
                    {0, 0, 0, 0, 0, 0, 0, 0, 32, 0, 0, 0, 192, 0, 0, 0, 32, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 24*/},
                    /* nAYnrFilter4Coef[25] */
                    {0, 0, 0, 0, 0, 0, 32, 0, 0, 0, 0, 0, 192, 0, 0, 0, 0, 0, 32, 0, 0, 0, 0, 0, 0, /*0 - 24*/},
                },
                /* 3 */
                {
                    /* nAYnrFilter0Coef[25] */
                    {0, 1, 1, 1, 0, 1, 11, 30, 11, 1, 1, 30, 80, 30, 1, 1, 11, 30, 11, 1, 0, 1, 1, 1, 0, /*0 - 24*/},
                    /* nAYnrFilter1Coef[25] */
                    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 56, 136, 56, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 24*/},
                    /* nAYnrFilter2Coef[25] */
                    {0, 0, 4, 0, 0, 0, 0, 56, 0, 0, 0, 0, 136, 0, 0, 0, 0, 56, 0, 0, 0, 0, 4, 0, 0, /*0 - 24*/},
                    /* nAYnrFilter3Coef[25] */
                    {0, 0, 0, 0, 0, 0, 0, 0, 32, 0, 0, 0, 192, 0, 0, 0, 32, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 24*/},
                    /* nAYnrFilter4Coef[25] */
                    {0, 0, 0, 0, 0, 0, 32, 0, 0, 0, 0, 0, 192, 0, 0, 0, 0, 0, 32, 0, 0, 0, 0, 0, 0, /*0 - 24*/},
                },
            },
            /* 8 */
            {
                /* 0 */
                {
                    /* nAYnrFilter0Coef[25] */
                    {0, 1, 1, 1, 0, 1, 11, 30, 11, 1, 1, 30, 80, 30, 1, 1, 11, 30, 11, 1, 0, 1, 1, 1, 0, /*0 - 24*/},
                    /* nAYnrFilter1Coef[25] */
                    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 56, 136, 56, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 24*/},
                    /* nAYnrFilter2Coef[25] */
                    {0, 0, 4, 0, 0, 0, 0, 56, 0, 0, 0, 0, 136, 0, 0, 0, 0, 56, 0, 0, 0, 0, 4, 0, 0, /*0 - 24*/},
                    /* nAYnrFilter3Coef[25] */
                    {0, 0, 0, 0, 0, 0, 0, 0, 32, 0, 0, 0, 192, 0, 0, 0, 32, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 24*/},
                    /* nAYnrFilter4Coef[25] */
                    {0, 0, 0, 0, 0, 0, 32, 0, 0, 0, 0, 0, 192, 0, 0, 0, 0, 0, 32, 0, 0, 0, 0, 0, 0, /*0 - 24*/},
                },
                /* 1 */
                {
                    /* nAYnrFilter0Coef[25] */
                    {0, 1, 1, 1, 0, 1, 11, 30, 11, 1, 1, 30, 80, 30, 1, 1, 11, 30, 11, 1, 0, 1, 1, 1, 0, /*0 - 24*/},
                    /* nAYnrFilter1Coef[25] */
                    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 56, 136, 56, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 24*/},
                    /* nAYnrFilter2Coef[25] */
                    {0, 0, 4, 0, 0, 0, 0, 56, 0, 0, 0, 0, 136, 0, 0, 0, 0, 56, 0, 0, 0, 0, 4, 0, 0, /*0 - 24*/},
                    /* nAYnrFilter3Coef[25] */
                    {0, 0, 0, 0, 0, 0, 0, 0, 32, 0, 0, 0, 192, 0, 0, 0, 32, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 24*/},
                    /* nAYnrFilter4Coef[25] */
                    {0, 0, 0, 0, 0, 0, 32, 0, 0, 0, 0, 0, 192, 0, 0, 0, 0, 0, 32, 0, 0, 0, 0, 0, 0, /*0 - 24*/},
                },
                /* 2 */
                {
                    /* nAYnrFilter0Coef[25] */
                    {0, 1, 1, 1, 0, 1, 11, 30, 11, 1, 1, 30, 80, 30, 1, 1, 11, 30, 11, 1, 0, 1, 1, 1, 0, /*0 - 24*/},
                    /* nAYnrFilter1Coef[25] */
                    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 56, 136, 56, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 24*/},
                    /* nAYnrFilter2Coef[25] */
                    {0, 0, 4, 0, 0, 0, 0, 56, 0, 0, 0, 0, 136, 0, 0, 0, 0, 56, 0, 0, 0, 0, 4, 0, 0, /*0 - 24*/},
                    /* nAYnrFilter3Coef[25] */
                    {0, 0, 0, 0, 0, 0, 0, 0, 32, 0, 0, 0, 192, 0, 0, 0, 32, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 24*/},
                    /* nAYnrFilter4Coef[25] */
                    {0, 0, 0, 0, 0, 0, 32, 0, 0, 0, 0, 0, 192, 0, 0, 0, 0, 0, 32, 0, 0, 0, 0, 0, 0, /*0 - 24*/},
                },
                /* 3 */
                {
                    /* nAYnrFilter0Coef[25] */
                    {0, 1, 1, 1, 0, 1, 11, 30, 11, 1, 1, 30, 80, 30, 1, 1, 11, 30, 11, 1, 0, 1, 1, 1, 0, /*0 - 24*/},
                    /* nAYnrFilter1Coef[25] */
                    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 56, 136, 56, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 24*/},
                    /* nAYnrFilter2Coef[25] */
                    {0, 0, 4, 0, 0, 0, 0, 56, 0, 0, 0, 0, 136, 0, 0, 0, 0, 56, 0, 0, 0, 0, 4, 0, 0, /*0 - 24*/},
                    /* nAYnrFilter3Coef[25] */
                    {0, 0, 0, 0, 0, 0, 0, 0, 32, 0, 0, 0, 192, 0, 0, 0, 32, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 24*/},
                    /* nAYnrFilter4Coef[25] */
                    {0, 0, 0, 0, 0, 0, 32, 0, 0, 0, 0, 0, 192, 0, 0, 0, 0, 0, 32, 0, 0, 0, 0, 0, 0, /*0 - 24*/},
                },
            },
            /* 9 */
            {
                /* 0 */
                {
                    /* nAYnrFilter0Coef[25] */
                    {0, 1, 1, 1, 0, 1, 11, 30, 11, 1, 1, 30, 80, 30, 1, 1, 11, 30, 11, 1, 0, 1, 1, 1, 0, /*0 - 24*/},
                    /* nAYnrFilter1Coef[25] */
                    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 56, 136, 56, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 24*/},
                    /* nAYnrFilter2Coef[25] */
                    {0, 0, 4, 0, 0, 0, 0, 56, 0, 0, 0, 0, 136, 0, 0, 0, 0, 56, 0, 0, 0, 0, 4, 0, 0, /*0 - 24*/},
                    /* nAYnrFilter3Coef[25] */
                    {0, 0, 0, 0, 0, 0, 0, 0, 32, 0, 0, 0, 192, 0, 0, 0, 32, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 24*/},
                    /* nAYnrFilter4Coef[25] */
                    {0, 0, 0, 0, 0, 0, 32, 0, 0, 0, 0, 0, 192, 0, 0, 0, 0, 0, 32, 0, 0, 0, 0, 0, 0, /*0 - 24*/},
                },
                /* 1 */
                {
                    /* nAYnrFilter0Coef[25] */
                    {0, 1, 1, 1, 0, 1, 11, 30, 11, 1, 1, 30, 80, 30, 1, 1, 11, 30, 11, 1, 0, 1, 1, 1, 0, /*0 - 24*/},
                    /* nAYnrFilter1Coef[25] */
                    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 56, 136, 56, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 24*/},
                    /* nAYnrFilter2Coef[25] */
                    {0, 0, 4, 0, 0, 0, 0, 56, 0, 0, 0, 0, 136, 0, 0, 0, 0, 56, 0, 0, 0, 0, 4, 0, 0, /*0 - 24*/},
                    /* nAYnrFilter3Coef[25] */
                    {0, 0, 0, 0, 0, 0, 0, 0, 32, 0, 0, 0, 192, 0, 0, 0, 32, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 24*/},
                    /* nAYnrFilter4Coef[25] */
                    {0, 0, 0, 0, 0, 0, 32, 0, 0, 0, 0, 0, 192, 0, 0, 0, 0, 0, 32, 0, 0, 0, 0, 0, 0, /*0 - 24*/},
                },
                /* 2 */
                {
                    /* nAYnrFilter0Coef[25] */
                    {0, 1, 1, 1, 0, 1, 11, 30, 11, 1, 1, 30, 80, 30, 1, 1, 11, 30, 11, 1, 0, 1, 1, 1, 0, /*0 - 24*/},
                    /* nAYnrFilter1Coef[25] */
                    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 56, 136, 56, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 24*/},
                    /* nAYnrFilter2Coef[25] */
                    {0, 0, 4, 0, 0, 0, 0, 56, 0, 0, 0, 0, 136, 0, 0, 0, 0, 56, 0, 0, 0, 0, 4, 0, 0, /*0 - 24*/},
                    /* nAYnrFilter3Coef[25] */
                    {0, 0, 0, 0, 0, 0, 0, 0, 32, 0, 0, 0, 192, 0, 0, 0, 32, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 24*/},
                    /* nAYnrFilter4Coef[25] */
                    {0, 0, 0, 0, 0, 0, 32, 0, 0, 0, 0, 0, 192, 0, 0, 0, 0, 0, 32, 0, 0, 0, 0, 0, 0, /*0 - 24*/},
                },
                /* 3 */
                {
                    /* nAYnrFilter0Coef[25] */
                    {0, 1, 1, 1, 0, 1, 11, 30, 11, 1, 1, 30, 80, 30, 1, 1, 11, 30, 11, 1, 0, 1, 1, 1, 0, /*0 - 24*/},
                    /* nAYnrFilter1Coef[25] */
                    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 56, 136, 56, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 24*/},
                    /* nAYnrFilter2Coef[25] */
                    {0, 0, 4, 0, 0, 0, 0, 56, 0, 0, 0, 0, 136, 0, 0, 0, 0, 56, 0, 0, 0, 0, 4, 0, 0, /*0 - 24*/},
                    /* nAYnrFilter3Coef[25] */
                    {0, 0, 0, 0, 0, 0, 0, 0, 32, 0, 0, 0, 192, 0, 0, 0, 32, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 24*/},
                    /* nAYnrFilter4Coef[25] */
                    {0, 0, 0, 0, 0, 0, 32, 0, 0, 0, 0, 0, 192, 0, 0, 0, 0, 0, 32, 0, 0, 0, 0, 0, 0, /*0 - 24*/},
                },
            },
            /* 10 */
            {
                /* 0 */
                {
                    /* nAYnrFilter0Coef[25] */
                    {0, 1, 1, 1, 0, 1, 11, 30, 11, 1, 1, 30, 80, 30, 1, 1, 11, 30, 11, 1, 0, 1, 1, 1, 0, /*0 - 24*/},
                    /* nAYnrFilter1Coef[25] */
                    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 56, 136, 56, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 24*/},
                    /* nAYnrFilter2Coef[25] */
                    {0, 0, 4, 0, 0, 0, 0, 56, 0, 0, 0, 0, 136, 0, 0, 0, 0, 56, 0, 0, 0, 0, 4, 0, 0, /*0 - 24*/},
                    /* nAYnrFilter3Coef[25] */
                    {0, 0, 0, 0, 0, 0, 0, 0, 32, 0, 0, 0, 192, 0, 0, 0, 32, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 24*/},
                    /* nAYnrFilter4Coef[25] */
                    {0, 0, 0, 0, 0, 0, 32, 0, 0, 0, 0, 0, 192, 0, 0, 0, 0, 0, 32, 0, 0, 0, 0, 0, 0, /*0 - 24*/},
                },
                /* 1 */
                {
                    /* nAYnrFilter0Coef[25] */
                    {0, 1, 1, 1, 0, 1, 11, 30, 11, 1, 1, 30, 80, 30, 1, 1, 11, 30, 11, 1, 0, 1, 1, 1, 0, /*0 - 24*/},
                    /* nAYnrFilter1Coef[25] */
                    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 56, 136, 56, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 24*/},
                    /* nAYnrFilter2Coef[25] */
                    {0, 0, 4, 0, 0, 0, 0, 56, 0, 0, 0, 0, 136, 0, 0, 0, 0, 56, 0, 0, 0, 0, 4, 0, 0, /*0 - 24*/},
                    /* nAYnrFilter3Coef[25] */
                    {0, 0, 0, 0, 0, 0, 0, 0, 32, 0, 0, 0, 192, 0, 0, 0, 32, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 24*/},
                    /* nAYnrFilter4Coef[25] */
                    {0, 0, 0, 0, 0, 0, 32, 0, 0, 0, 0, 0, 192, 0, 0, 0, 0, 0, 32, 0, 0, 0, 0, 0, 0, /*0 - 24*/},
                },
                /* 2 */
                {
                    /* nAYnrFilter0Coef[25] */
                    {0, 1, 1, 1, 0, 1, 11, 30, 11, 1, 1, 30, 80, 30, 1, 1, 11, 30, 11, 1, 0, 1, 1, 1, 0, /*0 - 24*/},
                    /* nAYnrFilter1Coef[25] */
                    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 56, 136, 56, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 24*/},
                    /* nAYnrFilter2Coef[25] */
                    {0, 0, 4, 0, 0, 0, 0, 56, 0, 0, 0, 0, 136, 0, 0, 0, 0, 56, 0, 0, 0, 0, 4, 0, 0, /*0 - 24*/},
                    /* nAYnrFilter3Coef[25] */
                    {0, 0, 0, 0, 0, 0, 0, 0, 32, 0, 0, 0, 192, 0, 0, 0, 32, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 24*/},
                    /* nAYnrFilter4Coef[25] */
                    {0, 0, 0, 0, 0, 0, 32, 0, 0, 0, 0, 0, 192, 0, 0, 0, 0, 0, 32, 0, 0, 0, 0, 0, 0, /*0 - 24*/},
                },
                /* 3 */
                {
                    /* nAYnrFilter0Coef[25] */
                    {0, 1, 1, 1, 0, 1, 11, 30, 11, 1, 1, 30, 80, 30, 1, 1, 11, 30, 11, 1, 0, 1, 1, 1, 0, /*0 - 24*/},
                    /* nAYnrFilter1Coef[25] */
                    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 56, 136, 56, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 24*/},
                    /* nAYnrFilter2Coef[25] */
                    {0, 0, 4, 0, 0, 0, 0, 56, 0, 0, 0, 0, 136, 0, 0, 0, 0, 56, 0, 0, 0, 0, 4, 0, 0, /*0 - 24*/},
                    /* nAYnrFilter3Coef[25] */
                    {0, 0, 0, 0, 0, 0, 0, 0, 32, 0, 0, 0, 192, 0, 0, 0, 32, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 24*/},
                    /* nAYnrFilter4Coef[25] */
                    {0, 0, 0, 0, 0, 0, 32, 0, 0, 0, 0, 0, 192, 0, 0, 0, 0, 0, 32, 0, 0, 0, 0, 0, 0, /*0 - 24*/},
                },
            },
            /* 11 */
            {
                /* 0 */
                {
                    /* nAYnrFilter0Coef[25] */
                    {0, 1, 1, 1, 0, 1, 11, 30, 11, 1, 1, 30, 80, 30, 1, 1, 11, 30, 11, 1, 0, 1, 1, 1, 0, /*0 - 24*/},
                    /* nAYnrFilter1Coef[25] */
                    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 56, 136, 56, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 24*/},
                    /* nAYnrFilter2Coef[25] */
                    {0, 0, 4, 0, 0, 0, 0, 56, 0, 0, 0, 0, 136, 0, 0, 0, 0, 56, 0, 0, 0, 0, 4, 0, 0, /*0 - 24*/},
                    /* nAYnrFilter3Coef[25] */
                    {0, 0, 0, 0, 0, 0, 0, 0, 32, 0, 0, 0, 192, 0, 0, 0, 32, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 24*/},
                    /* nAYnrFilter4Coef[25] */
                    {0, 0, 0, 0, 0, 0, 32, 0, 0, 0, 0, 0, 192, 0, 0, 0, 0, 0, 32, 0, 0, 0, 0, 0, 0, /*0 - 24*/},
                },
                /* 1 */
                {
                    /* nAYnrFilter0Coef[25] */
                    {0, 1, 1, 1, 0, 1, 11, 30, 11, 1, 1, 30, 80, 30, 1, 1, 11, 30, 11, 1, 0, 1, 1, 1, 0, /*0 - 24*/},
                    /* nAYnrFilter1Coef[25] */
                    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 56, 136, 56, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 24*/},
                    /* nAYnrFilter2Coef[25] */
                    {0, 0, 4, 0, 0, 0, 0, 56, 0, 0, 0, 0, 136, 0, 0, 0, 0, 56, 0, 0, 0, 0, 4, 0, 0, /*0 - 24*/},
                    /* nAYnrFilter3Coef[25] */
                    {0, 0, 0, 0, 0, 0, 0, 0, 32, 0, 0, 0, 192, 0, 0, 0, 32, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 24*/},
                    /* nAYnrFilter4Coef[25] */
                    {0, 0, 0, 0, 0, 0, 32, 0, 0, 0, 0, 0, 192, 0, 0, 0, 0, 0, 32, 0, 0, 0, 0, 0, 0, /*0 - 24*/},
                },
                /* 2 */
                {
                    /* nAYnrFilter0Coef[25] */
                    {0, 1, 1, 1, 0, 1, 11, 30, 11, 1, 1, 30, 80, 30, 1, 1, 11, 30, 11, 1, 0, 1, 1, 1, 0, /*0 - 24*/},
                    /* nAYnrFilter1Coef[25] */
                    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 56, 136, 56, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 24*/},
                    /* nAYnrFilter2Coef[25] */
                    {0, 0, 4, 0, 0, 0, 0, 56, 0, 0, 0, 0, 136, 0, 0, 0, 0, 56, 0, 0, 0, 0, 4, 0, 0, /*0 - 24*/},
                    /* nAYnrFilter3Coef[25] */
                    {0, 0, 0, 0, 0, 0, 0, 0, 32, 0, 0, 0, 192, 0, 0, 0, 32, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 24*/},
                    /* nAYnrFilter4Coef[25] */
                    {0, 0, 0, 0, 0, 0, 32, 0, 0, 0, 0, 0, 192, 0, 0, 0, 0, 0, 32, 0, 0, 0, 0, 0, 0, /*0 - 24*/},
                },
                /* 3 */
                {
                    /* nAYnrFilter0Coef[25] */
                    {0, 1, 1, 1, 0, 1, 11, 30, 11, 1, 1, 30, 80, 30, 1, 1, 11, 30, 11, 1, 0, 1, 1, 1, 0, /*0 - 24*/},
                    /* nAYnrFilter1Coef[25] */
                    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 56, 136, 56, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 24*/},
                    /* nAYnrFilter2Coef[25] */
                    {0, 0, 4, 0, 0, 0, 0, 56, 0, 0, 0, 0, 136, 0, 0, 0, 0, 56, 0, 0, 0, 0, 4, 0, 0, /*0 - 24*/},
                    /* nAYnrFilter3Coef[25] */
                    {0, 0, 0, 0, 0, 0, 0, 0, 32, 0, 0, 0, 192, 0, 0, 0, 32, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 24*/},
                    /* nAYnrFilter4Coef[25] */
                    {0, 0, 0, 0, 0, 0, 32, 0, 0, 0, 0, 0, 192, 0, 0, 0, 0, 0, 32, 0, 0, 0, 0, 0, 0, /*0 - 24*/},
                },
            },
        },
        /* tAYnrFilterStrength */
        {
            /* 0 */
            {
                /* 0 */
                {
                    /* nAYnrFilter0Strength */
                    128,
                    /* nAYnrFilter1Strength[2] */
                    {192, 16, /*0 - 1*/},
                    /* nAYnrFilter2Strength[2] */
                    {192, 16, /*0 - 1*/},
                    /* nAYnrFilter3Strength[2] */
                    {192, 16, /*0 - 1*/},
                    /* nAYnrFilter4Strength[2] */
                    {192, 16, /*0 - 1*/},
                },
                /* 1 */
                {
                    /* nAYnrFilter0Strength */
                    128,
                    /* nAYnrFilter1Strength[2] */
                    {192, 16, /*0 - 1*/},
                    /* nAYnrFilter2Strength[2] */
                    {192, 16, /*0 - 1*/},
                    /* nAYnrFilter3Strength[2] */
                    {192, 16, /*0 - 1*/},
                    /* nAYnrFilter4Strength[2] */
                    {192, 16, /*0 - 1*/},
                },
                /* 2 */
                {
                    /* nAYnrFilter0Strength */
                    128,
                    /* nAYnrFilter1Strength[2] */
                    {128, 64, /*0 - 1*/},
                    /* nAYnrFilter2Strength[2] */
                    {128, 64, /*0 - 1*/},
                    /* nAYnrFilter3Strength[2] */
                    {128, 64, /*0 - 1*/},
                    /* nAYnrFilter4Strength[2] */
                    {128, 64, /*0 - 1*/},
                },
                /* 3 */
                {
                    /* nAYnrFilter0Strength */
                    128,
                    /* nAYnrFilter1Strength[2] */
                    {128, 64, /*0 - 1*/},
                    /* nAYnrFilter2Strength[2] */
                    {128, 64, /*0 - 1*/},
                    /* nAYnrFilter3Strength[2] */
                    {128, 64, /*0 - 1*/},
                    /* nAYnrFilter4Strength[2] */
                    {128, 64, /*0 - 1*/},
                },
            },
            /* 1 */
            {
                /* 0 */
                {
                    /* nAYnrFilter0Strength */
                    128,
                    /* nAYnrFilter1Strength[2] */
                    {192, 16, /*0 - 1*/},
                    /* nAYnrFilter2Strength[2] */
                    {192, 16, /*0 - 1*/},
                    /* nAYnrFilter3Strength[2] */
                    {192, 16, /*0 - 1*/},
                    /* nAYnrFilter4Strength[2] */
                    {192, 16, /*0 - 1*/},
                },
                /* 1 */
                {
                    /* nAYnrFilter0Strength */
                    128,
                    /* nAYnrFilter1Strength[2] */
                    {192, 16, /*0 - 1*/},
                    /* nAYnrFilter2Strength[2] */
                    {192, 16, /*0 - 1*/},
                    /* nAYnrFilter3Strength[2] */
                    {192, 16, /*0 - 1*/},
                    /* nAYnrFilter4Strength[2] */
                    {192, 16, /*0 - 1*/},
                },
                /* 2 */
                {
                    /* nAYnrFilter0Strength */
                    128,
                    /* nAYnrFilter1Strength[2] */
                    {128, 64, /*0 - 1*/},
                    /* nAYnrFilter2Strength[2] */
                    {128, 64, /*0 - 1*/},
                    /* nAYnrFilter3Strength[2] */
                    {128, 64, /*0 - 1*/},
                    /* nAYnrFilter4Strength[2] */
                    {128, 64, /*0 - 1*/},
                },
                /* 3 */
                {
                    /* nAYnrFilter0Strength */
                    128,
                    /* nAYnrFilter1Strength[2] */
                    {128, 64, /*0 - 1*/},
                    /* nAYnrFilter2Strength[2] */
                    {128, 64, /*0 - 1*/},
                    /* nAYnrFilter3Strength[2] */
                    {128, 64, /*0 - 1*/},
                    /* nAYnrFilter4Strength[2] */
                    {128, 64, /*0 - 1*/},
                },
            },
            /* 2 */
            {
                /* 0 */
                {
                    /* nAYnrFilter0Strength */
                    128,
                    /* nAYnrFilter1Strength[2] */
                    {192, 16, /*0 - 1*/},
                    /* nAYnrFilter2Strength[2] */
                    {192, 16, /*0 - 1*/},
                    /* nAYnrFilter3Strength[2] */
                    {192, 16, /*0 - 1*/},
                    /* nAYnrFilter4Strength[2] */
                    {192, 16, /*0 - 1*/},
                },
                /* 1 */
                {
                    /* nAYnrFilter0Strength */
                    128,
                    /* nAYnrFilter1Strength[2] */
                    {192, 16, /*0 - 1*/},
                    /* nAYnrFilter2Strength[2] */
                    {192, 16, /*0 - 1*/},
                    /* nAYnrFilter3Strength[2] */
                    {192, 16, /*0 - 1*/},
                    /* nAYnrFilter4Strength[2] */
                    {192, 16, /*0 - 1*/},
                },
                /* 2 */
                {
                    /* nAYnrFilter0Strength */
                    128,
                    /* nAYnrFilter1Strength[2] */
                    {128, 64, /*0 - 1*/},
                    /* nAYnrFilter2Strength[2] */
                    {128, 64, /*0 - 1*/},
                    /* nAYnrFilter3Strength[2] */
                    {128, 64, /*0 - 1*/},
                    /* nAYnrFilter4Strength[2] */
                    {128, 64, /*0 - 1*/},
                },
                /* 3 */
                {
                    /* nAYnrFilter0Strength */
                    128,
                    /* nAYnrFilter1Strength[2] */
                    {128, 64, /*0 - 1*/},
                    /* nAYnrFilter2Strength[2] */
                    {128, 64, /*0 - 1*/},
                    /* nAYnrFilter3Strength[2] */
                    {128, 64, /*0 - 1*/},
                    /* nAYnrFilter4Strength[2] */
                    {128, 64, /*0 - 1*/},
                },
            },
            /* 3 */
            {
                /* 0 */
                {
                    /* nAYnrFilter0Strength */
                    64,
                    /* nAYnrFilter1Strength[2] */
                    {192, 16, /*0 - 1*/},
                    /* nAYnrFilter2Strength[2] */
                    {192, 16, /*0 - 1*/},
                    /* nAYnrFilter3Strength[2] */
                    {192, 16, /*0 - 1*/},
                    /* nAYnrFilter4Strength[2] */
                    {192, 16, /*0 - 1*/},
                },
                /* 1 */
                {
                    /* nAYnrFilter0Strength */
                    128,
                    /* nAYnrFilter1Strength[2] */
                    {192, 16, /*0 - 1*/},
                    /* nAYnrFilter2Strength[2] */
                    {192, 16, /*0 - 1*/},
                    /* nAYnrFilter3Strength[2] */
                    {192, 16, /*0 - 1*/},
                    /* nAYnrFilter4Strength[2] */
                    {192, 16, /*0 - 1*/},
                },
                /* 2 */
                {
                    /* nAYnrFilter0Strength */
                    256,
                    /* nAYnrFilter1Strength[2] */
                    {256, 0, /*0 - 1*/},
                    /* nAYnrFilter2Strength[2] */
                    {256, 0, /*0 - 1*/},
                    /* nAYnrFilter3Strength[2] */
                    {128, 64, /*0 - 1*/},
                    /* nAYnrFilter4Strength[2] */
                    {128, 64, /*0 - 1*/},
                },
                /* 3 */
                {
                    /* nAYnrFilter0Strength */
                    256,
                    /* nAYnrFilter1Strength[2] */
                    {256, 0, /*0 - 1*/},
                    /* nAYnrFilter2Strength[2] */
                    {256, 0, /*0 - 1*/},
                    /* nAYnrFilter3Strength[2] */
                    {128, 64, /*0 - 1*/},
                    /* nAYnrFilter4Strength[2] */
                    {128, 64, /*0 - 1*/},
                },
            },
            /* 4 */
            {
                /* 0 */
                {
                    /* nAYnrFilter0Strength */
                    64,
                    /* nAYnrFilter1Strength[2] */
                    {192, 16, /*0 - 1*/},
                    /* nAYnrFilter2Strength[2] */
                    {192, 16, /*0 - 1*/},
                    /* nAYnrFilter3Strength[2] */
                    {192, 16, /*0 - 1*/},
                    /* nAYnrFilter4Strength[2] */
                    {192, 16, /*0 - 1*/},
                },
                /* 1 */
                {
                    /* nAYnrFilter0Strength */
                    128,
                    /* nAYnrFilter1Strength[2] */
                    {192, 16, /*0 - 1*/},
                    /* nAYnrFilter2Strength[2] */
                    {192, 16, /*0 - 1*/},
                    /* nAYnrFilter3Strength[2] */
                    {192, 16, /*0 - 1*/},
                    /* nAYnrFilter4Strength[2] */
                    {192, 16, /*0 - 1*/},
                },
                /* 2 */
                {
                    /* nAYnrFilter0Strength */
                    256,
                    /* nAYnrFilter1Strength[2] */
                    {256, 0, /*0 - 1*/},
                    /* nAYnrFilter2Strength[2] */
                    {256, 0, /*0 - 1*/},
                    /* nAYnrFilter3Strength[2] */
                    {128, 64, /*0 - 1*/},
                    /* nAYnrFilter4Strength[2] */
                    {128, 64, /*0 - 1*/},
                },
                /* 3 */
                {
                    /* nAYnrFilter0Strength */
                    256,
                    /* nAYnrFilter1Strength[2] */
                    {256, 0, /*0 - 1*/},
                    /* nAYnrFilter2Strength[2] */
                    {256, 0, /*0 - 1*/},
                    /* nAYnrFilter3Strength[2] */
                    {128, 64, /*0 - 1*/},
                    /* nAYnrFilter4Strength[2] */
                    {128, 64, /*0 - 1*/},
                },
            },
            /* 5 */
            {
                /* 0 */
                {
                    /* nAYnrFilter0Strength */
                    64,
                    /* nAYnrFilter1Strength[2] */
                    {192, 16, /*0 - 1*/},
                    /* nAYnrFilter2Strength[2] */
                    {192, 16, /*0 - 1*/},
                    /* nAYnrFilter3Strength[2] */
                    {192, 16, /*0 - 1*/},
                    /* nAYnrFilter4Strength[2] */
                    {192, 16, /*0 - 1*/},
                },
                /* 1 */
                {
                    /* nAYnrFilter0Strength */
                    128,
                    /* nAYnrFilter1Strength[2] */
                    {192, 16, /*0 - 1*/},
                    /* nAYnrFilter2Strength[2] */
                    {192, 16, /*0 - 1*/},
                    /* nAYnrFilter3Strength[2] */
                    {192, 16, /*0 - 1*/},
                    /* nAYnrFilter4Strength[2] */
                    {192, 16, /*0 - 1*/},
                },
                /* 2 */
                {
                    /* nAYnrFilter0Strength */
                    256,
                    /* nAYnrFilter1Strength[2] */
                    {256, 0, /*0 - 1*/},
                    /* nAYnrFilter2Strength[2] */
                    {256, 0, /*0 - 1*/},
                    /* nAYnrFilter3Strength[2] */
                    {128, 64, /*0 - 1*/},
                    /* nAYnrFilter4Strength[2] */
                    {128, 64, /*0 - 1*/},
                },
                /* 3 */
                {
                    /* nAYnrFilter0Strength */
                    256,
                    /* nAYnrFilter1Strength[2] */
                    {256, 0, /*0 - 1*/},
                    /* nAYnrFilter2Strength[2] */
                    {256, 0, /*0 - 1*/},
                    /* nAYnrFilter3Strength[2] */
                    {128, 64, /*0 - 1*/},
                    /* nAYnrFilter4Strength[2] */
                    {128, 64, /*0 - 1*/},
                },
            },
            /* 6 */
            {
                /* 0 */
                {
                    /* nAYnrFilter0Strength */
                    128,
                    /* nAYnrFilter1Strength[2] */
                    {192, 16, /*0 - 1*/},
                    /* nAYnrFilter2Strength[2] */
                    {192, 16, /*0 - 1*/},
                    /* nAYnrFilter3Strength[2] */
                    {192, 16, /*0 - 1*/},
                    /* nAYnrFilter4Strength[2] */
                    {192, 16, /*0 - 1*/},
                },
                /* 1 */
                {
                    /* nAYnrFilter0Strength */
                    128,
                    /* nAYnrFilter1Strength[2] */
                    {192, 16, /*0 - 1*/},
                    /* nAYnrFilter2Strength[2] */
                    {192, 16, /*0 - 1*/},
                    /* nAYnrFilter3Strength[2] */
                    {192, 16, /*0 - 1*/},
                    /* nAYnrFilter4Strength[2] */
                    {192, 16, /*0 - 1*/},
                },
                /* 2 */
                {
                    /* nAYnrFilter0Strength */
                    256,
                    /* nAYnrFilter1Strength[2] */
                    {256, 0, /*0 - 1*/},
                    /* nAYnrFilter2Strength[2] */
                    {256, 0, /*0 - 1*/},
                    /* nAYnrFilter3Strength[2] */
                    {128, 64, /*0 - 1*/},
                    /* nAYnrFilter4Strength[2] */
                    {128, 64, /*0 - 1*/},
                },
                /* 3 */
                {
                    /* nAYnrFilter0Strength */
                    256,
                    /* nAYnrFilter1Strength[2] */
                    {256, 0, /*0 - 1*/},
                    /* nAYnrFilter2Strength[2] */
                    {256, 0, /*0 - 1*/},
                    /* nAYnrFilter3Strength[2] */
                    {128, 64, /*0 - 1*/},
                    /* nAYnrFilter4Strength[2] */
                    {128, 64, /*0 - 1*/},
                },
            },
            /* 7 */
            {
                /* 0 */
                {
                    /* nAYnrFilter0Strength */
                    128,
                    /* nAYnrFilter1Strength[2] */
                    {192, 16, /*0 - 1*/},
                    /* nAYnrFilter2Strength[2] */
                    {192, 16, /*0 - 1*/},
                    /* nAYnrFilter3Strength[2] */
                    {192, 16, /*0 - 1*/},
                    /* nAYnrFilter4Strength[2] */
                    {192, 16, /*0 - 1*/},
                },
                /* 1 */
                {
                    /* nAYnrFilter0Strength */
                    128,
                    /* nAYnrFilter1Strength[2] */
                    {192, 16, /*0 - 1*/},
                    /* nAYnrFilter2Strength[2] */
                    {192, 16, /*0 - 1*/},
                    /* nAYnrFilter3Strength[2] */
                    {192, 16, /*0 - 1*/},
                    /* nAYnrFilter4Strength[2] */
                    {192, 16, /*0 - 1*/},
                },
                /* 2 */
                {
                    /* nAYnrFilter0Strength */
                    256,
                    /* nAYnrFilter1Strength[2] */
                    {256, 0, /*0 - 1*/},
                    /* nAYnrFilter2Strength[2] */
                    {256, 0, /*0 - 1*/},
                    /* nAYnrFilter3Strength[2] */
                    {128, 64, /*0 - 1*/},
                    /* nAYnrFilter4Strength[2] */
                    {128, 64, /*0 - 1*/},
                },
                /* 3 */
                {
                    /* nAYnrFilter0Strength */
                    256,
                    /* nAYnrFilter1Strength[2] */
                    {256, 0, /*0 - 1*/},
                    /* nAYnrFilter2Strength[2] */
                    {256, 0, /*0 - 1*/},
                    /* nAYnrFilter3Strength[2] */
                    {128, 64, /*0 - 1*/},
                    /* nAYnrFilter4Strength[2] */
                    {128, 64, /*0 - 1*/},
                },
            },
            /* 8 */
            {
                /* 0 */
                {
                    /* nAYnrFilter0Strength */
                    128,
                    /* nAYnrFilter1Strength[2] */
                    {192, 16, /*0 - 1*/},
                    /* nAYnrFilter2Strength[2] */
                    {192, 16, /*0 - 1*/},
                    /* nAYnrFilter3Strength[2] */
                    {192, 16, /*0 - 1*/},
                    /* nAYnrFilter4Strength[2] */
                    {192, 16, /*0 - 1*/},
                },
                /* 1 */
                {
                    /* nAYnrFilter0Strength */
                    128,
                    /* nAYnrFilter1Strength[2] */
                    {192, 16, /*0 - 1*/},
                    /* nAYnrFilter2Strength[2] */
                    {192, 16, /*0 - 1*/},
                    /* nAYnrFilter3Strength[2] */
                    {192, 16, /*0 - 1*/},
                    /* nAYnrFilter4Strength[2] */
                    {192, 16, /*0 - 1*/},
                },
                /* 2 */
                {
                    /* nAYnrFilter0Strength */
                    256,
                    /* nAYnrFilter1Strength[2] */
                    {256, 0, /*0 - 1*/},
                    /* nAYnrFilter2Strength[2] */
                    {256, 0, /*0 - 1*/},
                    /* nAYnrFilter3Strength[2] */
                    {128, 64, /*0 - 1*/},
                    /* nAYnrFilter4Strength[2] */
                    {128, 64, /*0 - 1*/},
                },
                /* 3 */
                {
                    /* nAYnrFilter0Strength */
                    256,
                    /* nAYnrFilter1Strength[2] */
                    {256, 0, /*0 - 1*/},
                    /* nAYnrFilter2Strength[2] */
                    {256, 0, /*0 - 1*/},
                    /* nAYnrFilter3Strength[2] */
                    {128, 64, /*0 - 1*/},
                    /* nAYnrFilter4Strength[2] */
                    {128, 64, /*0 - 1*/},
                },
            },
            /* 9 */
            {
                /* 0 */
                {
                    /* nAYnrFilter0Strength */
                    128,
                    /* nAYnrFilter1Strength[2] */
                    {192, 16, /*0 - 1*/},
                    /* nAYnrFilter2Strength[2] */
                    {192, 16, /*0 - 1*/},
                    /* nAYnrFilter3Strength[2] */
                    {192, 16, /*0 - 1*/},
                    /* nAYnrFilter4Strength[2] */
                    {192, 16, /*0 - 1*/},
                },
                /* 1 */
                {
                    /* nAYnrFilter0Strength */
                    128,
                    /* nAYnrFilter1Strength[2] */
                    {192, 16, /*0 - 1*/},
                    /* nAYnrFilter2Strength[2] */
                    {192, 16, /*0 - 1*/},
                    /* nAYnrFilter3Strength[2] */
                    {192, 16, /*0 - 1*/},
                    /* nAYnrFilter4Strength[2] */
                    {192, 16, /*0 - 1*/},
                },
                /* 2 */
                {
                    /* nAYnrFilter0Strength */
                    128,
                    /* nAYnrFilter1Strength[2] */
                    {192, 16, /*0 - 1*/},
                    /* nAYnrFilter2Strength[2] */
                    {192, 16, /*0 - 1*/},
                    /* nAYnrFilter3Strength[2] */
                    {192, 16, /*0 - 1*/},
                    /* nAYnrFilter4Strength[2] */
                    {192, 16, /*0 - 1*/},
                },
                /* 3 */
                {
                    /* nAYnrFilter0Strength */
                    256,
                    /* nAYnrFilter1Strength[2] */
                    {256, 0, /*0 - 1*/},
                    /* nAYnrFilter2Strength[2] */
                    {256, 0, /*0 - 1*/},
                    /* nAYnrFilter3Strength[2] */
                    {128, 64, /*0 - 1*/},
                    /* nAYnrFilter4Strength[2] */
                    {128, 64, /*0 - 1*/},
                },
            },
            /* 10 */
            {
                /* 0 */
                {
                    /* nAYnrFilter0Strength */
                    128,
                    /* nAYnrFilter1Strength[2] */
                    {192, 16, /*0 - 1*/},
                    /* nAYnrFilter2Strength[2] */
                    {192, 16, /*0 - 1*/},
                    /* nAYnrFilter3Strength[2] */
                    {192, 16, /*0 - 1*/},
                    /* nAYnrFilter4Strength[2] */
                    {192, 16, /*0 - 1*/},
                },
                /* 1 */
                {
                    /* nAYnrFilter0Strength */
                    128,
                    /* nAYnrFilter1Strength[2] */
                    {192, 16, /*0 - 1*/},
                    /* nAYnrFilter2Strength[2] */
                    {192, 16, /*0 - 1*/},
                    /* nAYnrFilter3Strength[2] */
                    {192, 16, /*0 - 1*/},
                    /* nAYnrFilter4Strength[2] */
                    {192, 16, /*0 - 1*/},
                },
                /* 2 */
                {
                    /* nAYnrFilter0Strength */
                    128,
                    /* nAYnrFilter1Strength[2] */
                    {192, 16, /*0 - 1*/},
                    /* nAYnrFilter2Strength[2] */
                    {192, 16, /*0 - 1*/},
                    /* nAYnrFilter3Strength[2] */
                    {192, 16, /*0 - 1*/},
                    /* nAYnrFilter4Strength[2] */
                    {192, 16, /*0 - 1*/},
                },
                /* 3 */
                {
                    /* nAYnrFilter0Strength */
                    256,
                    /* nAYnrFilter1Strength[2] */
                    {256, 0, /*0 - 1*/},
                    /* nAYnrFilter2Strength[2] */
                    {256, 0, /*0 - 1*/},
                    /* nAYnrFilter3Strength[2] */
                    {128, 64, /*0 - 1*/},
                    /* nAYnrFilter4Strength[2] */
                    {128, 64, /*0 - 1*/},
                },
            },
            /* 11 */
            {
                /* 0 */
                {
                    /* nAYnrFilter0Strength */
                    128,
                    /* nAYnrFilter1Strength[2] */
                    {192, 16, /*0 - 1*/},
                    /* nAYnrFilter2Strength[2] */
                    {192, 16, /*0 - 1*/},
                    /* nAYnrFilter3Strength[2] */
                    {192, 16, /*0 - 1*/},
                    /* nAYnrFilter4Strength[2] */
                    {192, 16, /*0 - 1*/},
                },
                /* 1 */
                {
                    /* nAYnrFilter0Strength */
                    128,
                    /* nAYnrFilter1Strength[2] */
                    {192, 16, /*0 - 1*/},
                    /* nAYnrFilter2Strength[2] */
                    {192, 16, /*0 - 1*/},
                    /* nAYnrFilter3Strength[2] */
                    {192, 16, /*0 - 1*/},
                    /* nAYnrFilter4Strength[2] */
                    {192, 16, /*0 - 1*/},
                },
                /* 2 */
                {
                    /* nAYnrFilter0Strength */
                    128,
                    /* nAYnrFilter1Strength[2] */
                    {192, 16, /*0 - 1*/},
                    /* nAYnrFilter2Strength[2] */
                    {192, 16, /*0 - 1*/},
                    /* nAYnrFilter3Strength[2] */
                    {192, 16, /*0 - 1*/},
                    /* nAYnrFilter4Strength[2] */
                    {192, 16, /*0 - 1*/},
                },
                /* 3 */
                {
                    /* nAYnrFilter0Strength */
                    256,
                    /* nAYnrFilter1Strength[2] */
                    {256, 0, /*0 - 1*/},
                    /* nAYnrFilter2Strength[2] */
                    {256, 0, /*0 - 1*/},
                    /* nAYnrFilter3Strength[2] */
                    {128, 64, /*0 - 1*/},
                    /* nAYnrFilter4Strength[2] */
                    {128, 64, /*0 - 1*/},
                },
            },
        },
        /* nAYnrStrength[16][4] */
        {
            {128, 64, 16, 0, /*0 - 3*/},
            {128, 64, 16, 0, /*0 - 3*/},
            {128, 64, 32, 0, /*0 - 3*/},
            {128, 64, 32, 0, /*0 - 3*/},
            {128, 96, 96, 0, /*0 - 3*/},
            {200, 96, 96, 0, /*0 - 3*/},
            {200, 96, 96, 0, /*0 - 3*/},
            {200, 164, 128, 0, /*0 - 3*/},
            {200, 184, 168, 0, /*0 - 3*/},
            {256, 204, 198, 0, /*0 - 3*/},
            {256, 224, 218, 0, /*0 - 3*/},
            {256, 244, 238, 0, /*0 - 3*/},
        },
    },
};

static AX_ISP_IQ_CNR_PARAM_T cnr_param_sdr = {
    /* nCnrEn */
    1,
    /* nAutoMode */
    1,
    /* nRefMode */
    1,
    /* tManualParam */
    {
        /* nCnrLevel */
        16,
        /* nCnrInvNrLut[4] */
        {0, 0, 0, 0, /*0 - 3*/},
    },
    /* tAutoParam */
    {
        /* nParamGrpNum */
        12,
        /* nRefVal[16] */
        {1024, 2048, 16384, 32768, 49152, 65536, 65537, 131072, 262144, 393216, 524288, 1048576, /*0 - 11*/},
        /* nCnrLevel[16] */
        {16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, /*0 - 11*/},
        /* nCnrInvNrLut[16][4] */
        {
            {2047, 2047, 2047, 2047, /*0 - 3*/},
            {1024, 1024, 1024, 1024, /*0 - 3*/},
            {512, 512, 512, 512, /*0 - 3*/},
            {256, 256, 256, 256, /*0 - 3*/},
            {128, 128, 128, 128, /*0 - 3*/},
            {64, 64, 64, 64, /*0 - 3*/},
            {0, 0, 0, 0, /*0 - 3*/},
            {0, 0, 0, 0, /*0 - 3*/},
            {0, 0, 0, 0, /*0 - 3*/},
            {0, 0, 0, 0, /*0 - 3*/},
            {0, 0, 0, 0, /*0 - 3*/},
            {0, 0, 0, 0, /*0 - 3*/},
        },
    },
};

static AX_ISP_IQ_ACNR_PARAM_T acnr_param_sdr = {
    /* nAutoMode */
    1,
    /* nRefMode */
    1,
    /* tManualParam */
    {
        /* nACnrCoringThre[4] */
        {80, 80, 80, 80, /*0 - 3*/},
        /* nACnrCoringSlope[4] */
        {3, 3, 3, 3, /*0 - 3*/},
        /* nACnrEdgeThre[4] */
        {1024, 1024, 1024, 1024, /*0 - 3*/},
        /* nACnrEdgeSlope[4] */
        {0, 0, 0, 0, /*0 - 3*/},
        /* nACnrNvLut[36] */
        {
            16383, 16383, 16383, 16383, 16383, 16383, 16383, 16383, 16383, 16383, 16383, 16383, 16383, 16383, 16383, 16383, 16383, 16383, 16383, 16383, 16383, 16383, 16383, 16383, 16383, 16383, 16383, 32, 32, 32, 32, 32,  /* 0 - 31*/
            32, 32, 32, 32, /*32 - 35*/
        },
        /* nACnrStrength[4] */
        {256, 256, 256, 0, /*0 - 3*/},
    },
    /* tAutoParam */
    {
        /* nParamGrpNum */
        12,
        /* nRefVal[16] */
        {1024, 2048, 16384, 32768, 49152, 65536, 65537, 131072, 262144, 393216, 524288, 1048576, /*0 - 11*/},
        /* nACnrCoringThre[16][4] */
        {
            {80, 80, 80, 80, /*0 - 3*/},
            {80, 80, 80, 80, /*0 - 3*/},
            {80, 80, 80, 80, /*0 - 3*/},
            {80, 80, 80, 80, /*0 - 3*/},
            {80, 80, 80, 80, /*0 - 3*/},
            {80, 80, 80, 80, /*0 - 3*/},
            {80, 80, 80, 80, /*0 - 3*/},
            {80, 80, 80, 80, /*0 - 3*/},
            {80, 80, 80, 80, /*0 - 3*/},
            {80, 80, 80, 80, /*0 - 3*/},
            {80, 80, 80, 80, /*0 - 3*/},
            {80, 80, 80, 80, /*0 - 3*/},
        },
        /* nACnrCoringSlope[16][4] */
        {
            {3, 3, 3, 3, /*0 - 3*/},
            {3, 3, 3, 3, /*0 - 3*/},
            {3, 3, 3, 3, /*0 - 3*/},
            {3, 3, 3, 3, /*0 - 3*/},
            {3, 3, 3, 3, /*0 - 3*/},
            {3, 3, 3, 3, /*0 - 3*/},
            {3, 3, 3, 3, /*0 - 3*/},
            {3, 3, 3, 3, /*0 - 3*/},
            {3, 3, 3, 3, /*0 - 3*/},
            {3, 3, 3, 3, /*0 - 3*/},
            {3, 3, 3, 3, /*0 - 3*/},
            {3, 3, 3, 3, /*0 - 3*/},
        },
        /* nACnrEdgeThre[16][4] */
        {
            {1024, 1024, 1024, 1024, /*0 - 3*/},
            {1024, 1024, 1024, 1024, /*0 - 3*/},
            {1024, 1024, 1024, 1024, /*0 - 3*/},
            {1024, 1024, 1024, 1024, /*0 - 3*/},
            {1024, 1024, 1024, 1024, /*0 - 3*/},
            {1024, 1024, 1024, 1024, /*0 - 3*/},
            {1024, 1024, 1024, 1024, /*0 - 3*/},
            {1024, 1024, 1024, 1024, /*0 - 3*/},
            {1024, 1024, 1024, 1024, /*0 - 3*/},
            {1024, 1024, 1024, 1024, /*0 - 3*/},
            {1024, 1024, 1024, 1024, /*0 - 3*/},
            {1024, 1024, 1024, 1024, /*0 - 3*/},
        },
        /* nACnrEdgeSlope[16][4] */
        {
            {0, 0, 0, 0, /*0 - 3*/},
            {0, 0, 0, 0, /*0 - 3*/},
            {0, 0, 0, 0, /*0 - 3*/},
            {0, 0, 0, 0, /*0 - 3*/},
            {0, 0, 0, 0, /*0 - 3*/},
            {0, 0, 0, 0, /*0 - 3*/},
            {0, 0, 0, 0, /*0 - 3*/},
            {0, 0, 0, 0, /*0 - 3*/},
            {0, 0, 0, 0, /*0 - 3*/},
            {0, 0, 0, 0, /*0 - 3*/},
            {0, 0, 0, 0, /*0 - 3*/},
            {0, 0, 0, 0, /*0 - 3*/},
        },
        /* nACnrNvLut[16][36] */
        {
            {
                128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,  /* 0 - 31*/
                64, 64, 64, 64, /*32 - 35*/
            },
            {
                256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 128, 128, 128, 128, 128, 128, 128, 128, 128, 64, 64, 64, 64, 64,  /* 0 - 31*/
                64, 64, 64, 64, /*32 - 35*/
            },
            {
                1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 512, 512, 512, 512, 512, 512, 512, 512, 512, 64, 64, 64, 64, 64,  /* 0 - 31*/
                64, 64, 64, 64, /*32 - 35*/
            },
            {
                2048, 2048, 2048, 2048, 2048, 2048, 2048, 2048, 2048, 2048, 2048, 2048, 2048, 2048, 2048, 2048, 2048, 2048, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 64, 64, 64, 64, 64,  /* 0 - 31*/
                64, 64, 64, 64, /*32 - 35*/
            },
            {
                4096, 4096, 4096, 4096, 4096, 4096, 4096, 4096, 4096, 4096, 4096, 4096, 4096, 4096, 4096, 4096, 4096, 4096, 2048, 2048, 2048, 2048, 2048, 2048, 2048, 2048, 2048, 64, 64, 64, 64, 64,  /* 0 - 31*/
                64, 64, 64, 64, /*32 - 35*/
            },
            {
                8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192, 4096, 4096, 4096, 4096, 4096, 4096, 4096, 4096, 4096, 64, 64, 64, 64, 64,  /* 0 - 31*/
                64, 64, 64, 64, /*32 - 35*/
            },
            {
                16383, 16383, 16383, 16383, 16383, 16383, 16383, 16383, 16383, 16383, 16383, 16383, 16383, 16383, 16383, 16383, 16383, 16383, 8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192, 16383, 16383, 16383, 16383, 16383,  /* 0 - 31*/
                16383, 16383, 16383, 16383, /*32 - 35*/
            },
            {
                16383, 16383, 16383, 16383, 16383, 16383, 16383, 16383, 16383, 16383, 16383, 16383, 16383, 16383, 16383, 16383, 16383, 16383, 16383, 16383, 16383, 16383, 16383, 16383, 16383, 16383, 16383, 16383, 16383, 16383, 16383, 16383,  /* 0 - 31*/
                16383, 16383, 16383, 16383, /*32 - 35*/
            },
            {
                16383, 16383, 16383, 16383, 16383, 16383, 16383, 16383, 16383, 16383, 16383, 16383, 16383, 16383, 16383, 16383, 16383, 16383, 16383, 16383, 16383, 16383, 16383, 16383, 16383, 16383, 16383, 16383, 16383, 16383, 16383, 16383,  /* 0 - 31*/
                16383, 16383, 16383, 16383, /*32 - 35*/
            },
            {
                16383, 16383, 16383, 16383, 16383, 16383, 16383, 16383, 16383, 16383, 16383, 16383, 16383, 16383, 16383, 16383, 16383, 16383, 16383, 16383, 16383, 16383, 16383, 16383, 16383, 16383, 16383, 16383, 16383, 16383, 16383, 16383,  /* 0 - 31*/
                16383, 16383, 16383, 16383, /*32 - 35*/
            },
            {
                16383, 16383, 16383, 16383, 16383, 16383, 16383, 16383, 16383, 16383, 16383, 16383, 16383, 16383, 16383, 16383, 16383, 16383, 16383, 16383, 16383, 16383, 16383, 16383, 16383, 16383, 16383, 16383, 16383, 16383, 16383, 16383,  /* 0 - 31*/
                16383, 16383, 16383, 16383, /*32 - 35*/
            },
            {
                16383, 16383, 16383, 16383, 16383, 16383, 16383, 16383, 16383, 16383, 16383, 16383, 16383, 16383, 16383, 16383, 16383, 16383, 16383, 16383, 16383, 16383, 16383, 16383, 16383, 16383, 16383, 16383, 16383, 16383, 16383, 16383,  /* 0 - 31*/
                16383, 16383, 16383, 16383, /*32 - 35*/
            },
        },
        /* nACnrStrength[16][4] */
        {
            {256, 256, 164, 0, /*0 - 3*/},
            {256, 256, 164, 0, /*0 - 3*/},
            {256, 256, 164, 0, /*0 - 3*/},
            {256, 256, 128, 0, /*0 - 3*/},
            {256, 256, 128, 0, /*0 - 3*/},
            {256, 256, 64, 0, /*0 - 3*/},
            {256, 256, 64, 0, /*0 - 3*/},
            {256, 256, 64, 0, /*0 - 3*/},
            {256, 256, 64, 0, /*0 - 3*/},
            {256, 256, 64, 0, /*0 - 3*/},
            {256, 256, 64, 0, /*0 - 3*/},
            {256, 256, 64, 0, /*0 - 3*/},
        },
    },
};

static AX_ISP_IQ_SHARPEN_PARAM_T sharpen_param_sdr = {
    /* nShpEn */
    1,
    /* nShpMotionEn */
    1,
    /* nColorTargetEn[3] */
    {1, 0, 0, /*0 - 2*/},
    /* nAutoMode */
    1,
    /* nRefMode */
    1,
    /* nIoFlag[3] */
    {0, 0, 0, /*0 - 2*/},
    /* tManualParam */
    {
        /* nShpMotionStren */
        -191,
        /* nShpStillStren */
        0,
        /* nShpCenter[3][3] */
        {
            {1664, 3072, 2048, /*0 - 2*/},
            {-527, -415, -223, /*0 - 2*/},
            {-367, 352, 240, /*0 - 2*/},
        },
        /* nShpRadius[3][3] */
        {
            {256, 256, 512, /*0 - 2*/},
            {128, 128, 96, /*0 - 2*/},
            {128, 128, 160, /*0 - 2*/},
        },
        /* nShpSmooth[3][3] */
        {
            {6, 5, 6, /*0 - 2*/},
            {3, 4, 3, /*0 - 2*/},
            {4, 4, 4, /*0 - 2*/},
        },
        /* nShpLevelLut0[51] */
        {
            1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, -255, -255, -255, -255, -255, -255, -255, -255, -255, -255, -255, -255, -255, -255, -255,  /* 0 - 31*/
            -255, -255, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, /*32 - 50*/
        },
        /* nShpCoring[3] */
        {0, 0, 0, /*0 - 2*/},
        /* nShpEdSlope[3] */
        {16, 16, 16, /*0 - 2*/},
        /* nShpEdOffset[3] */
        {-63, -63, -63, /*0 - 2*/},
        /* nShpEdLlimit[3] */
        {128, 128, 128, /*0 - 2*/},
        /* nShpGain[3][2] */
        {
            {10, 10, /*0 - 1*/},
            {24, 24, /*0 - 1*/},
            {24, 24, /*0 - 1*/},
        },
        /* nShpLimit[3][2] */
        {
            {-511, 512, /*0 - 1*/},
            {-511, 512, /*0 - 1*/},
            {-511, 512, /*0 - 1*/},
        },
        /* nShpOsLimit[3][2] */
        {
            {-31, 32, /*0 - 1*/},
            {-127, 127, /*0 - 1*/},
            {-127, 127, /*0 - 1*/},
        },
        /* nShpOsGain[3] */
        {4, 6, 4, /*0 - 2*/},
        /* nShpHpfBsigma[3] */
        {8, 16, 24, /*0 - 2*/},
        /* nShpHpfDsigma[3] */
        {24, 24, 32, /*0 - 2*/},
        /* nShpHpfScale[3] */
        {128, 128, 128, /*0 - 2*/},
    },
    /* tAutoParam */
    {
        /* nParamGrpNum */
        12,
        /* nRefVal[16] */
        {1024, 2048, 16384, 32768, 49152, 65536, 65537, 131072, 262144, 393216, 524888, 1048576, /*0 - 11*/},
        /* nShpMotionStren[16] */
        {-96, -96, -128, -128, -164, -164, -164, -164, -128, -128, 0, 0, /*0 - 11*/},
        /* nShpStillStren[16] */
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 11*/},
        /* tShpCenter[16][3][3] */
        {
            /* tShpCenter 0 */
            {
                {1648, 2048, 2048, /*0 - 2*/},
                {-496, -224, -224, /*0 - 2*/},
                {448, 240, 240, /*0 - 2*/},
            },
            /* tShpCenter 1 */
            {
                {1648, 2048, 2048, /*0 - 2*/},
                {-496, -224, -224, /*0 - 2*/},
                {448, 240, 240, /*0 - 2*/},
            },
            /* tShpCenter 2 */
            {
                {2224, 2048, 2048, /*0 - 2*/},
                {-544, -224, -224, /*0 - 2*/},
                {544, 240, 240, /*0 - 2*/},
            },
            /* tShpCenter 3 */
            {
                {2224, 2048, 2048, /*0 - 2*/},
                {-544, -224, -224, /*0 - 2*/},
                {544, 240, 240, /*0 - 2*/},
            },
            /* tShpCenter 4 */
            {
                {2224, 2048, 2048, /*0 - 2*/},
                {-544, -224, -224, /*0 - 2*/},
                {544, 240, 240, /*0 - 2*/},
            },
            /* tShpCenter 5 */
            {
                {2224, 2048, 2048, /*0 - 2*/},
                {-544, -224, -224, /*0 - 2*/},
                {544, 240, 240, /*0 - 2*/},
            },
            /* tShpCenter 6 */
            {
                {2224, 2048, 2048, /*0 - 2*/},
                {-544, -224, -224, /*0 - 2*/},
                {544, 240, 240, /*0 - 2*/},
            },
            /* tShpCenter 7 */
            {
                {2224, 2048, 2048, /*0 - 2*/},
                {-544, -224, -224, /*0 - 2*/},
                {544, 240, 240, /*0 - 2*/},
            },
            /* tShpCenter 8 */
            {
                {1664, 3072, 2048, /*0 - 2*/},
                {-528, -416, -224, /*0 - 2*/},
                {-368, 352, 240, /*0 - 2*/},
            },
            /* tShpCenter 9 */
            {
                {1664, 3072, 2048, /*0 - 2*/},
                {-528, -416, -224, /*0 - 2*/},
                {-368, 352, 240, /*0 - 2*/},
            },
            /* tShpCenter 10 */
            {
                {1664, 3072, 2048, /*0 - 2*/},
                {-528, -416, -224, /*0 - 2*/},
                {-368, 352, 240, /*0 - 2*/},
            },
            /* tShpCenter 11 */
            {
                {1664, 3072, 2048, /*0 - 2*/},
                {-528, -416, -224, /*0 - 2*/},
                {-368, 352, 240, /*0 - 2*/},
            },
        },
        /* tShpRadius[16][3][3] */
        {
            /* tShpRadius 0 */
            {
                {256, 512, 512, /*0 - 2*/},
                {272, 96, 96, /*0 - 2*/},
                {208, 160, 160, /*0 - 2*/},
            },
            /* tShpRadius 1 */
            {
                {256, 512, 512, /*0 - 2*/},
                {272, 96, 96, /*0 - 2*/},
                {208, 160, 160, /*0 - 2*/},
            },
            /* tShpRadius 2 */
            {
                {256, 512, 512, /*0 - 2*/},
                {128, 96, 96, /*0 - 2*/},
                {128, 160, 160, /*0 - 2*/},
            },
            /* tShpRadius 3 */
            {
                {512, 512, 512, /*0 - 2*/},
                {128, 96, 96, /*0 - 2*/},
                {128, 160, 160, /*0 - 2*/},
            },
            /* tShpRadius 4 */
            {
                {256, 512, 512, /*0 - 2*/},
                {128, 96, 96, /*0 - 2*/},
                {128, 160, 160, /*0 - 2*/},
            },
            /* tShpRadius 5 */
            {
                {256, 512, 512, /*0 - 2*/},
                {128, 96, 96, /*0 - 2*/},
                {128, 160, 160, /*0 - 2*/},
            },
            /* tShpRadius 6 */
            {
                {256, 512, 512, /*0 - 2*/},
                {128, 96, 96, /*0 - 2*/},
                {128, 160, 160, /*0 - 2*/},
            },
            /* tShpRadius 7 */
            {
                {256, 512, 512, /*0 - 2*/},
                {128, 96, 96, /*0 - 2*/},
                {128, 160, 160, /*0 - 2*/},
            },
            /* tShpRadius 8 */
            {
                {256, 256, 512, /*0 - 2*/},
                {128, 128, 96, /*0 - 2*/},
                {128, 128, 160, /*0 - 2*/},
            },
            /* tShpRadius 9 */
            {
                {256, 256, 512, /*0 - 2*/},
                {128, 128, 96, /*0 - 2*/},
                {128, 128, 160, /*0 - 2*/},
            },
            /* tShpRadius 10 */
            {
                {256, 256, 512, /*0 - 2*/},
                {128, 128, 96, /*0 - 2*/},
                {128, 128, 160, /*0 - 2*/},
            },
            /* tShpRadius 11 */
            {
                {256, 256, 512, /*0 - 2*/},
                {128, 128, 96, /*0 - 2*/},
                {128, 128, 160, /*0 - 2*/},
            },
        },
        /* tShpSmooth[16][3][3] */
        {
            /* tShpSmooth 0 */
            {
                {5, 6, 6, /*0 - 2*/},
                {4, 3, 3, /*0 - 2*/},
                {4, 4, 4, /*0 - 2*/},
            },
            /* tShpSmooth 1 */
            {
                {5, 6, 6, /*0 - 2*/},
                {4, 3, 3, /*0 - 2*/},
                {4, 4, 4, /*0 - 2*/},
            },
            /* tShpSmooth 2 */
            {
                {7, 6, 6, /*0 - 2*/},
                {4, 3, 3, /*0 - 2*/},
                {4, 4, 4, /*0 - 2*/},
            },
            /* tShpSmooth 3 */
            {
                {7, 6, 6, /*0 - 2*/},
                {4, 3, 3, /*0 - 2*/},
                {4, 4, 4, /*0 - 2*/},
            },
            /* tShpSmooth 4 */
            {
                {7, 6, 6, /*0 - 2*/},
                {4, 3, 3, /*0 - 2*/},
                {4, 4, 4, /*0 - 2*/},
            },
            /* tShpSmooth 5 */
            {
                {7, 6, 6, /*0 - 2*/},
                {4, 3, 3, /*0 - 2*/},
                {4, 4, 4, /*0 - 2*/},
            },
            /* tShpSmooth 6 */
            {
                {7, 6, 6, /*0 - 2*/},
                {4, 3, 3, /*0 - 2*/},
                {4, 4, 4, /*0 - 2*/},
            },
            /* tShpSmooth 7 */
            {
                {7, 6, 6, /*0 - 2*/},
                {4, 3, 3, /*0 - 2*/},
                {4, 4, 4, /*0 - 2*/},
            },
            /* tShpSmooth 8 */
            {
                {6, 5, 6, /*0 - 2*/},
                {3, 4, 3, /*0 - 2*/},
                {4, 4, 4, /*0 - 2*/},
            },
            /* tShpSmooth 9 */
            {
                {6, 5, 6, /*0 - 2*/},
                {3, 4, 3, /*0 - 2*/},
                {4, 4, 4, /*0 - 2*/},
            },
            /* tShpSmooth 10 */
            {
                {6, 5, 6, /*0 - 2*/},
                {3, 4, 3, /*0 - 2*/},
                {4, 4, 4, /*0 - 2*/},
            },
            /* tShpSmooth 11 */
            {
                {6, 5, 6, /*0 - 2*/},
                {3, 4, 3, /*0 - 2*/},
                {4, 4, 4, /*0 - 2*/},
            },
        },
        /* nShpLevelLut0[16][51] */
        {
            {
                -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,  /* 0 - 31*/
                256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, /*32 - 50*/
            },
            {
                -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,  /* 0 - 31*/
                256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, /*32 - 50*/
            },
            {
                -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,  /* 0 - 31*/
                256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, /*32 - 50*/
            },
            {
                -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,  /* 0 - 31*/
                256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, /*32 - 50*/
            },
            {
                -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,  /* 0 - 31*/
                256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, /*32 - 50*/
            },
            {
                -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,  /* 0 - 31*/
                256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, /*32 - 50*/
            },
            {
                -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,  /* 0 - 31*/
                256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, /*32 - 50*/
            },
            {
                -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,  /* 0 - 31*/
                256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, /*32 - 50*/
            },
            {
                1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, -256, -256, -256, -256, -256, -256, -256, -256, -256, -256, -256, -256, -256, -256, -256,  /* 0 - 31*/
                -256, -256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, /*32 - 50*/
            },
            {
                1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, -256, -256, -256, -256, -256, -256, -256, -256, -256, -256, -256, -256, -256, -256, -256,  /* 0 - 31*/
                -256, -256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, /*32 - 50*/
            },
            {
                1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, -256, -256, -256, -256, -256, -256, -256, -256, -256, -256, -256, -256, -256, -256, -256,  /* 0 - 31*/
                -256, -256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, /*32 - 50*/
            },
            {
                1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, -256, -256, -256, -256, -256, -256, -256, -256, -256, -256, -256, -256, -256, -256, -256,  /* 0 - 31*/
                -256, -256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, /*32 - 50*/
            },
        },
        /* nShpCoring[16][3] */
        {
            {8, 8, 8, /*0 - 2*/},
            {8, 8, 8, /*0 - 2*/},
            {8, 8, 8, /*0 - 2*/},
            {8, 8, 8, /*0 - 2*/},
            {8, 8, 8, /*0 - 2*/},
            {8, 8, 8, /*0 - 2*/},
            {8, 8, 8, /*0 - 2*/},
            {12, 12, 12, /*0 - 2*/},
            {16, 16, 16, /*0 - 2*/},
            {20, 20, 20, /*0 - 2*/},
            {24, 24, 24, /*0 - 2*/},
            {28, 28, 28, /*0 - 2*/},
        },
        /* nShpEdSlope[16][3] */
        {
            {16, 16, 16, /*0 - 2*/},
            {16, 16, 16, /*0 - 2*/},
            {16, 16, 16, /*0 - 2*/},
            {16, 16, 16, /*0 - 2*/},
            {16, 16, 16, /*0 - 2*/},
            {16, 16, 16, /*0 - 2*/},
            {16, 16, 16, /*0 - 2*/},
            {16, 16, 16, /*0 - 2*/},
            {16, 16, 16, /*0 - 2*/},
            {16, 16, 16, /*0 - 2*/},
            {16, 16, 16, /*0 - 2*/},
            {16, 16, 16, /*0 - 2*/},
        },
        /* nShpEdOffset[16][3] */
        {
            {-64, 32, 0, /*0 - 2*/},
            {-64, 32, 0, /*0 - 2*/},
            {-64, 32, 0, /*0 - 2*/},
            {-64, 32, 0, /*0 - 2*/},
            {-64, 32, 0, /*0 - 2*/},
            {-64, -64, 0, /*0 - 2*/},
            {-64, -64, 0, /*0 - 2*/},
            {-64, -64, 0, /*0 - 2*/},
            {-64, -64, 0, /*0 - 2*/},
            {-64, -64, 0, /*0 - 2*/},
            {-64, -64, 0, /*0 - 2*/},
            {-64, -64, 0, /*0 - 2*/},
        },
        /* nShpEdLlimit[16][3] */
        {
            {255, 255, 255, /*0 - 2*/},
            {255, 255, 255, /*0 - 2*/},
            {255, 255, 255, /*0 - 2*/},
            {255, 255, 255, /*0 - 2*/},
            {255, 255, 255, /*0 - 2*/},
            {255, 255, 255, /*0 - 2*/},
            {255, 255, 255, /*0 - 2*/},
            {255, 255, 255, /*0 - 2*/},
            {128, 128, 128, /*0 - 2*/},
            {128, 128, 128, /*0 - 2*/},
            {128, 128, 128, /*0 - 2*/},
            {128, 128, 128, /*0 - 2*/},
        },
        /* nShpGain[16][3][2] */
        {
            /* nShpGain 0 */
            {
                {10, 10, /*0 - 1*/},
                {48, 48, /*0 - 1*/},
                {64, 64, /*0 - 1*/},
            },
            /* nShpGain 1 */
            {
                {10, 10, /*0 - 1*/},
                {48, 48, /*0 - 1*/},
                {64, 64, /*0 - 1*/},
            },
            /* nShpGain 2 */
            {
                {10, 10, /*0 - 1*/},
                {32, 32, /*0 - 1*/},
                {48, 48, /*0 - 1*/},
            },
            /* nShpGain 3 */
            {
                {10, 10, /*0 - 1*/},
                {28, 28, /*0 - 1*/},
                {32, 32, /*0 - 1*/},
            },
            /* nShpGain 4 */
            {
                {8, 8, /*0 - 1*/},
                {24, 24, /*0 - 1*/},
                {24, 24, /*0 - 1*/},
            },
            /* nShpGain 5 */
            {
                {8, 8, /*0 - 1*/},
                {24, 24, /*0 - 1*/},
                {24, 24, /*0 - 1*/},
            },
            /* nShpGain 6 */
            {
                {8, 8, /*0 - 1*/},
                {24, 24, /*0 - 1*/},
                {24, 24, /*0 - 1*/},
            },
            /* nShpGain 7 */
            {
                {8, 8, /*0 - 1*/},
                {20, 20, /*0 - 1*/},
                {20, 20, /*0 - 1*/},
            },
            /* nShpGain 8 */
            {
                {8, 8, /*0 - 1*/},
                {12, 12, /*0 - 1*/},
                {12, 12, /*0 - 1*/},
            },
            /* nShpGain 9 */
            {
                {4, 4, /*0 - 1*/},
                {6, 6, /*0 - 1*/},
                {6, 6, /*0 - 1*/},
            },
            /* nShpGain 10 */
            {
                {3, 3, /*0 - 1*/},
                {6, 6, /*0 - 1*/},
                {6, 6, /*0 - 1*/},
            },
            /* nShpGain 11 */
            {
                {2, 2, /*0 - 1*/},
                {3, 3, /*0 - 1*/},
                {3, 3, /*0 - 1*/},
            },
        },
        /* nShpLimit[16][3][2] */
        {
            /* nShpLimit 0 */
            {
                {-512, 512, /*0 - 1*/},
                {-512, 512, /*0 - 1*/},
                {-512, 512, /*0 - 1*/},
            },
            /* nShpLimit 1 */
            {
                {-512, 512, /*0 - 1*/},
                {-512, 512, /*0 - 1*/},
                {-512, 512, /*0 - 1*/},
            },
            /* nShpLimit 2 */
            {
                {-512, 512, /*0 - 1*/},
                {-512, 512, /*0 - 1*/},
                {-512, 512, /*0 - 1*/},
            },
            /* nShpLimit 3 */
            {
                {-512, 512, /*0 - 1*/},
                {-512, 512, /*0 - 1*/},
                {-512, 512, /*0 - 1*/},
            },
            /* nShpLimit 4 */
            {
                {-512, 512, /*0 - 1*/},
                {-512, 512, /*0 - 1*/},
                {-512, 512, /*0 - 1*/},
            },
            /* nShpLimit 5 */
            {
                {-512, 512, /*0 - 1*/},
                {-512, 512, /*0 - 1*/},
                {-512, 512, /*0 - 1*/},
            },
            /* nShpLimit 6 */
            {
                {-512, 512, /*0 - 1*/},
                {-512, 512, /*0 - 1*/},
                {-512, 512, /*0 - 1*/},
            },
            /* nShpLimit 7 */
            {
                {-512, 512, /*0 - 1*/},
                {-512, 512, /*0 - 1*/},
                {-512, 512, /*0 - 1*/},
            },
            /* nShpLimit 8 */
            {
                {-512, 512, /*0 - 1*/},
                {-512, 512, /*0 - 1*/},
                {-512, 512, /*0 - 1*/},
            },
            /* nShpLimit 9 */
            {
                {-512, 512, /*0 - 1*/},
                {-512, 512, /*0 - 1*/},
                {-512, 512, /*0 - 1*/},
            },
            /* nShpLimit 10 */
            {
                {-512, 512, /*0 - 1*/},
                {-512, 512, /*0 - 1*/},
                {-512, 512, /*0 - 1*/},
            },
            /* nShpLimit 11 */
            {
                {-512, 512, /*0 - 1*/},
                {-512, 512, /*0 - 1*/},
                {-512, 512, /*0 - 1*/},
            },
        },
        /* nShpOsLimit[16][3][2] */
        {
            /* nShpOsLimit 0 */
            {
                {-128, 127, /*0 - 1*/},
                {-128, 127, /*0 - 1*/},
                {-128, 127, /*0 - 1*/},
            },
            /* nShpOsLimit 1 */
            {
                {-128, 127, /*0 - 1*/},
                {-128, 127, /*0 - 1*/},
                {-128, 127, /*0 - 1*/},
            },
            /* nShpOsLimit 2 */
            {
                {-128, 127, /*0 - 1*/},
                {-128, 127, /*0 - 1*/},
                {-128, 127, /*0 - 1*/},
            },
            /* nShpOsLimit 3 */
            {
                {-32, 32, /*0 - 1*/},
                {-128, 127, /*0 - 1*/},
                {-128, 127, /*0 - 1*/},
            },
            /* nShpOsLimit 4 */
            {
                {-32, 32, /*0 - 1*/},
                {-128, 127, /*0 - 1*/},
                {-128, 127, /*0 - 1*/},
            },
            /* nShpOsLimit 5 */
            {
                {-32, 32, /*0 - 1*/},
                {-128, 127, /*0 - 1*/},
                {-128, 127, /*0 - 1*/},
            },
            /* nShpOsLimit 6 */
            {
                {-32, 32, /*0 - 1*/},
                {-128, 127, /*0 - 1*/},
                {-128, 127, /*0 - 1*/},
            },
            /* nShpOsLimit 7 */
            {
                {-32, 32, /*0 - 1*/},
                {-128, 127, /*0 - 1*/},
                {-128, 127, /*0 - 1*/},
            },
            /* nShpOsLimit 8 */
            {
                {-32, 32, /*0 - 1*/},
                {-128, 127, /*0 - 1*/},
                {-128, 127, /*0 - 1*/},
            },
            /* nShpOsLimit 9 */
            {
                {-32, 32, /*0 - 1*/},
                {-128, 127, /*0 - 1*/},
                {-128, 127, /*0 - 1*/},
            },
            /* nShpOsLimit 10 */
            {
                {-32, 32, /*0 - 1*/},
                {-128, 127, /*0 - 1*/},
                {-128, 127, /*0 - 1*/},
            },
            /* nShpOsLimit 11 */
            {
                {-32, 32, /*0 - 1*/},
                {-128, 127, /*0 - 1*/},
                {-128, 127, /*0 - 1*/},
            },
        },
        /* nShpOsGain[16][3] */
        {
            {8, 6, 6, /*0 - 2*/},
            {8, 6, 6, /*0 - 2*/},
            {8, 6, 6, /*0 - 2*/},
            {6, 6, 6, /*0 - 2*/},
            {6, 6, 6, /*0 - 2*/},
            {6, 6, 6, /*0 - 2*/},
            {6, 6, 6, /*0 - 2*/},
            {6, 6, 6, /*0 - 2*/},
            {4, 6, 4, /*0 - 2*/},
            {4, 6, 4, /*0 - 2*/},
            {4, 6, 4, /*0 - 2*/},
            {4, 6, 4, /*0 - 2*/},
        },
        /* nShpHpfBsigma[16][3] */
        {
            {0, 16, 16, /*0 - 2*/},
            {0, 16, 16, /*0 - 2*/},
            {0, 16, 24, /*0 - 2*/},
            {0, 16, 24, /*0 - 2*/},
            {0, 16, 24, /*0 - 2*/},
            {0, 16, 24, /*0 - 2*/},
            {0, 16, 24, /*0 - 2*/},
            {0, 16, 24, /*0 - 2*/},
            {8, 16, 24, /*0 - 2*/},
            {8, 16, 24, /*0 - 2*/},
            {8, 16, 24, /*0 - 2*/},
            {8, 16, 24, /*0 - 2*/},
        },
        /* nShpHpfDsigma[16][3] */
        {
            {24, 24, 32, /*0 - 2*/},
            {24, 24, 32, /*0 - 2*/},
            {24, 24, 32, /*0 - 2*/},
            {24, 24, 32, /*0 - 2*/},
            {24, 24, 32, /*0 - 2*/},
            {24, 24, 32, /*0 - 2*/},
            {24, 24, 32, /*0 - 2*/},
            {24, 24, 32, /*0 - 2*/},
            {24, 24, 32, /*0 - 2*/},
            {24, 24, 32, /*0 - 2*/},
            {24, 24, 32, /*0 - 2*/},
            {24, 24, 32, /*0 - 2*/},
        },
        /* nShpHpfScale[16][3] */
        {
            {128, 128, 128, /*0 - 2*/},
            {128, 128, 128, /*0 - 2*/},
            {128, 128, 128, /*0 - 2*/},
            {128, 128, 128, /*0 - 2*/},
            {128, 128, 128, /*0 - 2*/},
            {128, 128, 128, /*0 - 2*/},
            {64, 64, 64, /*0 - 2*/},
            {64, 64, 64, /*0 - 2*/},
            {128, 128, 128, /*0 - 2*/},
            {128, 128, 128, /*0 - 2*/},
            {128, 128, 128, /*0 - 2*/},
            {128, 128, 128, /*0 - 2*/},
        },
    },
};

static AX_ISP_IQ_MDE_PARAM_T mde_param_sdr = {
    /* nColorTargetEn[8] */
    {0, 0, 0, 0, 0, 0, 0, 0, /*0 - 7*/},
    /* nAutoMode */
    1,
    /* nRefMode */
    1,
    /* nIoFlag[8] */
    {0, 0, 0, 0, 0, 0, 0, 0, /*0 - 7*/},
    /* tManualParam */
    {
        /* tMdeCenter[3][8] */
        {
            {1616, 1872, 352, 1408, 656, 1120, 1200, 2400, /*0 - 7*/},
            {-479, -1071, 1600, -799, 528, 256, 0, 0, /*0 - 7*/},
            {-415, -1407, -271, 128, 656, -799, 0, 0, /*0 - 7*/},
        },
        /* tMdeRadius[3][8] */
        {
            {480, 64, 64, 64, 64, 64, 64, 64, /*0 - 7*/},
            {96, 64, 64, 64, 64, 64, 64, 64, /*0 - 7*/},
            {96, 64, 64, 64, 64, 64, 64, 64, /*0 - 7*/},
        },
        /* tMdeSmooth[3][8] */
        {
            {3, 3, 3, 3, 3, 3, 3, 3, /*0 - 7*/},
            {2, 3, 3, 3, 3, 3, 3, 3, /*0 - 7*/},
            {1, 3, 3, 3, 3, 3, 3, 3, /*0 - 7*/},
        },
        /* nMdeStrength0[4][9] */
        {
            {128, 128, 128, 128, 128, 128, 128, 128, 128, /*0 - 8*/},
            {393, 1828, 1516, 1578, 1516, 1890, 1142, 1329, 1391, /*0 - 8*/},
            {0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 8*/},
            {0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 8*/},
        },
        /* nMdeStrength1[4][9] */
        {
            {0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 8*/},
            {0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 8*/},
            {0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 8*/},
            {0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 8*/},
        },
        /* nMdeStrength2[4][9] */
        {
            {0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 8*/},
            {0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 8*/},
            {0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 8*/},
            {0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 8*/},
        },
        /* nMdeStrength3[4][5] */
        {
            {0, 0, 0, 0, 0, /*0 - 4*/},
            {0, 0, 0, 0, 0, /*0 - 4*/},
            {0, 0, 0, 0, 0, /*0 - 4*/},
            {0, 0, 0, 0, 0, /*0 - 4*/},
        },
        /* nMdeStrength4[4][5] */
        {
            {0, 0, 0, 0, 0, /*0 - 4*/},
            {0, 0, 0, 0, 0, /*0 - 4*/},
            {0, 0, 0, 0, 0, /*0 - 4*/},
            {0, 0, 0, 0, 0, /*0 - 4*/},
        },
        /* nMdeStrength5[4][5] */
        {
            {0, 0, 0, 0, 0, /*0 - 4*/},
            {0, 0, 0, 0, 0, /*0 - 4*/},
            {0, 0, 0, 0, 0, /*0 - 4*/},
            {0, 0, 0, 0, 0, /*0 - 4*/},
        },
        /* nMdeStrength6[4] */
        {0, 0, 0, 0, /*0 - 3*/},
        /* nMdeStrength7[4] */
        {0, 0, 0, 0, /*0 - 3*/},
        /* nMdeLumaLut[4][129] */
        {
            {
                384, 384, 384, 384, 384, 384, 384, 384, 384, 384, 384, 384, 384, 384, 384, 384, 384, 384, 384, 384, 384, 384, 384, 384, 384, 384, 384, 384, 384, 384, 384, 384,  /* 0 - 31*/
                384, 384, 384, 384, 384, 384, 384, 384, 384, 384, 384, 384, 384, 384, 384, 384, 384, 384, 384, 384, 384, 384, 384, 384, 384, 384, 384, 384, 384, 384, 384, 384,  /* 32 - 63*/
                384, 384, 384, 384, 384, 384, 384, 384, 384, 384, 384, 384, 384, 384, 384, 384, 384, 384, 384, 384, 384, 384, 384, 384, 384, 384, 384, 384, 384, 384, 384, 384,  /* 64 - 95*/
                384, 384, 384, 384, 384, 384, 384, 384, 384, 384, 384, 384, 384, 384, 384, 384, 384, 384, 384, 384, 384, 384, 384, 384, 384, 384, 384, 384, 384, 384, 384, 384,  /* 96 - 127*/
                384, /*128 - 128*/
            },
            {
                256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,  /* 0 - 31*/
                256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,  /* 32 - 63*/
                256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,  /* 64 - 95*/
                256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,  /* 96 - 127*/
                256, /*128 - 128*/
            },
            {
                256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,  /* 0 - 31*/
                256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,  /* 32 - 63*/
                256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,  /* 64 - 95*/
                256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,  /* 96 - 127*/
                256, /*128 - 128*/
            },
            {
                256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,  /* 0 - 31*/
                256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,  /* 32 - 63*/
                256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,  /* 64 - 95*/
                256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,  /* 96 - 127*/
                256, /*128 - 128*/
            },
        },
        /* nMdeLevelLut[4][129] */
        {
            {
                0, 32, 64, 96, 128, 160, 192, 224, 256, 288, 320, 352, 384, 416, 448, 480, 512, 544, 576, 608, 640, 672, 704, 736, 768, 800, 832, 864, 896, 928, 960, 992,  /* 0 - 31*/
                1024, 1056, 1088, 1120, 1152, 1184, 1216, 1248, 1280, 1312, 1344, 1376, 1408, 1440, 1472, 1504, 1536, 1568, 1600, 1632, 1664, 1696, 1728, 1760, 1792, 1824, 1856, 1888, 1920, 1952, 1984, 2016,  /* 32 - 63*/
                2048, 2080, 2112, 2144, 2176, 2208, 2240, 2272, 2304, 2336, 2368, 2400, 2432, 2464, 2496, 2528, 2560, 2592, 2624, 2656, 2688, 2720, 2752, 2784, 2816, 2848, 2880, 2912, 2944, 2976, 3008, 3040,  /* 64 - 95*/
                3072, 3104, 3136, 3168, 3200, 3232, 3264, 3296, 3328, 3360, 3392, 3424, 3456, 3488, 3520, 3552, 3584, 3616, 3648, 3680, 3712, 3744, 3776, 3808, 3840, 3872, 3904, 3936, 3968, 4000, 4032, 4064,  /* 96 - 127*/
                4095, /*128 - 128*/
            },
            {
                0, 32, 64, 96, 128, 160, 192, 224, 256, 288, 320, 352, 384, 416, 448, 480, 512, 544, 576, 608, 640, 672, 704, 736, 768, 800, 832, 864, 896, 928, 960, 992,  /* 0 - 31*/
                1024, 1056, 1088, 1120, 1152, 1184, 1216, 1248, 1280, 1312, 1344, 1376, 1408, 1440, 1472, 1504, 1536, 1568, 1600, 1632, 1664, 1696, 1728, 1760, 1792, 1824, 1856, 1888, 1920, 1952, 1984, 2016,  /* 32 - 63*/
                2048, 2080, 2112, 2144, 2176, 2208, 2240, 2272, 2304, 2336, 2368, 2400, 2432, 2464, 2496, 2528, 2560, 2592, 2624, 2656, 2688, 2720, 2752, 2784, 2816, 2848, 2880, 2912, 2944, 2976, 3008, 3040,  /* 64 - 95*/
                3072, 3104, 3136, 3168, 3200, 3232, 3264, 3296, 3328, 3360, 3392, 3424, 3456, 3488, 3520, 3552, 3584, 3616, 3648, 3680, 3712, 3744, 3776, 3808, 3840, 3872, 3904, 3936, 3968, 4000, 4032, 4064,  /* 96 - 127*/
                4095, /*128 - 128*/
            },
            {
                0, 32, 64, 96, 128, 160, 192, 224, 256, 288, 320, 352, 384, 416, 448, 480, 512, 544, 576, 608, 640, 672, 704, 736, 768, 800, 832, 864, 896, 928, 960, 992,  /* 0 - 31*/
                1024, 1056, 1088, 1120, 1152, 1184, 1216, 1248, 1280, 1312, 1344, 1376, 1408, 1440, 1472, 1504, 1536, 1568, 1600, 1632, 1664, 1696, 1728, 1760, 1792, 1824, 1856, 1888, 1920, 1952, 1984, 2016,  /* 32 - 63*/
                2048, 2080, 2112, 2144, 2176, 2208, 2240, 2272, 2304, 2336, 2368, 2400, 2432, 2464, 2496, 2528, 2560, 2592, 2624, 2656, 2688, 2720, 2752, 2784, 2816, 2848, 2880, 2912, 2944, 2976, 3008, 3040,  /* 64 - 95*/
                3072, 3104, 3136, 3168, 3200, 3232, 3264, 3296, 3328, 3360, 3392, 3424, 3456, 3488, 3520, 3552, 3584, 3616, 3648, 3680, 3712, 3744, 3776, 3808, 3840, 3872, 3904, 3936, 3968, 4000, 4032, 4064,  /* 96 - 127*/
                4095, /*128 - 128*/
            },
            {
                0, 32, 64, 96, 128, 160, 192, 224, 256, 288, 320, 352, 384, 416, 448, 480, 512, 544, 576, 608, 640, 672, 704, 736, 768, 800, 832, 864, 896, 928, 960, 992,  /* 0 - 31*/
                1024, 1056, 1088, 1120, 1152, 1184, 1216, 1248, 1280, 1312, 1344, 1376, 1408, 1440, 1472, 1504, 1536, 1568, 1600, 1632, 1664, 1696, 1728, 1760, 1792, 1824, 1856, 1888, 1920, 1952, 1984, 2016,  /* 32 - 63*/
                2048, 2080, 2112, 2144, 2176, 2208, 2240, 2272, 2304, 2336, 2368, 2400, 2432, 2464, 2496, 2528, 2560, 2592, 2624, 2656, 2688, 2720, 2752, 2784, 2816, 2848, 2880, 2912, 2944, 2976, 3008, 3040,  /* 64 - 95*/
                3072, 3104, 3136, 3168, 3200, 3232, 3264, 3296, 3328, 3360, 3392, 3424, 3456, 3488, 3520, 3552, 3584, 3616, 3648, 3680, 3712, 3744, 3776, 3808, 3840, 3872, 3904, 3936, 3968, 4000, 4032, 4064,  /* 96 - 127*/
                4095, /*128 - 128*/
            },
        },
        /* nMdeGain[4] */
        {256, 256, 256, 256, /*0 - 3*/},
    },
    /* tAutoParam */
    {
        /* nParamGrpNum */
        12,
        /* nRefVal[16] */
        {1024, 2048, 16384, 32768, 49152, 65536, 65537, 131072, 262144, 465920, 532480, 1048576, /*0 - 11*/},
        /* tMdeCenter[16][3][8] */
        {
            /* tMdeCenter 0 */
            {
                {1216, 1872, 352, 1408, 656, 1120, 1200, 2400, /*0 - 7*/},
                {-704, -1072, 1600, -800, 528, 256, 0, 0, /*0 - 7*/},
                {1152, -1408, -272, 128, 656, -800, 0, 0, /*0 - 7*/},
            },
            /* tMdeCenter 1 */
            {
                {1216, 1872, 352, 1408, 656, 1120, 1200, 2400, /*0 - 7*/},
                {-704, -1072, 1600, -800, 528, 256, 0, 0, /*0 - 7*/},
                {1152, -1408, -272, 128, 656, -800, 0, 0, /*0 - 7*/},
            },
            /* tMdeCenter 2 */
            {
                {1216, 1872, 352, 1408, 656, 1120, 1200, 2400, /*0 - 7*/},
                {-704, -1072, 1600, -800, 528, 256, 0, 0, /*0 - 7*/},
                {1152, -1408, -272, 128, 656, -800, 0, 0, /*0 - 7*/},
            },
            /* tMdeCenter 3 */
            {
                {1216, 1872, 352, 1408, 656, 1120, 1200, 2400, /*0 - 7*/},
                {-704, -1072, 1600, -800, 528, 256, 0, 0, /*0 - 7*/},
                {1152, -1408, -272, 128, 656, -800, 0, 0, /*0 - 7*/},
            },
            /* tMdeCenter 4 */
            {
                {1216, 1872, 352, 1408, 656, 1120, 1200, 2400, /*0 - 7*/},
                {-704, -1072, 1600, -800, 528, 256, 0, 0, /*0 - 7*/},
                {1152, -1408, -272, 128, 656, -800, 0, 0, /*0 - 7*/},
            },
            /* tMdeCenter 5 */
            {
                {1216, 1872, 352, 1408, 656, 1120, 1200, 2400, /*0 - 7*/},
                {-704, -1072, 1600, -800, 528, 256, 0, 0, /*0 - 7*/},
                {1152, -1408, -272, 128, 656, -800, 0, 0, /*0 - 7*/},
            },
            /* tMdeCenter 6 */
            {
                {1216, 1872, 352, 1408, 656, 1120, 1200, 2400, /*0 - 7*/},
                {-704, -1072, 1600, -800, 528, 256, 0, 0, /*0 - 7*/},
                {1152, -1408, -272, 128, 656, -800, 0, 0, /*0 - 7*/},
            },
            /* tMdeCenter 7 */
            {
                {1216, 1872, 352, 1408, 656, 1120, 1200, 2400, /*0 - 7*/},
                {-704, -1072, 1600, -800, 528, 256, 0, 0, /*0 - 7*/},
                {1152, -1408, -272, 128, 656, -800, 0, 0, /*0 - 7*/},
            },
            /* tMdeCenter 8 */
            {
                {1216, 1872, 352, 1408, 656, 1120, 1200, 2400, /*0 - 7*/},
                {-704, -1072, 1600, -800, 528, 256, 0, 0, /*0 - 7*/},
                {1152, -1408, -272, 128, 656, -800, 0, 0, /*0 - 7*/},
            },
            /* tMdeCenter 9 */
            {
                {1616, 1872, 352, 1408, 656, 1120, 1200, 2400, /*0 - 7*/},
                {-480, -1072, 1600, -800, 528, 256, 0, 0, /*0 - 7*/},
                {-416, -1408, -272, 128, 656, -800, 0, 0, /*0 - 7*/},
            },
            /* tMdeCenter 10 */
            {
                {1616, 1872, 352, 1408, 656, 1120, 1200, 2400, /*0 - 7*/},
                {-480, -1072, 1600, -800, 528, 256, 0, 0, /*0 - 7*/},
                {-416, -1408, -272, 128, 656, -800, 0, 0, /*0 - 7*/},
            },
            /* tMdeCenter 11 */
            {
                {1616, 1872, 352, 1408, 656, 1120, 1200, 2400, /*0 - 7*/},
                {-480, -1072, 1600, -800, 528, 256, 0, 0, /*0 - 7*/},
                {-416, -1408, -272, 128, 656, -800, 0, 0, /*0 - 7*/},
            },
        },
        /* tMdeRadius[16][3][8] */
        {
            /* tMdeRadius 0 */
            {
                {64, 64, 64, 64, 64, 64, 64, 64, /*0 - 7*/},
                {64, 64, 64, 64, 64, 64, 64, 64, /*0 - 7*/},
                {64, 64, 64, 64, 64, 64, 64, 64, /*0 - 7*/},
            },
            /* tMdeRadius 1 */
            {
                {64, 64, 64, 64, 64, 64, 64, 64, /*0 - 7*/},
                {64, 64, 64, 64, 64, 64, 64, 64, /*0 - 7*/},
                {64, 64, 64, 64, 64, 64, 64, 64, /*0 - 7*/},
            },
            /* tMdeRadius 2 */
            {
                {64, 64, 64, 64, 64, 64, 64, 64, /*0 - 7*/},
                {64, 64, 64, 64, 64, 64, 64, 64, /*0 - 7*/},
                {64, 64, 64, 64, 64, 64, 64, 64, /*0 - 7*/},
            },
            /* tMdeRadius 3 */
            {
                {64, 64, 64, 64, 64, 64, 64, 64, /*0 - 7*/},
                {64, 64, 64, 64, 64, 64, 64, 64, /*0 - 7*/},
                {64, 64, 64, 64, 64, 64, 64, 64, /*0 - 7*/},
            },
            /* tMdeRadius 4 */
            {
                {64, 64, 64, 64, 64, 64, 64, 64, /*0 - 7*/},
                {64, 64, 64, 64, 64, 64, 64, 64, /*0 - 7*/},
                {64, 64, 64, 64, 64, 64, 64, 64, /*0 - 7*/},
            },
            /* tMdeRadius 5 */
            {
                {64, 64, 64, 64, 64, 64, 64, 64, /*0 - 7*/},
                {64, 64, 64, 64, 64, 64, 64, 64, /*0 - 7*/},
                {64, 64, 64, 64, 64, 64, 64, 64, /*0 - 7*/},
            },
            /* tMdeRadius 6 */
            {
                {64, 64, 64, 64, 64, 64, 64, 64, /*0 - 7*/},
                {64, 64, 64, 64, 64, 64, 64, 64, /*0 - 7*/},
                {64, 64, 64, 64, 64, 64, 64, 64, /*0 - 7*/},
            },
            /* tMdeRadius 7 */
            {
                {64, 64, 64, 64, 64, 64, 64, 64, /*0 - 7*/},
                {64, 64, 64, 64, 64, 64, 64, 64, /*0 - 7*/},
                {64, 64, 64, 64, 64, 64, 64, 64, /*0 - 7*/},
            },
            /* tMdeRadius 8 */
            {
                {64, 64, 64, 64, 64, 64, 64, 64, /*0 - 7*/},
                {64, 64, 64, 64, 64, 64, 64, 64, /*0 - 7*/},
                {64, 64, 64, 64, 64, 64, 64, 64, /*0 - 7*/},
            },
            /* tMdeRadius 9 */
            {
                {480, 64, 64, 64, 64, 64, 64, 64, /*0 - 7*/},
                {96, 64, 64, 64, 64, 64, 64, 64, /*0 - 7*/},
                {96, 64, 64, 64, 64, 64, 64, 64, /*0 - 7*/},
            },
            /* tMdeRadius 10 */
            {
                {480, 64, 64, 64, 64, 64, 64, 64, /*0 - 7*/},
                {96, 64, 64, 64, 64, 64, 64, 64, /*0 - 7*/},
                {96, 64, 64, 64, 64, 64, 64, 64, /*0 - 7*/},
            },
            /* tMdeRadius 11 */
            {
                {480, 64, 64, 64, 64, 64, 64, 64, /*0 - 7*/},
                {96, 64, 64, 64, 64, 64, 64, 64, /*0 - 7*/},
                {96, 64, 64, 64, 64, 64, 64, 64, /*0 - 7*/},
            },
        },
        /* tMdeSmooth[16][3][8] */
        {
            /* tMdeSmooth 0 */
            {
                {3, 3, 3, 3, 3, 3, 3, 3, /*0 - 7*/},
                {3, 3, 3, 3, 3, 3, 3, 3, /*0 - 7*/},
                {3, 3, 3, 3, 3, 3, 3, 3, /*0 - 7*/},
            },
            /* tMdeSmooth 1 */
            {
                {3, 3, 3, 3, 3, 3, 3, 3, /*0 - 7*/},
                {3, 3, 3, 3, 3, 3, 3, 3, /*0 - 7*/},
                {3, 3, 3, 3, 3, 3, 3, 3, /*0 - 7*/},
            },
            /* tMdeSmooth 2 */
            {
                {3, 3, 3, 3, 3, 3, 3, 3, /*0 - 7*/},
                {3, 3, 3, 3, 3, 3, 3, 3, /*0 - 7*/},
                {3, 3, 3, 3, 3, 3, 3, 3, /*0 - 7*/},
            },
            /* tMdeSmooth 3 */
            {
                {3, 3, 3, 3, 3, 3, 3, 3, /*0 - 7*/},
                {3, 3, 3, 3, 3, 3, 3, 3, /*0 - 7*/},
                {3, 3, 3, 3, 3, 3, 3, 3, /*0 - 7*/},
            },
            /* tMdeSmooth 4 */
            {
                {3, 3, 3, 3, 3, 3, 3, 3, /*0 - 7*/},
                {3, 3, 3, 3, 3, 3, 3, 3, /*0 - 7*/},
                {3, 3, 3, 3, 3, 3, 3, 3, /*0 - 7*/},
            },
            /* tMdeSmooth 5 */
            {
                {3, 3, 3, 3, 3, 3, 3, 3, /*0 - 7*/},
                {3, 3, 3, 3, 3, 3, 3, 3, /*0 - 7*/},
                {3, 3, 3, 3, 3, 3, 3, 3, /*0 - 7*/},
            },
            /* tMdeSmooth 6 */
            {
                {3, 3, 3, 3, 3, 3, 3, 3, /*0 - 7*/},
                {3, 3, 3, 3, 3, 3, 3, 3, /*0 - 7*/},
                {3, 3, 3, 3, 3, 3, 3, 3, /*0 - 7*/},
            },
            /* tMdeSmooth 7 */
            {
                {3, 3, 3, 3, 3, 3, 3, 3, /*0 - 7*/},
                {3, 3, 3, 3, 3, 3, 3, 3, /*0 - 7*/},
                {3, 3, 3, 3, 3, 3, 3, 3, /*0 - 7*/},
            },
            /* tMdeSmooth 8 */
            {
                {3, 3, 3, 3, 3, 3, 3, 3, /*0 - 7*/},
                {3, 3, 3, 3, 3, 3, 3, 3, /*0 - 7*/},
                {3, 3, 3, 3, 3, 3, 3, 3, /*0 - 7*/},
            },
            /* tMdeSmooth 9 */
            {
                {3, 3, 3, 3, 3, 3, 3, 3, /*0 - 7*/},
                {2, 3, 3, 3, 3, 3, 3, 3, /*0 - 7*/},
                {1, 3, 3, 3, 3, 3, 3, 3, /*0 - 7*/},
            },
            /* tMdeSmooth 10 */
            {
                {3, 3, 3, 3, 3, 3, 3, 3, /*0 - 7*/},
                {2, 3, 3, 3, 3, 3, 3, 3, /*0 - 7*/},
                {1, 3, 3, 3, 3, 3, 3, 3, /*0 - 7*/},
            },
            /* tMdeSmooth 11 */
            {
                {3, 3, 3, 3, 3, 3, 3, 3, /*0 - 7*/},
                {2, 3, 3, 3, 3, 3, 3, 3, /*0 - 7*/},
                {1, 3, 3, 3, 3, 3, 3, 3, /*0 - 7*/},
            },
        },
        /* nMdeStrength0[16][4][9] */
        {
            /* nMdeStrength0 0 */
            {
                {0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 8*/},
                {0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 8*/},
                {0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 8*/},
                {0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 8*/},
            },
            /* nMdeStrength0 1 */
            {
                {0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 8*/},
                {0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 8*/},
                {0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 8*/},
                {0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 8*/},
            },
            /* nMdeStrength0 2 */
            {
                {0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 8*/},
                {0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 8*/},
                {0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 8*/},
                {0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 8*/},
            },
            /* nMdeStrength0 3 */
            {
                {0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 8*/},
                {0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 8*/},
                {0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 8*/},
                {0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 8*/},
            },
            /* nMdeStrength0 4 */
            {
                {0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 8*/},
                {0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 8*/},
                {0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 8*/},
                {0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 8*/},
            },
            /* nMdeStrength0 5 */
            {
                {0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 8*/},
                {0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 8*/},
                {0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 8*/},
                {0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 8*/},
            },
            /* nMdeStrength0 6 */
            {
                {0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 8*/},
                {0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 8*/},
                {0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 8*/},
                {0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 8*/},
            },
            /* nMdeStrength0 7 */
            {
                {0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 8*/},
                {0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 8*/},
                {0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 8*/},
                {0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 8*/},
            },
            /* nMdeStrength0 8 */
            {
                {128, 128, 128, 128, 128, 128, 128, 128, 128, /*0 - 8*/},
                {393, 1828, 1516, 1578, 1516, 1890, 1142, 1329, 1391, /*0 - 8*/},
                {0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 8*/},
                {0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 8*/},
            },
            /* nMdeStrength0 9 */
            {
                {128, 128, 128, 128, 128, 128, 128, 128, 128, /*0 - 8*/},
                {393, 1828, 1516, 1578, 1516, 1890, 1142, 1329, 1391, /*0 - 8*/},
                {0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 8*/},
                {0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 8*/},
            },
            /* nMdeStrength0 10 */
            {
                {128, 128, 128, 128, 128, 128, 128, 128, 128, /*0 - 8*/},
                {393, 1828, 1516, 1578, 1516, 1890, 1142, 1329, 1391, /*0 - 8*/},
                {0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 8*/},
                {0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 8*/},
            },
            /* nMdeStrength0 11 */
            {
                {128, 128, 128, 128, 128, 128, 128, 128, 128, /*0 - 8*/},
                {393, 1828, 1516, 1578, 1516, 1890, 1142, 1329, 1391, /*0 - 8*/},
                {0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 8*/},
                {0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 8*/},
            },
        },
        /* nMdeStrength1[16][4][9] */
        {
            /* nMdeStrength1 0 */
            {
                {0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 8*/},
                {0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 8*/},
                {0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 8*/},
                {0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 8*/},
            },
            /* nMdeStrength1 1 */
            {
                {0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 8*/},
                {0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 8*/},
                {0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 8*/},
                {0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 8*/},
            },
            /* nMdeStrength1 2 */
            {
                {0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 8*/},
                {0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 8*/},
                {0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 8*/},
                {0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 8*/},
            },
            /* nMdeStrength1 3 */
            {
                {0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 8*/},
                {0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 8*/},
                {0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 8*/},
                {0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 8*/},
            },
            /* nMdeStrength1 4 */
            {
                {0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 8*/},
                {0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 8*/},
                {0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 8*/},
                {0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 8*/},
            },
            /* nMdeStrength1 5 */
            {
                {0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 8*/},
                {0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 8*/},
                {0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 8*/},
                {0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 8*/},
            },
            /* nMdeStrength1 6 */
            {
                {0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 8*/},
                {0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 8*/},
                {0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 8*/},
                {0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 8*/},
            },
            /* nMdeStrength1 7 */
            {
                {0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 8*/},
                {0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 8*/},
                {0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 8*/},
                {0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 8*/},
            },
            /* nMdeStrength1 8 */
            {
                {0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 8*/},
                {0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 8*/},
                {0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 8*/},
                {0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 8*/},
            },
            /* nMdeStrength1 9 */
            {
                {0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 8*/},
                {0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 8*/},
                {0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 8*/},
                {0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 8*/},
            },
            /* nMdeStrength1 10 */
            {
                {0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 8*/},
                {0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 8*/},
                {0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 8*/},
                {0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 8*/},
            },
            /* nMdeStrength1 11 */
            {
                {0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 8*/},
                {0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 8*/},
                {0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 8*/},
                {0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 8*/},
            },
        },
        /* nMdeStrength2[16][4][9] */
        {
            /* nMdeStrength2 0 */
            {
                {0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 8*/},
                {0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 8*/},
                {0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 8*/},
                {0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 8*/},
            },
            /* nMdeStrength2 1 */
            {
                {0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 8*/},
                {0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 8*/},
                {0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 8*/},
                {0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 8*/},
            },
            /* nMdeStrength2 2 */
            {
                {0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 8*/},
                {0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 8*/},
                {0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 8*/},
                {0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 8*/},
            },
            /* nMdeStrength2 3 */
            {
                {0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 8*/},
                {0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 8*/},
                {0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 8*/},
                {0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 8*/},
            },
            /* nMdeStrength2 4 */
            {
                {0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 8*/},
                {0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 8*/},
                {0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 8*/},
                {0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 8*/},
            },
            /* nMdeStrength2 5 */
            {
                {0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 8*/},
                {0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 8*/},
                {0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 8*/},
                {0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 8*/},
            },
            /* nMdeStrength2 6 */
            {
                {0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 8*/},
                {0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 8*/},
                {0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 8*/},
                {0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 8*/},
            },
            /* nMdeStrength2 7 */
            {
                {0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 8*/},
                {0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 8*/},
                {0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 8*/},
                {0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 8*/},
            },
            /* nMdeStrength2 8 */
            {
                {0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 8*/},
                {0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 8*/},
                {0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 8*/},
                {0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 8*/},
            },
            /* nMdeStrength2 9 */
            {
                {0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 8*/},
                {0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 8*/},
                {0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 8*/},
                {0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 8*/},
            },
            /* nMdeStrength2 10 */
            {
                {0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 8*/},
                {0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 8*/},
                {0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 8*/},
                {0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 8*/},
            },
            /* nMdeStrength2 11 */
            {
                {0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 8*/},
                {0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 8*/},
                {0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 8*/},
                {0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 8*/},
            },
        },
        /* nMdeStrength3[16][4][5] */
        {
            /* nMdeStrength3 0 */
            {
                {0, 0, 0, 0, 0, /*0 - 4*/},
                {0, 0, 0, 0, 0, /*0 - 4*/},
                {0, 0, 0, 0, 0, /*0 - 4*/},
                {0, 0, 0, 0, 0, /*0 - 4*/},
            },
            /* nMdeStrength3 1 */
            {
                {0, 0, 0, 0, 0, /*0 - 4*/},
                {0, 0, 0, 0, 0, /*0 - 4*/},
                {0, 0, 0, 0, 0, /*0 - 4*/},
                {0, 0, 0, 0, 0, /*0 - 4*/},
            },
            /* nMdeStrength3 2 */
            {
                {0, 0, 0, 0, 0, /*0 - 4*/},
                {0, 0, 0, 0, 0, /*0 - 4*/},
                {0, 0, 0, 0, 0, /*0 - 4*/},
                {0, 0, 0, 0, 0, /*0 - 4*/},
            },
            /* nMdeStrength3 3 */
            {
                {0, 0, 0, 0, 0, /*0 - 4*/},
                {0, 0, 0, 0, 0, /*0 - 4*/},
                {0, 0, 0, 0, 0, /*0 - 4*/},
                {0, 0, 0, 0, 0, /*0 - 4*/},
            },
            /* nMdeStrength3 4 */
            {
                {0, 0, 0, 0, 0, /*0 - 4*/},
                {0, 0, 0, 0, 0, /*0 - 4*/},
                {0, 0, 0, 0, 0, /*0 - 4*/},
                {0, 0, 0, 0, 0, /*0 - 4*/},
            },
            /* nMdeStrength3 5 */
            {
                {0, 0, 0, 0, 0, /*0 - 4*/},
                {0, 0, 0, 0, 0, /*0 - 4*/},
                {0, 0, 0, 0, 0, /*0 - 4*/},
                {0, 0, 0, 0, 0, /*0 - 4*/},
            },
            /* nMdeStrength3 6 */
            {
                {0, 0, 0, 0, 0, /*0 - 4*/},
                {0, 0, 0, 0, 0, /*0 - 4*/},
                {0, 0, 0, 0, 0, /*0 - 4*/},
                {0, 0, 0, 0, 0, /*0 - 4*/},
            },
            /* nMdeStrength3 7 */
            {
                {0, 0, 0, 0, 0, /*0 - 4*/},
                {0, 0, 0, 0, 0, /*0 - 4*/},
                {0, 0, 0, 0, 0, /*0 - 4*/},
                {0, 0, 0, 0, 0, /*0 - 4*/},
            },
            /* nMdeStrength3 8 */
            {
                {0, 0, 0, 0, 0, /*0 - 4*/},
                {0, 0, 0, 0, 0, /*0 - 4*/},
                {0, 0, 0, 0, 0, /*0 - 4*/},
                {0, 0, 0, 0, 0, /*0 - 4*/},
            },
            /* nMdeStrength3 9 */
            {
                {0, 0, 0, 0, 0, /*0 - 4*/},
                {0, 0, 0, 0, 0, /*0 - 4*/},
                {0, 0, 0, 0, 0, /*0 - 4*/},
                {0, 0, 0, 0, 0, /*0 - 4*/},
            },
            /* nMdeStrength3 10 */
            {
                {0, 0, 0, 0, 0, /*0 - 4*/},
                {0, 0, 0, 0, 0, /*0 - 4*/},
                {0, 0, 0, 0, 0, /*0 - 4*/},
                {0, 0, 0, 0, 0, /*0 - 4*/},
            },
            /* nMdeStrength3 11 */
            {
                {0, 0, 0, 0, 0, /*0 - 4*/},
                {0, 0, 0, 0, 0, /*0 - 4*/},
                {0, 0, 0, 0, 0, /*0 - 4*/},
                {0, 0, 0, 0, 0, /*0 - 4*/},
            },
        },
        /* nMdeStrength4[16][4][5] */
        {
            /* nMdeStrength4 0 */
            {
                {0, 0, 0, 0, 0, /*0 - 4*/},
                {0, 0, 0, 0, 0, /*0 - 4*/},
                {0, 0, 0, 0, 0, /*0 - 4*/},
                {0, 0, 0, 0, 0, /*0 - 4*/},
            },
            /* nMdeStrength4 1 */
            {
                {0, 0, 0, 0, 0, /*0 - 4*/},
                {0, 0, 0, 0, 0, /*0 - 4*/},
                {0, 0, 0, 0, 0, /*0 - 4*/},
                {0, 0, 0, 0, 0, /*0 - 4*/},
            },
            /* nMdeStrength4 2 */
            {
                {0, 0, 0, 0, 0, /*0 - 4*/},
                {0, 0, 0, 0, 0, /*0 - 4*/},
                {0, 0, 0, 0, 0, /*0 - 4*/},
                {0, 0, 0, 0, 0, /*0 - 4*/},
            },
            /* nMdeStrength4 3 */
            {
                {0, 0, 0, 0, 0, /*0 - 4*/},
                {0, 0, 0, 0, 0, /*0 - 4*/},
                {0, 0, 0, 0, 0, /*0 - 4*/},
                {0, 0, 0, 0, 0, /*0 - 4*/},
            },
            /* nMdeStrength4 4 */
            {
                {0, 0, 0, 0, 0, /*0 - 4*/},
                {0, 0, 0, 0, 0, /*0 - 4*/},
                {0, 0, 0, 0, 0, /*0 - 4*/},
                {0, 0, 0, 0, 0, /*0 - 4*/},
            },
            /* nMdeStrength4 5 */
            {
                {0, 0, 0, 0, 0, /*0 - 4*/},
                {0, 0, 0, 0, 0, /*0 - 4*/},
                {0, 0, 0, 0, 0, /*0 - 4*/},
                {0, 0, 0, 0, 0, /*0 - 4*/},
            },
            /* nMdeStrength4 6 */
            {
                {0, 0, 0, 0, 0, /*0 - 4*/},
                {0, 0, 0, 0, 0, /*0 - 4*/},
                {0, 0, 0, 0, 0, /*0 - 4*/},
                {0, 0, 0, 0, 0, /*0 - 4*/},
            },
            /* nMdeStrength4 7 */
            {
                {0, 0, 0, 0, 0, /*0 - 4*/},
                {0, 0, 0, 0, 0, /*0 - 4*/},
                {0, 0, 0, 0, 0, /*0 - 4*/},
                {0, 0, 0, 0, 0, /*0 - 4*/},
            },
            /* nMdeStrength4 8 */
            {
                {0, 0, 0, 0, 0, /*0 - 4*/},
                {0, 0, 0, 0, 0, /*0 - 4*/},
                {0, 0, 0, 0, 0, /*0 - 4*/},
                {0, 0, 0, 0, 0, /*0 - 4*/},
            },
            /* nMdeStrength4 9 */
            {
                {0, 0, 0, 0, 0, /*0 - 4*/},
                {0, 0, 0, 0, 0, /*0 - 4*/},
                {0, 0, 0, 0, 0, /*0 - 4*/},
                {0, 0, 0, 0, 0, /*0 - 4*/},
            },
            /* nMdeStrength4 10 */
            {
                {0, 0, 0, 0, 0, /*0 - 4*/},
                {0, 0, 0, 0, 0, /*0 - 4*/},
                {0, 0, 0, 0, 0, /*0 - 4*/},
                {0, 0, 0, 0, 0, /*0 - 4*/},
            },
            /* nMdeStrength4 11 */
            {
                {0, 0, 0, 0, 0, /*0 - 4*/},
                {0, 0, 0, 0, 0, /*0 - 4*/},
                {0, 0, 0, 0, 0, /*0 - 4*/},
                {0, 0, 0, 0, 0, /*0 - 4*/},
            },
        },
        /* nMdeStrength5[16][4][5] */
        {
            /* nMdeStrength5 0 */
            {
                {0, 0, 0, 0, 0, /*0 - 4*/},
                {0, 0, 0, 0, 0, /*0 - 4*/},
                {0, 0, 0, 0, 0, /*0 - 4*/},
                {0, 0, 0, 0, 0, /*0 - 4*/},
            },
            /* nMdeStrength5 1 */
            {
                {0, 0, 0, 0, 0, /*0 - 4*/},
                {0, 0, 0, 0, 0, /*0 - 4*/},
                {0, 0, 0, 0, 0, /*0 - 4*/},
                {0, 0, 0, 0, 0, /*0 - 4*/},
            },
            /* nMdeStrength5 2 */
            {
                {0, 0, 0, 0, 0, /*0 - 4*/},
                {0, 0, 0, 0, 0, /*0 - 4*/},
                {0, 0, 0, 0, 0, /*0 - 4*/},
                {0, 0, 0, 0, 0, /*0 - 4*/},
            },
            /* nMdeStrength5 3 */
            {
                {0, 0, 0, 0, 0, /*0 - 4*/},
                {0, 0, 0, 0, 0, /*0 - 4*/},
                {0, 0, 0, 0, 0, /*0 - 4*/},
                {0, 0, 0, 0, 0, /*0 - 4*/},
            },
            /* nMdeStrength5 4 */
            {
                {0, 0, 0, 0, 0, /*0 - 4*/},
                {0, 0, 0, 0, 0, /*0 - 4*/},
                {0, 0, 0, 0, 0, /*0 - 4*/},
                {0, 0, 0, 0, 0, /*0 - 4*/},
            },
            /* nMdeStrength5 5 */
            {
                {0, 0, 0, 0, 0, /*0 - 4*/},
                {0, 0, 0, 0, 0, /*0 - 4*/},
                {0, 0, 0, 0, 0, /*0 - 4*/},
                {0, 0, 0, 0, 0, /*0 - 4*/},
            },
            /* nMdeStrength5 6 */
            {
                {0, 0, 0, 0, 0, /*0 - 4*/},
                {0, 0, 0, 0, 0, /*0 - 4*/},
                {0, 0, 0, 0, 0, /*0 - 4*/},
                {0, 0, 0, 0, 0, /*0 - 4*/},
            },
            /* nMdeStrength5 7 */
            {
                {0, 0, 0, 0, 0, /*0 - 4*/},
                {0, 0, 0, 0, 0, /*0 - 4*/},
                {0, 0, 0, 0, 0, /*0 - 4*/},
                {0, 0, 0, 0, 0, /*0 - 4*/},
            },
            /* nMdeStrength5 8 */
            {
                {0, 0, 0, 0, 0, /*0 - 4*/},
                {0, 0, 0, 0, 0, /*0 - 4*/},
                {0, 0, 0, 0, 0, /*0 - 4*/},
                {0, 0, 0, 0, 0, /*0 - 4*/},
            },
            /* nMdeStrength5 9 */
            {
                {0, 0, 0, 0, 0, /*0 - 4*/},
                {0, 0, 0, 0, 0, /*0 - 4*/},
                {0, 0, 0, 0, 0, /*0 - 4*/},
                {0, 0, 0, 0, 0, /*0 - 4*/},
            },
            /* nMdeStrength5 10 */
            {
                {0, 0, 0, 0, 0, /*0 - 4*/},
                {0, 0, 0, 0, 0, /*0 - 4*/},
                {0, 0, 0, 0, 0, /*0 - 4*/},
                {0, 0, 0, 0, 0, /*0 - 4*/},
            },
            /* nMdeStrength5 11 */
            {
                {0, 0, 0, 0, 0, /*0 - 4*/},
                {0, 0, 0, 0, 0, /*0 - 4*/},
                {0, 0, 0, 0, 0, /*0 - 4*/},
                {0, 0, 0, 0, 0, /*0 - 4*/},
            },
        },
        /* nMdeStrength6[16][4] */
        {
            {0, 0, 0, 0, /*0 - 3*/},
            {0, 0, 0, 0, /*0 - 3*/},
            {0, 0, 0, 0, /*0 - 3*/},
            {0, 0, 0, 0, /*0 - 3*/},
            {0, 0, 0, 0, /*0 - 3*/},
            {0, 0, 0, 0, /*0 - 3*/},
            {0, 0, 0, 0, /*0 - 3*/},
            {0, 0, 0, 0, /*0 - 3*/},
            {0, 0, 0, 0, /*0 - 3*/},
            {0, 0, 0, 0, /*0 - 3*/},
            {0, 0, 0, 0, /*0 - 3*/},
            {0, 0, 0, 0, /*0 - 3*/},
        },
        /* nMdeStrength7[16][4] */
        {
            {0, 0, 0, 0, /*0 - 3*/},
            {0, 0, 0, 0, /*0 - 3*/},
            {0, 0, 0, 0, /*0 - 3*/},
            {0, 0, 0, 0, /*0 - 3*/},
            {0, 0, 0, 0, /*0 - 3*/},
            {0, 0, 0, 0, /*0 - 3*/},
            {0, 0, 0, 0, /*0 - 3*/},
            {0, 0, 0, 0, /*0 - 3*/},
            {0, 0, 0, 0, /*0 - 3*/},
            {0, 0, 0, 0, /*0 - 3*/},
            {0, 0, 0, 0, /*0 - 3*/},
            {0, 0, 0, 0, /*0 - 3*/},
        },
        /* nMdeLumaLut[16][4][129] */
        {
            /* nMdeLumaLut 0 */
            {
                {
                    256, 264, 269, 271, 271, 269, 265, 260, 256, 251, 246, 242, 240, 240, 242, 247, 256, 267, 282, 299, 317, 335, 353, 369, 384, 395, 402, 407, 408, 407, 401, 393,  /* 0 - 31*/
                    381, 367, 350, 331, 312, 294, 277, 263, 251, 245, 241, 241, 242, 245, 249, 253, 256, 257, 257, 256, 255, 253, 251, 250, 248, 248, 249, 250, 251, 252, 254, 255,  /* 32 - 63*/
                    256, 256, 255, 255, 254, 253, 253, 252, 251, 252, 252, 252, 253, 254, 254, 254, 255, 254, 254, 253, 253, 252, 253, 254, 256, 258, 261, 264, 266, 267, 265, 262,  /* 64 - 95*/
                    255, 244, 230, 214, 196, 177, 157, 138, 119, 101, 84, 69, 56, 45, 35, 27, 22, 18, 16, 16, 17, 19, 22, 26, 30, 33, 36, 39, 40, 41, 39, 37,  /* 96 - 127*/
                    32, /*128 - 128*/
                },
                {
                    384, 383, 382, 382, 382, 382, 383, 383, 384, 384, 384, 385, 385, 385, 385, 384, 384, 383, 381, 380, 380, 379, 380, 381, 384, 387, 391, 395, 397, 398, 397, 392,  /* 0 - 31*/
                    384, 370, 354, 335, 315, 295, 277, 261, 250, 242, 239, 239, 242, 246, 250, 253, 256, 256, 254, 251, 247, 243, 239, 236, 235, 235, 237, 239, 243, 247, 250, 253,  /* 32 - 63*/
                    256, 256, 256, 255, 253, 251, 249, 247, 246, 245, 245, 246, 247, 249, 250, 252, 254, 255, 256, 257, 257, 257, 257, 257, 256, 254, 253, 251, 250, 249, 250, 251,  /* 64 - 95*/
                    255, 259, 264, 269, 272, 272, 268, 260, 246, 224, 198, 168, 136, 104, 74, 48, 27, 12, 3, 0, 1, 6, 13, 22, 32, 41, 50, 57, 61, 61, 57, 47,  /* 96 - 127*/
                    32, /*128 - 128*/
                },
                {
                    256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,  /* 0 - 31*/
                    256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,  /* 32 - 63*/
                    256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,  /* 64 - 95*/
                    256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,  /* 96 - 127*/
                    256, /*128 - 128*/
                },
                {
                    256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,  /* 0 - 31*/
                    256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,  /* 32 - 63*/
                    256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,  /* 64 - 95*/
                    256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,  /* 96 - 127*/
                    256, /*128 - 128*/
                },
            },
            /* nMdeLumaLut 1 */
            {
                {
                    256, 264, 269, 271, 271, 269, 265, 260, 256, 251, 246, 242, 240, 240, 242, 247, 256, 267, 282, 299, 317, 335, 353, 369, 384, 395, 402, 407, 408, 407, 401, 393,  /* 0 - 31*/
                    381, 367, 350, 331, 312, 294, 277, 263, 251, 245, 241, 241, 242, 245, 249, 253, 256, 257, 257, 256, 255, 253, 251, 250, 248, 248, 249, 250, 251, 252, 254, 255,  /* 32 - 63*/
                    256, 256, 255, 255, 254, 253, 253, 252, 251, 252, 252, 252, 253, 254, 254, 254, 255, 254, 254, 253, 253, 252, 253, 254, 256, 258, 261, 264, 266, 267, 265, 262,  /* 64 - 95*/
                    255, 244, 230, 214, 196, 177, 157, 138, 119, 101, 84, 69, 56, 45, 35, 27, 22, 18, 16, 16, 17, 19, 22, 26, 30, 33, 36, 39, 40, 41, 39, 37,  /* 96 - 127*/
                    32, /*128 - 128*/
                },
                {
                    384, 383, 382, 382, 382, 382, 383, 383, 384, 384, 384, 385, 385, 385, 385, 384, 384, 383, 381, 380, 380, 379, 380, 381, 384, 387, 391, 395, 397, 398, 397, 392,  /* 0 - 31*/
                    384, 370, 354, 335, 315, 295, 277, 261, 250, 242, 239, 239, 242, 246, 250, 253, 256, 256, 254, 251, 247, 243, 239, 236, 235, 235, 237, 239, 243, 247, 250, 253,  /* 32 - 63*/
                    256, 256, 256, 255, 253, 251, 249, 247, 246, 245, 245, 246, 247, 249, 250, 252, 254, 255, 256, 257, 257, 257, 257, 257, 256, 254, 253, 251, 250, 249, 250, 251,  /* 64 - 95*/
                    255, 259, 264, 269, 272, 272, 268, 260, 246, 224, 198, 168, 136, 104, 74, 48, 27, 12, 3, 0, 1, 6, 13, 22, 32, 41, 50, 57, 61, 61, 57, 47,  /* 96 - 127*/
                    32, /*128 - 128*/
                },
                {
                    256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,  /* 0 - 31*/
                    256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,  /* 32 - 63*/
                    256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,  /* 64 - 95*/
                    256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,  /* 96 - 127*/
                    256, /*128 - 128*/
                },
                {
                    256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,  /* 0 - 31*/
                    256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,  /* 32 - 63*/
                    256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,  /* 64 - 95*/
                    256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,  /* 96 - 127*/
                    256, /*128 - 128*/
                },
            },
            /* nMdeLumaLut 2 */
            {
                {
                    256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,  /* 0 - 31*/
                    256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,  /* 32 - 63*/
                    256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,  /* 64 - 95*/
                    256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,  /* 96 - 127*/
                    256, /*128 - 128*/
                },
                {
                    256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,  /* 0 - 31*/
                    256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,  /* 32 - 63*/
                    256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,  /* 64 - 95*/
                    256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,  /* 96 - 127*/
                    256, /*128 - 128*/
                },
                {
                    256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,  /* 0 - 31*/
                    256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,  /* 32 - 63*/
                    256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,  /* 64 - 95*/
                    256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,  /* 96 - 127*/
                    256, /*128 - 128*/
                },
                {
                    256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,  /* 0 - 31*/
                    256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,  /* 32 - 63*/
                    256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,  /* 64 - 95*/
                    256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,  /* 96 - 127*/
                    256, /*128 - 128*/
                },
            },
            /* nMdeLumaLut 3 */
            {
                {
                    256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,  /* 0 - 31*/
                    256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,  /* 32 - 63*/
                    256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,  /* 64 - 95*/
                    256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,  /* 96 - 127*/
                    256, /*128 - 128*/
                },
                {
                    256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,  /* 0 - 31*/
                    256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,  /* 32 - 63*/
                    256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,  /* 64 - 95*/
                    256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,  /* 96 - 127*/
                    256, /*128 - 128*/
                },
                {
                    256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,  /* 0 - 31*/
                    256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,  /* 32 - 63*/
                    256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,  /* 64 - 95*/
                    256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,  /* 96 - 127*/
                    256, /*128 - 128*/
                },
                {
                    256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,  /* 0 - 31*/
                    256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,  /* 32 - 63*/
                    256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,  /* 64 - 95*/
                    256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,  /* 96 - 127*/
                    256, /*128 - 128*/
                },
            },
            /* nMdeLumaLut 4 */
            {
                {
                    256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,  /* 0 - 31*/
                    256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,  /* 32 - 63*/
                    256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,  /* 64 - 95*/
                    256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,  /* 96 - 127*/
                    256, /*128 - 128*/
                },
                {
                    256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,  /* 0 - 31*/
                    256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,  /* 32 - 63*/
                    256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,  /* 64 - 95*/
                    256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,  /* 96 - 127*/
                    256, /*128 - 128*/
                },
                {
                    256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,  /* 0 - 31*/
                    256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,  /* 32 - 63*/
                    256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,  /* 64 - 95*/
                    256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,  /* 96 - 127*/
                    256, /*128 - 128*/
                },
                {
                    256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,  /* 0 - 31*/
                    256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,  /* 32 - 63*/
                    256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,  /* 64 - 95*/
                    256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,  /* 96 - 127*/
                    256, /*128 - 128*/
                },
            },
            /* nMdeLumaLut 5 */
            {
                {
                    256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,  /* 0 - 31*/
                    256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,  /* 32 - 63*/
                    256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,  /* 64 - 95*/
                    256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,  /* 96 - 127*/
                    256, /*128 - 128*/
                },
                {
                    256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,  /* 0 - 31*/
                    256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,  /* 32 - 63*/
                    256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,  /* 64 - 95*/
                    256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,  /* 96 - 127*/
                    256, /*128 - 128*/
                },
                {
                    256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,  /* 0 - 31*/
                    256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,  /* 32 - 63*/
                    256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,  /* 64 - 95*/
                    256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,  /* 96 - 127*/
                    256, /*128 - 128*/
                },
                {
                    256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,  /* 0 - 31*/
                    256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,  /* 32 - 63*/
                    256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,  /* 64 - 95*/
                    256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,  /* 96 - 127*/
                    256, /*128 - 128*/
                },
            },
            /* nMdeLumaLut 6 */
            {
                {
                    312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312,  /* 0 - 31*/
                    312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312,  /* 32 - 63*/
                    312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312,  /* 64 - 95*/
                    312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312,  /* 96 - 127*/
                    312, /*128 - 128*/
                },
                {
                    256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,  /* 0 - 31*/
                    256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,  /* 32 - 63*/
                    256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,  /* 64 - 95*/
                    256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,  /* 96 - 127*/
                    256, /*128 - 128*/
                },
                {
                    256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,  /* 0 - 31*/
                    256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,  /* 32 - 63*/
                    256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,  /* 64 - 95*/
                    256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,  /* 96 - 127*/
                    256, /*128 - 128*/
                },
                {
                    256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,  /* 0 - 31*/
                    256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,  /* 32 - 63*/
                    256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,  /* 64 - 95*/
                    256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,  /* 96 - 127*/
                    256, /*128 - 128*/
                },
            },
            /* nMdeLumaLut 7 */
            {
                {
                    312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312,  /* 0 - 31*/
                    312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312,  /* 32 - 63*/
                    312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312,  /* 64 - 95*/
                    312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312,  /* 96 - 127*/
                    312, /*128 - 128*/
                },
                {
                    256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,  /* 0 - 31*/
                    256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,  /* 32 - 63*/
                    256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,  /* 64 - 95*/
                    256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,  /* 96 - 127*/
                    256, /*128 - 128*/
                },
                {
                    256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,  /* 0 - 31*/
                    256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,  /* 32 - 63*/
                    256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,  /* 64 - 95*/
                    256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,  /* 96 - 127*/
                    256, /*128 - 128*/
                },
                {
                    256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,  /* 0 - 31*/
                    256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,  /* 32 - 63*/
                    256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,  /* 64 - 95*/
                    256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,  /* 96 - 127*/
                    256, /*128 - 128*/
                },
            },
            /* nMdeLumaLut 8 */
            {
                {
                    312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312,  /* 0 - 31*/
                    312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312,  /* 32 - 63*/
                    312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312,  /* 64 - 95*/
                    312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312,  /* 96 - 127*/
                    312, /*128 - 128*/
                },
                {
                    256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,  /* 0 - 31*/
                    256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,  /* 32 - 63*/
                    256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,  /* 64 - 95*/
                    256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,  /* 96 - 127*/
                    256, /*128 - 128*/
                },
                {
                    256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,  /* 0 - 31*/
                    256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,  /* 32 - 63*/
                    256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,  /* 64 - 95*/
                    256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,  /* 96 - 127*/
                    256, /*128 - 128*/
                },
                {
                    256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,  /* 0 - 31*/
                    256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,  /* 32 - 63*/
                    256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,  /* 64 - 95*/
                    256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,  /* 96 - 127*/
                    256, /*128 - 128*/
                },
            },
            /* nMdeLumaLut 9 */
            {
                {
                    312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312,  /* 0 - 31*/
                    312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312,  /* 32 - 63*/
                    312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312,  /* 64 - 95*/
                    312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312, 312,  /* 96 - 127*/
                    312, /*128 - 128*/
                },
                {
                    256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,  /* 0 - 31*/
                    256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,  /* 32 - 63*/
                    256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,  /* 64 - 95*/
                    256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,  /* 96 - 127*/
                    256, /*128 - 128*/
                },
                {
                    256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,  /* 0 - 31*/
                    256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,  /* 32 - 63*/
                    256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,  /* 64 - 95*/
                    256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,  /* 96 - 127*/
                    256, /*128 - 128*/
                },
                {
                    256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,  /* 0 - 31*/
                    256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,  /* 32 - 63*/
                    256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,  /* 64 - 95*/
                    256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,  /* 96 - 127*/
                    256, /*128 - 128*/
                },
            },
            /* nMdeLumaLut 10 */
            {
                {
                    412, 412, 412, 412, 412, 412, 412, 412, 412, 412, 412, 412, 412, 412, 412, 412, 412, 412, 412, 412, 412, 412, 412, 412, 412, 412, 412, 412, 412, 412, 412, 412,  /* 0 - 31*/
                    412, 412, 412, 412, 412, 412, 412, 412, 412, 412, 412, 412, 412, 412, 412, 412, 412, 412, 412, 412, 412, 412, 412, 412, 412, 412, 412, 412, 412, 412, 412, 412,  /* 32 - 63*/
                    412, 412, 412, 412, 412, 412, 412, 412, 412, 412, 412, 412, 412, 412, 412, 412, 412, 412, 412, 412, 412, 412, 412, 412, 412, 412, 412, 412, 412, 412, 412, 412,  /* 64 - 95*/
                    412, 412, 412, 412, 412, 412, 412, 412, 412, 412, 412, 412, 412, 412, 412, 412, 412, 412, 412, 412, 412, 412, 412, 412, 412, 412, 412, 412, 412, 412, 412, 412,  /* 96 - 127*/
                    412, /*128 - 128*/
                },
                {
                    256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,  /* 0 - 31*/
                    256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,  /* 32 - 63*/
                    256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,  /* 64 - 95*/
                    256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,  /* 96 - 127*/
                    256, /*128 - 128*/
                },
                {
                    256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,  /* 0 - 31*/
                    256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,  /* 32 - 63*/
                    256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,  /* 64 - 95*/
                    256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,  /* 96 - 127*/
                    256, /*128 - 128*/
                },
                {
                    256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,  /* 0 - 31*/
                    256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,  /* 32 - 63*/
                    256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,  /* 64 - 95*/
                    256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,  /* 96 - 127*/
                    256, /*128 - 128*/
                },
            },
            /* nMdeLumaLut 11 */
            {
                {
                    412, 412, 412, 412, 412, 412, 412, 412, 412, 412, 412, 412, 412, 412, 412, 412, 412, 412, 412, 412, 412, 412, 412, 412, 412, 412, 412, 412, 412, 412, 412, 412,  /* 0 - 31*/
                    412, 412, 412, 412, 412, 412, 412, 412, 412, 412, 412, 412, 412, 412, 412, 412, 412, 412, 412, 412, 412, 412, 412, 412, 412, 412, 412, 412, 412, 412, 412, 412,  /* 32 - 63*/
                    412, 412, 412, 412, 412, 412, 412, 412, 412, 412, 412, 412, 412, 412, 412, 412, 412, 412, 412, 412, 412, 412, 412, 412, 412, 412, 412, 412, 412, 412, 412, 412,  /* 64 - 95*/
                    412, 412, 412, 412, 412, 412, 412, 412, 412, 412, 412, 412, 412, 412, 412, 412, 412, 412, 412, 412, 412, 412, 412, 412, 412, 412, 412, 412, 412, 412, 412, 412,  /* 96 - 127*/
                    412, /*128 - 128*/
                },
                {
                    256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,  /* 0 - 31*/
                    256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,  /* 32 - 63*/
                    256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,  /* 64 - 95*/
                    256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,  /* 96 - 127*/
                    256, /*128 - 128*/
                },
                {
                    256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,  /* 0 - 31*/
                    256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,  /* 32 - 63*/
                    256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,  /* 64 - 95*/
                    256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,  /* 96 - 127*/
                    256, /*128 - 128*/
                },
                {
                    256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,  /* 0 - 31*/
                    256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,  /* 32 - 63*/
                    256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,  /* 64 - 95*/
                    256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,  /* 96 - 127*/
                    256, /*128 - 128*/
                },
            },
        },
        /* nMdeLevelLut[16][4][129] */
        {
            /* nMdeLevelLut 0 */
            {
                {
                    0, 32, 64, 96, 128, 160, 192, 224, 256, 288, 320, 352, 384, 416, 448, 480, 512, 544, 576, 608, 640, 672, 704, 736, 768, 800, 832, 864, 896, 928, 960, 992,  /* 0 - 31*/
                    1024, 1056, 1088, 1120, 1152, 1184, 1216, 1248, 1280, 1312, 1344, 1376, 1408, 1440, 1472, 1504, 1536, 1568, 1600, 1632, 1664, 1696, 1728, 1760, 1792, 1824, 1856, 1888, 1920, 1952, 1984, 2016,  /* 32 - 63*/
                    2048, 2080, 2112, 2144, 2176, 2208, 2240, 2272, 2304, 2336, 2368, 2400, 2432, 2464, 2496, 2528, 2560, 2592, 2624, 2656, 2688, 2720, 2752, 2784, 2816, 2848, 2880, 2912, 2944, 2976, 3008, 3040,  /* 64 - 95*/
                    3072, 3104, 3136, 3168, 3200, 3232, 3264, 3296, 3328, 3360, 3392, 3424, 3456, 3488, 3520, 3552, 3584, 3616, 3648, 3680, 3712, 3744, 3776, 3808, 3840, 3872, 3904, 3936, 3968, 4000, 4032, 4064,  /* 96 - 127*/
                    4095, /*128 - 128*/
                },
                {
                    0, 32, 64, 96, 128, 160, 192, 224, 256, 288, 320, 352, 384, 416, 448, 480, 512, 544, 576, 608, 640, 672, 704, 736, 768, 800, 832, 864, 896, 928, 960, 992,  /* 0 - 31*/
                    1024, 1056, 1088, 1120, 1152, 1184, 1216, 1248, 1280, 1312, 1344, 1376, 1408, 1440, 1472, 1504, 1536, 1568, 1600, 1632, 1664, 1696, 1728, 1760, 1792, 1824, 1856, 1888, 1920, 1952, 1984, 2016,  /* 32 - 63*/
                    2048, 2080, 2112, 2144, 2176, 2208, 2240, 2272, 2304, 2336, 2368, 2400, 2432, 2464, 2496, 2528, 2560, 2592, 2624, 2656, 2688, 2720, 2752, 2784, 2816, 2848, 2880, 2912, 2944, 2976, 3008, 3040,  /* 64 - 95*/
                    3072, 3104, 3136, 3168, 3200, 3232, 3264, 3296, 3328, 3360, 3392, 3424, 3456, 3488, 3520, 3552, 3584, 3616, 3648, 3680, 3712, 3744, 3776, 3808, 3840, 3872, 3904, 3936, 3968, 4000, 4032, 4064,  /* 96 - 127*/
                    4095, /*128 - 128*/
                },
                {
                    0, 32, 64, 96, 128, 160, 192, 224, 256, 288, 320, 352, 384, 416, 448, 480, 512, 544, 576, 608, 640, 672, 704, 736, 768, 800, 832, 864, 896, 928, 960, 992,  /* 0 - 31*/
                    1024, 1056, 1088, 1120, 1152, 1184, 1216, 1248, 1280, 1312, 1344, 1376, 1408, 1440, 1472, 1504, 1536, 1568, 1600, 1632, 1664, 1696, 1728, 1760, 1792, 1824, 1856, 1888, 1920, 1952, 1984, 2016,  /* 32 - 63*/
                    2048, 2080, 2112, 2144, 2176, 2208, 2240, 2272, 2304, 2336, 2368, 2400, 2432, 2464, 2496, 2528, 2560, 2592, 2624, 2656, 2688, 2720, 2752, 2784, 2816, 2848, 2880, 2912, 2944, 2976, 3008, 3040,  /* 64 - 95*/
                    3072, 3104, 3136, 3168, 3200, 3232, 3264, 3296, 3328, 3360, 3392, 3424, 3456, 3488, 3520, 3552, 3584, 3616, 3648, 3680, 3712, 3744, 3776, 3808, 3840, 3872, 3904, 3936, 3968, 4000, 4032, 4064,  /* 96 - 127*/
                    4095, /*128 - 128*/
                },
                {
                    0, 32, 64, 96, 128, 160, 192, 224, 256, 288, 320, 352, 384, 416, 448, 480, 512, 544, 576, 608, 640, 672, 704, 736, 768, 800, 832, 864, 896, 928, 960, 992,  /* 0 - 31*/
                    1024, 1056, 1088, 1120, 1152, 1184, 1216, 1248, 1280, 1312, 1344, 1376, 1408, 1440, 1472, 1504, 1536, 1568, 1600, 1632, 1664, 1696, 1728, 1760, 1792, 1824, 1856, 1888, 1920, 1952, 1984, 2016,  /* 32 - 63*/
                    2048, 2080, 2112, 2144, 2176, 2208, 2240, 2272, 2304, 2336, 2368, 2400, 2432, 2464, 2496, 2528, 2560, 2592, 2624, 2656, 2688, 2720, 2752, 2784, 2816, 2848, 2880, 2912, 2944, 2976, 3008, 3040,  /* 64 - 95*/
                    3072, 3104, 3136, 3168, 3200, 3232, 3264, 3296, 3328, 3360, 3392, 3424, 3456, 3488, 3520, 3552, 3584, 3616, 3648, 3680, 3712, 3744, 3776, 3808, 3840, 3872, 3904, 3936, 3968, 4000, 4032, 4064,  /* 96 - 127*/
                    4095, /*128 - 128*/
                },
            },
            /* nMdeLevelLut 1 */
            {
                {
                    0, 32, 64, 96, 128, 160, 192, 224, 256, 288, 320, 352, 384, 416, 448, 480, 512, 544, 576, 608, 640, 672, 704, 736, 768, 800, 832, 864, 896, 928, 960, 992,  /* 0 - 31*/
                    1024, 1056, 1088, 1120, 1152, 1184, 1216, 1248, 1280, 1312, 1344, 1376, 1408, 1440, 1472, 1504, 1536, 1568, 1600, 1632, 1664, 1696, 1728, 1760, 1792, 1824, 1856, 1888, 1920, 1952, 1984, 2016,  /* 32 - 63*/
                    2048, 2080, 2112, 2144, 2176, 2208, 2240, 2272, 2304, 2336, 2368, 2400, 2432, 2464, 2496, 2528, 2560, 2592, 2624, 2656, 2688, 2720, 2752, 2784, 2816, 2848, 2880, 2912, 2944, 2976, 3008, 3040,  /* 64 - 95*/
                    3072, 3104, 3136, 3168, 3200, 3232, 3264, 3296, 3328, 3360, 3392, 3424, 3456, 3488, 3520, 3552, 3584, 3616, 3648, 3680, 3712, 3744, 3776, 3808, 3840, 3872, 3904, 3936, 3968, 4000, 4032, 4064,  /* 96 - 127*/
                    4095, /*128 - 128*/
                },
                {
                    0, 32, 64, 96, 128, 160, 192, 224, 256, 288, 320, 352, 384, 416, 448, 480, 512, 544, 576, 608, 640, 672, 704, 736, 768, 800, 832, 864, 896, 928, 960, 992,  /* 0 - 31*/
                    1024, 1056, 1088, 1120, 1152, 1184, 1216, 1248, 1280, 1312, 1344, 1376, 1408, 1440, 1472, 1504, 1536, 1568, 1600, 1632, 1664, 1696, 1728, 1760, 1792, 1824, 1856, 1888, 1920, 1952, 1984, 2016,  /* 32 - 63*/
                    2048, 2080, 2112, 2144, 2176, 2208, 2240, 2272, 2304, 2336, 2368, 2400, 2432, 2464, 2496, 2528, 2560, 2592, 2624, 2656, 2688, 2720, 2752, 2784, 2816, 2848, 2880, 2912, 2944, 2976, 3008, 3040,  /* 64 - 95*/
                    3072, 3104, 3136, 3168, 3200, 3232, 3264, 3296, 3328, 3360, 3392, 3424, 3456, 3488, 3520, 3552, 3584, 3616, 3648, 3680, 3712, 3744, 3776, 3808, 3840, 3872, 3904, 3936, 3968, 4000, 4032, 4064,  /* 96 - 127*/
                    4095, /*128 - 128*/
                },
                {
                    0, 32, 64, 96, 128, 160, 192, 224, 256, 288, 320, 352, 384, 416, 448, 480, 512, 544, 576, 608, 640, 672, 704, 736, 768, 800, 832, 864, 896, 928, 960, 992,  /* 0 - 31*/
                    1024, 1056, 1088, 1120, 1152, 1184, 1216, 1248, 1280, 1312, 1344, 1376, 1408, 1440, 1472, 1504, 1536, 1568, 1600, 1632, 1664, 1696, 1728, 1760, 1792, 1824, 1856, 1888, 1920, 1952, 1984, 2016,  /* 32 - 63*/
                    2048, 2080, 2112, 2144, 2176, 2208, 2240, 2272, 2304, 2336, 2368, 2400, 2432, 2464, 2496, 2528, 2560, 2592, 2624, 2656, 2688, 2720, 2752, 2784, 2816, 2848, 2880, 2912, 2944, 2976, 3008, 3040,  /* 64 - 95*/
                    3072, 3104, 3136, 3168, 3200, 3232, 3264, 3296, 3328, 3360, 3392, 3424, 3456, 3488, 3520, 3552, 3584, 3616, 3648, 3680, 3712, 3744, 3776, 3808, 3840, 3872, 3904, 3936, 3968, 4000, 4032, 4064,  /* 96 - 127*/
                    4095, /*128 - 128*/
                },
                {
                    0, 32, 64, 96, 128, 160, 192, 224, 256, 288, 320, 352, 384, 416, 448, 480, 512, 544, 576, 608, 640, 672, 704, 736, 768, 800, 832, 864, 896, 928, 960, 992,  /* 0 - 31*/
                    1024, 1056, 1088, 1120, 1152, 1184, 1216, 1248, 1280, 1312, 1344, 1376, 1408, 1440, 1472, 1504, 1536, 1568, 1600, 1632, 1664, 1696, 1728, 1760, 1792, 1824, 1856, 1888, 1920, 1952, 1984, 2016,  /* 32 - 63*/
                    2048, 2080, 2112, 2144, 2176, 2208, 2240, 2272, 2304, 2336, 2368, 2400, 2432, 2464, 2496, 2528, 2560, 2592, 2624, 2656, 2688, 2720, 2752, 2784, 2816, 2848, 2880, 2912, 2944, 2976, 3008, 3040,  /* 64 - 95*/
                    3072, 3104, 3136, 3168, 3200, 3232, 3264, 3296, 3328, 3360, 3392, 3424, 3456, 3488, 3520, 3552, 3584, 3616, 3648, 3680, 3712, 3744, 3776, 3808, 3840, 3872, 3904, 3936, 3968, 4000, 4032, 4064,  /* 96 - 127*/
                    4095, /*128 - 128*/
                },
            },
            /* nMdeLevelLut 2 */
            {
                {
                    0, 32, 64, 96, 128, 160, 192, 224, 256, 288, 320, 352, 384, 416, 448, 480, 512, 544, 576, 608, 640, 672, 704, 736, 768, 800, 832, 864, 896, 928, 960, 992,  /* 0 - 31*/
                    1024, 1056, 1088, 1120, 1152, 1184, 1216, 1248, 1280, 1312, 1344, 1376, 1408, 1440, 1472, 1504, 1536, 1568, 1600, 1632, 1664, 1696, 1728, 1760, 1792, 1824, 1856, 1888, 1920, 1952, 1984, 2016,  /* 32 - 63*/
                    2048, 2080, 2112, 2144, 2176, 2208, 2240, 2272, 2304, 2336, 2368, 2400, 2432, 2464, 2496, 2528, 2560, 2592, 2624, 2656, 2688, 2720, 2752, 2784, 2816, 2848, 2880, 2912, 2944, 2976, 3008, 3040,  /* 64 - 95*/
                    3072, 3104, 3136, 3168, 3200, 3232, 3264, 3296, 3328, 3360, 3392, 3424, 3456, 3488, 3520, 3552, 3584, 3616, 3648, 3680, 3712, 3744, 3776, 3808, 3840, 3872, 3904, 3936, 3968, 4000, 4032, 4064,  /* 96 - 127*/
                    4095, /*128 - 128*/
                },
                {
                    0, 32, 64, 96, 128, 160, 192, 224, 256, 288, 320, 352, 384, 416, 448, 480, 512, 544, 576, 608, 640, 672, 704, 736, 768, 800, 832, 864, 896, 928, 960, 992,  /* 0 - 31*/
                    1024, 1056, 1088, 1120, 1152, 1184, 1216, 1248, 1280, 1312, 1344, 1376, 1408, 1440, 1472, 1504, 1536, 1568, 1600, 1632, 1664, 1696, 1728, 1760, 1792, 1824, 1856, 1888, 1920, 1952, 1984, 2016,  /* 32 - 63*/
                    2048, 2080, 2112, 2144, 2176, 2208, 2240, 2272, 2304, 2336, 2368, 2400, 2432, 2464, 2496, 2528, 2560, 2592, 2624, 2656, 2688, 2720, 2752, 2784, 2816, 2848, 2880, 2912, 2944, 2976, 3008, 3040,  /* 64 - 95*/
                    3072, 3104, 3136, 3168, 3200, 3232, 3264, 3296, 3328, 3360, 3392, 3424, 3456, 3488, 3520, 3552, 3584, 3616, 3648, 3680, 3712, 3744, 3776, 3808, 3840, 3872, 3904, 3936, 3968, 4000, 4032, 4064,  /* 96 - 127*/
                    4095, /*128 - 128*/
                },
                {
                    0, 32, 64, 96, 128, 160, 192, 224, 256, 288, 320, 352, 384, 416, 448, 480, 512, 544, 576, 608, 640, 672, 704, 736, 768, 800, 832, 864, 896, 928, 960, 992,  /* 0 - 31*/
                    1024, 1056, 1088, 1120, 1152, 1184, 1216, 1248, 1280, 1312, 1344, 1376, 1408, 1440, 1472, 1504, 1536, 1568, 1600, 1632, 1664, 1696, 1728, 1760, 1792, 1824, 1856, 1888, 1920, 1952, 1984, 2016,  /* 32 - 63*/
                    2048, 2080, 2112, 2144, 2176, 2208, 2240, 2272, 2304, 2336, 2368, 2400, 2432, 2464, 2496, 2528, 2560, 2592, 2624, 2656, 2688, 2720, 2752, 2784, 2816, 2848, 2880, 2912, 2944, 2976, 3008, 3040,  /* 64 - 95*/
                    3072, 3104, 3136, 3168, 3200, 3232, 3264, 3296, 3328, 3360, 3392, 3424, 3456, 3488, 3520, 3552, 3584, 3616, 3648, 3680, 3712, 3744, 3776, 3808, 3840, 3872, 3904, 3936, 3968, 4000, 4032, 4064,  /* 96 - 127*/
                    4095, /*128 - 128*/
                },
                {
                    0, 32, 64, 96, 128, 160, 192, 224, 256, 288, 320, 352, 384, 416, 448, 480, 512, 544, 576, 608, 640, 672, 704, 736, 768, 800, 832, 864, 896, 928, 960, 992,  /* 0 - 31*/
                    1024, 1056, 1088, 1120, 1152, 1184, 1216, 1248, 1280, 1312, 1344, 1376, 1408, 1440, 1472, 1504, 1536, 1568, 1600, 1632, 1664, 1696, 1728, 1760, 1792, 1824, 1856, 1888, 1920, 1952, 1984, 2016,  /* 32 - 63*/
                    2048, 2080, 2112, 2144, 2176, 2208, 2240, 2272, 2304, 2336, 2368, 2400, 2432, 2464, 2496, 2528, 2560, 2592, 2624, 2656, 2688, 2720, 2752, 2784, 2816, 2848, 2880, 2912, 2944, 2976, 3008, 3040,  /* 64 - 95*/
                    3072, 3104, 3136, 3168, 3200, 3232, 3264, 3296, 3328, 3360, 3392, 3424, 3456, 3488, 3520, 3552, 3584, 3616, 3648, 3680, 3712, 3744, 3776, 3808, 3840, 3872, 3904, 3936, 3968, 4000, 4032, 4064,  /* 96 - 127*/
                    4095, /*128 - 128*/
                },
            },
            /* nMdeLevelLut 3 */
            {
                {
                    0, 32, 64, 96, 128, 160, 192, 224, 256, 288, 320, 352, 384, 416, 448, 480, 512, 544, 576, 608, 640, 672, 704, 736, 768, 800, 832, 864, 896, 928, 960, 992,  /* 0 - 31*/
                    1024, 1056, 1088, 1120, 1152, 1184, 1216, 1248, 1280, 1312, 1344, 1376, 1408, 1440, 1472, 1504, 1536, 1568, 1600, 1632, 1664, 1696, 1728, 1760, 1792, 1824, 1856, 1888, 1920, 1952, 1984, 2016,  /* 32 - 63*/
                    2048, 2080, 2112, 2144, 2176, 2208, 2240, 2272, 2304, 2336, 2368, 2400, 2432, 2464, 2496, 2528, 2560, 2592, 2624, 2656, 2688, 2720, 2752, 2784, 2816, 2848, 2880, 2912, 2944, 2976, 3008, 3040,  /* 64 - 95*/
                    3072, 3104, 3136, 3168, 3200, 3232, 3264, 3296, 3328, 3360, 3392, 3424, 3456, 3488, 3520, 3552, 3584, 3616, 3648, 3680, 3712, 3744, 3776, 3808, 3840, 3872, 3904, 3936, 3968, 4000, 4032, 4064,  /* 96 - 127*/
                    4095, /*128 - 128*/
                },
                {
                    0, 32, 64, 96, 128, 160, 192, 224, 256, 288, 320, 352, 384, 416, 448, 480, 512, 544, 576, 608, 640, 672, 704, 736, 768, 800, 832, 864, 896, 928, 960, 992,  /* 0 - 31*/
                    1024, 1056, 1088, 1120, 1152, 1184, 1216, 1248, 1280, 1312, 1344, 1376, 1408, 1440, 1472, 1504, 1536, 1568, 1600, 1632, 1664, 1696, 1728, 1760, 1792, 1824, 1856, 1888, 1920, 1952, 1984, 2016,  /* 32 - 63*/
                    2048, 2080, 2112, 2144, 2176, 2208, 2240, 2272, 2304, 2336, 2368, 2400, 2432, 2464, 2496, 2528, 2560, 2592, 2624, 2656, 2688, 2720, 2752, 2784, 2816, 2848, 2880, 2912, 2944, 2976, 3008, 3040,  /* 64 - 95*/
                    3072, 3104, 3136, 3168, 3200, 3232, 3264, 3296, 3328, 3360, 3392, 3424, 3456, 3488, 3520, 3552, 3584, 3616, 3648, 3680, 3712, 3744, 3776, 3808, 3840, 3872, 3904, 3936, 3968, 4000, 4032, 4064,  /* 96 - 127*/
                    4095, /*128 - 128*/
                },
                {
                    0, 32, 64, 96, 128, 160, 192, 224, 256, 288, 320, 352, 384, 416, 448, 480, 512, 544, 576, 608, 640, 672, 704, 736, 768, 800, 832, 864, 896, 928, 960, 992,  /* 0 - 31*/
                    1024, 1056, 1088, 1120, 1152, 1184, 1216, 1248, 1280, 1312, 1344, 1376, 1408, 1440, 1472, 1504, 1536, 1568, 1600, 1632, 1664, 1696, 1728, 1760, 1792, 1824, 1856, 1888, 1920, 1952, 1984, 2016,  /* 32 - 63*/
                    2048, 2080, 2112, 2144, 2176, 2208, 2240, 2272, 2304, 2336, 2368, 2400, 2432, 2464, 2496, 2528, 2560, 2592, 2624, 2656, 2688, 2720, 2752, 2784, 2816, 2848, 2880, 2912, 2944, 2976, 3008, 3040,  /* 64 - 95*/
                    3072, 3104, 3136, 3168, 3200, 3232, 3264, 3296, 3328, 3360, 3392, 3424, 3456, 3488, 3520, 3552, 3584, 3616, 3648, 3680, 3712, 3744, 3776, 3808, 3840, 3872, 3904, 3936, 3968, 4000, 4032, 4064,  /* 96 - 127*/
                    4095, /*128 - 128*/
                },
                {
                    0, 32, 64, 96, 128, 160, 192, 224, 256, 288, 320, 352, 384, 416, 448, 480, 512, 544, 576, 608, 640, 672, 704, 736, 768, 800, 832, 864, 896, 928, 960, 992,  /* 0 - 31*/
                    1024, 1056, 1088, 1120, 1152, 1184, 1216, 1248, 1280, 1312, 1344, 1376, 1408, 1440, 1472, 1504, 1536, 1568, 1600, 1632, 1664, 1696, 1728, 1760, 1792, 1824, 1856, 1888, 1920, 1952, 1984, 2016,  /* 32 - 63*/
                    2048, 2080, 2112, 2144, 2176, 2208, 2240, 2272, 2304, 2336, 2368, 2400, 2432, 2464, 2496, 2528, 2560, 2592, 2624, 2656, 2688, 2720, 2752, 2784, 2816, 2848, 2880, 2912, 2944, 2976, 3008, 3040,  /* 64 - 95*/
                    3072, 3104, 3136, 3168, 3200, 3232, 3264, 3296, 3328, 3360, 3392, 3424, 3456, 3488, 3520, 3552, 3584, 3616, 3648, 3680, 3712, 3744, 3776, 3808, 3840, 3872, 3904, 3936, 3968, 4000, 4032, 4064,  /* 96 - 127*/
                    4095, /*128 - 128*/
                },
            },
            /* nMdeLevelLut 4 */
            {
                {
                    0, 32, 64, 96, 128, 160, 192, 224, 256, 288, 320, 352, 384, 416, 448, 480, 512, 544, 576, 608, 640, 672, 704, 736, 768, 800, 832, 864, 896, 928, 960, 992,  /* 0 - 31*/
                    1024, 1056, 1088, 1120, 1152, 1184, 1216, 1248, 1280, 1312, 1344, 1376, 1408, 1440, 1472, 1504, 1536, 1568, 1600, 1632, 1664, 1696, 1728, 1760, 1792, 1824, 1856, 1888, 1920, 1952, 1984, 2016,  /* 32 - 63*/
                    2048, 2080, 2112, 2144, 2176, 2208, 2240, 2272, 2304, 2336, 2368, 2400, 2432, 2464, 2496, 2528, 2560, 2592, 2624, 2656, 2688, 2720, 2752, 2784, 2816, 2848, 2880, 2912, 2944, 2976, 3008, 3040,  /* 64 - 95*/
                    3072, 3104, 3136, 3168, 3200, 3232, 3264, 3296, 3328, 3360, 3392, 3424, 3456, 3488, 3520, 3552, 3584, 3616, 3648, 3680, 3712, 3744, 3776, 3808, 3840, 3872, 3904, 3936, 3968, 4000, 4032, 4064,  /* 96 - 127*/
                    4095, /*128 - 128*/
                },
                {
                    0, 32, 64, 96, 128, 160, 192, 224, 256, 288, 320, 352, 384, 416, 448, 480, 512, 544, 576, 608, 640, 672, 704, 736, 768, 800, 832, 864, 896, 928, 960, 992,  /* 0 - 31*/
                    1024, 1056, 1088, 1120, 1152, 1184, 1216, 1248, 1280, 1312, 1344, 1376, 1408, 1440, 1472, 1504, 1536, 1568, 1600, 1632, 1664, 1696, 1728, 1760, 1792, 1824, 1856, 1888, 1920, 1952, 1984, 2016,  /* 32 - 63*/
                    2048, 2080, 2112, 2144, 2176, 2208, 2240, 2272, 2304, 2336, 2368, 2400, 2432, 2464, 2496, 2528, 2560, 2592, 2624, 2656, 2688, 2720, 2752, 2784, 2816, 2848, 2880, 2912, 2944, 2976, 3008, 3040,  /* 64 - 95*/
                    3072, 3104, 3136, 3168, 3200, 3232, 3264, 3296, 3328, 3360, 3392, 3424, 3456, 3488, 3520, 3552, 3584, 3616, 3648, 3680, 3712, 3744, 3776, 3808, 3840, 3872, 3904, 3936, 3968, 4000, 4032, 4064,  /* 96 - 127*/
                    4095, /*128 - 128*/
                },
                {
                    0, 32, 64, 96, 128, 160, 192, 224, 256, 288, 320, 352, 384, 416, 448, 480, 512, 544, 576, 608, 640, 672, 704, 736, 768, 800, 832, 864, 896, 928, 960, 992,  /* 0 - 31*/
                    1024, 1056, 1088, 1120, 1152, 1184, 1216, 1248, 1280, 1312, 1344, 1376, 1408, 1440, 1472, 1504, 1536, 1568, 1600, 1632, 1664, 1696, 1728, 1760, 1792, 1824, 1856, 1888, 1920, 1952, 1984, 2016,  /* 32 - 63*/
                    2048, 2080, 2112, 2144, 2176, 2208, 2240, 2272, 2304, 2336, 2368, 2400, 2432, 2464, 2496, 2528, 2560, 2592, 2624, 2656, 2688, 2720, 2752, 2784, 2816, 2848, 2880, 2912, 2944, 2976, 3008, 3040,  /* 64 - 95*/
                    3072, 3104, 3136, 3168, 3200, 3232, 3264, 3296, 3328, 3360, 3392, 3424, 3456, 3488, 3520, 3552, 3584, 3616, 3648, 3680, 3712, 3744, 3776, 3808, 3840, 3872, 3904, 3936, 3968, 4000, 4032, 4064,  /* 96 - 127*/
                    4095, /*128 - 128*/
                },
                {
                    0, 32, 64, 96, 128, 160, 192, 224, 256, 288, 320, 352, 384, 416, 448, 480, 512, 544, 576, 608, 640, 672, 704, 736, 768, 800, 832, 864, 896, 928, 960, 992,  /* 0 - 31*/
                    1024, 1056, 1088, 1120, 1152, 1184, 1216, 1248, 1280, 1312, 1344, 1376, 1408, 1440, 1472, 1504, 1536, 1568, 1600, 1632, 1664, 1696, 1728, 1760, 1792, 1824, 1856, 1888, 1920, 1952, 1984, 2016,  /* 32 - 63*/
                    2048, 2080, 2112, 2144, 2176, 2208, 2240, 2272, 2304, 2336, 2368, 2400, 2432, 2464, 2496, 2528, 2560, 2592, 2624, 2656, 2688, 2720, 2752, 2784, 2816, 2848, 2880, 2912, 2944, 2976, 3008, 3040,  /* 64 - 95*/
                    3072, 3104, 3136, 3168, 3200, 3232, 3264, 3296, 3328, 3360, 3392, 3424, 3456, 3488, 3520, 3552, 3584, 3616, 3648, 3680, 3712, 3744, 3776, 3808, 3840, 3872, 3904, 3936, 3968, 4000, 4032, 4064,  /* 96 - 127*/
                    4095, /*128 - 128*/
                },
            },
            /* nMdeLevelLut 5 */
            {
                {
                    0, 32, 64, 96, 128, 160, 192, 224, 256, 288, 320, 352, 384, 416, 448, 480, 512, 544, 576, 608, 640, 672, 704, 736, 768, 800, 832, 864, 896, 928, 960, 992,  /* 0 - 31*/
                    1024, 1056, 1088, 1120, 1152, 1184, 1216, 1248, 1280, 1312, 1344, 1376, 1408, 1440, 1472, 1504, 1536, 1568, 1600, 1632, 1664, 1696, 1728, 1760, 1792, 1824, 1856, 1888, 1920, 1952, 1984, 2016,  /* 32 - 63*/
                    2048, 2080, 2112, 2144, 2176, 2208, 2240, 2272, 2304, 2336, 2368, 2400, 2432, 2464, 2496, 2528, 2560, 2592, 2624, 2656, 2688, 2720, 2752, 2784, 2816, 2848, 2880, 2912, 2944, 2976, 3008, 3040,  /* 64 - 95*/
                    3072, 3104, 3136, 3168, 3200, 3232, 3264, 3296, 3328, 3360, 3392, 3424, 3456, 3488, 3520, 3552, 3584, 3616, 3648, 3680, 3712, 3744, 3776, 3808, 3840, 3872, 3904, 3936, 3968, 4000, 4032, 4064,  /* 96 - 127*/
                    4095, /*128 - 128*/
                },
                {
                    0, 32, 64, 96, 128, 160, 192, 224, 256, 288, 320, 352, 384, 416, 448, 480, 512, 544, 576, 608, 640, 672, 704, 736, 768, 800, 832, 864, 896, 928, 960, 992,  /* 0 - 31*/
                    1024, 1056, 1088, 1120, 1152, 1184, 1216, 1248, 1280, 1312, 1344, 1376, 1408, 1440, 1472, 1504, 1536, 1568, 1600, 1632, 1664, 1696, 1728, 1760, 1792, 1824, 1856, 1888, 1920, 1952, 1984, 2016,  /* 32 - 63*/
                    2048, 2080, 2112, 2144, 2176, 2208, 2240, 2272, 2304, 2336, 2368, 2400, 2432, 2464, 2496, 2528, 2560, 2592, 2624, 2656, 2688, 2720, 2752, 2784, 2816, 2848, 2880, 2912, 2944, 2976, 3008, 3040,  /* 64 - 95*/
                    3072, 3104, 3136, 3168, 3200, 3232, 3264, 3296, 3328, 3360, 3392, 3424, 3456, 3488, 3520, 3552, 3584, 3616, 3648, 3680, 3712, 3744, 3776, 3808, 3840, 3872, 3904, 3936, 3968, 4000, 4032, 4064,  /* 96 - 127*/
                    4095, /*128 - 128*/
                },
                {
                    0, 32, 64, 96, 128, 160, 192, 224, 256, 288, 320, 352, 384, 416, 448, 480, 512, 544, 576, 608, 640, 672, 704, 736, 768, 800, 832, 864, 896, 928, 960, 992,  /* 0 - 31*/
                    1024, 1056, 1088, 1120, 1152, 1184, 1216, 1248, 1280, 1312, 1344, 1376, 1408, 1440, 1472, 1504, 1536, 1568, 1600, 1632, 1664, 1696, 1728, 1760, 1792, 1824, 1856, 1888, 1920, 1952, 1984, 2016,  /* 32 - 63*/
                    2048, 2080, 2112, 2144, 2176, 2208, 2240, 2272, 2304, 2336, 2368, 2400, 2432, 2464, 2496, 2528, 2560, 2592, 2624, 2656, 2688, 2720, 2752, 2784, 2816, 2848, 2880, 2912, 2944, 2976, 3008, 3040,  /* 64 - 95*/
                    3072, 3104, 3136, 3168, 3200, 3232, 3264, 3296, 3328, 3360, 3392, 3424, 3456, 3488, 3520, 3552, 3584, 3616, 3648, 3680, 3712, 3744, 3776, 3808, 3840, 3872, 3904, 3936, 3968, 4000, 4032, 4064,  /* 96 - 127*/
                    4095, /*128 - 128*/
                },
                {
                    0, 32, 64, 96, 128, 160, 192, 224, 256, 288, 320, 352, 384, 416, 448, 480, 512, 544, 576, 608, 640, 672, 704, 736, 768, 800, 832, 864, 896, 928, 960, 992,  /* 0 - 31*/
                    1024, 1056, 1088, 1120, 1152, 1184, 1216, 1248, 1280, 1312, 1344, 1376, 1408, 1440, 1472, 1504, 1536, 1568, 1600, 1632, 1664, 1696, 1728, 1760, 1792, 1824, 1856, 1888, 1920, 1952, 1984, 2016,  /* 32 - 63*/
                    2048, 2080, 2112, 2144, 2176, 2208, 2240, 2272, 2304, 2336, 2368, 2400, 2432, 2464, 2496, 2528, 2560, 2592, 2624, 2656, 2688, 2720, 2752, 2784, 2816, 2848, 2880, 2912, 2944, 2976, 3008, 3040,  /* 64 - 95*/
                    3072, 3104, 3136, 3168, 3200, 3232, 3264, 3296, 3328, 3360, 3392, 3424, 3456, 3488, 3520, 3552, 3584, 3616, 3648, 3680, 3712, 3744, 3776, 3808, 3840, 3872, 3904, 3936, 3968, 4000, 4032, 4064,  /* 96 - 127*/
                    4095, /*128 - 128*/
                },
            },
            /* nMdeLevelLut 6 */
            {
                {
                    0, 32, 64, 96, 128, 160, 192, 224, 256, 288, 320, 352, 384, 416, 448, 480, 512, 544, 576, 608, 640, 672, 704, 736, 768, 800, 832, 864, 896, 928, 960, 992,  /* 0 - 31*/
                    1024, 1056, 1088, 1120, 1152, 1184, 1216, 1248, 1280, 1312, 1344, 1376, 1408, 1440, 1472, 1504, 1536, 1568, 1600, 1632, 1664, 1696, 1728, 1760, 1792, 1824, 1856, 1888, 1920, 1952, 1984, 2016,  /* 32 - 63*/
                    2048, 2080, 2112, 2144, 2176, 2208, 2240, 2272, 2304, 2336, 2368, 2400, 2432, 2464, 2496, 2528, 2560, 2592, 2624, 2656, 2688, 2720, 2752, 2784, 2816, 2848, 2880, 2912, 2944, 2976, 3008, 3040,  /* 64 - 95*/
                    3072, 3104, 3136, 3168, 3200, 3232, 3264, 3296, 3328, 3360, 3392, 3424, 3456, 3488, 3520, 3552, 3584, 3616, 3648, 3680, 3712, 3744, 3776, 3808, 3840, 3872, 3904, 3936, 3968, 4000, 4032, 4064,  /* 96 - 127*/
                    4095, /*128 - 128*/
                },
                {
                    0, 32, 64, 96, 128, 160, 192, 224, 256, 288, 320, 352, 384, 416, 448, 480, 512, 544, 576, 608, 640, 672, 704, 736, 768, 800, 832, 864, 896, 928, 960, 992,  /* 0 - 31*/
                    1024, 1056, 1088, 1120, 1152, 1184, 1216, 1248, 1280, 1312, 1344, 1376, 1408, 1440, 1472, 1504, 1536, 1568, 1600, 1632, 1664, 1696, 1728, 1760, 1792, 1824, 1856, 1888, 1920, 1952, 1984, 2016,  /* 32 - 63*/
                    2048, 2080, 2112, 2144, 2176, 2208, 2240, 2272, 2304, 2336, 2368, 2400, 2432, 2464, 2496, 2528, 2560, 2592, 2624, 2656, 2688, 2720, 2752, 2784, 2816, 2848, 2880, 2912, 2944, 2976, 3008, 3040,  /* 64 - 95*/
                    3072, 3104, 3136, 3168, 3200, 3232, 3264, 3296, 3328, 3360, 3392, 3424, 3456, 3488, 3520, 3552, 3584, 3616, 3648, 3680, 3712, 3744, 3776, 3808, 3840, 3872, 3904, 3936, 3968, 4000, 4032, 4064,  /* 96 - 127*/
                    4095, /*128 - 128*/
                },
                {
                    0, 32, 64, 96, 128, 160, 192, 224, 256, 288, 320, 352, 384, 416, 448, 480, 512, 544, 576, 608, 640, 672, 704, 736, 768, 800, 832, 864, 896, 928, 960, 992,  /* 0 - 31*/
                    1024, 1056, 1088, 1120, 1152, 1184, 1216, 1248, 1280, 1312, 1344, 1376, 1408, 1440, 1472, 1504, 1536, 1568, 1600, 1632, 1664, 1696, 1728, 1760, 1792, 1824, 1856, 1888, 1920, 1952, 1984, 2016,  /* 32 - 63*/
                    2048, 2080, 2112, 2144, 2176, 2208, 2240, 2272, 2304, 2336, 2368, 2400, 2432, 2464, 2496, 2528, 2560, 2592, 2624, 2656, 2688, 2720, 2752, 2784, 2816, 2848, 2880, 2912, 2944, 2976, 3008, 3040,  /* 64 - 95*/
                    3072, 3104, 3136, 3168, 3200, 3232, 3264, 3296, 3328, 3360, 3392, 3424, 3456, 3488, 3520, 3552, 3584, 3616, 3648, 3680, 3712, 3744, 3776, 3808, 3840, 3872, 3904, 3936, 3968, 4000, 4032, 4064,  /* 96 - 127*/
                    4095, /*128 - 128*/
                },
                {
                    0, 32, 64, 96, 128, 160, 192, 224, 256, 288, 320, 352, 384, 416, 448, 480, 512, 544, 576, 608, 640, 672, 704, 736, 768, 800, 832, 864, 896, 928, 960, 992,  /* 0 - 31*/
                    1024, 1056, 1088, 1120, 1152, 1184, 1216, 1248, 1280, 1312, 1344, 1376, 1408, 1440, 1472, 1504, 1536, 1568, 1600, 1632, 1664, 1696, 1728, 1760, 1792, 1824, 1856, 1888, 1920, 1952, 1984, 2016,  /* 32 - 63*/
                    2048, 2080, 2112, 2144, 2176, 2208, 2240, 2272, 2304, 2336, 2368, 2400, 2432, 2464, 2496, 2528, 2560, 2592, 2624, 2656, 2688, 2720, 2752, 2784, 2816, 2848, 2880, 2912, 2944, 2976, 3008, 3040,  /* 64 - 95*/
                    3072, 3104, 3136, 3168, 3200, 3232, 3264, 3296, 3328, 3360, 3392, 3424, 3456, 3488, 3520, 3552, 3584, 3616, 3648, 3680, 3712, 3744, 3776, 3808, 3840, 3872, 3904, 3936, 3968, 4000, 4032, 4064,  /* 96 - 127*/
                    4095, /*128 - 128*/
                },
            },
            /* nMdeLevelLut 7 */
            {
                {
                    0, 32, 64, 96, 128, 160, 192, 224, 256, 288, 320, 352, 384, 416, 448, 480, 512, 544, 576, 608, 640, 672, 704, 736, 768, 800, 832, 864, 896, 928, 960, 992,  /* 0 - 31*/
                    1024, 1056, 1088, 1120, 1152, 1184, 1216, 1248, 1280, 1312, 1344, 1376, 1408, 1440, 1472, 1504, 1536, 1568, 1600, 1632, 1664, 1696, 1728, 1760, 1792, 1824, 1856, 1888, 1920, 1952, 1984, 2016,  /* 32 - 63*/
                    2048, 2080, 2112, 2144, 2176, 2208, 2240, 2272, 2304, 2336, 2368, 2400, 2432, 2464, 2496, 2528, 2560, 2592, 2624, 2656, 2688, 2720, 2752, 2784, 2816, 2848, 2880, 2912, 2944, 2976, 3008, 3040,  /* 64 - 95*/
                    3072, 3104, 3136, 3168, 3200, 3232, 3264, 3296, 3328, 3360, 3392, 3424, 3456, 3488, 3520, 3552, 3584, 3616, 3648, 3680, 3712, 3744, 3776, 3808, 3840, 3872, 3904, 3936, 3968, 4000, 4032, 4064,  /* 96 - 127*/
                    4095, /*128 - 128*/
                },
                {
                    0, 32, 64, 96, 128, 160, 192, 224, 256, 288, 320, 352, 384, 416, 448, 480, 512, 544, 576, 608, 640, 672, 704, 736, 768, 800, 832, 864, 896, 928, 960, 992,  /* 0 - 31*/
                    1024, 1056, 1088, 1120, 1152, 1184, 1216, 1248, 1280, 1312, 1344, 1376, 1408, 1440, 1472, 1504, 1536, 1568, 1600, 1632, 1664, 1696, 1728, 1760, 1792, 1824, 1856, 1888, 1920, 1952, 1984, 2016,  /* 32 - 63*/
                    2048, 2080, 2112, 2144, 2176, 2208, 2240, 2272, 2304, 2336, 2368, 2400, 2432, 2464, 2496, 2528, 2560, 2592, 2624, 2656, 2688, 2720, 2752, 2784, 2816, 2848, 2880, 2912, 2944, 2976, 3008, 3040,  /* 64 - 95*/
                    3072, 3104, 3136, 3168, 3200, 3232, 3264, 3296, 3328, 3360, 3392, 3424, 3456, 3488, 3520, 3552, 3584, 3616, 3648, 3680, 3712, 3744, 3776, 3808, 3840, 3872, 3904, 3936, 3968, 4000, 4032, 4064,  /* 96 - 127*/
                    4095, /*128 - 128*/
                },
                {
                    0, 32, 64, 96, 128, 160, 192, 224, 256, 288, 320, 352, 384, 416, 448, 480, 512, 544, 576, 608, 640, 672, 704, 736, 768, 800, 832, 864, 896, 928, 960, 992,  /* 0 - 31*/
                    1024, 1056, 1088, 1120, 1152, 1184, 1216, 1248, 1280, 1312, 1344, 1376, 1408, 1440, 1472, 1504, 1536, 1568, 1600, 1632, 1664, 1696, 1728, 1760, 1792, 1824, 1856, 1888, 1920, 1952, 1984, 2016,  /* 32 - 63*/
                    2048, 2080, 2112, 2144, 2176, 2208, 2240, 2272, 2304, 2336, 2368, 2400, 2432, 2464, 2496, 2528, 2560, 2592, 2624, 2656, 2688, 2720, 2752, 2784, 2816, 2848, 2880, 2912, 2944, 2976, 3008, 3040,  /* 64 - 95*/
                    3072, 3104, 3136, 3168, 3200, 3232, 3264, 3296, 3328, 3360, 3392, 3424, 3456, 3488, 3520, 3552, 3584, 3616, 3648, 3680, 3712, 3744, 3776, 3808, 3840, 3872, 3904, 3936, 3968, 4000, 4032, 4064,  /* 96 - 127*/
                    4095, /*128 - 128*/
                },
                {
                    0, 32, 64, 96, 128, 160, 192, 224, 256, 288, 320, 352, 384, 416, 448, 480, 512, 544, 576, 608, 640, 672, 704, 736, 768, 800, 832, 864, 896, 928, 960, 992,  /* 0 - 31*/
                    1024, 1056, 1088, 1120, 1152, 1184, 1216, 1248, 1280, 1312, 1344, 1376, 1408, 1440, 1472, 1504, 1536, 1568, 1600, 1632, 1664, 1696, 1728, 1760, 1792, 1824, 1856, 1888, 1920, 1952, 1984, 2016,  /* 32 - 63*/
                    2048, 2080, 2112, 2144, 2176, 2208, 2240, 2272, 2304, 2336, 2368, 2400, 2432, 2464, 2496, 2528, 2560, 2592, 2624, 2656, 2688, 2720, 2752, 2784, 2816, 2848, 2880, 2912, 2944, 2976, 3008, 3040,  /* 64 - 95*/
                    3072, 3104, 3136, 3168, 3200, 3232, 3264, 3296, 3328, 3360, 3392, 3424, 3456, 3488, 3520, 3552, 3584, 3616, 3648, 3680, 3712, 3744, 3776, 3808, 3840, 3872, 3904, 3936, 3968, 4000, 4032, 4064,  /* 96 - 127*/
                    4095, /*128 - 128*/
                },
            },
            /* nMdeLevelLut 8 */
            {
                {
                    0, 32, 64, 96, 128, 160, 192, 224, 256, 288, 320, 352, 384, 416, 448, 480, 512, 544, 576, 608, 640, 672, 704, 736, 768, 800, 832, 864, 896, 928, 960, 992,  /* 0 - 31*/
                    1024, 1056, 1088, 1120, 1152, 1184, 1216, 1248, 1280, 1312, 1344, 1376, 1408, 1440, 1472, 1504, 1536, 1568, 1600, 1632, 1664, 1696, 1728, 1760, 1792, 1824, 1856, 1888, 1920, 1952, 1984, 2016,  /* 32 - 63*/
                    2048, 2080, 2112, 2144, 2176, 2208, 2240, 2272, 2304, 2336, 2368, 2400, 2432, 2464, 2496, 2528, 2560, 2592, 2624, 2656, 2688, 2720, 2752, 2784, 2816, 2848, 2880, 2912, 2944, 2976, 3008, 3040,  /* 64 - 95*/
                    3072, 3104, 3136, 3168, 3200, 3232, 3264, 3296, 3328, 3360, 3392, 3424, 3456, 3488, 3520, 3552, 3584, 3616, 3648, 3680, 3712, 3744, 3776, 3808, 3840, 3872, 3904, 3936, 3968, 4000, 4032, 4064,  /* 96 - 127*/
                    4095, /*128 - 128*/
                },
                {
                    0, 32, 64, 96, 128, 160, 192, 224, 256, 288, 320, 352, 384, 416, 448, 480, 512, 544, 576, 608, 640, 672, 704, 736, 768, 800, 832, 864, 896, 928, 960, 992,  /* 0 - 31*/
                    1024, 1056, 1088, 1120, 1152, 1184, 1216, 1248, 1280, 1312, 1344, 1376, 1408, 1440, 1472, 1504, 1536, 1568, 1600, 1632, 1664, 1696, 1728, 1760, 1792, 1824, 1856, 1888, 1920, 1952, 1984, 2016,  /* 32 - 63*/
                    2048, 2080, 2112, 2144, 2176, 2208, 2240, 2272, 2304, 2336, 2368, 2400, 2432, 2464, 2496, 2528, 2560, 2592, 2624, 2656, 2688, 2720, 2752, 2784, 2816, 2848, 2880, 2912, 2944, 2976, 3008, 3040,  /* 64 - 95*/
                    3072, 3104, 3136, 3168, 3200, 3232, 3264, 3296, 3328, 3360, 3392, 3424, 3456, 3488, 3520, 3552, 3584, 3616, 3648, 3680, 3712, 3744, 3776, 3808, 3840, 3872, 3904, 3936, 3968, 4000, 4032, 4064,  /* 96 - 127*/
                    4095, /*128 - 128*/
                },
                {
                    0, 32, 64, 96, 128, 160, 192, 224, 256, 288, 320, 352, 384, 416, 448, 480, 512, 544, 576, 608, 640, 672, 704, 736, 768, 800, 832, 864, 896, 928, 960, 992,  /* 0 - 31*/
                    1024, 1056, 1088, 1120, 1152, 1184, 1216, 1248, 1280, 1312, 1344, 1376, 1408, 1440, 1472, 1504, 1536, 1568, 1600, 1632, 1664, 1696, 1728, 1760, 1792, 1824, 1856, 1888, 1920, 1952, 1984, 2016,  /* 32 - 63*/
                    2048, 2080, 2112, 2144, 2176, 2208, 2240, 2272, 2304, 2336, 2368, 2400, 2432, 2464, 2496, 2528, 2560, 2592, 2624, 2656, 2688, 2720, 2752, 2784, 2816, 2848, 2880, 2912, 2944, 2976, 3008, 3040,  /* 64 - 95*/
                    3072, 3104, 3136, 3168, 3200, 3232, 3264, 3296, 3328, 3360, 3392, 3424, 3456, 3488, 3520, 3552, 3584, 3616, 3648, 3680, 3712, 3744, 3776, 3808, 3840, 3872, 3904, 3936, 3968, 4000, 4032, 4064,  /* 96 - 127*/
                    4095, /*128 - 128*/
                },
                {
                    0, 32, 64, 96, 128, 160, 192, 224, 256, 288, 320, 352, 384, 416, 448, 480, 512, 544, 576, 608, 640, 672, 704, 736, 768, 800, 832, 864, 896, 928, 960, 992,  /* 0 - 31*/
                    1024, 1056, 1088, 1120, 1152, 1184, 1216, 1248, 1280, 1312, 1344, 1376, 1408, 1440, 1472, 1504, 1536, 1568, 1600, 1632, 1664, 1696, 1728, 1760, 1792, 1824, 1856, 1888, 1920, 1952, 1984, 2016,  /* 32 - 63*/
                    2048, 2080, 2112, 2144, 2176, 2208, 2240, 2272, 2304, 2336, 2368, 2400, 2432, 2464, 2496, 2528, 2560, 2592, 2624, 2656, 2688, 2720, 2752, 2784, 2816, 2848, 2880, 2912, 2944, 2976, 3008, 3040,  /* 64 - 95*/
                    3072, 3104, 3136, 3168, 3200, 3232, 3264, 3296, 3328, 3360, 3392, 3424, 3456, 3488, 3520, 3552, 3584, 3616, 3648, 3680, 3712, 3744, 3776, 3808, 3840, 3872, 3904, 3936, 3968, 4000, 4032, 4064,  /* 96 - 127*/
                    4095, /*128 - 128*/
                },
            },
            /* nMdeLevelLut 9 */
            {
                {
                    0, 32, 64, 96, 128, 160, 192, 224, 256, 288, 320, 352, 384, 416, 448, 480, 512, 544, 576, 608, 640, 672, 704, 736, 768, 800, 832, 864, 896, 928, 960, 992,  /* 0 - 31*/
                    1024, 1056, 1088, 1120, 1152, 1184, 1216, 1248, 1280, 1312, 1344, 1376, 1408, 1440, 1472, 1504, 1536, 1568, 1600, 1632, 1664, 1696, 1728, 1760, 1792, 1824, 1856, 1888, 1920, 1952, 1984, 2016,  /* 32 - 63*/
                    2048, 2080, 2112, 2144, 2176, 2208, 2240, 2272, 2304, 2336, 2368, 2400, 2432, 2464, 2496, 2528, 2560, 2592, 2624, 2656, 2688, 2720, 2752, 2784, 2816, 2848, 2880, 2912, 2944, 2976, 3008, 3040,  /* 64 - 95*/
                    3072, 3104, 3136, 3168, 3200, 3232, 3264, 3296, 3328, 3360, 3392, 3424, 3456, 3488, 3520, 3552, 3584, 3616, 3648, 3680, 3712, 3744, 3776, 3808, 3840, 3872, 3904, 3936, 3968, 4000, 4032, 4064,  /* 96 - 127*/
                    4095, /*128 - 128*/
                },
                {
                    0, 32, 64, 96, 128, 160, 192, 224, 256, 288, 320, 352, 384, 416, 448, 480, 512, 544, 576, 608, 640, 672, 704, 736, 768, 800, 832, 864, 896, 928, 960, 992,  /* 0 - 31*/
                    1024, 1056, 1088, 1120, 1152, 1184, 1216, 1248, 1280, 1312, 1344, 1376, 1408, 1440, 1472, 1504, 1536, 1568, 1600, 1632, 1664, 1696, 1728, 1760, 1792, 1824, 1856, 1888, 1920, 1952, 1984, 2016,  /* 32 - 63*/
                    2048, 2080, 2112, 2144, 2176, 2208, 2240, 2272, 2304, 2336, 2368, 2400, 2432, 2464, 2496, 2528, 2560, 2592, 2624, 2656, 2688, 2720, 2752, 2784, 2816, 2848, 2880, 2912, 2944, 2976, 3008, 3040,  /* 64 - 95*/
                    3072, 3104, 3136, 3168, 3200, 3232, 3264, 3296, 3328, 3360, 3392, 3424, 3456, 3488, 3520, 3552, 3584, 3616, 3648, 3680, 3712, 3744, 3776, 3808, 3840, 3872, 3904, 3936, 3968, 4000, 4032, 4064,  /* 96 - 127*/
                    4095, /*128 - 128*/
                },
                {
                    0, 32, 64, 96, 128, 160, 192, 224, 256, 288, 320, 352, 384, 416, 448, 480, 512, 544, 576, 608, 640, 672, 704, 736, 768, 800, 832, 864, 896, 928, 960, 992,  /* 0 - 31*/
                    1024, 1056, 1088, 1120, 1152, 1184, 1216, 1248, 1280, 1312, 1344, 1376, 1408, 1440, 1472, 1504, 1536, 1568, 1600, 1632, 1664, 1696, 1728, 1760, 1792, 1824, 1856, 1888, 1920, 1952, 1984, 2016,  /* 32 - 63*/
                    2048, 2080, 2112, 2144, 2176, 2208, 2240, 2272, 2304, 2336, 2368, 2400, 2432, 2464, 2496, 2528, 2560, 2592, 2624, 2656, 2688, 2720, 2752, 2784, 2816, 2848, 2880, 2912, 2944, 2976, 3008, 3040,  /* 64 - 95*/
                    3072, 3104, 3136, 3168, 3200, 3232, 3264, 3296, 3328, 3360, 3392, 3424, 3456, 3488, 3520, 3552, 3584, 3616, 3648, 3680, 3712, 3744, 3776, 3808, 3840, 3872, 3904, 3936, 3968, 4000, 4032, 4064,  /* 96 - 127*/
                    4095, /*128 - 128*/
                },
                {
                    0, 32, 64, 96, 128, 160, 192, 224, 256, 288, 320, 352, 384, 416, 448, 480, 512, 544, 576, 608, 640, 672, 704, 736, 768, 800, 832, 864, 896, 928, 960, 992,  /* 0 - 31*/
                    1024, 1056, 1088, 1120, 1152, 1184, 1216, 1248, 1280, 1312, 1344, 1376, 1408, 1440, 1472, 1504, 1536, 1568, 1600, 1632, 1664, 1696, 1728, 1760, 1792, 1824, 1856, 1888, 1920, 1952, 1984, 2016,  /* 32 - 63*/
                    2048, 2080, 2112, 2144, 2176, 2208, 2240, 2272, 2304, 2336, 2368, 2400, 2432, 2464, 2496, 2528, 2560, 2592, 2624, 2656, 2688, 2720, 2752, 2784, 2816, 2848, 2880, 2912, 2944, 2976, 3008, 3040,  /* 64 - 95*/
                    3072, 3104, 3136, 3168, 3200, 3232, 3264, 3296, 3328, 3360, 3392, 3424, 3456, 3488, 3520, 3552, 3584, 3616, 3648, 3680, 3712, 3744, 3776, 3808, 3840, 3872, 3904, 3936, 3968, 4000, 4032, 4064,  /* 96 - 127*/
                    4095, /*128 - 128*/
                },
            },
            /* nMdeLevelLut 10 */
            {
                {
                    0, 32, 64, 96, 128, 160, 192, 224, 256, 288, 320, 352, 384, 416, 448, 480, 512, 544, 576, 608, 640, 672, 704, 736, 768, 800, 832, 864, 896, 928, 960, 992,  /* 0 - 31*/
                    1024, 1056, 1088, 1120, 1152, 1184, 1216, 1248, 1280, 1312, 1344, 1376, 1408, 1440, 1472, 1504, 1536, 1568, 1600, 1632, 1664, 1696, 1728, 1760, 1792, 1824, 1856, 1888, 1920, 1952, 1984, 2016,  /* 32 - 63*/
                    2048, 2080, 2112, 2144, 2176, 2208, 2240, 2272, 2304, 2336, 2368, 2400, 2432, 2464, 2496, 2528, 2560, 2592, 2624, 2656, 2688, 2720, 2752, 2784, 2816, 2848, 2880, 2912, 2944, 2976, 3008, 3040,  /* 64 - 95*/
                    3072, 3104, 3136, 3168, 3200, 3232, 3264, 3296, 3328, 3360, 3392, 3424, 3456, 3488, 3520, 3552, 3584, 3616, 3648, 3680, 3712, 3744, 3776, 3808, 3840, 3872, 3904, 3936, 3968, 4000, 4032, 4064,  /* 96 - 127*/
                    4095, /*128 - 128*/
                },
                {
                    0, 32, 64, 96, 128, 160, 192, 224, 256, 288, 320, 352, 384, 416, 448, 480, 512, 544, 576, 608, 640, 672, 704, 736, 768, 800, 832, 864, 896, 928, 960, 992,  /* 0 - 31*/
                    1024, 1056, 1088, 1120, 1152, 1184, 1216, 1248, 1280, 1312, 1344, 1376, 1408, 1440, 1472, 1504, 1536, 1568, 1600, 1632, 1664, 1696, 1728, 1760, 1792, 1824, 1856, 1888, 1920, 1952, 1984, 2016,  /* 32 - 63*/
                    2048, 2080, 2112, 2144, 2176, 2208, 2240, 2272, 2304, 2336, 2368, 2400, 2432, 2464, 2496, 2528, 2560, 2592, 2624, 2656, 2688, 2720, 2752, 2784, 2816, 2848, 2880, 2912, 2944, 2976, 3008, 3040,  /* 64 - 95*/
                    3072, 3104, 3136, 3168, 3200, 3232, 3264, 3296, 3328, 3360, 3392, 3424, 3456, 3488, 3520, 3552, 3584, 3616, 3648, 3680, 3712, 3744, 3776, 3808, 3840, 3872, 3904, 3936, 3968, 4000, 4032, 4064,  /* 96 - 127*/
                    4095, /*128 - 128*/
                },
                {
                    0, 32, 64, 96, 128, 160, 192, 224, 256, 288, 320, 352, 384, 416, 448, 480, 512, 544, 576, 608, 640, 672, 704, 736, 768, 800, 832, 864, 896, 928, 960, 992,  /* 0 - 31*/
                    1024, 1056, 1088, 1120, 1152, 1184, 1216, 1248, 1280, 1312, 1344, 1376, 1408, 1440, 1472, 1504, 1536, 1568, 1600, 1632, 1664, 1696, 1728, 1760, 1792, 1824, 1856, 1888, 1920, 1952, 1984, 2016,  /* 32 - 63*/
                    2048, 2080, 2112, 2144, 2176, 2208, 2240, 2272, 2304, 2336, 2368, 2400, 2432, 2464, 2496, 2528, 2560, 2592, 2624, 2656, 2688, 2720, 2752, 2784, 2816, 2848, 2880, 2912, 2944, 2976, 3008, 3040,  /* 64 - 95*/
                    3072, 3104, 3136, 3168, 3200, 3232, 3264, 3296, 3328, 3360, 3392, 3424, 3456, 3488, 3520, 3552, 3584, 3616, 3648, 3680, 3712, 3744, 3776, 3808, 3840, 3872, 3904, 3936, 3968, 4000, 4032, 4064,  /* 96 - 127*/
                    4095, /*128 - 128*/
                },
                {
                    0, 32, 64, 96, 128, 160, 192, 224, 256, 288, 320, 352, 384, 416, 448, 480, 512, 544, 576, 608, 640, 672, 704, 736, 768, 800, 832, 864, 896, 928, 960, 992,  /* 0 - 31*/
                    1024, 1056, 1088, 1120, 1152, 1184, 1216, 1248, 1280, 1312, 1344, 1376, 1408, 1440, 1472, 1504, 1536, 1568, 1600, 1632, 1664, 1696, 1728, 1760, 1792, 1824, 1856, 1888, 1920, 1952, 1984, 2016,  /* 32 - 63*/
                    2048, 2080, 2112, 2144, 2176, 2208, 2240, 2272, 2304, 2336, 2368, 2400, 2432, 2464, 2496, 2528, 2560, 2592, 2624, 2656, 2688, 2720, 2752, 2784, 2816, 2848, 2880, 2912, 2944, 2976, 3008, 3040,  /* 64 - 95*/
                    3072, 3104, 3136, 3168, 3200, 3232, 3264, 3296, 3328, 3360, 3392, 3424, 3456, 3488, 3520, 3552, 3584, 3616, 3648, 3680, 3712, 3744, 3776, 3808, 3840, 3872, 3904, 3936, 3968, 4000, 4032, 4064,  /* 96 - 127*/
                    4095, /*128 - 128*/
                },
            },
            /* nMdeLevelLut 11 */
            {
                {
                    0, 32, 64, 96, 128, 160, 192, 224, 256, 288, 320, 352, 384, 416, 448, 480, 512, 544, 576, 608, 640, 672, 704, 736, 768, 800, 832, 864, 896, 928, 960, 992,  /* 0 - 31*/
                    1024, 1056, 1088, 1120, 1152, 1184, 1216, 1248, 1280, 1312, 1344, 1376, 1408, 1440, 1472, 1504, 1536, 1568, 1600, 1632, 1664, 1696, 1728, 1760, 1792, 1824, 1856, 1888, 1920, 1952, 1984, 2016,  /* 32 - 63*/
                    2048, 2080, 2112, 2144, 2176, 2208, 2240, 2272, 2304, 2336, 2368, 2400, 2432, 2464, 2496, 2528, 2560, 2592, 2624, 2656, 2688, 2720, 2752, 2784, 2816, 2848, 2880, 2912, 2944, 2976, 3008, 3040,  /* 64 - 95*/
                    3072, 3104, 3136, 3168, 3200, 3232, 3264, 3296, 3328, 3360, 3392, 3424, 3456, 3488, 3520, 3552, 3584, 3616, 3648, 3680, 3712, 3744, 3776, 3808, 3840, 3872, 3904, 3936, 3968, 4000, 4032, 4064,  /* 96 - 127*/
                    4095, /*128 - 128*/
                },
                {
                    0, 32, 64, 96, 128, 160, 192, 224, 256, 288, 320, 352, 384, 416, 448, 480, 512, 544, 576, 608, 640, 672, 704, 736, 768, 800, 832, 864, 896, 928, 960, 992,  /* 0 - 31*/
                    1024, 1056, 1088, 1120, 1152, 1184, 1216, 1248, 1280, 1312, 1344, 1376, 1408, 1440, 1472, 1504, 1536, 1568, 1600, 1632, 1664, 1696, 1728, 1760, 1792, 1824, 1856, 1888, 1920, 1952, 1984, 2016,  /* 32 - 63*/
                    2048, 2080, 2112, 2144, 2176, 2208, 2240, 2272, 2304, 2336, 2368, 2400, 2432, 2464, 2496, 2528, 2560, 2592, 2624, 2656, 2688, 2720, 2752, 2784, 2816, 2848, 2880, 2912, 2944, 2976, 3008, 3040,  /* 64 - 95*/
                    3072, 3104, 3136, 3168, 3200, 3232, 3264, 3296, 3328, 3360, 3392, 3424, 3456, 3488, 3520, 3552, 3584, 3616, 3648, 3680, 3712, 3744, 3776, 3808, 3840, 3872, 3904, 3936, 3968, 4000, 4032, 4064,  /* 96 - 127*/
                    4095, /*128 - 128*/
                },
                {
                    0, 32, 64, 96, 128, 160, 192, 224, 256, 288, 320, 352, 384, 416, 448, 480, 512, 544, 576, 608, 640, 672, 704, 736, 768, 800, 832, 864, 896, 928, 960, 992,  /* 0 - 31*/
                    1024, 1056, 1088, 1120, 1152, 1184, 1216, 1248, 1280, 1312, 1344, 1376, 1408, 1440, 1472, 1504, 1536, 1568, 1600, 1632, 1664, 1696, 1728, 1760, 1792, 1824, 1856, 1888, 1920, 1952, 1984, 2016,  /* 32 - 63*/
                    2048, 2080, 2112, 2144, 2176, 2208, 2240, 2272, 2304, 2336, 2368, 2400, 2432, 2464, 2496, 2528, 2560, 2592, 2624, 2656, 2688, 2720, 2752, 2784, 2816, 2848, 2880, 2912, 2944, 2976, 3008, 3040,  /* 64 - 95*/
                    3072, 3104, 3136, 3168, 3200, 3232, 3264, 3296, 3328, 3360, 3392, 3424, 3456, 3488, 3520, 3552, 3584, 3616, 3648, 3680, 3712, 3744, 3776, 3808, 3840, 3872, 3904, 3936, 3968, 4000, 4032, 4064,  /* 96 - 127*/
                    4095, /*128 - 128*/
                },
                {
                    0, 32, 64, 96, 128, 160, 192, 224, 256, 288, 320, 352, 384, 416, 448, 480, 512, 544, 576, 608, 640, 672, 704, 736, 768, 800, 832, 864, 896, 928, 960, 992,  /* 0 - 31*/
                    1024, 1056, 1088, 1120, 1152, 1184, 1216, 1248, 1280, 1312, 1344, 1376, 1408, 1440, 1472, 1504, 1536, 1568, 1600, 1632, 1664, 1696, 1728, 1760, 1792, 1824, 1856, 1888, 1920, 1952, 1984, 2016,  /* 32 - 63*/
                    2048, 2080, 2112, 2144, 2176, 2208, 2240, 2272, 2304, 2336, 2368, 2400, 2432, 2464, 2496, 2528, 2560, 2592, 2624, 2656, 2688, 2720, 2752, 2784, 2816, 2848, 2880, 2912, 2944, 2976, 3008, 3040,  /* 64 - 95*/
                    3072, 3104, 3136, 3168, 3200, 3232, 3264, 3296, 3328, 3360, 3392, 3424, 3456, 3488, 3520, 3552, 3584, 3616, 3648, 3680, 3712, 3744, 3776, 3808, 3840, 3872, 3904, 3936, 3968, 4000, 4032, 4064,  /* 96 - 127*/
                    4095, /*128 - 128*/
                },
            },
        },
        /* nMdeGain[16][4] */
        {
            {256, 256, 256, 256, /*0 - 3*/},
            {256, 256, 256, 256, /*0 - 3*/},
            {256, 256, 256, 256, /*0 - 3*/},
            {256, 256, 256, 256, /*0 - 3*/},
            {256, 256, 256, 256, /*0 - 3*/},
            {256, 256, 256, 256, /*0 - 3*/},
            {312, 256, 256, 256, /*0 - 3*/},
            {312, 256, 256, 256, /*0 - 3*/},
            {312, 256, 256, 256, /*0 - 3*/},
            {312, 256, 256, 256, /*0 - 3*/},
            {412, 256, 256, 256, /*0 - 3*/},
            {412, 256, 256, 256, /*0 - 3*/},
        },
    },
};

static AX_ISP_IQ_CCMP_PARAM_T ccmp_param_sdr = {
    /* nChromaCompEn */
    1,
    /* nAutoMode */
    1,
    /* nRefMode */
    1,
    /* tManualParam */
    {
        /* nChromaCompY[29] */
        {512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, /*0 - 28*/},
        /* nChromaCompSat[23] */
        {200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, /*0 - 22*/},
    },
    /* tAutoParam */
    {
        /* nParamGrpNum */
        9,
        /* nRefVal[16] */
        {1024, 16384, 32768, 65536, 131072, 252144, 450335, 900670, 1801340, 1024, 1024, 1024, 1024, 1024, 1024, 1024, /*0 - 15*/},
        /* nChromaCompY[16][29] */
        {
            {512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, /*0 - 28*/},
            {512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, /*0 - 28*/},
            {512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, /*0 - 28*/},
            {512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, /*0 - 28*/},
            {512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, /*0 - 28*/},
            {300, 320, 330, 340, 350, 360, 370, 380, 390, 421, 444, 469, 490, 506, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, /*0 - 28*/},
            {300, 320, 330, 340, 350, 360, 370, 380, 390, 421, 444, 469, 490, 506, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, /*0 - 28*/},
            {300, 320, 330, 340, 350, 360, 370, 380, 390, 421, 444, 469, 490, 506, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, /*0 - 28*/},
            {300, 320, 330, 340, 350, 360, 370, 380, 390, 421, 444, 469, 490, 506, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, /*0 - 28*/},
            {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 28*/},
            {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 28*/},
            {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 28*/},
            {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 28*/},
            {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 28*/},
            {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 28*/},
            {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 28*/},
        },
        /* nChromaCompSat[16][23] */
        {
            {512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, /*0 - 22*/},
            {512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, /*0 - 22*/},
            {512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, /*0 - 22*/},
            {450, 450, 450, 450, 450, 450, 450, 450, 450, 450, 450, 450, 450, 450, 450, 450, 450, 450, 450, 450, 450, 450, 450, /*0 - 22*/},
            {400, 400, 400, 400, 400, 400, 400, 400, 400, 400, 400, 400, 400, 400, 400, 400, 400, 400, 400, 400, 400, 400, 400, /*0 - 22*/},
            {350, 350, 350, 350, 350, 350, 350, 350, 350, 350, 350, 350, 350, 350, 350, 350, 350, 350, 350, 350, 350, 350, 350, /*0 - 22*/},
            {300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, /*0 - 22*/},
            {300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, /*0 - 22*/},
            {300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, /*0 - 22*/},
            {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 22*/},
            {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 22*/},
            {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 22*/},
            {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 22*/},
            {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 22*/},
            {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 22*/},
            {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 22*/},
        },
    },
};

static AX_ISP_IQ_SCM_PARAM_T scm_param_sdr = {
    /* nScmEn */
    1,
    /* nAutoMode */
    0,
    /* nRefMode */
    0,
    /* nScmIoFlag */
    0,
    /* tManualParam */
    {
        /* nScmColor[2] */
        {0, 0, /*0 - 1*/},
        /* nScmCenterY */
        0,
        /* nScmCenterUv[2] */
        {0, 0, /*0 - 1*/},
        /* nScmRadius[3] */
        {0, 0, 0, /*0 - 2*/},
        /* nScmSmooth[3] */
        {0, 0, 0, /*0 - 2*/},
    },
    /* tAutoParam */
    {
        /* nParamGrpNum */
        16,
        /* nRefVal[16] */
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 15*/},
        /* nScmColor[16][2] */
        {
            {0, 0, /*0 - 1*/},
            {0, 0, /*0 - 1*/},
            {0, 0, /*0 - 1*/},
            {0, 0, /*0 - 1*/},
            {0, 0, /*0 - 1*/},
            {0, 0, /*0 - 1*/},
            {0, 0, /*0 - 1*/},
            {0, 0, /*0 - 1*/},
            {0, 0, /*0 - 1*/},
            {0, 0, /*0 - 1*/},
            {0, 0, /*0 - 1*/},
            {0, 0, /*0 - 1*/},
            {0, 0, /*0 - 1*/},
            {0, 0, /*0 - 1*/},
            {0, 0, /*0 - 1*/},
            {0, 0, /*0 - 1*/},
        },
        /* nScmCenterY[16] */
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*0 - 15*/},
        /* nScmCenterUv[16][2] */
        {
            {0, 0, /*0 - 1*/},
            {0, 0, /*0 - 1*/},
            {0, 0, /*0 - 1*/},
            {0, 0, /*0 - 1*/},
            {0, 0, /*0 - 1*/},
            {0, 0, /*0 - 1*/},
            {0, 0, /*0 - 1*/},
            {0, 0, /*0 - 1*/},
            {0, 0, /*0 - 1*/},
            {0, 0, /*0 - 1*/},
            {0, 0, /*0 - 1*/},
            {0, 0, /*0 - 1*/},
            {0, 0, /*0 - 1*/},
            {0, 0, /*0 - 1*/},
            {0, 0, /*0 - 1*/},
            {0, 0, /*0 - 1*/},
        },
        /* nScmRadius[16][3] */
        {
            {0, 0, 0, /*0 - 2*/},
            {0, 0, 0, /*0 - 2*/},
            {0, 0, 0, /*0 - 2*/},
            {0, 0, 0, /*0 - 2*/},
            {0, 0, 0, /*0 - 2*/},
            {0, 0, 0, /*0 - 2*/},
            {0, 0, 0, /*0 - 2*/},
            {0, 0, 0, /*0 - 2*/},
            {0, 0, 0, /*0 - 2*/},
            {0, 0, 0, /*0 - 2*/},
            {0, 0, 0, /*0 - 2*/},
            {0, 0, 0, /*0 - 2*/},
            {0, 0, 0, /*0 - 2*/},
            {0, 0, 0, /*0 - 2*/},
            {0, 0, 0, /*0 - 2*/},
            {0, 0, 0, /*0 - 2*/},
        },
        /* nScmSmooth[16][3] */
        {
            {0, 0, 0, /*0 - 2*/},
            {0, 0, 0, /*0 - 2*/},
            {0, 0, 0, /*0 - 2*/},
            {0, 0, 0, /*0 - 2*/},
            {0, 0, 0, /*0 - 2*/},
            {0, 0, 0, /*0 - 2*/},
            {0, 0, 0, /*0 - 2*/},
            {0, 0, 0, /*0 - 2*/},
            {0, 0, 0, /*0 - 2*/},
            {0, 0, 0, /*0 - 2*/},
            {0, 0, 0, /*0 - 2*/},
            {0, 0, 0, /*0 - 2*/},
            {0, 0, 0, /*0 - 2*/},
            {0, 0, 0, /*0 - 2*/},
            {0, 0, 0, /*0 - 2*/},
            {0, 0, 0, /*0 - 2*/},
        },
    },
};

static AX_ISP_IQ_YCPROC_PARAM_T ycproc_param_sdr = {
    /* nYCprocEn */
    1,
    /* nBrightness */
    256,
    /* nContrast */
    256,
    /* nSaturation */
    4096,
    /* nHue */
    0,
};

static AX_ISP_IQ_YCRT_PARAM_T ycrt_param_sdr = {
    /* nYcrtEn */
    1,
    /* nSignalRangeMode */
    0,
    /* nYrtInputRange[2] */
    {256, 4095, /*0 - 1*/},
    /* nYrtOutputRange[2] */
    {256, 3840, /*0 - 1*/},
    /* nCrtInputRange[2] */
    {256, 3840, /*0 - 1*/},
    /* nCrtOutputRange[2] */
    {256, 3840, /*0 - 1*/},
    /* nClipLevelY[2] */
    {0, 4095, /*0 - 1*/},
    /* nClipLevelUV[2] */
    {-2048, 2047, /*0 - 1*/},
};

static AX_ISP_IQ_DEPWL_PARAM_T depwl_param_sdr = {
    /* nUserModeEn */
    0,
    /* tDePwlLut */
    {
        /* nLut[513] */
        {
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  /* 0 - 31*/
            0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  /* 32 - 63*/
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  /* 64 - 95*/
            2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4,  /* 96 - 127*/
            4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 5, 6, 6, 6, 6, 6, 6, 6, 6, 6, 7, 7, 7, 7, 7, 7, 7, 7,  /* 128 - 159*/
            7, 8, 8, 8, 8, 9, 9, 9, 9, 10, 10, 10, 10, 11, 11, 11, 11, 11, 12, 12, 12, 12, 13, 13, 13, 13, 14, 14, 14, 14, 15, 15,  /* 160 - 191*/
            15, 15, 16, 16, 17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 22, 23, 23, 24, 24, 25, 25, 26, 26, 27, 27, 28, 28, 29, 29, 30,  /* 192 - 223*/
            30, 31, 32, 33, 34, 35, 36, 37, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 52, 53, 54, 55, 56, 57, 58, 59,  /* 224 - 255*/
            60, 62, 64, 66, 67, 69, 71, 73, 75, 77, 79, 81, 82, 84, 86, 88, 90, 92, 94, 96, 97, 99, 101, 103, 105, 107, 109, 111, 112, 114, 116, 118,  /* 256 - 287*/
            120, 124, 127, 131, 135, 139, 142, 146, 150, 154, 157, 161, 165, 169, 172, 176, 180, 184, 187, 191, 195, 199, 202, 206, 210, 214, 217, 221, 225, 229, 232, 236,  /* 288 - 319*/
            240, 247, 255, 262, 270, 277, 285, 292, 300, 307, 315, 322, 330, 337, 345, 352, 360, 367, 375, 382, 390, 397, 405, 412, 420, 427, 435, 442, 450, 457, 465, 472,  /* 320 - 351*/
            480, 495, 510, 525, 540, 555, 570, 585, 600, 615, 630, 645, 660, 675, 690, 705, 720, 735, 750, 765, 780, 795, 810, 825, 840, 855, 870, 885, 900, 915, 930, 945,  /* 352 - 383*/
            960, 990, 1020, 1050, 1080, 1110, 1140, 1170, 1200, 1230, 1260, 1290, 1320, 1350, 1380, 1410, 1440, 1470, 1500, 1530, 1560, 1590, 1620, 1650, 1680, 1710, 1740, 1770, 1800, 1830, 1860, 1890,  /* 384 - 415*/
            1920, 1980, 2040, 2100, 2160, 2220, 2280, 2340, 2400, 2460, 2520, 2580, 2640, 2700, 2760, 2820, 2880, 2940, 3000, 3060, 3120, 3180, 3240, 3300, 3360, 3420, 3480, 3540, 3600, 3660, 3720, 3780,  /* 416 - 447*/
            3840, 3960, 4080, 4200, 4320, 4440, 4560, 4680, 4800, 4920, 5040, 5160, 5280, 5400, 5520, 5640, 5760, 5880, 6000, 6120, 6240, 6360, 6480, 6600, 6720, 6840, 6960, 7080, 7200, 7320, 7440, 7560,  /* 448 - 479*/
            7680, 7919, 8159, 8399, 8639, 8879, 9119, 9359, 9599, 9839, 10079, 10319, 10559, 10799, 11039, 11279, 11519, 11759, 11999, 12239, 12479, 12719, 12959, 13199, 13439, 13679, 13919, 14159, 14399, 14639, 14879, 15119,  /* 480 - 511*/
            15359, /*512 - 512*/
        },
    },
};

static AX_ISP_IQ_LDC_PARAM_T ldc_param_sdr = {
    /* nLdcEnable */
    0,
    /* nType */
    0,
    /* tLdcV1Param */
    {
        /* bAspect */
        0,
        /* nXRatio */
        0,
        /* nYRatio */
        0,
        /* nXYRatio */
        0,
        /* nCenterXOffset */
        0,
        /* nCenterYOffset */
        0,
        /* nDistortionRatio */
        0,
        /* nSpreadCoef */
        0,
    },
    /* tLdcV2Param */
    {
        /* nMatrix[3][3] */
        {
            {0, 0, 0, /*0 - 2*/},
            {0, 0, 0, /*0 - 2*/},
            {0, 0, 100, /*0 - 2*/},
        },
        /* nDistortionCoeff[8] */
        {0, 0, 0, 0, 0, 0, 0, 0, /*0 - 7*/},
    },
};

static AX_ISP_IQ_DIS_PARAM_T dis_param_sdr = {
    /* bDisEnable */
    0,
    /* nDisType */
    0,
    /* tDisV1Param */
    {
        /* nDelayFrameNum */
        0,
        /* nHistoryFrameNum */
        2,
        /* nCropRatio */
        204,
        /* nAlgoType */
        1,
        /* nFramePosWeights[16] */
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /* 0 - 16 */},
        /* nLargeMovementThreshold */
        350,
        /* nDecayRate */
        192,
        /* nFeatPtsMinNum */
        8,
        /* nKFPredictedError */
        256,
        /* nKFProcessNoise */
        12,
        /* nKFObservationNoise */
        128,
        /* nDebugMode */
        0,
    },
    /* tDisV2Param */
    {
        /* nCropRatio */
        204,
        /* nTimeLag */
        0,
        /* nIntrinsics[3][3] */
        {
            {256, 0, 0, /*0 - 2*/},
            {0, 256, 0, /*0 - 2*/},
            {0, 0, 256, /*0 - 2*/},
        },
        /* nExtrinsics[3][3] */
        {
            {256, 0, 0, /*0 - 2*/},
            {0, 256, 0, /*0 - 2*/},
            {0, 0, 256, /*0 - 2*/},
        },
        /* nStaticTheta */
        20,
        /* nZeroBiasTheta */
        1,
        /* nErrorToWarning */
        983,
        /* nAnyToSafe */
        655,
        /* nSafeToWarning */
        655,
        /* nWarningScale */
        282,
    },
};

static AX_ISP_IQ_ME_PARAM_T me_param_sdr = {
    /* nRefMode */
    1,
    /* nAutoMode */
    0,
    /* tGlbParam */
    {
        /* nClipMotionMode */
        1,
        /* nFpdNmsTopk */
        2,
        /* nLayerNum */
        3,
    },
    /* tManualParam */
    {
        /* nZeroMotion */
        0,
        /* nFpdAlpha */
        2,
        /* nFpdResponseThres */
        50,
        /* nFpdScaleRange */
        0,
        /* nFpdNmsRadius */
        4,
    },
    /* tAutoParam */
    {
        /* nParamGrpNum */
        0,
        /* nRefVal[16] */
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  /*0 - 15*/},
        /* nZeroMotion[16] */
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  /*0 - 15*/},
        /* nFpdAlpha[16] */
        {2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,   /*0 - 15*/},
        /* nFpdResponseThres[16] */
        {50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50,   /*0 - 15*/},
        /* nFpdScaleRange[16] */
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   /*0 - 15*/},
        /* nFpdNmsRadius[16] */
        {4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,   /*0 - 15*/},
    },
};

static AX_ISP_IQ_DS_PARAM_T darkshading_param_sdr = {
    /* nAutoMode */
    0,
    /* nMeshOffset */
    0,
    /* nMeshRows */
    24,
    /* nMeshCols */
    24,
    /* tManualParam */
    {
        {
            {
                {0, },
                {0, },
            },
        },
    },
    /* tAutoParam */
    {
        /* tHcgAutoTable */
        {
            /* nGainGrpNum */
            0,
            /* nExposeTimeGrpNum */
            0,
            /* nGain[16] */
            {0, },
            /* nExposeTime[16] */
            {0, },
            /* tMeshTab[16][16] */
            {
                {
                    {
                        {
                            {0, },
                            {0, },
                        },
                    },
                },
            },
        },
        /* tLcgAutoTable */
        {
            /* nGainGrpNum */
            0,
            /* nExposeTimeGrpNum */
            0,
            /* nGain[16] */
            {0, },
            /* nExposeTime[16] */
            {0, },
            /* tMeshTab[16][16] */
            {
                {
                    {
                        {
                            {0, },
                            {0, },
                        },
                    },
                },
            },
        },
    },
};


#endif //__IMX678_SDR_H__
