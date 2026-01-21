#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "axcl_rt_device.h"
#include "randomizer.hpp"
#include "serializer.hpp"

#define IMPLEMENT_SERIALIZE(...)                        \
    do {                                                \
        SERILAIZER()->input()->serialize(__VA_ARGS__);  \
        axclError ret = initialize_random<axclError>(); \
        SERILAIZER()->output()->serialize(ret);         \
        return ret;                                     \
    } while (0)

AXCL_EXPORT axclError axclrtSetDevice(int32_t deviceId) {
    IMPLEMENT_SERIALIZE(deviceId);
}

AXCL_EXPORT axclError axclrtResetDevice(int32_t deviceId) {
    IMPLEMENT_SERIALIZE(deviceId);
}

AXCL_EXPORT axclError axclrtGetDevice(int32_t *deviceId) {
    SERILAIZER()->input()->serialize();
    *deviceId = initialize_random<int32_t>();
    axclError ret = initialize_random<axclError>();
    SERILAIZER()->output()->serialize(ret, *deviceId);
    return ret;
}

AXCL_EXPORT axclError axclrtGetDeviceCount(uint32_t *count) {
    SERILAIZER()->input()->serialize();
    *count = initialize_random<uint32_t>();
    axclError ret = initialize_random<axclError>();
    SERILAIZER()->output()->serialize(ret, *count);
    return ret;
}

AXCL_EXPORT axclError axclrtGetDeviceList(axclrtDeviceList *deviceList) {
    SERILAIZER()->input()->serialize();

    memset(deviceList, 0, sizeof(axclrtDeviceList));
    deviceList->num = static_cast<uint32_t>(create_int32_random_instance(0, AXCL_MAX_DEVICE_COUNT));
    for (uint32_t i = 0; i < deviceList->num; ++i) {
        deviceList->devices[i] = initialize_random<int32_t>();
    }
    axclError ret = 0;
    SERILAIZER()->output()->serialize(ret, *deviceList);
    return ret;
}

AXCL_EXPORT axclError axclrtSynchronizeDevice() {
    IMPLEMENT_SERIALIZE();
}

AXCL_EXPORT axclError axclrtGetDeviceProperties(int32_t deviceId, axclrtDeviceProperties *properties) {
    SERILAIZER()->input()->serialize(deviceId);
    memset(properties, 0, sizeof(axclrtDeviceProperties));
    strcpy(properties->swVersion, "1.0.0");
    properties->uid = initialize_random<uint64_t>();
    properties->pciDomain = initialize_random<uint32_t>();
    properties->pciBusID = initialize_random<uint32_t>();
    properties->pciDeviceID = initialize_random<uint32_t>();
    properties->temperature = initialize_random<int32_t>();
    properties->totalMemSize = initialize_random<int32_t>();
    properties->freeMemSize = initialize_random<int32_t>();
    properties->totalCmmSize = initialize_random<int32_t>();
    properties->freeCmmSize = initialize_random<int32_t>();
    properties->cpuLoading = initialize_random<int32_t>();
    properties->npuLoading = initialize_random<int32_t>();
    for (size_t i = 0; i < sizeof(properties->reserved) / sizeof(properties->reserved[0]); ++i) {
        properties->reserved[i] = initialize_random<int32_t>();
    }
    axclError ret = initialize_random<axclError>();
    SERILAIZER()->output()->serialize(ret, *properties);
    return ret;
}