#!/bin/sh

dev_cnt=0
net_num=0

echo "start set pcie rc multi net..."

/soc/scripts/pcie_load_drv.sh -r

pcie_bdf=$(lspci | grep 1f4b:0650 | awk '{print $1}')
echo "$pcie_bdf" > /tmp/tmp_bdf
while IFS= read -r line; do
    echo 1 > /sys/bus/pci/devices/${line}/remove
    dev_cnt=$(($dev_cnt+1))
done < /tmp/tmp_bdf
rm /tmp/tmp_bdf

echo 1 > /sys/bus/pci/rescan

insmod /soc/ko/ax_pcie_rc_net.ko

echo "find $dev_cnt net dev"
busybox brctl addbr br0
while [ $dev_cnt -gt 0 ]; do
    echo "set ax-net$net_num"
    ifconfig ax-net$net_num up
    busybox brctl addif br0 ax-net$net_num
    dev_cnt=$(($dev_cnt-1))
    net_num=$(($net_num+1))
done
ifconfig br0 192.168.1.1

echo 500000 > /proc/sys/net/core/rmem_default
echo 500000 > /proc/sys/net/core/rmem_max

echo "configure success!"

