#/bin/sh

PLUGINDIR="../../plugins"
PLUGINSUBDIR=".libs"

TEMP=`mktemp /tmp/guardian-tst.XXXXXX`

nm -g $PLUGINDIR/pgsql-db-plugin/$PLUGINSUBDIR/pgsql-db-plugin.so \
| awk '{ print $3 }' | sed '/^$/d' > $TEMP
    
diff -u $TEMP $SOURCE_DIR/tests/db-plugins/pgsql-db-plugin.symbols
RET=$?

unlink $TEMP

exit $RET
