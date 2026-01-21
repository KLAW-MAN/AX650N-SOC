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

#include <functional>
#include <vector>
#include <utility>

template<typename T>
class res_guard {
public:
    res_guard() = delete;

    res_guard(T resource, std::function<void(T&)> destructor)
        : m_resource(resource), m_destructor(std::move(destructor)) {}

    res_guard(std::function<T()> resource_creator, std::function<void(T&)> destructor)
        : m_resource(resource_creator()), m_destructor(std::move(destructor)) {}

    res_guard(res_guard&& other) noexcept
        : m_resource(std::exchange(other.m_resource, T{})), m_destructor(std::move(other.m_destructor)) {}

    res_guard& operator=(res_guard&& other) noexcept {
        if (this != &other) {
            m_resource = std::exchange(other.m_resource, T{});
            m_destructor = std::move(other.m_destructor);
        }
        return *this;
    }

    res_guard(const res_guard&) = delete;
    res_guard& operator=(const res_guard&) = delete;

    ~res_guard() noexcept {
        if (m_destructor) {
            m_destructor(m_resource);
        }
    }

    T& get() {
        return m_resource;
    }

private:
    T m_resource;
    std::function<void(T&)> m_destructor;
};

template<typename T>
class res_vector_guard {
public:
    res_vector_guard() = delete;

    res_vector_guard(std::vector<T> resources, std::function<void(T&)> destructor)
        : m_resources(std::move(resources)), m_destructor(std::move(destructor)) {}

    res_vector_guard(std::function<std::vector<T>()> resource_creator, std::function<void(T&)> destructor)
        : m_resources(resource_creator()), m_destructor(std::move(destructor)) {}

    res_vector_guard(res_vector_guard&& other) noexcept
        : m_resources(std::exchange(other.m_resources, std::vector<T>{})), m_destructor(std::move(other.m_destructor)) {}

    res_vector_guard& operator=(res_vector_guard&& other) noexcept {
        if (this != &other) {
            m_resources = std::exchange(other.m_resources, std::vector<T>{});
            m_destructor = std::move(other.m_destructor);
        }
        return *this;
    }

    res_vector_guard(const res_vector_guard&) = delete;
    res_vector_guard& operator=(const res_vector_guard&) = delete;

    ~res_vector_guard() noexcept {
        if (m_destructor) {
            for (auto& resource : m_resources) {
                m_destructor(resource);
            }
        }
    }

    std::vector<T>& get() {
        return m_resources;
    }

    T* data() {
        return m_resources.data();
    }

private:
    std::vector<T> m_resources;
    std::function<void(T&)> m_destructor;
};
