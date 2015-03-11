
CONFIG_FILE=$srcdir/sqlite3-db-listprop.conf

KEYS=`./plugin-loader --db-listprop sqlite3-db-plugin`
RET=$?
if [ $RET -ne 0 ]
then
    exit $RET
fi

DIFF=`echo "$KEYS" | diff -u $CONFIG_FILE -`
RET=$?

printf "%s\n" "$DIFF" >&2

exit $RET
