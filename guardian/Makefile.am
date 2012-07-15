bin_PROGRAMS = guardian

guardian_SOURCES = \
    guardian.c \
    settings.c settings.h \
    util.c util.h

guardian_LDADD = \
    $(top_builddir)/libguardian/libguardian-$(LIBGUARDIAN_VERSION_API).la \
    $(PCRE_LIBS) \
    $(OPENSSL_LIBS)

guardian_LDFLAGS = \
    $(PLATFORM_LDFLAGS)

guardian_CFLAGS = \
    $(PCRE_CFLAGS) \
    $(OPENSSL_CFLAGS) \
    -DSYSCONFDIR=\"$(sysconfdir)\" \
    -DPLUGINDIR=\"$(libdir)/guardian\" \
    -Wall

guardian_DEPENDENCIES = \
    $(top_builddir)/libguardian/libguardian-$(LIBGUARDIAN_VERSION_API).la

INCLUDES = \
    -I$(top_srcdir)
