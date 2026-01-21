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

#include <cstdint>
#include <string>

namespace common {

#if defined(ENV_CHIP_SERIES_MC50) || defined(ENV_CHIP_SERIES_MC20E)
inline std::string get_visual_mode_string(const uint32_t& mode_value) {
#if defined(ENV_CHIP_SERIES_MC50)
    switch (mode_value) {
        case 0:
            return "Disable";
        case 1:
            return "Standard";
        case 2:
            return "BigLittle";
        case 4:
            return "LittleBig";
        default:
            return {};
    }
#endif

#if defined(ENV_CHIP_SERIES_MC20E)
    switch (mode_value) {
        case 0:
            return "Disable";
        case 1:
            return "Enable";
        default:
            return {};
    }
#endif
}
#endif

#if defined(ENV_CHIP_SERIES_MC50) || defined(ENV_CHIP_SERIES_MC20E)
inline std::string get_model_type_string(const uint32_t& type_value) {
#if defined(ENV_CHIP_SERIES_MC50)
    switch (type_value) {
        case 0:
            return "1 Core";
        case 1:
            return "2 Core";
        case 2:
            return "3 Core";
        default:
            return {};
    }
#endif
#if defined(ENV_CHIP_SERIES_MC20E)
    switch (type_value) {
        case 0:
            return "Half";
        case 1:
            return "Full";
        default:
            return {};
    }
#endif
}
#endif

}
