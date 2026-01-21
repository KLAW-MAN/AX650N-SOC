#ifndef __AXCL_WORKER_RUNTIME_H__
#define __AXCL_WORKER_RUNTIME_H__

#include "axcl_base.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Worker status codes
 */
typedef enum {
    AXCL_WORKER_STATUS_OK = 0,
    AXCL_WORKER_STATUS_PARENT_DEAD = 1,
    AXCL_WORKER_STATUS_MAX
} AXCL_WORKER_STATUS_E;

/**
 * @brief Initialize the runtime for user worker process.
 *        This function should be invoked at first.
 *        The runtime will automatically wait for configuration from parent process
 *        and start heartbeat monitoring and PCIE listening threads.
 * @return 0 if success, < 0 if failed.
 */
 AXCL_EXPORT int32_t axclrtWorkerInit();

/**
 * @brief Deinitialize the runtime for user worker process.
 *        This function should be invoked at last.
 *
 */
AXCL_EXPORT int32_t axclrtWorkerDeInit();

/**
 * @brief Send data via PCIE interface.
 *        This interface is used for PCIE communication, not for parent-child process IPC.
 *        The PCIE ports are configured via IPC during initialization.
 * @param buf The data to send via PCIE.
 * @param size The size of the data to send.
 * @param timeout The timeout in milliseconds, if < 0, INFINITE.
 * @return The number of bytes sent.  if < 0, failed.
 */
AXCL_EXPORT int32_t axclrtWorkerSend(const void *buf, uint32_t size, int32_t timeout);

/**
 * @brief Receive data via PCIE interface.
 *        This interface is used for PCIE communication, not for parent-child process IPC.
 *        The PCIE ports are configured via IPC during initialization.
 * @param buf The buffer to receive the PCIE data.
 * @param buf_size The size of the buffer.
 * @param recvlen The length of the received data.
 * @param timeout The timeout in milliseconds, if < 0, INFINITE.
 * @return The number of bytes received.  if < 0, failed.
 */
AXCL_EXPORT int32_t axclrtWorkerRecv(void *buf, uint32_t buf_size, uint32_t* recvlen, int32_t timeout);

/**
 * @brief Register parent process exception callback
 * @param cb Callback function pointer, parameter is status code (AXCL_WORKER_STATUS_OK=normal, AXCL_WORKER_STATUS_PARENT_DEAD=parent process exception, etc.)
 */
typedef void (*axclrtWorkerStatusCallback)(AXCL_WORKER_STATUS_E status);
AXCL_EXPORT void axclrtWorkerSetStatusCallback(axclrtWorkerStatusCallback cb);

#ifdef __cplusplus
}
#endif

#endif /* __AXCL_WORKER_RUNTIME_H__ */