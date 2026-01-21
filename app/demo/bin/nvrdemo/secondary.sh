#!/bin/sh
cur_path=$(cd "$(dirname $0)";pwd)

process=nvrdemo

# pid=$(pidof ${process})
# if [ $pid ]; then
#   echo "${process} is already running, please check the process(pid: $pid) first."
#   exit 1;
# fi

if [ -e /tmp/core* ]; then
  echo "exist coredump file under path: /tmp, please deal with coredump file first."
  exit 1;
fi

#set -e
cd $cur_path

export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/opt/lib/qt-5.15.7/lib
export QT_QPA_PLATFORM_PLUGIN_PATH=/opt/lib/qt-5.15.7/plugins
export QT_QPA_FONTDIR=/opt/lib/qt-5.15.7/fonts
export QT_QPA_PLATFORM=linuxfb:fb=/dev/fb2:size=1920x1080

#export QT_QPA_GENERIC_PLUGINS=evdevmouse:/dev/input/event1
#export QT_QPA_EVDEV_MOUSE_PARAMETERS=/dev/input/event1
#export QT_QPA_FB_HIDECURSOR=0

#export XDG_RUNTIME_DIR=/usr/lib
export RUNLEVEL=3

#fbset -fb /dev/fb0 -xres 1920 -yres 1080 -vxres 1920 -vyres 1080 -depth 24


# Enable core dump
# Check whether config coredump path (Only "-q 0" to disable config)
if [[ $(expr match "$*" ".*-q\s0.*") != 0 ]]
then
  EnableCoreDump=0
else
  EnableCoreDump=1
fi

# Run in background
RunInBackground=0

# Check whether start with gdb debug mode
if [[ $(expr match "$*" ".*-d.*") != 0 ]]
then
  debug="gdb --args"
else
  debug=""
fi


# Open core dump
if [ $EnableCoreDump == 1 ] ; then
  ulimit -c unlimited
  echo /opt/data/core-%e-%p-%t > /proc/sys/kernel/core_pattern
  echo "enable core dump ..."
fi

md5=`md5sum ${process} | awk '{ print $1 }'`
echo "lanuching ${process}, md5: ${md5} ..."

# launch
if [ $RunInBackground == 1 ] ; then
  nohup $debug ./${process} "$*" &
else
  $debug ./${process} $*
fi