/**************************************************************************************************
 *
 * Copyright (c) 2019-2025 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#include "hvcp.hpp"
#include "axcl_logger.hpp"
#include "ppl_hvcp.hpp"
#include "res_guard.hpp"

#include "yolox.hpp"

#include <cstring>

#define TAG "axclite-hvcp"

namespace axclite {
hvcp::hvcp(axcl_ppl ppl, const int32_t device_id, const axcl_hvcp_param &param)
    : m_ppl(ppl), m_device_id(device_id), m_param(param), m_engine_param({}), m_image_info({}) {
}

void hvcp::run() {
    axclrtContext context;
    axclrtCreateContext(&context, m_device_id);

    axclite_frame frame;
    while (m_thread.running()) {
        // 1. hold one frame
        auto frame_holder = res_guard<bool>(
            [this, &frame] {
                return m_frames.pop(frame, -1);
            },
            [this, &frame](bool &flag) {
                if (flag) {
                    release_frame(frame);
                    flag = false;
                }
            });
        if (!frame_holder.get()) {
            break;
        }

        // 2. set input buffer from frame
        const auto input_ptr = reinterpret_cast<void *>(frame.frame.stVFrame.u64PhyAddr[0]);
        const auto input_size = axclrtEngineGetInputSizeByIndex(m_engine_param.info, 0, 0);
        if (auto ret = axclrtEngineSetInputBufferByIndex(m_engine_param.io, 0, input_ptr, input_size); AXCL_SUCC != ret) {
            LOG_MM_E(TAG, "axclrtEngineSetInputBufferByIndex() fail, ret = {:#x}", static_cast<uint32_t>(ret));
            break;
        }

        // 3. run engine
        auto status = axclrtEngineExecute(m_engine_param.handle, m_engine_param.context, 0, m_engine_param.io);

        // 4. release frame
        frame_holder.get() = false;
        release_frame(frame);

        // 5. check engine execute status
        if (AXCL_SUCC != status) {
            LOG_MM_E(TAG, "axclrtEngineExecute() fail, ret = {:#x}", static_cast<uint32_t>(status));
            break;
        }

        LOG_M_I(TAG, "model execute success, frame {} {}x{} blk {:#x}",
            frame.frame.stVFrame.u64SeqNum, frame.frame.stVFrame.u32Width,
            frame.frame.stVFrame.u32Height, frame.frame.stVFrame.u32BlkId[0]);

        // 6. get output buffer to host
        for (size_t i = 0; i < m_engine_param.buffer.size(); ++i) {
            void* buffer = nullptr;
            uint64_t size = 0;
            if (const auto ret = axclrtEngineGetOutputBufferByIndex(m_engine_param.io, i, &buffer, &size); AXCL_SUCC != ret) {
                LOG_MM_E(TAG, "axclrtEngineGetOutputBufferByIndex() fail, ret = {:#x}", static_cast<uint32_t>(ret));
                break;
            }

            if (const auto ret = axclrtMemcpy(m_engine_param.buffer[i], buffer, size, AXCL_MEMCPY_DEVICE_TO_HOST); AXCL_SUCC != ret) {
                LOG_MM_E(TAG, "axclrtMemcpy() fail, ret = {:#x}", static_cast<uint32_t>(ret));
                break;
            }
        }

        // 7. post process
        axcl_ppl_hvcp_output output{};
        output.frame_id = frame.frame.stVFrame.u64SeqNum;
        LOG_MM_I(TAG, "pop frame {} {}x{} blk {:#x}, run hvcp",
            frame.frame.stVFrame.u64SeqNum, frame.frame.stVFrame.u32Width,
            frame.frame.stVFrame.u32Height, frame.frame.stVFrame.u32BlkId[0]);
        hvcp_post_process(output, m_image_info, m_engine_param.info, m_engine_param.buffer, m_image_info.height, m_image_info.width);

        // 8. notify thread to show result
        notify_output(output);
    }

    axclrtDestroyContext(context);
}

axclError hvcp::init() {
    m_image_info.width = 1024;
    m_image_info.height = 576;
    m_image_info.img_fmt = AX_FORMAT_YUV420_SEMIPLANAR;

    // NOTE: if not only one model in pipeline, users should move the engine init outside
    auto engine_holder = res_guard<int>(
        [this] {
            const int ret = axclrtEngineInit(static_cast<axclrtEngineVNpuKind>(m_param.vnpu_kind));
            if (AXCL_SUCC != ret) {
                LOG_MM_E(TAG, "axclrtEngineInit() fail, ret = {:#x}", static_cast<uint32_t>(ret));
            }
            return ret;
        },
        [](int &ret) {
            if (0 == ret) {
                axclrtEngineFinalize();
                ret = -1;
            }
        });
    if (0 != engine_holder.get()) {
        return engine_holder.get();
    }

    auto model_holder = res_guard<int>(
        [this] {
            const int ret = axclrtEngineLoadFromFile(m_param.model_path, &m_engine_param.handle);
            if ( AXCL_SUCC != ret) {
                LOG_MM_E(TAG, "axclrtEngineLoadFromFile(model: {}) fail, ret = {:#x}", m_param.model_path, static_cast<uint32_t>(ret));
            }
            return ret;
        },
        [this](int &ret) {
            if (0 == ret) {
                axclrtEngineUnload(m_engine_param.handle);
                ret = -1;
            }
        });
    if (0 != model_holder.get()) {
        return model_holder.get();
    }

    if (const auto ret = axclrtEngineCreateContext(m_engine_param.handle, &m_engine_param.context); AXCL_SUCC != ret) {
        LOG_MM_E(TAG, "axclrtEngineCreateContext() fail, ret = {:#x}", static_cast<uint32_t>(ret));
        return ret;
    }

    auto info_holder = res_guard<int>(
        [this] {
            const auto ret = axclrtEngineGetIOInfo(m_engine_param.handle, &m_engine_param.info);
            if (AXCL_SUCC != ret) {
                LOG_MM_E(TAG, "axclrtEngineGetIOInfo() fail, ret = {:#x}", static_cast<uint32_t>(ret));
            }
            return ret;
        },
        [&](int &ret) {
            if (0 == ret) {
                axclrtEngineDestroyIOInfo(m_engine_param.info);
                ret = -1;
            }
        });
    if (0 != info_holder.get()) {
        return info_holder.get();
    }

    auto io_holder = res_guard<int>(
        [this] {
            const auto ret = axclrtEngineCreateIO(m_engine_param.info, &m_engine_param.io);
            if (AXCL_SUCC != ret) {
                LOG_MM_E(TAG, "axclrtEngineGetIO() fail, ret = {:#x}", static_cast<uint32_t>(ret));
            }
            return ret;
        },
        [this](int &ret) {
            if (0 == ret) {
                axclrtEngineDestroyIO(m_engine_param.io);
                ret = -1;
            }
        });
    if (0 != io_holder.get()) {
        return io_holder.get();
    }

    auto output_buffer_holder = res_vector_guard<void*> (
        [this] {
            const auto num_outputs = axclrtEngineGetNumOutputs(m_engine_param.info);
            std::vector<void*> output_buffer(num_outputs, nullptr);
            for (uint32_t i = 0; i < num_outputs; ++i) {
                const uint64_t size = axclrtEngineGetOutputSizeByIndex(m_engine_param.info, 0, i);
                if (0 == size) {
                    LOG_MM_E(TAG, "axclrtEngineGetOutputSizeByIndex({}) fail", i);
                    break;
                }

                if (AXCL_SUCC != axclrtMalloc(&output_buffer[i], size, {})) {
                    LOG_MM_E(TAG, "axclrtMalloc() fail");
                    break;
                }
            }
            return output_buffer;
        },
        [this](void* buffer) {
            if (nullptr != buffer) {
                axclrtFree(buffer);
            }
        });

    for (const auto& buffer : output_buffer_holder.get()) {
        if (nullptr == buffer) {
            return AXCL_ERR_NO_MEMORY;
        }
    }

    for (size_t i = 0; i < output_buffer_holder.get().size(); ++i) {
        const void* buffer = output_buffer_holder.get()[i];
        const auto size = axclrtEngineGetOutputSizeByIndex(m_engine_param.info, 0, i);
        if (const int ret = axclrtEngineSetOutputBufferByIndex(m_engine_param.io, i, buffer, size); AXCL_SUCC != ret) {
            LOG_MM_E(TAG, "axclrtEngineSetOutputBufferByIndex() fail, ret = {:#x}", static_cast<uint32_t>(ret));
            return ret;
        }
    }

    const auto num_outputs = output_buffer_holder.get().size();
    m_engine_param.buffer.resize(num_outputs, nullptr);

    for (size_t i = 0; i < num_outputs; ++i) {
        const auto size = axclrtEngineGetOutputSizeByIndex(m_engine_param.info, 0, i);
        if (const auto ret = axclrtMallocHost(&m_engine_param.buffer[i], size); AXCL_SUCC != ret) {
            LOG_MM_E(TAG, "axclrtMallocHost() for output index {} fail, ret = {:#x}", i, static_cast<uint32_t>(ret));
            for (size_t j = 0; j < i; ++j) {
                if (nullptr != m_engine_param.buffer[j]) {
                    axclrtFreeHost(m_engine_param.buffer[j]);
                    m_engine_param.buffer[j] = nullptr;
                }
            }
            return AXCL_ERR_NO_MEMORY;
        }
    }

    for (auto& buffer : output_buffer_holder.get()) {
        buffer = nullptr;
    }

    io_holder.get() = -1;
    info_holder.get() = -1;
    model_holder.get() = -1;
    engine_holder.get() = -1;

    return AXCL_SUCC;
}

axclError hvcp::deinit() {

    for (auto& buffer : m_engine_param.buffer) {
        if (nullptr != buffer) {
            axclrtFreeHost(buffer);
            buffer = nullptr;
        }
    }
    m_engine_param.buffer.clear();

    if (nullptr != m_engine_param.io) {

        for (uint32_t i = 0; i < axclrtEngineGetNumOutputs(m_engine_param.info); ++i) {
            void* buffer = nullptr;
            uint64_t size = 0;
            if (const auto ret = axclrtEngineGetOutputBufferByIndex(m_engine_param.io, i, &buffer, &size); AXCL_SUCC == ret) {
                axclrtFree(buffer);
            }
        }

        axclrtEngineDestroyIO(m_engine_param.io);
        m_engine_param.io = nullptr;

        axclrtEngineDestroyIOInfo(m_engine_param.info);
        m_engine_param.info = nullptr;

        axclrtEngineUnload(m_engine_param.handle);
        m_engine_param.context = 0;
        m_engine_param.handle = 0;
    }

    // NOTE: if not only one model in pipeline, users should move the engine deinit outside
    axclrtEngineFinalize();

    return AXCL_SUCC;
}

const hvcp_input_image_info &hvcp::get_input_image_info() const {
    return m_image_info;
}

axclError hvcp::start() {
    LOG_MM_D(TAG, "+++");

    m_thread.start("hvcp", &hvcp::run, this);

    LOG_MM_D(TAG, "---");
    return AXCL_SUCC;
}

axclError hvcp::stop() {
    LOG_MM_D(TAG, "+++");

    m_thread.stop();
    m_frames.wakeup();
    m_thread.join();

    /* release all input frames */
    if (const size_t count = m_frames.size(); count > 0) {
        axclite_frame frame;
        for (size_t i = 0; i < count; ++i) {
            if (m_frames.pop(frame, 0)) {
                std::ignore = frame.decrease_ref_cnt();
            }
        }
    }

    LOG_MM_D(TAG, "---");
    return AXCL_SUCC;
}

