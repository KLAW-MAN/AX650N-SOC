### Description

The sample code here is for the IVE (Intelligent Video Analysis Engine) module provided whithin the Aixin SDK package, which facilitates customers to quickly understand and rightly use these IVE related interfaces.
`axcl_sample_ive` is generated with this smaple code and located at the directory of opt/bin，which is showing how to use it.

### Usage
```bash
Usage : ./axcl_sample_ive -c case_index [options]
        -d | --device_id: Device index from 0 to connected device num - 1, optional
        -c | --case_index:Calc case index, default:0
                0-DMA.
                1-DualPicCalc.
                2-HysEdge and CannyEdge.
                3-CCL.
                4-Erode and Dilate.
                5-Filter.
                6-Hist and EqualizeHist.
                7-Integ.
                8-MagAng.
                9-Sobel.
                10-GMM and GMM2.
                11-Thresh.
                12-16bit to 8bit.
                13-Multi Calc.
                14-Crop and Resize.
                15-CSC.
                16-CropResize2.
                17-MatMul.
        -e | --engine_choice:Choose engine id, default:0
                0-IVE; 1-TDP; 2-VGP; 3-VPP; 4-GDC; 5-DSP; 6-NPU; 7-CPU; 8-MAU.
                For Crop and Resize case, cropimage support IVE/VGP/VPP engine, cropresize and cropresize_split_yuv support VGP/VPP engine.
                For CSC case, support TDP/VGP/VPP engine.
                For CropResize2 case, support VGP/VPP engine.
                For MatMul case, support NPU/MAU engine.
        -m | --mode_choice:Choose test mode, default:0
                For DualPicCalc case, indicate dual pictures calculation task:
                  0-add; 1-sub; 2-and; 3-or; 4-xor; 5-mse.
                For HysEdge and CannyEdge case, indicate hys edge or canny edge calculation task:
                  0-hys edge; 1-canny edge.
                For Erode and Dilate case, indicate erode or dilate calculation task:
                  0-erode; 1-dilate.
                For Hist and EqualizeHist case, indicate hist or equalize hist calculation task:
                  0-hist; 1-equalize hist.
                For GMM and GMM2 case, indicate gmm or gmm2 calculation task:
                  0-gmm; 1-gmm2.
                For Crop and Resize case, indicate cropimage, cropresize, cropresize_split_yuv calculation task:
                  0-crop image; 1-crop_resize; 2-cropresize_split_yuv.
                For CropResize2 case, indicate crop_resize2 or cropresize2_split_yuv calculation task:
                  0-crop_resize2; 1-cropresize2_split_yuv.
        -t | --type_image:Image type index refer to IVE_IMAGE_TYPE_E(IVE engine) or AX_IMG_FORMAT_E(other engine)
                Note:
                  1. For all case, both input and output image types need to be specified in the same order as the specified input and output file order.
                  2. If no type is specified, i.e. a type value of -1 is passed in, then a legal type is specified, as qualified by the API documentation.
                  3. Multiple input and output image types, separated by spaces.
                  4. For One-dimensional data (such as AX_IVE_MEM_INFO_T type data), do not require a type to be specified.
        -i | --input_files:Input image files, if there are multiple inputs, separated by spaces.
        -o | --output_files:Output image files or dir, if there are multiple outputs, separated by spaces
                Note:for DMA, Crop Resize, blob of CCL case and CropResize2 case must be specified as directory.
        -w | --width:Image width of inputs, default:1280.
        -h | --height:Image height of inputs, default:720.
        -p | --param_list:Control parameters list or file(in json data format)
                Note:
                  5. Please refer to the json file in the '/opt/data/ive/' corresponding directory of each test case.
                  6. For MagAng, Multi Calc and CSC case, no need control parameters.
        -a | --align_need:Does the width/height/stride need to be aligned automatically, default:0.
                  0-no; 1-yes.
        -? | --help:Show usage help.
```

### Examples

> [!NOTE]
>
> - Sample code is only for API demo, but in fact specific configrature parameter is needed according to user context.
> - Please refer to document named "42 - AX IVE API"  for paramter limition.
> - Memory filled with input and output data must be alloced by user.
> - Image data of input and output must be specified by user.
> - The number of input images (or data) from different CV may not be similar.
> - The data type of 2-D images must be defined clearly ,or as default value.
> - These Key parameter is formated as Json string or Json file. Please refer to .json file and code in some directories of /opt/data/ive/.

1. show help text
   ```bash
   ./axcl_sample_ive -?
   ```

2. DMA usage (source resolution: 1280 x 720, input/output type : U8C1, Json file used to config control paramter)
   ```bash
   ./axcl_sample_ive -c 0 -w 1280 -h 720 -i /opt/data/ive/common/1280x720_u8c1_gray.yuv -o /opt/data/ive/dma/ -t 0 0 -p /opt/data/ive/dma/dma.json
   ```

3. MagAndAng usage (source resolution: 1280 x 720, input parameter(grad_h, grad_v)'s data type: U16C1, output parameter (ang_output)'s data type : U8C1)
   ```bash
   ./axcl_sample_ive -c 8 -w 1280 -h 720 -i /opt/data/ive/common/1280x720_u16c1_gray.yuv /opt/data/ive/common/1280x720_u16c1_gray_2.yuv -o /opt/data/ive/common/mag_output.bin /opt/data/ive/common/ang_output.bin -t 9 9 9 0
   ```

