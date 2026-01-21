# UsrWorker

## Definition
**usrworker**: An application running in device (EP).



## Overview

AXCL runtime supports to run **usrworker** application in device (EP) ,  the sequence is:

1. Host transfers the **usrworker** application to the specified device directory.
2. Host sends commands to device to run the **usrworker** application.
3. Host and **usrworker** support full-duplex communication through specified APIs.
4. Host sends commands to terminate the usrworker.



## Example

SDK provides an example named **axcl_run_usrworker** to show how to run **usrworker** named **sample_usrworker_device** in device.
> host sample : `/usr/bin/axcl/axcl_run_usrworker`
> **usrworker**   : `/usr/bin/axcl/usrworker/sample_usrworker_device`
> source code : `axcl/sample/usrworker`

```bash
usage: ./axcl_run_usrworker--usrworker=string [options] ...
options:
  -i, --usrworker    usrworker application launched in device (string)
  -d, --device       device index from 0 to connected device num - 1 (int [=0])
      --json         axcl.json path (string [=./axcl.json])
  -?, --help         print this message
```

```bash
# axcl_run_usrworker -i /opt/bin/axcl/sample_usrworker_device
[INFO ][                   active_device][  72]: device index: 0, bus number: 129
[INFO ][                launch_usrworker][  96]: transfer /opt/bin/axcl/sample_usrworker_device to /opt/bin/sample_usrworker
[INFO ][                launch_usrworker][ 107]: launch /opt/bin/sample_usrworker
[INFO ][                launch_usrworker][ 116]: send: Hello World!
[INFO ][                launch_usrworker][ 129]: recv: Hello World!
[INFO ][                launch_usrworker][ 135]: usrworker 1003 terminated
```



## **usrworker** Development

**usrworker** application is forked and started by the **slave_worker** application in device. The **slave_worker** application continuously monitors the **usrworker** keep-alive heartbeat through IPC.

AXCL provides **libaxclrt_worker.so** communication library which supports:
- IPC with **slave_worker** application for keep-alive heartbeat;
- Full-duplex data communication with host.

**usrworker** must link **libaxclrt_worker.so**, refer to source code`axcl/sample/usrworker/device`:

```C++
int main(int argc, char* argv[]) {
    /* initialize worker runtime */
    axclrtWorkerInit();

    /* TODO: add code here */

    /* deinitialize worker runtime */
    axclrtWorkerDeInit();
    return 0;
}
```



### Communication API

> header file : `axcl/include/external/axcl_worker_runtime.h`
> so bin file   : `msp/out/lib/libaxclrt_worker.so`

#### axclrtWorkerInit
​	Communication initialization.

##### Definintion

​	*int32_t axclrtWorkerInit()*;

##### Parameters
​	NA

##### Return
​	0: success, other indicates error

##### Description
- Called in the `main` function at process entry.
- Call `axclrtWorkerDeInit` to deinitialization.



#### axclrtWorkerDeInit

​        Communication deinitialization.

##### Definintion
​	*int32_t axclrtWorkerDeInit()*;

##### Parameters
​	NA

##### Return
​	0: success, other indicates error

##### Description
   NA



#### axclrtWorkerSend

​         Send data to host.

##### Definintion
​	*int32_t axclrtWorkerSend(const void \*buf, uint32_t size, int32_t timeout);*

##### Parameters
| Parameter | Definition      | Type | Description           |
| --------- | --------------- | ---- | --------------------- |
| buf       | const void \*   | Input | Data pointer to send  |
| size      | uint32_t        | Input | Data length in bytes  |
| timeout   | int32_t         | Input | Timeout in milliseconds |

##### Return
​	0 success, other indicates error

##### Description
​	NA



#### axclrtWorkerRecv

​        Receive data from host.

##### Definintion
​	*int32_t axclrtWorkerRecv(void \*buf, uint32_t buf_size, uint32_t \*recvlen, int32_t timeout);*

##### Parameters
| Parameter | Definition      | Type | Description                    |
| --------- | --------------- | ---- | ------------------------------ |
| buf       | void \*         | Output | Receive data buffer pointer    |
| buf_size  | uint32_t        | Input | Receive buffer size in bytes   |
| recvlen   | uint32_t \*     | Output | Return data bytes read from HOST |
| timeout   | int32_t         | Input | Timeout in milliseconds        |

##### Return
​	0 success, other indicates error

##### Description
- Receive data buffer should be allocated and freed by user.
- If buffer is too small, API returns `AXCL_ERR_USRWORK_BUFFER_TOO_SMALL`.
- timeout < 0 means wait indefinitely until host has data.
- `axclrtWorkerSend` and `axclrtWorkerRecv` API don't care about data content, communication protocol is defined by **usrworker**.



### Compilation

