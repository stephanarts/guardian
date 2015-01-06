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
#include "log.h"

#define MAX_FORMATS 50
#define MAX_FORMAT_LENGTH 20

static long n_formats = 0;

static char formats[MAX_FORMATS][MAX_FORMAT_LENGTH];
static char format_names[MAX_FORMATS][MAX_FORMAT_LENGTH];
static pcre *format_exp[MAX_FORMATS];
static pcre_extra *format_exp_extra[MAX_FORMATS];

/**
 * Build a regexp that can be used to find a string
 * matching the strftime() format in a buffer.
 */
static int
_guardian_timestamp_build_regexp (
        const char *format,
        pcre ** regexp);

/**
 * Register a Timestamp format, the timestamp code
 * uses strptime() internally, so a timestamp format
 * must be written according to the format defined by
 * strftime().
 */
int
guardian_register_timestamp (
        const char *format,
        const char *name)
{
    const char *err = NULL;

    if (n_formats < MAX_FORMATS)
    {

        /*
         * Build a regexp capable of finding this timestamp in a string.
         */

        _guardian_timestamp_build_regexp (format, &format_exp[n_formats]);
        if (format_exp[n_formats])
        {
            format_exp_extra[n_formats] =
                    pcre_study (format_exp[n_formats], 0, &err);
            if (err)
            {
                guardian_log_error ("Study Failed: %s\n", err);
            }
        }
        strncpy (formats[n_formats], format, MAX_FORMAT_LENGTH);
        strncpy (format_names[n_formats], name, MAX_FORMAT_LENGTH);
        n_formats++;
        return n_formats - 1;
    }
    return -1;
}

/**
 * Build a regexp that can be used to find a string
 * matching the strftime() format in a buffer.
 *
 * TODO:
 *  - Properly check buffer boundaries.
 *  - Add support for more strftime parameters.
 */
static int
_guardian_timestamp_build_regexp (
        const char *format,
        pcre ** regexp)
{

    const char *errors = NULL;
    int     err_offset;
    char    buffer[200];

    int     i = 0,
            a = 1;
    buffer[0] = '(';
    //memcpy (&buffer[0], "(?<TIMESTAMP>", 13);
    //a = 13;

    for (i = 0; format[i] != '\0'; ++i)
    {

        /*
         * Lazy, most we can write to buffer in one loop is 17 bytes. If it
         * could possibly get close to that, we break from the loop.
         */
        if (a > 175)
        {

            /*
             * Could not build regexp, does not fit buffer.
             */
            return 1;
        }
        switch (format[i])
        {
        case '%':
            switch (format[i + 1])
            {
            case 'T':
                memcpy (&buffer[a], "\\d{2}:\\d{2}:\\d{2}", 17);
                a += 17;
                break;
            case 'e':
                memcpy (&buffer[a], "\\d{1,2}", 7);
                a += 7;
                break;
            case 'm':
            case 'd':
                memcpy (&buffer[a], "\\d{2}", 5);
                a += 5;
                break;
            case 'b':
                memcpy (&buffer[a], "\\D{3}", 5);
                a += 5;
                break;
            case 'Y':
                memcpy (&buffer[a], "\\d{4}", 5);
                a += 5;
                break;
            case 'z':
                memcpy (&buffer[a], "[\\+-]\\d{4}", 10);
                a += 10;
                break;
            }
            i++;
            break;
        default:
            buffer[a] = format[i];
            a++;
            break;
        }
    }
    buffer[a] = ')';
    buffer[a + 1] = '\0';

    *regexp = pcre_compile (
            buffer,
            PCRE_FIRSTLINE |
            PCRE_MULTILINE |
            PCRE_NEWLINE_ANYCRLF |
            PCRE_UTF8,
            &errors,
            &err_offset,
            NULL);
    if (errors)
    {
        fprintf (stderr, "Compile regexp failed\n%s\n", errors);
    }
    //fprintf (stderr, "%s\n", buffer);

    return 0;
}

int
guardian_timestamp_init (void)
{
    /* Standard Syslog Timestamp format */
    /* Nov 20 23:12:14 */
    guardian_register_timestamp (
            "%b %e %T",
            "TIMESTAMP_SYSLOG");

    /* 2005/11/20 23:12:14 */
    guardian_register_timestamp (
            "%Y/%m/%d %T",
            "TIMESTAMP_NGINX");

    /* 127.0.0.1 - - [01/Nov/2014:08:05:10 +0100] */
    guardian_register_timestamp (
            "%d/%b/%Y:%T %z",
            "TIMESTAMP_ISO.");

    /* 127.0.0.1 - - [01/Nov/2014T08:05:10 +0100] */
    guardian_register_timestamp (
            "%d/%b/%YT%T %z",
            "TIMESTAMP_ISO...");

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
        size_t len,
        int hint,
        struct tm *timeptr)
{
    int     i = 0;
    int     ret = 0;
    int     offsets[18];
    const char *timestamp;

    if (hint == -1)
    {
        for (i = 0; i < n_formats; ++i)
        {

            timestamp = buffer;

            if (format_exp[i] != NULL)
            {
                fprintf (stderr, "Executing regexp\n");
                ret = pcre_exec (
                        format_exp[i],
                        format_exp_extra[i],
                        buffer,
                        len,
                        0,
                        PCRE_NOTEMPTY,
                        offsets,
                        18);

                if (ret >= 0)
                {
                    pcre_get_substring (
                            buffer,
                            offsets,
                            ret,
                            1,
                            &timestamp);
                    fprintf (stderr, "Get substring\n");
                } else
                {
                    fprintf (stderr, "regexp failed\n");
                }
            }
            fprintf (stderr, "}%s\n", timestamp);
            if (strptime (timestamp, formats[i], timeptr))
            {
                return 0;
            }
        }
    } else
    {
        timestamp = buffer;

        if (format_exp[hint] != NULL)
        {
            ret = pcre_exec (
                    format_exp[hint],
                    format_exp_extra[hint],
                    buffer,
                    len,
                    0,
                    PCRE_NOTEMPTY,
                    offsets,
                    18);

            if (ret >= 0)
            {
                pcre_get_substring (
                        buffer,
                        offsets,
                        ret,
                        1,
                        &timestamp);
                fprintf (stderr, "Get substring\n");
            }
        }
        if (strptime (timestamp, formats[hint], timeptr))
        {
            return 0;
        }
    }
    return 1;
}
