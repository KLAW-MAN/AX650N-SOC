#!/bin/sh

cur_path=$(cd "$(dirname $0)";pwd)
cd $cur_path

process="aicard_slave"
pid=$(pidof ${process})
if [ $pid ]; then
  echo "${process} is already running, please check the process(pid: $pid) first."
  exit 1
fi

# ----------------------------------------------------------------------------------------------#
#
# settings
#
# ----------------------------------------------------------------------------------------------#

# Check whether config coredump path (Only "-q 0" to disable config)
if [[ $(expr match "$*" ".*-q 0.*") != 0 ]]
then
  core_dump=0
else
  core_dump=1
fi

# set vdec log to error
export VDEC_LOG_LEVEL=3

# set sys log to error
export SYS_LOG_level=3

# 0: none
# 1: syslog
# 2: applog
# 4: stdout  (default)
export APP_LOG_TARGET=4

# 1: CRITICAL
# 2: ERROR
# 3: WARNING
# 4: NOTICE  (default)
# 5: INFORMATION
# 6: VERBOSE
# 7: DATA
export APP_LOG_LEVEL=3

# force to change decoded fps
# VDEC_GRPX_DECODED_FPS which X is VDEC group number started with 0
# for example:
#    set group 0 fps to 30.
# export VDEC_GRP0_DECODED_FPS=30

echo "rm all syslogs ..."
rm /opt/data/AXSyslog/syslog/*
mkdir -p /var/log/

# ----------------------------------------------------------------------------------------------#
#
# configuring
#
# ----------------------------------------------------------------------------------------------#
# enable core dump
if [ ${core_dump} == 1 ] ; then
    ulimit -c unlimited
    echo /var/log/core-%e-%p-%t > /proc/sys/kernel/core_pattern
    echo "enable core dump ..."
fi

# mount videos from axera nfs (Slave side use video files just for self validation)
mount_video=0
if [ ${mount_video} == 1 ] ; then
    mount_path="/opt/data/aicard_slv"
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
    mount -t nfs -o nolock 10.126.12.107:/home/mount/videos/mp4 ${mount_path}
    echo "mount 10.126.12.107:/home/mount/videos/mp4 success ..."
fi

# ----------------------------------------------------------------------------------------------#
#
#  launching
#
# ----------------------------------------------------------------------------------------------#

md5=`md5sum ${process} | awk '{ print $1 }'`
echo "launching ${process}, md5: ${md5} ..."
./aicard_slave
