EXTRA_DIST = $(TESTS)

TESTS = \
    file-verify.test      \
    file-read.test        \
    expression.test \
    timestamp-syslog.test \
    source-read.test

XFAIL_TESTS =

check_PROGRAMS = \
    file-helper \
    source-helper \
    timestamp-helper \
    expression-helper \
    libguardian-helper

file_helper_LDADD = \
    $(top_builddir)/libguardian/libguardian-$(LIBGUARDIAN_VERSION_API).la \
    $(PCRE_LIBS) \
    -lgrok

source_helper_LDADD = \
    $(top_builddir)/libguardian/libguardian-$(LIBGUARDIAN_VERSION_API).la \
    $(PCRE_LIBS) \
    -lgrok

timestamp_helper_LDADD = \
    $(top_builddir)/libguardian/libguardian-$(LIBGUARDIAN_VERSION_API).la \
    $(PCRE_LIBS) \
    -lgrok

expression_helper_LDADD = \
    $(top_builddir)/libguardian/libguardian-$(LIBGUARDIAN_VERSION_API).la \
    $(PCRE_LIBS) \
    -lgrok

libguardian_helper_LDADD = \
    $(top_builddir)/libguardian/libguardian-$(LIBGUARDIAN_VERSION_API).la \
    $(PCRE_LIBS)

AM_CPPFLAGS = \
    -I$(top_srcdir)
