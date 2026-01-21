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
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>
#include "axcl.h"
#include "axcl_sys.h"
#include "axcl_ivps.h"
#include "axcl_skel.h"
#include "axcl_npu.h"
#include "axcl_rt.h"

#include "skel_log.h"

#ifndef AX_MAX
#define AX_MAX(a, b) (((a) > (b)) ? (a) : (b))
#endif

#ifndef AX_MIN
#define AX_MIN(a, b) (((a) < (b)) ? (a) : (b))
#endif

#define DETECT_SKEL_POINT_COUNT 256

typedef struct _AI_Detection_Box_t {
    AX_F32 fX, fY, fW, fH;
} AI_Detection_Box_t;

typedef struct _AI_Detection_Point_t {
    AX_F32 fX, fY;
} AI_Detection_Point_t;

typedef struct _AI_Detection_SkelResult_t {
    AX_U8 nPointNum;
    AI_Detection_Point_t tPoint[DETECT_SKEL_POINT_COUNT];
    AI_Detection_Box_t tBox;
} AI_Detection_SkelResult_t;

static int g_exit = 0;
static void signal_handler(int s) {
    printf("\n====================== caught signal: %d ======================\n", s);
    g_exit = 1;
}

static AX_U64 get_tick_count(AX_VOID) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (ts.tv_sec * 1000 + ts.tv_nsec / 1000000);
}

static AX_U64 get_tick_count_us(AX_VOID) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (ts.tv_sec * 1000000 + ts.tv_nsec / 1000);
}

static AX_VOID ShowUsage(AX_VOID) {
    printf("usage: ./%s <options> ...\n", AXCL_SAMPLE_NAME);
    printf("options:\n");
    printf("-i, \tInput File(yuv)\n");
    printf("-r, \tInput File Resolution(wxh)(yuv: should be input)\n");
    printf("-m, \tModels deployment path(path name)\n");
    printf("-t, \tRepeat times((unsigned int), default=1)\n");
    printf("-I, \tInterval repeat time((unsigned int)ms, default=0)\n");
    printf("-c, \tConfidence((float: 0-1), default=0)\n");
    printf("-H, \tHuman track size limit((unsigned int), default=0)\n");
    printf("-V, \tVehicle track size limit((unsigned int), default=0)\n");
    printf("-C, \tCylcle track size limit((unsigned int), default=0)\n");
    printf("-d, \tDevice index from 0 to connected device num - 1\n");
    printf("-e, \tSkel detect type((unsigned int), default=1)\n"
                "\t\t0: detect only\n"
                "\t\t1: detect + track\n");
    printf("-N, \tSkel NPU type((unsigned int), default=0(VNPU Disable)\n"
                "\t\t0: VNPU Disable\n"
                "\t\t1: STD-VNPU Default\n"
                "\t\t2: STD-VNPU1\n"
                "\t\t3: STD-VNPU2\n"
                "\t\t4: STD-VNPU3\n"
                "\t\t5: BIG-LITTLE VNPU Default\n"
                "\t\t6: BIG-LITTLE VNPU1\n"
                "\t\t7: BIG-LITTLE VNPU2\n");
    printf("-p, \tSkel PPL((unsigned int), default=1)\n"
                "\t\t1: AXCL_SKEL_PPL_HVCP\n"
                "\t\t2: AXCL_SKEL_PPL_FACE\n");
    printf("-v, \tLog level((unsigned int), default=5)\n"
                "\t\t0: LOG_EMERGENCY_LEVEL\n"
                "\t\t1: LOG_ALERT_LEVEL\n"
                "\t\t2: LOG_CRITICAL_LEVEL\n"
                "\t\t3: LOG_ERROR_LEVEL\n"
                "\t\t4: LOG_WARN_LEVEL\n"
                "\t\t5: LOG_NOTICE_LEVEL\n"
                "\t\t6: LOG_INFO_LEVEL\n"
                "\t\t7: LOG_DEBUG_LEVEL\n");
    printf("-j, \taxcl.json path\n");
    printf("-h, \tprint this message\n");
}

