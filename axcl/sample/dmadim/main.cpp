/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include "../utils/image_320x240_nv12.h"
#include "../utils/image_640x480_nv12.h"
#include "../utils/logger.h"
#include "axcl.h"
#include "cmdline.h"

#define IS_AXCL_UT_IMAGE(image) (0 == strcmp(image, "axcl.ut.image"))
static int dma_copy();
static int dma_memset();
static int dma_checksum();
static int dma_copy2d(const char *image, uint32_t width, uint32_t height);

int main(int argc, char *argv[]) {
    SAMPLE_LOG_I("============== %s sample started %s %s ==============\n", AXCL_BUILD_VERSION, __DATE__, __TIME__);

    cmdline::parser a;
    a.add<uint32_t>("device", 'd', "device index from 0 to connected device num - 1", false, 0, cmdline::range(0, AXCL_MAX_DEVICE_COUNT - 1));
    a.add<std::string>("image", 'i', "nv12 image file path", true);
    a.add<uint32_t>("width", 'w', "width of nv12 image", true);
    a.add<uint32_t>("height", 'h', "height of nv12 image", true);
    a.add<std::string>("json", '\0', "axcl.json path", false, "./axcl.json");
    a.parse_check(argc, argv);
    const uint32_t device_index = a.get<uint32_t>("device");
    const std::string image = a.get<std::string>("image");
    uint32_t width = a.get<uint32_t>("width");
    uint32_t height = a.get<uint32_t>("height");
    const std::string json = a.get<std::string>("json");

    axclError ret;
    SAMPLE_LOG_I("json: %s", json.c_str());
    ret = axclInit(json.c_str());
    if (AXCL_SUCC != ret) {
        return 1;
    }

    axclrtDeviceList device_list;
    if (axclError ret = axclrtGetDeviceList(&device_list); AXCL_SUCC != ret || 0 == device_list.num) {
        SAMPLE_LOG_E("no device is connected");
        axclFinalize();
        return ret;
    }

    if (device_index >= device_list.num) {
        SAMPLE_LOG_E("device index %d is out of connected device num %d", device_index, device_list.num);
        axclFinalize();
        return 1;
    }

    const int32_t device_id = device_list.devices[device_index];
    SAMPLE_LOG_I("device index: %d, bus number: %d", device_index, device_id);

    ret = axclrtSetDevice(device_id);
    if (AXCL_SUCC != ret) {
        axclFinalize();
        return 1;
    }

    dma_copy();
    dma_memset();
    dma_checksum();
    dma_copy2d(image.c_str(), width, height);

    axclrtResetDevice(device_id);
    axclFinalize();

    SAMPLE_LOG_I("============== %s sample exited %s %s ==============\n", AXCL_BUILD_VERSION, __DATE__, __TIME__);
    return 0;
}

static int dma_copy() {
    axclError ret;
    void *dev_mem[2] = {nullptr, nullptr};
    constexpr uint32_t SIZE = 4 * 1024 * 1024;

    if (ret = axclrtMalloc(&dev_mem[0], SIZE, AXCL_MEM_MALLOC_NORMAL_ONLY); AXCL_SUCC != ret) {
        SAMPLE_LOG_E("alloc device 0x%x bytes memory fail, ret = 0x%x", SIZE, ret);
        return ret;
    }

    if (ret = axclrtMemset(dev_mem[0], 0xAA, SIZE); AXCL_SUCC != ret) {
        SAMPLE_LOG_E("memset device memory %p to 0xAA, ret = 0x%x", dev_mem[0], ret);
        axclrtFree(dev_mem[0]);
        return ret;
    }

    if (ret = axclrtMalloc(&dev_mem[1], SIZE, AXCL_MEM_MALLOC_NORMAL_ONLY); AXCL_SUCC != ret) {
        SAMPLE_LOG_E("alloc device 0x%x bytes memory fail, ret = 0x%x", SIZE, ret);
        axclrtFree(dev_mem[0]);
        return ret;
    }

    if (ret = AXCL_DMA_MemCopy(reinterpret_cast<AX_U64>(dev_mem[1]), reinterpret_cast<AX_U64>(dev_mem[0]), SIZE); AXCL_SUCC != ret) {
        SAMPLE_LOG_E("dma copy device memory from %p to %p fail, ret = 0x%x", dev_mem[0], dev_mem[1], ret);
        axclrtFree(dev_mem[0]);
        axclrtFree(dev_mem[1]);
        return ret;
    }

    if (ret = axclrtMemcmp(dev_mem[0], dev_mem[1], SIZE); 0 != ret) {
        SAMPLE_LOG_E("compare device memory %p with %p fail, ret = 0x%x", dev_mem[0], dev_mem[1], ret);
    }

    SAMPLE_LOG_I("dma copy device memory succeed, from %p to %p", dev_mem[0], dev_mem[1]);
    axclrtFree(dev_mem[0]);
    axclrtFree(dev_mem[1]);
    return ret;
}

