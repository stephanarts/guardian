#/bin/sh

PLUGINDIR="../../plugins"
PLUGINSUBDIR=".libs"

TEMP=`mktemp /tmp/guardian-tst.XXXXXX`

nm -g $PLUGINDIR/sqlite3-db-plugin/$PLUGINSUBDIR/sqlite3-db-plugin.so \
| awk '{ print $3 }' | sed '/^$/d' > $TEMP
    
diff -u $TEMP $SOURCE_DIR/tests/db-plugins/sqlite3-db-plugin.symbols
RET=$?

exit $RET
