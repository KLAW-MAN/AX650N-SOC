1. Function Description:

This module is provided as sample code for the video encoding unit (H.264, H.265, JPEG, MJPEG) within the SDK package.
It is designed to help customers quickly understand and master the usage of video encoding-related interfaces.
The code demonstrates the following processes: initialization of the video encoding module, sending frame data via an encoding Send thread,
obtaining and saving encoded stream data via an encoding Get thread, and deinitialization of the video encoding module.

After compilation, the executable sample_venc is located in the /opt/bin directory and can be used to verify video encoding functionality.
•-w: Configure the source data width.
•-h: Configure the source data height.
•-i: Path to the input source data.
•-l: Input source data YUV format (1: I420; 3: NV12; 4: NV21; 13: YUYV422; 14: UYVY422). Default is 1.
•-N: Configure the number of encoding channels. By default, it enables four channels for encoding H.264, H.265, JPEG, and MJPEG.
•-n: When loop encoding is enabled, specify the number of frames to encode.
•-W: Whether to write the encoded stream to a file (default is 1, which means writing to a file. 0: do not write).

2. Usage Examples:

Example 1: View help information
     sample_venc -H

Example 2: Enable two channels to encode 1080p NV12 format (Channel 0: H.264, Channel 1: H.265)
     sample_venc -w 1920 -h 1080 -i 1080p_nv12.yuv -N 2 -l 3

Example 3: Enable two channels to loop encode 3840x2160 NV21 format (Channel 0: H.264, Channel 1: H.265), encoding 10 frames
     sample_venc -w 3840 -h 2160 -i 3840x2160_nv21.yuv -N 2 -l 4 -n 10

3. Execution Results:

Upon successful execution, press Ctrl+C to exit. Stream files should be generated in the current directory with extensions like .264, .265, .jpg, or .mjpg.
Users can open these files to view the actual results.

4. Notes:

•a) Some parameters in the example code may not be optimal and are only intended for API demonstration.
    In actual development, users need to configure parameters according to specific business scenarios.
•b) H.264/H.265 support a maximum resolution of 8192x8192.
•c) JPEG/MJPEG support a maximum resolution of 16384x16384.