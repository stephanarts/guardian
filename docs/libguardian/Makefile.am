EXTRA_DIST = libguardian.conf 

docdir = $(datadir)/docs/libguardian/

#doc_DATA = html/

if MAINTAINER_MODE

BUILT_SOURCES = html

html: libguardian.conf
	doxygen libguardian.conf

endif
