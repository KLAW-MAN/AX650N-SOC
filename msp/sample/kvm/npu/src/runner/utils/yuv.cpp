#include "yuv.hpp"
#include <algorithm>
#include <cstring>
#include <fstream>
#include <stdexcept>
#include "ax_sys_api.h"
#include "sample_log.h"

bool image_frame::alloc() {
    AX_S32 ret = AX_SYS_MemAlloc((AX_U64*)&phy_addr, (AX_VOID**)&vir_addr, size, 128, NULL);
    if (0 != ret) {
        ALOGE("AX_SYS_MemAlloc() failed, ret: 0x%x", ret);
        return false;
    }

    return true;
}

void image_frame::free() {
    if (phy_addr) {
        AX_SYS_MemFree(phy_addr, vir_addr);
        phy_addr = 0;
        vir_addr = nullptr;
        size = 0;
    }
}

void image_frame::dump(const std::string& output_path) {
    std::ofstream ofs(output_path, std::ios::binary);
    ofs.write(reinterpret_cast<const char*>(vir_addr), size);
    ofs.close();
}

bool split_yuv444p_nv12(const std::vector<uint8_t>& yuv444p, std::vector<uint8_t>& nv12, std::vector<uint8_t>& uv12, uint32_t width,
                        uint32_t height) {
    const size_t nv12_size = (size_t)(width * height * 3 / 2);
    nv12.resize(nv12_size);
    uv12.resize(nv12_size);

    /* 01: split yuv444p to nv12 */
    const size_t y_size = (size_t)(width * height);
    const uint8_t* u = &yuv444p[y_size];
    const uint8_t* v = &yuv444p[y_size * 2];

    /* Y */
    std::memcpy(nv12.data(), yuv444p.data(), y_size);

    /* UV */
    uint8_t* uv = &nv12[y_size];
    uint32_t k = 0;
    for (uint32_t j = 0; j < height / 2; ++j) {
        for (uint32_t i = 0; i < width / 2; ++i) {
            /**
             *    U1   U2  U3  U4
             *    U5   U6  U7  U8
             *    U9  U10 U11 U12
             *   U13  U14 U15 U16
             *
             *    V1   V2  V3  V4
             *    V5   V6  V7  V8
             *    V9  V10 V11 V12
             *   V13  V14 V15 V16
             *
             *   UV = avg of 2x2
             *   uv[0] = (U1  +  U2 +  U5 +  U6) / 4
             *   uv[1] = (V1  +  V2 +  V5 +  V6) / 4
             *   uv[2] = (U3  +  U4 + U11 + U12) / 4
             *   uv[3] = (V3  +  V4 + V11 + V12) / 4
             *   uv[4] = (U9  + U10 + U13 + U14) / 4
             *   uv[5] = (V9  + V10 + V13 + V14) / 4
             *   uv[6] = (U11 + U12 + U15 + U16) / 4
             *   uv[7] = (V11 + V12 + V15 + V16) / 4
             */
            int pos1 = (j * 2) * width + (i * 2);
            int pos2 = (j * 2) * width + (i * 2 + 1);
            int pos3 = ((j * 2 + 1) * width) + (i * 2);
            int pos4 = ((j * 2 + 1) * width) + (i * 2 + 1);

            int avg_u = (u[pos1] + u[pos2] + u[pos3] + u[pos4]) / 4;
            int avg_v = (v[pos1] + v[pos2] + v[pos3] + v[pos4]) / 4;

            uv[k++] = std::clamp(avg_u, 0, 255);
            uv[k++] = std::clamp(avg_v, 0, 255);
        }
    }

    /* 01: split yuv444p to uv12 */

    /* Y = UUUU VVVV */
    uint8_t* yu = &uv12[0];
    uint8_t* yv = &uv12[y_size / 2];
    for (uint32_t j = 1; j < height; j += 2) {
        /**
         *    U1   U2  U3  U4
         *    U5   U6  U7  U8
         *    U9  U10 U11 U12
         *   U13  U14 U15 U16
         *
         *    V1   V2  V3  V4
         *    V5   V6  V7  V8
         *    V9  V10 V11 V12
         *   V13  V14 V15 V16
         *
         *  Y:
         *    U5   U6   U7   U8
         *   U13  U14  U15  U16
         *    V5   V6   V7   V8
         *   V13  V14  V15  V16
         *
         */
        std::memcpy(yu, &u[j * width], width);
        std::memcpy(yv, &v[j * width], width);
        yu += width;
        yv += width;
    }

    /* UV = UV UV UV */
    k = 0;
    uv = &uv12[y_size];
    for (uint32_t j = 0; j < height / 2; ++j) {
        for (uint32_t i = 0; i < width / 2; ++i) {
            /**
             *    U1   U2  U3  U4
             *    U5   U6  U7  U8
             *    U9  U10 U11 U12
             *   U13  U14 U15 U16
             *
             *    V1   V2  V3  V4
             *    V5   V6  V7  V8
             *    V9  V10 V11 V12
             *   V13  V14 V15 V16
             *
             * UV:
             *   U2   V2   U4   V4
             *  U10  V10  U12  V12
             */
            uv[k++] = u[(j * 2) * width + (i * 2 + 1)];
            uv[k++] = v[(j * 2) * width + (i * 2 + 1)];
        }
    }

    return true;
}

