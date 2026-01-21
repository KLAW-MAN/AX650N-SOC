#!/bin/sh
cur_path=$(cd "$(dirname $0)";pwd)

process=frtdemo

pid=$(pidof ${process})
if [ $pid ]; then
  echo "${process} is already running, please check the process(pid: $pid) first."
  exit 1;
fi

if [ -e /tmp/core* ]; then
  echo "exist coredump file under path: /tmp, please deal with coredump file first."
  exit 1;
fi

if [ $# -lt 2 ] ; then
  echo "USAGE: $0 -p <ppl> -s <sensor type> [-n <scenario>] [-l <log level>] [-t <log target>]"
  echo " e.g.: $0 -p 0 -s 0                  -- Start with ITS and dual os08a20"
  echo " e.g.: $0 -p 1 -s 0 -l 2 -t 2        -- Start with IPC and dual os08a20, log level set to ERROR, log target set to APP"
  echo " e.g.: $0 -p 2 -s 5 -n 0             -- Start with Pano dual os04a10 and select scenario 0"
  echo " ---------------------------------------------------------------------------------------------------------------------------"
  echo " Command details:"
  echo "   p: pipeline index, indicates ppl load type, like ITS or IPC or Pano etc."
  echo "      0: ITS pipeline "
  echo "      1: IPC pipeline"
  echo "      2: Pano pipeline"
  echo "   s: sensor type."
  echo "      0: DUAL OS08A20 "
  echo "      1: Single OS08A20"
  echo "      2: DUAL OS08B10"
  echo "      3: Single OS08B10"
  echo "      4: Single SC910gs"
  echo "      5: Pano Dual OS04A10"
  echo "      6: Single OS08A20 + Dual 0SO4A10(Pano)"
  echo "      7: Single OS04A10"
  echo "      8: Quad OS04A10"
  echo "      9: Dummy Quad OS04A10"
  echo "      10: Dummy Six OS04A10"
  echo "      11: Dummy Eight OS04A10"
  echo "      12: Dummy Quad OS08A20"
  echo "      13: Single SC410gs"
  echo "      14: Quad OS08A20"
  echo "      15: Quad SC500AI"
  echo "      16: Dummy Quad SC500AI"
  echo "      17: Quad IMX678"
  echo "      18: Dummy Quad IMX678"
  echo "      19: Pano OS04A10x8"
  echo "      20: Pano SC450AIx8"
  echo "   n: scenario, indicates the scenario to load, which is always defined in config files."
  echo "      ITS:"
  echo "          0: Dual sensor in T3DNR+T2DNR+T2DNR mode(default)"
  echo "          1: Dual sensor in T3DNR+T2DNR+AI2DNR mode"
  echo "          2: Single sensor in (AI3D_T2DNR<->T3DNR)+T2DNR+AI2DNR mode"
  echo "          3: Single sensor with flash mode with two pipes"
  echo "          4: Dual sensor in 4k@30fps AICE<->AINR SDR/HDR mode"
  echo "          5: Dual sensor in 4k@50fps AI3D_T2DNR<->T3DNR SDR mode"
  echo "          6: Single sensor in 4k@30fps AI3D_T2DNR<->T3DNR SDR mode"
  echo "          7: Single sensor in 4M@30fps DCG-HDR mode"
  echo "          8: Single sensor in 4M@30fps DCG-VS HDR mode"
  echo "          9: Single sensor with flash mode with three pipes"
  echo "          11: Single sensor for intelligent monitoring and recording system"
  echo "      IPC:"
  echo "          0: Single sensor(default)"
  echo "          1: Single sensor + Dual sensor(8+4+4)"
  echo "          2: Single sensor in board 8+4+4"
  echo "          3: Quad 4K/4M sensor"
  echo "          4: Eight sensor"
  echo "          5: Six sensor"
  echo "          6: Quad 5M sensor"
  echo "      Pano:"
  echo "          0: Dual sensor(default)"
  echo "          1: Dual sensor in board 8+4+4"
  echo "          2: Eight sensor"
  echo "   l: log level, indicates the log level."
  echo "      CRITICAL = 1, ERROR = 2 (DEFAULT), WARN = 3, NOTICE = 4, INFO = 5, DEBUG = 6, DATA = 7"
  echo "   t: log target, indicates the log output targets."
  echo "      SYSLOG = 1, APPLOG = 2, STDOUT = 4 (DEFAULT) (Calculate the sum if multiple targets is required)"
  echo "   d: GNU debugger."
  exit 1;
fi

#set -e
cd $cur_path

export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:./lib

# Enable core dump
# Check whether config coredump path (Only "-q 0" to disable config)
if [[ $(expr match "$*" ".*-q 0.*") != 0 ]]
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
echo "launching ${process}, md5: ${md5} ..."

while getopts "p:s:n:" opt
do
   case "$opt" in
      p ) p_value="$OPTARG" ;;
      s ) s_value="$OPTARG" ;;
      n ) n_value="$OPTARG" ;;
      ? ) echo "............: -$optopt" ;;
   esac
done

if [ $p_value == 0 -a $s_value == 1 -a $n_value == 6 ] ; then
    export SKEL_RESIZE_INTF_TYPE=TDP
fi

if [ $p_value == 2 -a $n_value == 2 ] ; then
    ax_lookat 0x0425023C -s 0x00010003
    ax_lookat 0x04250248 -s 0x00010003
fi

if [ $p_value == 2 -a $s_value == 20 ] ; then
    insmod /soc/ko/ax_vsync_ctrl.ko
fi

export AX_AVSCALI_IGNORE_INTEGRITY_DATA_CHECK=1

# launch
if [ $RunInBackground == 1 ] ; then
  nohup $debug ./${process} "$*" &
else
  $debug ./${process} $*
fi
