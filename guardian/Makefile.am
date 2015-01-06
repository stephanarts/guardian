bin_PROGRAMS = guardian

guardian_SOURCES = \
    guardian.c \
    shell.c shell.h \
    interactive_menu.c interactive_menu.h \
    search_dialog.c search_dialog.h \
    timeframe_dialog.c timeframe_dialog.h

guardian_LDADD = \
    $(PCRE_LIBS) \
    $(OPENSSL_LIBS) \
    $(ZMQ_LIBS) \
    $(PTHREAD_LIBS) \
    -lncurses \
    -lrt \
    -ledit

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
