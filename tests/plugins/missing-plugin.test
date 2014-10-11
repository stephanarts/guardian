#!/bin/sh

PLUGIN=missing
LINE="Jan 01 10:12:03 localhost kernel: [ 0000.000000] Initializing cgroup subsys cpu"

OUTPUT=`./plugin-loader $plugin < "$LINE" 2>&1`;
if [ "$?" -ne "0" ]; then
    echo "$OUTPUT" | sed -re 's/(.*)/pl> \1/g' > $0.log;
    exit 0;
else
    echo "$OUTPUT" | sed -re 's/(.*)/pl> \1/g' > $0.log;
    exit 1;
fi
