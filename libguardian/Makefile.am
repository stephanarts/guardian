
lib_LTLIBRARIES = \
    libguardian-1.0.la

libguardian_includedir = \
    $(includedir)/guardian-$(LIBGUARDIAN_VERSION_API)/guardian

libguardian_1_0_la_SOURCES = \
    memory.c \
    assert.c \
    log.c \
    error.c \
    source.c \
    sourceengine.c \
    sourcetype.c \
    plugin.c \
    libguardian.c

libguardian_include_HEADERS = \
    memory.h \
    assert.h \
    log.h \
    error.h \
    sourceengine.h \
    sourcetype.h \
    source.h \
    plugin.h \
    libguardian.h

libguardian_1_0_la_CFLAGS =               \
    -DLIBGUARDIAN_COMPILATION

libguardian_1_0_la_LDFLAGS =              \
    -export-dynamic                       \
    -export-symbols-regex "^[^_].*"       \
    -version-info $(LIBGUARDIAN_VERINFO)  \
    $(PLATFORM_LDFLAGS)

libguardian_1_0_la_LIBADD = \
    -ldl
