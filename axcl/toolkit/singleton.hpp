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
#include <type_traits>

namespace axcl {

/* https://stackoverflow.com/questions/34519073/inherit-singleton */
template <typename T>
class singleton {
public:
    static T *get_instance(void) noexcept {
        static T instance;
        return &instance;
    };

protected:
    singleton(void) noexcept = default;
    singleton(const singleton &rhs) = delete;
    singleton &operator=(const singleton &rhs) = delete;

    virtual ~singleton(void) = default;
};

}  // namespace axcl
