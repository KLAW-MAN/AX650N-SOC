#!/bin/sh

if [ $# -eq 0 ]; then
    mode="-i"
else
    mode=$1
fi

function load_drv()
{
    insmod /soc/ko/ax_pcie_slave_dev.ko
    insmod /soc/ko/ax_pcie_msg.ko
    insmod /soc/ko/ax_pcie_dma_slave.ko
    insmod /soc/ko/ax_pcie_mmb.ko
    insmod /soc/ko/axcl_slave.ko
    insmod /soc/ko/ax_pcie_p2p_ep.ko
}

function remove_drv()
{
    echo "removing pcie drives ..."
    rmmod ax_pcie_p2p_ep
    rmmod axcl_slave
    rmmod ax_pcie_mmb
    rmmod ax_pcie_dma_slave
    rmmod ax_pcie_msg
    rmmod ax_pcie_slave_dev
    echo "remove pcie drives done."
}

function auto_drv()
{
    if [ "$mode" == "-i" ]; then
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
