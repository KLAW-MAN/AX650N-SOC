#!/bin/sh

# usage
# ./usb-mass-storage.sh usb2/usb3 start
# ./usb-mass-storage.sh usb2/usb3 stop

usb_dev_controller=${1}_mass_storage
# current_path=`pwd`
export CONFIGFS_HOME=/etc/configfs
UDC=$CONFIGFS_HOME/usb_gadget/${usb_dev_controller}/UDC
#SD Card Dev Node
SD=/dev/mmcblk1p1

function mass_storage_stop()
{
    echo > ${UDC}
}

function mass_storage_start()
{
    mkdir -p $CONFIGFS_HOME
    if [ ! -d "${CONFIGFS_HOME}/usb_gadget/" ]; then
            mount none $CONFIGFS_HOME -t configfs
    fi

    mkdir -p $CONFIGFS_HOME/usb_gadget/${usb_dev_controller}
    cd $CONFIGFS_HOME/usb_gadget/${usb_dev_controller}

    echo 0x32c9 > idVendor
    echo 0x2004 > idProduct

    mkdir -p strings/0x409 -m 0770
    echo "axera-ax650" > strings/0x409/serialnumber
    echo "axera" > strings/0x409/manufacturer
    echo "ax650-mass-storage" > strings/0x409/product

    mkdir -p configs/c.1 -m 0770
    mkdir -p configs/c.1/strings/0x409 -m 0770
    echo "mass_storage" > configs/c.1/strings/0x409/configuration
    echo 120 > configs/c.1/MaxPower

    mkdir -p functions/mass_storage.usb0
    echo ${SD} >functions/mass_storage.usb0/lun.0/file
    ln -s functions/mass_storage.usb0 configs/c.1

    #echo "usb mass storage start"
    if [ ${1} = "usb2" ]; then
            echo 28100000.dwc3 > ${UDC}
    fi

    if [ ${1} = "usb3" ]; then
            echo 30c00000.dwc3 > ${UDC}
    fi
}

if [ $# != 2 ]; then
	echo "usage:"
	echo "usb-mass-storage.sh usb2/usb3 start"
	echo "usb-mass-storage.sh usb2/usb3 stop"
	exit 0
fi

if [ $2 == "stop" ]; then
        if [ -f ${UDC} ]; then
            read line < ${UDC}
            if [ -z "${line}" ]; then
                    echo "usb mass storage has stopped" 	# so we don't need to stop it again
                    exit 0
            fi
        else
            echo "usb mass storage didn't start"
            exit 0
        fi

        echo "usb mass storage stop"
        mass_storage_stop
        exit 0
fi

if [ $2 == "start" ]; then
    if [ -f ${UDC} ]; then
            read line < ${UDC}
            if [ -n "${line}" ]; then
                    echo "usb mass storage has enabled" 	# so we don't need to enable it again
                    exit 0
            fi
    fi
    echo "usb mass storage start"
    mass_storage_start $1
    exit 0
fi

# cd ${current_path}
