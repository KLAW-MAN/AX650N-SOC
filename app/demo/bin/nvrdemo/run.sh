#!/bin/sh
cur_path=$(cd "$(dirname $0)";pwd)

process=nvrdemo

pid=$(pidof ${process})
if [ $pid ]; then
  echo "${process} is already running, please check the process(pid: $pid) first."
  exit 1;
fi

if [ -e /tmp/core* ]; then
  echo "exist coredump file under path: /tmp, please deal with coredump file first."
  exit 1;
fi

if [[ $(expr match "$(date)" ".*1970.*") != 0 ]]; then
  echo "system time must be calibrated first, wait a moment and run again please..."
  exit 1;
fi

#set -e
cd $cur_path

export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/opt/lib/qt-5.15.7/lib
export QT_QPA_PLATFORM_PLUGIN_PATH=/opt/lib/qt-5.15.7/plugins
export QT_QPA_FONTDIR=/opt/lib/qt-5.15.7/fonts
export QT_QPA_PLATFORM=linuxfb:fb=/dev/fb0:size=1920x1080

# net
echo 1048576 > /proc/sys/net/core/rmem_max
echo  786432 > /proc/sys/net/core/rmem_default

#export QT_QPA_GENERIC_PLUGINS=evdevmouse:/dev/input/event1
#export QT_QPA_EVDEV_MOUSE_PARAMETERS=/dev/input/event1
export QT_QPA_FB_HIDECURSOR=0

#export XDG_RUNTIME_DIR=/usr/lib
export RUNLEVEL=3

#fbset -fb /dev/fb0 -xres 1920 -yres 1080 -vxres 1920 -vyres 1080 -depth 24


# Enable core dump
EnableCoreDump=1
# Run in background
RunInBackground=0
# 0: none
# 1: syslog
# 2: applog
# 4: stdout  (default)
export APP_LOG_TARGET=4

# Check whether start with gdb debug mode
if [[ $(expr match "$*" ".*-d.*") != 0 ]]
then
  debug="gdb --args"
else
  debug=""
fi

# Check whether config coredump path (Only "-q 0" to disable config)
if [[ $(expr match "$*" ".*-q\s0.*") != 0 ]]
then
  EnableCoreDump=0
else
  EnableCoreDump=1
fi

# Check whether test suite is enabled, and if so, mount the datastream file path for playback module
if [[ $(expr match "$*" ".*-u\s[1-9].*") != 0 ]]
then
  mount_video=1
else
  mount_video=0
fi

# mount datastream files from axera nfs
if [ ${mount_video} == 1 ] ; then
    mount_path="/opt/bin/NVRDemo/datastream_ts"
    if [ ! -d ${mount_path} ] ; then
        mkdir -p ${mount_path}
    else
        mountpoint -q ${mount_path}
        mount_flag=$?
        if [ ${mount_flag} == 0 ] ; then
            umount ${mount_path}
            echo "umount ${mount_path}"
        fi
    fi
    mount -t nfs -o nolock 10.126.12.107:/home/mount/videos/datastream_ts ${mount_path}
    echo "mount 10.126.12.107:/home/mount/videos/datastream_ts success ..."
fi

# rm syslog
echo "rm syslog"
rm /opt/data/AXSyslog/syslog/*

# Open core dump
if [ $EnableCoreDump == 1 ] ; then
  ulimit -c unlimited
  echo /opt/data/core-%e-%p-%t > /proc/sys/kernel/core_pattern
  echo "enable core dump ..."
fi

md5=`md5sum ${process} | awk '{ print $1 }'`
echo "launching ${process}, md5: ${md5} ..."

# launch
if [ $RunInBackground == 1 ] ; then
  nohup $debug ./${process} "$*" &
else
  $debug ./${process} $*
fi