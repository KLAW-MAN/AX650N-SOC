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

#include <cmdline.h>
#include <utilities/file.hpp>

#include "common/types.hpp"

#if defined(ENV_CHIP_SERIES_MC50) || defined(ENV_AXCL_RUNTIME_API_ENABLE) || defined(ENV_AXCL_NATIVE_API_ENABLE)
#define CONFIG_MC50
#endif
#if defined(ENV_CHIP_SERIES_MC20E)
#define CONFIG_MC20E
#endif
#if defined(ENV_AXCL_RUNTIME_API_ENABLE) || defined(ENV_AXCL_NATIVE_API_ENABLE)
#define CONFIG_AXCL_API
#endif

namespace common {

constexpr char MODEL_PATH_STR[] = "model";
constexpr char REPEAT_COUNT_STR[] = "repeat";
constexpr char WARMUP_COUNT_STR[] = "warmup";
constexpr char SLEEP_TIMES_STR[] = "sleep";
constexpr char VISUAL_NPU_MODE_STR[] = "vnpu";
constexpr char AFFINITY_STR[] = "affinity";
constexpr char PARALLEL_MODE_STR[] = "parallel";
constexpr char BATCH_STR[] = "batch";
constexpr char SHAPE_GROUP_STR[] = "group";
constexpr char INPUT_FOLDER_STR[] = "input-folder";
constexpr char OUTPUT_FOLDER_STR[] = "output-folder";
constexpr char LIST_FILE_STR[] = "list";
constexpr char CONFIG_PATH_STR[] = "config";
constexpr char DEVICE_INDEX_STR[] = "device";
constexpr char VERIFY_MODE_STR[] = "verify";
constexpr char CONFIG_FILE_DEFAULT[] = "/usr/local/axcl/axcl.json";

#if defined(CONFIG_MC50)
constexpr char VISUAL_NPU_HELP_STR[] = "type of Visual-NPU inited {0=Disable, 1=STD, 2=BigLittle, 3=LittleBig}";
constexpr int VNPU_MODE_MAX = 3;
constexpr int AFFINITY_MAX = 0b111;
#endif

#if defined(CONFIG_MC20E)
constexpr char VISUAL_NPU_HELP_STR[] = "type of Visual-NPU inited {0=Disable, 1=ENABLE}";
constexpr int VNPU_MODE_MAX = 1;
constexpr int AFFINITY_MAX = 0b11;
#endif

struct config {
    config() = delete;

