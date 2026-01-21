#!/bin/sh

export LD_LIBRARY_PATH=/opt/swupdate/

rootfs=`swupdate -g`

echo "rootfs = $rootfs"
if [ $rootfs == '/dev/mtdblock7' ];then
	selection="-e stable,copy2"
else
	selection="-e stable,copy1"
fi

#SWUPDATE_ARGS="-H beaglebone:1.0 ${selection} -f /etc/swupdate.cfg"

swupdate ${selection} -f /etc/swupdate.cfg -i /tmp/update.swu -v
