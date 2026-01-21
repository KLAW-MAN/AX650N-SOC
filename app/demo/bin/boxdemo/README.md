#BoxDemo
## How to run?
1. cd /opt/bin/BoxDemo
2. vi box.conf to configure the log level, as well as the detection switch, etc.。[Parameters Description](#ParamConfig)
3. ./run.sh

## How to compile?
1. cd app/demo/src/ppl/box
2. make p=xxx clean
3. make p=xxx
4. make p=xxx install
> p=xxx set project name, e.g. make p=AX650_emmc

# <a href="#ParamConfig">Parameters Configure</a>

|   #   | Parameter             | Range        | Description                             |
| ----- | ----------------------| ------------ | --------------------------------------- |
|   1   | [DETECT]enable        |              | 0: disable, 1:enable                    |
|   2   | [DISPC]dev            | [0 - 1]      | 0: HDMI-0, 1: HDMI-1                    |
|   3   | [STREAM]count         | [1 - 25]     | count of channels                       |