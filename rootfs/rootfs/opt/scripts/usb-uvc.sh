#!/bin/sh
set -e

CONFIGFS="/etc/configfs"
GADGET="$CONFIGFS/usb_gadget"
VID="0x32C9"
PID="0x2001"
SERIAL="0123456789"
MANUF="Axera"
PRODUCT="Axera-Camera"
PACKET_SIZE=3072
PACKET_SIZE1=1024
PACKET_SIZE2=1024
PACKET_SIZE3=512
PACKET_SIZE4=512

BOARD=ax650-demo

USB2_NAME=usb2
USB3_NAME=usb3


if [ $# != 3 ]; then
	echo "usage:"
	echo "$0 start|stop usb2|usb3 1|2"
    echo "$0 start usb3 1"
    echo "$0 stop usb3 1"
	exit 0
fi

test_usb=${2}
uvc_num=${3}

# 0: dummy sensor, 1:os04a10, 2:os08a20
sensor_type=${4}

# 0: iso mode, 1: bulk mode.
bulk_mode=${5}

if [ "start" != ${1} -a "stop" != ${1} ]; then
    echo " command start or stop is supported"
    exit 1
fi

if [ "usb2" != ${test_usb} -a "usb3" != ${test_usb} ]; then
    echo "usb2 or usb3 is supported"
    exit 1
fi

if [ 4 -lt ${uvc_num} ]; then
    echo "uvc_num is out of range, 4 is the maximum"
    exit 1
fi

case $BOARD in
    "ax650-demo")
        UDC_USB2=28100000.dwc3
        UDC_USB3=30c00000.dwc3

        if [ ${test_usb} == ${USB2_NAME} ]; then
            UDC=$UDC_USB2
        elif [ ${test_usb} == ${USB3_NAME} ]; then
            UDC=$UDC_USB3
        else
            UDC=$UDC_USB2
        fi
        ;;
esac

echo "platform info:"
echo "  board : $BOARD"
echo "  udc   : $UDC"

UDC_EN=$GADGET/${test_usb}_uvc/UDC

create_frame() {
    # Example usage:
    # create_frame <function name> <width> <height> <format> <name>

    FUNCTION=$1
    WIDTH=$2
    HEIGHT=$3
    FORMAT=$4
    NAME=$5

    wdir=functions/$FUNCTION/streaming/$FORMAT/$NAME/${HEIGHT}p

    mkdir -p $wdir
    echo $WIDTH > $wdir/wWidth
    echo $HEIGHT > $wdir/wHeight

    if [ ${FORMAT} != "framebased" ];  then
    echo $(( $WIDTH * $HEIGHT * 2 )) > $wdir/dwMaxVideoFrameBufferSize
    fi

    hdir=functions/$FUNCTION/streaming/$FORMAT/$NAME
    if [ ${FORMAT} == "framebased" ];  then
        if [ ${NAME} == "fb1" ]; then
            echo -ne \\x48\\x32\\x36\\x34\\x00\\x00\\x10\\x00\\x80\\x00\\x00\\xaa\\x00\\x38\\x9b\\x71 > $hdir/guidFormat
        fi
        if [ ${NAME} == "fb2" ]; then
            echo -ne \\x48\\x32\\x36\\x35\\x00\\x00\\x10\\x00\\x80\\x00\\x00\\xaa\\x00\\x38\\x9b\\x71 > $hdir/guidFormat
        fi
    fi

    echo 400000 > $wdir/dwFrameInterval
    # configure FrameRate
	# dwFrameInterfal is in 100-ns units (fps = 1/(dwFrameInterval * 10000000))
    # 166666 -> 60 fps
	# 333333 -> 30 fps
	# 400000 -> 25 fps
	# 2000000 -> 5 fps
    #cat <<EOF > $wdir/dwFrameInterval
    #166666
    #333333
    #2000000
    #EOF
}

