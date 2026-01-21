#include <stdio.h>
#include <algorithm>
#include <chrono>
#include <cstdint>
#include <filesystem>
#include <numeric>
#include <string>
#include <vector>
#include "ax_sys_api.h"
#include "cmdline.hpp"
#include "model.hpp"
#include "vdsp.hpp"
#include "yuv.hpp"

static bool do_merge_by_npu(const std::string& axmodel_path, const image_frame& nv12, const image_frame& uv12, uint32_t width,
                            uint32_t height, uint32_t algo, image_frame& output, uint32_t repeat);

int main(int argc, char* argv[]) {
    cmdline::parser cmd;
    cmd.add<std::string>("axmodel_path", 'm', "axmodel path", true);
    cmd.add<std::string>("input", 'i', "rgb888 file path", true);
    cmd.add<uint32_t>("width", 'w', "image width", true);
    cmd.add<uint32_t>("height", 'h', "image width", true);
    cmd.add<uint32_t>("repeat", 'r', "repeat times, default: 1", false, 1);
    cmd.add<std::string>("itcm", '\0', "itcm.bin path, default: /opt/data/dsp/itcm.bin", false, "/opt/data/dsp/itcm.bin");
    cmd.add<std::string>("sram", '\0', "sram.bin path, default: /opt/data/dsp/sram.bin", false, "/opt/data/dsp/sram.bin");
    cmd.add<std::string>("output", 'o', "image output directory", false, "./");
    cmd.add<uint32_t>("algo", '\0', "split and merge algorithm", false, 0, cmdline::oneof<uint32_t>(0, 1));
    cmd.parse_check(argc, argv);

    std::string axmodel_path = cmd.get<std::string>("axmodel_path");
    std::string input_image_path = cmd.get<std::string>("input");
    uint32_t width = cmd.get<uint32_t>("width");
    uint32_t height = cmd.get<uint32_t>("height");
    uint32_t repeat = cmd.get<uint32_t>("repeat");
    std::string itcm_path = cmd.get<std::string>("itcm");
    std::string sram_path = cmd.get<std::string>("sram");
    uint32_t algo = cmd.get<uint32_t>("algo");
    std::string output_dir = cmd.get<std::string>("output");
    if (!std::filesystem::exists(output_dir)) {
        std::filesystem::create_directories(output_dir);
    }

    AX_SYS_Init();

    if (!init_vdsp(itcm_path, sram_path)) {
        return -1;
    }

    image_frame input_rgb_frame;
    if (!load_image(input_image_path, width, height, width * height * 3, input_rgb_frame)) {
        deinit_vdsp();
        return -1;
    }

    /* split by vdsp */
    image_frame nv12_frame[2];
    if (!split_rgb_to_nv12_by_vdsp(input_rgb_frame, nv12_frame, algo)) {
        deinit_vdsp();
        return -1;
    }

    input_rgb_frame.free();
    nv12_frame[0].dump(output_dir + "/algo" + std::to_string(algo) + "_" + "input_" + std::to_string(width) + "x" + std::to_string(height) +
                       ".1.nv12");
    nv12_frame[1].dump(output_dir + "/algo" + std::to_string(algo) + "_" + "input_" + std::to_string(width) + "x" + std::to_string(height) +
                       ".2.nv12");

    /* merge by vdsp */
    image_frame output_rgb_frame_dsp;
    if (!merge_nv12_to_rgb_by_vdsp(nv12_frame, output_rgb_frame_dsp, algo)) {
        deinit_vdsp();
        return -1;
    } else {
        std::string output = output_dir + "/algo" + std::to_string(algo) + "_" + "output_" + std::to_string(width) + "x" +
                             std::to_string(height) + ".dsp.rgb";
        output_rgb_frame_dsp.dump(output);
        output_rgb_frame_dsp.free();
        ALOGI("dsp output is saved to %s", output.c_str());
    }

    deinit_vdsp();

    /* merge by npu */
    image_frame output_rgb_frame_npu;
    if (!do_merge_by_npu(axmodel_path, nv12_frame[0], nv12_frame[1], width, height, algo, output_rgb_frame_npu, repeat)) {
        return -1;
    } else {
        std::string output = output_dir + "/algo" + std::to_string(algo) + "_" + "output_" + std::to_string(width) + "x" +
                             std::to_string(height) + ".npu.rgb";
        output_rgb_frame_npu.dump(output);
        output_rgb_frame_npu.free();
        ALOGI("npu output is saved to %s", output.c_str());
    }

    nv12_frame[0].free();
    nv12_frame[1].free();

    AX_SYS_Deinit();
    return 0;
}

static bool do_merge_by_npu(const std::string& axmodel_path, const image_frame& nv12, const image_frame& uv12, uint32_t width,
                            uint32_t height, uint32_t algo, image_frame& output, uint32_t repeat) {
    model npu;
    if (!npu.init(axmodel_path)) {
        ALOGE("load %s fail", axmodel_path.c_str());
        return false;
    }

    output.width = width;
    output.height = height;
    output.size = width * height * 3;
    if (!output.alloc()) {
        npu.deinit();
        return false;
    }

    std::vector<uint64_t> elapsed;
    elapsed.resize(repeat);

    for (uint32_t i = 0; i < repeat; ++i) {
        auto begin = std::chrono::steady_clock::now();
        if (!npu.inference(nv12, uv12, algo, output)) {
            ALOGE("npu inference failed");
            return false;
        }
        auto end = std::chrono::steady_clock::now();
        elapsed[i] = (uint64_t)(std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count());
    }

    for (uint32_t i = 0; i < repeat; ++i) {
        ALOGI("elapsed[%4d] = %ld us", i + 1, elapsed[i]);
    }

    uint64_t avg = std::accumulate(elapsed.begin(), elapsed.end(), 0) * 1.0 / repeat;
    uint64_t max = *std::max_element(elapsed.begin(), elapsed.end());
    uint64_t min = *std::min_element(elapsed.begin(), elapsed.end());
    ALOGI("min = %ld, avg = %ld, max = %ld us", min, avg, max);

    npu.deinit();
    return true;
}
