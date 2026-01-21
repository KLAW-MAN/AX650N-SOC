#!/bin/sh

# ----------------------------------------------------------------------------------------------#
#
#  AX650 PORT:G4, Impact modules are RGMII0/PCIE0/HDMI0/SPI-M1
#
# ----------------------------------------------------------------------------------------------#
#
ax_lookat 0x0425040C -s 0x00010003
ax_lookat 0x04250418 -s 0x00010003
ax_lookat 0x04250424 -s 0x00010083
ax_lookat 0x04250430 -s 0x00010083
ax_lookat 0x0425043C -s 0x00010003
ax_lookat 0x04250448 -s 0x00010003
ax_lookat 0x04250454 -s 0x00010003
ax_lookat 0x04250460 -s 0x00010003
ax_lookat 0x0425046C -s 0x00010003
ax_lookat 0x04250478 -s 0x00010003
ax_lookat 0x04250484 -s 0x00010003
ax_lookat 0x04250490 -s 0x00010003
ax_lookat 0x0425049C -s 0x00010003
ax_lookat 0x042504A8 -s 0x00010003
ax_lookat 0x042504B4 -s 0x00010003
ax_lookat 0x042504C0 -s 0x00010003
ax_lookat 0x042504CC -s 0x00010003
ax_lookat 0x042504D8 -s 0x00010093
ax_lookat 0x042504E4 -s 0x00010093
ax_lookat 0x042504F0 -s 0x00010003
ax_lookat 0x042504FC -s 0x00010093
ax_lookat 0x04250508 -s 0x00010093
ax_lookat 0x04250514 -s 0x00010083
ax_lookat 0x04250520 -s 0x00010083