create_uvc() {
    # Example usage:
    #	create_uvc <target config> <function name>
    #	create_uvc config/c.1 uvc.0
    CONFIG=$1
    FUNCTION=$2
    MAXPACKETSIZE=$3
    INTERVAL=$4
    BURST=$5

    echo "	Creating UVC gadget function: $FUNCTION"
    mkdir -p functions/$FUNCTION

    # support os08a20 sensor resolution
    # the max resolution is 3840x2160
    # 360p, 480p, 540p, 720p, 1080p for YUY2 format
    create_frame $FUNCTION 1920 1080 uncompressed u
    create_frame $FUNCTION 1280 720 uncompressed u
    create_frame $FUNCTION 960  540 uncompressed u
    create_frame $FUNCTION 640  480 uncompressed u
    create_frame $FUNCTION 640  360 uncompressed u
    # 360p, 540p, 720p, 1080p, 4K for MJPG format
    create_frame $FUNCTION 3840 2160 mjpeg m
    create_frame $FUNCTION 1920 1080 mjpeg m
    create_frame $FUNCTION 1280 720 mjpeg m
    create_frame $FUNCTION 960  540 mjpeg m
    create_frame $FUNCTION 640  360 mjpeg m
    # 360p, 540p, 720p, 1080p, 4K for H264 format
    create_frame $FUNCTION 3840 2160 framebased fb1
    create_frame $FUNCTION 1920 1080 framebased fb1
    create_frame $FUNCTION 1280 720  framebased fb1
    create_frame $FUNCTION 960  540  framebased fb1
    create_frame $FUNCTION 640  360  framebased fb1
    # 360p, 540p, 720p, 1080p, 4K for H265 format
    create_frame $FUNCTION 3840 2160 framebased fb2
    create_frame $FUNCTION 1920 1080 framebased fb2
    create_frame $FUNCTION 1280 720  framebased fb2
    create_frame $FUNCTION 960  540  framebased fb2
    create_frame $FUNCTION 640  360  framebased fb2

    if [ ! -d functions/$FUNCTION/streaming/header/h ]; then
        mkdir -p functions/$FUNCTION/streaming/header/h
    fi
    cd functions/$FUNCTION/streaming/header/h

    ln -s ../../uncompressed/u
    ln -s ../../mjpeg/m
    ln -s ../../framebased/fb1
    ln -s ../../framebased/fb2

    cd ../../class/fs
    ln -s ../../header/h
    cd ../../class/hs
    ln -s ../../header/h
    cd ../../class/ss
    ln -s ../../header/h

    cd ../../../control
    if [ ! -d header/h ]; then
        mkdir -p header/h
    fi
    ln -s header/h class/fs
    ln -s header/h class/ss

    cd ../../../

    echo $MAXPACKETSIZE > functions/$FUNCTION/streaming_maxpacket
    if [ ${test_usb} == ${USB2_NAME} ]; then
        echo $INTERVAL > functions/$FUNCTION/streaming_interval
        echo $BURST > functions/$FUNCTION/streaming_maxburst
         # if the number of UVC Camera >=2,
         # then set mult to 0 since we need to leave enough bandwith for other UVC
        if [ ${uvc_num} -ge 2 ]; then
            echo 0 > functions/$FUNCTION/streaming_mult
        else
            echo 2 > functions/$FUNCTION/streaming_mult
        fi
    else
        echo $BURST > functions/$FUNCTION/streaming_maxburst
        echo $INTERVAL > functions/$FUNCTION/streaming_interval
        echo 2 > functions/$FUNCTION/streaming_mult
    fi

    echo 64 > functions/$FUNCTION/uvc_num_request
    mult=$(cat functions/$FUNCTION/streaming_mult)
    burst=$(cat functions/$FUNCTION/streaming_maxburst)
    interval=$(cat functions/$FUNCTION/streaming_interval)
    uvc_num_request=$(cat functions/$FUNCTION/uvc_num_request)
    echo "	$FUNCTION: maxpacketsize:$MAXPACKETSIZE, mult:$mult , burst:$burst, interval:$interval, uvc_num_request:$uvc_num_request "

    ln -s functions/$FUNCTION $CONFIG
}

