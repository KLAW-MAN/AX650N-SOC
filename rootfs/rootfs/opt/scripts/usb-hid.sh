#!/bin/sh

# usage
# ./usb-hid.sh usb2/3 start
# ./usb-hid.sh usb2/3 stop

usb_dev_controller=${1}_hid
# current_path=`pwd`
export CONFIGFS_HOME=/etc/configfs
UDC=$CONFIGFS_HOME/usb_gadget/${usb_dev_controller}/UDC

function hid_stop()
{
    echo > ${UDC}
    find $CONFIGFS_HOME/usb_gadget/${usb_dev_controller} -type l | xargs -I {} rm {}
}

function hid_start()
{
    mkdir -p $CONFIGFS_HOME
    if [ ! -d "${CONFIGFS_HOME}/usb_gadget/" ]; then
            mount none $CONFIGFS_HOME -t configfs
    fi

    mkdir -p $CONFIGFS_HOME/usb_gadget/${usb_dev_controller}
    cd $CONFIGFS_HOME/usb_gadget/${usb_dev_controller}

    echo 0x32c9 > idVendor
    echo 0x2007 > idProduct

    mkdir -p strings/0x409 -m 0770
    echo "axera-ax650" > strings/0x409/serialnumber
    echo "axera" > strings/0x409/manufacturer
    echo "ax650-hid" > strings/0x409/product

    mkdir -p configs/c.1 -m 0770
    mkdir -p configs/c.1/strings/0x409 -m 0770
    echo "hid" > configs/c.1/strings/0x409/configuration
    echo 120 > configs/c.1/MaxPower

    mkdir -p functions/hid.usb0
    # Keyboard
    echo 1 > functions/hid.usb0/protocol
    echo 1 > functions/hid.usb0/subclass
    echo 8 > functions/hid.usb0/report_length
    echo -ne \\x05\\x01\\x09\\x06\\xa1\\x01\\x05\\x07\\x19\\xe0\\x29\\xe7\\x15\\x00\\x25\\x01\\x75\\x01\\x95\\x08\\x81\\x02\\x95\\x01\\x75\\x08\\x81\\x03\\x95\\x05\\x75\\x01\\x05\\x08\\x19\\x01\\x29\\x05\\x91\\x02\\x95\\x01\\x75\\x03\\x91\\x03\\x95\\x06\\x75\\x08\\x15\\x00\\x25\\x65\\x05\\x07\\x19\\x00\\x29\\x65\\x81\\x00\\xc0 > functions/hid.usb0/report_desc

    ln -s functions/hid.usb0 configs/c.1

    echo "enable usb hid gadget"
    if [ ${1} = "usb3" ]; then
            echo 30c00000.dwc3 > ${UDC}
    fi

    if [ ${1} = "usb2" ]; then
            echo 28100000.dwc3 > ${UDC}
    fi
}

if [ $# != 2 ]; then
	echo "usage:"
	echo "usb-hid.sh usb2/3 start"
	echo "usb-hid.sh usb2/3 stop"
	exit 0
fi

if [ $2 == "stop" ]; then
        if [ -f ${UDC} ]; then
            read line < ${UDC}
            if [ -z "${line}" ]; then
                    echo "usb hid has stopped" 	# so we don't need to stop it again
                    exit 0
            fi
        else
            echo "usb hid didn't start"
            exit 0
        fi

        echo "usb hid stop"
        hid_stop
        exit 0
fi

if [ $2 == "start" ]; then
    if [ -f ${UDC} ]; then
            read line < ${UDC}
            if [ -n "${line}" ]; then
                    echo "usb hid has enabled" 	# so we don't need to enable it again
                    exit 0
            fi
    fi
    echo "usb hid start"
    hid_start $1
    exit 0
fi

# cd ${current_path}
