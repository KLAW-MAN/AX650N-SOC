#!/bin/sh

# ----------------------------------------------------------------------------------------------#
#
#  TP2803 BT656 NTSC(720x480I30) PCLK:13.5M BT-CLK:27M
#
# ----------------------------------------------------------------------------------------------#
#
function bt656_ntsc_interlaced() {
    local bus=$1
	local addr=$2

    echo "bt656 ntsc interlaced i2c:${bus} chip:${addr}"
    i2cset -f -y ${bus} ${addr} 0x02 0x03
    i2cset -f -y ${bus} ${addr} 0x03 0x62
    i2cset -f -y ${bus} ${addr} 0x05 0x24
    i2cset -f -y ${bus} ${addr} 0x07 0x82
    i2cset -f -y ${bus} ${addr} 0x08 0x38
    i2cset -f -y ${bus} ${addr} 0x09 0x42
    i2cset -f -y ${bus} ${addr} 0x0a 0x28
    i2cset -f -y ${bus} ${addr} 0x0b 0x05
    i2cset -f -y ${bus} ${addr} 0x0c 0x04
    i2cset -f -y ${bus} ${addr} 0x0d 0x04
    i2cset -f -y ${bus} ${addr} 0x0e 0x3c
    i2cset -f -y ${bus} ${addr} 0x0f 0x5a
    i2cset -f -y ${bus} ${addr} 0x10 0x03
    i2cset -f -y ${bus} ${addr} 0x11 0x24
    i2cset -f -y ${bus} ${addr} 0x12 0x00
    i2cset -f -y ${bus} ${addr} 0x13 0x3c
    i2cset -f -y ${bus} ${addr} 0x15 0x38
    i2cset -f -y ${bus} ${addr} 0x1b 0x97
    i2cset -f -y ${bus} ${addr} 0x1c 0x81
    i2cset -f -y ${bus} ${addr} 0x1d 0xb6
    i2cset -f -y ${bus} ${addr} 0x1e 0x80
    i2cset -f -y ${bus} ${addr} 0x20 0x21
    i2cset -f -y ${bus} ${addr} 0x21 0xf0
    i2cset -f -y ${bus} ${addr} 0x22 0x7c
    i2cset -f -y ${bus} ${addr} 0x23 0x1f
    i2cset -f -y ${bus} ${addr} 0x29 0x34
    i2cset -f -y ${bus} ${addr} 0x2a 0x19
    i2cset -f -y ${bus} ${addr} 0x3b 0xd0
    i2cset -f -y ${bus} ${addr} 0x3c 0x10
    i2cset -f -y ${bus} ${addr} 0x3e 0x18
    i2cset -f -y ${bus} ${addr} 0x45 0x40
    i2cset -f -y ${bus} ${addr} 0xf1 0x18
    i2cset -f -y ${bus} ${addr} 0xf2 0x16
    i2cset -f -y ${bus} ${addr} 0xf4 0xC1
    i2cset -f -y ${bus} ${addr} 0x3a 0x00
}

