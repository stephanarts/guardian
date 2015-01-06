/*-
 * Copyright (c) 2012 Stephan Arts. All Rights Reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY [LICENSOR] "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE,DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdlib.h>

#include <stdarg.h>

#ifdef HAVE_SYS_SYSLOG_H
#include <sys/syslog.h>
#endif

#include "assert.h"

static int syslog_opened = 0;

void
guardian_log_init (int perror)
{
    if (syslog_opened == 1)
    {
        guardian_assert_critical ("Syslog already initialised");
    }
    if (perror == 1)
    {
        openlog (PACKAGE_NAME, LOG_PID | LOG_PERROR, LOG_DAEMON);
    } else
    {
        openlog (PACKAGE_NAME, LOG_PID, LOG_DAEMON);
    }

    syslog_opened = 1;
}

void
guardian_log_mask (int mask)
{
    setlogmask (LOG_UPTO (mask));
}

void
guardian_log_alert (const char *format,...)
{
    va_list arg;

    va_start (arg, format);
    vsyslog (LOG_DAEMON | LOG_ALERT, format, arg);
    va_end (arg);

    return;
}

void
guardian_log_critical (const char *format,...)
{
    va_list arg;

    va_start (arg, format);
    vsyslog (LOG_DAEMON | LOG_CRIT, format, arg);
    va_end (arg);

    return;
}

void
guardian_log_error (const char *format,...)
{
    va_list arg;

    va_start (arg, format);
    vsyslog (LOG_DAEMON | LOG_ERR, format, arg);
    va_end (arg);

    return;
}

void
guardian_log_warning (const char *format,...)
{
    va_list arg;

    va_start (arg, format);
    vsyslog (LOG_DAEMON | LOG_WARNING, format, arg);
    va_end (arg);

    return;
}

void
guardian_log_notice (const char *format,...)
{
    va_list arg;

    va_start (arg, format);
    vsyslog (LOG_DAEMON | LOG_NOTICE, format, arg);
    va_end (arg);

    return;
}

void
guardian_log_info (const char *format,...)
{
    va_list arg;

    va_start (arg, format);
    vsyslog (LOG_DAEMON | LOG_INFO, format, arg);
    va_end (arg);

    return;
}

void
guardian_log_debug (const char *format,...)
{
    va_list arg;

    va_start (arg, format);
    vsyslog (LOG_DAEMON | LOG_DEBUG, format, arg);
    va_end (arg);

    return;
}
