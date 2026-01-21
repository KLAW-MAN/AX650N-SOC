### Sample for memcpy between host and device

         HOST          |               DEVICE
      host_mem[0] -----------> dev_mem[0]
                                    |---------> dev_mem[1]
      host_mem[1] <----------------------------------|

1. alloc 2 host memories: *host_mem[2]*
2. alloc 2 device memories: *dev_mem[2]*
3. memcpy from host_mem[0] to dev_mem[0] by AXCL_MEMCPY_HOST_TO_DEVICE
4. memcpy from dev_mem[0] to dev_mem[1] by AXCL_MEMCPY_DEVICE_TO_DEVICE
5. memcpy from dev_mem[1] to host_mem[0] by AXCL_MEMCPY_DEVICE_TO_HOST
6. memcmp between host_mem[0] and host_mem[1]

### Usage
```bash
usage: ./axcl_sample_memory [options] ...
options:
  -d, --device    device index from 0 to connected device num - 1 (int [=0])
      --json      axcl.json path (string [=./axcl.json])
  -?, --help      print this message
```

### Example

```bash
$ ./axcl_sample_memory  -d 0
[INFO ][                            main][  32]: ============== V2.26.1 sample started Feb 13 2025 11:09:59 ==============
[INFO ][                           setup][ 112]: json: ./axcl.json
[INFO ][                           setup][ 131]: device index: 0, bus number: 129
[INFO ][                            main][  51]: alloc host and device memory, size: 0x800000
[INFO ][                            main][  63]: memory [0]: host 0xffff967fb010, device 0x14926f000
[INFO ][                            main][  63]: memory [1]: host 0xffff95ffa010, device 0x149a6f000
[INFO ][                            main][  69]: memcpy from host memory[0] 0xffff967fb010 to device memory[0] 0x14926f000
[INFO ][                            main][  75]: memcpy device memory[0] 0x14926f000 to device memory[1] 0x149a6f000
[INFO ][                            main][  81]: memcpy device memory[1] 0x149a6f000 to host memory[0] 0xffff95ffa010
[INFO ][                            main][  88]: compare host memory[0] 0xffff967fb010 and host memory[1] 0xffff95ffa010 success
[INFO ][                         cleanup][ 146]: deactive device 129 and cleanup axcl
[INFO ][                            main][ 106]: ============== V2.26.1 sample exited Feb 13 2025 11:09:59 ==============
```
