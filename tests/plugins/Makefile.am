EXTRA_DIST = $(TESTS)

TESTS = \
    missing-plugin.test \
    syslog.test

check_PROGRAMS = plugin-loader


plugin_loader_LDADD = \
    $(top_builddir)/libguardian/libguardian-$(LIBGUARDIAN_VERSION_API).la \
    $(PCRE_LIBS)

INCLUDES = \
    -I$(top_srcdir)
