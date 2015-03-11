
CONFIG_FILE=sqlite3-db-connect.conf

cat $srcdir/../../data/schema/sqlite3.schema |\
    sqlite3 sqlite3-db-connect.db

./plugin-loader --db-connect \
                --config=$srcdir/$CONFIG_FILE \
                sqlite3-db-plugin
RET=$?

unlink sqlite3-db-connect.db

exit $RET
