#!/bin/sh

MTD_DEV=/dev/mtd0
SPL_BIN=/soc/spl/spl_AX650_slave_signed.bin
SPL_RD=/soc/spl/spl_AX650_slave_signed_read_after_dd.bin

echo "step 1: erase $MTD_DEV"
flash_eraseall /dev/mtd0
sync
sleep 1

echo "step 2: Downloading $SPL_BIN to $MTD_DEV"
dd if=$SPL_BIN of=$MTD_DEV bs=4k count=64
echo "step 3: Synchronizing ..."
sync
sleep 1

echo "step 4: read $MTD_DEV to $SPL_RD"
dd if=$MTD_DEV bs=4k count=64  of=$SPL_RD  bs=4k count=64

echo "step 5: check download result"
ck1=`cksum $SPL_BIN | awk -F" " '{print $1}'`
ck2=`cksum $SPL_RD | awk -F" " '{print $1}'`
if [ $ck1 -eq $ck2 ]
then
echo "Download $SPL_BIN to $MTD_DEV success!!"
else
echo "Download $SPL_BIN to $MTD_DEV Failed!!"
exit 1;
fi

echo "OK"
