
CONFIG_FILE=$SOURCE_DIR/tests/db-plugins/pgsql-db-listprop.conf

./plugin-loader --db-listprop \
                --config=$CONFIG_FILE \
                pgsql-db-plugin
RET=$?

exit $RET
