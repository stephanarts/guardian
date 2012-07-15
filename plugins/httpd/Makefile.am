
plugindir = \
    $(libdir)/guardian

plugin_LTLIBRARIES = \
    httpd.la

httpd_la_SOURCES = \
    httpd.c

httpd_la_LDFLAGS =       \
    -avoid-version       \
    -module              \
    $(PLATFORM_LDFLAGS)

INCLUDES = \
    -I$(top_srcdir)
