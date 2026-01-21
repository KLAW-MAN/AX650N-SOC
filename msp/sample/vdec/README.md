1）Function description:
This module is provided as sample code for the video decoding unit within the SDK package.
It is designed to help customers quickly understand and master the usage of video decoding-related interfaces.
The code demonstrates the following processes: initialization of the video decoding module, sending stream data via an decoding Send thread,
obtaining and saving decoded frame data via an decoding Get thread, and deinitialization of the video decoding module.

After compilation, the executable sample_vdec is located in the /opt/bin directory and can be used to verify video decoding functionality.

-i : Path to the input source data.
-c：Configure the number of decoding groups；Default  is 1.
-T：Decoding type；26：jpegdec； 96：h264 vdec； 265: h265 vdec.
-w：Whether to save YUV frame to file; 0：no save; 1：save.
-L：Loop decoding number；Default  is 1.
-W: Width parameter, used for internal reference frame buffer application, Default  is 8192. When paying attention to memory, please configure it according to the actual value.
-H: Height parameter, used for internal reference frame buffer application, Default  is 8192. When paying attention to memory, please configure it according to the actual value.
-M: Send stream mode；0：send stream in NAL mode，one NAL unit everytime；
                1：send stream in FRAME mode，one FRAME unit everytime；
                2: send stream in SLICE mode，one SLICE unit everytime（Not supported yet）；
                3：send stream in STREAM mode，send variable length stream each time（JPEG/MJPEG not support）；
                4：Compatible sending mode，one frame stream can be sent multiple times，when one frame sended end, setting bEndOfFrame AX_TRUE；
-s：When STREAM mode, the parameter is valid, the length of each time stream sent, the value is valid if greater than zero and less than input buffer size,
       input buffer size is 3MBytes in sample, default value is 1MBytes.
--VdChn： Channel number， AX650 VDEC support number:0~2， JDEC just support number 0.
--res： Set the output resolution, the default typical resolution is 1920x1080.
--select 1：call AX_VDEC_SelectGrp（Default）；0：disable

--outFormat：output YUV frame format；0: YUV400, 3: NV12 （for 8-bit/10-bit stream）, 4: NV21（for 8-bit/10-bit stream）,
                       42:10bitP010, 40:10bitY/U/V 4 pixels in 5 bytes. default: 3

2）Usage Examples:
Example 1: View help information
/opt/bin/sample_vdec  -h

Example 2: decoding 1080p jpeg，and save the decoded yuv to the current directory
/opt/bin/sample_vdec -i /opt/data/vdec/1080P_qp9.jpg -T 26 -w 1 --res=1920x1080 -W 1920 -H 1088

Example 3：decoding1080p h264，and save the decoded yuv to the current directory
/opt/bin/sample_vdec -i /opt/data/vdec/1080p.h264 -T 96 -w 1 --res=1920x1080 -W 1920 -H 1088

3）Execution Results:
After successful operation, the decoded yuv data should be generated in the current directory, name group0.yuv, users can open it to see the actual effect.

4）Sample Log switch：
echo 18 7 > /proc/ax_proc/logctl         # enable DEBUG LOG output； For specific LEVEL please refer to enumeration type AX_LOG_LEVEL_E  in ax_global_type.h

5）AX SDK Vdec Log switch：
echo 8 7 > /proc/ax_proc/logctl         # enable DEBUG LOG output； For specific LEVEL please refer to enumeration type AX_LOG_LEVEL_E in ax_global_type.h


echo target [file, console, null] > /proc/ax_proc/logctl      # file：LOG save on board path: /opt/data/AXSyslog/syslog/ ； console：LOG output from current terminal

