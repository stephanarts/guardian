
lib_LTLIBRARIES = \
    libguardian-1.0.la

libguardian_includedir = \
    $(includedir)/guardian-$(LIBGUARDIAN_VERSION_API)/guardian

libguardian_1_0_la_SOURCES = \
    memory.c \
    assert.c \
    item.c \
    value.c \
    log.c \
    error.c \
    plugin.c \
    plugin-db.c \
    libguardian.c

libguardian_include_HEADERS = \
    memory.h \
    assert.h \
    log.h \
    types.h \
    error.h \
    item.h \
    value.h \
    itemtype.h \
    plugin.h \
    plugin-db.h \
    libguardian.h

libguardian_1_0_la_CFLAGS =               \
    $(PTHREAD_CFLAGS) \
    $(PCRE_CFLAGS) \
    $(ZMQ_CFLAGS) \
    $(OPENSSL_CFLAGS) \
    -std=c99 \
    -DLIBGUARDIAN_COMPILATION

libguardian_1_0_la_LDFLAGS =              \
    -export-dynamic                       \
    -export-symbols-regex "^[^_].*"       \
    -version-info $(LIBGUARDIAN_VERINFO)  \
    $(PLATFORM_LDFLAGS)

libguardian_1_0_la_LIBADD = \
    $(OPENSSL_LIBS) \
    $(PCRE_LIBS) \
    $(ZMQ_LIBS) \
    $(PTHREAD_LIBS)

AM_CPPFLAGS = \
    -I$(top_srcdir)
