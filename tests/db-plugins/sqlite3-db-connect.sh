
CONFIG_FILE=sqlite3-db-connect.conf

./plugin-loader --db-connect \
                --config=$srcdir/$CONFIG_FILE \
                sqlite3-db-plugin
RET=$?

exit $RET
