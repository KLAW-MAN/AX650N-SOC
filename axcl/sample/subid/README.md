### Brief
This guide explains how to modify the sub vendor ID or sub device ID of a PCIe EP device with NOR flash storage.

:::{Caution}
- Ensure the HOST driver has been updated accordingly; otherwise the device cannot be probed.
- PCIe sub id can only be changed on NOR flash.
:::

### Usage
```bash
usage: ./axcl_change_subid [options] ...
options:
  -d, --device       device index from 0 to connected device num - 1 (unsigned int [=0])
      --subvendor    sub vendor id (decimal) (unsigned int [=8011])
      --subdevice    sub device id (decimal) (unsigned int [=1616])
      --json         axcl.json path (string [=./axcl.json])
  -?, --help         print this message
```

### Example

```bash
# change the 1st connected device sub id to 0x650
$ ./axcl_change_subid --subdevice=1616 -d 0
```
