/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#include "main.hpp"
#include "config.hpp"

#include "version.hpp"
#include "utilities/timer.hpp"

#include "middleware/runner.hpp"

#if defined(CONFIG_AXCL_API)
#include "middleware/axcl_runtime_runner.hpp"
#include "middleware/axcl_native_runner.hpp"
#endif
#if defined(ENV_AX_LOCAL_API_ENABLE)
#include "middleware/ax_npu_runner.hpp"
#endif

#include <memory>

int main(const int argc, char* argv[])
{
    common::config cfg(argc, argv);
    if (!cfg.check()) {
        return -1;
    }

    std::unique_ptr<middleware::runner> runner;

#if defined(CONFIG_AXCL_API)
    if (0 == cfg.api) {
        runner = std::make_unique<middleware::runtime_runner>();
    }
    if (1 == cfg.api) {
        runner = std::make_unique<middleware::native_runner>();
    }
#if defined(ENV_AX_LOCAL_API_ENABLE)
    if (2 == cfg.api) {
        runner = std::make_unique<middleware::npu_runner>();
    }
#endif
#elif defined(ENV_AX_LOCAL_API_ENABLE)
    runner = std::make_unique<middleware::npu_runner>()
#else
#error "No NPU API is enabled."
#endif

    if (!runner->init(cfg.config_file, cfg.device_index, cfg.vnpu_mode)) {
        fprintf(stderr, "[ERROR] Init failed.\n");
        return -1;
    }

    if (!runner->load(cfg.model_path)) {
        fprintf(stderr, "[ERROR] Loading model {%s} failed.\n", cfg.model_path.c_str());
        return -1;
    }

    if (cfg.has_group && cfg.group >= runner->get_shape_group_count()) {
        fprintf(stderr, "[ERROR] Selected shape group index {%d vs. %ju} is out of range.\n", cfg.group, runner->get_shape_group_count());
        return -1;
    }

    if (!runner->prepare(cfg.feed_mode, cfg.feed_mode, cfg.group, cfg.batch)) {
        fprintf(stderr, "[ERROR] Prepare for model {%s} failed.\n", cfg.model_path.c_str());
        return -1;
    }

    std::vector<float> elapsed_times;
    float cost_min = 0.f, cost_max = 0.f, cost_average = 0.f;

    auto run = [&runner, &cfg, &elapsed_times]() {
        for (uint32_t i = 0; i < cfg.warmup_count; i++) {
            if (!runner->run(cfg.parallel_mode)) {
                fprintf(stderr, "[ERROR] Warmup model {%s} failed.\n", cfg.model_path.c_str());
                return -1;
            }
            if (cfg.has_sleep_times) {
                runner->sleep_for(cfg.sleep_times);
            }
        }
        for (uint32_t i = 0; i < cfg.repeat_count; i++) {
            utilities::timer timer;
            const auto flag = runner->run(cfg.parallel_mode);
            timer.stop();
            if (!flag) {
                fprintf(stderr, "[ERROR] Run model {%s} failed.\n", cfg.model_path.c_str());
                return -1;
            }
            elapsed_times.push_back(timer.elapsed<utilities::timer::milliseconds>());
            if (cfg.has_sleep_times) {
                runner->sleep_for(cfg.sleep_times);
            }
        }
        return 0;
    };

    if (!cfg.feed_mode) {
        fprintf(stdout, "   Run AxModel:\n");
        fprintf(stdout, "         model: %s\n", cfg.model_path.c_str());
        fprintf(stdout, "          type: %s\n", common::get_model_type_string(runner->get_model_type()).c_str());
        fprintf(stdout, "          vnpu: %s\n", common::get_visual_mode_string(runner->get_npu_type()).c_str());
        fprintf(stdout, "        warmup: %d\n", cfg.warmup_count);
        fprintf(stdout, "        repeat: %d\n", cfg.repeat_count);
        if (cfg.has_sleep_times) {
            fprintf(stdout, "         sleep: %dms\n", cfg.sleep_times);
        }
        if (0 == cfg.batch) {
            fprintf(stdout, "         batch: { auto: %d }\n", runner->get_batch_size());
        }
        else {
            fprintf(stdout, "         batch: %d\n", runner->get_batch_size());
        }
        if (cfg.has_group || 1 < cfg.group) {
            fprintf(stdout, "  shape groups: %d/%ju\n", cfg.group + 1, runner->get_shape_group_count());
        }

        fprintf(stdout, "    axclrt ver: %s\n", runner->get_library_version().c_str());
        fprintf(stdout, "   pulsar2 ver: %s\n", runner->get_model_version().c_str());
        fprintf(stdout, "      tool ver: %s\n", VERSION_STRING);
        fprintf(stdout, "      cmm size: %ju Bytes\n", runner->get_cmm_usage());
        fflush(stdout);

        elapsed_times.reserve(cfg.repeat_count);
        if (const auto ret = run(); 0 != ret) {
            return ret;
        }
        display_costs(elapsed_times, cost_min, cost_max, cost_average);
    }
    else {
        bool verified = false, saved = false;

        const auto list = middleware::runner::read(cfg.list_file);
        if (list.empty()) {
            fprintf(stderr, "[ERROR] Read stimulus list{%s} failed.\n", cfg.input_folder.c_str());
            return -1;
        }

        elapsed_times.reserve(cfg.repeat_count * list.size());
        for (const auto& file : list) {
            if (!runner->feed(cfg.input_folder, file)) {
                fprintf(stderr, "[ERROR] Feed stimulus failed.\n");
                return -1;
            }

            if (const auto ret = run(); 0 != ret) {
                return ret;
            }

            if (cfg.has_verify_mode_flag) {
                verified = runner->verify(cfg.output_folder, file);
            } else {
                if (saved = runner->save(cfg.output_folder, file); !saved) {
                    fprintf(stderr, "[ERROR] Save stimulus failed.\n");
                }
            }
        }

        if (!elapsed_times.empty()) {
            display_costs(elapsed_times, cost_min, cost_max, cost_average);
        }

        if (cfg.has_verify_mode_flag && !verified) {
            fprintf(stderr, "[ERROR] Verify ground truth failed.\n");
            fflush(stderr);
            return -1;
        }

        if (!cfg.has_verify_mode_flag && !saved) {
            fprintf(stderr, "[ERROR] Save output failed.\n");
            fflush(stderr);
            return -1;
        }
    }

    return 0;
}
