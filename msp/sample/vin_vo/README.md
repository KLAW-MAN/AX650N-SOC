1) Function description:
The code below the vin_vo folder is the sample reference code provided by Aiwel SDK package, which is convenient for customers to quickly understand the flow
of VIN/VO two modules. The sample code demonstrates the following functions: VIN/VO module initialization, single camera SDR/HDR initialization, LINK between
VIN and VO. We provide sensor initialization configuration for os08a20.


2) Example of use:
Single camera single camera os08a20, SDR mode, channel 0 display, run the command as follows:
. /sample_vin_vo -c 0 -s 0 -v dsi0@1920x1080@60

-c: select different scenes, 0-single shot os08a20
-v: Select the type, resolution and refresh rate of the display device, e.g. dsi0@1920x1080@60
-s: select which channel of sensor output is sent to VO for display, default channel 0.
Note: The use of the -v parameter above can refer to sample_vo, and other parameters can refer to sample_vin.

3) Notes:
(1) How to apply common pool memory block? Our company provides a memory configuration table (see the definition in sample_vin_vo.c file). Users can
    configure the buf of each module of the whole system according to their own business, this design is more conducive to understanding and use.
(2) If you need to configure other parameters, such as rawType, modify the sample code.
(3) This sample only supports one way video streaming display, in the case of MIPI dual-screen, dsi0 display video, dsi1 display color bar.