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

#include <stdexcept>
#include <type_traits>

namespace axcl {
/**
 * not safe, because max and min not check
 */
template <typename EnumFrom, typename EnumTo>
EnumTo enum_cast(EnumFrom from) {
    static_assert(std::is_enum<EnumFrom>::value, "type1 must be an enum class");
    static_assert(std::is_enum<EnumTo>::value, "type2 must be an enum class");

    auto value = static_cast<std::underlying_type_t<EnumFrom>>(from);
    return static_cast<EnumTo>(static_cast<std::underlying_type_t<EnumTo>>(value));
}

template <typename EnumA, typename EnumB>
bool enum_equal(EnumA a, EnumB b) {
    static_assert(std::is_enum<EnumA>::value, "type1 must be an enum class");
    static_assert(std::is_enum<EnumB>::value, "type2 must be an enum class");
    return static_cast<std::underlying_type_t<EnumA>>(a) == static_cast<std::underlying_type_t<EnumB>>(b);
}

}  // namespace axcl