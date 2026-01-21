#!/bin/sh

# usage
# ./gser.sh usb2  ./gser.sh usb3  ./gser.sh usb2 disable

usb_dev_controller=${1}_gser
# current_path=`pwd`
export CONFIGFS_HOME=/etc/configfs
UDC=$CONFIGFS_HOME/usb_gadget/${usb_dev_controller}/UDC

if [ $# == 0 ]; then
	echo "usage:"
	echo "./gser.sh usb2  ./gser.sh usb3  ./gser.sh usb2 disable"
	exit 0
fi


if [ $# == 2 ]; then
	if [ $2 == "disable" ]; then
		echo > ${UDC}
		exit 0
	fi
fi

if [ -f ${UDC} ]; then
	echo "enable usb gadget"
	if [ ${1} = "usb2" ]; then
		echo 28100000.dwc3 > ${UDC}
	else
		echo 28100000.dwc3 > ${UDC}
	fi

	exit 0
fi


mkdir -p $CONFIGFS_HOME
if [ ! -d "${CONFIGFS_HOME}/usb_gadget/" ]; then
	mount none $CONFIGFS_HOME -t configfs
fi

mkdir -p $CONFIGFS_HOME/usb_gadget/${usb_dev_controller}
cd $CONFIGFS_HOME/usb_gadget/${usb_dev_controller}

echo 0x2222 > idVendor
echo 0x1111 > idProduct

mkdir -p strings/0x409
echo 20200210 > strings/0x409/serialnumber
echo "AIchip" > strings/0x409/manufacturer
echo "usb generic serial gadget" > strings/0x409/product

mkdir -p configs/c.1

mkdir -p configs/c.1/strings/0x409
echo gser > configs/c.1/strings/0x409/configuration
echo 120 > configs/c.1/MaxPower

mkdir -p functions/gser.usb0

ln -s functions/gser.usb0 configs/c.1

echo "enable usb gadget"
if [ ${1} = "usb2" ]; then
	echo 28100000.dwc3 > UDC
else
	echo 28100000.dwc3 > UDC
fi

# cd ${current_path}
