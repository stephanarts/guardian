
CONFIG_FILE=$srcdir/pgsql-db-listprop.conf

KEYS=`./plugin-loader --db-listprop pgsql-db-plugin`
RET=$?
if [ $RET -ne 0 ]
then
    exit $RET
fi

DIFF=`echo "$KEYS" | diff -u $CONFIG_FILE -`
RET=$?

printf "%s\n" "$DIFF" >&2

exit $RET
