bin_PROGRAMS = guardiand

guardiand_SOURCES = \
    guardian.c \
    settings.c settings.h \
    worker.c worker.h \
    scheduler.c scheduler.h \
    db.c db.h \
    util.c util.h

guardiand_LDADD = \
    $(top_builddir)/libguardian/libguardian-$(LIBGUARDIAN_VERSION_API).la \
    $(PCRE_LIBS) \
    $(OPENSSL_LIBS) \
    $(ZMQ_LIBS) \
    $(SQLITE3_LIBS) \
    $(PTHREAD_LIBS) -lrt

guardiand_LDFLAGS = \
    $(PLATFORM_LDFLAGS)

guardiand_CFLAGS = \
    $(PCRE_CFLAGS) \
    $(OPENSSL_CFLAGS) \
    $(ZMQ_CFLAGS) \
    $(SQLITE3_CFLAGS) \
    $(PTHREAD_CFLAGS) \
    -DSYSCONFDIR=\"$(sysconfdir)\" \
    -DPLUGINDIR=\"$(libdir)/guardian\" \
    -Wall

guardiand_DEPENDENCIES = \
    $(top_builddir)/libguardian/libguardian-$(LIBGUARDIAN_VERSION_API).la

AM_CPPFLAGS = \
    -I$(top_srcdir)