### Key parameter in Json file
1. **dma.json**
   - `mode`, `x0`, `y0`, `h_seg`, `v_seg`, `elem_size` and `set_val` are the value of respective member in structure `AX_IVE_DMA_CTRL_T` such as `enMode`, `u16CrpX0`, `u16CrpY0`, `u8HorSegSize`, `u8VerSegRows`, `u8ElemSize`, `u64Val`.
   - `w_out` and `h_out` is respectivly the width and height of output image, only for `AX_IVE_DMA_MODE_DIRECT_COPY` mode in DMA.
2. **dualpics.json**
   - `x` and `y` are the value of `u1q7X` and `u1q7Y` in structure `AX_IVE_ADD_CTRL_T` for ADD CV.
   - `mode` is the value of `enMode` in structure `AX_IVE_SUB_CTRL_T` for Sub CV.
   - `mse_coef` is the value of `u1q15MseCoef` in structure `AX_IVE_MSE_CTRL_T` for MSE CV.
3. **ccl.json**
   - `mode` is the value of `enMode` of structure `AX_IVE_CCL_CTRL_T` for CCL CV.
4. **ed.json**
   - `mask` is all values of `au8Mask[25]` in structure `AX_IVE_ERODE_CTRL_T` for Erode CV or `AX_IVE_DILATE_CTRL_T` for Dilate CV.
5. **filter.json**
   - `mask` is all values of `as6q10Mask[25]` in structure `AX_IVE_FILTER_CTRL_T` for Filter CV.
6. **hist.json**
   - `histeq_coef` is the value of `u0q20HistEqualCoef` in structure `AX_IVE_EQUALIZE_HIST_CTRL_T` for EqualizeHist CV.
7. **integ.json**
   - `out_ctl` is the value of `enOutCtrl` in structure `AX_IVE_INTEG_CTRL_T` for Integ CV.
8. **sobel.json**
   - `mask` is the value of `as6q10Mask[25]` in structure `AX_IVE_SOBEL_CTRL_T` for Sobel CV.
9. **gmm.json**
   - `init_var`, `min_var`, `init_w`, `lr`, `bg_r`, `var_thr` and `thr` are respectivly the value of `u14q4InitVar`, `u14q4MinVar`, `u1q10InitWeight`, `u1q7LearnRate`, `u1q7BgRatio`, `u4q4VarThr` and `u8Thr` in structure `AX_IVE_GMM_CTRL_T` for GMM CV.
10. **gmm2.json:**
    - `init_var`, `min_var`, `max_var`, `lr`, `bg_r`, `var_thr`, `var_thr_chk`, `ct` and `thr` are respectivly the value of `u14q4InitVar`, `u14q4MinVar`, `u14q4MaxVar`, `u1q7LearnRate`, `u1q7BgRatio`, `u4q4VarThr`, `u4q4VarThrCheck`, `s1q7CT` and `u8Thr` in structure `AX_IVE_GMM2_CTRL_T` for GMM2 CV.
11. **thresh.json**
    - `mode`, `thr_l`, `thr_h`, `min_val`, `mid_val` and `max_val` are repectivly the value of `enMode`, `u8LowThr`, `u8HighThr`, `u8MinVal`, `u8MidVal` and `u8MaxVal` in structure `AX_IVE_THRESH_CTRL_T` for Thresh CV.
12. **16bit_8bit.json**
    - `mode`, `gain` and `bias` are repectivly the value of `enMode`, `s1q14Gain` and `s16Bias` in structure `AX_IVE_16BIT_TO_8BIT_CTRL_T` for 16BitTo8Bit CV.
13. **crop_resize.json**
    - When CropImage is enabled, num is the value of `u16Num` in structure `AX_IVE_CROP_IMAGE_CTRL_T` and boxs is the array type of crop image in which `x`,`y`,`w` and `h` are respecivly the value of `u16X`, `u16Y`, `u16Width` and `u16Height` in structure `AX_IVE_RECT_U16_T`.
    - When CropResize or CropResizeForSplitYUV mode is enabled, `num` is the value of `u16Num` in structure `AX_IVE_CROP_RESIZE_CTRL_T` and `align0`, `align1`, `enAlign[1]`, `bcolor`, `w_out` and `h_out` are respectivly the value of `enAlign[0]`, `enAlign[1]`, `u32BorderColor`, `width` and `height` of output image.
14. **crop_resize2.json**
    - `num` is the value of `u16Num` in structure `AX_IVE_CROP_IMAGE_CTRL_T`.
    - `res_out` is the arrayes of width and height of output image.
    - **`src_boxs` is the array of cropped range from source image, `dst_boxs` is the array of range to resized image.**
15. **matmul.json**
    - `mau_i`, `ddr_rdw`, `en_mul_res`, `en_topn_res`, `order` and `topn` are respectivly the value of `enMauId`, `s32DdrReadBandwidthLimit`, `bEnableMulRes`, `bEnableTopNRes`, `enOrder` and `s32TopN` in structure `AX_IVE_MAU_MATMUL_CTRL_T`.
    - `type_in` is the value of `stMatQ` and `stMatB` in structure `AX_IVE_MAU_MATMUL_INPUT_T`.
    - `type_mul_res` and `type_topn_res` are the value of `stMulRes` and `sfTopNRes` in structure `AX_IVE_MAU_MATMUL_OUTPUT_T`.
    - `q_shape` and `b_shape` are the value of `pShape` in `stMatQ` and `stMatB` of structure `AX_IVE_MAU_MATMUL_INPUT_T`.