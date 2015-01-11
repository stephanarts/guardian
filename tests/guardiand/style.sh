#!/bin/sh

TEMP_FILENAME=`mktemp`

CODE=$SOURCE_DIR/guardiand/*.c
HEADERS=$SOURCE_DIR/guardiand/*.h

INDENT_PARAMS='-bacc -bl -ta -bc -i4 -pcs -nut -bbb -di8 -nlp -ci8'
INDENT_PARAMS="$INDENT_PARAMS -TGuardianError"
INDENT_PARAMS="$INDENT_PARAMS -TGuardianPlugin"

RET=0


for f in $CODE
do
    indent $f $TEMP_FILENAME $INDENT_PARAMS
    diff -u $f $TEMP_FILENAME
    if test $? -ne 0
    then
        RET=1
    fi
done

for f in $HEADERS
do
    indent $f $TEMP_FILENAME $INDENT_PARAMS
    diff -u $f $TEMP_FILENAME
    if test $? -ne 0
    then
        RET=1
    fi
done

unlink $TEMP_FILENAME

exit $RET
