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

#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif

#ifdef HAVE_STDIO_H
#include <stdio.h>
#endif

#include <openssl/sha.h>

#include <string.h>

#include <pcre.h>

#include <errno.h>

#include <sys/stat.h>

#include <stdarg.h>

#include <libguardian/libguardian.h>

#define DATA_BUFFER_SIZE 1024
#define ISO_TIMESTAMP_MAX_LEN 20

/*** FIELD MATCHING REGULAR EXPRESSIONS ***/

/** TIMESTAMP regular expression */
#define TIMESTAMP_REGEX "(Jan|Feb|Mar|Apr|Jun|Jul|Aug|Sep|Oct|Nov|Dec)[ ]+([1-9]+)[ ]+([0-9]+)[:]([0-9]+)[:]([0-9]+)"

/** SYSLOG_TAG regular expression */
#define SYSLOG_TAG_REGEX ""

static pcre *pcre_date_context = NULL;

static unsigned int min_offset = 0;

GuardianSourcetype *syslog_type = NULL;
GuardianField      *timestamp_field = NULL;
GuardianField      *syslog_tag_field = NULL;

static int
_plugin_extract_timestamp (
        size_t len,
        const char *entry,
        char *timestamp);

static int
_syslog_tag_compare_func (
        const void *a,
        const void *b );

GuardianPlugin *
guardian_plugin_init (void)
{
    const char *errors = NULL;
    int err_offset;
    GuardianPlugin *plugin;

    guardian_log_info("Initialise syslog plugin");

    plugin = guardian_new (sizeof (GuardianPlugin), 1);

    pcre_date_context = pcre_compile (
            TIMESTAMP_REGEX,
            PCRE_FIRSTLINE |
            PCRE_MULTILINE |
            PCRE_NEWLINE_ANYCRLF |
            PCRE_UTF8,
            &errors,
            &err_offset,
            NULL);

    return plugin;
}

static int
_plugin_extract_timestamp ( size_t len, const char *entry, char *timestamp)
{
    int         offsets[18];
    int         ret;
    int         month_id = 0;
    int         day_id = 0;
    int         hour_id = 0;
    int         minute_id = 0;
    int         second_id = 0;
    const char *str;

    ret = pcre_exec (
            pcre_date_context,
            NULL,
            entry,
            len,
            0,
            PCRE_NOTEMPTY,
            offsets,
            18);
    if ( ret >= 0 )
    {
        if ( timestamp != NULL )
        {
            pcre_get_substring (
                entry,
                offsets,
                ret,
                1,
                &str);

            if (0 == strcmp (str, "Jan"))
            {
                month_id = 1;
            }
            if (0 == strcmp (str, "Feb"))
            {
                month_id = 2;
            }
            if (0 == strcmp (str, "Mar"))
            {
                month_id = 3;
            }
            if (0 == strcmp (str, "Apr"))
            {
                month_id = 4;
            }
            if (0 == strcmp (str, "May"))
            {
                month_id = 5;
            }
            if (0 == strcmp (str, "Jun"))
            {
                month_id = 6;
            }
            if (0 == strcmp (str, "Jul"))
            {
                month_id = 7;
            }
            if (0 == strcmp (str, "Aug"))
            {
                month_id = 8;
            }
            if (0 == strcmp (str, "Sep"))
            {
                month_id = 9;
            }
            if (0 == strcmp (str, "Okt"))
            {
                month_id = 10;
            }
            if (0 == strcmp (str, "Nov"))
            {
                month_id = 11;
            }
            if (0 == strcmp (str, "Dec"))
            {
                month_id = 12;
            }
            if (month_id > 0)
            {

                pcre_get_substring (
                    entry,
                    offsets,
                    ret,
                    2,
                    &str);
                day_id = strtol (str, NULL, 10);

                pcre_get_substring (
                    entry,
                    offsets,
                    ret,
                    3,
                    &str);
                hour_id = strtol (str, NULL, 10);

                pcre_get_substring (
                    entry,
                    offsets,
                    ret,
                    4,
                    &str);
                minute_id = strtol (str, NULL, 10);

                pcre_get_substring (
                    entry,
                    offsets,
                    ret,
                    5,
                    &str);
                second_id = strtol (str, NULL, 10);

                if (min_offset == 0)
                {
                    snprintf (timestamp, ISO_TIMESTAMP_MAX_LEN,
                            "XXXX%.2d%.2dT%.2d%.2d%.2dZ",
                            month_id, 
                            day_id,
                            hour_id,
                            minute_id,
                            second_id);
                }
                else
                {
                    if (min_offset > 0)
                    {
                        snprintf (timestamp, ISO_TIMESTAMP_MAX_LEN,
                                "XXXX%.2d%.2dT%.2d%.2d%.2d+%.2d%.2d",
                                month_id, 
                                day_id,
                                hour_id,
                                minute_id,
                                second_id,
                                min_offset/60,
                                min_offset%60);
                    }
                    else
                    {
                        snprintf (timestamp, ISO_TIMESTAMP_MAX_LEN,
                                "XXXX%.2d%.2dT%.2d%.2d%.2d-%.2d%.2d",
                                month_id, 
                                day_id,
                                hour_id,
                                minute_id,
                                second_id,
                                min_offset/60,
                                min_offset%60);
                    }
                }
            }
            else
            {
                /*
                 * ERROR: Unknown month, this should be already caught by the
                 * regular expression.
                 */
                return 1;
            }
        }
    }

    return 0;
}

static int
_syslog_tag_compare_func (
        const void *a,
        const void *b )
{
    return strcmp (a, b);
}
