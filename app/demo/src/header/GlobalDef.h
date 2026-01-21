/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#pragma once

#ifdef AX_MEM_CHECK
#include <mcheck.h>
#define AX_MTRACE_ENTER(name) do { \
    if (!getenv("MALLOC_TRACE")) { \
        setenv("MALLOC_TRACE", ""#name"_mtrace.log", 1); \
    } \
    mtrace();\
}while(0)

#define AX_MTRACE_LEAVE do { \
    printf("please wait mtrace flush log to file...\n"); \
    sleep(30);\
}while(0)

#else
#define AX_MTRACE_ENTER(name)
#define AX_MTRACE_LEAVE
#endif

#define SDK_VERSION_PREFIX  "Ax_Version"

#ifndef AX_MAX
#define AX_MAX(a, b)        (((a) > (b)) ? (a) : (b))
#endif

#ifndef AX_MIN
#define AX_MIN(a, b)        (((a) < (b)) ? (a) : (b))
#endif

#ifndef ALIGN_UP
#define ALIGN_UP(x, align) (((x) + ((align) - 1)) & ~((align)-1))
#endif

#ifndef ALIGN_DOWN
#define ALIGN_DOWN(x, align) ((x) & ~((align)-1))
#endif

#ifndef ALIGN_COMM_UP
#define ALIGN_COMM_UP(x, align) ((((x) + ((align) - 1)) / (align)) * (align))
#endif

#ifndef ALIGN_COMM_DOWN
#define ALIGN_COMM_DOWN(x, align) (((x) / (align)) * (align))
#endif

#ifndef ADAPTER_RANGE
#define ADAPTER_RANGE(v, min, max)    ((v) < (min)) ? (min) : ((v) > (max)) ? (max) : (v)
#endif

#ifndef AX_BIT_CHECK
#define AX_BIT_CHECK(v, b) (((AX_U32)(v) & (1 << (b))))
#endif

#ifndef AX_BIT_SET
#define AX_BIT_SET(v, b) ((v) |= (1 << (b)))
#endif

#ifndef AX_BIT_CLEAR
#define AX_BIT_CLEAR(v, b) ((v) &= ~(1 << (b)))
#endif

#define SAFE_DELETE_PTR(p) {if(p){delete p;p = nullptr;}}

#define NOT_USED(x) ((void*) x)

#define AX_VIN_FBC_WIDTH_ALIGN_VAL (256)
#define AX_VIN_IPC_FBC_WIDTH_ALIGN_VAL (128)
#define AX_IVPS_FBC_WIDTH_ALIGN_VAL (128)
#define AX_ENCODER_FBC_WIDTH_ALIGN_VAL (128)

#define AX_IVPS_FILTER_NUM (2)

#define AX_IVPS_FBC_STRIDE_ALIGN_VAL (256)
#define AX_IVPS_NONE_FBC_STRIDE_ALIGN_VAL (16)
#define AX_ENCODER_FBC_STRIDE_ALIGN_VAL (256)
#define AX_ENCODER_NONE_FBC_STRIDE_ALIGN_VAL (16)

#define AX_SHIFT_LEFT_ALIGN(a) (1 << (a))

/* VDEC stride align 256 */
#define VDEC_STRIDE_ALIGN AX_SHIFT_LEFT_ALIGN(8)

#define ADAPTER_INT2BOOLSTR(val) (val == 1 ? "true" : "false")
#define ADAPTER_INT2BOOL(val) (val == 1 ? AX_TRUE : AX_FALSE)
#define ADAPTER_BOOL2BOOLSTR(val) (val == AX_TRUE ? "true" : "false")
#define ADAPTER_BOOLSTR2INT(val) (strcmp(val, "true") == 0 ? 1 : 0)

#define AX_APP_LOCKQ_CAPACITY (3)

#define AX_EZOOM_MAX  (32)

#define APP_VIN_STITCH_GRP (0)

#define WEB_SET_BOOL(var, obj, key)  do{\
    if(!obj) return AX_FALSE;\
    cchar *pStr = mprGetJson(obj, key);\
    if (pStr) {\
        var = strcmp(pStr, "true") == 0 ? AX_TRUE : AX_FALSE;\
    } else {\
        return AX_FALSE;\
    }\
}while(0)

#define WEB_SET_INT(var, obj, key)  do{\
    if(!obj) return AX_FALSE;\
    cchar *pStr = mprGetJson(obj, key);\
    if (pStr) {\
        var = atoi(pStr);\
    } else {\
        return AX_FALSE;\
    }\
}while(0)

#define WEB_SET_HEX(var, obj, key)  do{\
    if(!obj) return AX_FALSE;\
    cchar *pStr = mprGetJson(obj, key);\
    char *pEnd = NULL;\
    if (pStr) {\
        var = strtol(pStr, &pEnd, 16);\
    } else {\
        return AX_FALSE;\
    }\
}while(0)

#define WEB_SET_INT_OBJ(var, obj)  do{\
    if(!obj) return AX_FALSE;\
    cchar *pStr = obj->value;\
    if (pStr) {\
        var = atoi(pStr);\
    } else {\
        return AX_FALSE;\
    }\
}while(0)

#define WRITE_JSON_STR(obj, key, value)  \
    mprWriteJson(obj, key, value.c_str(), MPR_JSON_STRING)

#define WRITE_JSON_PSTR(obj, key, value)  \
    mprWriteJson(obj, key, value, MPR_JSON_STRING)

#define WRITE_JSON_INT(obj, key, value)  \
    mprWriteJson(obj, key, std::to_string(value).c_str(), MPR_JSON_NUMBER)

