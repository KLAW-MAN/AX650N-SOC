#!/bin/sh

if [ $# -eq 0 ]; then
    mode="-i"
else
    mode=$1
fi

DEFAULT_CMM_POOL_CONFIG=anonymous,0,0x148000000,3072M
cmm_pool_config=$DEFAULT_CMM_POOL_CONFIG

function load_board_cmm_config()
{
    # 0x4200184: COMM_SYS_DUMMY_SW1 to store board_id
    local output=$(ax_lookat 0x4200184)
    local regval=$(echo "$output" | sed -n '2p' | sed 's/.*[[:space:]]\([0-9a-fA-F]\{8\}\)[[:space:]]*$/\1/')
    if [ -z "$regval" ]; then
        echo "fail to get board id"
        return
    fi

    # the high 16bit value stores the board id
    local board_hex=${regval:0:4}
    local board_id=$(printf "%d" "0x$board_hex")
    echo "board id: $board_id"

    local conf_file="/soc/scripts/board_cmm.conf"
    if [ -r "$conf_file" ] && [ -s "$conf_file" ]; then
        local line=$(grep -E "^[[:space:]]*$board_id[[:space:]]*=" "$conf_file")
        if [ -n "$line" ]; then
            cmm_pool_config=$(echo "$line" | sed "s/^[[:space:]]*$board_id[[:space:]]*=[[:space:]]*//")
        else
            echo "board_id $board_id is not defined in board_cmm.conf"
        fi
    else
        echo "board_cmm.conf not found or empty"
    fi

    echo "cmm_pool_config: $cmm_pool_config"
}


function load_drv()
{
    insmod /soc/ko/ax_mailbox.ko
    insmod /soc/ko/ax_vdsp.ko &
    insmod /soc/ko/ax_cmm.ko cmmpool=$cmm_pool_config
    insmod /soc/ko/ax_sys.ko
    insmod /soc/ko/ax_pool.ko
    insmod /soc/ko/ax_base.ko
    insmod /soc/ko/ax_ivps.ko
    insmod /soc/ko/ax_tdp.ko
    insmod /soc/ko/ax_vdec.ko &
    insmod /soc/ko/ax_vpp.ko &
    insmod /soc/ko/ax_vgp.ko &
    insmod /soc/ko/ax_gdc.ko &
    insmod /soc/ko/ax_npu.ko
    insmod /soc/ko/ax_venc.ko proc_num=16
    insmod /soc/ko/ax_jenc.ko &
    insmod /soc/ko/ax_jdec.ko &
    insmod /soc/ko/ax_ive.ko &
}

function remove_drv()
{
    rmmod ax_ive
    rmmod ax_jdec
    rmmod ax_jenc
    rmmod ax_venc
    rmmod ax_npu
    rmmod ax_gdc
    rmmod ax_vgp
    rmmod ax_vpp
    rmmod ax_vdec
    rmmod ax_tdp
    rmmod ax_ivps
    rmmod ax_base
    rmmod ax_pool
    rmmod ax_sys
    rmmod ax_cmm
    rmmod axcl_slave
    rmmod ax_pcie_mmb
    rmmod ax_pcie_dma_slave
    rmmod ax_pcie_msg
    rmmod ax_pcie_slave_dev
    rmmod ax_vdsp
    rmmod ax_mailbox
}

function auto_drv()
{
    if [ "$mode" == "-i" ]; then
        load_board_cmm_config
        load_drv
    elif [ "$mode" == "-r" ]; then
        remove_drv
    else
        echo "[error] Invalid param, please use the following parameters:"
        echo "-i:  insmod"
        echo "-r:  rmmod"
    fi
}

auto_drv

exit 0