AX_S32 ParseConfigParam(const AXCL_SKEL_CONFIG_T *pstConfig) {
    if (pstConfig->nSize > 0 && pstConfig->pstItems) {
        for (size_t i = 0; i < pstConfig->nSize; i++) {
            if (pstConfig->pstItems[i].pstrType && pstConfig->pstItems[i].pstrValue) {
                // cmd: "body_max_target_count", value_type: AXCL_SKEL_COMMON_THRESHOLD_CONFIG_T *
                if (strcmp(pstConfig->pstItems[i].pstrType, "body_max_target_count") == 0) {
                    if (pstConfig->pstItems[i].nValueSize == sizeof(AXCL_SKEL_COMMON_THRESHOLD_CONFIG_T)) {
                        AXCL_SKEL_COMMON_THRESHOLD_CONFIG_T *pstConf =
                            (AXCL_SKEL_COMMON_THRESHOLD_CONFIG_T *)pstConfig->pstItems[i].pstrValue;
                        ALOGI("SKEL get %s: %d", pstConfig->pstItems[i].pstrType, (AX_U8)pstConf->fValue);
                    } else {
                        ALOGE("SKEL %s size(%d) no match", pstConfig->pstItems[i].pstrType, pstConfig->pstItems[i].nValueSize);
                    }
                }
                // cmd: "vehicle_max_target_count", value_type: AXCL_SKEL_COMMON_THRESHOLD_CONFIG_T *
                else if (strcmp(pstConfig->pstItems[i].pstrType, "vehicle_max_target_count") == 0) {
                    if (pstConfig->pstItems[i].nValueSize == sizeof(AXCL_SKEL_COMMON_THRESHOLD_CONFIG_T)) {
                        AXCL_SKEL_COMMON_THRESHOLD_CONFIG_T *pstConf =
                            (AXCL_SKEL_COMMON_THRESHOLD_CONFIG_T *)pstConfig->pstItems[i].pstrValue;
                        ALOGI("SKEL get %s: %d", pstConfig->pstItems[i].pstrType, (AX_U8)pstConf->fValue);
                    } else {
                        ALOGE("SKEL %s size(%d) no match", pstConfig->pstItems[i].pstrType, pstConfig->pstItems[i].nValueSize);
                    }
                }
                // cmd: "cycle_max_target_count", value_type: AXCL_SKEL_COMMON_THRESHOLD_CONFIG_T *
                else if (strcmp(pstConfig->pstItems[i].pstrType, "cycle_max_target_count") == 0) {
                    if (pstConfig->pstItems[i].nValueSize == sizeof(AXCL_SKEL_COMMON_THRESHOLD_CONFIG_T)) {
                        AXCL_SKEL_COMMON_THRESHOLD_CONFIG_T *pstConf =
                            (AXCL_SKEL_COMMON_THRESHOLD_CONFIG_T *)pstConfig->pstItems[i].pstrValue;
                        ALOGI("SKEL get %s: %d", pstConfig->pstItems[i].pstrType, (AX_U8)pstConf->fValue);
                    } else {
                        ALOGE("SKEL %s size(%d) no match", pstConfig->pstItems[i].pstrType, pstConfig->pstItems[i].nValueSize);
                    }
                }
                // cmd: "body_confidence", value_type: AXCL_SKEL_COMMON_THRESHOLD_CONFIG_T *
                else if (strcmp(pstConfig->pstItems[i].pstrType, "body_confidence") == 0) {
                    if (pstConfig->pstItems[i].nValueSize == sizeof(AXCL_SKEL_COMMON_THRESHOLD_CONFIG_T)) {
                        AXCL_SKEL_COMMON_THRESHOLD_CONFIG_T *pstConf =
                            (AXCL_SKEL_COMMON_THRESHOLD_CONFIG_T *)pstConfig->pstItems[i].pstrValue;
                        ALOGI("SKEL get %s: %f", pstConfig->pstItems[i].pstrType, pstConf->fValue);
                    } else {
                        ALOGE("SKEL %s size(%d) no match", pstConfig->pstItems[i].pstrType, pstConfig->pstItems[i].nValueSize);
                    }
                }
                // cmd: "face_confidence", value_type: AXCL_SKEL_COMMON_THRESHOLD_CONFIG_T *
                else if (strcmp(pstConfig->pstItems[i].pstrType, "face_confidence") == 0) {
                    if (pstConfig->pstItems[i].nValueSize == sizeof(AXCL_SKEL_COMMON_THRESHOLD_CONFIG_T)) {
                        AXCL_SKEL_COMMON_THRESHOLD_CONFIG_T *pstConf =
                            (AXCL_SKEL_COMMON_THRESHOLD_CONFIG_T *)pstConfig->pstItems[i].pstrValue;
                        ALOGI("SKEL get %s: %f", pstConfig->pstItems[i].pstrType, pstConf->fValue);
                    } else {
                        ALOGE("SKEL %s size(%d) no match", pstConfig->pstItems[i].pstrType, pstConfig->pstItems[i].nValueSize);
                    }
                }
                // cmd: "vehicle_confidence", value_type: AXCL_SKEL_COMMON_THRESHOLD_CONFIG_T *
                else if (strcmp(pstConfig->pstItems[i].pstrType, "vehicle_confidence") == 0) {
                    if (pstConfig->pstItems[i].nValueSize == sizeof(AXCL_SKEL_COMMON_THRESHOLD_CONFIG_T)) {
                        AXCL_SKEL_COMMON_THRESHOLD_CONFIG_T *pstConf =
                            (AXCL_SKEL_COMMON_THRESHOLD_CONFIG_T *)pstConfig->pstItems[i].pstrValue;
                        ALOGI("SKEL get %s: %f", pstConfig->pstItems[i].pstrType, pstConf->fValue);
                    } else {
                        ALOGE("SKEL %s size(%d) no match", pstConfig->pstItems[i].pstrType, pstConfig->pstItems[i].nValueSize);
                    }
                }
                // cmd: "cycle_confidence", value_type: AXCL_SKEL_COMMON_THRESHOLD_CONFIG_T *
                else if (strcmp(pstConfig->pstItems[i].pstrType, "cycle_confidence") == 0) {
                    if (pstConfig->pstItems[i].nValueSize == sizeof(AXCL_SKEL_COMMON_THRESHOLD_CONFIG_T)) {
                        AXCL_SKEL_COMMON_THRESHOLD_CONFIG_T *pstConf =
                            (AXCL_SKEL_COMMON_THRESHOLD_CONFIG_T *)pstConfig->pstItems[i].pstrValue;
                        ALOGI("SKEL get %s: %f", pstConfig->pstItems[i].pstrType, pstConf->fValue);
                    } else {
                        ALOGE("SKEL %s size(%d) no match", pstConfig->pstItems[i].pstrType, pstConfig->pstItems[i].nValueSize);
                    }
                }
                // cmd: "plate_confidence", value_type: AXCL_SKEL_COMMON_THRESHOLD_CONFIG_T *
                else if (strcmp(pstConfig->pstItems[i].pstrType, "plate_confidence") == 0) {
                    if (pstConfig->pstItems[i].nValueSize == sizeof(AXCL_SKEL_COMMON_THRESHOLD_CONFIG_T)) {
                        AXCL_SKEL_COMMON_THRESHOLD_CONFIG_T *pstConf =
                            (AXCL_SKEL_COMMON_THRESHOLD_CONFIG_T *)pstConfig->pstItems[i].pstrValue;
                        ALOGI("SKEL get %s: %f", pstConfig->pstItems[i].pstrType, pstConf->fValue);
                    } else {
                        ALOGE("SKEL %s size(%d) no match", pstConfig->pstItems[i].pstrType, pstConfig->pstItems[i].nValueSize);
                    }
                }
                // cmd: "crop_encoder_qpLevel", value_type: AXCL_SKEL_COMMON_THRESHOLD_CONFIG_T *
                else if (strcmp(pstConfig->pstItems[i].pstrType, "crop_encoder_qpLevel") == 0) {
                    if (pstConfig->pstItems[i].nValueSize == sizeof(AXCL_SKEL_COMMON_THRESHOLD_CONFIG_T)) {
                        AXCL_SKEL_COMMON_THRESHOLD_CONFIG_T *pstConf =
                            (AXCL_SKEL_COMMON_THRESHOLD_CONFIG_T *)pstConfig->pstItems[i].pstrValue;
                        ALOGI("SKEL get %s: %f", pstConfig->pstItems[i].pstrType, pstConf->fValue);
                    } else {
                        ALOGE("SKEL %s size(%d) no match", pstConfig->pstItems[i].pstrType, pstConfig->pstItems[i].nValueSize);
                    }
                }
                // cmd: "body_min_size",  value_type: AXCL_SKEL_OBJECT_SIZE_FILTER_CONFIG_T *
                else if (strcmp(pstConfig->pstItems[i].pstrType, "body_min_size") == 0) {
                    if (pstConfig->pstItems[i].nValueSize == sizeof(AXCL_SKEL_OBJECT_SIZE_FILTER_CONFIG_T)) {
                        AXCL_SKEL_OBJECT_SIZE_FILTER_CONFIG_T *pstConf =
                            (AXCL_SKEL_OBJECT_SIZE_FILTER_CONFIG_T *)pstConfig->pstItems[i].pstrValue;
                        ALOGI("SKEL get %s %dx%d", pstConfig->pstItems[i].pstrType, pstConf->nWidth, pstConf->nHeight);
                    } else {
                        ALOGE("SKEL %s size(%d) no match", pstConfig->pstItems[i].pstrType, pstConfig->pstItems[i].nValueSize);
                    }
                }
                // cmd: "face_min_size",  value_type: AXCL_SKEL_OBJECT_SIZE_FILTER_CONFIG_T *
                else if (strcmp(pstConfig->pstItems[i].pstrType, "face_min_size") == 0) {
                    if (pstConfig->pstItems[i].nValueSize == sizeof(AXCL_SKEL_OBJECT_SIZE_FILTER_CONFIG_T)) {
                        AXCL_SKEL_OBJECT_SIZE_FILTER_CONFIG_T *pstConf =
                            (AXCL_SKEL_OBJECT_SIZE_FILTER_CONFIG_T *)pstConfig->pstItems[i].pstrValue;
                        ALOGI("SKEL get %s %dx%d", pstConfig->pstItems[i].pstrType, pstConf->nWidth, pstConf->nHeight);
                    } else {
                        ALOGE("SKEL %s size(%d) no match", pstConfig->pstItems[i].pstrType, pstConfig->pstItems[i].nValueSize);
                    }
                }
                // cmd: "vehicle_min_size",  value_type: AXCL_SKEL_OBJECT_SIZE_FILTER_CONFIG_T *
                else if (strcmp(pstConfig->pstItems[i].pstrType, "vehicle_min_size") == 0) {
                    if (pstConfig->pstItems[i].nValueSize == sizeof(AXCL_SKEL_OBJECT_SIZE_FILTER_CONFIG_T)) {
                        AXCL_SKEL_OBJECT_SIZE_FILTER_CONFIG_T *pstConf =
                            (AXCL_SKEL_OBJECT_SIZE_FILTER_CONFIG_T *)pstConfig->pstItems[i].pstrValue;
                        ALOGI("SKEL get %s %dx%d", pstConfig->pstItems[i].pstrType, pstConf->nWidth, pstConf->nHeight);
                    } else {
                        ALOGE("SKEL %s size(%d) no match", pstConfig->pstItems[i].pstrType, pstConfig->pstItems[i].nValueSize);
                    }
                }
                // cmd: "cycle_min_size",  value_type: AXCL_SKEL_OBJECT_SIZE_FILTER_CONFIG_T *
                else if (strcmp(pstConfig->pstItems[i].pstrType, "cycle_min_size") == 0) {
                    if (pstConfig->pstItems[i].nValueSize == sizeof(AXCL_SKEL_OBJECT_SIZE_FILTER_CONFIG_T)) {
                        AXCL_SKEL_OBJECT_SIZE_FILTER_CONFIG_T *pstConf =
                            (AXCL_SKEL_OBJECT_SIZE_FILTER_CONFIG_T *)pstConfig->pstItems[i].pstrValue;
                        ALOGI("SKEL get %s %dx%d", pstConfig->pstItems[i].pstrType, pstConf->nWidth, pstConf->nHeight);
                    } else {
                        ALOGE("SKEL %s size(%d) no match", pstConfig->pstItems[i].pstrType, pstConfig->pstItems[i].nValueSize);
                    }
                }
                // cmd: "plate_min_size",  value_type: AXCL_SKEL_OBJECT_SIZE_FILTER_CONFIG_T *
                else if (strcmp(pstConfig->pstItems[i].pstrType, "plate_min_size") == 0) {
                    if (pstConfig->pstItems[i].nValueSize == sizeof(AXCL_SKEL_OBJECT_SIZE_FILTER_CONFIG_T)) {
                        AXCL_SKEL_OBJECT_SIZE_FILTER_CONFIG_T *pstConf =
                            (AXCL_SKEL_OBJECT_SIZE_FILTER_CONFIG_T *)pstConfig->pstItems[i].pstrValue;
                        ALOGI("SKEL get %s %dx%d", pstConfig->pstItems[i].pstrType, pstConf->nWidth, pstConf->nHeight);
                    } else {
                        ALOGE("SKEL %s size(%d) no match", pstConfig->pstItems[i].pstrType, pstConfig->pstItems[i].nValueSize);
                    }
                }
                // cmd: "detect_roi",  value_type: AXCL_SKEL_ROI_CONFIG_T *
                else if (strcmp(pstConfig->pstItems[i].pstrType, "detect_roi") == 0) {
                    if (pstConfig->pstItems[i].nValueSize == sizeof(AXCL_SKEL_ROI_CONFIG_T)) {
                        AXCL_SKEL_ROI_CONFIG_T *pstConf = (AXCL_SKEL_ROI_CONFIG_T *)pstConfig->pstItems[i].pstrValue;
                        ALOGI("SKEL get %s [%d]:[%f,%f,%f,%f]", pstConfig->pstItems[i].pstrType, pstConf->bEnable, pstConf->stRect.fX,
                              pstConf->stRect.fY, pstConf->stRect.fW, pstConf->stRect.fH);
                    } else {
                        ALOGE("SKEL %s size(%d) no match", pstConfig->pstItems[i].pstrType, pstConfig->pstItems[i].nValueSize);
                    }
                }
                // cmd: "push_strategy",  value_type: AXCL_SKEL_PUSH_STRATEGY_T *
                else if (strcmp(pstConfig->pstItems[i].pstrType, "push_strategy") == 0) {
                    if (pstConfig->pstItems[i].nValueSize == sizeof(AXCL_SKEL_PUSH_STRATEGY_T)) {
                        AXCL_SKEL_PUSH_STRATEGY_T *pstConf = (AXCL_SKEL_PUSH_STRATEGY_T *)pstConfig->pstItems[i].pstrValue;
                        ALOGI("SKEL get %s [mode:%d, times:%d, count:%d, same:%d]", pstConfig->pstItems[i].pstrType, pstConf->ePushMode, pstConf->nIntervalTimes,
                                pstConf->nPushCounts, pstConf->bPushSameFrame);
                    } else {
                        ALOGE("SKEL %s size(%d) no match", pstConfig->pstItems[i].pstrType, pstConfig->pstItems[i].nValueSize);
                    }
                }
                // cmd: "crop_encoder",  value_type: AXCL_SKEL_CROP_ENCODER_THRESHOLD_CONFIG_T *
                else if (strcmp(pstConfig->pstItems[i].pstrType, "crop_encoder") == 0) {
                    if (pstConfig->pstItems[i].nValueSize == sizeof(AXCL_SKEL_CROP_ENCODER_THRESHOLD_CONFIG_T)) {
                        AXCL_SKEL_CROP_ENCODER_THRESHOLD_CONFIG_T *pstConf = (AXCL_SKEL_CROP_ENCODER_THRESHOLD_CONFIG_T *)pstConfig->pstItems[i].pstrValue;
                        ALOGI("SKEL get %s [%f, %f, %f, %f]", pstConfig->pstItems[i].pstrType,
                                pstConf->fScaleLeft, pstConf->fScaleRight,
                                pstConf->fScaleTop, pstConf->fScaleBottom);
                    } else {
                        ALOGE("SKEL %s size(%d) no match", pstConfig->pstItems[i].pstrType, pstConfig->pstItems[i].nValueSize);
                    }
                }
                // cmd: "resize_panorama_encoder_config",  value_type: AXCL_SKEL_RESIZE_CONFIG *
                else if (strcmp(pstConfig->pstItems[i].pstrType, "resize_panorama_encoder_config") == 0) {
                    if (pstConfig->pstItems[i].nValueSize == sizeof(AXCL_SKEL_RESIZE_CONFIG_T)) {
                        AXCL_SKEL_RESIZE_CONFIG_T *pstConf = (AXCL_SKEL_RESIZE_CONFIG_T *)pstConfig->pstItems[i].pstrValue;
                        ALOGI("SKEL get %s [%f, %f]", pstConfig->pstItems[i].pstrType, pstConf->fW, pstConf->fH);
                    } else {
                        ALOGE("SKEL %s size(%d) no match", pstConfig->pstItems[i].pstrType, pstConfig->pstItems[i].nValueSize);
                    }
                }
                // cmd: "push_panorama",  value_type: AXCL_SKEL_PUSH_PANORAMA_CONFIG_T *
                else if (strcmp(pstConfig->pstItems[i].pstrType, "push_panorama") == 0) {
                    if (pstConfig->pstItems[i].nValueSize == sizeof(AXCL_SKEL_PUSH_PANORAMA_CONFIG_T)) {
                        AXCL_SKEL_PUSH_PANORAMA_CONFIG_T *pstConf = (AXCL_SKEL_PUSH_PANORAMA_CONFIG_T *)pstConfig->pstItems[i].pstrValue;
                        ALOGI("SKEL get %s [Enable: %d, Quality: %d]", pstConfig->pstItems[i].pstrType,
                                pstConf->bEnable, pstConf->nQuality);
                    } else {
                        ALOGE("SKEL %s size(%d) no match", pstConfig->pstItems[i].pstrType, pstConfig->pstItems[i].nValueSize);
                    }
                }
                // cmd: "push_quality_body",  value_type: AXCL_SKEL_ATTR_FILTER_CONFIG_T *
                else if (strcmp(pstConfig->pstItems[i].pstrType, "push_quality_body") == 0) {
                    if (pstConfig->pstItems[i].nValueSize == sizeof(AXCL_SKEL_ATTR_FILTER_CONFIG_T)) {
                        AXCL_SKEL_ATTR_FILTER_CONFIG_T *pstConf = (AXCL_SKEL_ATTR_FILTER_CONFIG_T *)pstConfig->pstItems[i].pstrValue;
                        ALOGI("SKEL get %s [Q: %f]", pstConfig->pstItems[i].pstrType, pstConf->stCommonAttrFilterConfig.fQuality);
                    } else {
                        ALOGE("SKEL %s size(%d) no match", pstConfig->pstItems[i].pstrType, pstConfig->pstItems[i].nValueSize);
                    }
                }
                // cmd: "push_quality_vehicle",  value_type: AXCL_SKEL_ATTR_FILTER_CONFIG_T *
                else if (strcmp(pstConfig->pstItems[i].pstrType, "push_quality_vehicle") == 0) {
                    if (pstConfig->pstItems[i].nValueSize == sizeof(AXCL_SKEL_ATTR_FILTER_CONFIG_T)) {
                        AXCL_SKEL_ATTR_FILTER_CONFIG_T *pstConf = (AXCL_SKEL_ATTR_FILTER_CONFIG_T *)pstConfig->pstItems[i].pstrValue;
                        ALOGI("SKEL get %s [Q: %f]", pstConfig->pstItems[i].pstrType, pstConf->stCommonAttrFilterConfig.fQuality);
                    } else {
                        ALOGE("SKEL %s size(%d) no match", pstConfig->pstItems[i].pstrType, pstConfig->pstItems[i].nValueSize);
                    }
                }
                // cmd: "push_quality_cycle",  value_type: AXCL_SKEL_ATTR_FILTER_CONFIG_T *
                else if (strcmp(pstConfig->pstItems[i].pstrType, "push_quality_cycle") == 0) {
                    if (pstConfig->pstItems[i].nValueSize == sizeof(AXCL_SKEL_ATTR_FILTER_CONFIG_T)) {
                        AXCL_SKEL_ATTR_FILTER_CONFIG_T *pstConf = (AXCL_SKEL_ATTR_FILTER_CONFIG_T *)pstConfig->pstItems[i].pstrValue;
                        ALOGI("SKEL get %s [Q: %f]", pstConfig->pstItems[i].pstrType, pstConf->stCommonAttrFilterConfig.fQuality);
                    } else {
                        ALOGE("SKEL %s size(%d) no match", pstConfig->pstItems[i].pstrType, pstConfig->pstItems[i].nValueSize);
                    }
                }
                // cmd: "push_quality_face",  value_type: AXCL_SKEL_ATTR_FILTER_CONFIG_T *
                else if (strcmp(pstConfig->pstItems[i].pstrType, "push_quality_face") == 0) {
                    if (pstConfig->pstItems[i].nValueSize == sizeof(AXCL_SKEL_ATTR_FILTER_CONFIG_T)) {
                        AXCL_SKEL_ATTR_FILTER_CONFIG_T *pstConf = (AXCL_SKEL_ATTR_FILTER_CONFIG_T *)pstConfig->pstItems[i].pstrValue;
                        ALOGI("SKEL get %s [W: %d, H: %d, P: %f, Y: %f, R: %f, B: %f]", pstConfig->pstItems[i].pstrType,
                                pstConf->stFaceAttrFilterConfig.nWidth, pstConf->stFaceAttrFilterConfig.nHeight,
                                pstConf->stFaceAttrFilterConfig.stPoseblur.fPitch, pstConf->stFaceAttrFilterConfig.stPoseblur.fYaw,
                                pstConf->stFaceAttrFilterConfig.stPoseblur.fRoll, pstConf->stFaceAttrFilterConfig.stPoseblur.fBlur);
                    } else {
                        ALOGE("SKEL %s size(%d) no match", pstConfig->pstItems[i].pstrType, pstConfig->pstItems[i].nValueSize);
                    }
                }
                // cmd: "push_quality_plate",  value_type: AXCL_SKEL_ATTR_FILTER_CONFIG_T *
                else if (strcmp(pstConfig->pstItems[i].pstrType, "push_quality_plate") == 0) {
                    if (pstConfig->pstItems[i].nValueSize == sizeof(AXCL_SKEL_ATTR_FILTER_CONFIG_T)) {
                        AXCL_SKEL_ATTR_FILTER_CONFIG_T *pstConf = (AXCL_SKEL_ATTR_FILTER_CONFIG_T *)pstConfig->pstItems[i].pstrValue;
                        ALOGI("SKEL get %s [Q: %f]", pstConfig->pstItems[i].pstrType, pstConf->stCommonAttrFilterConfig.fQuality);
                    } else {
                        ALOGE("SKEL %s size(%d) no match", pstConfig->pstItems[i].pstrType, pstConfig->pstItems[i].nValueSize);
                    }
                }
                // cmd: "push_bind_enable",  value_type: AXCL_SKEL_COMMON_THRESHOLD_CONFIG_T *
                else if (strcmp(pstConfig->pstItems[i].pstrType, "push_bind_enable") == 0) {
                    if (pstConfig->pstItems[i].nValueSize == sizeof(AXCL_SKEL_COMMON_THRESHOLD_CONFIG_T)) {
                        AXCL_SKEL_COMMON_THRESHOLD_CONFIG_T *pstConf = (AXCL_SKEL_COMMON_THRESHOLD_CONFIG_T *)pstConfig->pstItems[i].pstrValue;
                        ALOGI("SKEL get %s : %f", pstConfig->pstItems[i].pstrType, pstConf->fValue);
                    } else {
                        ALOGE("SKEL %s size(%d) no match", pstConfig->pstItems[i].pstrType, pstConfig->pstItems[i].nValueSize);
                    }
                }
                // cmd: "track_disable",  value_type: AXCL_SKEL_COMMON_THRESHOLD_CONFIG_T *
                else if (strcmp(pstConfig->pstItems[i].pstrType, "track_disable") == 0) {
                    if (pstConfig->pstItems[i].nValueSize == sizeof(AXCL_SKEL_COMMON_THRESHOLD_CONFIG_T)) {
                        AXCL_SKEL_COMMON_THRESHOLD_CONFIG_T *pstConf = (AXCL_SKEL_COMMON_THRESHOLD_CONFIG_T *)pstConfig->pstItems[i].pstrValue;
                        ALOGI("SKEL get %s : %f", pstConfig->pstItems[i].pstrType, pstConf->fValue);
                    } else {
                        ALOGE("SKEL %s size(%d) no match", pstConfig->pstItems[i].pstrType, pstConfig->pstItems[i].nValueSize);
                    }
                }
                else {
                    ALOGE("SKEL cmd: %s not support", pstConfig->pstItems[i].pstrType);
                }
            }
        }
    }

    return 0;
}

