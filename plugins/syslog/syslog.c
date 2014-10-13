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

static void        *_ctx = NULL;

static void
_plugin_register_types ( GuardianPlugin *plugin );

static int
_plugin_engine_update_source (
        GuardianSourceEngine *engine,
        GuardianSource       *source );

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
guardian_plugin_init (void *ctx)
{
    const char *errors = NULL;
    int err_offset;
    GuardianPlugin *plugin;

    /* Can't initialise this plugin twice */
    if (_ctx != NULL) {
        return NULL;
    }

    plugin = guardian_new (sizeof (GuardianPlugin), 1);

    _ctx = ctx;
    plugin->register_types = _plugin_register_types;

    pcre_date_context = pcre_compile (
            TIMESTAMP_REGEX,
            PCRE_FIRSTLINE |
            PCRE_MULTILINE |
            PCRE_NEWLINE_ANYCRLF |
            PCRE_UTF8,
            &errors,
            &err_offset,
            NULL);

    guardian_get_timezone_gmt_offset (&min_offset);

    return plugin;
}

static void
_plugin_register_types ( GuardianPlugin *plugin )
{
    GuardianSourceEngine *engine;

    if (syslog_type == NULL)
    {
        engine = guardian_source_engine_new ( );

        engine->update_source = _plugin_engine_update_source;

        syslog_type = guardian_sourcetype_register (
                "syslog",
                "^(.*)$",
                engine );

        timestamp_field = guardian_field_lookup (
                "__timestamp");

        syslog_tag_field = guardian_field_register (
                "_syslog_tag",
                _syslog_tag_compare_func );

    }
    return;
}