# ----------------------------------------------------------------------------------------------#
#
#  TP2803 BT656 PAL(720x576I25) PCLK:13.5M BT-CLK:27M
#
# ----------------------------------------------------------------------------------------------#
#
function bt656_pal_interlaced() {
    local bus=$1
	local addr=$2

    echo "bt656 pal interlaced i2c:${bus} chip:${addr}"
    i2cset -f -y ${bus} ${addr} 0x02 0x1a
    i2cset -f -y ${bus} ${addr} 0x03 0x63
    i2cset -f -y ${bus} ${addr} 0x05 0x20
    i2cset -f -y ${bus} ${addr} 0x07 0xC1
    i2cset -f -y ${bus} ${addr} 0x08 0x38
    i2cset -f -y ${bus} ${addr} 0x09 0x42
    i2cset -f -y ${bus} ${addr} 0x0a 0x28
    i2cset -f -y ${bus} ${addr} 0x0b 0x05
    i2cset -f -y ${bus} ${addr} 0x0c 0x04
    i2cset -f -y ${bus} ${addr} 0x0d 0x04
    i2cset -f -y ${bus} ${addr} 0x0e 0x3c
    i2cset -f -y ${bus} ${addr} 0x0f 0x60
    i2cset -f -y ${bus} ${addr} 0x10 0x03
    i2cset -f -y ${bus} ${addr} 0x11 0x20
    i2cset -f -y ${bus} ${addr} 0x12 0x04
    i2cset -f -y ${bus} ${addr} 0x13 0x3f
    i2cset -f -y ${bus} ${addr} 0x15 0x3b
    i2cset -f -y ${bus} ${addr} 0x1b 0xa0
    i2cset -f -y ${bus} ${addr} 0x1c 0x88
    i2cset -f -y ${bus} ${addr} 0x1d 0xc1
    i2cset -f -y ${bus} ${addr} 0x1e 0x5f
    i2cset -f -y ${bus} ${addr} 0x20 0x48
    i2cset -f -y ${bus} ${addr} 0x21 0xbb
    i2cset -f -y ${bus} ${addr} 0x22 0x2e
    i2cset -f -y ${bus} ${addr} 0x23 0x8b
    i2cset -f -y ${bus} ${addr} 0x29 0x34
    i2cset -f -y ${bus} ${addr} 0x2a 0x19
    i2cset -f -y ${bus} ${addr} 0x3b 0xd0
    i2cset -f -y ${bus} ${addr} 0x3c 0x10
    i2cset -f -y ${bus} ${addr} 0x3e 0x18
    i2cset -f -y ${bus} ${addr} 0x45 0x40
    i2cset -f -y ${bus} ${addr} 0xf4 0xa1
    i2cset -f -y ${bus} ${addr} 0x3a 0x00
}

# ----------------------------------------------------------------------------------------------#
#
#  TP2803 BT1120 NTSC(720x480I30) PCLK:13.5M BT-CLK:13.5M
#
# ----------------------------------------------------------------------------------------------#
#
function bt1120_ntsc_interlaced() {
    local bus=$1
	local addr=$2

    echo "bt1120 ntsc interlaced i2c:${bus} chip:${addr}"
    i2cset -f -y ${bus} ${addr} 0x02 0x09
    i2cset -f -y ${bus} ${addr} 0x03 0x62
    i2cset -f -y ${bus} ${addr} 0x05 0x24
    i2cset -f -y ${bus} ${addr} 0x07 0x01
    i2cset -f -y ${bus} ${addr} 0x08 0x38
    i2cset -f -y ${bus} ${addr} 0x09 0x42
    i2cset -f -y ${bus} ${addr} 0x0a 0x28
    i2cset -f -y ${bus} ${addr} 0x0b 0x05
    i2cset -f -y ${bus} ${addr} 0x0c 0x04
    i2cset -f -y ${bus} ${addr} 0x0d 0x04
    i2cset -f -y ${bus} ${addr} 0x0e 0x3c
    i2cset -f -y ${bus} ${addr} 0x0f 0x5a
    i2cset -f -y ${bus} ${addr} 0x10 0x03
    i2cset -f -y ${bus} ${addr} 0x11 0x16
    i2cset -f -y ${bus} ${addr} 0x12 0x04
    i2cset -f -y ${bus} ${addr} 0x13 0x3c
    i2cset -f -y ${bus} ${addr} 0x15 0x38
    i2cset -f -y ${bus} ${addr} 0x18 0x10
    i2cset -f -y ${bus} ${addr} 0x1b 0x97
    i2cset -f -y ${bus} ${addr} 0x1c 0x81
    i2cset -f -y ${bus} ${addr} 0x1d 0xb6
    i2cset -f -y ${bus} ${addr} 0x1e 0x80
    i2cset -f -y ${bus} ${addr} 0x20 0x48
    i2cset -f -y ${bus} ${addr} 0x21 0xbb
    i2cset -f -y ${bus} ${addr} 0x22 0x2e
    i2cset -f -y ${bus} ${addr} 0x23 0x8b
    i2cset -f -y ${bus} ${addr} 0x29 0x34
    i2cset -f -y ${bus} ${addr} 0x2a 0x19
    i2cset -f -y ${bus} ${addr} 0x3b 0xd0
    i2cset -f -y ${bus} ${addr} 0x3c 0x10
    i2cset -f -y ${bus} ${addr} 0x3e 0x18
    i2cset -f -y ${bus} ${addr} 0x45 0x40
    i2cset -f -y ${bus} ${addr} 0xf1 0x08
    i2cset -f -y ${bus} ${addr} 0xf2 0x0e
    i2cset -f -y ${bus} ${addr} 0xf3 0x4a
    i2cset -f -y ${bus} ${addr} 0xf4 0x21
    i2cset -f -y ${bus} ${addr} 0x3a 0x00
}

