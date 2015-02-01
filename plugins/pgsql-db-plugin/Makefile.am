
plugindir = \
    $(libdir)/guardian

plugin_LTLIBRARIES = \
    pgsql-db-plugin.la

pgsql_db_plugin_la_SOURCES = \
    pgsql-db-plugin.c

pgsql_db_plugin_la_LDFLAGS =       \
    -avoid-version       \
    -module              \
    $(PLATFORM_LDFLAGS)

pgsql_db_plugin_la_LIBADD = \
    $(POSTGRESQL_LIBS)

pgsql_db_plugin_la_CFLAGS =   \
    $(PCRE_CFLAGS) \
    $(POSTGRESQL_CFLAGS) \
    -std=c99

AM_CPPFLAGS = \
    -I$(top_srcdir)