int LoadFileToMem(const AX_CHAR *pFile, AX_VOID **ppYUVData, AX_U32 *pLen)
{
    /* Reading input file */
    FILE *f_in = fopen(pFile, "rb");
    if (f_in == NULL) {
        ALOGE("Unable to open input file\n");
        return -1;
    }

    /* file i/o pointer to full */
    fseek(f_in, 0L, SEEK_END);
    AX_U32 nFileSize = ftell(f_in);
    rewind(f_in);

    if (ppYUVData) {
        AX_VOID *pData = NULL;
        pData = (AX_VOID *)malloc(nFileSize);

        if (!pData) {
            ALOGE("malloc failed");
            fclose(f_in);
            return -1;
        }

        nFileSize = fread((AX_U8 *)pData, 1, nFileSize, f_in);

        AX_VOID *pAddr = NULL;
        axclrtMalloc(&pAddr, nFileSize, AXCL_MEM_MALLOC_NORMAL_ONLY);

        if (!pAddr) {
            free(pData);
            ALOGE("axclrtMalloc failed");
            fclose(f_in);
            return -1;
        }

        axclrtMemcpy(pAddr, pData, nFileSize, AXCL_MEMCPY_HOST_TO_DEVICE);

        *ppYUVData = pAddr;

        free(pData);
    }

    if (pLen) {
        *pLen = nFileSize;
    }

    fclose(f_in);

    return 0;
}

