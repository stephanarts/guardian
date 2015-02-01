EXTRA_DIST = $(TESTS)

TESTS = \
    style.sh

TEST_EXTENSIONS = .sh
SH_LOG_COMPILER = $(SH)
AM_TESTS_ENVIRONMENT = SOURCE_DIR='$(top_srcdir)'; export SOURCE_DIR;


XFAIL_TESTS = style.sh

check_PROGRAMS = \
    libguardian-helper

libguardian_helper_LDADD = \
    $(top_builddir)/libguardian/libguardian-$(LIBGUARDIAN_VERSION_API).la \
    $(PCRE_LIBS)

AM_CPPFLAGS = \
    -I$(top_srcdir)
