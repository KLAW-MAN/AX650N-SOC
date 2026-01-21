#!/bin/sh

DOCKER_PATH=$1
OUT_ROOTFS=$2

echo "start docker pack...."
cp -rf $DOCKER_PATH/etc/init.d/* $OUT_ROOTFS/etc/init.d
cp -rf $DOCKER_PATH/etc/ssl $OUT_ROOTFS/etc/
cp -rf $DOCKER_PATH/etc/iptables.conf $OUT_ROOTFS/etc/
sed -i "/LD_LIBRARY_PATH=/a $(grep XTABLES_LIBDIR $DOCKER_PATH/etc/profile)" $OUT_ROOTFS/etc/profile

PATH_ROOTFS=$(grep -w PATH= $OUT_ROOTFS/etc/profile | awk -F "\"" '{ print $2}')
PATH_DOCKER=$(grep -w PATH= $DOCKER_PATH/etc/profile | awk -F "\"" '{ print $2}')
#PATH_NEW="PATH=""\""$PATH_ROOTFS:$PATH_DOCKER"\""
PATH_NEW="PATH=""\""$PATH_ROOTFS:$PATH_DOCKER"\""
echo $PATH_NEW 
#sed -i "/^PATH=/c $PATH_NEW/" $OUT_ROOTFS/etc/profile
sed -i "/^PATH=/c $PATH_NEW" $OUT_ROOTFS/etc/profile

PATH_ROOTFSLIB=$(grep -w "LD_LIBRARY_PATH" $OUT_ROOTFS/etc/profile | awk -F "\"" '{ print $2 }')
PATH_DOCKERLIB=$(grep -w "LD_LIBRARY_PATH" $DOCKER_PATH/etc/profile | awk -F "\"" '{ print $2 }')
#LIB_NEW="LD_LIBRARY_PATH=""\""$PATH_ROOTFSLIB:$PATH_DOCKERLIB"\""
PATH_LIB_NEW="LD_LIBRARY_PATH=""\""$PATH_ROOTFSLIB:$PATH_DOCKERLIB"\""
echo $PATH_LIB_NEW 
#sed -i "/^LD_LIBRARY_PATH=/c $PATH_LIB_NEW/" $OUT_ROOTFS/etc/profile
sed -i "/^LD_LIBRARY_PATH=/c $PATH_LIB_NEW" $OUT_ROOTFS/etc/profile
