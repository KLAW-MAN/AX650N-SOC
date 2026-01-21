#FRTDemo (frontend demo)

## How to run?

### Run ITS Pipeline

1. cd /opt/bin/FRTDemo
2. (Option) Configure pipeline: vi config/its/ppl.json, to configure pipeline
3. (Option) Configure IVPS parameters: vi config/its/ivps.json
4. (Option) Configure Venc parameters: vi config/its/encoder.json
5. (Option) Configure some function parameters:  vi config/its/options.ini  [Parameter Description](#ParamConfigure)
6. ./run.sh -p 0 -s 1 -n 2 [Parameter Description](#StartupParameters)

> Add -d for GDB debugging. It internally uses the SIGUSR2 signal as a timer, so when debugging with GDB, input "***handle SIGUSR2 nostop noprint***" to ignore the SIGUSR2 signal.

### Run IPC Pipeline

1. cd /opt/bin/FRTDemo
2. ./run.sh -p 1 -s 1 [Parameter Description](#StartupParameters)

## How to connect the Sensors for Pano Pipeline

1. Select the ADP_RX_DPHY board model "ADP_RX_DPHY_4X4LANE_V1_0, 2022/06/17". For the Pano Dual OS04A10 sensor, connect the two OS04A10 sensors to RX4/5 and RX6/7, respectively.
2. The relevant configuration parameter files for AVS are stored in the /param/avs/os04a10 directory.

## How to compile?

1. cd app/demo/src/ppl/frt
2. make p=xxx clean
3. make p=xxx
4. make p=xxx install

> p=xxx set project name, e.g. make p=AX650_emmc

## How to preview?

1. RTSP Stream Preview: You can preview the RTSP stream using third-party tools, such as VLC, by entering the RTSP stream address (refer to the terminal output: "Play the stream using url: <<<<< rtsp://IP:8554/axstream0 >>>>>").
2. Web Preview: You can preview the video by opening the webpage in Chrome (refer to the terminal output: "Preview the video using URL: <<<<< http://IP:8080 >>>>>").

# <a href="#StartupParameters">Startup Parameters</a>

p: pipeline index, indicates ppl load type, like ITS or IPC or Pano etc.
0: ITS pipeline (DEFAULT)
1: IPC pipeline
2: Pano pipeline
s: sensor type.
0: DUAL OS08A20 (DEFAULT)
1: Single OS08A20
2: DUAL OS08B10
3: Single OS08B10
4: Single SC910gs
5: Pano Dual 0SO4A10
6: Single OS08A20 + Dual 0SO4A10(Pano)
7: Single OS04A10
8: Quad OS04A10
9: Dummy Quad OS04A10
10: Dummy Six OS04A10
11: Dummy Eight OS04A10
13: Single SC410gs
14: Quad OS08A20
15: Quad SC500AI
16: Dummy Quad SC500AI
17: Quad IMX678
18: Dummy Quad IMX678
n: scenario, indicates the scenario to load, which is always defined in config files.
ITS:"
0: Dual sensor in T3DNR+T2DNR+T2DNR mode(default)"
1: Dual sensor in T3DNR+T2DNR+AI2DNR mode"
2: Single sensor in (AI3D_T2DNR<->T3DNR)+T2DNR+AI2DNR mode"
3: Single sensor with flash mode with two pipes"
4: Dual sensor in 4k@30fps AICE<->AINR SDR/HDR mode"
5: Dual sensor in 4k@50fps AI3D_T2DNR<->T3DNR SDR mode"
6: Single sensor in 4k@30fps AI3D_T2DNR<->T3DNR SDR mode"
7: Single sensor in 4M@30fps DCG-HDR mode"
8: Single sensor in 4M@30fps DCG-VS HDR mode"
9: Single sensor with flash mode with three pipes"
11: Single sensor for intelligent monitoring and recording system"
IPC:"
0: Single sensor(default)"
1: Single sensor + Dual sensor(8+4+4)"
2: Single sensor in board 8+4+4"
3: Quad 4K/4M sensor"
4: Eight sensor"
5: Six sensor"
6: Quad 5M sensor"
Pano:"
0: Dual sensor(default)"
1: Dual sensor in board 8+4+4"
l: log level, indicates the log level.
CRITICAL = 1, ERROR = 2 (DEFAULT), WARN = 3, NOTICE = 4, INFO = 5, DEBUG = 6, DATA = 7
t: log target, indicates the log output targets.
SYSLOG = 1, APPLOG = 2, STDOUT = 4 (DEFAULT) (Calculate the sum if multiple targets is required)
d: start with gdb for debugging, value **NOT REQUIRED**
u: testsuite type.
0: Dual default
1: Single default

# <a href="#ParamConfigure">Parameters Configure</a>


| # | Parameter           | Range         | Description                                 |
| - | ------------------- | ------------- | ------------------------------------------- |
| 1 | RTSPMaxFrmSize      | [0 - 8000000] | Unit: Bypes                                 |
| 2 | WebVencFrmSizeRatio |               | The size ratio of Web cached frames to YUV. |
| 3 | WebJencFrmSizeRatio |               | The size ratio of Web cached frames to YUV. |
| 4 | PrintFPS            |               | 0: not print 1: print                       |
| 5 | EnableOSD           |               | 0：disable OSD 1: enalbe OSD                |
| 6 | EnableMp4Record     |               | 0：disable mp4 record 1: enable mp4 record  |
| 7 | MP4RecordSavedPath  |               | mp4 record path                             |
