EXTRA_DIST = $(TESTS)

TESTS = \
    sqlite3-symbols.sh      \
    sqlite3-api.sh          \
    sqlite3-db-connect.sh   \
    pgsql-symbols.sh        \
    pgsql-api.sh            \
    pgsql-db-connect.sh

TEST_EXTENSIONS = .sh
SH_LOG_COMPILER = $(SH)
AM_TESTS_ENVIRONMENT = SOURCE_DIR='$(top_srcdir)'; export SOURCE_DIR;

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