    explicit config(const int argc, char* argv[]) {
        cmdline::parser cmd;
        cmd.add<std::string>(MODEL_PATH_STR, 'm', "path to a model file", true);

        cmd.add<int>(REPEAT_COUNT_STR, 'r', "repeat times running a model", false, 1, cmdline::range(1, INT32_MAX));
        cmd.add<int>(WARMUP_COUNT_STR, 'w', "repeat times before running a model to warming up", false, 1, cmdline::range(0, INT32_MAX));
        cmd.add<int>(SLEEP_TIMES_STR, 's', "sleep millisecond after running a model once", false, 0, cmdline::range(0, INT32_MAX));

#if defined(CONFIG_MC50) || defined(CONFIG_MC20E)
        cmd.add<int>(VISUAL_NPU_MODE_STR, 'v', VISUAL_NPU_HELP_STR, false, 0, cmdline::range(0, VNPU_MODE_MAX));
        cmd.add<int>(AFFINITY_STR, 'a', "npu affinity when running a model", false, AFFINITY_MAX, cmdline::range(1, AFFINITY_MAX));
#endif

#if defined(CONFIG_MC50)
        cmd.add<int>(PARALLEL_MODE_STR, 'p', "parallel run model using all affinity npu cores", false, 0, cmdline::range(0, 1));
#endif

        cmd.add<int>(BATCH_STR, 'b', "the batch will running", false, 0, cmdline::range(0, 100));
        cmd.add<int>(SHAPE_GROUP_STR, 'g', "the selected group of shapes", false, 0);

        cmd.add<std::string>(INPUT_FOLDER_STR, 'i', "the folder of each inputs (folders) located", false);
        cmd.add<std::string>(OUTPUT_FOLDER_STR, 'o', "the folder of each outputs (folders) will saved in", false);
        cmd.add<std::string>(LIST_FILE_STR, 'l', "the list of inputs which will test", false);

#if defined(CONFIG_AXCL_API)
        cmd.add<std::string>(CONFIG_PATH_STR, 'c', R"(axcl config file "axcl.json" path)", false, CONFIG_FILE_DEFAULT);
        cmd.add<int>(DEVICE_INDEX_STR, 'd', "axcl device index", false, 0, cmdline::range(0, 32 - 1));
#endif

#if defined(CONFIG_AXCL_API) && defined(ENV_AX_LOCAL_API_ENABLE)
        cmd.add<int>("api", 'x', R"(api, 0="axcl runtime", 1="axcl native", 2="ax local engine")", false, 0, cmdline::range(0, 2));
#elif defined(CONFIG_AXCL_API) && !defined(ENV_AX_LOCAL_API_ENABLE)
        cmd.add<int>("api", 'x', R"(api, 0="axcl runtime", 1="axcl native")", false, 0, cmdline::range(0, 1));
#endif

        cmd.add(VERIFY_MODE_STR, 0, "verify outputs after running model");

        cmd.parse_check(argc, argv);

        this->model_path = cmd.get<std::string>(MODEL_PATH_STR);

        this->has_repeat_count = cmd.exist(REPEAT_COUNT_STR);
        this->repeat_count = cmd.get<int>(REPEAT_COUNT_STR);

        this->has_warmup_count = cmd.exist(WARMUP_COUNT_STR);
        this->warmup_count = cmd.get<int>(WARMUP_COUNT_STR);

        this->has_sleep_times = cmd.exist(SLEEP_TIMES_STR);
        this->sleep_times = cmd.get<int>(SLEEP_TIMES_STR);

#if defined(CONFIG_MC50) || defined(CONFIG_MC20E)
        this->has_vnpu_mode = cmd.exist(VISUAL_NPU_MODE_STR);
        this->vnpu_mode = cmd.get<int>(VISUAL_NPU_MODE_STR);

        this->has_affinity = cmd.exist(AFFINITY_STR);
        this->affinity = cmd.get<int>(AFFINITY_STR);
#endif

#if defined(CONFIG_MC50)
        this->has_parallel_mode = cmd.exist(PARALLEL_MODE_STR);
        this->parallel_mode = (0 != cmd.get<int>(PARALLEL_MODE_STR));
#endif

        this->has_batch = cmd.exist(BATCH_STR);
        this->batch = cmd.get<int>(BATCH_STR);

        this->has_group = cmd.exist(SHAPE_GROUP_STR);
        this->group = cmd.get<int>(SHAPE_GROUP_STR);

        this->has_input_folder = cmd.exist(INPUT_FOLDER_STR);
        this->input_folder = cmd.get<std::string>(INPUT_FOLDER_STR);
        this->has_output_folder = cmd.exist(OUTPUT_FOLDER_STR);
        this->output_folder = cmd.get<std::string>(OUTPUT_FOLDER_STR);
        this->has_list_file = cmd.exist(LIST_FILE_STR);
        this->list_file = cmd.get<std::string>(LIST_FILE_STR);

#if defined(CONFIG_AXCL_API)
        this->has_config_file = cmd.exist(CONFIG_PATH_STR);
        this->config_file = cmd.get<std::string>(CONFIG_PATH_STR);
        this->has_device_index = cmd.exist(DEVICE_INDEX_STR);
        this->device_index = cmd.get<int>(DEVICE_INDEX_STR);
        this->has_api = cmd.exist("api");
        this->api = cmd.get<int>("api");
#endif

        this->has_verify_mode_flag = cmd.exist(VERIFY_MODE_STR);
    }

