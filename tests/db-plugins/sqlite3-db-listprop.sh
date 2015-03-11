
CONFIG_FILE=$SOURCE_DIR/tests/db-plugins/sqlite3-db-listprop.conf

./plugin-loader --db-listprop \
                --config=$CONFIG_FILE \
                sqlite3-db-plugin
RET=$?

exit $RET
