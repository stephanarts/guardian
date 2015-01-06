EXTRA_DIST = $(TESTS)

TESTS = \
    style.test

XFAIL_TESTS =

check_PROGRAMS = \
    libguardian-helper

libguardian_helper_LDADD = \
    $(top_builddir)/libguardian/libguardian-$(LIBGUARDIAN_VERSION_API).la \
    $(PCRE_LIBS)

AM_CPPFLAGS = \
    -I$(top_srcdir)
