

./plugin-loader --db-connect \
                --config=sqlite3-db-connect.conf \
                sqlite3-db-plugin
RET=$?

exit $RET
