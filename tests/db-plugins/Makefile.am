EXTRA_DIST = \
    $(TESTS) \
    $(TESTSDATA)

TESTSDATA = \
    $(SQLITE_TESTDATA) \
    $(PGSQL_TESTDATA)


TESTS = \
    $(SQLITE_TESTS) \
    $(PGSQL_TESTS)

SQLITE_TESTS = \
    sqlite3-symbols.sh      \
    sqlite3-api.sh          \
    sqlite3-db-listprop.sh  \
    sqlite3-db-connect.sh

SQLITE_TESTDATA = \
    sqlite3-db-connect.conf  \
    sqlite3-db-listprop.conf \
    sqlite3-db-plugin.symbols

PGSQL_TESTS = \
    pgsql-symbols.sh        \
    pgsql-api.sh            \
    pgsql-db-listprop.sh  \
    pgsql-db-connect.sh

PGSQL_TESTDATA = \
    pgsql-db-connect.conf  \
    pgsql-db-listprop.conf \
    pgsql-db-plugin.symbols


TEST_EXTENSIONS = .sh
SH_LOG_COMPILER = $(SH)

XFAIL_TESTS = \
    pgsql-db-connect.sh

check_PROGRAMS = plugin-loader

plugin_loader_LDADD = \
    $(top_builddir)/libguardian/libguardian-$(LIBGUARDIAN_VERSION_API).la \
    $(ZMQ_LIBS) \
    $(PCRE_LIBS)

plugin_loader_CFLAGS = \
    $(PCRE_CFLAGS) \
    $(ZMQ_CFLAGS) \
    -Wall

AM_CPPFLAGS = \
    -I$(top_srcdir)
