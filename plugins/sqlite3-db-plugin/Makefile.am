
plugindir = \
    $(libdir)/guardian

plugin_LTLIBRARIES = \
    sqlite3-db-plugin.la

sqlite3_db_plugin_la_SOURCES = \
    plugin.c plugin.h \
    db.c db.h \
    host.c host.h \
    ns.c ns.h \
    metric.c metric.h \
    perm.c perm.h \
    value.c value.h

sqlite3_db_plugin_la_LDFLAGS =       \
    -avoid-version                   \
    -export-symbols-regex "^[^_].*"  \
    -module                          \
    $(PLATFORM_LDFLAGS)

sqlite3_db_plugin_la_LIBADD = \
    $(SQLITE3_LIBS)

sqlite3_db_plugin_la_CFLAGS =   \
    $(PCRE_CFLAGS) \
    $(SQLITE3_CFLAGS) \
    -DSCHEMADIR=\"$(datadir)/guardian/schema\" \
    -std=c99

AM_CPPFLAGS = \
    -I$(top_srcdir)
