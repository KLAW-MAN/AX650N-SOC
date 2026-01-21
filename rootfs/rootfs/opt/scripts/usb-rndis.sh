#!/bin/sh

# usage
# ./usb-rndis.sh usb2/3 start
# ./usb-rndis.sh usb2/3 stop

usb_dev_controller=${1}_rndis
# current_path=`pwd`
export CONFIGFS_HOME=/etc/configfs
UDC=$CONFIGFS_HOME/usb_gadget/${usb_dev_controller}/UDC

function rndis_stop()
{
    echo > ${UDC}
    rm $CONFIGFS_HOME/usb_gadget/${usb_dev_controller}/configs/c.1/rndis.usb0
}

function rndis_start()
{
    mkdir -p $CONFIGFS_HOME
    if [ ! -d "${CONFIGFS_HOME}/usb_gadget/" ]; then
            mount none $CONFIGFS_HOME -t configfs
    fi

    mkdir -p $CONFIGFS_HOME/usb_gadget/${usb_dev_controller}
    cd $CONFIGFS_HOME/usb_gadget/${usb_dev_controller}

    echo 0x32c9 > idVendor
    echo 0x2005 > idProduct

    mkdir -p strings/0x409 -m 0770
    echo "axera-ax650" > strings/0x409/serialnumber
    echo "axera" > strings/0x409/manufacturer
    echo "ax650-rndis" > strings/0x409/product

    mkdir -p configs/c.1 -m 0770
    mkdir -p configs/c.1/strings/0x409 -m 0770
    echo "rndis" > configs/c.1/strings/0x409/configuration
    echo 120 > configs/c.1/MaxPower

	mkdir -p functions/rndis.usb0

	ln -s functions/rndis.usb0 configs/c.1

    echo "enable usb gadget rndis"
    if [ ${1} = "usb3" ]; then
            echo 30c00000.dwc3 > ${UDC}
    fi

    if [ ${1} = "usb2" ]; then
            echo 28100000.dwc3 > ${UDC}
    fi
}

if [ $# != 2 ]; then
	echo "usage:"
	echo "usb-rndis.sh usb2/3 start"
	echo "usb-rndis.sh usb2/3 stop"
	exit 0
fi

if [ $2 == "stop" ]; then
        if [ -f ${UDC} ]; then
            read line < ${UDC}
            if [ -z "${line}" ]; then
                    echo "usb rndis has stopped" 	# so we don't need to stop it again
                    exit 0
            fi
        else
            echo "usb rndis didn't start"
            exit 0
        fi

        echo "usb rndis stop"
        rndis_stop
        exit 0
fi

if [ $2 == "start" ]; then
    if [ -f ${UDC} ]; then
            read line < ${UDC}
            if [ -n "${line}" ]; then
                    echo "usb rndis has enabled" 	# so we don't need to enable it again
                    exit 0
            fi
    fi
    echo "usb rndis start"
    rndis_start $1
    exit 0
fi

# cd ${current_path}
