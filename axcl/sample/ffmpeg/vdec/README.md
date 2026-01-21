1）Function description:
This module is provided as sample ffmpeg api code for the video decoding unit within the SDK package.
It is designed to help customers quickly understand and master the usage of video ffmpeg decoding-related interfaces.

After compilation, the executable axcl_sample_ffmpeg_vdec is located in the /opt/bin/axcl directory and can be used to verify video decoding functionality.

-i : input stream file.
-c：decoder name.     h264_axdec or hevc_axdec
-y：Whether to save YUV frame to file; 0：no save; 1：save.
-p：pixel format.               nv12, nv21, default: nv12.
-r: decoder resize, just scaler down function.       widthxheight
-v: set logging level

2）Usage Examples:
Example 1: View help information
/opt/bin/axcl/axcl_sample_ffmpeg_vdec  -h

Example 2: decoding 1080p h264，and save the decoded yuv to the current directory
/opt/bin/axcl/axcl_sample_ffmpeg_vdec -c h264_axdec -i /opt/data/vdec/ 1080p.h264 -y 1

Example 3：decoding1080p h265，and save the decoded yuv to the current directory
mount -t nfs -o nolock 10.126.12.109:/sw_nas/nfs_share /mnt
/opt/bin/axcl/axcl_sample_ffmpeg_vdec -c hevc_axdec -i /mnt/vdec/stream/cmodel_1080P_360frms_BJOpera.hevc -y 1

3）Execution Results:
After successful operation, the decoded yuv data should be generated in the current directory, name file.yuv, users can open it to see the actual effect.