# ----------------------------------------------------------------------------------------------#
#
#  TP2803 BT1120 PAL(720x576I25) PCLK:13.5M BT-CLK:13.5M
#
# ----------------------------------------------------------------------------------------------#
#
function bt1120_pal_interlaced() {
    local bus=$1
	local addr=$2

    echo "bt1120 pal interlaced i2c:${bus} chip:${addr}"
    i2cset -f -y ${bus} ${addr} 0x02 0x1a
    i2cset -f -y ${bus} ${addr} 0x03 0x63
    i2cset -f -y ${bus} ${addr} 0x05 0x20
    i2cset -f -y ${bus} ${addr} 0x07 0x01
    i2cset -f -y ${bus} ${addr} 0x08 0x38
    i2cset -f -y ${bus} ${addr} 0x09 0x42
    i2cset -f -y ${bus} ${addr} 0x0a 0x28
    i2cset -f -y ${bus} ${addr} 0x0b 0x05
    i2cset -f -y ${bus} ${addr} 0x0c 0x04
    i2cset -f -y ${bus} ${addr} 0x0d 0x04
    i2cset -f -y ${bus} ${addr} 0x0e 0x3c
    i2cset -f -y ${bus} ${addr} 0x0f 0x60
    i2cset -f -y ${bus} ${addr} 0x10 0x03
    i2cset -f -y ${bus} ${addr} 0x11 0x20
    i2cset -f -y ${bus} ${addr} 0x12 0x04
    i2cset -f -y ${bus} ${addr} 0x13 0x3f
    i2cset -f -y ${bus} ${addr} 0x15 0x3b
    i2cset -f -y ${bus} ${addr} 0x1b 0xa0
    i2cset -f -y ${bus} ${addr} 0x1c 0x88
    i2cset -f -y ${bus} ${addr} 0x1d 0xc1
    i2cset -f -y ${bus} ${addr} 0x1e 0x5f
    i2cset -f -y ${bus} ${addr} 0x20 0x48
    i2cset -f -y ${bus} ${addr} 0x21 0xbb
    i2cset -f -y ${bus} ${addr} 0x22 0x2e
    i2cset -f -y ${bus} ${addr} 0x23 0x8b
    i2cset -f -y ${bus} ${addr} 0x29 0x34
    i2cset -f -y ${bus} ${addr} 0x2a 0x19
    i2cset -f -y ${bus} ${addr} 0x3b 0xd0
    i2cset -f -y ${bus} ${addr} 0x3c 0x10
    i2cset -f -y ${bus} ${addr} 0x3e 0x18
    i2cset -f -y ${bus} ${addr} 0x45 0x40
    i2cset -f -y ${bus} ${addr} 0xf1 0x08
    i2cset -f -y ${bus} ${addr} 0xf2 0x0e
    i2cset -f -y ${bus} ${addr} 0xf3 0x4a
    i2cset -f -y ${bus} ${addr} 0xf4 0x21
    i2cset -f -y ${bus} ${addr} 0x3a 0x00
}

