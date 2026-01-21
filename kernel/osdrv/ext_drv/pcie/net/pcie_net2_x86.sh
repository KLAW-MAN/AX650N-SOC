#!/bin/bash

action=$1
IP=192.168.1.2

function start_rc_net()
{
    echo "start pcie rc net..."

    echo 3 > /proc/sys/vm/drop_caches
    insmod ./out/ko/ax_pcie_net2.ko
    sleep 0.1
    ifconfig ax-net0 ${IP}

    echo "start sucess, IP: ${IP}"
}

function remove_rc_net()
{
    echo "remove pcie rc net..."

    ifconfig ax-net0 down
    rmmod ax_pcie_net2

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
