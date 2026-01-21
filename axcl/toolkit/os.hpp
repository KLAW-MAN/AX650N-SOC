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
// xp : Cross-Platform Toolkit

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <thread>

/* ------------------------------------------------------------------------------------------------
 * 1. Platform-independent IOCTL command construction macros
 *    Simulate Linux _IOC/_IO/_IOR/_IOW/_IOWR semantics on Windows
 * ------------------------------------------------------------------------------------------------ */
#if defined(WINDOWS)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <direct.h>
#include <errno.h>
#include <io.h>
#include <sys/stat.h>
#include <windows.h>
#include <winioctl.h>

// Define ssize_t for Windows
#ifndef _SSIZE_T_DEFINED
#define _SSIZE_T_DEFINED
typedef long long ssize_t;
#endif

// clang-format off
#define FSCTL_AXCLDEV_BASE      FILE_DEVICE_UNKNOWN
#define _AXCLDEV_CTL_CODE(_Function, _Method, _Access)  CTL_CODE(FSCTL_AXCLDEV_BASE, _Function, _Method, _Access)

#define AX_IO(type,nr)        _AXCLDEV_CTL_CODE((type) + (nr), METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)
#define AX_IOR(type,nr,size)  _AXCLDEV_CTL_CODE((type) + (nr), METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)
#define AX_IOW(type,nr,size)  _AXCLDEV_CTL_CODE((type) + (nr), METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)
#define AX_IOWR(type,nr,size) _AXCLDEV_CTL_CODE((type) + (nr), METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)

#define IOC_AXCL_WRITE          AX_IOWR(0, 13, 0)
#define IOC_AXCL_READ           AX_IOWR(0, 14, 0)
#define IOC_AXCL_POLL           AX_IOWR(0, 15, 0)
#define IOC_AXCL_MAP            AX_IOWR(0, 16, 0)
#define IOC_AXCL_UNMAP          AX_IOWR(0, 17, 0)

#define AXCL_STATUS_POLLIN      0x00000001      // data available
#define AXCL_STATUS_POLLNVAL    0x00000002    // invalid handle
#define AXCL_STATUS_POLLHUP     0x00000004     // remote closed/device removed
#define AXCL_STATUS_ERROR       0x00000008       // other error
#define AXCL_STATUS_POLLRDNORM  0x00000010  // normal data available
#define AXCL_STATUS_POLLREMOVE  0x00000020  // remove listen
// clang-format on

#else  // defined(WINDOWS)

#include <errno.h>
#include <error.h>
#include <fcntl.h>
#include <linux/ioctl.h>
#include <pthread.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/select.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <unistd.h>
#include <cstring>

// clang-format off
#define AX_IO     _IO
#define AX_IOR    _IOR
#define AX_IOW    _IOW
#define AX_IOWR   _IOWR
// clang-format on

#endif  // defined(WINDOWS)

/* ------------------------------------------------------------------------------------------------
 * 2. Type definitions and constant definitions
 * ------------------------------------------------------------------------------------------------ */
// clang-format off
// Device operation related type definitions
#if defined(WINDOWS)
    using xp_dev_t = HANDLE;
    const xp_dev_t XP_INVALID_DEV = INVALID_HANDLE_VALUE;
#else
    using xp_dev_t = int;
    constexpr xp_dev_t XP_INVALID_DEV = -1;
#endif

// File operation related type definitions
#if defined(WINDOWS)
    using xp_file_t = HANDLE;
    using xp_stat_t = struct _stat64;
    #define XP_INVALID_FILE INVALID_HANDLE_VALUE
    #define XP_FILE_READ_ONLY GENERIC_READ
    #define XP_MAP_READ_ONLY FILE_MAP_READ
#else
    using xp_file_t = int;
    using xp_stat_t = struct stat;
    #define XP_INVALID_FILE -1
    #define XP_FILE_READ_ONLY O_RDONLY
    #define XP_MAP_READ_ONLY PROT_READ
#endif

