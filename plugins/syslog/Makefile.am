
plugindir = \
    $(libdir)/guardian

plugin_LTLIBRARIES = \
    syslog.la

syslog_la_SOURCES = \
    syslog.c

syslog_la_LDFLAGS =   \
    -avoid-version       \
    -module              \
    -export-dynamic      \
    $(PLATFORM_LDFLAGS)

syslog_la_CFLAGS =   \
    $(PCRE_CFLAGS) \
    -std=c99

AM_CPPFLAGS = \
    -I$(top_srcdir)
