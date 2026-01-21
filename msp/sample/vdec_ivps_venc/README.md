1）Function description:
This module is provided as sample code for the video decoding、IVPS、encoding unit within the SDK package.
It is designed to help customers quickly understand and master the usage of video decoding、IVPS、encoding related interfaces.
The code demonstrates the following processes: initialization of the video decoding module, sending stream data via an decoding Send thread,
initialization of IVPS/VENC module, and deinitialization of each module.

After compilation, the executable sample_vdec_ivps_venc is located in the /opt/bin directory and can be used to verify VDEC link IVPS，IVPS link venc functionality.
The decoding of this module is only supported in frame mode, and other modes are not supported.

2) Parameter description：
-i:  Path to the input source data.
-T：Decoding type；26：jpegdec； 96：h264 vdec； 265: h265 vdec
-L：Loop decoding number，Default  is 0, expressed as infinite times
--res: Set the output resolution, the default typical resolution is 1920x1080. Used to configure vdec、ivps、venc width and height attributes. Configuration is required.
-W: width parameter, used for internal reference frame buffer application, Default  is 8192. When paying attention to memory, please configure it according to the actual value.
-H: Height parameter, used for internal reference frame buffer application, Default  is 8192. When paying attention to memory, please configure it according to the actual value.
-c：Configure the number of decoding groups
--enDisplayMode：0=preview mode，1=back pressure mode(VENC does not support back pressure mode, so this sample just supports preview mode)

3) Usage Examples:
View help information:
/opt/bin/sample_vdec_ivps_venc

H264 protocol：
./sample_vdec_ivps_venc -i /opt/data/vdec/1080p.h264 --res=1920x1080 -W 1920 -H 1088 -L 100 -c 1 --enDisplayMode 0 -T 96

jpeg:
./sample_vdec_ivps_venc -i /opt/data/vdec/1080P_qp9.jpg --res=1920x1080 -W 1920 -H 1088 -L 100 -c 1 --enDisplayMode 0 -T 26

4）Notes:
(1）If the user needs to configure other parameters, such as rawType, just modify the sample code.
(2）This sample used vdec chn 0 link ivps 0, ivps 0 link venc 0 by default。

5）Sample Log switch：
echo 18 7 > /proc/ax_proc/logctl         # enable DEBUG LOG output； For specific LEVEL please refer to enumeration type AX_LOG_LEVEL_E in ax_global_type.h

6）AX SDK Vdec Log switch：
echo 8 7 > /proc/ax_proc/logctl         # enable DEBUG LOG output； For specific LEVEL please refer to enumeration type AX_LOG_LEVEL_E  in ax_global_type.h

echo target [file, console, null] > /proc/ax_proc/logctl      # file：LOG save on board path: /opt/data/AXSyslog/syslog/； console：LOG output from current terminal.

