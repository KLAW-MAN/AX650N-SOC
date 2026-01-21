/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#include "common/args.hpp"
#include "common/types.hpp"
#include "common/sampler.hpp"
#include "common/detection.hpp"
#include "common/mscoco.hpp"
#include "common/palette.hpp"

#include "utilities/timer.hpp"
#include "utilities/file.hpp"
#include "utilities/scalar_guard.hpp"
#include "utilities/vector_guard.hpp"

#include <axcl.h>

#include <cmdline.h>
#include <opencv2/opencv.hpp>

#include <algorithm>
#include <numeric>

constexpr char  DEFAULT_CONFIG[] = "/usr/local/axcl/axcl.json";

constexpr int   DEFAULT_IMG_H = 640;
constexpr int   DEFAULT_IMG_W = 640;
constexpr float DEFAULT_PROB_THRESHOLD = 0.45f;
constexpr float DEFAULT_IOU_THRESHOLD = 0.45f;

constexpr int   DEFAULT_LOOP_COUNT = 1;

const std::vector<std::vector<detection::box>> anchors{
    {{10, 13}, {16, 30}, {33, 23}},
    {{30, 61}, {62, 45}, {59, 119}},
    {{116, 90}, {156, 198}, {373, 326}}
};

const std::vector<int> strides{8, 16, 32};
std::vector<cv::Vec3b> palette = common::palette(std::size(MSCOCO_CLASSES));

static void draw_objects(const cv::Mat& bgr, const std::string& name, const std::vector<detection::object>& objects) {
    cv::Mat image = bgr.clone();

    for (const auto& [rect, prob, label] : objects) {
        const auto& [x, y, w, h] = rect;
        fprintf(stdout, "%2d: %3.0f%%, [%4.0f, %4.0f, %4.0f, %4.0f], %s\n",
            label, prob * 100, x, y, x + w, y + h, MSCOCO_CLASSES[label]);

        const cv::Rect obj_rect{static_cast<int>(x), static_cast<int>(y), static_cast<int>(w), static_cast<int>(h)};

        const auto& color = palette[label];
        cv::rectangle(image, obj_rect, color, 2);

        char text[256];
        sprintf(text, "%s %.1f%%", MSCOCO_CLASSES[label], prob * 100);

        int baseLine = 0;
        const cv::Size label_size = cv::getTextSize(text, cv::FONT_HERSHEY_TRIPLEX, 0.5, 1, &baseLine);

        int cv_x = std::max(static_cast<int>(x - 1), 0);
        int cv_y = std::max(static_cast<int>(y + 1) - label_size.height - baseLine, 0);

        if (cv_x + label_size.width > image.cols) {
            cv_x = image.cols - label_size.width;
        }
        if (cv_y + label_size.height > image.rows) {
            cv_y = image.rows - label_size.height;
        }

        const cv::Size text_background_size(label_size.width, label_size.height + baseLine);
        const cv::Rect text_background(cv::Point(cv_x, cv_y), text_background_size);
        cv::rectangle(image, text_background, color, -1);

        const cv::Point text_origin(cv_x, cv_y + label_size.height);
        const auto text_color = common::get_complementary_color(color);
        cv::putText(image, text, text_origin, cv::FONT_HERSHEY_TRIPLEX, 0.5, text_color);
    }

    cv::imwrite(std::string(name) + ".jpg", image);
}

