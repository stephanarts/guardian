/*-
 * Copyright (c) 2014 Stephan Arts. All Rights Reserved.
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

#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif

#ifdef HAVE_STDIO_H
#include <stdio.h>
#endif

#ifdef HAVE_STRING_H
#include <string.h>
#endif

#include <time.h>

#include <pcre.h>

#include "timestamp.h"

#define MAX_FORMATS 50
#define MAX_FORMAT_LENGTH 20

static long n_formats = 0;

static char formats[MAX_FORMATS][MAX_FORMAT_LENGTH];

/**
 * Register a Timestamp format, the timestamp code
 * uses strptime() internally, so a timestamp format
 * must be written according to the format defined by
 * strftime().
 */
int
guardian_register_timestamp (
    const char   *format)
{
    if (n_formats < MAX_FORMATS)
    {
        strncpy(formats[n_formats], format, MAX_FORMAT_LENGTH);
        n_formats++;
        return n_formats-1;
    }
    return -1;
}

int
guardian_timestamp_init(void)
{
    /* Standard Syslog Timestamp format */
    guardian_register_timestamp (
        "%b %e %T");

    return 0;
}

/**
 * guardian_extract_timestamp:
 *
 * Returns:
 *  0 on success;
 */
int
guardian_extract_timestamp (
    const char *buffer,
    int         hint,
    struct tm  *timeptr)
{
    if (strptime(buffer, formats[hint], timeptr)) {
        return 0;
    }
    return 1;
}
