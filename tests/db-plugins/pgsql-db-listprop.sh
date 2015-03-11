
CONFIG_FILE=pgsql-db-listprop.conf

./plugin-loader --db-listprop \
                --config=$srcdir/$CONFIG_FILE \
                pgsql-db-plugin
RET=$?

exit $RET