AX_S32 main(AX_S32 argc, AX_CHAR *argv[]) {
    AX_S32 ret = 0;
    AX_S32 c;
    AX_S32 isExit = 0;
    const AX_CHAR *InputFile = NULL;
    const AX_CHAR *InputResolution = NULL;
    const AX_CHAR *ModelsPath = NULL;
    const AX_CHAR *JsonPath = "./axcl.json";
    AX_S32 nRepeatTimes = 1;
    AX_S32 nPPL = AXCL_SKEL_PPL_HVCP;
    AX_S32 nDetectType = 1;
    AX_S32 nNpuType = 0;
    AX_S32 nInterval = 0;
    AX_U32 nStride = 1920;
    AX_U32 nWidth = 1920;
    AX_U32 nHeight = 1080;
    AX_VOID *pYUVData = NULL;
    AXCL_SKEL_HANDLE pHandle = NULL;
    AXCL_SKEL_FRAME_T stFrame = {0};
    AXCL_SKEL_RESULT_T *pstResult = NULL;
    AX_U32 nFileSize = 0;
    AX_U64 nStartTime = 0;
    AX_U64 nInitElasped = 0;
    AX_U64 nCreateElasped = 0;
    AX_U64 nProcessElasped = 0;
    AX_U64 nResultElasped = 0;
    AX_U64 nResultElaspedMin = (AX_U64)-1;
    AX_U64 nResultElaspedMax = 0;
    AX_U64 nResultElaspedTotal = 0;
    AX_F32 fConfidence = 0.0;
    AX_U32 nHumantracksize = 0;
    AX_U32 nVehicletracksize = 0;
    AX_U32 nCycletracksize = 0;
    AX_S32 nDeviceId = 0;

#if defined(AXCL_BUILD_VERSION)
    printf("SKEL sample: %s build: %s %s\n", AXCL_BUILD_VERSION, __DATE__, __TIME__);
#endif

    signal(SIGINT, signal_handler);

    while ((c = getopt(argc, argv, "i:r:I:m:t:d:e:u:p:v:c:N:H:V:C:j:h::")) != -1) {
        isExit = 0;
        switch (c) {
            case 'i':
                InputFile = (const AX_CHAR *)optarg;
                break;
            case 'r':
                InputResolution = (const AX_CHAR *)optarg;
                break;
            case 'I':
                nInterval = atoi(optarg);
                break;
            case 'm':
                ModelsPath = (const AX_CHAR *)optarg;
                break;
            case 't':
                nRepeatTimes = atoi(optarg);
                break;
            case 'd':
                nDeviceId = atoi(optarg);
                break;
            case 'e':
                nDetectType = atoi(optarg);
                break;
            case 'N':
                nNpuType = atoi(optarg);
                break;
            case 'p':
                nPPL = atoi(optarg);
                break;
            case 'c':
                fConfidence = atof(optarg);
                break;
            case 'H':
                nHumantracksize = atoi(optarg);
                break;
            case 'V':
                nVehicletracksize = atoi(optarg);
                break;
            case 'C':
                nCycletracksize = atoi(optarg);
                break;
            case 'v':
                log_level = atoi(optarg);
                break;
            case 'j':
                JsonPath = (const AX_CHAR *)optarg;
                break;
            case 'h':
                isExit = 1;
                break;
            default:
                isExit = 1;
                break;
        }
    }

    if (isExit || !InputFile || !InputResolution || (nPPL < AXCL_SKEL_PPL_HVCP || nPPL >= (AXCL_SKEL_PPL_MAX))
        || (log_level < 0 || log_level >= SKEL_LOG_MAX)
        || (fConfidence < 0 || fConfidence > 1)
        || (nDetectType < 0 || nDetectType > 1)
        || (nNpuType > 7)) {
        ShowUsage();
        exit(0);
    }

    if (nRepeatTimes <= 0) {
        nRepeatTimes = 1;
    }

    if (nInterval < 0) {
        nInterval = 0;
    }
    nInterval = nInterval * 1000;

    if (access(InputFile, 0) != 0) {
        ALOGE("%s not exist", InputFile);
        exit(0);
    }

    if (InputResolution) {
        AX_CHAR *temp_p = strstr(InputResolution, "x");

        if (!temp_p || strlen(temp_p) <= 1) {
            ShowUsage();
            exit(0);
        }

        nWidth = atoi(InputResolution);
        nStride = nWidth;
        nHeight = atoi(temp_p + 1);
    }

    /* axcl initialize */
    if (JsonPath) {
        ALOGI("json: %s", JsonPath);
    }
    ret = axclInit(JsonPath);
    if (AXCL_SUCC != ret) {
        ALOGE("axcl init fail, ret = 0x%x", ret);
        return ret;
    }

    axclrtDeviceList deviceList;
    ret = axclrtGetDeviceList(&deviceList);
    if (AXCL_SUCC != ret || 0 == deviceList.num) {
        ALOGE("no device is connected");
        axclFinalize();
        return ret;
    }

    if (nDeviceId < 0 || nDeviceId >= (AX_S32)deviceList.num) {
        ALOGE("device index %d is out of connected device num %d", nDeviceId, deviceList.num);
        axclFinalize();
        return 1;
    }

    ALOGI("device index: %d, bus number: %d", nDeviceId, deviceList.devices[nDeviceId]);
    nDeviceId = deviceList.devices[nDeviceId];

    ret = axclrtSetDevice(nDeviceId);
    if (AXCL_SUCC != ret) {
        ALOGE("active device, ret = 0x%x", ret);
        goto EXIT0;
    }

    /* init sys module */
    ALOGI("init sys");
    ret = AXCL_SYS_Init();
    if (0 != ret) {
        ALOGE("init sys, ret = 0x%x", ret);
        goto EXIT0;
    }

    /* init ivps module */
    ALOGI("init ivps");
    ret = AXCL_IVPS_Init();
    if (0 != ret) {
        ALOGE("init ivps, ret = 0x%x", ret);
        goto EXIT0;
    }

    /* init engine module */
    AX_ENGINE_NPU_ATTR_T npu_attr;
    memset(&npu_attr, 0, sizeof(npu_attr));
    if (nNpuType == 0) {
        npu_attr.eHardMode = AX_ENGINE_VIRTUAL_NPU_DISABLE;
    }
    else if (nNpuType >= 1 && nNpuType <= 4) {
        npu_attr.eHardMode = AX_ENGINE_VIRTUAL_NPU_STD;
    }
    else if (nNpuType >= 5 && nNpuType <= 7) {
        npu_attr.eHardMode = AX_ENGINE_VIRTUAL_NPU_BIG_LITTLE;
    }

    ALOGI("init engine");
    ret = AXCL_ENGINE_Init(&npu_attr);
    if (0 != ret) {
        ALOGE("init sys, ret = 0x%x", ret);
        goto EXIT0;
    }

    {
        ret = LoadFileToMem(InputFile, &pYUVData, &nFileSize);

        if (ret != 0
            || pYUVData == NULL) {
            ALOGE("Load input file fail");
            goto EXIT1;
        }

        if (nFileSize != nWidth * nHeight * 3 / 2) {
            ALOGE("%s file(size: %d) is not %dx%d", InputFile, nFileSize, nWidth, nHeight);
            goto EXIT1;
        }
    }

    if (nWidth%2 == 1
        || nHeight%2 == 1) {
        ALOGE("wxh(%dx%d) should be even", nWidth, nHeight);
        goto EXIT1;
    }

    nStartTime = get_tick_count();

    AXCL_SKEL_INIT_PARAM_T stInitParam = {0};

    stInitParam.nDeviceId = nDeviceId;

    if (ModelsPath) {
        stInitParam.pStrModelDeploymentPath = ModelsPath;
    }
    else {
        // VNPU DISABLE
        if (nNpuType == 0) {
            stInitParam.pStrModelDeploymentPath = "/opt/etc/axcl/skelModels/1024x576/full";
        }
        else {
            stInitParam.pStrModelDeploymentPath = "/opt/etc/axcl/skelModels/512x288/part";
        }
    }

    ALOGI("init skel");
    ret = AXCL_SKEL_Init(&stInitParam);

    nInitElasped = get_tick_count() - nStartTime;

    if (0 != ret) {
        ALOGE("SKEL init fail, ret = 0x%x", ret);

        goto EXIT1;
    }

    // get version
    {
        const AXCL_SKEL_VERSION_INFO_T *pstVersion = NULL;

        ret = AXCL_SKEL_GetVersion(&pstVersion);

        if (0 != ret) {
            ALOGI("SKEL get version fail, ret = 0x%x", ret);

            if (pstVersion) {
                AXCL_SKEL_Release((AX_VOID *)pstVersion);
            }
            goto EXIT1;
        }

        ALOGI("SKEL version: %s", pstVersion->pstrVersion);

        if (pstVersion) {
            AXCL_SKEL_Release((AX_VOID *)pstVersion);
        }
    }

    // get capability
    {
        const AXCL_SKEL_CAPABILITY_T *pstCapability = NULL;

        ret = AXCL_SKEL_GetCapability(&pstCapability);

        if (0 != ret) {
            ALOGI("SKEL get capability fail, ret = 0x%x", ret);

            if (pstCapability) {
                AXCL_SKEL_Release((AX_VOID *)pstCapability);
            }
            goto EXIT1;
        }

        for (size_t i = 0; i < pstCapability->nPPLConfigSize; i++) {
            ALOGI("SKEL capability[%ld]: (ePPL: %d, PPLConfigKey: %s)", i, pstCapability->pstPPLConfig[i].ePPL,
                  pstCapability->pstPPLConfig[i].pstrPPLConfigKey);
        }

        if (pstCapability) {
            AXCL_SKEL_Release((AX_VOID *)pstCapability);
        }
    }

    AXCL_SKEL_HANDLE_PARAM_T stHandleParam = {0};

    stHandleParam.ePPL = (AXCL_SKEL_PPL_E)nPPL;
    stHandleParam.nFrameDepth = 1;
    stHandleParam.nFrameCacheDepth = 0;
    stHandleParam.nIoDepth = 0;
    stHandleParam.nWidth = nWidth;
    stHandleParam.nHeight = nHeight;

    // config settings (if need)
    AXCL_SKEL_CONFIG_T stConfig = {0};
    AXCL_SKEL_CONFIG_ITEM_T stItems[16] = {0};
    AX_U8 itemIndex = 0;
    stConfig.nSize = 0;
    stConfig.pstItems = &stItems[0];

    AXCL_SKEL_COMMON_THRESHOLD_CONFIG_T stTrackDisableThreshold = {0};

    // detect only (disable track + disable push)
    if (nDetectType == 0) {
        // track_disable
        stConfig.pstItems[itemIndex].pstrType = (AX_CHAR *)"track_disable";
        stTrackDisableThreshold.fValue = 1;
        stConfig.pstItems[itemIndex].pstrValue = (AX_VOID *)&stTrackDisableThreshold;
        stConfig.pstItems[itemIndex].nValueSize = sizeof(AXCL_SKEL_COMMON_THRESHOLD_CONFIG_T);
        itemIndex++;
    }

    if (itemIndex > 0) {
        stConfig.nSize = itemIndex;
        stHandleParam.stConfig = stConfig;
    }

    // default NPU
    if (nNpuType == 0 || nNpuType == 1 || nNpuType == 5) {
        stHandleParam.nNpuType = (AX_S32)AXCL_SKEL_NPU_DEFAULT;
    }
    // STD-NPU
    else if (nNpuType == 2) {
        stHandleParam.nNpuType = (AX_S32)AXCL_SKEL_STD_VNPU_1;
    }
    else if (nNpuType == 3) {
        stHandleParam.nNpuType = (AX_S32)AXCL_SKEL_STD_VNPU_2;
    }
    else if (nNpuType == 4) {
        stHandleParam.nNpuType = (AX_S32)AXCL_SKEL_STD_VNPU_3;
    }
    // BL-NPU
    else if (nNpuType == 6) {
        stHandleParam.nNpuType = (AX_S32)AXCL_SKEL_BL_VNPU_1;
    }
    else if (nNpuType == 7) {
        stHandleParam.nNpuType = (AX_S32)AXCL_SKEL_BL_VNPU_2;
    }

    nStartTime = get_tick_count();

    ret = AXCL_SKEL_Create(&stHandleParam, &pHandle);

    nCreateElasped = get_tick_count() - nStartTime;

    if (0 != ret) {
        ALOGE("SKEL Create Handle fail, ret = 0x%x", ret);

        goto EXIT2;
    }

    if (fConfidence == 0) {
        fConfidence = 0.5;
    }

    // set config
    {
        AXCL_SKEL_CONFIG_T stConfig = {0};
        AXCL_SKEL_CONFIG_ITEM_T stItems[64] = {0};
        AX_U8 itemIndex = 0;
        stConfig.nSize = 0;
        stConfig.pstItems = &stItems[0];

        // body_max_target_count
        stConfig.pstItems[itemIndex].pstrType = (AX_CHAR *)"body_max_target_count";
        AXCL_SKEL_COMMON_THRESHOLD_CONFIG_T stBodyMaxTargetCount = {0};
        stBodyMaxTargetCount.fValue = (AX_F32)nHumantracksize;
        stConfig.pstItems[itemIndex].pstrValue = (AX_VOID *)&stBodyMaxTargetCount;
        stConfig.pstItems[itemIndex].nValueSize = sizeof(AXCL_SKEL_COMMON_THRESHOLD_CONFIG_T);
        itemIndex++;

        // vehicle_max_target_count
        stConfig.pstItems[itemIndex].pstrType = (AX_CHAR *)"vehicle_max_target_count";
        AXCL_SKEL_COMMON_THRESHOLD_CONFIG_T stVehicleMaxTargetCount = {0};
        stVehicleMaxTargetCount.fValue = (AX_F32)nVehicletracksize;
        stConfig.pstItems[itemIndex].pstrValue = (AX_VOID *)&stVehicleMaxTargetCount;
        stConfig.pstItems[itemIndex].nValueSize = sizeof(AXCL_SKEL_COMMON_THRESHOLD_CONFIG_T);
        itemIndex++;

        // cycle_max_target_count
        stConfig.pstItems[itemIndex].pstrType = (AX_CHAR *)"cycle_max_target_count";
        AXCL_SKEL_COMMON_THRESHOLD_CONFIG_T stCycleMaxTargetCount = {0};
        stCycleMaxTargetCount.fValue = (AX_F32)nCycletracksize;
        stConfig.pstItems[itemIndex].pstrValue = (AX_VOID *)&stCycleMaxTargetCount;
        stConfig.pstItems[itemIndex].nValueSize = sizeof(AXCL_SKEL_COMMON_THRESHOLD_CONFIG_T);
        itemIndex++;

        // body_confidence
        stConfig.pstItems[itemIndex].pstrType = (AX_CHAR *)"body_confidence";
        AXCL_SKEL_COMMON_THRESHOLD_CONFIG_T stBodyConfidence = {0};
        stBodyConfidence.fValue = fConfidence;
        stConfig.pstItems[itemIndex].pstrValue = (AX_VOID *)&stBodyConfidence;
        stConfig.pstItems[itemIndex].nValueSize = sizeof(AXCL_SKEL_COMMON_THRESHOLD_CONFIG_T);
        itemIndex++;

        // face_confidence
        stConfig.pstItems[itemIndex].pstrType = (AX_CHAR *)"face_confidence";
        AXCL_SKEL_COMMON_THRESHOLD_CONFIG_T stFaceConfidence = {0};
        stFaceConfidence.fValue = fConfidence;
        stConfig.pstItems[itemIndex].pstrValue = (AX_VOID *)&stFaceConfidence;
        stConfig.pstItems[itemIndex].nValueSize = sizeof(AXCL_SKEL_COMMON_THRESHOLD_CONFIG_T);
        itemIndex++;

        // vehicle_confidence
        stConfig.pstItems[itemIndex].pstrType = (AX_CHAR *)"vehicle_confidence";
        AXCL_SKEL_COMMON_THRESHOLD_CONFIG_T stVehicleConfidence = {0};
        stVehicleConfidence.fValue = fConfidence;
        stConfig.pstItems[itemIndex].pstrValue = (AX_VOID *)&stVehicleConfidence;
        stConfig.pstItems[itemIndex].nValueSize = sizeof(AXCL_SKEL_COMMON_THRESHOLD_CONFIG_T);
        itemIndex++;

        // cycle_confidence
        stConfig.pstItems[itemIndex].pstrType = (AX_CHAR *)"cycle_confidence";
        AXCL_SKEL_COMMON_THRESHOLD_CONFIG_T stCycleConfidence = {0};
        stCycleConfidence.fValue = fConfidence;
        stConfig.pstItems[itemIndex].pstrValue = (AX_VOID *)&stCycleConfidence;
        stConfig.pstItems[itemIndex].nValueSize = sizeof(AXCL_SKEL_COMMON_THRESHOLD_CONFIG_T);
        itemIndex++;

        // plate_confidence
        stConfig.pstItems[itemIndex].pstrType = (AX_CHAR *)"plate_confidence";
        AXCL_SKEL_COMMON_THRESHOLD_CONFIG_T stPlateConfidence = {0};
        stPlateConfidence.fValue = fConfidence;
        stConfig.pstItems[itemIndex].pstrValue = (AX_VOID *)&stPlateConfidence;
        stConfig.pstItems[itemIndex].nValueSize = sizeof(AXCL_SKEL_COMMON_THRESHOLD_CONFIG_T);
        itemIndex++;

        // crop_encoder_qpLevel
        stConfig.pstItems[itemIndex].pstrType = (AX_CHAR *)"crop_encoder_qpLevel";
        AXCL_SKEL_COMMON_THRESHOLD_CONFIG_T stCropEncoderQpLevelThreshold = {0};
        stCropEncoderQpLevelThreshold.fValue = 90;
        stConfig.pstItems[itemIndex].pstrValue = (AX_VOID *)&stCropEncoderQpLevelThreshold;
        stConfig.pstItems[itemIndex].nValueSize = sizeof(AXCL_SKEL_COMMON_THRESHOLD_CONFIG_T);
        itemIndex++;

        // body_min_size
        stConfig.pstItems[itemIndex].pstrType = (AX_CHAR *)"body_min_size";
        AXCL_SKEL_OBJECT_SIZE_FILTER_CONFIG_T stBodyMinSize = {0};
        stBodyMinSize.nWidth = 0;
        stBodyMinSize.nHeight = 0;
        stConfig.pstItems[itemIndex].pstrValue = (AX_VOID *)&stBodyMinSize;
        stConfig.pstItems[itemIndex].nValueSize = sizeof(AXCL_SKEL_OBJECT_SIZE_FILTER_CONFIG_T);
        itemIndex++;

        // face_min_size
        stConfig.pstItems[itemIndex].pstrType = (AX_CHAR *)"face_min_size";
        AXCL_SKEL_OBJECT_SIZE_FILTER_CONFIG_T stFaceMinSize = {0};
        stFaceMinSize.nWidth = 0;
        stFaceMinSize.nHeight = 0;
        stConfig.pstItems[itemIndex].pstrValue = (AX_VOID *)&stFaceMinSize;
        stConfig.pstItems[itemIndex].nValueSize = sizeof(AXCL_SKEL_OBJECT_SIZE_FILTER_CONFIG_T);
        itemIndex++;

        // vehicle_min_size
        stConfig.pstItems[itemIndex].pstrType = (AX_CHAR *)"vehicle_min_size";
        AXCL_SKEL_OBJECT_SIZE_FILTER_CONFIG_T stVehicleMinSize = {0};
        stVehicleMinSize.nWidth = 0;
        stVehicleMinSize.nHeight = 0;
        stConfig.pstItems[itemIndex].pstrValue = (AX_VOID *)&stVehicleMinSize;
        stConfig.pstItems[itemIndex].nValueSize = sizeof(AXCL_SKEL_OBJECT_SIZE_FILTER_CONFIG_T);
        itemIndex++;

        // cycle_min_size
        stConfig.pstItems[itemIndex].pstrType = (AX_CHAR *)"cycle_min_size";
        AXCL_SKEL_OBJECT_SIZE_FILTER_CONFIG_T stCycleMinSize = {0};
        stCycleMinSize.nWidth = 0;
        stCycleMinSize.nHeight = 0;
        stConfig.pstItems[itemIndex].pstrValue = (AX_VOID *)&stCycleMinSize;
        stConfig.pstItems[itemIndex].nValueSize = sizeof(AXCL_SKEL_OBJECT_SIZE_FILTER_CONFIG_T);
        itemIndex++;

        // plate_min_size
        stConfig.pstItems[itemIndex].pstrType = (AX_CHAR *)"plate_min_size";
        AXCL_SKEL_OBJECT_SIZE_FILTER_CONFIG_T stPlateMinSize = {0};
        stPlateMinSize.nWidth = 0;
        stPlateMinSize.nHeight = 0;
        stConfig.pstItems[itemIndex].pstrValue = (AX_VOID *)&stPlateMinSize;
        stConfig.pstItems[itemIndex].nValueSize = sizeof(AXCL_SKEL_OBJECT_SIZE_FILTER_CONFIG_T);
        itemIndex++;

        // detect_roi
        stConfig.pstItems[itemIndex].pstrType = (AX_CHAR *)"detect_roi";
        AXCL_SKEL_ROI_CONFIG_T stDetectRoi = {0};
        stDetectRoi.bEnable = AX_FALSE;
        stDetectRoi.stRect.fX = 0;
        stDetectRoi.stRect.fY = 0;
        stDetectRoi.stRect.fW = 0;
        stDetectRoi.stRect.fH = 0;
        stConfig.pstItems[itemIndex].pstrValue = (AX_VOID *)&stDetectRoi;
        stConfig.pstItems[itemIndex].nValueSize = sizeof(AXCL_SKEL_ROI_CONFIG_T);
        itemIndex++;

        // crop_encoder
        stConfig.pstItems[itemIndex].pstrType = (AX_CHAR *)"crop_encoder";
        AXCL_SKEL_CROP_ENCODER_THRESHOLD_CONFIG_T stCropEncoderThreshold = {0};
        stCropEncoderThreshold.fScaleLeft = 0;
        stCropEncoderThreshold.fScaleRight = 0;
        stCropEncoderThreshold.fScaleTop = 0;
        stCropEncoderThreshold.fScaleBottom = 0;
        stConfig.pstItems[itemIndex].pstrValue = (AX_VOID *)&stCropEncoderThreshold;
        stConfig.pstItems[itemIndex].nValueSize = sizeof(AXCL_SKEL_CROP_ENCODER_THRESHOLD_CONFIG_T);
        itemIndex++;

        // resize_panorama_encoder_config
        stConfig.pstItems[itemIndex].pstrType = (AX_CHAR *)"resize_panorama_encoder_config";
        AXCL_SKEL_RESIZE_CONFIG_T stPanoramaResizeConfig = {0};
        stPanoramaResizeConfig.fW = 0;
        stPanoramaResizeConfig.fH = 0;
        stConfig.pstItems[itemIndex].pstrValue = (AX_VOID *)&stPanoramaResizeConfig;
        stConfig.pstItems[itemIndex].nValueSize = sizeof(AXCL_SKEL_RESIZE_CONFIG_T);
        itemIndex++;

        // push_panorama
        stConfig.pstItems[itemIndex].pstrType = (AX_CHAR *)"push_panorama";
        AXCL_SKEL_PUSH_PANORAMA_CONFIG_T stPushPanoramaConfig = {0};
        stPushPanoramaConfig.bEnable = AX_FALSE;
        stPushPanoramaConfig.nQuality = 0;
        stConfig.pstItems[itemIndex].pstrValue = (AX_VOID *)&stPushPanoramaConfig;
        stConfig.pstItems[itemIndex].nValueSize = sizeof(AXCL_SKEL_PUSH_PANORAMA_CONFIG_T);
        itemIndex++;

        // push_quality_body
        stConfig.pstItems[itemIndex].pstrType = (AX_CHAR *)"push_quality_body";
        AXCL_SKEL_ATTR_FILTER_CONFIG_T stBodyAttrFilter = {0};
        stBodyAttrFilter.stCommonAttrFilterConfig.fQuality = 0;
        stConfig.pstItems[itemIndex].pstrValue = (AX_VOID *)&stBodyAttrFilter;
        stConfig.pstItems[itemIndex].nValueSize = sizeof(AXCL_SKEL_ATTR_FILTER_CONFIG_T);
        itemIndex++;

        // push_quality_vehicle
        stConfig.pstItems[itemIndex].pstrType = (AX_CHAR *)"push_quality_vehicle";
        AXCL_SKEL_ATTR_FILTER_CONFIG_T stVehicleAttrFilter = {0};
        stVehicleAttrFilter.stCommonAttrFilterConfig.fQuality = 0;
        stConfig.pstItems[itemIndex].pstrValue = (AX_VOID *)&stVehicleAttrFilter;
        stConfig.pstItems[itemIndex].nValueSize = sizeof(AXCL_SKEL_ATTR_FILTER_CONFIG_T);
        itemIndex++;

        // push_quality_cycle
        stConfig.pstItems[itemIndex].pstrType = (AX_CHAR *)"push_quality_cycle";
        AXCL_SKEL_ATTR_FILTER_CONFIG_T stCycleAttrFilter = {0};
        stCycleAttrFilter.stCommonAttrFilterConfig.fQuality = 0;
        stConfig.pstItems[itemIndex].pstrValue = (AX_VOID *)&stCycleAttrFilter;
        stConfig.pstItems[itemIndex].nValueSize = sizeof(AXCL_SKEL_ATTR_FILTER_CONFIG_T);
        itemIndex++;

        // push_quality_face
        stConfig.pstItems[itemIndex].pstrType = (AX_CHAR *)"push_quality_face";
        AXCL_SKEL_ATTR_FILTER_CONFIG_T stFaceAttrFilter = {0};
        stFaceAttrFilter.stFaceAttrFilterConfig.nWidth = 0;
        stFaceAttrFilter.stFaceAttrFilterConfig.nHeight = 0;
        stFaceAttrFilter.stFaceAttrFilterConfig.stPoseblur.fPitch = 180;
        stFaceAttrFilter.stFaceAttrFilterConfig.stPoseblur.fYaw = 180;
        stFaceAttrFilter.stFaceAttrFilterConfig.stPoseblur.fRoll = 180;
        stFaceAttrFilter.stFaceAttrFilterConfig.stPoseblur.fBlur = 1.0;
        stConfig.pstItems[itemIndex].nValueSize = sizeof(AXCL_SKEL_ATTR_FILTER_CONFIG_T);
        stConfig.pstItems[itemIndex].pstrValue = (AX_VOID *)&stFaceAttrFilter;
        itemIndex++;

        // push_quality_plate
        stConfig.pstItems[itemIndex].pstrType = (AX_CHAR *)"push_quality_plate";
        AXCL_SKEL_ATTR_FILTER_CONFIG_T stPlateAttrFilter = {0};
        stPlateAttrFilter.stCommonAttrFilterConfig.fQuality = 0;
        stConfig.pstItems[itemIndex].pstrValue = (AX_VOID *)&stPlateAttrFilter;
        stConfig.pstItems[itemIndex].nValueSize = sizeof(AXCL_SKEL_ATTR_FILTER_CONFIG_T);
        itemIndex++;

        stConfig.nSize = itemIndex;

        ret = AXCL_SKEL_SetConfig(pHandle, &stConfig);

        if (0 != ret) {
            ALOGE("SKEL AXCL_SKEL_SetConfig, ret = 0x%x", ret);

            goto EXIT3;
        }
    }

    // get config
    {
        const AXCL_SKEL_CONFIG_T *pstConfig = NULL;

        ret = AXCL_SKEL_GetConfig(pHandle, &pstConfig);

        if (0 != ret) {
            ALOGE("SKEL AXCL_SKEL_GetConfig, ret = 0x%x", ret);

            if (pstConfig) {
                AXCL_SKEL_Release((AX_VOID *)pstConfig);
            }

            goto EXIT3;
        }

        ParseConfigParam(pstConfig);

        if (pstConfig) {
            AXCL_SKEL_Release((AX_VOID *)pstConfig);
        }
    }

    ALOGN("Task infomation:");
    ALOGN("\tInput file: %s", InputFile);
    ALOGN("\tInput file resolution: %dx%d", nWidth, nHeight);
    ALOGN("\tRepeat times: %d", nRepeatTimes);
    ALOGN("SKEL Init Elapse:");
    ALOGN("\tAXCL_SKEL_Init: %lld ms", nInitElasped);
    ALOGN("\tAXCL_SKEL_Create: %lld ms", nCreateElasped);

    stFrame.stFrame.u32Width = nWidth;
    stFrame.stFrame.u32Height = nHeight;
    stFrame.stFrame.enImgFormat = AX_FORMAT_YUV420_SEMIPLANAR;
    stFrame.stFrame.u32FrameSize = nFileSize;
    stFrame.stFrame.u32PicStride[0] = nStride;
    stFrame.stFrame.u32PicStride[1] = nStride;
    stFrame.stFrame.u32PicStride[2] = nStride;
    stFrame.stFrame.u64PhyAddr[0] = (AX_U64)pYUVData;
    stFrame.stFrame.u64PhyAddr[1] = (AX_U64)pYUVData + nStride * nHeight;
    stFrame.stFrame.u64PhyAddr[2] = 0;
    stFrame.stFrame.u64VirAddr[0] = 0;
    stFrame.stFrame.u64VirAddr[1] = 0;
    stFrame.stFrame.u64VirAddr[2] = 0;

    stFrame.nStreamId = 0;

    for (AX_U32 nRepeat = 0; nRepeat < nRepeatTimes && !g_exit; nRepeat++) {
        stFrame.nFrameId = nRepeat + 1;
        ALOGN("SKEL Process times: %d", nRepeat + 1);
        nStartTime = get_tick_count_us();

        ret = AXCL_SKEL_SendFrame(pHandle, &stFrame, -1);

        nProcessElasped = get_tick_count_us() - nStartTime;

        if (0 != ret) {
            ALOGE("SKEL Process fail, ret = 0x%x", ret);

            goto EXIT3;
        }

        ALOGN("SKEL Process Elapse:");
        ALOGN("\tAXCL_SKEL_SendFrame: %lld us", nProcessElasped);

        nStartTime = get_tick_count_us();

        ret = AXCL_SKEL_GetResult(pHandle, &pstResult, -1);

        nResultElasped = get_tick_count_us() - nStartTime;

        if (0 != ret) {
            ALOGE("SKEL get result fail, ret = 0x%x", ret);

            goto EXIT3;
        }

        ALOGN("\tAXCL_SKEL_GetResult: %lld us", nResultElasped);

        nResultElaspedTotal += nResultElasped;

        if (nProcessElasped + nResultElasped > nResultElaspedMax) {
            nResultElaspedMax = nProcessElasped + nResultElasped;
        }

        if (nProcessElasped + nResultElasped < nResultElaspedMin) {
            nResultElaspedMin = nProcessElasped + nResultElasped;
        }

        ALOGN("SKEL Process Result:");

        ALOGI("\tFrameId: %lld", pstResult->nFrameId);
        ALOGI("\tnOriginal WxH: %dx%d", pstResult->nOriginalWidth, pstResult->nOriginalHeight);

        ALOGN("\tObject Num: %d", pstResult->nObjectSize);

        AX_U32 nSkelSize = 0;
        AI_Detection_SkelResult_t Skels[50] = {0};
        for (size_t i = 0; i < AX_MIN(50, pstResult->nObjectSize); i++) {
            AXCL_SKEL_OBJECT_ITEM_T *pstItems = &pstResult->pstObjectItems[i];

            ALOGI("\t\tFrameId: %lld", pstItems->nFrameId);
            ALOGI("\t\tTrackId: %lld, TrackState: %d", pstItems->nTrackId, pstItems->eTrackState);

            ALOGN("\t\tRect[%ld] %s: [%f, %f, %f, %f], Confidence: %f", i, pstItems->pstrObjectCategory,
                  pstItems->stRect.fX,
                  pstItems->stRect.fY, pstItems->stRect.fW,
                  pstItems->stRect.fH, pstItems->fConfidence);

            // get detect box only new or update state
            if (pstItems->eTrackState == AXCL_SKEL_TRACK_STATUS_NEW
                || pstItems->eTrackState == AXCL_SKEL_TRACK_STATUS_UPDATE) {
                Skels[nSkelSize].tBox.fX = pstItems->stRect.fX;
                Skels[nSkelSize].tBox.fY = pstItems->stRect.fY;
                Skels[nSkelSize].tBox.fW = pstItems->stRect.fW;
                Skels[nSkelSize].tBox.fH = pstItems->stRect.fH;

                ALOGN("\t\t[%ld]Point Set Size: %d", i, pstItems->nPointSetSize);

                // point
                Skels[nSkelSize].nPointNum = AX_MIN(DETECT_SKEL_POINT_COUNT, pstItems->nPointSetSize);
                for (size_t j = 0; j < Skels[i].nPointNum; j++) {
                    ALOGI("\t\t\tPoint[%ld] %s: [%f, %f] Confidence: %f", j, pstItems->pstPointSet[j].pstrObjectCategory,
                        pstItems->pstPointSet[j].stPoint.fX,
                        pstItems->pstPointSet[j].stPoint.fY,
                        pstItems->pstPointSet[j].fConfidence);
                        Skels[nSkelSize].tPoint[j].fX = pstItems->pstPointSet[j].stPoint.fX;
                        Skels[nSkelSize].tPoint[j].fY = pstItems->pstPointSet[j].stPoint.fY;
                }

                nSkelSize ++;
            }
        }

        if (pstResult) {
            AXCL_SKEL_Release((AX_VOID *)pstResult);
        }

        if (nInterval > 0) {
            usleep(nInterval);
        }
    }

    ALOGN("SKEL Process Elapsed Info: Repeats: %d, (min: %lld us, avr: %lld us, max: %lld us)",
            nRepeatTimes,
            nResultElaspedMin,
            (nRepeatTimes == 0) ? 0 : (nResultElaspedTotal / nRepeatTimes),
            nResultElaspedMax);

EXIT3:
    if (pHandle) {
        AXCL_SKEL_Destroy(pHandle);
    }

EXIT2:
    AXCL_SKEL_DeInit();

EXIT1:
    AXCL_ENGINE_Deinit();

EXIT0:
    if (pYUVData) {
        axclrtFree(pYUVData);
    }

    AXCL_IVPS_Deinit();

    AXCL_SYS_Deinit();

    ALOGI("axcl deinit");
    axclrtResetDevice(nDeviceId);
    axclFinalize();

    return (0 != ret) ? -1 : 0;
}
