
lib_LTLIBRARIES = \
    libguardian-1.0.la

libguardian_includedir = \
    $(includedir)/guardian-$(LIBGUARDIAN_VERSION_API)/guardian

libguardian_1_0_la_SOURCES = \
    memory.c \
    assert.c \
    file.c \
    log.c \
    error.c \
    source.c \
    sourceengine.c \
    sourcetype.c \
    timestamp.c \
    entry.c \
    field.c \
    plugin.c \
    libguardian.c

libguardian_include_HEADERS = \
    memory.h \
    assert.h \
    file.h \
    log.h \
    types.h \
    error.h \
    source.h \
    sourceengine.h \
    sourcetype.h \
    timestamp.h \
    entry.h \
    field.h \
    plugin.h \
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