static int dma_memset() {
    axclError ret;
    void *dev_mem = nullptr;
    constexpr uint32_t SIZE = 4 * 1024 * 1024;
    constexpr uint8_t VALUE = 0xAB;

    if (ret = axclrtMalloc(&dev_mem, SIZE, AXCL_MEM_MALLOC_NORMAL_ONLY); AXCL_SUCC != ret) {
        SAMPLE_LOG_E("alloc device 0x%x bytes memory fail, ret = 0x%x", SIZE, ret);
        return ret;
    }

    if (ret = AXCL_DMA_MemSet(reinterpret_cast<AX_U64>(dev_mem), VALUE, SIZE); AXCL_SUCC != ret) {
        SAMPLE_LOG_E("dma memset device memory %p to 0x%x fail, ret = 0x%x", dev_mem, VALUE, ret);
    } else {
        SAMPLE_LOG_I("dma memset device memory succeed, %p to 0x%x", dev_mem, VALUE);
    }

    axclrtFree(dev_mem);
    return ret;
}

static int dma_checksum() {
    axclError ret;
    void *dev_mem = nullptr;
    constexpr uint32_t SIZE = 4 * 1024 * 1024;

    if (ret = axclrtMalloc(&dev_mem, SIZE, AXCL_MEM_MALLOC_NORMAL_ONLY); AXCL_SUCC != ret) {
        SAMPLE_LOG_E("alloc device 0x%x bytes memory fail, ret = 0x%x", SIZE, ret);
        return ret;
    }

    if (ret = axclrtMemset(dev_mem, 0xAA, SIZE); AXCL_SUCC != ret) {
        SAMPLE_LOG_E("memset device memory %p to 0xAA, ret = 0x%x", dev_mem, ret);
        axclrtFree(dev_mem);
        return ret;
    }

    AX_U32 chksum;
    if (ret = AXCL_DMA_CheckSum(&chksum, reinterpret_cast<AX_U64>(dev_mem), SIZE); AXCL_SUCC != ret) {
        SAMPLE_LOG_E("dma checksum fail, ret = 0x%x", ret);
    } else {
        SAMPLE_LOG_I("dma checksum succeed, checksum = 0x%x", chksum);
    }

    axclrtFree(dev_mem);
    return ret;
}

static uint32_t load_image_to_device(const char *image, uint32_t image_size, void *&dev_mem) {
    uint32_t fsize;
    FILE *fp = NULL;
    bool load_image = !IS_AXCL_UT_IMAGE(image);
    if (!load_image) {
        fsize = image_size;
    } else {
        fp = fopen(image, "rb");
        if (!fp) {
            SAMPLE_LOG_E("fopen %s fail, %s", image, strerror(errno));
            return 0;
        }

        fseek(fp, 0, SEEK_END);
        fsize = ftell(fp);
        fseek(fp, 0, SEEK_SET);

        if (fsize != image_size) {
            SAMPLE_LOG_E("image %s is not nv12 image", image);
            fclose(fp);
            return 0;
        }
    }

    void *mem;
    axclrtMallocHost(&mem, fsize);

    if (load_image) {
        size_t sz = fread(mem, 1, fsize, fp);
        fclose(fp);

        if (sz != (size_t)fsize) {
            axclrtFreeHost(mem);
            SAMPLE_LOG_E("read %zd size from image, target size is %d", sz, fsize);
            return 0;
        }
    } else {
        memcpy(mem, image_640x480_nv12_yuv, fsize);
    }

    if (axclError ret = axclrtMalloc(&dev_mem, fsize, AXCL_MEM_MALLOC_NORMAL_ONLY); AXCL_SUCC != ret) {
        SAMPLE_LOG_E("alloc device 0x%x bytes memory fail, ret = 0x%x", fsize, ret);
        axclrtFreeHost(mem);
        return 0;
    }

    if (axclError ret = axclrtMemcpy(dev_mem, mem, fsize, AXCL_MEMCPY_HOST_TO_DEVICE); AXCL_SUCC != ret) {
        SAMPLE_LOG_E("memcpy host memory %p to device memory %p fail, ret = 0x%x", mem, dev_mem, ret);
        axclrtFreeHost(mem);
        axclrtFree(dev_mem);
        return 0;
    }

    axclrtFreeHost(mem);
    return fsize;
}

