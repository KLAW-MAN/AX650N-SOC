1) Function description:

This is a UVC (USB Video Class) sample code, which is convenient for users to quickly understand and master the VIN module, VENC module and other related interfaces to realize the UVC camera function.
The sample_uvc executable program and the uvc driver setting script uvc-gadget-composite.sh are located in the /opt/bin/sample_uvc/ directory.
The jpg format image file of the dummy sensor is located in the /opt/data/uvc/ directory, supporting the AX650 platform.

Supports single and dual os08a20 sensors:
Output resolution is 360p, 540p, 720p, 1080p, 4K, and the frame rate is 30 in MJPG/H264/H265 format video stream;
Output resolution is 360p@30fps, 480p@30fps, 540p@20fps, 720p@10fps, 1080p@5fps YUY2 format video stream.

Supports single and two dummy sensors:
Outputs a video stream in MJPG format with a resolution of 720p and a frame rate of 30. It reads fixed JPG format image data to act as sensor data, not real sensor data;
Outputs a video stream in YUY2 format with a resolution of 360p and a frame rate of 30, a resolution of 480p and a frame rate of 30, a resolution of 540p and a frame rate of 20, a resolution of 720p and a frame rate of 1080p, and acts as sensor data by drawing color bar image data. It is not real sensor data, so it is called a dummy sensor.

Supports starting ISP tuning through the application, using the tuning tool on the PC to perform IQ tuning,
debugging related IQ parameters, and viewing the real-time effect image through the UVC camera. The image is the image output of the VIN module CHN1 or CHN2.

The PC can use the PotPlayer player to debug the UVC Camera. The player supports video recording, screenshots, rotation, etc.

2) Usage example:

Prepare the hardware environment of the AX650 platform. After powering on, you can connect to the AX650 platform through the serial port. Use a typeC cable to connect the USB2/USB3 interface on the board to the PC.

Notes:
For the AX650A Demo board, when using one sensor, the sensor is connected to the MIPI RX0-3 interface; when using two sensors, the first sensor is connected to the MIPI RX0-3 interface, and the second sensor is connected to the MIPI RX4-7 interface.

Run the script to set the UVC driver. The fourth parameter is to select the sensor type, 0: dummy sensor, 1: os08a20 sensor. If you enter other values, an error will be reported; parameter three is the number of UVC channels actually created. Note that if several cameras are actually used, several UVC channels need to be opened.
Parameter five is to select the USB transmission mode, 0: iso mode, 1: bulk mode.
./opt/bin/sample_uvc/uvc-gadget-composite.sh start usb2 1 1 1

Parameter 1: start or stop
Parameter 2: usb2 or usb3 is supported
Parameter 3: uvc device count, support 2 devices at maximum
Parameter 4: 0: os08a20 sensor, 1: dummy sensor
Parameter 5: 0: iso mode, 1: bulk mode

Start UVC program on AX650 platform board
Example 1:
./opt/bin/sample_uvc/sample_uvc -d -n 4 -y 0 -b
The -y option selects the sensor type, 0: single os08a20 sensor, 1: two os08a20 sensors, 2: single dummy sensor, 3: two dummy sensors.
This command selects 0: single os08a20 sensor. By default, the LINK mode is used between the VIN module and the VENC module. The -b option indicates the selection of the usb bulk transmission mode.

Example 2:
./opt/bin/sample_uvc/sample_uvc -d -n 4 -y 0 -w 0
The -y option selects the sensor type, 0: single os08a20 sensor, 1: two os08a20 sensors, 2: single dummy sensor, 3: two dummy sensors.
This command selects 0: single os08a20 sensor. The -w 0 option indicates that the non-LINK mode is used between the VIN module and the VENC module. The USB iso transmission mode is selected by default without the -b option.

Example 3:
./opt/bin/sample_uvc/sample_uvc -d -n 4 -y 1 -p -w 1
The -y option selects the sensor type, 0: single os08a20 sensor, 1: two os08a20 sensors, 2: single dummy sensor, 3: two dummy sensors.
This command selects 1: two os08a20 sensors. The -p option turns on isp tuning. The -w 1 option indicates that the VIN module and the VENC module use the LINK mode.
If the -b option is not added, the USB iso transmission mode is selected by default.

Example 4:
./opt/bin/sample_uvc/sample_uvc -d -n 4 -y 2 -b
The -y option selects the sensor type, 0: single os08a20 sensor, 1: two os08a20 sensors, 2: single dummy sensor, 3: two dummy sensors.
This command selects 2: single dummy sensor. If the -i option is not added, the default image is /opt/data/uvc/1280x720.jpg. The -b option indicates that the USB bulk transmission mode is selected.

Example 5:
./opt/bin/sample_uvc/sample_uvc -d -n 4 -y 2 -i /opt/data/uvc/1280x720.jpg
The -y option selects the sensor type, 0: single os08a20 sensor, 1: two os08a20 sensors, 2: single dummy sensor, 3: two dummy sensors.
This command selects 2: single dummy sensor. The -i option specifies the image path, and the image resolution is 1280x720. Under the condition of -y 2, without the -i option, the default image is /opt/data/uvc/1280x720.jpg.

Example 6:
./opt/bin/sample_uvc/sample_uvc -d -n 4 -y 3 -i /opt/data/uvc/1280x720.jpg
The -y option selects the sensor type, 0: single os08a20 sensor, 1: two os08a20 sensors, 2: single dummy sensor, 3: two dummy sensors.
This command selects 3: two dummy sensors. The -i option specifies the image path, and the image resolution is 1280x720. Under the condition of -y 3, without the -i option, the default image is /opt/data/uvc/1280x720.jpg.

Parameter 1 -d: For uvc device, the dummy camera type is used, that is, the uvc device stand alone type has no v4l2 camera.
Parameter 2 -n: number of v4l2 buffers
Parameter 3 -y: sensor type
Parameter 4 -p: start isp tuning flag
Parameter 5 -w: VIN and VENC modules work in LINK mode
Parameter 6 -a: enable aiisp: 1, on; 0, off, AIISP is enabled by default
Parameter 7 -b: enable usb bulk transmission mode

UVC camera player software operation instructions
Download a PotPlayer software installation package from the Internet and install it on the PC. Open the PotPlayer video player on the PC, Alt+D shortcut key to open
Camera tab, select UVC Camera or USB Video Device in the device column (you can try to retrieve the update button to update the device status).

For os08a20 sensor, select in the Format column:
MJPG/H264/H265 (640x360, 960x540, 1280x720, 1920x1080, 3840x2160) format;
YUY2 (640x360, 640x480, 960x540, 1280x720, 1920x1080) format;
Then click the Open Device button to open the UVC Camera or USB Video Device.

For dummy sensor, select in the Format column:
MJPG (1280x720) format,
YUY2 (640x360, 640x480, 960x540, 1280x720, 1920x1080) format.
Then click the Open Device button to open the UVC Camera or USB Video Device.

In the upper left corner of the Potplayer main interface, click the drop-down menu "PotPlayer ∨", select the "Video" function, and select "Image Processing", "Video Recording", "Image Capture" and other functions in the submenu as needed to realize image processing, video recording, and screenshot saving functions.