
1）Function Description：

The sample code here is for the IVE (Intelligent Video Analysis Engine) module provided whithin the Aixin SDK package, which facilitates customers to quickly understand and rightly use these IVE related interfaces.
sample_ive is generated with this smaple code and located at the directory of opt/bin，which is showing how to use it.

The parameter description is as follows:
-c | --case_index: default value：0
    0-DMA
    1-DualPicCalc (Add、Sub、And、Or、Xor、Mse)
    2-HysEdge/CannyEdge
    3-CCL
    4-Erode/Dilate
    5-Filter 5x5
    6-Hist/EqualizeHist
    7-Integ
    8-MagAng
    9-Sobel 5x5 Sobel-like Gradient Calc
    10-GMM GMM/GMM2
    11-Thresh
    12-16BitTo8Bit
    13-DMA-Sub-Hist
    14-Crop Resize (CropImage、CropResize、CropResizeForSplitYUV)
    15-CSC
    16-CropResize2
    17-MatMul
-e | --engine_choice:
    0-IVE; 1-TDP; 2-VGP; 3-VPP; 4-GDC; 5-DSP; 6-NPU; 7-CPU; 8-MAU.
    CropImage : one of IVE/VGP/VPP hardware engine can be used for it
    CropResize/CropResizeForSplitYUV : one of VGP/VPP hardware engine can be uesd.
    CSC: one of TDP/VGP/VPP engine can be used for it.
    CropResize2/CropResize2ForSplitYUV: VGP or VPP engine can be used.
-m | --mode_choice: default value: 0
    For DualPicCalc, one of these CVes is enabled with the below option:
        0-add; 1-sub; 2-and; 3-or; 4-xor; 5-mse.
    For HysEdge/CannyEdge，HysEdge or CannyEdge is enabled with
        0-hys edge; 1-canny edge.
    For Erode/Dilate, Erode or Dilate is enabled with the below option:
        0-erode; 1-dilate.
    For Hist/EqualizeHist，Hist or EqualizeHist is enabled with the below option:
        0-hist; 1-equalize hist.
    For GMM, GMM or GMM2 is enabled with the below option:
        0-gmm; 1-gmm2.
    For Crop Resize，CropImage, CropResize or CropResizeForSplitYUV is enabled with the below option:
        0-crop image; 1-crop_resize; 2-cropresize_split_yuv.
    For CropResize2，CropResize2 or CropResize2ForSplitYUV is enabled with the below option:
        0-crop_resize2; 1-cropresize2_split_yuv.
-t | --type_image: image type index from enum type of AX_IVE_IMAGE_TYPE_E and AX_IMG_FORMAT_E(if the engine is IVE, please refer to
        AX_IVE_IMAGE_TYPE_E ，or else please refer to AX_IMG_FORMAT_E)
    Note:
        1. For all tests, the input and output image types need to be specified in the order of the specified input and output files.
        2. If no type is specified, that is, if the passed type value is -1, then a valid type is specified according to the API    documentation.
        3. Multiple input/output image types separated by spaces.
        4. One dimensional data types (such as AX_IVE_MEM_INFO_T type) do not require specifying a type.
-i | --input_files: Input image (or data) files, separated by spaces if there are multiple inputs.
-o | --output_files: Output image (or data) files or directories, separated by spaces if there are multiple outputs
    Note：
        1. For specific information, please refer to the JSON files in the/opt/data/eve/directory corresponding to each test case.
        2. For DMA, Crop Resize, blob in CCL, and Crop Resize2, the output directory must be specified.
-w | --width: Default value: 1280
-h | --height: Defalt Valie: 720
-p | --param_list: control parameter list or file formated with JSON
    Note: not need for MagAng、Multi Calc or CSC.
-a | --align_need: align for width, height and stride is enabled or disabled, default value : 0
    0-no; 1-yes.
-? | --help: showing text of how to use it。


2）Sample Demo：

Example One：show help text
./sample_ive -?

Example Two: DMA usage (source resolution: 1280 x 720, input/output type : U8C1, Json file used to config control paramter)
./sample_ive -c 0 -w 1280 -h 720 -i /opt/data/ive/common/1280x720_u8c1_gray.yuv -o /opt/data/ive/dma/ -t 0 0 -p /opt/data/ive/dma/dma.json

Example Three: MagAndAng usage (source resolution: 1280 x 720, input parameter(grad_h, grad_v)'s data type: U16C1, output parameter
                (ang_output)'s data type : U8C1)
./sample_ive. -c 8 -w 1280 -h 720 -i /opt/data/ive/common/1280x720_u16c1_gray.yuv /opt/data/ive/common/1280x720_u16c1_gray_2.yuv -o /opt/data/ive/common/mag_output.bin /opt/data/ive/common/ang_output.bin -t 9 9 9 0

3) Result:
the expected image or data file will be generated in output directory after it run successfullly .