static int dma_copy2d(const char *image, uint32_t width, uint32_t height) {
    /**
     * crop (0, 0, width/2, height/2) from nv12 by dma
     */
    if (IS_AXCL_UT_IMAGE(image)) {
        width = 640;
        height = 480;
    }

    void *image_dev_mem;
    uint32_t image_size = width * height * 3 / 2;
    if (image_size != load_image_to_device(image, image_size, image_dev_mem)) {
        return -1;
    }

    axclError ret;

    uint32_t dst_size[2];
    AX_DMADIM_DESC_XD_T desc[2] = {0};
    void *host_mem[2] = {nullptr, nullptr};

    /* Y */
    desc[0].tSrcInfo.u32Imgw = width / 2;
    desc[0].tSrcInfo.u32Stride[0] = width;
    desc[0].tSrcInfo.u64PhyAddr = reinterpret_cast<uint64_t>(image_dev_mem);
    desc[0].tDstInfo.u32Imgw = width / 2;
    desc[0].tDstInfo.u32Stride[0] = desc[0].tDstInfo.u32Imgw;
    desc[0].u16Ntiles[0] = height / 2;
    dst_size[0] = desc[0].tDstInfo.u32Stride[0] * desc[0].u16Ntiles[0];

    /* UV */
    desc[1].tSrcInfo.u32Imgw = desc[0].tSrcInfo.u32Imgw;
    desc[1].tSrcInfo.u32Stride[0] = width;
    desc[1].tSrcInfo.u64PhyAddr = reinterpret_cast<uint64_t>(image_dev_mem) + width * height;
    desc[1].tDstInfo.u32Imgw = desc[0].tDstInfo.u32Imgw;
    desc[1].tDstInfo.u32Stride[0] = desc[1].tDstInfo.u32Imgw;
    desc[1].u16Ntiles[0] = desc[0].u16Ntiles[0] / 2;
    dst_size[1] = desc[1].tDstInfo.u32Stride[0] * desc[1].u16Ntiles[0];

    for (int i = 0; i < 2; ++i) {
        void *dst_dev_mem;
        if (ret = axclrtMalloc(&dst_dev_mem, dst_size[i], AXCL_MEM_MALLOC_NORMAL_ONLY); AXCL_SUCC != ret) {
            SAMPLE_LOG_E("[%d] alloc device 0x%x bytes memory fail, ret = 0x%x", i, dst_size[i], ret);
            break;
        }

        desc[i].tDstInfo.u64PhyAddr = reinterpret_cast<uint64_t>(dst_dev_mem);
        if (ret = AXCL_DMA_MemCopyXD(desc[i], AX_DMADIM_2D); AXCL_SUCC != ret) {
            SAMPLE_LOG_E("[%d] dma memcpy 2D fail, ret = 0x%x", i, ret);
            break;
        } else {
            SAMPLE_LOG_I("[%d] dma memcpy 2D succeed", i);
        }

        axclrtMallocHost(&host_mem[i], dst_size[i]);
        if (ret = axclrtMemcpy(host_mem[i], dst_dev_mem, dst_size[i], AXCL_MEMCPY_DEVICE_TO_HOST); AXCL_SUCC != ret) {
            SAMPLE_LOG_E("[%d] memcpy from device memory %p to host memory %p fail, ret = 0x%x", i, dst_dev_mem, host_mem, ret);
            break;
        }
    }

    axclrtFree(image_dev_mem);

    if (desc[0].tDstInfo.u64PhyAddr) {
        axclrtFree(reinterpret_cast<void *>(desc[0].tDstInfo.u64PhyAddr));
    }
    if (desc[1].tDstInfo.u64PhyAddr) {
        axclrtFree(reinterpret_cast<void *>(desc[1].tDstInfo.u64PhyAddr));
    }

    if (AXCL_SUCC == ret) {
        char output[256];
        sprintf(output, "./dma2d_output_image_%dx%d.nv12", desc[0].tDstInfo.u32Stride[0], desc[0].u16Ntiles[0]);
        FILE *fp = fopen(output, "wb");
        if (fp) {
            fwrite(host_mem[0], 1, dst_size[0], fp);
            fwrite(host_mem[1], 1, dst_size[1], fp);
            fclose(fp);
            SAMPLE_LOG_I("%s is saved", output);
        }
    }

    if (IS_AXCL_UT_IMAGE(image)) {
        const uint8_t *p = image_320x240_nv12_yuv;
        if (0 != memcmp(p, host_mem[0], dst_size[0])) {
            SAMPLE_LOG_E("compare axcl.ut.image Y FAIL");
        }

        p += dst_size[0];

        if (0 != memcmp(p, host_mem[1], dst_size[1])) {
            SAMPLE_LOG_E("compare axcl.ut.image UV FAIL");
        }
    }

    axclrtFreeHost(host_mem[0]);
    axclrtFreeHost(host_mem[1]);

    SAMPLE_LOG_I("dma copy2d nv12 image pass");
    return ret;
}