#if defined(WINDOWS)
    #define SCHED_FIFO  1
    #define SCHED_RR    2
    #define SCHED_OTHER 0
#endif
// clang-format on

/* ------------------------------------------------------------------------------------------------
 * 3. Process and thread management interfaces
 * ------------------------------------------------------------------------------------------------ */

// Process/thread ID acquisition macro definitions
#if defined(WINDOWS)
#define gettid() static_cast<uint32_t>(GetCurrentThreadId())
#define getpid() static_cast<uint32_t>(GetCurrentProcessId())
#else
#define gettid() static_cast<uint32_t>(syscall(SYS_gettid))
#define getpid() static_cast<uint32_t>(::getpid())
#endif

/**
 * @brief Set current thread name
 * @param name Thread name
 */
inline void xp_thr_set_name(const char* name) {
#if defined(WINDOWS)
    // Not implemented on Windows platform yet
    (void)name;
#else
    pthread_setname_np(pthread_self(), name);
#endif
}

/**
 * @brief Set current thread priority
 * @param policy Thread priority policy
 * @param priority Thread priority
 */
inline void xp_thr_set_priority(int policy, int priority) {
#if defined(WINDOWS)
    // Not implemented on Windows platform yet
    (void)policy;
    (void)priority;
#else
    if ((SCHED_FIFO == policy || SCHED_RR == policy) && priority > 0) {
        struct sched_param sch;
        sch.sched_priority = priority;
        pthread_setschedparam(pthread_self(), policy, &sch);
    }
#endif
}

/* ------------------------------------------------------------------------------------------------
 * 4. Cross-platform device abstraction
 *    Provides unified open/close/ioctl/map/wait interfaces
 * ------------------------------------------------------------------------------------------------ */

/* --------------------------------------------------------------------------- */
/* Get system error code                                                     */
/* --------------------------------------------------------------------------- */
inline int xp_io_error() {
#if defined(WINDOWS)
    return static_cast<int>(::GetLastError());
#else
    return errno;
#endif
}

/* --------------------------------------------------------------------------- */
/* Device open/close                                                          */
/* --------------------------------------------------------------------------- */

/**
 * @brief Open device
 * @param name Device name, "/dev/xxx" on Linux; "xxx" on Windows (internally converted to "\\.\xxx")
 * @return Returns valid handle on success, XP_INVALID_DEV on failure
 */
inline xp_dev_t xp_io_open(const char* name) {
#if defined(WINDOWS)
    char buf[64];
    snprintf(buf, sizeof(buf), "\\\\.\\%s", name);

    HANDLE h =
        ::CreateFileA(buf, GENERIC_READ | GENERIC_WRITE, NULL, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED | FILE_ATTRIBUTE_NORMAL, NULL);

    return (h == INVALID_HANDLE_VALUE) ? XP_INVALID_DEV : h;
#else
    int fd = ::open(name, O_RDWR);
    return (fd < 0) ? XP_INVALID_DEV : fd;
#endif
}

/**
 * @brief Close device
 * @param h Handle, XP_INVALID_DEV is allowed (no operation)
 */
inline void xp_io_close(xp_dev_t h) {
    if (h == XP_INVALID_DEV) return;

#if defined(WINDOWS)
    ::CloseHandle(h);
#else
    ::close(h);
#endif
}

/* --------------------------------------------------------------------------- */
/* ioctl wrapper                                                              */
/* --------------------------------------------------------------------------- */
/**
 * @brief Full version ioctl, can specify input/output buffer sizes separately
 * @param h Device handle
 * @param cmd IOCTL command
 * @param in Input buffer pointer
 * @param in_bytes Input buffer byte count
 * @param out Output buffer pointer
 * @param out_bytes Output buffer byte count
 * @return Returns 0 on success, -1 on failure
 *
 * @note On Linux platform, in and out must point to the same buffer, so internal check and cast is performed
 */
