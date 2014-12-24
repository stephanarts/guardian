
plugindir = \
    $(libdir)/guardian

plugin_LTLIBRARIES = \
    generator-plugin.la

generator_plugin_la_SOURCES = \
    generator-engine.c \
    generator-plugin.c

generator_plugin_la_LDFLAGS =       \
    -avoid-version       \
    -module              \
    $(PLATFORM_LDFLAGS)

generator_plugin_la_CFLAGS =   \
    $(PCRE_CFLAGS) \
    -std=c99


AM_CPPFLAGS = \
    -I$(top_srcdir)
