
CONFIG_FILE=pgsql-db-connect.conf

./plugin-loader --db-connect \
                --config=$srcdir/$CONFIG_FILE \
                pgsql-db-plugin
RET=$?

exit $RET
