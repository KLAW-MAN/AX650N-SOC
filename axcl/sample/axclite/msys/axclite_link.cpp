/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#include "axclite_link.hpp"
#include "axcl_logger.hpp"

#define TAG "axclite-link"

namespace axclite {

axclError linker::link(const AX_MOD_INFO_T& src, const AX_MOD_INFO_T& dst) {
    std::lock_guard<std::mutex> lock(m_mtx);

    auto it = m_map.find(src);
    if (it != m_map.end() && it->second.find(dst) != it->second.end()) {
        LOG_MM_W(TAG, "[{}, {}, {}] is already linked to [{}, {}, {}]", static_cast<int32_t>(src.enModId), src.s32GrpId, src.s32ChnId,
                 static_cast<int32_t>(dst.enModId), dst.s32GrpId, dst.s32ChnId);
        return AXCL_SUCC;
    }

    if (axclError ret = AXCL_SYS_Link(&src, &dst); AXCL_SUCC != ret) {
        LOG_MM_E(TAG, "AXCL_SYS_Link: [{}, {}, {}] -> [{}, {}, {}] fail, ret = {:#x}", static_cast<int32_t>(src.enModId), src.s32GrpId,
                 src.s32ChnId, static_cast<int32_t>(dst.enModId), dst.s32GrpId, dst.s32ChnId, static_cast<uint32_t>(ret));
        return ret;
    }

    m_map[src].insert(dst);
    LOG_MM_I(TAG, "[{}, {}, {}] -> [{}, {}, {}] is linked", static_cast<int32_t>(src.enModId), src.s32GrpId, src.s32ChnId,
             static_cast<int32_t>(dst.enModId), dst.s32GrpId, dst.s32ChnId);

    return AXCL_SUCC;
}

axclError linker::unlink(const AX_MOD_INFO_T& src) {
    std::lock_guard<std::mutex> lock(m_mtx);

    auto it = m_map.find(src);
    if (it == m_map.end()) {
        LOG_MM_W(TAG, "no dst are linked to [{}, {}, {}]", static_cast<int32_t>(src.enModId), src.s32GrpId, src.s32ChnId);
        return AXCL_SUCC;
    }

    axclError err = AXCL_SUCC;
    auto& dst_set = it->second;
    for (auto dst_it = dst_set.begin(); dst_it != dst_set.end();) {
        const auto& dst = *dst_it;
        if (axclError ret = AXCL_SYS_UnLink(&src, &dst); AXCL_SUCC != ret) {
            LOG_MM_E(TAG, "AXCL_SYS_UnLink: [{}, {}, {}] -> [{}, {}, {}] fail, ret = {:#x}", static_cast<int32_t>(src.enModId),
                     src.s32GrpId, src.s32ChnId, static_cast<int32_t>(dst.enModId), dst.s32GrpId, dst.s32ChnId, static_cast<uint32_t>(ret));
            err = ret;
            ++dst_it;
        } else {
            LOG_MM_I(TAG, "[{}, {}, {}] -> [{}, {}, {}] is unlinked", static_cast<int32_t>(src.enModId), src.s32GrpId, src.s32ChnId,
                     static_cast<int32_t>(dst.enModId), dst.s32GrpId, dst.s32ChnId);
            dst_it = dst_set.erase(dst_it);
        }
    }

    if (dst_set.empty()) {
        m_map.erase(it);
    }

    return err;
}

axclError linker::unlink(const AX_MOD_INFO_T& src, const AX_MOD_INFO_T& dst) {
    std::lock_guard<std::mutex> lock(m_mtx);

    auto it = m_map.find(src);
    if (it == m_map.end()) {
        LOG_MM_E(TAG, "[{}, {}, {}] -> [{}, {}, {}] is not linked", static_cast<int32_t>(src.enModId), src.s32GrpId, src.s32ChnId,
                 static_cast<int32_t>(dst.enModId), dst.s32GrpId, dst.s32ChnId);
        return AXCL_ERR_LITE_MSYS_NO_LINKED;
    }

    auto& dst_set = it->second;
    auto dst_it = dst_set.find(dst);
    if (dst_it == dst_set.end()) {
        LOG_MM_E(TAG, "[{}, {}, {}] -> [{}, {}, {}] is not linked", static_cast<int32_t>(src.enModId), src.s32GrpId, src.s32ChnId,
                 static_cast<int32_t>(dst.enModId), dst.s32GrpId, dst.s32ChnId);
        return AXCL_ERR_LITE_MSYS_NO_LINKED;
    }

    if (axclError ret = AXCL_SYS_UnLink(&src, &dst); AXCL_SUCC != ret) {
        LOG_MM_E(TAG, "AXCL_SYS_UnLink: [{}, {}, {}] -> [{}, {}, {}] fail, ret = {:#x}", static_cast<int32_t>(src.enModId), src.s32GrpId,
                 src.s32ChnId, static_cast<int32_t>(dst.enModId), dst.s32GrpId, dst.s32ChnId, static_cast<uint32_t>(ret));
        return ret;
    }

    dst_set.erase(dst_it);
    if (dst_set.empty()) {
        m_map.erase(it);
    }

    LOG_MM_I(TAG, "[{}, {}, {}] -> [{}, {}, {}] is unlinked", static_cast<int32_t>(src.enModId), src.s32GrpId, src.s32ChnId,
             static_cast<int32_t>(dst.enModId), dst.s32GrpId, dst.s32ChnId);
    return AXCL_SUCC;
}

axclError linker::unlink() {
    std::lock_guard<std::mutex> lock(m_mtx);

    axclError err = AXCL_SUCC;
    auto it = m_map.begin();
    while (it != m_map.end()) {
        const auto& src = it->first;
        auto& dst_set = it->second;

        for (auto dst_it = dst_set.begin(); dst_it != dst_set.end();) {
            const auto& dst = *dst_it;
            if (axclError ret = AXCL_SYS_UnLink(&src, &dst); AXCL_SUCC != ret) {
                LOG_MM_E(TAG, "AXCL_SYS_UnLink: [{}, {}, {}] -> [{}, {}, {}] fail, ret = {:#x}", static_cast<int32_t>(src.enModId),
                         src.s32GrpId, src.s32ChnId, static_cast<int32_t>(dst.enModId), dst.s32GrpId, dst.s32ChnId, static_cast<uint32_t>(ret));
                err = ret;
                ++dst_it;
            } else {
                LOG_MM_I(TAG, "[{}, {}, {}] -> [{}, {}, {}] is unlinked", static_cast<int32_t>(src.enModId), src.s32GrpId, src.s32ChnId,
                         static_cast<int32_t>(dst.enModId), dst.s32GrpId, dst.s32ChnId);
                dst_it = dst_set.erase(dst_it);
            }
        }

        if (dst_set.empty()) {
            it = m_map.erase(it);
        } else {
            ++it;
        }
    }

    return err;
}

}  // namespace axclite