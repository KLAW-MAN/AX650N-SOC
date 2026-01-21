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

#include <mutex>
#include <unordered_map>
#include <unordered_set>
#include "axclite.h"

namespace axclite {

class AXCL_EXPORT linker {
    struct hash_fn {
        std::size_t operator()(const AX_MOD_INFO_T& m) const {
            return std::hash<int32_t>()(m.enModId) ^ std::hash<int32_t>()(m.s32GrpId) ^ std::hash<int32_t>()(m.s32ChnId);
        }
    };

    struct hash_equal {
        bool operator()(const AX_MOD_INFO_T& a, const AX_MOD_INFO_T& b) const {
            return ((a.enModId == b.enModId) && (a.s32GrpId == b.s32GrpId) && (a.s32ChnId == b.s32ChnId));
        }
    };

public:
    linker() = default;

    axclError link(const AX_MOD_INFO_T& src, const AX_MOD_INFO_T& dst);
    axclError unlink(const AX_MOD_INFO_T& src);
    axclError unlink(const AX_MOD_INFO_T& src, const AX_MOD_INFO_T& dst);
    axclError unlink();

private:
    std::mutex m_mtx;
    std::unordered_map<AX_MOD_INFO_T, std::unordered_set<AX_MOD_INFO_T, hash_fn, hash_equal>, hash_fn, hash_equal> m_map;
};

}  // namespace axclite