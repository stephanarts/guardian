
CONFIG_FILE=$SOURCE_DIR/tests/db-plugins/pgsql-db-connect.conf

./plugin-loader --db-connect \
                --config=$CONFIG_FILE \
                pgsql-db-plugin
RET=$?

exit $RET
