#!/bin/sh

if [ -z "$1" ]; then
  echo "Usage: $0 <number of streams> ./axcl_sample_transcode -i video.mp4 -d xx"
  exit 1
fi

# rm previous dump file
rm -f *.dump.pid*

export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/lib/axcl/ffmpeg

COUNT=$1

shift
COMMAND="./axcl_sample_transcode"

count=1
while [ $count -le $COUNT ]
do
  echo "Launch $COMMAND $@ $count"
  $COMMAND "$@" &
  count=$((count + 1))
done

wait
echo "All processes have finished."