4）Note：

    a)sample code is only for API demo, but in fact specific configrature parameter is needed according to user context.
    b)Please refer to document named "42 - AX IVE API"  for paramter limition.
    c)memory filled with input and output data must be alloced by user .
    d)image data of input and output must be specified by user.
    e)The number of input images (or data) from different CV may not be similar.
    f)The data type of 2-D images must be defined clearly ,or as default value.
    h)these Key parameter is formated as Json string or Json file. Please refer to .json file and code in some directories of /opt/data/ive/.


5）Key parameter in Json file:

    (1) in dma.json:
        mode, x0, y0, h_seg, v_seg, elem_size and set_val are the value of respective member in struct AX_IVE_DMA_CTRL_T
        such as enMode, u16CrpX0, u16CrpY0, u8HorSegSize, u8VerSegRows, u8ElemSize, u64Val.
        w_out and h_out is respectivly the width adn heigh of output image, only for AX_IVE_DMA_MODE_DIRECT_COPY mode in DMA.

    (2) in dualpics.json:
        x and y are the value of u1q7X and u1q7Y  in struct AX_IVE_ADD_CTRL_T for ADD CV.
        mode is the value of enMode in struct AX_IVE_SUB_CTRL_T for Sub CV.
        mse_coef is the value of u1q15MseCoef in struct AX_IVE_MSE_CTRL_T for MSE CV.

    (3) in ccl.json:
        mode is the value of enMode of struct AX_IVE_CCL_CTRL_T for CCL CV.

    (4) in ed.json:
        mask is all values of au8Mask[25] in struct AX_IVE_ERODE_CTRL_T for Erode CV or AX_IVE_DILATE_CTRL_T for Dilate CV.

    (5) in filter.json:
        mask is all values of as6q10Mask[25] in struct AX_IVE_FILTER_CTRL_T for Filter CV.

    (6) in hist.jsom:
        histeq_coef is the value of u0q20HistEqualCoef in struct AX_IVE_EQUALIZE_HIST_CTRL_T for EqualizeHist CV.

    (7) in integ.json:
        out_ctl is the value of enOutCtrl in struct AX_IVE_INTEG_CTRL_T for Integ CV.

    (8) in sobel.json:
        mask is the value of as6q10Mask[25] in struct AX_IVE_SOBEL_CTRL_T for Sobel CV.

    (9) in gmm.json:
        init_var, min_var, init_w, lr, bg_r, var_thr and thr are respectivly the value of  u14q4InitVar, u14q4MinVar, u1q10InitWeight, u1q7LearnRate, u1q7BgRatio, u4q4VarThr and u8Thr in struct AX_IVE_GMM_CTRL_T for GMM CV.
        in gmm2.json:
        init_var, min_var, max_var, lr, bg_r, var_thr, var_thr_chk, ct and thr are respectivly the value of u14q4InitVar, u14q4MinVar, u14q4MaxVar, u1q7LearnRate, u1q7BgRatio, u4q4VarThr, u4q4VarThrCheck, s1q7CT and  u8Thr in struct AX_IVE_GMM2_CTRL_T for GMM2 CV.

    (10) in thresh.json:
        mode, thr_l, thr_h, min_val, mid_val and max_val are repectivly the value of enMode, u8LowThr, u8HighThr, u8MinVal, u8MidVal and u8MaxVal in struct AX_IVE_THRESH_CTRL_T for Thresh CV

    (11) in 16bit_8bit.json:
        mode, gain and bias are repectivly the value of enMode, s1q14Gain and s16Bias in struct AX_IVE_16BIT_TO_8BIT_CTRL_T for 16BitTo8Bit CV.

    (12) in crop_resize.json:
        when CropImage is enabled, num is the value of u16Num in struct AX_IVE_CROP_IMAGE_CTRL_T and  boxs is the array type of crop image in which x,y,w and h are respecivly the value of u16X, u16Y, u16Width and u16Height in struct AX_IVE_RECT_U16_T.
        when CropResize or CropResizeForSplitYUV mode is enabled , num is the value of u16Num in struct AX_IVE_CROP_RESIZE_CTRL_T and align0, align1, enAlign[1] , bcolor, w_out and h_out are respectivly the value of enAlign[0], enAlign[1], u32BorderColor, width and height of output image.

    (13) in crop_resize2.json:
        num is the value of u16Num in struct AX_IVE_CROP_IMAGE_CTRL_T; res_out is the arrayes of width and height of output image;
        src_boxs is the array of cropped range from source image; dst_boxs is the array of range to resized image.

    (14) in matmul.json:
        mau_i, ddr_rdw, en_mul_res, en_topn_res, order and topn are respectivly the value of enMauId, s32DdrReadBandwidthLimit, bEnableMulRes, bEnableTopNRes, enOrder and s32TopN in struct AX_IVE_MAU_MATMUL_CTRL_T; type_in is the value of stMatQ and stMatB in struct AX_IVE_MAU_MATMUL_INPUT_T; type_mul_res and type_topn_res are the value of stMulRes and sfTopNRes in struct AX_IVE_MAU_MATMUL_OUTPUT_T; q_shape and b_shape are the value of pShape in stMatQ and stMatB of struct AX_IVE_MAU_MATMUL_INPUT_T.