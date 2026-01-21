#pragma once

#include <ax_engine_api.h>
#include <ax_sys_api.h>
#include "ax_model_runner/ax_model_runner_ax650.hpp"
#include "utils/sample_log.h"
#include "utils/timer.hpp"
#include "utils/yuv.hpp"

class model {
private:
    ax_runner_ax650 m_model;

public:
    bool init(std::string axmodel_path) {
        if (int ret = m_model.init(axmodel_path.c_str(), false); ret != 0) {
            ALOGE("load %s, ret: %d, 0x%x", axmodel_path.c_str(), ret, (uint32_t)ret);
            return false;
        }

        return true;
    }

    void deinit() {
        m_model.release();
    }

    bool inference(const image_frame &nv12, const image_frame &uv12, uint32_t algo, const image_frame &output) {
        if (algo == 0) {
             /* input nv12 */
             unsigned int yv0_index = m_model.get_input("yv0").nIdx;
             m_model.update_input_io_data(yv0_index, nv12.phy_addr, nv12.vir_addr);

             unsigned int uv1_index = m_model.get_input("uv1").nIdx;
             m_model.update_input_io_data(uv1_index, uv12.phy_addr, uv12.vir_addr);
        } else {
            /* input nv12 */
            uint64_t input_nv12_y_phy = nv12.phy_addr;
            void *input_nv12_y_vir = nv12.vir_addr;
            uint32_t input_nv12_y_size = nv12.width * nv12.height;
            unsigned int input_nv12_y_index = m_model.get_input("y").nIdx;
            m_model.update_input_io_data(input_nv12_y_index, input_nv12_y_phy, input_nv12_y_vir);

            uint64_t input_nv12_uv_phy = input_nv12_y_phy + input_nv12_y_size;
            void *input_nv12_uv_vir = (void *)((uint64_t)input_nv12_y_vir + input_nv12_y_size);
            unsigned int input_nv12_uv_index = m_model.get_input("uv").nIdx;
            m_model.update_input_io_data(input_nv12_uv_index, input_nv12_uv_phy, input_nv12_uv_vir);

            /* input uv12 */
            uint64_t input_uv12_phy = uv12.phy_addr;
            void *input_uv12_vir = uv12.vir_addr;
            unsigned int input_uv12_index = m_model.get_input("uv_left").nIdx;
            m_model.update_input_io_data(input_uv12_index, input_uv12_phy, input_uv12_vir);
        }

        /* output rgb */
        uint64_t output_rgb_phy = output.phy_addr;
        void *output_rgb_vir = output.vir_addr;
        unsigned int output_rgb_index = m_model.get_output("rgb").nIdx;
        m_model.update_output_io_data(output_rgb_index, output_rgb_phy, output_rgb_vir);

        /* npu */
        if (int ret = m_model.inference(); ret != 0) {
            ALOGE("npu inference fail, ret = %d, 0x%x", ret, (uint32_t)ret);
            return false;
        }

        return true;
    }
};