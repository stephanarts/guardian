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


#include <errno.h>

#include <sys/stat.h>

#include <stdarg.h>

#include <libguardian/libguardian.h>

#define DATA_BUFFER_SIZE 1024

#define DATE_REGEX "(Jan|Feb|Mar|Apr|Jun|Jul|Aug|Sep|Oct|Nov|Dec)[ ]+([0-9]+)[ ]+([0-9]+)[:]([0-9]+)[:]([0-9]+)"

GuardianSourcetype *syslog_type = NULL;

static void
_plugin_register_types ( GuardianPlugin *plugin );

static void
_plugin_engine_update_source (
        GuardianSourceEngine *engine,
        GuardianSource       *source );

GuardianPlugin *
guardian_plugin_init ()
{
    GuardianPlugin *plugin = guardian_new (sizeof (GuardianPlugin), 1);

    plugin->register_types = _plugin_register_types;

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
    }
    return;
}

static void
_plugin_engine_update_source (
        GuardianSourceEngine *engine,
        GuardianSource       *source )
{
    int error_sv;

    FILE *f = NULL;
    int fd;
    struct stat buffer;

    const char *path = guardian_source_get_path (source);
    size_t st_size = guardian_source_get_size (source);

    char data_buffer[DATA_BUFFER_SIZE];
    char *str_ptr = &data_buffer[0];
    char *end_ptr = &data_buffer[DATA_BUFFER_SIZE-1];
    char *ptr;
    size_t _size;
    size_t s_offset;

    const char *entry;

    size_t offset;
    size_t len;

    f = fopen (path, "r");
    if (f == NULL)
    {
        error_sv = errno;
        guardian_log_warning ("Can not open file: %s:'%s'", path, strerror (error_sv));
        return;
    }

    fd = fileno (f);
    
    fstat (fd, &buffer);

    /*
     * Logs are rotated... so st_stat can be smaller then source->st_stat
     *
     */
    if (buffer.st_size > st_size)
    {
        /*
         * Move the file position indicator to the last known end of the file.
         */
        fseek (f, st_size, SEEK_SET);

        /*
         * Fill the remaining part of the buffer (complete buffer on first try)
         */
        while ((_size = fread (str_ptr, 1, end_ptr-str_ptr, f)) > 0)
        {
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
            guardian_sourcetype_get_entry (
                    syslog_type,
                    data_buffer,
                    DATA_BUFFER_SIZE,
                    0,
                    &entry,
                    &offset,
                    &len);

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

            printf("%s\n", entry);
        }

        s_offset = 0;

        while (s_offset < _size)
        {
            guardian_sourcetype_get_entry (
                    syslog_type,
                    &data_buffer[s_offset],
                    _size-s_offset,
                    0,
                    &entry,
                    &offset,
                    &len);
            ptr = &data_buffer[len+offset+1];
            while (*ptr == '\n' || *ptr == '\r')
                ptr++;

            s_offset+=len+offset+1;
            
            printf("%s\n", entry);
        }
    }

    /*
     * Set the current size of the file.
     */
    guardian_source_set_size ( source, buffer.st_size);


    fclose (f);

}
