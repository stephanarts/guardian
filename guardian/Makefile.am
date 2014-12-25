bin_PROGRAMS = guardian

guardian_SOURCES = \
    guardian.c \
    interactive_menu.c \
    search_dialog.c \
    timeframe_dialog.c

guardian_LDADD = \
    $(PCRE_LIBS) \
    $(OPENSSL_LIBS) \
    $(ZMQ_LIBS) \
    $(PTHREAD_LIBS) \
    -lncurses \
    -lrt

guardian_LDFLAGS = \
    $(PLATFORM_LDFLAGS)

guardian_CFLAGS = \
    $(PCRE_CFLAGS) \
    $(OPENSSL_CFLAGS) \
    $(ZMQ_CFLAGS) \
    $(PTHREAD_CFLAGS) \
    -DSYSCONFDIR=\"$(sysconfdir)\" \
    -DPLUGINDIR=\"$(libdir)/guardian\" \
    -Wall

AM_CPPFLAGS = \
    -I$(top_srcdir)
