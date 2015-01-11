#!/bin/sh

PLUGIN=system-plugin

./plugin-loader $PLUGIN
if [ "$?" -ne "0" ]; then
    exit 1;
else
    exit 0;
fi
