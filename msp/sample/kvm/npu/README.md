# NPU Merge

## Compile and Run
1. Place sample_kvm_npu/ in the msp/sample/kvm/npu directory
2. cd msp/sample/kvm/npu && make p=AX650_emmc clean all install
3. Run on board
   - Copy msp/out/bin/sample_kvm_npu, sample_kvm_npu/rgb/src_3840x2160.rgb, and sample_kvm_npu/model/algo1_nv12_to_rgb_3840x2160_0718.axmodel to the board
   - ./sample_kvm_npu -m algo1_nv12_to_rgb_3840x2160_0718.axmodel -i src_3840x2160.rgb -w 3840 -h 2160 --algo 1 -r 10



## Usage
```bash
usage: ./sample_kvm_npu --axmodel_path=string --input=string --width=unsigned int --height=unsigned int [options] ...
options:
  -m, --axmodel_path    axmodel path (string)
  -i, --input           rgb888 file path (string)
  -w, --width           image width (unsigned int)
  -h, --height          image width (unsigned int)
  -r, --repeat          repeat times, default: 1 (unsigned int [=1])
      --itcm            itcm.bin path, default: /opt/data/dsp/itcm.bin (string [=/opt/data/dsp/itcm.bin])
      --sram            sram.bin path, default: /opt/data/dsp/sram.bin (string [=/opt/data/dsp/sram.bin])
  -o, --output          image output directory (string [=./])
      --algo            split and merge algorithm (unsigned int [=0])
  -?, --help            print this message
```
--input: Input RGB888 image size must match the model size
--algo: Supports algo0(YV0+UV1) and algo1(NV12+UV12), vdsp and npu model algo must be consistent

 ## Model Description
 The sample_kvm_npu/model/ directory contains some supported resolution models, which need to be consistent with the input image size.


 ## Running Examples
 ### algo0
  ```bash
/opt/bin # ./sample_kvm_npu -m ./algo0_nv12_to_rgb_3840x2160_v2_0730.axmodel -i
 src_3840x2160.rgb -w 3840 -h 2160 --algo 0 -r 10
[I][       split_rgb_to_nv12_by_vdsp][  85]: split vdsp algo = 0x0
[I][       merge_nv12_to_rgb_by_vdsp][ 117]: merge vdsp algo = 0x0
[I][                            main][  79]: dsp output is saved to .//algo0_output_3840x2160.dsp.rgb

input size: 2
    name:      uv1 [UINT8] [FEATUREMAP]
        1 x 3240 x 3840 x 1

    name:      yv0 [UINT8] [FEATUREMAP]
        1 x 3240 x 3840 x 1


output size: 1
    name:      rgb [UINT8]
        1 x 2160 x 3840 x 3

[I][                 do_merge_by_npu][ 133]: elapsed[   1] = 10663 us
[I][                 do_merge_by_npu][ 133]: elapsed[   2] = 10545 us
[I][                 do_merge_by_npu][ 133]: elapsed[   3] = 10525 us
[I][                 do_merge_by_npu][ 133]: elapsed[   4] = 10535 us
[I][                 do_merge_by_npu][ 133]: elapsed[   5] = 10527 us
[I][                 do_merge_by_npu][ 133]: elapsed[   6] = 10547 us
[I][                 do_merge_by_npu][ 133]: elapsed[   7] = 10529 us
[I][                 do_merge_by_npu][ 133]: elapsed[   8] = 10530 us
[I][                 do_merge_by_npu][ 133]: elapsed[   9] = 10514 us
[I][                 do_merge_by_npu][ 133]: elapsed[  10] = 10511 us
[I][                 do_merge_by_npu][ 139]: min = 10511, avg = 10542, max = 10663 us
[I][                            main][  93]: npu output is saved to .//algo0_output_3840x2160.npu.rgb
```

 ### algo1
 ```bash
/opt/bin # ./sample_kvm_npu -m algo1_nv12_to_rgb_3840x2160_0718.axmodel -i src_3840x2160.rgb -w 3840 -h 2160 --algo 1 -r 10
[I][       split_rgb_to_nv12_by_vdsp][  85]: split vdsp algo = 0x10
[I][       merge_nv12_to_rgb_by_vdsp][ 117]: merge vdsp algo = 0x10
[I][                            main][  79]: dsp output is saved to .//algo1_output_3840x2160.dsp.rgb

input size: 3
    name:       uv [UINT8] [FEATUREMAP]
        1 x 1080 x 3840 x 1

    name:  uv_left [UINT8] [FEATUREMAP]
        1 x 3240 x 3840 x 1

    name:        y [UINT8] [FEATUREMAP]
        1 x 2160 x 3840 x 1


output size: 1
    name:      rgb [UINT8]
        1 x 2160 x 3840 x 3

[I][                 do_merge_by_npu][ 133]: elapsed[   1] = 14171 us
[I][                 do_merge_by_npu][ 133]: elapsed[   2] = 14077 us
[I][                 do_merge_by_npu][ 133]: elapsed[   3] = 14049 us
[I][                 do_merge_by_npu][ 133]: elapsed[   4] = 14068 us
[I][                 do_merge_by_npu][ 133]: elapsed[   5] = 14117 us
[I][                 do_merge_by_npu][ 133]: elapsed[   6] = 14048 us
[I][                 do_merge_by_npu][ 133]: elapsed[   7] = 14086 us
[I][                 do_merge_by_npu][ 133]: elapsed[   8] = 14056 us
[I][                 do_merge_by_npu][ 133]: elapsed[   9] = 14107 us
[I][                 do_merge_by_npu][ 133]: elapsed[  10] = 14037 us
[I][                 do_merge_by_npu][ 139]: min = 14037, avg = 14081, max = 14171 us
[I][                            main][  93]: npu output is saved to .//algo1_output_3840x2160.npu.rgb
```