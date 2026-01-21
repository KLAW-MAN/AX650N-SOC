sample_ives

1) Functional description:
The code under the ives folder is the sample reference code provided by the Aixin SDK package, which is convenient for customers to quickly understand the configuration process of the entire IVES module.
The sample code demonstrates the following functions: MD motion detection and OD occlusion detection
(1) The sample/ives/md folder is the sample reference code for MD motion detection
(2) The sample/ives/od folder is the sample reference code for OD occlusion detection
(3) The sample/ives/scd folder is the sample reference code for SCD scene switching detection

2) Usage example:
 (1) MD
      ./sample_ives -t 0
      sample will read the 3 images under /opt/data/ives to simulate the MD results, where:
         1920x1080_ref0.nv12.yuv is the reference background
         1920x1080_det1.nv12.yuv and 1920x1080_det2.nv12.yuv are the detection images
 (2) OD
      ./sample_ives -t 1

 (3) SCD
      ./sample_ives -t 2 --ref=save_1280x720_1.nv12 --cur=save_1280x720_2.nv12 --width=1280 --height=720


3) Sample running results:
 (1) MD
/opt/bin # sample_ives -t 0
IVES sample: V0.31.0_20220409003934 build: Apr  9 2022 01:11:33
MD (Chn 0, img: 1, elapsed time: 7 ms
IMAGE 1 MB THRS: 20 x 15 = 300
 1  0  1  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0
 0  1  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0
 0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0
 0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0
 0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0
 0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0
 0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0
 0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0
 0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0
 0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0
 0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0
 0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0
 0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0
 0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0
 0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  1
MD (Chn 0, img: 2, elapsed time: 7 ms
IMAGE 2 MB THRS: 20 x 15 = 300
 0  0  0  0  0  0  0  0  1  0  0  0  0  0  0  0  0  0  0  0
 0  0  0  1  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0
 0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0
 0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0
 0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0
 0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0
 0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0
 0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0
 0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0
 0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0
 0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0
 0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0
 0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0
 0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0
 0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  1  0  0

 (2) OD
/opt/bin # sample_ives -t 1
IVES sample: V0.31.0_20220409003934 build: Apr  9 2022 01:11:33
OD (Chn 0, img: 0, od: 0, elapsed time: 6 ms

 (3) SCD
/opt/bin # ./sample_ives -t 2 --ref=save_1280x720_bgr1.nv12 --cur=save_1280x720_bgr2.nv12 --width=1280 --height=720
IVES sample: V0.39.0 build: Jun 29 2022 20:54:33
SCD (Chn 1, img: 1, elapsed time: 172 ms
SCD (Chn 1, img: 2, elapsed time: 172 ms
Scene change detection result: unchanged

4) Notes:
(1) The area configuration of MD cannot exceed the image width and height, and the area must be divisible by macroblocks. For details, please refer to the user manual of IVES;
(2) The OD sample code only shows how to call the API, without configuring the environment LUX. For details on how to obtain AE LUX from ISP and configure LUX, please consult Aixin engineers or refer to the OD function in IPCDemo.