    [[nodiscard]] bool check() {
        if (!utilities::exists(this->model_path) && !utilities::is_regular_file(this->model_path)) {
            fprintf(stderr, "[ERROR] Model file {%s} is not exists.\n", this->model_path.c_str());
            return false;
        }

#if defined(CONFIG_MC50)
        if (this->has_affinity) {
            if (2 <= this->vnpu_mode && 0 < (this->affinity & 0b100)) {
                fprintf(stderr, "[ERROR] Model affinity cannot be set as { %s } with visual npu mode { %s }.\n", "0b1XX",
                        get_visual_mode_string(this->vnpu_mode).c_str());
                return false;
            }
            if (0 == this->vnpu_mode && 0 < (this->affinity & 0b110)) {
                fprintf(stderr, "[ERROR] Model affinity cannot be set as { %s } with visual npu mode { %s }.\n", "0b11X",
                        get_visual_mode_string(this->vnpu_mode).c_str());
                return false;
            }
        } else {
            if (1 == this->vnpu_mode) {
                this->affinity = AFFINITY_MAX;
            } else {
                this->affinity = 0b1;
            }
        }
#endif

#if defined(CONFIG_MC20E)
        if (this->has_affinity) {
            if (0 == this->vnpu_mode && 0 < (this->affinity & 0b10)) {
                fprintf(stderr, "[ERROR] Model affinity cannot be set as { %s } with visual npu mode { %s }.\n", "0b1X",
                        get_visual_mode_string(this->vnpu_mode).c_str());
                return false;
            }
        } else {
            if (1 == this->vnpu_mode) {
                this->affinity = AFFINITY_MAX;
            } else {
                this->affinity = 0b1;
            }
        }
#endif

        // feed mode: feed inputs, run model, get outputs
        this->feed_mode = this->has_input_folder || this->has_output_folder || this->has_list_file;

        // check feeding mode params are all specified
        if (this->feed_mode) {
            if (!this->has_input_folder) {
                fprintf(stderr, "[ERROR] Must have option --%s when wants feed data and gets inference result.\n", INPUT_FOLDER_STR);
            }
            if (!this->has_output_folder) {
                fprintf(stderr, "[ERROR] Must have option --%s when wants feed data and gets inference result.\n", OUTPUT_FOLDER_STR);
            }
            if (!this->has_list_file) {
                fprintf(stderr, "[ERROR] Must have option --%s when wants feed data and gets inference result.\n", LIST_FILE_STR);
            }
            fflush(stderr);

            if (!this->has_input_folder || !this->has_output_folder || !this->has_list_file) {
                fprintf(stderr, "[ERROR] One of these options { --%s, --%s, --%s } be specified, others must be specified also.\n",
                        INPUT_FOLDER_STR, OUTPUT_FOLDER_STR, LIST_FILE_STR);
                return false;
            }

            if (this->has_warmup_count || this->has_repeat_count) {
                fprintf(stderr, "[WARNING] One of options { --%s, --%s } be specified, and may cause huge running time costs.\n",
                        WARMUP_COUNT_STR, REPEAT_COUNT_STR);
            }
            fflush(stderr);
        }

        if (!this->feed_mode) {
            if (this->has_verify_mode_flag) {
                fprintf(
                    stderr,
                    "[WARNING] Option { --%s } will be ignored, this option only takes effect when options { --%s, --%s, --%s } be specified.\n",
                    VERIFY_MODE_STR, INPUT_FOLDER_STR, OUTPUT_FOLDER_STR, LIST_FILE_STR);
            }
            fflush(stderr);
        }

        if (this->feed_mode) {
            if (!utilities::exists(this->list_file) && !utilities::is_regular_file(this->list_file)) {
                fprintf(stderr, "[ERROR] Model input list {%s} is not exists.\n", this->list_file.c_str());
                return false;
            }
            if (!utilities::exists(this->input_folder) && !utilities::is_directory(this->input_folder)) {
                fprintf(stderr, "[ERROR] Model input folder {%s} is not exists.\n", this->input_folder.c_str());
                return false;
            }
            if (!utilities::exists(this->output_folder) && !utilities::is_directory(this->output_folder)) {
                fprintf(stderr, "[WARNING] Model output folder {%s} is not exists.\n", this->output_folder.c_str());
                if (!utilities::create_directory(this->output_folder)) {
                    fprintf(stderr, "[ERROR] Create output folder {%s} failed.\n", this->output_folder.c_str());
                    fflush(stderr);
                    return false;
                }
            }
        }

        return true;
    }

    std::string model_path;

    bool feed_mode = false;

    bool has_repeat_count = false;
    uint32_t repeat_count = 0;

    bool has_warmup_count = false;
    uint32_t warmup_count = 0;

    bool has_sleep_times = false;
    uint32_t sleep_times = 0;

    bool has_vnpu_mode = false;
    uint32_t vnpu_mode = 0;

    bool has_affinity = false;
    uint32_t affinity = 0;

    bool has_parallel_mode = false;
    bool parallel_mode = false;

    bool has_batch = false;
    uint32_t batch = 0;

    bool has_group = false;
    uint32_t group = 0;

    bool has_input_folder = false;
    std::string input_folder;
    bool has_output_folder = false;
    std::string output_folder;
    bool has_list_file = false;
    std::string list_file;

    bool has_verify_mode_flag = false;

    bool has_config_file = false;
    std::string config_file;
    bool has_device_index = false;
    int32_t device_index = 0;
    bool has_api = false;
    int32_t api;
};

}
