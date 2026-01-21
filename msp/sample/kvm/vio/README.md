Hardware preparation:
1. You need to connect the UXE INTIO_GPIO5 of U4 to the GPIO2_A2 pin of AX650
2. Connect UXE as a slave device to AX650
If you are not sure, please consult the relevant hardware engineer

Program parameter description
-f: yuv422 8bit:0; yuv422 10bit: 1; rgb888: 2; default 2
-w: set width, default 3840
-h: set Height, default 2160
--fps=60: set fps, default 60
-s: show fb, default false

Currently supports RGB888: 3840_2160_60, 3840_2160_30, 1920_1080_60
Usage example:
sample_kvm_vio -f 2 -w 3840 -h 2160 --fps=60