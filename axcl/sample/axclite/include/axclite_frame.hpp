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

#include "axclite.h"

namespace axclite {

struct AXCL_EXPORT axclite_frame {
    int32_t module = AXCL_LITE_NONE;
    int32_t grp = 0;
    int32_t chn = 0;

    union {
        AX_VIDEO_FRAME_INFO_T frame;
        AX_VENC_STREAM_T stream;
    };

    axclite_frame() = default;

    axclite_frame(int32_t _module, int32_t _grp, int32_t _chn, const AX_VIDEO_FRAME_INFO_T &_frame)
        : module(_module), grp(_grp), chn(_chn), frame(_frame) {
    }

    axclite_frame(int32_t _module, int32_t _grp, int32_t _chn, const AX_VENC_STREAM_T &_stream)
        : module(_module), grp(_grp), chn(_chn), stream(_stream) {
    }

    axclite_frame(const axclite_frame &) = default;
    axclite_frame(axclite_frame &&) noexcept = default;
    axclite_frame &operator=(const axclite_frame &) = default;
    axclite_frame &operator=(axclite_frame &&) noexcept = default;

    axclError increase_ref_cnt() const {
        for (uint32_t i = 0; i < AX_MAX_COLOR_COMPONENT; ++i) {
            if (AX_INVALID_BLOCKID != frame.stVFrame.u32BlkId[i]) {
                if (axclError ret = AXCL_POOL_IncreaseRefCnt(frame.stVFrame.u32BlkId[i]); AXCL_SUCC != ret) {
                    for (uint32_t j = 0; j < i; ++j) {
                        if (AX_INVALID_BLOCKID != frame.stVFrame.u32BlkId[j]) {
                            AXCL_POOL_DecreaseRefCnt(frame.stVFrame.u32BlkId[j]);
                        }
                    }

                    return ret;
                }
            }
        }

        return AXCL_SUCC;
    }

    axclError decrease_ref_cnt() const {
        for (uint32_t i = 0; i < AX_MAX_COLOR_COMPONENT; ++i) {
            if (AX_INVALID_BLOCKID != frame.stVFrame.u32BlkId[i]) {
                if (axclError ret = AXCL_POOL_DecreaseRefCnt(frame.stVFrame.u32BlkId[i]); AXCL_SUCC != ret) {
                    return ret;
                }
            }
        }

        return AXCL_SUCC;
    }
};

typedef axclite_frame axclite_frame;

}  // namespace axclite