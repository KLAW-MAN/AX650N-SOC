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

#include <filesystem>

#if !defined(WINDOWS)
#include "utilities/scalar_guard.hpp"
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#endif // !WINDOWS

#if defined(WINDOWS)
#include <Windows.h>
#endif // WINDOWS

#include <utilities/file.hpp>

namespace utilities {

struct file_mapper {
    file_mapper() = delete;

    explicit file_mapper(const std::string& path) {
        try {
            auto size = std::filesystem::file_size(path);
            this->size_ = size;
        } catch (...) {
            return;
        }

#if !defined(WINDOWS)
        auto fd_guard = scalar_guard<int>(
            ::open(path.c_str(), O_RDONLY),
            [](const int& fd) { if (fd != -1) ::close(fd); }
        );

        if (-1 == fd_guard.get()) {
            return;
        }

        auto map_guard = scalar_guard<void*>(
            ::mmap(nullptr, this->size_, PROT_READ, MAP_SHARED, fd_guard.get(), 0),
            [this](void*& addr) { if (MAP_FAILED != addr && nullptr != addr) ::munmap(addr, this->size_); }
        );

        if (MAP_FAILED == map_guard.get()) {
            return;
        }

        std::swap(this->fd_, fd_guard.get());
        std::swap(this->buffer_, map_guard.get());

#elif defined(WINDOWS)
        HANDLE hFile = CreateFileA(path.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr,
                                 OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
        if (hFile == INVALID_HANDLE_VALUE) {
            return;
        }

        HANDLE hMapping = CreateFileMapping(hFile, nullptr, PAGE_READONLY, 0, 0, nullptr);
        if (hMapping == nullptr) {
            CloseHandle(hFile);
            return;
        }

        void* buffer = MapViewOfFile(hMapping, FILE_MAP_READ, 0, 0, 0);
        if (buffer == nullptr) {
            CloseHandle(hMapping);
            CloseHandle(hFile);
            return;
        }

        this->hFile_ = hFile;
        this->hMapping_ = hMapping;
        this->buffer_ = buffer;
#endif
    }

    ~file_mapper() {
#if !defined(WINDOWS)
        if (nullptr != this->buffer_) {
            ::munmap(this->buffer_, this->size_);
            this->buffer_ = nullptr;
        }
        if (-1 != this->fd_) {
            ::close(this->fd_);
            this->fd_ = -1;
        }
#elif defined(WINDOWS)
        if (nullptr != this->buffer_) {
            UnmapViewOfFile(this->buffer_);
            this->buffer_ = nullptr;
        }
        if (this->hMapping_ != nullptr) {
            CloseHandle(this->hMapping_);
            this->hMapping_ = nullptr;
        }
        if (this->hFile_ != INVALID_HANDLE_VALUE) {
            CloseHandle(this->hFile_);
            this->hFile_ = INVALID_HANDLE_VALUE;
        }
#endif
    }

    [[nodiscard]] void* get() const {
        return this->buffer_;
    }

    [[nodiscard]] uint64_t size() const {
        return this->size_;
    }

private:
#if !defined(WINDOWS)
    int fd_ = -1;
#elif defined(WINDOWS)
    HANDLE hFile_ = INVALID_HANDLE_VALUE;
    HANDLE hMapping_ = nullptr;
#endif
    void* buffer_ = nullptr;
    uint64_t size_ = 0;
};

}
