#include <stdarg.h>
#include <stdio.h>
#include "axcl.h"
#include "cmdline.h"

#define COLOR_RED       "\033[1;30;31m"
#define COLOR_GREEN     "\033[1;30;32m"
#define COLOR_YELLOW    "\033[1;30;33m"
#define COLOR_DEFAULT   "\033[0m"

#define LOG_E(fmt, ...) printf(COLOR_RED    "[FAIL ][%32s][%4d]: " fmt COLOR_DEFAULT, __func__, __LINE__, ##__VA_ARGS__)
#define LOG_W(fmt, ...) printf(COLOR_YELLOW "[WARN ][%32s][%4d]: " fmt COLOR_DEFAULT, __func__, __LINE__, ##__VA_ARGS__)
#define LOG_I(fmt, ...) printf(COLOR_GREEN  "[INFO ][%32s][%4d]: " fmt COLOR_DEFAULT, __func__, __LINE__, ##__VA_ARGS__)

int main(int argc, char **argv) {
    cmdline::parser a;
    a.add<uint32_t>("device", 'd', "device index from 0 to connected device num - 1", false, 0,
                    cmdline::range(0, AXCL_MAX_DEVICE_COUNT - 1));
    a.add<uint32_t>("subvendor", '\0', "sub vendor id (decimal)", false, 0x1F4B);
    a.add<uint32_t>("subdevice", '\0', "sub device id (decimal)", false, 0x0650);
    a.add<std::string>("json", '\0', "axcl.json path", false, "./axcl.json");

    a.parse_check(argc, argv);

    const uint32_t sub_device_id = a.get<uint32_t>("subdevice");
    const uint32_t sub_vendor_id = a.get<uint32_t>("subvendor");
    const uint32_t device_index = a.get<uint32_t>("device");
    const std::string json = a.get<std::string>("json");

    LOG_W("-----------------------------------------------------------------------------------------------------------------------\n");
    LOG_W("This sample is used to change sub vendor id and sub device id of PCIe EP device ONLY with NOR flash as storage.\n");
    LOG_W("To change sub vendor id and sub device id may cause device not work properly, please confirm the following information:\n");
    LOG_W("  target sub vendor id: 0x%x, target sub device id: 0x%x\n", sub_vendor_id, sub_device_id);
    LOG_W("-----------------------------------------------------------------------------------------------------------------------\n");
    LOG_W("Press y(Y) to continue, or press CTRL+C to cancel: ");
    char c;
    if (scanf("%c", &c) != 1) {
        return 0;
    }

    if (c != 'Y' && c != 'y') {
        return 0;
    }

    axclError ret;
    if (ret = axclInit(json.c_str()); AXCL_SUCC != ret) {
        LOG_E("axclInit(%s) fail, ret = 0x%x\n", json.c_str(), static_cast<uint32_t>(ret));
        return ret;
    }

    axclrtDeviceList device_list;
    if (ret = axclrtGetDeviceList(&device_list); AXCL_SUCC != ret || 0 == device_list.num) {
        LOG_E("no device is connected\n");
        axclFinalize();
        return ret;
    }

    if (device_index >= device_list.num) {
        LOG_E("device index %d is out of connected device num %d\n", device_index, device_list.num);
        axclFinalize();
        return -1;
    }

    LOG_I("device index: %d, bus number: %d\n", device_index, device_list.devices[device_index]);
    const int32_t device_id = device_list.devices[device_index];

    if (ret = axclrtSetDevice(device_id); AXCL_SUCC != ret) {
        LOG_E("axclrtSetDevice(%d) fail, ret = 0x%x\n", device_id, static_cast<uint32_t>(ret));
        axclFinalize();
        return ret;
    }

    /* change sub vendor id and sub device id */
    ret = axclrtControlChangePCIeSubId(sub_vendor_id, sub_device_id);

    axclrtResetDevice(device_id);
    axclFinalize();

    if (AXCL_SUCC != ret) {
        LOG_E("change sub vendor id and sub device id to (0x%x, 0x%x) fail, ret = 0x%x\n", sub_vendor_id, sub_device_id,
              static_cast<uint32_t>(ret));
    } else {
        LOG_I("change sub vendor id and sub device id to (0x%x, 0x%x) success\n", sub_vendor_id, sub_device_id);
    }

    return 0;
}