# ----------------------------------------------------------------------------------------------#
#
#  TP2803 BT656 1280x720P25 PCLK:37.125M BT-CLK:74.25M
#
# ----------------------------------------------------------------------------------------------#
#
function bt656_720p25_progressive() {
    local bus=$1
	local addr=$2

    echo "bt656 720p25 progressive i2c:${bus} chip:${addr}"
    i2cset -f -y ${bus} ${addr} 0x00 0x00
    i2cset -f -y ${bus} ${addr} 0x01 0x00
    i2cset -f -y ${bus} ${addr} 0x02 0x8B
    i2cset -f -y ${bus} ${addr} 0x03 0x62
    i2cset -f -y ${bus} ${addr} 0x04 0x00
    i2cset -f -y ${bus} ${addr} 0x05 0x70
    i2cset -f -y ${bus} ${addr} 0x06 0x00
    i2cset -f -y ${bus} ${addr} 0x07 0xC1
    i2cset -f -y ${bus} ${addr} 0x08 0x7A
    i2cset -f -y ${bus} ${addr} 0x09 0x7A
    i2cset -f -y ${bus} ${addr} 0x0A 0x7A
    i2cset -f -y ${bus} ${addr} 0x0B 0x15
    i2cset -f -y ${bus} ${addr} 0x0C 0x04
    i2cset -f -y ${bus} ${addr} 0x0D 0xF0
    i2cset -f -y ${bus} ${addr} 0x0E 0x86
    i2cset -f -y ${bus} ${addr} 0x0F 0xBC
    i2cset -f -y ${bus} ${addr} 0x10 0x17
    i2cset -f -y ${bus} ${addr} 0x11 0xB0
    i2cset -f -y ${bus} ${addr} 0x12 0x00
    i2cset -f -y ${bus} ${addr} 0x13 0x3C
    i2cset -f -y ${bus} ${addr} 0x14 0x38
    i2cset -f -y ${bus} ${addr} 0x15 0x39
    i2cset -f -y ${bus} ${addr} 0x16 0xEB
    i2cset -f -y ${bus} ${addr} 0x17 0x08
    i2cset -f -y ${bus} ${addr} 0x18 0x00
    i2cset -f -y ${bus} ${addr} 0x19 0xF0
    i2cset -f -y ${bus} ${addr} 0x1A 0x10
    i2cset -f -y ${bus} ${addr} 0x1B 0xA4
    i2cset -f -y ${bus} ${addr} 0x1C 0x72
    i2cset -f -y ${bus} ${addr} 0x1D 0x98
    i2cset -f -y ${bus} ${addr} 0x1E 0x80
    i2cset -f -y ${bus} ${addr} 0x1F 0x00
    i2cset -f -y ${bus} ${addr} 0x20 0x27
    i2cset -f -y ${bus} ${addr} 0x21 0x88
    i2cset -f -y ${bus} ${addr} 0x22 0x04
    i2cset -f -y ${bus} ${addr} 0x23 0x23
    i2cset -f -y ${bus} ${addr} 0x24 0x86
    i2cset -f -y ${bus} ${addr} 0x25 0x40
    i2cset -f -y ${bus} ${addr} 0x26 0x00
    i2cset -f -y ${bus} ${addr} 0x27 0x56
    i2cset -f -y ${bus} ${addr} 0x28 0x00
    i2cset -f -y ${bus} ${addr} 0x29 0x34
    i2cset -f -y ${bus} ${addr} 0x2A 0x19
    i2cset -f -y ${bus} ${addr} 0x2B 0x5e
    i2cset -f -y ${bus} ${addr} 0x2C 0x60
    i2cset -f -y ${bus} ${addr} 0x2D 0x00
    i2cset -f -y ${bus} ${addr} 0x2E 0x40
    i2cset -f -y ${bus} ${addr} 0x2F 0x80
    i2cset -f -y ${bus} ${addr} 0x30 0x01
    i2cset -f -y ${bus} ${addr} 0x31 0x00
    i2cset -f -y ${bus} ${addr} 0x32 0x00
    i2cset -f -y ${bus} ${addr} 0x33 0x00
    i2cset -f -y ${bus} ${addr} 0x34 0x00
    i2cset -f -y ${bus} ${addr} 0x35 0x00
    i2cset -f -y ${bus} ${addr} 0x36 0x00
    i2cset -f -y ${bus} ${addr} 0x37 0x00
    i2cset -f -y ${bus} ${addr} 0x38 0x00
    i2cset -f -y ${bus} ${addr} 0x39 0x00
    i2cset -f -y ${bus} ${addr} 0x3A 0x00
    i2cset -f -y ${bus} ${addr} 0x3B 0x90
    i2cset -f -y ${bus} ${addr} 0x3C 0x10
    i2cset -f -y ${bus} ${addr} 0x3D 0x00
    i2cset -f -y ${bus} ${addr} 0x3E 0x18
    i2cset -f -y ${bus} ${addr} 0x3F 0x00
    i2cset -f -y ${bus} ${addr} 0x45 0x41
    i2cset -f -y ${bus} ${addr} 0xF0 0x00
    i2cset -f -y ${bus} ${addr} 0xF1 0x68
    i2cset -f -y ${bus} ${addr} 0xF2 0x1E
    i2cset -f -y ${bus} ${addr} 0xF3 0x4a
    i2cset -f -y ${bus} ${addr} 0xF4 0x21
    i2cset -f -y ${bus} ${addr} 0xF5 0x00
}

