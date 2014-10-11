dnl
dnl Copyright (c) 2012
dnl
dnl Stephan Arts <stephan@xfce.org>
dnl

dnl ***************************
dnl *** Version information ***
dnl ***************************
m4_define([libguardian_verinfo], [1:0:0]) dnl current:revision:age
m4_define([libguardian_version_api], [1.0])
m4_define([guardian_version_major], [0])
m4_define([guardian_version_minor], [0])
m4_define([guardian_version_micro], [1])
m4_define([guardian_version_tag], [git]) # Leave empty for releases
m4_define([guardian_version], [guardian_version_major().guardian_version_minor().guardian_version_micro()ifelse(guardian_version_tag(), [], [], [-guardian_version_tag()])])

dnl *******************************************
dnl *** Debugging support for DEV snapshots ***
dnl *******************************************
m4_define([guardian_debug_default], [ifelse(guardian_version_tag(), [git], [full], [minimum])])

AC_COPYRIGHT([Copyright (c) 2014
           Stephan Arts <stephan@xfce.org>.])

AC_INIT([guardian], [guardian_version], [])
AC_PREREQ([2.50])

AM_INIT_AUTOMAKE
AM_CONFIG_HEADER([config.h])
AC_CONFIG_MACRO_DIR([m4])
AM_MAINTAINER_MODE

dnl check for UNIX variants
AC_AIX
AC_ISC_POSIX
AC_MINIX
AM_CONDITIONAL([HAVE_CYGWIN], [test "`uname | grep \"CYGWIN\"`" != ""])


dnl ********************************
dnl *** Check for basic programs ***
dnl ********************************
AC_PROG_CC
AC_PROG_LD

AC_PROG_INSTALL
AC_PROG_INTLTOOL([0.31], [no-xml])

AM_PROG_CC_C_O

dnl **************************
dnl *** Initialize libtool ***
dnl **************************
LT_PREREQ([2.2.6])
LT_INIT([disable-static])

LT_LIB_DLLOAD

dnl **************************************
dnl *** Substitute version information ***
dnl **************************************
LIBGUARDIAN_VERINFO=libguardian_verinfo()
AC_SUBST([LIBGUARDIAN_VERINFO])


dnl **************************
dnl *** Substitute version ***
dnl **************************
LIBGUARDIAN_VERSION=guardian_version()
LIBGUARDIAN_VERSION_API=libguardian_version_api()
LIBGUARDIAN_VERSION_MAJOR=guardian_version_major()
LIBGUARDIAN_VERSION_MINOR=guardian_version_minor()
LIBGUARDIAN_VERSION_MICRO=guardian_version_micro()
AC_SUBST([LIBGUARDIAN_VERSION])
AC_SUBST([LIBGUARDIAN_VERSION_API])
AC_SUBST([LIBGUARDIAN_VERSION_MAJOR])
AC_SUBST([LIBGUARDIAN_VERSION_MINOR])
AC_SUBST([LIBGUARDIAN_VERSION_MICRO])


dnl ***************************************
dnl *** Check for libraries and headers ***
dnl ***************************************
AC_HEADER_STDC
AC_CHECK_LIBM
AC_SUBST(LIBM)

dnl *** Check for stdio.h
dnl ***************************
AC_CHECK_HEADER(stdio.h,
    [ AC_DEFINE([HAVE_STDIO_H], [1],
                [Define to 1 if you have the <stdio.h> header file])
    ]
)

dnl *** Check for dirent.h
dnl ***************************
AC_CHECK_HEADER(dirent.h,
    [ AC_DEFINE([HAVE_DIRENT_H], [1],
                [Define to 1 if you have the <dirent.h> header file])
    ]
)

dnl *** Check for sys/select.h
dnl ***************************
AC_CHECK_HEADER(sys/select.h,
    [ AC_DEFINE([HAVE_SYS_SELECT_H], [1],
                [Define to 1 if you have the <sys/select.h> header file])
    ],
    [ AC_MSG_ERROR([Can't find the <sys/select.h> header file]) ]
)

dnl *** Check for sys/syslog.h
dnl ***************************
AC_CHECK_HEADER(sys/syslog.h,
    [ AC_DEFINE([HAVE_SYS_SYSLOG_H], [1],
                [Define to 1 if you have the <sys/syslog.h> header file])
    ]
)

dnl *** Check for pthread.h
dnl ***************************
AC_CHECK_HEADER(pthread.h,
    [ AC_DEFINE([HAVE_PTHREAD_H], [1],
                [Define to 1 if you have the <pthread.h> header file])
      AX_PTHREAD()
    ],
    [ AC_MSG_ERROR([Can't find the <pthread.h> header file]) ]
)

dnl *** Check for getopt.h
dnl ***************************
AC_CHECK_HEADER(getopt.h,
    [ AC_DEFINE([HAVE_GETOPT_H], [1],
                [Define to 1 if you have the <getopt.h> header file])
    ],
    [ AC_MSG_ERROR([Can't find header <getopt.h>]) ]
)

PKG_CHECK_MODULES([PCRE], [libpcre >= 8.0])
AC_SUBST(PCRE_LIBS)
AC_SUBST(PCRE_CFLAGS)

PKG_CHECK_MODULES([OPENSSL], [openssl >= 0.9.8])
AC_SUBST(OPENSSL_LIBS)
AC_SUBST(OPENSSL_CFLAGS)

PKG_CHECK_MODULES([ZMQ], [libzmq >= 3.2.0])
AC_SUBST(ZMQ_LIBS)
AC_SUBST(ZMQ_CFLAGS)

dnl *****************************
dnl *** Configuration options ***
dnl *****************************

AC_ARG_ENABLE(assert,
	[  --enable-assert         enable assertions],
	[ AC_DEFINE([ENABLE_ASSERT], [1],
	            [Define to 1 if you have enabled assertions])
	]
)

AC_OUTPUT([
Makefile
po/Makefile.in
libguardian/Makefile
guardiand/Makefile
guardian/Makefile
data/Makefile
plugins/Makefile
plugins/syslog/Makefile
plugins/httpd/Makefile
tests/Makefile
tests/libguardian/Makefile
tests/plugins/Makefile
docs/Makefile
docs/man/Makefile
docs/man/guardian.conf.5
docs/libguardian/Makefile
])

echo "----------------------------------------"
echo "$PACKAGE $VERSION"
echo ""
echo "The binary will be installed in $prefix/bin"
echo "----------------------------------------"
