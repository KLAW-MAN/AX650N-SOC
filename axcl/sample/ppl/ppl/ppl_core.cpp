/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#include "ppl_core.hpp"
#include <exception>
#include "axclite_msys.hpp"
#include "axcl_logger.hpp"
#include "ppl_hvcp.hpp"
#include "ppl_transcode.hpp"

#define TAG "ppl-core"

static inline ippl* get_ppl_impl(axcl_ppl handle) {
    ippl *obj = reinterpret_cast<ippl *>(handle);
    if (!obj) {
        LOG_MM_E(TAG, "invalid ppl handle");
    }
    return obj;
}

#define GET_PPL(handle) get_ppl_impl(handle)

#define CHECK_NULL_PTR(p)                      \
    if (!(p)) {                                \
        LOG_MM_E(TAG, "nil parameter");        \
        return AXCL_ERR_LITE_PPL_NULL_POINTER; \
    }

axclError ppl_core::init(axcl_ppl_init_param *param) {
    std::lock_guard<std::mutex> lck(m_mtx);
    if (m_inited) {
        LOG_MM_W(TAG, "axcl ppl has been initialized!");
        return AXCL_SUCC;
    }

    axclError ret;
    if (ret = check_init_param(param); AXCL_SUCC != ret) {
        return ret;
    }

    if (ret = axclInit(param->json); AXCL_SUCC != ret) {
        LOG_MM_E(TAG, "axclInit() fail, ret = {:#x}", static_cast<uint32_t>(ret));
        return ret;
    }

    axclrtDeviceList device_list;
    if (axclError ret = axclrtGetDeviceList(&device_list); AXCL_SUCC != ret || 0 == device_list.num) {
        LOG_MM_E(TAG, "no device is connected");
        axclFinalize();
        return ret;
    }

    m_device = device_list.devices[param->device_index];
    param->device_id = m_device;
    LOG_MM_I(TAG, "device index: {}, bus number: {}", param->device_index, m_device);

    if (ret = axclrtSetDevice(m_device); AXCL_SUCC != ret) {
        LOG_MM_E(TAG, "axclrtSetDevice(device: {}) fail, ret = {:#x}", m_device, static_cast<uint32_t>(ret));

        axclFinalize();
        return ret;
    }

    axclite_msys_attr attr = {};
    attr.modules = (0 == param->modules) ? AXCL_LITE_DEFAULT : param->modules;
    attr.max_vdec_grp = param->max_vdec_grp;
    attr.max_venc_thd = param->max_venc_thd;
    attr.vdec_clk = param->vdec_clk;
    if (ret = MSYS()->init(attr); AXCL_SUCC != ret) {
        axclrtResetDevice(m_device);
        axclFinalize();
        return ret;
    }

    m_inited = true;
    return AXCL_SUCC;
}

axclError ppl_core::deinit() {
    std::lock_guard<std::mutex> lck(m_mtx);

    if (!m_inited) {
        LOG_MM_W(TAG, "axcl ppl has not been initialized yet");
        return AXCL_SUCC;
    }

    axclError ret;
    if (ret = MSYS()->deinit(); AXCL_SUCC != ret) {
        return ret;
    }

    if (ret = axclrtResetDevice(m_device); AXCL_SUCC != ret) {
        LOG_MM_E(TAG, "axclrtResetDevice(device: {}) fail, ret = {:#x}", m_device, static_cast<uint32_t>(ret));
        return ret;
    }

    m_device = 0;
    axclFinalize();

    m_inited = false;
    return AXCL_SUCC;
}