function uvc_start()
{
    echo "Creating the USB UVC gadget"
    mkdir -p $CONFIGFS
    if [ ! -d "${CONFIGFS}/usb_gadget/" ]; then
            mount none $CONFIGFS -t configfs
    fi

    mkdir -p $GADGET/${test_usb}_uvc

    cd $GADGET/${test_usb}_uvc
    if [ $? -ne 0 ]; then
        echo "Error creating usb gadget in configfs"
        exit 1;
    fi

    echo "Setting Vendor and Product ID's"
    echo $VID > idVendor
    echo $PID > idProduct

    echo "Setting English strings"
    mkdir -p strings/0x409
    echo $SERIAL > strings/0x409/serialnumber
    echo $MANUF > strings/0x409/manufacturer
    echo $PRODUCT > strings/0x409/product

    echo "Creating Config"

    mkdir -p configs/c.1
    mkdir -p configs/c.1/strings/0x409

    echo "Creating UVC camera functions..."

    if [ ${uvc_num} == 1 ]; then # 1 UVC Camera
        create_uvc configs/c.1/f1 uvc.0 ${PACKET_SIZE} 1 15
    fi

    if [ ${uvc_num} == 2 ]; then  # 2 UVC Camera
        create_uvc configs/c.1/f1 uvc.0 ${PACKET_SIZE1} 2 12
        create_uvc configs/c.1/f2 uvc.1 ${PACKET_SIZE2} 2 12
    fi

    if [ ${uvc_num} == 3 ]; then  # 3 UVC Camera
        create_uvc configs/c.1/f1 uvc.0 ${PACKET_SIZE1} 2 12
        create_uvc configs/c.1/f2 uvc.1 ${PACKET_SIZE2} 2 12
        create_uvc configs/c.1/f3 uvc.2 ${PACKET_SIZE3} 2 12
    fi

    if [ ${uvc_num} == 4 ]; then  # 4 UVC Camera
        create_uvc configs/c.1/f1 uvc.0 ${PACKET_SIZE1} 2 12
        create_uvc configs/c.1/f2 uvc.1 ${PACKET_SIZE2} 2 12
        create_uvc configs/c.1/f3 uvc.2 ${PACKET_SIZE3} 2 12
        create_uvc configs/c.1/f4 uvc.3 ${PACKET_SIZE4} 2 12
    fi

    echo "Binding USB Device Controller"
    echo $UDC > $UDC_EN
    echo "OK"
}

function uvc_stop()
{
    echo "Stopping the USB gadget"

    set +e # Ignore all errors here on a best effort

    cd $GADGET/${test_usb}_uvc

    if [ $? -ne 0 ]; then
        echo "Error: no configfs gadget found"
        exit 1;
    fi

    echo "Unbinding USB Device Controller"
    grep $UDC UDC && echo "" > $UDC_EN

    echo "Removing gadget directory"
    cd /
    rm $GADGET/${test_usb}_uvc -rf  > /dev/null 2>&1
    rm $GADGET/${test_usb}_uvc -rf  > /dev/null 2>&1

    echo "OK"
}

case "$1" in
    start)
        if [ -f ${UDC_EN} ]; then
            read line < ${UDC_EN}
            if [ -n "${line}" ]; then
                    echo "usb uvc has enabled" 	# so we don't need to enable it again
                    exit 0
            fi
        fi
        echo "usb uvc start"
        uvc_start
        ;;

    stop)
        if [ -f ${UDC_EN} ]; then
            read line < ${UDC_EN}
            if [ -z "${line}" ]; then
                    echo "usb uvc has stopped" 	# so we don't need to stop it again
                    exit 0
            fi
        else
            echo "usb uvc didn't start"
            exit 0
        fi

        echo "usb uvc stop"
        uvc_stop
        ;;
    *)
        echo "Usage : $0 {start|stop}"
esac

