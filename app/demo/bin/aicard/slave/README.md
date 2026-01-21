# AiCardDemo

## How to run?

1. cd /opt/bin/AiCard/slave
2. vi aicard_slave.conf to configure the number of decode channels, etc.[Parameters Description](#ParamConfig)
3. ./run.sh

## How to compile?

1. cd app/demo/src/ppl/aicard
2. make p=xxx clean
3. make p=xxx
4. make p=xxx install

> p=xxx set project name, e.g. make p=AX650_emmc

# <a href="#ParamConfig">Parameters Configure</a>


| # | Parameter           | Range | Description             |
| - | -------------- | -------- | ---------------- |
| 1 | [VDEC]count    |          | count of channels         |
| 2 | [DETECT]enable | [1 - 32] | 0: disable, 1: enable |
