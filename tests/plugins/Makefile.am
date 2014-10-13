EXTRA_DIST = $(TESTS)

TESTS = \
    missing-plugin.test \
    httpd.test \
    syslog.test

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
