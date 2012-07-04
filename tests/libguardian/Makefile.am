EXTRA_DIST = defs $(TESTS)

TESTS = \
    timezone.test

check_PROGRAMS = libguardian-helper


libguardian_helper_LDADD = \
    $(top_builddir)/libguardian/libguardian-$(LIBGUARDIAN_VERSION_API).la \
    $(PCRE_LIBS)
