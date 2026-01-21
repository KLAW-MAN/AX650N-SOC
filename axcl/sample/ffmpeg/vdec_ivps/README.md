1）Function description:
This module is provided as sample ffmpeg api code for the video decoding and filter unit within the SDK package.
It is designed to help customers quickly understand and master the usage of video ffmpeg decoding-related interfaces.

After compilation, the executable axcl_sample_ffmpeg_vdec_ivps is located in the /opt/bin/axcl directory and can be used to verify video decoding functionality.

-i : input stream file.
-c：decoder name.     h264_axdec or hevc_axdec
-y：Whether to save YUV frame to file; 0：no save; 1：save.
-r: decoder resize, just scaler down function.       widthxheight

-f: filter for resize yuv and format switch.       ax_scale=width:height
-v: set logging level

2）Usage Examples:
Example 1: View help information
/opt/bin/axcl/axcl_sample_ffmpeg_vdec_ivps  -h

Example 2: decoding 1080p h264，and save the filter yuv to the current directory
/opt/bin/axcl/axcl_sample_ffmpeg_vdec_ivps -c h264_axdec -i 1080p.h264 -f "ax_scale=1280:720,hwdownload,format=nv12" -y 1

Example 3：decoding1080p h265，and save the filter yuv to the current directory
/opt/bin/axcl/axcl_sample_ffmpeg_vdec_ivps -c hevc_axdec -i 1080p.hevc -f "ax_scale=1280:720,hwdownload,format=nv12" -y 1

3）Execution Results:
After successful operation, the filter yuv data should be generated in the current directory, name out.yuv, users can open it to see the actual effect.

