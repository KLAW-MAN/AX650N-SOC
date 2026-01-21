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
#include <cstdint>
#include <string>
#include <thread>
#include "axcl.h"
#include "cmdline.h"
#include "logger.h"
#include "signal_handler.hpp"


int main(int argc, char *argv[]) {
    SAMPLE_LOG_I("============== %s sample started %s %s =============", AXCL_BUILD_VERSION, __DATE__, __TIME__);

    signal_handler signal;

    cmdline::parser a;
    a.add<int32_t>("device", 'd', "device index [-1, connected device num - 1], -1: traverse all devices", false, -1,
                   cmdline::range(-1, AXCL_MAX_DEVICE_COUNT - 1));
    a.add<std::string>("json", '\0', "axcl.json path", false, "./axcl.json");
    a.parse_check(argc, argv);
    const int32_t device_index = a.get<int32_t>("device");
    const std::string json = a.get<std::string>("json");

    axclError ret;
    std::vector<int32_t> devices;
    std::vector<std::thread> threads;

    /* init axcl */
    if (ret = axclInit(json.c_str()); AXCL_SUCC != ret) {
        return 1;
    }

    /* get device list */
    axclrtDeviceList device_list;
    if (ret = axclrtGetDeviceList(&device_list); AXCL_SUCC != ret || 0 == device_list.num) {
        SAMPLE_LOG_E("no device is connected");
        axclFinalize();
        return ret;
    } else {
        if (device_index >= static_cast<int32_t>(device_list.num)) {
            SAMPLE_LOG_E("device index %d is out of connected device num %d", device_index, device_list.num);
            axclFinalize();
            return 1;
        }

        if (device_index < 0) {
            devices.assign(device_list.devices, device_list.devices + device_list.num);
        } else {
            devices.push_back(device_list.devices[device_index]);
        }
    }

    /* active each device to setup connection */
    for (size_t i = 0; i < devices.size(); i++) {
        if (ret = axclrtSetDevice(devices[i]); AXCL_SUCC != ret) {
            SAMPLE_LOG_E("active device %d failed, ret = 0x%x", devices[i], ret);

            /* if active device failed, deactive all active devices and deinit axcl */
            for (size_t j = 0; j < i; j++) {
                axclrtResetDevice(devices[j]);
            }

            axclFinalize();
            return 1;
        }
    }

    for (size_t i = 0; i < devices.size(); i++) {
        threads.emplace_back(
            [](int32_t device) -> void {
                axclError ret;

                /**
                 * create context
                 *  Each thread should create a context to specify which device the APIs should work on.
                 *
                 *  As for more information about context and thread, please refer to:
                 *  https://axcl-docs.readthedocs.io/zh-cn/latest/doc_introduction.html#context
                 *
                 */
                axclrtContext context;
                if (ret = axclrtCreateContext(&context, device); AXCL_SUCC != ret) {
                    return;
                }

                /* check current device is what we want, here is just for test, not mandatory */
                int32_t current_device;
                if (ret = axclrtGetDevice(&current_device); AXCL_SUCC != ret) {
                    SAMPLE_LOG_E("get current device failed, ret = 0x%x", ret);
                } else {
                    if (current_device != device) {
                        SAMPLE_LOG_E("current device is %2d, not expected %2d", current_device, device);
                    }
                }

                /* get device properties */
                axclrtDeviceProperties prop;
                if (ret = axclrtGetDeviceProperties(device, &prop); AXCL_SUCC != ret) {
                    SAMPLE_LOG_E("get device properties failed, ret = 0x%x", ret);
                } else {
                    SAMPLE_LOG_I("[%04x:%02x.0] software version: %s", prop.pciDomain, prop.pciBusID, prop.swVersion);
                    SAMPLE_LOG_I("[%04x:%02x.0] uid             : 0x%llx", prop.pciDomain, prop.pciBusID, (unsigned long long)prop.uid);
                    SAMPLE_LOG_I("[%04x:%02x.0] temperature     : %d", prop.pciDomain, prop.pciBusID, prop.temperature);
                    SAMPLE_LOG_I("[%04x:%02x.0] total mem size  : %-8d KB", prop.pciDomain, prop.pciBusID, prop.totalMemSize);
                    SAMPLE_LOG_I("[%04x:%02x.0] free  mem size  : %-8d KB", prop.pciDomain, prop.pciBusID, prop.freeMemSize);
                    SAMPLE_LOG_I("[%04x:%02x.0] total cmm size  : %-8d KB", prop.pciDomain, prop.pciBusID, prop.totalCmmSize);
                    SAMPLE_LOG_I("[%04x:%02x.0] free  cmm size  : %-8d KB", prop.pciDomain, prop.pciBusID, prop.freeCmmSize);
                    SAMPLE_LOG_I("[%04x:%02x.0] avg cpu loading : %.1f%%", prop.pciDomain, prop.pciBusID, prop.cpuLoading / 100.0);
                    SAMPLE_LOG_I("[%04x:%02x.0] avg npu loading : %.1f%%", prop.pciDomain, prop.pciBusID, prop.npuLoading / 100.0);
                }

                /**
                 * Here just an example to malloc and free memory from device.
                 */
                constexpr size_t size = 1024 * 1024;
                void *mem;
                if (ret = axclrtMalloc(&mem, size, AXCL_MEM_MALLOC_HUGE_FIRST); AXCL_SUCC != ret) {
                    SAMPLE_LOG_E("malloc %zu bytes memory from device %2d failed, ret = 0x%x", size, device, ret);
                } else {
                    SAMPLE_LOG_I("malloc %zu bytes memory from device %2d success, addr = %p", size, device, mem);
                    axclrtFree(mem);
                }

                /* destroy context*/
                axclrtDestroyContext(context);
            },
            devices[i]);
    }

    /* wait for all threads to finish */
    for (auto &t : threads) {
        t.join();
    }

    /* deactive each device to close connection */
    for (auto &device : devices) {
        axclrtResetDevice(device);
    }

    /* deinit axcl */
    axclFinalize();

    SAMPLE_LOG_I("============== %s sample exited %s %s ==============\n", AXCL_BUILD_VERSION, __DATE__, __TIME__);
    return 0;
}