axclError ppl_core::create(axcl_ppl *ppl, const axcl_ppl_param *param) {
    CHECK_NULL_PTR(ppl);
    CHECK_NULL_PTR(param);

    ippl *obj;
    switch (param->ppl) {
        case AXCL_PPL_TRANSCODE:
            obj = new (std::nothrow) ppl_transcode(m_id++, m_device, *reinterpret_cast<const axcl_ppl_transcode_param *>(param->param));
            break;
        case AXCL_PPL_HVCP:
            obj = new (std::nothrow) ppl_hvcp(m_id++, m_device, *reinterpret_cast<const axcl_ppl_hvcp_param *>(param->param));
            break;
        default:
            obj = nullptr;
            LOG_MM_E(TAG, "unsupport ppl {}", static_cast<int32_t>(param->ppl));
            break;
    }

    if (!obj) {
        LOG_MM_E(TAG, "create ppl {} instance fail", static_cast<int32_t>(param->ppl));
        return AXCL_ERR_LITE_PPL_CREATE;
    }

    if (axclError ret = obj->init(); AXCL_SUCC != ret) {
        delete obj;
        return ret;
    }

    *ppl = reinterpret_cast<axcl_ppl>(obj);
    return AXCL_SUCC;
}

axclError ppl_core::destroy(axcl_ppl ppl) {
    ippl *obj = GET_PPL(ppl);
    if (!obj) {
        return AXCL_ERR_LITE_PPL_INVALID_PPL;
    }
    if (axclError ret = obj->deinit(); AXCL_SUCC != ret) {
        return ret;
    }

    delete obj;
    return AXCL_SUCC;
}

axclError ppl_core::start(axcl_ppl ppl) {
    ippl *obj = GET_PPL(ppl);
    if (!obj) {
        return AXCL_ERR_LITE_PPL_INVALID_PPL;
    }
    return obj->start();
}

axclError ppl_core::stop(axcl_ppl ppl) {
    ippl *obj = GET_PPL(ppl);
    if (!obj) {
        return AXCL_ERR_LITE_PPL_INVALID_PPL;
    }
    return obj->stop();
}

axclError ppl_core::send_stream(axcl_ppl ppl, const axcl_ppl_input_stream *stream, AX_S32 timeout) {
    CHECK_NULL_PTR(stream);

    ippl *obj = GET_PPL(ppl);
    if (!obj) {
        return AXCL_ERR_LITE_PPL_INVALID_PPL;
    }
    return obj->send_stream(stream, timeout);
}

axclError ppl_core::get_attr(axcl_ppl ppl, const char *name, void *attr) {
    CHECK_NULL_PTR(name);
    CHECK_NULL_PTR(attr);

    ippl *obj = GET_PPL(ppl);
    if (!obj) {
        return AXCL_ERR_LITE_PPL_INVALID_PPL;
    }
    return obj->get_attr(name, attr);
}

axclError ppl_core::set_attr(axcl_ppl ppl, const char *name, const void *attr) {
    CHECK_NULL_PTR(name);
    CHECK_NULL_PTR(attr);

    ippl *obj = GET_PPL(ppl);
    if (!obj) {
        return AXCL_ERR_LITE_PPL_INVALID_PPL;
    }
    return obj->set_attr(name, attr);
}

axclError ppl_core::check_init_param(const axcl_ppl_init_param *param) {
    CHECK_NULL_PTR(param);

    /*
        if (param->device <= 0) {
            LOG_MM_E(TAG, "invalid device id {}", param->device);
            return AXCL_ERR_LITE_PPL_ILLEGAL_PARAM;
        }
    */

    if (param->max_vdec_grp < 1) {
        LOG_MM_E(TAG, "invalid max vdec grp num {}", param->max_vdec_grp);
        return AXCL_ERR_LITE_PPL_ILLEGAL_PARAM;
    }

    if ((param->modules & AXCL_LITE_VENC) || (param->modules & AXCL_LITE_JENC)) {
        if (param->max_venc_thd < 1) {
            LOG_MM_E(TAG, "invalid max venc thread num {}", param->max_venc_thd);
            return AXCL_ERR_LITE_PPL_ILLEGAL_PARAM;
        }
    }

    return AXCL_SUCC;
}