int32_t hvcp::recv_frame(const axclite_frame &frame) {
    return send_frame(frame) ? 0 : 1;
}

bool hvcp::send_frame(const axclite_frame &frame) {
    if (skip(frame)) {
        return false;
    }

    if (const axclError ret = frame.increase_ref_cnt(); AXCL_SUCC != ret) {
        LOG_MM_E(TAG, "increase frame {} ref cnt fail, ret = {:#x}", frame.frame.stVFrame.u64SeqNum, static_cast<uint32_t>(ret));
        return false;
    }

    if (!m_frames.push(frame)) {
        release_frame(frame);
        LOG_MM_E(TAG, "push frame {} into hvcp fifo fail", frame.frame.stVFrame.u64SeqNum);
        return false;
    }

    LOG_MM_I(TAG, "push frame {} {}x{} blk {:#x} into hvcp fifo", frame.frame.stVFrame.u64SeqNum, frame.frame.stVFrame.u32Width,
             frame.frame.stVFrame.u32Height, frame.frame.stVFrame.u32BlkId[0]);
    return true;
}

bool hvcp::release_frame(const axclite_frame &frame) {
    if (const axclError ret = frame.decrease_ref_cnt(); AXCL_SUCC != ret) {
        LOG_MM_E(TAG, "decrease frame {} ref cnt fail, ret = {:#x}", frame.frame.stVFrame.u64SeqNum, static_cast<uint32_t>(ret));
        return false;
    }

    return true;
}

bool hvcp::skip(const axclite_frame &frame) {
    if (m_param.skip <= 1) {
        return false;
    }

    return (1 != (frame.frame.stVFrame.u64SeqNum % m_param.skip));
}

void hvcp::notify_output(const axcl_ppl_hvcp_output &output) {
    m_param.callback(m_ppl, &output, m_param.userdata);
}

}  // namespace axclite