inline int xp_io_ioctl(xp_dev_t h, unsigned long cmd, const void* in, size_t in_bytes, void* out, size_t out_bytes) {
#if defined(WINDOWS)
    DWORD ret = 0;
    return ::DeviceIoControl(h, cmd, const_cast<void*>(in), static_cast<DWORD>(in_bytes), out, static_cast<DWORD>(out_bytes), &ret, nullptr)
               ? 0
               : -1;
#else
    if (in != out) return -1;
    return ::ioctl(h, cmd, out);
#endif
}

/**
 * @brief Map device memory to user space
 * @param h Device handle
 * @param size Mapping length (bytes)
 * @return Returns mapping start address on success, nullptr on failure
 *
 * @note On Windows platform, temporary Section objects are created/closed internally, caller doesn't need to care
 */
inline void* xp_mmap_dev(xp_dev_t h, size_t size) {
#if defined(WINDOWS)
    void* ptr = nullptr;
    size_t OutputBufferLen = sizeof(void*);
    DWORD bytesReturn = 0;

    BOOL ret = TRUE;
    ret = ::DeviceIoControl(h, IOC_AXCL_MAP, static_cast<void*>(&size), static_cast<DWORD>(sizeof(size_t)), &ptr,
                            static_cast<DWORD>(OutputBufferLen), &bytesReturn, nullptr);

    if (ret) {
        return ptr;
    } else {
        return nullptr;
    }
#else
    return ::mmap(nullptr, size, PROT_READ | PROT_WRITE, MAP_SHARED, h, 0);
#endif
}

/**
 * @brief Unmap device memory
 * @param addr Mapping start address
 * @param size Mapping length (bytes)
 * @return Returns true on success, false on failure
 */
inline bool xp_munmap_dev(xp_dev_t h, void* addr, size_t size) {
    (void)size;
    if (!addr) return false;
#if defined(WINDOWS)
    UCHAR OutputBuffer[32] = {0};
    size_t OutputBufferLen = sizeof(OutputBuffer);
    DWORD bytesReturn = 0;

    BOOL ret = TRUE;
    ret = ::DeviceIoControl(h, IOC_AXCL_UNMAP, &addr, static_cast<DWORD>(sizeof(void*)), static_cast<void*>(OutputBuffer),
                            static_cast<DWORD>(OutputBufferLen), &bytesReturn, nullptr);

    return ret == TRUE;
#else
    return ::munmap(addr, size) == 0;
#endif
}

/**
 * @brief Read data from device/file
 * @param handle Device handle (xp_dev_t) or file handle (xp_file_t)
 * @param buf Buffer to receive data
 * @param count Number of bytes to read
 * @return Returns actual bytes read on success, -1 on failure
 */
inline ssize_t xp_io_read(xp_dev_t handle, void* buf, size_t count) {
    if (handle == XP_INVALID_DEV || !buf || count == 0) {
        return -1;
    }

#if defined(WINDOWS)
    UCHAR InputBuffer[32] = {0};
    size_t InputBufferLen = sizeof(InputBuffer);
    DWORD bytesRead = 0;

    BOOL ret = TRUE;
    ret = ::DeviceIoControl(handle, IOC_AXCL_READ, static_cast<void*>(&InputBuffer), static_cast<DWORD>(InputBufferLen),
                            static_cast<void*>(buf), static_cast<DWORD>(count), &bytesRead, nullptr);

    if (ret) {
        return static_cast<ssize_t>(bytesRead);
    } else {
        return -1;
    }
#else
    return ::read(handle, buf, count);
#endif
}

/**
 * @brief Write data to device/file
 * @param handle Device handle (xp_dev_t) or file handle (xp_file_t)
 * @param buf Data buffer to write
 * @param count Number of bytes to write
 * @return Returns actual bytes written on success, -1 on failure
 */
