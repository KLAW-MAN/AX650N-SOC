#!/bin/sh

role=$1


function start_ep_net()
{
    echo "start pcie slave net..."

    insmod /soc/ko/ax_pcie_ep_net2.ko
    ifconfig ax-net0 192.168.1.1

    echo 500000 > /proc/sys/net/core/rmem_default
    echo 500000 > /proc/sys/net/core/rmem_max
}

function start_rc_net()
{
    echo "start pcie host net..."

    insmod /soc/ko/ax_pcie_rc_net2.ko
    ifconfig ax-net0 192.168.1.2

    echo 500000 > /proc/sys/net/core/rmem_default
    echo 500000 > /proc/sys/net/core/rmem_max
}

main()
{
    if [ "$role" == "slave" ]; then
        start_ep_net
    elif [ "$role" == "master" ]; then
        start_rc_net
    else
        echo "invalid param, use following param"
        echo "$0 slave/master"
    fi
}

main

