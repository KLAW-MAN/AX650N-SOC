### runtime sample
1. Initialize axcl runtime by axclrtInit.
2. Active EP by axclrtSetDevice.
3. Create context by axclrtCreateDevice for main thread. (optional)
4. Create and destory thread context. (must)
5. Destory context of main thread.
6. Deactive EP by axclrtResetDevice
7. Deinitialize runtime by axclFinalize


### usage
```bash
usage: ./axcl_sample_runtime [options] ...
options:
  -d, --device    device index [-1, connected device num - 1], -1: traverse all devices (int [=-1])
      --json      axcl.json path (string [=./axcl.json])
  -?, --help      print this message
```

### example

```bash
$ ./axcl_sample_runtime
[INFO ][                            main][  22]: ============== V3.0.0 sample started Mar 12 2025 16:21:21 ==============
[INFO ][                      operator()][ 104]: malloc 1048576 bytes memory from device 13 success, addr = 0x14926f000
[INFO ][                      operator()][ 104]: malloc 1048576 bytes memory from device 11 success, addr = 0x14926f000
[INFO ][                      operator()][ 104]: malloc 1048576 bytes memory from device 14 success, addr = 0x14926f000
[INFO ][                      operator()][ 104]: malloc 1048576 bytes memory from device  5 success, addr = 0x14926f000
[INFO ][                      operator()][ 104]: malloc 1048576 bytes memory from device  3 success, addr = 0x14926f000
[INFO ][                      operator()][ 104]: malloc 1048576 bytes memory from device 10 success, addr = 0x14926f000
[INFO ][                      operator()][ 104]: malloc 1048576 bytes memory from device  4 success, addr = 0x14926f000
[INFO ][                      operator()][ 104]: malloc 1048576 bytes memory from device  9 success, addr = 0x14926f000
[INFO ][                            main][ 127]: ============== V3.0.0 sample exited Mar 12 2025 16:21:21 ==============
```
