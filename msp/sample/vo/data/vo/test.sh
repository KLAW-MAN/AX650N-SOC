#!/bin/sh

# ----------------------------------------------------------------------------------------------#
#
#  DUMP TEST
#
# ----------------------------------------------------------------------------------------------#
#
function vo_dump_test() {
    local layer_id=$1
    local chn_id=$2
    local mode=$3

    output=$(/opt/bin/dump_tools/ax_vo_dump -l $layer_id -c $chn_id -m $mode -n 1 -p $vdt_dir)
    #echo $output
}

if [ $# -lt 1 ] ; then
  echo "USAGE:"
  echo "        -------------------------------"
  echo '       |$1         |$2       |$3       |'
  echo "       |-----------|---------|---------|"
  echo "       |TEST-TYPE  |RESERVED |RESERVED |"
  echo "        ------------------------------"
  echo "       TEST-TYPE: DUMP"
  echo "       e.g.: $0 DUMP  -- VO DUMP TEST"
  exit 1
fi

# ----------------------------------------------------------------------------------------------#
#
#  TEST ENTRY
#
# ----------------------------------------------------------------------------------------------#
#
if [ $1 == "DUMP" ] ; then
  vdt_dir=/opt/data/vo-dump-test
  for i in 0 1 2
  do
    if [ -d $vdt_dir ]; then
      rm $vdt_dir -Rf
    fi

    mkdir $vdt_dir

    vo_dump_test 0 0 $i

    if [ $i -eq 0 ]; then
      if [ -f $vdt_dir/layer0_chn0*.nv12 ]; then
        echo "Dump layer0-chn0 success!!"
      else
        echo "Dump layer0-chn0 failed!!"
      fi
    elif [ $i -eq 1 ] ; then
      if [ -f $vdt_dir/layer0_*.nv12 ]; then
        echo "Dump layer0 out success!!"
      else
        echo "Dump layer0 out failed!!"
      fi
    elif [ $i -eq 2 ] ; then
      if [ -f $vdt_dir/layer0_chn0_*.nv12 ]; then
        echo "Dump layer0-chn0 success!!"
      else
        echo "Dump layer0-chn0 failed!!"
      fi

      rm $vdt_dir/layer0_chn0*.nv12

      if [ -f $vdt_dir/layer0_*.nv12 ]; then
        echo "Dump layer0 out success!!"
      else
        echo "Dump layer0 out failed!!"
      fi
    fi
  done

  rm $vdt_dir -Rf

else
  echo "TEST-TYPE:$3 unsupported"
fi
