#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

struct image_frame {
    uint64_t phy_addr = 0;
    void* vir_addr = nullptr;
    uint32_t width = 0;
    uint32_t height = 0;
    uint32_t size = 0;

    bool alloc();
    void free();
    void dump(const std::string& output_path);
};

bool load_image(const std::string& input, std::vector<uint8_t>& image, uint32_t size);
bool load_image(const std::string& input, uint32_t width, uint32_t height, uint32_t size, image_frame& frame);
std::array<std::vector<uint8_t>, 4> split_yuv444p_to_4parts(const std::vector<uint8_t>& yuv444p, uint32_t width, uint32_t height);
bool split_yuv444p_nv12(const std::vector<uint8_t>& yuv444p, std::vector<uint8_t>& nv12, std::vector<uint8_t>& uv12, uint32_t width, uint32_t height);
bool merge_nv12_yuv444p(std::vector<uint8_t>& yuv444p, const uint8_t* nv12, const uint8_t* uv12, uint32_t width, uint32_t height);
void yuv444p_to_rgb888_bt709_full_range(const uint8_t* yuv444p, uint8_t* rgb888, uint32_t width, uint32_t height);