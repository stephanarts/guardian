bin_PROGRAMS = guardiand

guardiand_SOURCES = \
    guardian.c \
    settings.c settings.h \
    scheduler.c scheduler.h \
    util.c util.h

guardiand_LDADD = \
    $(top_builddir)/libguardian/libguardian-$(LIBGUARDIAN_VERSION_API).la \
    $(PCRE_LIBS) \
    $(OPENSSL_LIBS) \
    $(PTHREAD_LIBS) -lrt

guardiand_LDFLAGS = \
    $(PLATFORM_LDFLAGS)

guardiand_CFLAGS = \
    $(PCRE_CFLAGS) \
    $(OPENSSL_CFLAGS) \
    $(PTHREAD_CFLAGS) \
    -DSYSCONFDIR=\"$(sysconfdir)\" \
    -DPLUGINDIR=\"$(libdir)/guardian\" \
    -Wall

guardiand_DEPENDENCIES = \
    $(top_builddir)/libguardian/libguardian-$(LIBGUARDIAN_VERSION_API).la

INCLUDES = \
    -I$(top_srcdir)
