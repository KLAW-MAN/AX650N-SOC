#!/bin/sh

role=$1
PCIEx=$2


function start_ep_net()
{
    echo "start pcie ep net..."

    /soc/scripts/pcie_load_drv.sh -r

    insmod /soc/ko/pcie-ax-ep.ko
    insmod /soc/ko/ax_pcie_ep_net.ko

    mount -t configfs none /sys/kernel/config/
    cd /sys/kernel/config/pci_ep/
    mkdir functions/pci_epf_axnet/func1
    echo 16 > functions/pci_epf_axnet/func1/msi_interrupts
    ln -s functions/pci_epf_axnet/func1 controllers/${PCI_DEV}
    echo 1 > controllers/${PCI_DEV}/start

    ifconfig ax-net0 192.168.1.1

    echo 500000 > /proc/sys/net/core/rmem_default
    echo 500000 > /proc/sys/net/core/rmem_max
}

function start_rc_net()
{
    echo "start pcie rc net..."

    /soc/scripts/pcie_load_drv.sh -r

    BUS_NUM=$(lspci | grep 1f4b:0650 | awk '{print $1}')
    echo 1 > /sys/bus/pci/devices/${BUS_NUM}/remove
    echo 1 >/sys/bus/pci/rescan

    insmod /soc/ko/ax_pcie_rc_net.ko

    ifconfig ax-net0 192.168.1.2

    echo 500000 > /proc/sys/net/core/rmem_default
    echo 500000 > /proc/sys/net/core/rmem_max
}

main()
{
    if [ "$role" == "slave" ]; then
        if [ "$PCIEx" == "pcie0" ]; then
            PCI_DEV=40000000.pcie
        elif [ "$PCIEx" == "pcie1" ]; then
            PCI_DEV=60000000.pcie
        else
            echo "use defaulet pcie: pcie0"
            PCI_DEV=40000000.pcie
        fi
        start_ep_net
    elif [ "$role" == "master" ]; then
        start_rc_net
    else
        echo "invalid param, use following param"
        echo "$0 slave/master pcie0/pcie1"
    fi
}

main