# ----------------------------------------------------------------------------------------------#
#
#  TP2803 BT656 1280x720P30 PCLK:37.125M BT-CLK:74.25M
#
# ----------------------------------------------------------------------------------------------#
#
function bt656_720p30_progressive() {
    local bus=$1
	local addr=$2

    echo "bt656 720p30 progressive i2c:${bus} chip:${addr}"
    i2cset -f -y ${bus} ${addr} 0x00 0x00
    i2cset -f -y ${bus} ${addr} 0x01 0x00
    i2cset -f -y ${bus} ${addr} 0x02 0x8B
    i2cset -f -y ${bus} ${addr} 0x03 0x62
    i2cset -f -y ${bus} ${addr} 0x04 0x00
    i2cset -f -y ${bus} ${addr} 0x05 0x70
    i2cset -f -y ${bus} ${addr} 0x06 0x00
    i2cset -f -y ${bus} ${addr} 0x07 0xC2
    i2cset -f -y ${bus} ${addr} 0x08 0x7A
    i2cset -f -y ${bus} ${addr} 0x09 0x7A
    i2cset -f -y ${bus} ${addr} 0x0A 0x7A
    i2cset -f -y ${bus} ${addr} 0x0B 0x00
    i2cset -f -y ${bus} ${addr} 0x0C 0x04
    i2cset -f -y ${bus} ${addr} 0x0D 0xF0
    i2cset -f -y ${bus} ${addr} 0x0E 0x86
    i2cset -f -y ${bus} ${addr} 0x0F 0x72
    i2cset -f -y ${bus} ${addr} 0x10 0x06
    i2cset -f -y ${bus} ${addr} 0x11 0x64
    i2cset -f -y ${bus} ${addr} 0x12 0x00
    i2cset -f -y ${bus} ${addr} 0x13 0x3C
    i2cset -f -y ${bus} ${addr} 0x14 0x38
    i2cset -f -y ${bus} ${addr} 0x15 0x39
    i2cset -f -y ${bus} ${addr} 0x16 0xEB
    i2cset -f -y ${bus} ${addr} 0x17 0x08
    i2cset -f -y ${bus} ${addr} 0x18 0x00
    i2cset -f -y ${bus} ${addr} 0x19 0xF0
    i2cset -f -y ${bus} ${addr} 0x1A 0x10
    i2cset -f -y ${bus} ${addr} 0x1B 0xA4
    i2cset -f -y ${bus} ${addr} 0x1C 0x72
    i2cset -f -y ${bus} ${addr} 0x1D 0x98
    i2cset -f -y ${bus} ${addr} 0x1E 0x80
    i2cset -f -y ${bus} ${addr} 0x1F 0x00
    i2cset -f -y ${bus} ${addr} 0x20 0x27
    i2cset -f -y ${bus} ${addr} 0x21 0x72
    i2cset -f -y ${bus} ${addr} 0x22 0x80
    i2cset -f -y ${bus} ${addr} 0x23 0x77
    i2cset -f -y ${bus} ${addr} 0x24 0x86
    i2cset -f -y ${bus} ${addr} 0x25 0x40
    i2cset -f -y ${bus} ${addr} 0x26 0x00
    i2cset -f -y ${bus} ${addr} 0x27 0x56
    i2cset -f -y ${bus} ${addr} 0x28 0x00
    i2cset -f -y ${bus} ${addr} 0x29 0x36
    i2cset -f -y ${bus} ${addr} 0x2A 0x19
    i2cset -f -y ${bus} ${addr} 0x2B 0x5e
    i2cset -f -y ${bus} ${addr} 0x2C 0x60
    i2cset -f -y ${bus} ${addr} 0x2D 0x00
    i2cset -f -y ${bus} ${addr} 0x2E 0x40
    i2cset -f -y ${bus} ${addr} 0x2F 0x80
    i2cset -f -y ${bus} ${addr} 0x30 0x01
    i2cset -f -y ${bus} ${addr} 0x31 0x00
    i2cset -f -y ${bus} ${addr} 0x32 0x00
    i2cset -f -y ${bus} ${addr} 0x33 0x00
    i2cset -f -y ${bus} ${addr} 0x34 0x00
    i2cset -f -y ${bus} ${addr} 0x35 0x00
    i2cset -f -y ${bus} ${addr} 0x36 0x00
    i2cset -f -y ${bus} ${addr} 0x37 0x00
    i2cset -f -y ${bus} ${addr} 0x38 0x00
    i2cset -f -y ${bus} ${addr} 0x39 0x00
    i2cset -f -y ${bus} ${addr} 0x3A 0x00
    i2cset -f -y ${bus} ${addr} 0x3B 0xD0
    i2cset -f -y ${bus} ${addr} 0x3C 0x10
    i2cset -f -y ${bus} ${addr} 0x3D 0x00
    i2cset -f -y ${bus} ${addr} 0x3E 0x18
    i2cset -f -y ${bus} ${addr} 0x3F 0x00
    i2cset -f -y ${bus} ${addr} 0x45 0x40
    i2cset -f -y ${bus} ${addr} 0xF0 0x00
    i2cset -f -y ${bus} ${addr} 0xF1 0x68
    i2cset -f -y ${bus} ${addr} 0xF2 0x1E
    i2cset -f -y ${bus} ${addr} 0xF3 0x4a
    i2cset -f -y ${bus} ${addr} 0xF4 0x21
    i2cset -f -y ${bus} ${addr} 0xF5 0x00
}

