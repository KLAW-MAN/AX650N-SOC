#include <stdio.h>
#include <string.h>
#include <string>
#include "axcl.h"
#include "axcl_rt_usrwork.h"
#include "cmdline.h"
#include "logger.h"
#include "signal_handler.hpp"

static int32_t active_device(int32_t device_index);
static int32_t deactive_device();
static int32_t launch_usrworker(const char* src, const char* dst);
static int32_t device_id;

int main(int argc, char* argv[]) {
    int ret = 0;
    signal_handler handler;

    cmdline::parser a;
    a.add<std::string>("usrworker", 'i', "usrworker application launched in device", true);
    a.add<int32_t>("device", 'd', "device index from 0 to connected device num - 1", false, 0,
                   cmdline::range(0, AXCL_MAX_DEVICE_COUNT - 1));
    a.add<std::string>("json", '\0', "axcl.json path", false, "./axcl.json");

    a.parse_check(argc, argv);
    int32_t device_index = a.get<int32_t>("device");
    const std::string json = a.get<std::string>("json");
    const std::string fpath = a.get<std::string>("usrworker");

    SAMPLE_LOG_I("============== %s sample started %s %s ==============", AXCL_BUILD_VERSION, __DATE__, __TIME__);

    /* initialize */
    if (ret = axclInit(json.c_str()); AXCL_SUCC != ret) {
        SAMPLE_LOG_E("axclInit() fail, ret = 0x%x", (uint32_t)ret);
        return 1;
    }

    /* active device */
    if (ret = active_device(device_index); AXCL_SUCC != ret) {
        SAMPLE_LOG_E("active device %d fail, ret = 0x%x", device_index, (uint32_t)ret);
        axclFinalize();
        return 1;
    }

    /* launch usrworker */
    if (ret = launch_usrworker(fpath.c_str(), "/opt/bin/sample_usrworker"); AXCL_SUCC != ret) {
        SAMPLE_LOG_E("launch usrworker: %s fail, ret = 0x%x", fpath.c_str(), (uint32_t)ret);
        ret = 1;
    }

    /* deactive device */
    deactive_device();
    /* finalize */
    axclFinalize();

    SAMPLE_LOG_I("============== %s sample exited %s %s ==============\n", AXCL_BUILD_VERSION, __DATE__, __TIME__);
    return 0;
}

static int32_t active_device(int32_t device_index) {
    int ret;
    axclrtDeviceList device_list;
    if (ret = axclrtGetDeviceList(&device_list); AXCL_SUCC != ret || 0 == device_list.num) {
        SAMPLE_LOG_E("no device is connected");
        return ret;
    } else {
        if (device_index < 0 || device_index >= static_cast<int32_t>(device_list.num)) {
            SAMPLE_LOG_E("device index %d is out of connected device num %d", device_index, device_list.num);
            return -1;
        }

        SAMPLE_LOG_I("device index: %d, bus number: %d", device_index, device_list.devices[device_index]);
        device_id = device_list.devices[device_index];
        if (ret = axclrtSetDevice(device_id); AXCL_SUCC != ret) {
            SAMPLE_LOG_E("axclrtSetDevice(%d) fail, ret = 0x%x", device_id, (uint32_t)ret);
            return ret;
        }
    }

    return 0;
}

static int32_t deactive_device() {
    int ret;
    if (ret = axclrtResetDevice(device_id); AXCL_SUCC != ret) {
        SAMPLE_LOG_E("axclrtResetDevice(%d) fail, ret = 0x%x", device_id, (uint32_t)ret);
        return ret;
    }

    return 0;
}

static int32_t launch_usrworker(const char* src, const char* dst) {
    int ret;
    /* transfer usrworker to device */
    SAMPLE_LOG_I("transfer %s to %s", src, dst);
    if (ret = axclrtTransferFile(src, dst, FILE_TRANSFER_FROM_HOST_TO_DEVICE); AXCL_SUCC != ret) {
        SAMPLE_LOG_E("transfer %s to %s fail, ret = 0x%x", src, dst, (uint32_t)ret);
        axclFinalize();
        return 1;
    }

    /* launch usrworker */
    const char* argv[] = {"--arg1", "1", "--arg2", "abc"};
    constexpr int32_t argc = sizeof(argv) / sizeof(argv[0]);
    uint32_t pid = 0;
    SAMPLE_LOG_I("launch %s", dst);
    if (ret = axclrtExecWorker(dst, &argc, argv, &pid); AXCL_SUCC != ret) {
        SAMPLE_LOG_E("launch %s fail, ret = 0x%x", dst, (uint32_t)ret);
        return ret;
    }

    /* send message to usrworker */
    constexpr const char* send_msg = "Hello World!";
    constexpr uint32_t timeout = 5000;
    SAMPLE_LOG_I("send: %s", send_msg);
    if (ret = axclrtWorkerSend(pid, send_msg, static_cast<uint32_t>(strlen(send_msg)), timeout); AXCL_SUCC != ret) {
        SAMPLE_LOG_E("send message to usrworker fail, ret = 0x%x", (uint32_t)ret);
        return ret;
    }

    /* recv message from usrworker */
    char recv_msg[1024];
    uint32_t recv_len = 0;
    if (ret = axclrtWorkerRecv(pid, recv_msg, sizeof(recv_msg), &recv_len, timeout); AXCL_SUCC != ret) {
        SAMPLE_LOG_E("recv message from usrworker fail, ret = 0x%x", (uint32_t)ret);
        return ret;
    } else {
        SAMPLE_LOG_I("recv: %s", recv_msg);
    }

    /* send quit message to usrworker to terminate */
    axclrtWorkerSend(pid, "quit", 4, timeout);
    axclrtKillWorker(pid);

    /* remove usrworker from device */
    axclrtTransferFile(dst, nullptr, FILE_TRANSFER_REMOVE_DEVICE_FILE);
    SAMPLE_LOG_I("usrworker %d terminated", pid);

    return 0;
}