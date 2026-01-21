## Function Description:
- The code in the `isp_3a` folder is example reference code provided by the axera SDK package, which helps customers quickly understand the configuration process of the entire 3A module.
The example code demonstrates the following pipeline structure: single camera, dual-camera with dual 4K.
Currently, only the initialization configuration for the demo board-adapted sensor OV-OS08A20 is provided.  If needed, you can follow the steps in the <font color=#0000CD>“Add New Sensor Steps”</font> section below.
- AE, LSC, and AWB need to be implemented by writing init, run, and deinit functions based on the function pointers defined in AX_ISP_AE_REGFUNCS_T, AX_ISP_LSC_REGFUNCS_T, and AX_ISP_AWB_REGFUNCS_T.  Then, refer to `sample_isp_3a.c` to register AE, LSC, and AWB into the ISP pipeline.  AF only needs to run in a separate thread, and the AF statistics can be obtained through `AX_ISP_IQ_GetAF1Statistics`.  When there are AF statistics, it will return immediately;  otherwise, it will block for 200ms.  <font color=#0000CD>For specific usage, please refer to "23 - AX 3A Development Guide.docx"</font>.

## Usage Example:
For a single camera with a single path OS08A20 8M sensor running in SDR mode, run the following command:
```
./sample_isp_3a -c 1 -m 1 -e 1
```
- `-c`: Choose the scene:
1 - Single camera OS08A20
2 - Dual camera OS08A20
- `-m`: Choose the mode:
0 - LoadRaw mode
1 - Sensor mode
2 - TPG mode
- `-u`: Choose whether to use customized 3A:
0 - Do not use customized 3A
1 - Use customized 3A (defaults to on when `-u` is not configured)
- `-e`: Choose SDR/HDR mode:
1 - SDR (defaults to SDR)
2 - HDR 2DOL

## Steps to Add a New Sensor:
<font color=#0000CD>For the detailed code modification steps to add support for a new sensor, refer to the document "20 - AX Sensor Debugging Guide.docx"</font>

- Configure the common pool corresponding to the sensor
- Configure the parameters for the sensor, VIN, and Chn
- Register the sensor library
- Add the corresponding sensor case in the common sub-functions