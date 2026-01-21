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
#include "../utils/logger.h"
#include "axcl.h"
#include "cmdline.h"

int main(int argc, char *argv[]) {
    SAMPLE_LOG_I("============== APP(%s) Started %s %s ==============\n", AXCL_BUILD_VERSION, __DATE__, __TIME__);

    cmdline::parser a;
    a.add<uint32_t>("device", 'd', "device index from 0 to connected device num - 1", false, 0,
                    cmdline::range(0, AXCL_MAX_DEVICE_COUNT - 1));
    a.add<std::string>("json", '\0', "axcl.json path", false, "./axcl.json");
    a.parse_check(argc, argv);
    const uint32_t device_index = a.get<uint32_t>("device");
    const std::string json = a.get<std::string>("json");

    SAMPLE_LOG_I("json: %s", json.c_str());
    if (axclError ret = axclInit(json.c_str()); AXCL_SUCC != ret) {
        return ret;
    }

    axclrtDeviceList device_list;
    if (axclError ret = axclrtGetDeviceList(&device_list); AXCL_SUCC != ret || 0 == device_list.num) {
        SAMPLE_LOG_E("no device is connected");
        axclFinalize();
        return ret;
    }

    if (device_index >= device_list.num) {
        SAMPLE_LOG_E("device index %d is out of connected device num %d", device_index, device_list.num);
        axclFinalize();
        return 1;
    }

    const int32_t device_id = device_list.devices[device_index];
    SAMPLE_LOG_I("device index: %d, bus number: %d", device_index, device_id);

    if (axclError ret = axclrtSetDevice(device_id); AXCL_SUCC != ret) {
        axclFinalize();
        return ret;
    }

    axclrtResetDevice(device_id);
    axclFinalize();

    SAMPLE_LOG_I("============== APP(%s) Exited %s %s ==============\n", AXCL_BUILD_VERSION, __DATE__, __TIME__);
    return 0;
}