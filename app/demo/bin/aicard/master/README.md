# AiCardDemo

## How to run?

1. cd /opt/bin/AiCard/master
2. vi aicard_master.conf Configure the number of videos for screen preview, etc. [Parameters Description](#ParamConfig)
3. ./run.sh

## How to compile?

1. cd app/demo/src/ppl/aicard
2. make p=xxx clean
3. make p=xxx
4. make p=xxx install

> p=xxx set project name, e.g. make p=AX650_emmc

# <a href="#ParamConfig">Parameters Configure</a>


| # | Parameter     | Range    | Description          |
| - | ------------- | -------- | -------------------- |
| 1 | [DISPC]dev    | [0 - 1]  | 0: HDMI-0, 1: HDMI-1 |
| 2 | [STREAM]count | [1 - 32] | count of channels    |