- Since **usrworker** is executed in device,  so **usrworker**  should be compiled with **device compilation toolchain**, not host compilation toolchain.
- **usrworker** is transferred from host to deviceramdisk, please ensure ramdisk has sufficient space to store.



# Run Usrworker

AXCL runtime(`libaxcl_rt.so`) provides file transfer, run, communication and process termination APIs.
> header file: `axcl/include/external/axcl_rt_usrwork.h`
> so bin file  : `libaxcl_rt.so`

### axclrtTransferFile
​        Transfer file between host and device.

##### Definintion
​	*axclError axclrtTransferFile(const char \*src_path, const char \*dst_path, axclrtFileTransferPolicy policy)*;

##### Parameters
| Parameter | Definition                     | Type | Description                                                         |
| --------- | ------------------------------ | ---- | ------------------------------------------------------------------- |
| src_path  | const char \*                  | Input | Source file absolute path                                           |
| dst_path  | const char \*                  | Input | Target file absolute path                                           |
| policy    | axclrtFileTransferPolicy       | Input | **FILE_TRANSFER_FROM_HOST_TO_DEVICE**    : Transfer file from host to device<br />**FILE_TRANSFER_FROM_DEVICE_TO_HOST **   : Transfer file from device to host <br />**FILE_TRANSFER_FROM_DEVICE_TO_DEVICE **: Copy file within device<br />**FILE_TRANSFER_REMOVE_DEVICE_FILE**         : Delete device file |

##### Return
​	0 success, other indicates error

##### Description
​	Ensure  ramdisk capacity of device is sufficient for storage.



### axclrtExecWorker
​        Run **usrworker**.

##### Definintion
​	*axclError axclrtExecWorker(const char \*path, const int32_t \*argc, const char \*argv[], uint32_t \*pid);*

##### Parameters
| Parameter | Definition             | Type | Description                                                 |
| --------- | ---------------------- | ---- | ----------------------------------------------------------- |
| path      | const char \*          | Input | **usrworker** absolute path, transferred by `axclrtTransferFile` from host. |
| argc      | const int32_t \*       | Input | Number of parameters passed to **usrworker**.  0 means no parameters. |
| argv      | const char \**         | Input | Individual parameter strings passed to **usrworker**         |
| pid       | uint32_t \*            | Output | **usrworker** pid                                            |

##### Return
​	0 success, other indicates error

##### Description
- `argc` does not include *the program name* or *The name by which the program was invoked*.
- Call `axclrtKillWorker` to terminate **usrworker**.



### axclrtKillWorker

​       Terminate **usrworker**.

##### Definintion
​	*axclError axclrtKillWorker(uint32_t pid);*

##### Parameters
| Parameter | Definition | Type | Description        |
| --------- | ---------- | ---- | ------------------ |
| pid       | uint32_t   | Input | **usrworker** pid   |

##### Return
​	0 success, other indicates error

##### Description
​	Regardless of whether the **usrworker** exits automatically,  host must call this API to release resources.**



### axclrtWorkerSend

​        Send data to device.

##### Definintion
​	*axclError axclrtWorkerSend(uint32_t pid, const void \*buf, uint32_t size, int32_t timeout);*

##### Parameters
| Parameter | Definition      | Type | Description           |
| --------- | --------------- | ---- | --------------------- |
| pid       | uint32_t        | Input | **usrworker** pid      |
| buf       | const void \*   | Input | Send data buffer pointer |
| size      | uint32_t        | Input | Send data bytes       |
| timeout   | int32_t         | Input | Send timeout in milliseconds |

##### Return
​	0 success, other indicates error

##### Description
​	NA



### axclrtWorkerRecv

​        Read data from device.

##### Definintion
​	*axclError axclrtWorkerRecv(uint32_t pid, void \*buf, uint32_t bufsize, uint32_t \*recvlen, int32_t timeout);*

##### Parameters
| Parameter | Definition      | Type | Description                    |
| --------- | --------------- | ---- | ------------------------------ |
| pid       | uint32_t        | Input | **usrworker** pid               |
| buf       | void \*         | Output | Receive data buffer pointer    |
| bufsize   | uint32_t        | Input | Receive data buffer size in bytes |
| recvlen   | uint32_t \*     | Output | Actual received data in bytes  |
| timeout   | int32_t         | Input | Send timeout in milliseconds   |

##### Return
​	0 success, other indicates error

##### Description
- Receive data buffer should be allocated and freed by user.
- If buffer is too small, API returns `AXCL_ERR_USRWORK_BUFFER_TOO_SMALL`.
- timeout < 0 means wait indefinitely until device has data.
- `axclrtWorkerSend` and `axclrtWorkerRecv` API don't care about data content, communication protocol is defined by **usrworker**.
