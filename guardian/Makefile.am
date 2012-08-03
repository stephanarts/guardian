bin_PROGRAMS = guardian

guardian_SOURCES = \
    guardian.c

guardian_LDADD = \
    $(PCRE_LIBS) \
    $(OPENSSL_LIBS) \
    $(PTHREAD_LIBS) -lrt

guardian_LDFLAGS = \
    $(PLATFORM_LDFLAGS)

guardian_CFLAGS = \
    $(PCRE_CFLAGS) \
    $(OPENSSL_CFLAGS) \
    $(PTHREAD_CFLAGS) \
    -DSYSCONFDIR=\"$(sysconfdir)\" \
    -DPLUGINDIR=\"$(libdir)/guardian\" \
    -Wall

INCLUDES = \
    -I$(top_srcdir)