inline ssize_t xp_io_write(xp_dev_t handle, const void* buf, size_t count) {
    if (handle == XP_INVALID_DEV || !buf || count == 0) {
        return -1;
    }

#if defined(WINDOWS)
    UINT64 OutputBuffer = 0;
    DWORD bytesWrite = 0;

    BOOL ret = TRUE;
    ret = ::DeviceIoControl(handle,
                            IOC_AXCL_WRITE,
                            const_cast<void*>(buf),  static_cast<DWORD>(count),
                            static_cast<void*>(&OutputBuffer), static_cast<DWORD>(sizeof(UINT64)),
                            &bytesWrite, nullptr);

    if (ret) {
        return static_cast<ssize_t>(OutputBuffer);
    } else {
        return -1;
    }
#else
    return ::write(handle, buf, count);
#endif
}

#if defined(WINDOWS)
inline BOOL poll_internal(xp_dev_t hDevice, PVOID input, DWORD inputlen, DWORD &statusMask, DWORD dTimeout) {
    BOOL ret = TRUE;
    DWORD bytesReturned = 0;

    OVERLAPPED ov = {0};
    ov.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);  // manual event, initial state is false

    if (!ov.hEvent) {
        CloseHandle(ov.hEvent);
        return FALSE;
    }

    // input, output buffer, may not be used, but please provide all
    // note: do not read statusMask here - it is only valid after I/O is completed
    BOOL ok = DeviceIoControl(hDevice, IOC_AXCL_POLL,
                              (LPVOID)input,              // input buffer
                              (DWORD)inputlen,            // input length
                              (LPVOID)&statusMask,        // output buffer(user buffer)
                              (DWORD)sizeof(statusMask),  // output length
                              &bytesReturned,             // note: do not read statusMask here - it is only valid after I/O is completed
                              &ov);

    if (ok) {
        // synchronous completion (Driver has completed the request before returning)
        // to unify processing, still get the final status/byte count through GetOverlappedResult
        if (GetOverlappedResult(hDevice, &ov, &bytesReturned, FALSE)) {
            // successful completion, statusMask has been written by the driver
            printf("DeviceIoControl completed synchronously, mask=0x%08X, bytes=%lu\n", statusMask, bytesReturned);
            ret = TRUE;
        } else {
            // abnormal: theoretically, DeviceIoControl should complete when returning TRUE, but still do protection
            DWORD gle = GetLastError();
            printf("GetOverlappedResult after sync-complete failed, err=%lu\n", gle);
            ret = FALSE;
        }
    } else {
        DWORD gle = GetLastError();
        if (gle == ERROR_IO_PENDING) {
            // asynchronous pending, wait for event or timeout
            DWORD wait = WaitForSingleObject(ov.hEvent, dTimeout);  // dTimeout can be set to INFINITE
            if (wait == WAIT_OBJECT_0) {
                // completed, read the result (bWait = FALSE because already waitforsingleobject signaled, asynchronous immediately
                // returns)
                if (GetOverlappedResult(hDevice, &ov, &bytesReturned, FALSE)) {
                    //   printf("Async completed, mask=0x%08X, bytes=%lu\n", statusMask, bytesReturned);
                    ret = TRUE;
                } else {
                    // driver directly returns error, enter this branch
                    printf("GetOverlappedResult after wait failed, err=%lu\n", GetLastError());
                    ret = FALSE;
                }
            } else if (wait == WAIT_TIMEOUT) {
                // timeout: cancel the specified request, then wait for it to complete (or be cancelled)
                printf("Timeout, cancelling request\n");
                CancelIoEx(hDevice, &ov);  // Win7+
                // wait for cancellation to complete and get the final result, TRUE synchronous wait for cancellation to complete
                if (GetOverlappedResult(hDevice, &ov, &bytesReturned, TRUE)) {
                    // very rare: cancellation may still succeed (depends on timing)
                    printf("Cancelled but completed, mask=0x%08X, bytes=%lu\n", statusMask, bytesReturned);
                } else {
                    DWORD gle2 = GetLastError();
                    if (gle2 == ERROR_OPERATION_ABORTED) {
                        printf("Request cancelled successfully (ERROR_OPERATION_ABORTED)\n");
                    } else {
                        printf("GetOverlappedResult after cancel failed, err=%lu\n", gle2);
                    }
                }
                ret = FALSE;
            } else {  // WAIT_FAILED or other
                printf("WaitForSingleObject failed, err=%lu\n", GetLastError());
                // try to cancel and finish
                CancelIoEx(hDevice, &ov);
                GetOverlappedResult(hDevice, &ov, &bytesReturned,
                                    TRUE);  // wait for finish(ignore result) TRUE synchronous wait for cancellation to complete
                ret = FALSE;
            }
        } else {
            // DeviceIoControl immediately failed (not PENDING)
            printf("DeviceIoControl failed immediately, err=%lu\n", gle);
            ret = FALSE;
        }
    }

    if (ret == TRUE) {
        // === simulate poll revents check ===
        if (statusMask == 0xFFFFFFFF) {
            ret = FALSE;  // all 1 represents driver directly returns
        } else if (statusMask & (AXCL_STATUS_POLLIN | AXCL_STATUS_POLLRDNORM)) {
            ret = TRUE;  // there is data to read, correctly return data
        } else if (statusMask & AXCL_STATUS_POLLNVAL) {
            ret = FALSE;  // simulate POLLNVAL
        } else if (statusMask & (AXCL_STATUS_POLLHUP | AXCL_STATUS_POLLREMOVE)) {
            ret = FALSE;  // simulate POLLHUP, normally call wakeup poll to close poll operation
        } else if (statusMask & AXCL_STATUS_ERROR) {
            ret = FALSE;  // simulate other error
        }
    }

    CloseHandle(ov.hEvent);
    return ret;
}