int main(int argc, char* argv[]) {
    cmdline::parser cmd;

    cmd.add<std::string>("config", 'c', "config file: axcl.json", false, DEFAULT_CONFIG);
    cmd.add<int>("device", 'd', "device index (if multi-card available)", false, 0, cmdline::range(0, 3));

#if defined(ENV_CHIP_SERIES_MC50)
    cmd.add<int>("kind", 'k', "visual npu kind", false, 0, cmdline::range(0, 3));
#endif

#if defined(ENV_CHIP_SERIES_MC20E)
    cmd.add<int>("kind", 'k', "visual npu enable flag", false, 0, cmdline::range(0, 1));
#endif

    cmd.add<std::string>("model", 'm', "model file(a.k.a. .axmodel)", true, "");
    cmd.add<std::string>("image", 'i', "image file", true, "");
    cmd.add<std::string>("size", 'g', "input_h,input_w", false, std::to_string(DEFAULT_IMG_H) + "," + std::to_string(DEFAULT_IMG_W));

    cmd.add<float> ("prob", 'p', "probability threshold", false, DEFAULT_PROB_THRESHOLD);
    cmd.add<float> ("iou", 'u', "iou threshold", false, DEFAULT_IOU_THRESHOLD, cmdline::range(0.1f, 1.0f));

    cmd.add("swap-rb", 's', "swap rgb(bgr) to bgr(rgb)");

    cmd.add<int>("repeat", 'r', "repeat count", false, DEFAULT_LOOP_COUNT, cmdline::range(1, std::numeric_limits<int>::max()));
    cmd.parse_check(argc, argv);

    // 0-0. check config file
    if (const auto config = cmd.get<std::string>("config");
        cmd.exist("config") && (!utilities::exists(config) || !utilities::is_regular_file(config))) {
        fprintf(stderr, "Config file %s is not exist, please check it.\n", config.c_str());
        return -1;
    }

    // 0-1. check file exist
    const auto model_file = cmd.get<std::string>("model");
    const auto image_file = cmd.get<std::string>("image");

    if (const auto model_file_flag = utilities::exists(model_file), image_file_flag = utilities::exists(image_file);
        !model_file_flag | !image_file_flag) {
        auto show_error = [](const std::string& kind, const std::string& value) {
            fprintf(stderr, "Input file %s(%s) is not exist, please check it.\n", kind.c_str(), value.c_str());
        };

        if (!model_file_flag) { show_error("model", model_file); }
        if (!image_file_flag) { show_error("image", image_file); }

        return -1;
    }

    // 0-2. get input size
    std::array<int, 2> input_size = {DEFAULT_IMG_H, DEFAULT_IMG_W}; {
        auto input_size_string = cmd.get<std::string>("size");
        if (auto input_size_flag = common::parse_args(input_size_string, input_size); !input_size_flag) {
            auto show_error = [](const std::string& kind, const std::string& value) {
                fprintf(stderr, "Input %s(%s) is not allowed, please check it.\n", kind.c_str(), value.c_str());
            };

            show_error("size", input_size_string);
            return -1;
        }
    }

    // 0-3. get repeat count
    auto repeat = cmd.get<int>("repeat");

    // 1-0. get app args, can be removed from user's app
    const auto device_index = static_cast<uint32_t>(cmd.get<int>("device"));

    // 1-1 init axcl, using scalar_guard to ensure the finalization
    fprintf(stdout, "axcl initializing...\n");
    auto env_guard = utilities::scalar_guard<int32_t>(
        axclInit(cmd.exist("config") ? cmd.get<std::string>("config").c_str() : nullptr),
        [](const int32_t& code) {
            if (0 == code) {
                std::ignore = axclFinalize();
            }
        }
    );

    // 1-2. check the initialization result
    if (const int ret = env_guard.get(); 0 != ret) {
        fprintf(stderr, "Init axcl failed{0x%08X}.\n", ret);
        return false;
    }
    fprintf(stdout, "axcl inited.\n");

    // 1-3. get device list
    axclrtDeviceList lst;
    if (const auto ret = axclrtGetDeviceList(&lst); 0 != ret || 0 == lst.num) {
        fprintf(stderr,
            "Get axcl device failed{0x%08X}, find total %d device.\n", ret, lst.num);
        return false;
    }

    // 1-4. check the device index
    if (device_index >= lst.num) {
        fprintf(stderr,
            "Specified device index{%u} is out of range{total %d}.\n", device_index, lst.num);
        return false;
    }

    // 1-5. set device
    if (const auto ret = axclrtSetDevice(lst.devices[device_index]); 0 != ret) {
        fprintf(stderr, "Set axcl device as index{%u} failed{0x%08X}.\n", device_index, ret);
        return false;
    }
    fprintf(stdout,"Select axcl device{index: %u} as {%d}.\n", device_index, lst.devices[device_index]);

    // 1-6. init NPU
    const int kind = cmd.get<int>("kind");
    if (const int ret = axclrtEngineInit(static_cast<axclrtEngineVNpuKind>(kind)); 0 != ret) {
        fprintf(stderr, "Init axclrt Engine as kind{%s} failed{0x%08X}.\n", common::get_visual_mode_string(kind).c_str(), ret);
        return false;
    }
    fprintf(stdout, "axclrt Engine inited.\n");

    // 1-7. print args
    fprintf(stdout, "--------------------------------------\n");
    fprintf(stdout, "model file : %s\n", model_file.c_str());
    fprintf(stdout, "image file : %s\n", image_file.c_str());
    fprintf(stdout, "img height : %d\n", input_size[0]);
    fprintf(stdout, "img width  : %d\n", input_size[1]);
    fprintf(stdout, "--------------------------------------\n");

    // 2-1. load model
    auto m = utilities::scalar_guard<uint64_t>(
        [&model_file]() {
            if (uint64_t id; 0 == axclrtEngineLoadFromFile(model_file.c_str(), &id)) {
                return id;
            }
            fprintf(stderr, "Create model{%s} handle failed.\n", model_file.c_str());
            return uint64_t{0};
        },
        [](const uint64_t& id) {
            if (uint64_t{0} != id) {
                std::ignore = axclrtEngineUnload(id);
            }
        }
    );

    // 2-2. create context
    uint64_t ctx = 0;
    if (const auto ret = axclrtEngineCreateContext(m.get(), &ctx); 0 != ret) {
        fprintf(stderr, "Create model{%s} context failed.\n", model_file.c_str());
        return false;
    }

    // 2-3. get the IO info
    auto info = utilities::scalar_guard<axclrtEngineIOInfo>(
        [&m]() {
            axclrtEngineIOInfo i;
            if (0 == axclrtEngineGetIOInfo(m.get(), &i)) {
                return i;
            }
            fprintf(stderr, "Get model io info failed.\n");
            return axclrtEngineIOInfo{};
        },
        [](const axclrtEngineIOInfo& i) {
            std::ignore = axclrtEngineDestroyIOInfo(i);
        }
    );

    // 2-4. get the count of shape group
    auto io = utilities::scalar_guard<axclrtEngineIO>(
        [&info]() {
            axclrtEngineIO i;
            if (0 == axclrtEngineCreateIO(info.get(), &i)) {
                return i;
            }
            fprintf(stderr, "Create model io failed.\n");
            return axclrtEngineIO{};
        },
        [](const axclrtEngineIO& i) {
            std::ignore = axclrtEngineDestroyIO(i);
        }
    );

    // 2-5. get the count of inputs
    uint32_t input_count = 0;
    if (input_count = axclrtEngineGetNumInputs(info.get()); 0 == input_count) {
        fprintf(stderr, "Get model input count failed.\n");
        return false;
    }

    // 2-6. get inputs size
    std::vector<uint32_t> inputs_size(input_count, 0);
    for (uint32_t i = 0; i < input_count; i++) {
        inputs_size[i] = axclrtEngineGetInputSizeByIndex(info.get(), 0, i);
    }

    // 2-7. malloc inputs
    auto inputs = utilities::vector_guard<void*>(
        [&input_count, &inputs_size]() {
            std::vector<void*> ptrs(input_count, nullptr);
            for (uint32_t i = 0; i < input_count; i++) {
                if (const auto ret = axclrtMalloc(&ptrs[i], inputs_size[i], axclrtMemMallocPolicy{}); 0 != ret) {
                    fprintf(stderr, "Memory allocation for input tensor{index: %d} failed{0x%08X}.\n", i, ret);
                }
            }
            return ptrs;
        },
        [](void* ptr) {
            if (nullptr != ptr) {
                std::ignore = axclrtFree(ptr);
                ptr = nullptr;
            }
        }
    );

    // 2-8. check the inputs buffer
    for (uint32_t i = 0; i < input_count; i++) {
        if (nullptr == inputs.get()[i]) {
            return false;
        }
    }

    // 2-9. set inputs buffer
    for (uint32_t i = 0; i < input_count; i++) {
        if (const auto ret = axclrtEngineSetInputBufferByIndex(io.get(), i, inputs.get()[i], inputs_size[i]); 0 != ret) {
            fprintf(stderr, "Set input buffer{index: %d} failed{0x%08X}.\n", i, ret);
            return false;
        }
    }

    // 2-10. get the count of outputs
    uint32_t output_count = 0;
    if (output_count = axclrtEngineGetNumOutputs(info.get()); 0 == output_count) {
        fprintf(stderr, "Get model output count failed.\n");
        return false;
    }

    // 2-11. get outputs size
    std::vector<uint32_t> outputs_size(output_count, 0);
    for (uint32_t i = 0; i < output_count; i++) {
        outputs_size[i] = axclrtEngineGetOutputSizeByIndex(info.get(), 0, i);
    }

    // 2-12. malloc outputs
    auto outputs = utilities::vector_guard<void*>(
        [&output_count, &outputs_size]() {
            std::vector<void*> ptrs(output_count, nullptr);
            for (uint32_t i = 0; i < output_count; i++) {
                if (const auto ret = axclrtMalloc(&ptrs[i], outputs_size[i], axclrtMemMallocPolicy{}); 0 != ret) {
                    fprintf(stderr, "Memory allocation for output tensor{index: %d} failed{0x%08X}.\n", i, ret);
                }
            }
            return ptrs;
        },
        [](void* ptr) {
            if (nullptr != ptr) {
                std::ignore = axclrtFree(ptr);
                ptr = nullptr;
            }
        }
    );

    // 2-13. check the outputs buffer
    for (uint32_t i = 0; i < output_count; i++) {
        if (nullptr == outputs.get()[i]) {
            return false;
        }
    }

    // 2-14. set outputs buffer
    for (uint32_t i = 0; i < output_count; i++) {
        if (const auto ret = axclrtEngineSetOutputBufferByIndex(io.get(), i, outputs.get()[i], outputs_size[i]); 0 != ret) {
            fprintf(stderr, "Set output buffer{index: %d} failed{0x%08X}.\n", i, ret);
            return false;
        }
    }

    // 3-0. read image
    cv::Mat src = cv::imread(image_file);
    if (src.empty()) {
        fprintf(stderr, "Read image failed.\n");
        return -1;
    }

    // 3-1. malloc input host buffer
    std::vector<uint8_t> input_buffer(input_size[1] * input_size[0] * 3, 0);
    auto dst = cv::Mat(cv::Size(input_size[1], input_size[0]), CV_8UC3, input_buffer.data());

    // 3-2. resize & swap rgb to bgr
    preprocess::sampler sampler(preprocess::crop_type::letterbox, preprocess::resize_type::linear, cmd.exist("swap-rb"));
    sampler(src, dst);

    // 3-3. send input to device
    if (const auto ret = axclrtMemcpy(inputs.get()[0], input_buffer.data(), input_buffer.size(), AXCL_MEMCPY_HOST_TO_DEVICE); 0 != ret) {
        fprintf(stderr, "Copy input data to device failed{0x%08X}.\n", ret);
        return false;
    }

    // 3-4. run model
    std::vector<float> run_costs(repeat, 0);
    for (int i = 0; i < repeat; ++i) {
        utilities::timer tick;
        const auto ret = axclrtEngineExecute(m.get(), ctx, 0, io.get());
        run_costs[i] = tick.elapsed();
        if ( 0 != ret) {
            fprintf(stderr, "Run model failed{0x%08X}.\n", ret);
            return false;
        }
    }

    // 3-5. get outputs
    std::vector<std::vector<float>> outputs_buffer(outputs_size.size());
    for (uint32_t i = 0; i < output_count; i++) {
        // outputs_buffer[i].resize(outputs_size[i] / sizeof(float));
        outputs_buffer[i].resize(outputs_size[i] / sizeof(float));
        if (const auto ret = axclrtMemcpy(outputs_buffer[i].data(), outputs.get()[i], outputs_size[i], AXCL_MEMCPY_DEVICE_TO_HOST); 0 != ret) {
            fprintf(stderr, "Copy output data failed{0x%08X}.\n", ret);
            return false;
        }
    }

    // 3-6. post process
    const auto prob_threshold = cmd.get<float>("prob");
    const auto iou_threshold = cmd.get<float>("iou");

    utilities::timer postprocess_timer;
    detection::anchor_based stride(std::size(MSCOCO_CLASSES), input_size[0], input_size[1], prob_threshold, iou_threshold);
    for (uint32_t i = 0; i < output_count; i++) {
        stride.proposal(outputs_buffer[i].data(), strides[i], anchors[i]);
    }
    std::vector<detection::object> objects = stride.pick(src.rows, src.cols);
    postprocess_timer.stop();

    // 3-7. print time cost
    fprintf(stdout, "post process cost time:%.2f ms \n", postprocess_timer.elapsed());
    fprintf(stdout, "--------------------------------------\n");
    auto total_time = std::accumulate(run_costs.begin(), run_costs.end(), 0.f);
    auto [min, max] = std::minmax_element(run_costs.begin(), run_costs.end());
    fprintf(stdout, "Repeat %d times, avg time %.2f ms, max_time %.2f ms, min_time %.2f ms\n",
            static_cast<int>(run_costs.size()), total_time / static_cast<float>(run_costs.size()), *max, *min);
    fprintf(stdout, "--------------------------------------\n");

    draw_objects(src, "yolov5s_out", objects);

    return 0;
}
