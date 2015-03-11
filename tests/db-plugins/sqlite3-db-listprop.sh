
CONFIG_FILE=sqlite3-db-listprop.conf

./plugin-loader --db-listprop \
                --config=$srcdir/$CONFIG_FILE \
                sqlite3-db-plugin
RET=$?

exit $RET
