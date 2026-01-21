#!/bin/sh

#***NOTES: dispatch irqs to specified cpus

#****************************************
#function: dispatch_irq2cpu
#param1: irq_num
#param2: cpu_val
#****************************************
function dispatch_irq2cpu(){
	if [ $# -lt 2 ];then
		echo "[error] @irq2cpu: too few parameters"
		return
	fi
	local irq=$1
	local cpu_val=$2

	echo $cpu_val > /proc/irq/$irq/smp_affinity
	return
}

#****************************************
#function: dispatch_irqs2cpu
#param1: interrupt name list
#param2: cpu_id
#****************************************
function dispatch_irqs2cpu(){
	if [ $# -lt 2 ]; then
		echo "[error] @interrupts_2_cpu: too few parameters"
		return
	fi
	local cpu_id=${@: -1}
	local index=0
	let val=$((1<<$cpu_id))
	cpu_val=$(echo "obase=16;$val"|bc)

	for name in $@
	do
		index=`expr $index + 1`
		if [ $index -ge $# ];then
			break
		fi
		irqs=$(cat /proc/interrupts | grep -w $name | awk -F : '{print $1}' | sed 's/^[ \t]*//g')
		for irq in $irqs
		do
			dispatch_irq2cpu $irq $cpu_val
		done
	done
}

echo "run interrupt_affinity.sh start "

#dispatch interrupts to cpu7
cpu7_irqs=""npu_normal0" "npu_normal1" "npu_normal2" "npu_normal3" "npu_err""
dispatch_irqs2cpu $cpu7_irqs 7

#dispatch interrupts to cpu6
cpu6_irqs=""ax_cipher" "eth0" "eth1""
dispatch_irqs2cpu $cpu6_irqs 6

#dispatch interrupts to cpu5
cpu5_irqs=""ax_jdec_vcmd_mem" "ax_vdec_vcmd_mem" "ax_proton_intt""
dispatch_irqs2cpu $cpu5_irqs 5

#dispatch interrupts to cpu4
cpu4_irqs=""ax_vo" "jenc" "venc" "gdc_dev""
dispatch_irqs2cpu $cpu4_irqs 4

#dispatch interrupts to cpu3
cpu3_irqs=""vpp" "tdp0_dev" "tdp1_dev""
dispatch_irqs2cpu $cpu3_irqs 3

#dispatch interrupts to cpu2
cpu2_irqs=""mmc0" "mmc1" "ax_mailbox" "pcie-dma-irq""
dispatch_irqs2cpu $cpu2_irqs 2

#dispatch interrupts to cpu1
cpu1_irqs=""xhci-hcd:usb1" "10071000.i2s_mst" "10070000.i2s_mst" "10030000.hdmi" "10040000.hdmi""
dispatch_irqs2cpu $cpu1_irqs 1

echo "run interrupt_affinity.sh end "

