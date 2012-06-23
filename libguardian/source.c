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

#include <stdio.h>

#include <string.h>

#include <errno.h>

#ifdef HAVE_SYS_SYSLOG_H
#include <sys/syslog.h>
#endif

#include <sys/stat.h>

#include <stdarg.h>

#include "error.h"
#include "source.h"
#include "sourceengine.h"
#include "sourcetype.h"

struct _GuardianSource
{
    GuardianSourcetype *source_type;
    char *path;

    off_t st_size;  /* Size in bytes */
};


GuardianSource *
guardian_source_new (
        char *type,
        char *path,
        GuardianError **error )
{
    GuardianSource *source;
    GuardianSourcetype *source_type;

    if ( type == NULL )
    {
        if (*error)
        {
            *error = guardian_error_new ("SourceType 'NULL' unknown");
        }
        return NULL;
    }

    if ( path == NULL )
    {
        if (*error)
        {
            *error = guardian_error_new ("Can not create Source for 'NULL' path");
        }
        return NULL;
    }

    source_type  = guardian_sourcetype_lookup ( type );
    if ( source_type == NULL )
    {
        if (*error)
        {
            *error = guardian_error_new ("SourceType '%s' unknown, can not create source object for '%s'", type, path);
        }
        return NULL;
    }

    source = (GuardianSource *)malloc (sizeof (GuardianSource));
    source->source_type = source_type;

    source->path = (char *)malloc (strlen (path)+1);
    source->path = strcpy (source->path, path);

    return source;
}

const char *
guardian_source_get_path (
        GuardianSource *source )
{
    return source->path;
}

size_t
guardian_source_get_size (
        GuardianSource *source )
{
    return source->st_size;
}

void
guardian_source_set_size (
        GuardianSource *source,
        size_t s )
{
    source->st_size = s;
}

void
guardian_source_update (
        GuardianSource *source )
{
    GuardianSourceEngine *engine = guardian_sourcetype_get_engine (source->source_type);

    engine->update_source ( engine, source );
}