bool merge_nv12_yuv444p(std::vector<uint8_t>& yuv444p, const uint8_t* nv12, const uint8_t* uv12, uint32_t width, uint32_t height) {
    const size_t yuv444p_size = (size_t)(width * height * 3);
    yuv444p.resize(yuv444p_size);

    const size_t y_size = (size_t)(width * height);
    uint8_t* y = &yuv444p[0];
    uint8_t* u = &yuv444p[y_size];
    uint8_t* v = &yuv444p[y_size * 2];

    /* Y */
    std::memcpy(y, nv12, y_size);

    /* restore Y of uv12 to U and V of yuv444p */
    const uint8_t* uv12_yu = &uv12[0];
    const uint8_t* uv12_yv = &uv12[y_size / 2];
    for (uint32_t j = 0; j < height / 2; ++j) {
        std::memcpy(&u[((j * 2) + 1) * width], &uv12_yu[j * width], width);
        std::memcpy(&v[((j * 2) + 1) * width], &uv12_yv[j * width], width);
    }

    /* restore left U and V */
    const uint8_t* nv12_uv = &nv12[y_size];
    const uint8_t* uv12_uv = &uv12[y_size];
    for (uint32_t j = 0; j < height / 2; ++j) {
        for (uint32_t i = 0; i < width / 2; ++i) {
            int u1 = nv12_uv[j * width + i * 2];
            int u2 = uv12_uv[j * width + i * 2];
            int u5 = uv12_yu[j * width + i * 2];
            int u6 = uv12_yu[j * width + i * 2 + 1];
            int cu = u1 * 4 - u2 - u5 - u6;
            u[(j * 2) * width + (i * 2)] = std::clamp(cu, 0, 255);
            u[(j * 2) * width + (i * 2) + 1] = u2;

            int v1 = nv12_uv[j * width + i * 2 + 1];
            int v2 = uv12_uv[j * width + i * 2 + 1];
            int v5 = uv12_yv[j * width + i * 2];
            int v6 = uv12_yv[j * width + i * 2 + 1];
            int cv = v1 * 4 - v2 - v5 - v6;
            v[(j * 2) * width + (i * 2)] = std::clamp(cv, 0, 255);
            v[(j * 2) * width + (i * 2) + 1] = v2;
        }
    }

    return true;
}

bool load_image(const std::string& input, std::vector<uint8_t>& image, uint32_t size) {
    std::ifstream ifs(input, std::ios::binary);
    if (!ifs.is_open()) {
        ALOGE("open file fail: %s", input.c_str());
        return false;
    }

    image.resize(size);
    ifs.read(reinterpret_cast<char*>(image.data()), size);

    if (ifs.gcount() != size) {
        ALOGE("read file fail: %s", input.c_str());
        return false;
    }

    ifs.close();
    return true;
}

bool load_image(const std::string& input, uint32_t width, uint32_t height, uint32_t size, image_frame& frame) {
     std::ifstream ifs(input, std::ios::binary);
    if (!ifs.is_open()) {
        ALOGE("open file fail: %s", input.c_str());
        return false;
    }

    frame.width = width;
    frame.height = height;
    frame.size = size;
    if (!frame.alloc()) {
        return false;
    }

    ifs.read(reinterpret_cast<char*>(frame.vir_addr), size);
    if (ifs.gcount() != size) {
        ALOGE("read file fail: %s", input.c_str());
        return false;
    }

    ifs.close();
    return true;
}

std::array<std::vector<uint8_t>, 4> split_yuv444p_to_4parts(const std::vector<uint8_t>& yuv444p, uint32_t width, uint32_t height) {
    std::array<std::vector<uint8_t>, 4> subs;

    const uint8_t* y = yuv444p.data();
    const uint8_t* u = yuv444p.data() + width * height;
    const uint8_t* v = yuv444p.data() + width * height * 2;
    const uint32_t size = width * height / 4;

    for (uint32_t i = 0; i < 4; ++i) {
        subs[i].resize(size * 3);

        std::memcpy(subs[i].data(), y + i * size, size);
        std::memcpy(subs[i].data() + size, u + i * size, size);
        std::memcpy(subs[i].data() + size * 2, v + i * size, size);
    }

    return subs;
}

void yuv444p_to_rgb888_bt709_full_range(const uint8_t* yuv444p, uint8_t* rgb888, uint32_t width, uint32_t height) {
    // BT709 full range
    // R = Y + 1.5748 * (V - 128)
    // G = Y - 0.1873 * (U - 128) - 0.4681 * (V - 128)
    // B = Y + 1.8556 * (U - 128)

    const uint32_t pixels = width * height;
    const uint32_t y_size = pixels;
    const uint32_t u_size = pixels;

    const uint8_t* y_plane = yuv444p;
    const uint8_t* u_plane = yuv444p + y_size;
    const uint8_t* v_plane = yuv444p + y_size + u_size;

    for (uint32_t i = 0; i < pixels; ++i) {
        int y = y_plane[i];
        int u = u_plane[i] - 128.0f;
        int v = v_plane[i] - 128.0f;

        float r = y + 1.5748f * v;
        float g = y - 0.1873f * u - 0.4681f * v;
        float b = y + 1.8556f * u;

        rgb888[i * 3 + 0] = static_cast<uint8_t>(std::clamp(r, 0.0f, 255.0f));
        rgb888[i * 3 + 1] = static_cast<uint8_t>(std::clamp(g, 0.0f, 255.0f));
        rgb888[i * 3 + 2] = static_cast<uint8_t>(std::clamp(b, 0.0f, 255.0f));
    }
}