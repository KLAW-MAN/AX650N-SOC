#!/bin/bash
cur_path=$(cd "$(dirname $0)";pwd)
cd $cur_path

process="axcl_demo"
pid=$(pidof ${process})
if [ $pid ]; then
  echo "${process} is already running, please check the process(pid: $pid) first."
  exit 1
fi

# ----------------------------------------------------------------------------------------------#
#
#  launching
#
# ----------------------------------------------------------------------------------------------#

md5=`md5sum ${process} | awk '{ print $1 }'`
echo "launching ${process}, md5: ${md5} ..."
./axcl_demo $*