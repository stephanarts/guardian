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

#include <sys/syslog.h>

#ifdef HAVE_LIBGEN_H
#include <libgen.h>
#endif

#include <dirent.h>

#include <unistd.h>
#include <sys/types.h>

#include <stdarg.h>

#include "error.h"
#include "entry.h"
#include "source.h"
#include "sourceengine.h"
#include "sourcetype.h"
#include "log.h"

struct _GuardianSource
{
    /**
     * The SourceType (eg. syslog),
     *
     */
    GuardianSourcetype *source_type;

    /**
     * The source path (eg. /var/log/messages)
     */
    char *name;

    /**
     * The parent directory (eg. /var/log/)
     */
    char *dir;

    size_t          n_entries;
    GuardianEntry **entries;
};

/**
 * guardian_source_new:
 * @type:  The SourceType (Eg. "syslog")
 * @path:  The path       (Eg. "/var/log/messages")
 * @error: 
 *
 * Returns: GuardianSource object, or NULL
 */
GuardianSource *
guardian_source_new (
        char *type,
        char *name,
        GuardianError **error )
{
    GuardianSource *source;
    GuardianSourcetype *source_type;

    /** Check if a type is provided */
    if ( type == NULL )
    {
        if (error)
        {
            *error = guardian_error_new ("SourceType 'NULL' unknown");
        }
        return NULL;
    }

    /** Check if a name is provided */
    if ( name == NULL )
    {
        if (error)
        {
            *error = guardian_error_new (
                    "Can not create Source with no name.");
        }
        return NULL;
    }


    /** Look up the sourcetype object from it's name */
    source_type  = guardian_sourcetype_lookup ( type );
    if ( source_type == NULL )
    {
        if (error)
        {
            *error = guardian_error_new (
                    "SourceType '%s' unknown, can not create "
                    "source object for '%s'", type, name);
        }
        //return NULL;
    }

    source = (GuardianSource *)malloc (sizeof (GuardianSource));
    source->source_type = source_type;
    source->name = name;

    return source;
}

void
guardian_source_update (
    GuardianSource *source)
{
    GuardianSourceEngine *engine =
            guardian_sourcetype_get_engine (source->source_type);
    int ret; 


    ret = engine->update_source ( engine, source );

}
