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

#include <fstream>
#include <cctype>
#include <filesystem>
#include <string>

namespace utilities {
constexpr auto error_size = static_cast<uintmax_t>(-1);

enum class file_type {
    none,
    not_found,
    regular,
    directory,
    symlink,
    block,
    character,
    fifo,
    socket,
    unknown,
};

inline bool exists(const std::string& path) {
    return std::filesystem::exists(path);
}

inline uintmax_t file_size(const std::string& path) {
    std::error_code ec;
    auto size = std::filesystem::file_size(path, ec);
    return ec ? error_size : size;
}

inline file_type status(const std::string& path) {
    std::error_code ec;
    auto fs_status = std::filesystem::status(path, ec);
    if (ec) {
        return file_type::not_found;
    }

    if (std::filesystem::is_regular_file(fs_status)) {
        return file_type::regular;
    }
    if (std::filesystem::is_directory(fs_status)) {
        return file_type::directory;
    }
    if (std::filesystem::is_symlink(fs_status)) {
        return file_type::symlink;
    }
    if (std::filesystem::is_block_file(fs_status)) {
        return file_type::block;
    }
    if (std::filesystem::is_character_file(fs_status)) {
        return file_type::character;
    }
    if (std::filesystem::is_fifo(fs_status)) {
        return file_type::fifo;
    }
    if (std::filesystem::is_socket(fs_status)) {
        return file_type::socket;
    }
    return file_type::unknown;
}

inline bool is_regular_file(const std::string& path) {
    return std::filesystem::is_regular_file(path);
}

inline bool is_directory(const std::string& path) {
    return std::filesystem::is_directory(path);
}

inline bool is_empty(const std::string& path) {
    return std::filesystem::is_empty(path);
}

inline bool read(const std::string& file, void* data, const uintmax_t& size) {
    std::ifstream stream;
    stream.open(file, std::ios_base::binary | std::ios_base::in);
    if (stream.is_open()) {
        stream.read(static_cast<char*>(data), static_cast<std::streamsize>(size));
        stream.close();
        return true;
    }
    return false;
}

inline bool write(const std::string& file, const void* data, const uintmax_t& size) {
    std::ofstream stream;
    stream.open(file, std::ios_base::binary | std::ios_base::out);
    if (stream.is_open()) {
        stream.write(static_cast<const char*>(data), static_cast<std::streamsize>(size));
        stream.close();
        return true;
    }
    return false;
}

inline bool create_directory(const std::string& path) {
    std::error_code ec;
    return std::filesystem::create_directories(path, ec);
}

inline std::string get_file_name(const std::string& file) {
    return std::filesystem::path(file).filename().string();
}

inline std::string get_file_extension(const std::string& file) {
    return std::filesystem::path(file).extension().string();
}

inline std::string get_legal_name(const std::string& name) {
    std::string temp = name;

    size_t pos = temp.find_last_of("/\\");
    if (pos != std::string::npos) pos = pos + 1;

    for (size_t i = pos; i < temp.size(); ++i)
        if (!(::isalnum(temp[i]) || '-' == temp[i] || '_' == temp[i])) temp[i] = '_';
    return temp;
}

}