#define WRITE_JSON_BOOL(obj, key, value)  { \
    const char* str = value ? "true" : "false";\
    AX_S32 type = value ? MPR_JSON_TRUE : MPR_JSON_FALSE;\
    mprWriteJson(obj, key, str, type);\
}

#define WRITE_JSON_OBJ(obj, key, value)  \
    mprWriteJsonObj(obj, key, value)\

#define WRITE_JSON_HEX(obj, key, format, value)  { \
    AX_CHAR buffer[32] = {0};\
    sprintf(buffer, format, value);\
    mprWriteJson(obj, key, buffer, MPR_JSON_STRING);\
}

#define APPEND_JSON_OBJ(obj, value)  \
    AppenJsonChild(obj, value)

#define APPEND_JSON_STR(obj, value)  \
    AppenJsonChild(obj, mprCreateJsonValue(value.c_str(), MPR_JSON_STRING))

#define APPEND_JSON_INT(obj, value)  {\
    AppenJsonChild(obj, mprCreateJsonValue(std::to_string(value).c_str(), MPR_JSON_NUMBER));\
}

#define APPEND_JSON_2_INT(obj, value1, value2)  \
    AppenJsonChild(obj, mprCreateJsonValue(std::to_string(value1).c_str(), MPR_JSON_NUMBER));\
    AppenJsonChild(obj, mprCreateJsonValue(std::to_string(value2).c_str(), MPR_JSON_NUMBER))

#define JSON_PARSE_BOOL_ITEM(module, outData, jsonObj, tag, bItemCheck) do { \
    if (jsonObj.end() != jsonObj.find(tag)) { \
        outData = (AX_BOOL)jsonObj[tag].get<bool>(); \
    } else if (bItemCheck) { \
        LOG_MM_E(module, "Missed '%s' config!", tag); \
        return AX_FALSE; \
    } \
}while(0)

#define JSON_PARSE_DIGITAL_ITEM(module, outData, outDataType, jsonObj, tag, bItemCheck) do { \
    if (jsonObj.end() != jsonObj.find(tag)) { \
        outData = (outDataType)jsonObj[tag].get<double>(); \
    } else if (bItemCheck) { \
        LOG_MM_E(module, "Missed '%s' config!", tag); \
        return AX_FALSE; \
    } \
}while(0)

#define JSON_PARSE_STRING_ITEM(module, outData, jsonObj, tag, bItemCheck) do { \
    if (jsonObj.end() != jsonObj.find(tag)) { \
        outData = jsonObj[tag].get<std::string>(); \
    } else if (bItemCheck) { \
        LOG_MM_E(module, "Missed '%s' config!", tag); \
        return AX_FALSE; \
    } \
}while(0)

#define JSON_PARSE_CONVERT_STRING_ITEM(module, outData, jsonObj, tag, convertFunc, bItemCheck) do { \
    if (jsonObj.end() != jsonObj.find(tag)) { \
        outData = convertFunc(jsonObj[tag].get<std::string>()); \
    } else if (bItemCheck) { \
        LOG_MM_E(module, "Missed '%s' config!", tag); \
        return AX_FALSE; \
    } \
}while(0)

#define JSON_PARSE_FIND_ITEM(module, result, jsonObj, tag, bItemCheck) do { \
    if (jsonObj.end() == jsonObj.find(tag)) { \
        if (bItemCheck) { \
            LOG_MM_E(module, "Missed '%s' config!", tag); \
            return AX_FALSE; \
        } \
        result = AX_FALSE; \
    } else { \
        result = AX_TRUE; \
    } \
}while(0)

#define JSON_PARSE_ARRAY_ITEMS(module, outArray, jsonObj, tag, arrayItemType, bItemCheck) do { \
    if (jsonObj.end() != jsonObj.find(tag)) { \
        picojson::array &jsonArray = jsonObj[tag].get<picojson::array>(); \
        for (size_t i = 0; i < jsonArray.size(); i++) { \
            outArray = jsonArray[i].get<arrayItemType>(); \
        } \
    } else if (bItemCheck) { \
        LOG_MM_E(module, "Missed '%s' config!", tag); \
        return AX_FALSE; \
    } \
}while(0)

#define JSON_PARSE_CONVERT_ARRAY_INT_ITEMS_TO_BOOL(module, outArray, jsonObj, tag, bItemCheck) do { \
    if (jsonObj.end() != jsonObj.find(tag)) { \
        picojson::array &jsonArray = jsonObj[tag].get<picojson::array>(); \
        for (size_t i = 0; i < jsonArray.size(); i++) { \
            outArray = jsonArray[i].get<double>() ? AX_TRUE : AX_FALSE; \
        } \
    } else if (bItemCheck) { \
        LOG_MM_E(module, "Missed '%s' config!", tag); \
        return AX_FALSE; \
    } \
}while(0)

#define JSON_PARSE_CONVERT_ARRAY_ITEMS(module, outArray, jsonObj, tag, arrayItemType, convertFunc, bItemCheck) do { \
    if (jsonObj.end() != jsonObj.find(tag)) { \
        picojson::array &jsonArray = jsonObj[tag].get<picojson::array>(); \
        for (size_t i = 0; i < jsonArray.size(); i++) { \
            outArray = convertFunc(jsonArray[i].get<arrayItemType>()); \
        } \
    } else if (bItemCheck) { \
        LOG_MM_E(module, "Missed '%s' config!", tag); \
        return AX_FALSE; \
    } \
}while(0)
