#!/bin/sh
mode=$1
vendorid="1f4b"
pcie0_status=`cat /sys/firmware/devicetree/base/soc/pcie@40000000/status`
pcie1_status=`cat /sys/firmware/devicetree/base/soc/pcie@60000000/status`

function pcie_host_driver_remove()
{
    echo "pcie host driver remove"
    rmmod ax_pcie_p2p_rc
    rmmod ax_pcie_mmb
    rmmod ax_pcie_msg
    rmmod ax_pcie_boot
    rmmod ax_pcie_host_dev
}

function pcie_host_driver_load()
{
    echo "pcie host driver load"
    insmod /soc/ko/ax_pcie_host_dev.ko
    insmod /soc/ko/ax_pcie_boot.ko
    insmod /soc/ko/ax_pcie_msg.ko
    insmod /soc/ko/ax_pcie_mmb.ko
    insmod /soc/ko/ax_pcie_p2p_rc.ko
}

function pcie_slave_device_load()
{
    insmod /soc/ko/ax_pcie_slave_dev.ko
    insmod /soc/ko/ax_pcie_msg.ko
    insmod /soc/ko/ax_pcie_dma_slave.ko
    insmod /soc/ko/ax_pcie_mmb.ko
    insmod /soc/ko/ax_pcie_p2p_ep.ko
}

function pcie_slave_driver_remove()
{
    echo "pcie slave device remove"
    rmmod ax_pcie_p2p_ep
    rmmod ax_pcie_mmb
    rmmod ax_pcie_dma_slave
    rmmod ax_pcie_msg
    rmmod ax_pcie_slave_dev
}

main()
{
    type devmem2 > /dev/null 2>&1
    if [ $? -ne 0 ]; then
        pcie0_mode_sel=`devmem 0x30700010`
        pcie1_mode_sel=`devmem 0x30700034`
        if [[ $pcie0_mode_sel != 0x00000002 && $pcie1_mode_sel != 0x00000002 ]]; then
            exit 0
        fi
    else
        pcie0_mode_sel=`devmem2 0x30700010 |grep 0x30700010 |awk -F " " '{print$6}'`
        pcie1_mode_sel=`devmem2 0x30700010 |grep 0x30700034 |awk -F " " '{print$6}'`
        if [[ $pcie0_mode_sel != "0x2" && $pcie1_mode_sel != "0x2" ]]; then
            exit 0
        fi
    fi

    echo "run pcie_load_drv.sh start "
    if [ "$mode" == "-i" ]; then
        axdev=`lspci |grep ${vendorid}|wc -l`
        if [  $axdev -ne 0 ]; then
            echo "pcie host driver insmod"
            pcie_host_driver_load
        else
            if [[ $pcie0_status != "disabled" && $pcie0_mode_sel == 0x00000002 ]] || [[ $pcie1_status != "disabled" && $pcie1_mode_sel == 0x00000002 ]]; then
                echo "pcie slave driver insmod"
                pcie_slave_device_load
            fi
        fi
    elif [ "$mode" == "-r" ]; then
        axdev=`lspci |grep ${vendorid}|wc -l`
        if [  $axdev -ne 0 ]; then
            echo "pcie host driver rmmod"
            pcie_host_driver_remove
        else
            if [[ $pcie0_status != "disabled" && $pcie0_mode_sel == 0x00000002 ]] || [[ $pcie1_status != "disabled" && $pcie1_mode_sel == 0x00000002 ]]; then
                echo "pcie slave driver rmmod"
                pcie_slave_driver_remove
            fi
        fi
    else
        echo "invalid parameter, Please use the following parameters"
        echo "-i:  insmod"
        echo "-r:  rmmod"
    fi

    echo "run pcie_load_drv.sh end"
}
main
