
plugindir = \
    $(libdir)/guardian

plugin_LTLIBRARIES = \
    syslog-plugin.la

syslog_plugin_la_SOURCES = \
	syslog-file-engine.c \
	syslog-tcp-engine.c \
	syslog-udp-engine.c \
    syslog-plugin.c

syslog_plugin_la_LDFLAGS =   \
    -avoid-version       \
    -module              \
    -export-dynamic      \
    $(PLATFORM_LDFLAGS)

syslog_plugin_la_CFLAGS =   \
    $(PCRE_CFLAGS) \
    -std=c99

AM_CPPFLAGS = \
    -I$(top_srcdir)
