#pragma once
#include <string>
#include "yuv.hpp"

bool init_vdsp(const std::string& itcm_path, const std::string& sram_path);
bool split_rgb_to_nv12_by_vdsp(const image_frame& rgb, image_frame nv12[2], uint32_t algo);
bool merge_nv12_to_rgb_by_vdsp(const image_frame nv12[2], image_frame& rgb, uint32_t algo);
void deinit_vdsp();