# ----------------------------------------------------------------------------------------------#
#
#  TP2803 BT656 1280x720P30 PCLK:36M BT-CLK:72M
#
# ----------------------------------------------------------------------------------------------#
#
function bt656_720p30_36m_progressive() {
    local bus=$1
	local addr=$2

    echo "bt656 720p30 36m progressive i2c:${bus} chip:${addr}"
    i2cset -f -y ${bus} ${addr} 0x00 0x00
    i2cset -f -y ${bus} ${addr} 0x01 0x00
    i2cset -f -y ${bus} ${addr} 0x02 0x8B
    i2cset -f -y ${bus} ${addr} 0x03 0x62
    i2cset -f -y ${bus} ${addr} 0x04 0x00
    i2cset -f -y ${bus} ${addr} 0x05 0x6C
    i2cset -f -y ${bus} ${addr} 0x06 0x00
    i2cset -f -y ${bus} ${addr} 0x07 0xC1
    i2cset -f -y ${bus} ${addr} 0x08 0x76
    i2cset -f -y ${bus} ${addr} 0x09 0x76
    i2cset -f -y ${bus} ${addr} 0x0A 0x76
    i2cset -f -y ${bus} ${addr} 0x0B 0x00
    i2cset -f -y ${bus} ${addr} 0x0C 0x04
    i2cset -f -y ${bus} ${addr} 0x0D 0xF0
    i2cset -f -y ${bus} ${addr} 0x0E 0x82
    i2cset -f -y ${bus} ${addr} 0x0F 0x40
    i2cset -f -y ${bus} ${addr} 0x10 0x06
    i2cset -f -y ${bus} ${addr} 0x11 0x3E
    i2cset -f -y ${bus} ${addr} 0x12 0x00
    i2cset -f -y ${bus} ${addr} 0x13 0x3C
    i2cset -f -y ${bus} ${addr} 0x14 0x38
    i2cset -f -y ${bus} ${addr} 0x15 0x39
    i2cset -f -y ${bus} ${addr} 0x16 0xEB
    i2cset -f -y ${bus} ${addr} 0x17 0x10
    i2cset -f -y ${bus} ${addr} 0x18 0x10
    i2cset -f -y ${bus} ${addr} 0x19 0xF0
    i2cset -f -y ${bus} ${addr} 0x1A 0x10
    i2cset -f -y ${bus} ${addr} 0x1B 0xA4
    i2cset -f -y ${bus} ${addr} 0x1C 0x55
    i2cset -f -y ${bus} ${addr} 0x1D 0x76
    i2cset -f -y ${bus} ${addr} 0x1E 0x80
    i2cset -f -y ${bus} ${addr} 0x1F 0x00
    i2cset -f -y ${bus} ${addr} 0x20 0x28
    i2cset -f -y ${bus} ${addr} 0x21 0xAE
    i2cset -f -y ${bus} ${addr} 0x22 0x14
    i2cset -f -y ${bus} ${addr} 0x23 0x7A
    i2cset -f -y ${bus} ${addr} 0x24 0x86
    i2cset -f -y ${bus} ${addr} 0x25 0x40
    i2cset -f -y ${bus} ${addr} 0x26 0x00
    i2cset -f -y ${bus} ${addr} 0x27 0x56
    i2cset -f -y ${bus} ${addr} 0x28 0x00
    i2cset -f -y ${bus} ${addr} 0x29 0x34
    i2cset -f -y ${bus} ${addr} 0x2A 0x19
    i2cset -f -y ${bus} ${addr} 0x2B 0x5e
    i2cset -f -y ${bus} ${addr} 0x2C 0x60
    i2cset -f -y ${bus} ${addr} 0x2D 0x00
    i2cset -f -y ${bus} ${addr} 0x2E 0x00
    i2cset -f -y ${bus} ${addr} 0x2F 0x00
    i2cset -f -y ${bus} ${addr} 0x30 0x01
    i2cset -f -y ${bus} ${addr} 0x31 0x00
    i2cset -f -y ${bus} ${addr} 0x32 0x00
    i2cset -f -y ${bus} ${addr} 0x33 0x00
    i2cset -f -y ${bus} ${addr} 0x34 0x00
    i2cset -f -y ${bus} ${addr} 0x35 0x00
    i2cset -f -y ${bus} ${addr} 0x36 0x00
    i2cset -f -y ${bus} ${addr} 0x37 0x00
    i2cset -f -y ${bus} ${addr} 0x38 0x00
    i2cset -f -y ${bus} ${addr} 0x39 0x00
    i2cset -f -y ${bus} ${addr} 0x3A 0x00
    i2cset -f -y ${bus} ${addr} 0x3B 0xD0
    i2cset -f -y ${bus} ${addr} 0x3C 0x10
    i2cset -f -y ${bus} ${addr} 0x3D 0x80
    i2cset -f -y ${bus} ${addr} 0x3E 0x18
    i2cset -f -y ${bus} ${addr} 0x3F 0x00
    i2cset -f -y ${bus} ${addr} 0x45 0x40
    i2cset -f -y ${bus} ${addr} 0xF0 0x00
    i2cset -f -y ${bus} ${addr} 0xF1 0x18
    i2cset -f -y ${bus} ${addr} 0xF2 0x16
    i2cset -f -y ${bus} ${addr} 0xF3 0x4a
    i2cset -f -y ${bus} ${addr} 0xF4 0x61
    i2cset -f -y ${bus} ${addr} 0xF5 0x00
}

