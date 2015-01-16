#!/bin/sh

PLUGIN=missing

OUTPUT=`./plugin-loader $PLUGIN 2>&1`;
if [ "$?" -ne "0" ]; then
    echo "$OUTPUT"
    exit 0;
else
    exit 1;
fi
