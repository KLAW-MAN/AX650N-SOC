#!/bin/sh

# usage
# ./usb-adb.sh usb2/3 start
# ./usb-adb.sh usb2/3 stop

usb_dev_controller=${1}_adb
# current_path=`pwd`
export CONFIGFS_HOME=/etc/configfs
UDC=$CONFIGFS_HOME/usb_gadget/${usb_dev_controller}/UDC

function adb_stop()
{
    echo > ${UDC}
    killall adbd
    rm $CONFIGFS_HOME/usb_gadget/${usb_dev_controller}/configs/c.1/ffs.adb
    umount /dev/usb-ffs/adb
}

function adb_start()
{
    mkdir -p $CONFIGFS_HOME
    if [ ! -d "${CONFIGFS_HOME}/usb_gadget/" ]; then
            mount none $CONFIGFS_HOME -t configfs
    fi

    mkdir -p $CONFIGFS_HOME/usb_gadget/${usb_dev_controller}
    cd $CONFIGFS_HOME/usb_gadget/${usb_dev_controller}

    echo 0x32c9 > idVendor
    echo 0x2003 > idProduct

    mkdir -p strings/0x409 -m 0770
    echo "axera-ax650" > strings/0x409/serialnumber
    echo "axera" > strings/0x409/manufacturer
    echo "ax650-adb" > strings/0x409/product

    mkdir -p configs/c.1 -m 0770
    mkdir -p configs/c.1/strings/0x409 -m 0770
    echo "adb" > configs/c.1/strings/0x409/configuration
    echo 120 > configs/c.1/MaxPower

    mkdir -p functions/ffs.adb
    ln -s functions/ffs.adb configs/c.1

    mkdir -p /dev/usb-ffs/adb -m 0770
    mount -t functionfs adb /dev/usb-ffs/adb

    cd /
    adbd  &
    sleep 1

    echo "enable usb gadget adb"
    if [ ${1} = "usb3" ]; then
            echo 30c00000.dwc3 > ${UDC}
    fi

    if [ ${1} = "usb2" ]; then
            echo 28100000.dwc3 > ${UDC}
    fi
}

if [ $# != 2 ]; then
	echo "usage:"
	echo "usb-adb.sh usb2/3 start"
	echo "usb-adb.sh usb2/3 stop"
	exit 0
fi

if [ $2 == "stop" ]; then
        if [ -f ${UDC} ]; then
            read line < ${UDC}
            if [ -z "${line}" ]; then
                    echo "usb adb has stopped" 	# so we don't need to stop it again
                    exit 0
            fi
        else
            echo "usb adb didn't start"
            exit 0
        fi

        echo "usb adb stop"
        adb_stop
        exit 0
fi

if [ $2 == "start" ]; then
    if [ -f ${UDC} ]; then
            read line < ${UDC}
            if [ -n "${line}" ]; then
                    echo "usb adb has enabled" 	# so we don't need to enable it again
                    exit 0
            fi
    fi
    echo "usb adb start"
    adb_start $1
    exit 0
fi

# cd ${current_path}
