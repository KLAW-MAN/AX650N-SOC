### Description
The code under the skel folder is sample reference code of SKEL module.

### Usage
```bash
usage: ./axcl_sample_skel <options> ...
options:
-i,     Input File(yuv)
-r,     Input File Resolution(wxh)(yuv: should be input)
-m,     Models deployment path(path name)
-t,     Repeat times((unsigned int), default=1)
-I,     Interval repeat time((unsigned int)ms, default=0)
-c,     Confidence((float: 0-1), default=0)
-H,     Human track size limit((unsigned int), default=0)
-V,     Vehicle track size limit((unsigned int), default=0)
-C,     Cylcle track size limit((unsigned int), default=0)
-d,     Device index from 0 to connected device num - 1
-e,     Skel detect type((unsigned int), default=1)
                0: detect only
                1: detect + track
-N,     Skel NPU type((unsigned int), default=0(VNPU Disable)
                0: VNPU Disable
                1: STD-VNPU Default
                2: STD-VNPU1
                3: STD-VNPU2
                4: STD-VNPU3
                5: BIG-LITTLE VNPU Default
                6: BIG-LITTLE VNPU1
                7: BIG-LITTLE VNPU2
-p,     Skel PPL((unsigned int), default=1)
                1: AXCL_SKEL_PPL_HVCP
                2: AXCL_SKEL_PPL_FACE
-v,     Log level((unsigned int), default=5)
                0: LOG_EMERGENCY_LEVEL
                1: LOG_ALERT_LEVEL
                2: LOG_CRITICAL_LEVEL
                3: LOG_ERROR_LEVEL
                4: LOG_WARN_LEVEL
                5: LOG_NOTICE_LEVEL
                6: LOG_INFO_LEVEL
                7: LOG_DEBUG_LEVEL
-j,     axcl.json path
-h,     print this message
```

### Example
```bash
$ ./axcl_sample_skel -i 1024x576_traffic.yuv -r1024x576 -p1
[N][                            main][1048]: Task infomation:
[N][                            main][1049]:    Input file: 1024x576_traffic.yuv
[N][                            main][1050]:    Input file resolution: 1024x576
[N][                            main][1051]:    Repeat times: 1
[N][                            main][1052]: SKEL Init Elapse:
[N][                            main][1053]:    AXCL_SKEL_Init: 1 ms
[N][                            main][1054]:    AXCL_SKEL_Create: 1064 ms
[N][                            main][1074]: SKEL Process times: 1
[N][                            main][1087]: SKEL Process Elapse:
[N][                            main][1088]:    AXCL_SKEL_SendFrame: 11 us
[N][                            main][1102]:    AXCL_SKEL_GetResult: 7146 us
[N][                            main][1114]: SKEL Process Result:
[N][                            main][1119]:    Object Num: 19
[N][                            main][1129]:            Rect[0] plate: [554.796143, 485.107300, 38.924194, 13.589508], Confidence: 0.869044
[N][                            main][1142]:            [0]Point Set Size: 0
[N][                            main][1129]:            Rect[1] plate: [348.251434, 382.583344, 31.881897, 11.542175], Confidence: 0.832833
[N][                            main][1142]:            [1]Point Set Size: 0
[N][                            main][1129]:            Rect[2] plate: [539.461121, 266.972137, 24.163025, 10.055725], Confidence: 0.815062
[N][                            main][1142]:            [2]Point Set Size: 0
[N][                            main][1129]:            Rect[3] plate: [419.993195, 227.618103, 20.354401, 8.153931], Confidence: 0.527393
[N][                            main][1142]:            [3]Point Set Size: 0
[N][                            main][1129]:            Rect[4] body: [725.376526, 118.759689, 15.004700, 38.911102], Confidence: 0.685393
[N][                            main][1142]:            [4]Point Set Size: 0
[N][                            main][1129]:            Rect[5] vehicle: [310.423492, 305.094940, 130.774872, 118.264618], Confidence: 0.950425
[N][                            main][1142]:            [5]Point Set Size: 0
[N][                            main][1129]:            Rect[6] vehicle: [497.437317, 379.807190, 144.608582, 150.543457], Confidence: 0.950425
[N][                            main][1142]:            [6]Point Set Size: 0
[N][                            main][1129]:            Rect[7] vehicle: [508.856049, 207.152435, 81.899689, 78.981079], Confidence: 0.923359
[N][                            main][1142]:            [7]Point Set Size: 0
[N][                            main][1129]:            Rect[8] vehicle: [399.584351, 191.188858, 77.525116, 63.402054], Confidence: 0.891781
[N][                            main][1142]:            [8]Point Set Size: 0
[N][                            main][1129]:            Rect[9] vehicle: [400.602020, 95.998848, 38.924164, 30.192940], Confidence: 0.869044
[N][                            main][1142]:            [9]Point Set Size: 0
[N][                            main][1129]:            Rect[10] vehicle: [371.214691, 84.665039, 38.924164, 34.281708], Confidence: 0.850939
[N][                            main][1142]:            [10]Point Set Size: 0
[N][                            main][1129]:            Rect[11] vehicle: [430.419006, 82.658363, 27.090210, 20.746056], Confidence: 0.812156
[N][                            main][1142]:            [11]Point Set Size: 0
[N][                            main][1129]:            Rect[12] vehicle: [526.925476, 55.180939, 45.828491, 46.667572], Confidence: 0.805840
[N][                            main][1142]:            [12]Point Set Size: 0
[N][                            main][1129]:            Rect[13] vehicle: [248.044891, 98.645302, 40.362549, 30.192940], Confidence: 0.802549
[N][                            main][1142]:            [13]Point Set Size: 0
[N][                            main][1129]:            Rect[14] vehicle: [283.952637, 101.500259, 33.060028, 25.644196], Confidence: 0.721899
[N][                            main][1142]:            [14]Point Set Size: 0
[N][                            main][1129]:            Rect[15] vehicle: [450.306335, 69.872543, 21.145233, 22.389435], Confidence: 0.614565
[N][                            main][1142]:            [15]Point Set Size: 0
[N][                            main][1129]:            Rect[16] vehicle: [564.793213, 35.912395, 10.734192, 11.334644], Confidence: 0.576071
[N][                            main][1142]:            [16]Point Set Size: 0
[N][                            main][1129]:            Rect[17] vehicle: [780.513794, 133.513489, 62.139709, 46.891541], Confidence: 0.552055
[N][                            main][1142]:            [17]Point Set Size: 0
[N][                            main][1129]:            Rect[18] vehicle: [306.100739, 89.407288, 26.578796, 24.162994], Confidence: 0.537563
[N][                            main][1142]:            [18]Point Set Size: 0
[N][                            main][1168]: SKEL Process Elapsed Info: Repeats: 1, (min: 7157 us, avr: 7146 us, max: 7157 us)
```