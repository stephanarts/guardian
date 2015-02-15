#/bin/sh

PLUGINDIR="../../plugins"
PLUGINSUBDIR=".libs"

TEMP=`mktemp /tmp/guardian-tst.XXXXXX`

nm -g $PLUGINDIR/sqlite3-db-plugin/$PLUGINSUBDIR/sqlite3-db-plugin.so \
| awk '{ print $3 }' > $TEMP
    
diff -u $TEMP ./sqlite3-db-plugin.symbols
RET=$?

exit $RET