if [ $# -lt 3 ] ; then
  echo "USAGE:"
  echo "        ------------------------------"
  echo '       |$1         |$2      |$3       |'
  echo "       |-----------|--------|---------|"
  echo "       |BOARD-TYPE |I2C-BUS |SETTINGS |"
  echo "        ------------------------------"
  echo "       BOARD-TYPE: A or N"
  echo "       SETTINGS: 0 - BT656 NTSC INTERLACED"
  echo "                 1 - BT656 PAL INTERLACED"
  echo "                 2 - BT1120 NTSC INTERLACED"
  echo "                 3 - BT1120 PAL INTERLACED"
  echo "                 4 - BT656 720P25 PROGRESSIVE"
  echo "                 5 - BT656 720P30 PROGRESSIVE"
  echo "                 6 - BT656 720P30 PCLK-36M PROGRESSIVE"
  echo "       e.g.: $0 A 0 0  -- Use i2c0 settings0 to configure tp2803 on AX650A"
  exit 1
fi

# ----------------------------------------------------------------------------------------------#
#
#  TP2803 RESET, AX650N_EVB RESET-PIN:PORT2-A1(GPIO65), AX650A_DEMO RESET-PIN:PORT4-A4(GPIO132)
#
# ----------------------------------------------------------------------------------------------#
#
index=132
if [ $1 == "N" ] ; then
  index=65
fi

echo "reset-gpio:${index}"

if [ -d /sys/class/gpio/gpio${index} ]; then
  echo "gpio${index} already exists"
else
  echo ${index} > /sys/class/gpio/export
  usleep 1000
  echo "create gpio${index}"
fi

echo out > /sys/class/gpio/gpio${index}/direction
echo 1 > /sys/class/gpio/gpio${index}/value
usleep 10000
echo 0 > /sys/class/gpio/gpio${index}/value
usleep 20000
echo 1 > /sys/class/gpio/gpio${index}/value
usleep 30000

bus=$2
chip=0x45

#bt656_ntsc_interlaced $bus $chip
if [ $3 -eq 0 ] ; then
  bt656_ntsc_interlaced $bus $chip
elif [ $3 -eq 1 ] ; then
  bt656_pal_interlaced $bus $chip
elif [ $3 -eq 2 ] ; then
  bt1120_ntsc_interlaced $bus $chip
elif [ $3 -eq 3 ] ; then
  bt1120_pal_interlaced $bus $chip
elif [ $3 -eq 4 ] ; then
  bt656_720p25_progressive $bus $chip
elif [ $3 -eq 5 ] ; then
  bt656_720p30_progressive $bus $chip
elif [ $3 -eq 6 ] ; then
  bt656_720p30_36m_progressive $bus $chip
else
  echo "setting$3 unsupported"
fi
