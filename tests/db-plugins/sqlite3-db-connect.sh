
CONFIG_FILE=$SOURCE_DIR/tests/db-plugins/sqlite3-db-connect.conf

./plugin-loader --db-connect \
                --config=$CONFIG_FILE \
                sqlite3-db-plugin
RET=$?

exit $RET
