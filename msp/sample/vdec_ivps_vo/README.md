1）Function description:
This module is provided as sample code for the video decoding unit link other module unit within the SDK package.
It is designed to help customers quickly understand and master the usage of video decoding related interfaces.
The code demonstrates the following processes: initialization of the video decoding module, sending stream data via an decoding Send thread,
initialization of IVPS/VO module, and get YUV frame thread in VO offline mode, and deinitialization of each module.

After compilation, the executable sample_vdec_ivps_vo is located in the /opt/bin directory and can be used to verify VDEC link IVPS，IVPS link vo functionality.

2) Usage Examples:
View help information:
/opt/bin/sample_vdec_ivps_vo  -h

H264 protocol：
./sample_vdec_ivps_vo -i /opt/data/vdec/1080p.h264 -T 96 -W 1920 -H 1088

jpeg:
./sample_vdec_ivps_vo -i /opt/data/vdec/1080P_qp9.jpg -T 26 -W 1920 -H 1088

3) Parameter description：
-i:  Path to the input source data.
-T：Decoding type；26：jpegdec； 96：h264 vdec； 265: h265 vdec.
-L：Loop decoding number，Default  is 0, expressed as infinite times.
--res: Set the output resolution, the default typical resolution is 1920x1080, Used to configure vdec, ivps, and vo width and height attributes. Configuration is required.
-v:  (Not supported yet) Select display device type, resolution and refresh rate，for example: dsi0@1920x1080@60
-w : (Not supported yet) vo output YUV frame number，default value is 1，this value is only valid when the display is not connected, that is, -v is not selected，it is used to obtain YUV images using the vo offline function when the device is not connected to peripherals.
-W: Width parameter, used for internal reference frame buffer application, Default  is 8192. When paying attention to memory, please configure it according to the actual value.
-H: Height parameter, used for internal reference frame buffer application, Default  is 8192. When paying attention to memory, please configure it according to the actual value.
Notes：Since in sample, vo output YUV in offline mode and getframe operation executed in a thread，if the w value is greater than the actual number of frames, the thread cannot exit and requires Ctrl + C to exit.

4）Notes:
（1）If the user needs to configure other parameters, such as rawType, just modify the sample code.
（2）This sample only supports display of multiple video streams, in the case of MIPI dual screens, dsi0 displays video and dsi1 displays color bars.
（3）This sample used VDEC VdChn 0 link ivps 0, ivps 0 link vo 0 by default。


5）Sample Log switch：
echo 18 7 > /proc/ax_proc/logctl         # enable DEBUG LOG output；  For specific LEVEL please refer to enumeration type AX_LOG_LEVEL_E in ax_global_type.h

6）AX SDK Vdec Log switch：
echo 8 7 > /proc/ax_proc/logctl         # enable DEBUG LOG output；  For specific LEVEL please refer to enumeration type AX_LOG_LEVEL_E  in ax_global_type.h


echo target [file, console, null] > /proc/ax_proc/logctl      # file：LOG save on board path: /opt/data/AXSyslog/syslog/； console：LOG output from current terminal.

