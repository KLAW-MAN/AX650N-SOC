#!/bin/sh
board=$(cat /proc/ax_proc/chip_type)

funcProcess(){
    # wait 15 seconds untill demo run
    sleep 15

    # wait proc statitic
    cat /proc/ax_proc/vo/layer_status > /dev/null
    cat /proc/ax_proc/vo/layer_status > /dev/null
    cat /proc/ax_proc/vo/layer_status > /dev/null

    while :
    do
        ID=`ps -ef | grep axcl_demo | grep -v "grep" | awk '{print $1}'`
        if [ "ID" == "" ] ; then
            echo "axcl_demo process is exited"
            break
        fi

        cat /proc/ax_proc/vo/layer_status > /dev/null
        cat /proc/ax_proc/vo/layer_status > /dev/null
        cat /proc/ax_proc/vo/layer_status > /dev/null

        awk '/Chn CQDepth CQFill InFps  OutFps/ {flag=1; row=1; next} flag && NF {if (row <= 31) {if ($5 > 25) print "Row " row ": "$5" fps pass"; else print "Row " row ": "$5" fps fail"}; row++}' /proc/ax_proc/vo/layer_status

        sleep 5
    done
}

if [ "$board" == "AX650N_CHIP" ]; then
    echo "current board is ax650n"

    mkdir /opt/data/axcl_demo
    mkdir /opt/data/axcl_demo/videos
    cp /opt/bin/axcl/axcl_demo/res/bangkok_30952_1920x1080_30fps_gop60_4Mbps.mp4 /opt/data/axcl_demo/videos/
    sync

    cd /opt/bin/axcl/axcl_demo
    ./run.sh &
    funcProcess
else
    echo "current board is $board"
    echo "APP Started"
    echo "APP Exited"
fi
