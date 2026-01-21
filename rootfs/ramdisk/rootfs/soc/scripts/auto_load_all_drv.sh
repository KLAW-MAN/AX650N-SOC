#!/bin/sh

echo "run auto_load_all_drv.sh start "
insmod /soc/ko/ax_mailbox.ko
insmod /soc/ko/ax_vdsp.ko
insmod /soc/ko/ax_cmm.ko cmmpool=anonymous,0,0x150000000,3072M
insmod /soc/ko/ax_sys.ko
insmod /soc/ko/ax_pool.ko
insmod /soc/ko/ax_base.ko
insmod /soc/ko/ax_vdec.ko
insmod /soc/ko/ax_ivps.ko
insmod /soc/ko/ax_vpp.ko
insmod /soc/ko/ax_vgp.ko
insmod /soc/ko/ax_gdc.ko
insmod /soc/ko/ax_tdp.ko
insmod /soc/ko/ax_vo.ko
insmod /soc/ko/ax_mipi_rx.ko
insmod /soc/ko/ax_npu.ko
insmod /soc/ko/ax_ddr_dfs.ko

insmod /soc/ko/ax_cipher.ko

insmod /soc/ko/ax_venc.ko
insmod /soc/ko/ax_jenc.ko
insmod /soc/ko/ax_jdec.ko
insmod /soc/ko/ax_audio.ko
insmod /soc/ko/ax_ive.ko
insmod /soc/ko/ax_fb.ko

echo "run auto_load_all_drv.sh end "

exit 0
