/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#include "SensorFactory.hpp"
#include <exception>
#include "OS04a10.h"
#include "OS04a10Dcg.h"
#include "OS04a10Vs.h"
#include "OS04a102Lane.h"
#include "SC450AI2Lane.h"
#include "OS08a20.h"
#include "OS08b10.h"
#include "SC910gs.h"
#include "SC410gs.h"
#include "SC500ai.h"
#include "IMX678.h"

ISensor *CSensorFactory::CreateSensor(const SENSOR_CONFIG_T &tSensorCfg) {
    ISensor *pSnsInstance{nullptr};
    switch (tSensorCfg.eSensorType) {
        case E_SNS_TYPE_OS04A10:
        case E_SNS_TYPE_OS04A10_LF:
        case E_SNS_TYPE_OS04A10_SF:
        case E_SNS_TYPE_QUAD_OS04A10:
        case E_SNS_TYPE_OS04A10x2_PANO: {
            pSnsInstance = new COS04a10(tSensorCfg);
            break;
        }
        case E_SNS_TYPE_OS04A10x8_PANO: {
            pSnsInstance = new COS04a102Lane(tSensorCfg);
            break;
        }
        case E_SNS_TYPE_SC450AIx8_PANO: {
            pSnsInstance = new CSC450AI2Lane(tSensorCfg);
            break;
        }
        case E_SNS_TYPE_OS04A10_DCG: {
            pSnsInstance = new COS04a10Dcg(tSensorCfg);
            break;
        }
        case E_SNS_TYPE_OS04A10_VS: {
            pSnsInstance = new COS04a10Vs(tSensorCfg);
            break;
        }
        case E_SNS_TYPE_OS08A20:
        case E_SNS_TYPE_QUAD_OS08A20:
        case E_SNS_TYPE_OS08A20_LF:
        case E_SNS_TYPE_OS08A20_SF: {
            pSnsInstance = new COS08a20(tSensorCfg);
            break;
        }
        case E_SNS_TYPE_OS08B10:
            pSnsInstance = new COS08b10(tSensorCfg);
            break;
        case E_SNS_TYPE_SC910GS:
            pSnsInstance = new CSC910gs(tSensorCfg);
            break;
        case E_SNS_TYPE_SC410GS:
            pSnsInstance = new CSC410gs(tSensorCfg);
            break;
        case E_SNS_TYPE_SC500AI:
            pSnsInstance = new CSC500ai(tSensorCfg);
            break;
        case E_SNS_TYPE_QUAD_IMX678:
            pSnsInstance = new CIMX678(tSensorCfg);
            break;
        default: {
            return nullptr;
        }
    }

    return pSnsInstance;
}

AX_VOID CSensorFactory::DestorySensor(ISensor *&pSensor) {
    if (pSensor) {
        ((CBaseSensor *)pSensor)->DeInit();
        delete pSensor;
        pSensor = nullptr;
    }
}