#!/bin/bash

action=$1
IP=192.168.1.2

function start_rc_net()
{
    echo "start pcie rc net..."

    echo 3 > /proc/sys/vm/drop_caches

    BUS_NUM=$(lspci -nn -D | grep 1f4b | awk '{print $1}')
    echo 1 > /sys/bus/pci/devices/${BUS_NUM}/remove
    echo 1 > /sys/bus/pci/rescan

    insmod ./out/ko/ax_pcie_net.ko
    sleep 0.1
    ifconfig ax-net0 ${IP}

    echo "start sucess, IP: ${IP}"
}

function remove_rc_net()
{
    echo "remove pcie rc net..."

    ifconfig ax-net0 down
    rmmod ax_pcie_net

    echo "remove sucess"
}

main()
{
    if [ "$action" == "up" ]; then
        start_rc_net
    elif [ "$action" == "down" ]; then
        remove_rc_net
    else
        start_rc_net
    fi
}

main
