EXTRA_DIST = $(TESTS)

TESTS = \
    timezone.test    \
    file-verify.test \
    file-read.test   \
    source-read.test

XFAIL_TESTS =

check_PROGRAMS = \
    file-helper \
    source-helper \
    libguardian-helper

file_helper_LDADD = \
    $(top_builddir)/libguardian/libguardian-$(LIBGUARDIAN_VERSION_API).la \
    $(PCRE_LIBS)

source_helper_LDADD = \
    $(top_builddir)/libguardian/libguardian-$(LIBGUARDIAN_VERSION_API).la \
    $(PCRE_LIBS)

libguardian_helper_LDADD = \
    $(top_builddir)/libguardian/libguardian-$(LIBGUARDIAN_VERSION_API).la \
    $(PCRE_LIBS)

AM_CPPFLAGS = \
    -I$(top_srcdir)
