
1. Function Description:
The code under the VIN folder is an example reference code provided by the Axera SDK package, which facilitates customers to quickly understand the configuration process of the entire VIN module
The example code demonstrates the following pipeline configuration: single camera, dual camera dual 4K, dual camera multi PIPE, single camera AI-ISP
At present, only the initialization configuration of sensors adapted to the demo board is provided: OV-OS08A20， If necessary, you can follow point 4 "Add new sensor steps" below

2. Usage example:
Single camera single channel OS08A20 8M sensor, run one SDR, run the command as follows:
./sample_vin -c 1 -m 1 -e 1 [-d N]

-c: Choose different scenarios:
        0-TPG(DummySensor)
        1-Single camera OS08A20
        2-Dual camera OS08A20
        3- Dual camera OS08A20, each containing three PIPEs
        Demo of capturing and processing 4-its burst frames
        5-Four camera OS08A20
        6-Single camera SC910GSits to FPGA multi pipe mode, with each pipe individually controlling exposure parameters
        7-YUV422 to YUV420 (DummySensor)
        8-Single OS04A10 DCG HDR
        9-Single OS04A10 DCG VS HDR

-m: Choose different modes:
        0-LoadRaw mode
        1-sensor mode
        2-tpg mode

-a: Select enable AIISP mode:
        0-Turn off AIISP (default when - a is not configured)
        1- Enable AIISP

-e: Select SDR/HDR mode:
        1-SDR (default running SDR)
        2-HDR 2DOL
-d: The number of BLKs that need to be applied for during continuous dump, N - Add N Frame caches


3. Attention:
(1) How to apply for common pool memory blocks? Our company has provided a memory configuration table (as defined in the sample-vin. c file),
Users can configure the buf of each module of the entire system according to their own business here, which is more conducive to understanding and use
(2) The - d option generally does not need to be added. If continuous dump is required, this option needs to be added, followed by the number of frame buffers, such as/ sample_vin -c 1 -d 100
(3) Running DCG VS HDR will perform two pipeline switching actions, and each time the DCG path is switched, SDR/HDR switching will be performed. The VS path will not be interrupted during the switching process.

4. Steps to add new sensor support, detailed code modifications can be found in the document 《20-AX Sensor Debugging Guide.docx》
(1) Configure the common pool corresponding to the sensor
(2) Configure parameters for Sensor, VIN, and Chn
(3) Register Sensor Library
(4) Add the corresponding Sensor Case to the common subfunction