static int 
_plugin_engine_update_source (
        GuardianSourceEngine *engine,
        GuardianSource       *source )
{
    int error_sv;

    FILE *f = NULL;
    int fd;
    struct stat buffer;

    SHA_CTX context;
    char file_hash[20];

    int i = 0;
    SHA_CTX cache_context;
    char cache_file_hash[20];
    const char *cache_hash;

    const char *path = guardian_source_get_path (source);
    size_t st_size   = guardian_source_get_size (source);
    const char *hash;
    guardian_source_get_hash (source, &hash);

    char data_buffer[DATA_BUFFER_SIZE];
    char *str_ptr = &data_buffer[0];
    char *end_ptr = &data_buffer[DATA_BUFFER_SIZE-1];
    char *ptr;
    size_t _size;
    size_t s_offset;

    GuardianEntry *g_entry = NULL;

    const char *entry;
    char timestamp[ISO_TIMESTAMP_MAX_LEN];

    size_t offset;
    size_t len;

    int    n_new_entries = 0;

    f = fopen (path, "r");
    if (f == NULL)
    {
        error_sv = errno;
        guardian_log_warning ("Can not open file: %s:'%s'", path, strerror (error_sv));
        return -1;
    }

    fd = fileno (f);
    
    fstat (fd, &buffer);

    /*
     * Logs are rotated... so st_stat can be smaller then source->st_stat
     *
     * TODO...
     */
    if (buffer.st_size > st_size || 1 == 1)
    {

        SHA1_Init (&context);

        /*
         * If we have already read from this file before,
         * verify it's contents.
         */
        if (st_size > 0)
        {
            SHA1_Init (&cache_context);

            while (i < st_size)
            {
                if (i+DATA_BUFFER_SIZE < st_size)
                {
                    _size = fread (data_buffer, 1, DATA_BUFFER_SIZE, f);
                }
                else
                {
                    _size = fread (data_buffer, 1, st_size - i, f);
                }
                /*
                 * Add to the 'cache' context for verification.
                 */
                SHA1_Update (&cache_context, data_buffer, _size);

                /*
                 * Add to the new context, for calculating the new hash on success.
                 */
                SHA1_Update (&context, data_buffer, _size);
                i += _size;
            }

            SHA1_Final ((char *)cache_file_hash, &cache_context);

            guardian_source_get_hash (
                    source,
                    &cache_hash );

            /**
             * Compare if the hash is the same. If it is not the same,
             * we are dealing with a different file, or the log-file is
             * compromised.
             */
            if (memcmp (cache_hash, cache_file_hash, 20) == 0)
            {
                /* DEBUG */
                //printf("OK: Hash matches\n");
            }
            else
            {
                /* DEBUG */
                //printf("FAIL: Hash does not match\n");
            }
        }

#if 0
        /*
         * Nice for performance improvement... but it does not help 
         * if you want to prevent tampering with the log-files.
         */
        if ( st_size > 0 )
        {
            /*
             * Move the file position indicator to the last known end of the file.
             */
            fseek (f, st_size, SEEK_SET);

        }
#endif

        /*
         * Fill the remaining part of the buffer (complete buffer on first try)
         */
        while ((_size = fread (str_ptr, 1, end_ptr-str_ptr, f)) > 0)
        {
            SHA1_Update (&context, str_ptr, _size);
            /*
             * If the read number of bytes is smaller then the complete buffer,
             * adjust the size indicator and break out of this loop (we do not
             * need to read from the file again.
             */
            if (str_ptr+_size < end_ptr)
            {
                _size = (str_ptr-&data_buffer[0])+_size;
                break;
            }

            /*
             * Get the first entry from the buffer.
             */
            if ( guardian_sourcetype_get_entry (
                    syslog_type,
                    data_buffer,
                    DATA_BUFFER_SIZE,
                    0,
                    &entry,
                    &offset,
                    &len,
                    NULL ) == 0 )
            {

                /*
                 * Jump the pointer over the new-line characters
                 */
                ptr = &data_buffer[len+offset+1];
                while (*ptr == '\n' || *ptr == '\r')
                    ptr++;

                /*
                 * Shift the buffer to the left, so we can 'fill' the end
                 * at the next read.
                 */
                str_ptr = &data_buffer[0];

                while (ptr < end_ptr)
                {
                    *str_ptr = *ptr;
                    str_ptr++;
                    ptr++;
                }

                g_entry = guardian_entry_new (len, entry, NULL);

                n_new_entries++;

                guardian_source_push_entry ( source, g_entry );

                _plugin_extract_timestamp (len, entry, timestamp);

                /*
                 * TODO: Calculate the correct year, and overwrite it in the timestamp.
                 *
                 * Using memcpy, because strncpy gives meaning to '\0' bytes
                 */
                memcpy (timestamp, "2012", 4);

                guardian_field_add_entry (
                        timestamp_field,
                        g_entry,
                        ISO_TIMESTAMP_MAX_LEN,
                        timestamp );
            }
            else
            {
                break;
            }
        }

        /*
         * Calculate the file_hash
         */
        SHA1_Final ((char *)file_hash, &context);

        s_offset = 0;

        while (s_offset < _size)
        {
            if ( guardian_sourcetype_get_entry (
                    syslog_type,
                    &data_buffer[s_offset],
                    _size-s_offset,
                    0,
                    &entry,
                    &offset,
                    &len,
                    NULL ) == 0 )
            {
                ptr = &data_buffer[len+offset+1];
                while (*ptr == '\n' || *ptr == '\r')
                    ptr++;

                s_offset+=len+offset+1;

                g_entry = guardian_entry_new (len, entry, NULL);

                n_new_entries++;

                guardian_source_push_entry ( source, g_entry );
                
                _plugin_extract_timestamp (len, entry, timestamp);

                /*
                 * TODO: Calculate the correct year, and overwrite it in the timestamp.
                 *
                 * Using memcpy, because strncpy gives meaning to '\0' bytes
                 */
                memcpy (timestamp, "2012", 4);

                guardian_field_add_entry (
                        timestamp_field,
                        g_entry,
                        ISO_TIMESTAMP_MAX_LEN,
                        timestamp );
            }
            else
            {
                break;
            }
        }
    }

    /*
     * Set the current size of the file.
     */
    guardian_source_set_size ( source, buffer.st_size);
    guardian_source_set_hash ( source, file_hash);


    fclose (f);

    return n_new_entries;
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
