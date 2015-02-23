

./plugin-loader --db-connect \
                --config=pgsql-db-connect.conf \
                pgsql-db-plugin
RET=$?

exit $RET
