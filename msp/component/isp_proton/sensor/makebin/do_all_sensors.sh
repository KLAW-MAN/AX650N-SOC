#!/bin/bash

LIST=`find .. -name '*.h' | grep custom`
for f in $LIST; do
    echo processing $f
    ./makebin $f;
    if [ $? -ne 0 ]; then
        echo "[makebin failed]:" $f
        exit 1
    fi
done
