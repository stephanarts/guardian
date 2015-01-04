
plugindir = \
    $(libdir)/guardian

plugin_LTLIBRARIES = \
    system-plugin.la

system_plugin_la_SOURCES = \
    system-plugin.c

system_plugin_la_LDFLAGS =       \
    -avoid-version       \
    -module              \
    $(PLATFORM_LDFLAGS)

system_plugin_la_CFLAGS =   \
    $(PCRE_CFLAGS) \
    -std=c99


AM_CPPFLAGS = \
    -I$(top_srcdir)