inline ssize_t xp_io_poll(xp_dev_t handle, PVOID input, DWORD inputlen, DWORD &statusMask, DWORD timeout) {
    if (handle == XP_INVALID_DEV) {
        return -1;
    }

    BOOL ret = poll_internal(handle, input, inputlen, statusMask, timeout);
    return ret ? 0 : -1;
}

#endif

/**
 * @brief Open file (unified interface)
 * @param path File path
 * @param flags Open flags
 * @return File handle, returns XP_INVALID_FILE on failure
 */
inline xp_file_t xp_fs_open_file(const char* path, uint32_t flags) {
#if defined(WINDOWS)
    return CreateFileA(path, flags, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
#else
    return open(path, flags);
#endif
}

/**
 * @brief Close file (unified interface)
 * @param handle File handle
 */
inline void xp_fs_close_file(xp_file_t handle) {
#if defined(WINDOWS)
    if (handle != XP_INVALID_FILE) {
        CloseHandle(handle);
    }
#else
    if (handle != XP_INVALID_FILE) {
        close(handle);
    }
#endif
}

/**
 * @brief Map file (unified interface)
 * @param handle File handle
 * @param size File size
 * @param flags Mapping flags
 * @return Mapped memory address, returns nullptr on failure
 */
inline void* xp_mmap_file(xp_file_t handle, size_t size, uint32_t flags) {
#if defined(WINDOWS)
    HANDLE mapping = CreateFileMappingA(handle, NULL, PAGE_READONLY, 0, 0, NULL);
    if (mapping == NULL) {
        return nullptr;
    }
    void* addr = MapViewOfFile(mapping, flags, 0, 0, size);
    CloseHandle(mapping);
    return addr;
#else
    return mmap(nullptr, size, flags, MAP_SHARED, handle, 0);
#endif
}

/**
 * @brief Unmap file (unified interface)
 * @param addr Mapped memory address
 * @param size Memory size
 * @return Returns true on success, false on failure
 */
inline bool xp_munmap_file(void* addr, size_t size) {
#if defined(WINDOWS)
    return UnmapViewOfFile(addr) != 0;
#else
    return munmap(addr, size) == 0;
#endif
}

/**
 * @brief Get current timestamp (milliseconds)
 * @return Current timestamp (milliseconds)
 */
inline uint64_t xp_time_get_tick_ms() {
#if defined(WINDOWS)
    auto now = std::chrono::steady_clock::now();
    auto duration = now.time_since_epoch();
    return std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
#else
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (ts.tv_sec * 1000 + ts.tv_nsec / 1000000);
#endif
}