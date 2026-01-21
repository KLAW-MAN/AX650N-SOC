#!/bin/sh

echo "run auto_load_all_drv.sh start "
insmod /soc/ko/ax_mailbox.ko
#insmod /soc/ko/ax_vdsp.ko
insmod /soc/ko/ax_cmm.ko cmmpool=anonymous,0,0x180000000,4096M
insmod /soc/ko/ax_sys.ko
insmod /soc/ko/ax_pool.ko
insmod /soc/ko/ax_base.ko
#insmod /soc/ko/ax_vdec.ko
insmod /soc/ko/ax_ivps.ko
insmod /soc/ko/ax_vpp.ko
insmod /soc/ko/ax_vgp.ko
insmod /soc/ko/ax_gdc.ko
insmod /soc/ko/ax_tdp.ko
insmod /soc/ko/ax_fb.ko
#insmod /soc/ko/ax_vo.ko
insmod /soc/ko/ax_mipi_rx.ko
insmod /soc/ko/ax_npu.ko
insmod /soc/ko/ax_proton.ko
insmod /soc/ko/ax_ddr_dfs.ko

insmod /soc/ko/ax_cipher.ko

insmod /soc/ko/ax_venc.ko
insmod /soc/ko/ax_jenc.ko
#insmod /soc/ko/ax_jdec.ko
insmod /soc/ko/ax_audio.ko
#insmod /soc/ko/ax_pyralite.ko
insmod /soc/ko/ax_ive.ko
#insmod /soc/ko/ax_avs.ko



#Downclocking#
echo 1200000 > /sys/devices/system/cpu/cpufreq/policy0/scaling_setspeed
echo 400000000 > /sys/devices/platform/soc/soc:isp_ife/devfreq/soc:isp_ife/userspace/set_freq
echo 500000000 > /sys/devices/platform/soc/soc:isp_itp/devfreq/soc:isp_itp/userspace/set_freq
echo 500000000 > /sys/devices/platform/soc/1209c000.tdp/devfreq/1209c000.tdp/userspace/set_freq
echo 400000000 > /sys/devices/platform/soc/10130000.jenc/devfreq/10130000.jenc/userspace/set_freq
echo 400000000 > /sys/devices/platform/soc/1a000000.venc/devfreq/1a000000.venc/userspace/set_freq
echo 500000000 > /sys/devices/platform/soc/10103000.vgp/devfreq/10103000.vgp/userspace/set_freq

# Poweroff VDSP and VDEC ###
# step1 #
ax_lookat 0x4C0019C -s 0x4
ax_lookat 0x4C001B0 -s 0x4
# step2 #
ax_lookat 0x4C001DC -s 0xC
ax_lookat 0x4C001EC -s 0xC
ax_lookat 0x4C001FC -s 0xC
ax_lookat 0x4C0020C -s 0xC
ax_lookat 0x4C001BC -s 0xC
ax_lookat 0x4C001CC -s 0xC
ax_lookat 0x4C000DC -s 0xC
ax_lookat 0x4C00018

# pcie phy powerdown set
ax_lookat 0x30701018 -s 0x00100000 //set ctl0 phy0 reset
ax_lookat 0x3070101c -s 0x00100000 //set ctl0 phy1 reset
ax_lookat 0x30702020 -s 0x1 //clr ctl0 pipe_reset_n
ax_lookat 0x30701018 -s 0x10 //set ctl0 phy0 powerdown
ax_lookat 0x3070101c -s 0x10 //set ctl0 phy1 power down
#
ax_lookat 0x3070103c -s 0x00100000 //set ctl1 phy0 reset
ax_lookat 0x30701040 -s 0x00100000 //set ctl1 phy1 reset
ax_lookat 0x30702044 -s 0x1 //clr ctl1 pipe_reset_n
ax_lookat 0x3070103c -s 0x10 //set ctl1 phy0 powerdown
ax_lookat 0x30701040 -s 0x10 //set ctl1 phy1 powerdown

# HDMI phy powerdown set
ax_lookat 10033000 -s 0
ax_lookat 10034000 -s 0
#

echo "run auto_load_all_drv.sh end "

exit 0
