
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

httpd_la_CFLAGS =   \
    $(PCRE_CFLAGS) \
    -std=c99


AM_CPPFLAGS = \
    -I$(top_srcdir)
